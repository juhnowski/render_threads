

#include "master.h"

#include "../StreamContext.h"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// FFmpeg
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

        using namespace std;
        using namespace cv;

namespace cvrender {
    namespace master {

#undef av_err2str
#define av_err2str(errnum) av_make_error_string((char*)__builtin_alloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)

        struct input_t;
        struct input_t {
            string audio;
            string video;
        };

        static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame);

        static void add_audio_stream(AVStream *astrm, AVFormatContext *oc, enum AVCodecID codec_id);

        static void create_sdp(int *audio_port, int *video_port, vector<input_t> &sdp_vector);

        static void create_sdp(int *audio_port, int *video_port, vector<input_t> &sdp_vector) {

            int index = sdp_vector.size();
            struct input_t input;
            input.audio = to_string(index) + "_" + to_string(getpid()) + "_a.sdp";
            input.video = to_string(index) + "_" + to_string(getpid()) + "_v.sdp";
            const char *a_sdp = input.audio.c_str();
            const char *v_sdp = input.video.c_str();


            ofstream file_a;
            file_a.open(a_sdp);

            file_a << "v=0" << endl;
            file_a << "o=- 0 0 IN IP4 127.0.0.1" << endl;
            file_a << "s=No Name" << endl;
            file_a << "c=IN IP4 127.0.0.1" << endl;
            file_a << "t=0 0" << endl;
            file_a << "a=tool:libavformat 58.42.100" << endl;
            file_a << "m=audio " << *audio_port << " RTP/AVP 97" << endl;
            file_a << "b=AS:128" << endl;
            file_a << "a=rtpmap:97 MPEG4-GENERIC/44100/2" << endl;
            file_a
                    << "a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=121056E500"
                    << endl;
            file_a.flush();
            file_a.close();

            ofstream file_v;
            file_v.open(v_sdp);

            file_v << "v=0" << endl;
            file_v << "o=- 0 0 IN IP4 127.0.0.1" << endl;
            file_v << "s=No Name" << endl;
            file_v << "c=IN IP4 127.0.0.1" << endl;
            file_v << "t=0 0" << endl;
            file_v << "a=tool:libavformat 58.20.100" << endl;
            file_v << "m=video " << *video_port << " RTP/AVP 96" << endl;
            file_v << "b=AS:405" << endl;
            file_v << "a=rtpmap:96 H264/90000" << endl;
            file_v
                    << "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z01AH+iAKALdgLUBAQFAAAADAEAAAAwDxgxEgA==,aOvvIA==; profile-level-id=4D401F"
                    << endl;
            file_v.flush();
            file_v.close();

            sdp_vector.push_back(input);
        }

        static void logging(const char *fmt, ...);

        static void logging(const char *fmt, ...) {
            va_list args;
            fprintf(stderr, "LOG: ");
            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
            fprintf(stderr, "\n");
        }

        static void delete_sdp(vector<input_t> *);

        static void delete_sdp(vector<input_t> *sdp_vector) {
            vector<input_t> *files = (vector<input_t> *) sdp_vector;

            for (input_t rtp: *(vector<input_t> *) sdp_vector) {
                remove(rtp.audio.c_str());
                remove(rtp.video.c_str());
            }
        }

        static vector<input_t> sdp_vector = {};

        int master(StreamContext *ctx) {

            create_sdp(ctx->audio_port, ctx->video_port, sdp_vector);
            on_exit(reinterpret_cast<void (*)(int, void *)>(delete_sdp), &sdp_vector);

            const char *outfile = ctx->rtmp->c_str();

            setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "protocol_whitelist;file,rtp,udp", 1);

            // initialize FFmpeg library
            av_register_all();
//  av_log_set_level(AV_LOG_DEBUG);
            int ret;

            const int dst_width = 1280;
            const int dst_height = 720;
            const AVRational dst_fps = {30, 1};

            // initialize audio
//-----------------------------------------------------------------------------------------------
            AVFormatContext *input_format_context = nullptr;
            AVPacket packet;

            int response = 0;

            AVCodec *pLocalCodec = NULL;
            AVCodecParameters *pLocalCodecParameters = NULL;

            AVDictionary *d = NULL;
            av_dict_set(&d, "protocol_whitelist", "file,udp,rtp", 0);

            if ((ret = avformat_open_input(&input_format_context, ((input_t) sdp_vector.at(0)).audio.c_str(), NULL,
                                           &d)) < 0) {
                fprintf(stderr, "Could not open input file '%s'", ((input_t) sdp_vector.at(0)).audio.c_str());
                return 2;
            }

            if ((ret = avformat_find_stream_info(input_format_context, NULL)) < 0) {
                fprintf(stderr, "Failed to retrieve input stream information");
                return 2;
            }


            for (int i = 0; i < input_format_context->nb_streams; i++) {
                pLocalCodecParameters = input_format_context->streams[i]->codecpar;
                logging("AVStream->time_base before open coded %d/%d", input_format_context->streams[i]->time_base.num,
                        input_format_context->streams[i]->time_base.den);
                logging("AVStream->r_frame_rate before open coded %d/%d",
                        input_format_context->streams[i]->r_frame_rate.num,
                        input_format_context->streams[i]->r_frame_rate.den);
                logging("AVStream->start_time %" PRId64, input_format_context->streams[i]->start_time);
                logging("AVStream->duration %" PRId64, input_format_context->streams[i]->duration);

                logging("finding the proper decoder (CODEC)");

                pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

                if (pLocalCodec == NULL) {
                    logging("ERROR unsupported codec!");
                    return -1;
                }

                logging("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels,
                        pLocalCodecParameters->sample_rate);

                // print its name, id and bitrate
                logging("\tCodec %s ID %d bit_rate %lld", pLocalCodec->name, pLocalCodec->id,
                        pLocalCodecParameters->bit_rate);
            }


            AVCodecContext *pCodecContext = avcodec_alloc_context3(pLocalCodec);
            if (!pCodecContext) {
                logging("failed to allocated memory for AVCodecContext");
                return -1;
            }

            if (avcodec_parameters_to_context(pCodecContext, pLocalCodecParameters) < 0) {
                logging("failed to copy codec params to codec context");
                return -1;
            }

            if (avcodec_open2(pCodecContext, pLocalCodec, NULL) < 0) {
                logging("failed to open codec through avcodec_open2");
                return -1;
            }

            AVFrame *pFrame = av_frame_alloc();
            if (!pFrame) {
                logging("failed to allocated memory for AVFrame");
                return -1;
            }

            AVPacket *pPacket = av_packet_alloc();
            if (!pPacket) {
                logging("failed to allocated memory for AVPacket");
                return -1;
            }

//------------------------------------------------------------------------------------------------
            // initialize OpenCV capture as input frame generator

            cv::VideoCapture cvcap(((input_t) sdp_vector.at(0)).video, cv::CAP_FFMPEG);
            //cv::VideoCapture cvcap(argv[1]);
            if (!cvcap.isOpened()) {
                std::cerr << "fail to open cv::VideoCapture";
                return 2;
            }
            cvcap.set(cv::CAP_PROP_FRAME_WIDTH, dst_width);
            cvcap.set(cv::CAP_PROP_FRAME_HEIGHT, dst_height);

            // allocate cv::Mat with extra bytes (required by AVFrame::data)
            std::vector<uint8_t> imgbuf(dst_height * dst_width * 3 + 16);
            cv::Mat image(dst_height, dst_width, CV_8UC3, imgbuf.data(), dst_width * 3);

            // open output format context
            AVFormatContext *outctx = nullptr;
            ret = avformat_alloc_output_context2(&outctx, nullptr, "flv", nullptr);
            outctx->protocol_whitelist = "file,tcp,rtmp,udp,rtp";
            if (ret < 0) {
                std::cerr << "fail to avformat_alloc_output_context2(" << outfile << "): ret=" << ret;
                return 2;
            }

            // open output IO context
            ret = avio_open2(&outctx->pb, outfile, AVIO_FLAG_WRITE, nullptr, nullptr);
            if (ret < 0) {
                std::cerr << "fail to avio_open2: ret=" << ret;
                return 2;
            }

            // create new video stream
            AVCodec *vcodec = avcodec_find_encoder(outctx->oformat->video_codec);
            AVStream *vstrm = avformat_new_stream(outctx, vcodec);
            if (!vstrm) {
                std::cerr << "fail to avformat_new_stream for video stream";
                return 2;
            }

            vstrm->id = 0;
            outctx->streams[0] = vstrm;

//create new audio stream----------------------------------------------------------------------
            AVStream *astrm = nullptr;
            add_audio_stream(astrm, outctx, AV_CODEC_ID_AAC);//pLocalCodecParameters->codec_id);
//--------------------------------------------------------------------------------------------

            avcodec_get_context_defaults3(vstrm->codec, vcodec);
            vstrm->codec->codec_id = outctx->oformat->video_codec;
            vstrm->codec->width = dst_width;
            vstrm->codec->height = dst_height;
            vstrm->codec->pix_fmt = vcodec->pix_fmts[0];
            vstrm->codec->time_base = vstrm->time_base = av_inv_q(dst_fps);
            vstrm->r_frame_rate = vstrm->avg_frame_rate = dst_fps;
            if (outctx->oformat->flags & AVFMT_GLOBALHEADER)
                vstrm->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            // open video encoder
            ret = avcodec_open2(vstrm->codec, vcodec, nullptr);
            if (ret < 0) {
                std::cerr << "fail to avcodec_open2: ret=" << ret;
                return 2;
            }

            std::cout
                    << "outfile: " << outfile << "\n"
                    << "format:  " << outctx->oformat->name << "\n"
                    << "vcodec:  " << vcodec->name << "\n"
                    << "size:    " << dst_width << 'x' << dst_height << "\n"
                    << "fps:     " << av_q2d(dst_fps) << "\n"
                    << "pixfmt:  " << av_get_pix_fmt_name(vstrm->codec->pix_fmt) << "\n"
                    << std::flush;

            // initialize sample scaler
            SwsContext *swsctx = sws_getCachedContext(
                    nullptr, dst_width, dst_height, AV_PIX_FMT_BGR24,
                    dst_width, dst_height, vstrm->codec->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
            if (!swsctx) {
                std::cerr << "fail to sws_getCachedContext";
                return 2;
            }

            // allocate frame buffer for encoding
            AVFrame *frame = av_frame_alloc();
            std::vector<uint8_t> framebuf(avpicture_get_size(vstrm->codec->pix_fmt, dst_width, dst_height));
            avpicture_fill(reinterpret_cast<AVPicture *>(frame), framebuf.data(), vstrm->codec->pix_fmt, dst_width,
                           dst_height);
            frame->width = dst_width;
            frame->height = dst_height;
            frame->format = static_cast<int>(vstrm->codec->pix_fmt);

            // encoding loop
            avformat_write_header(outctx, nullptr);
            int64_t frame_pts = 0;
            unsigned nb_frames = 0;
            bool end_of_stream = false;
            int got_pkt = 0;
            double alpha = 0.5; double beta = 0.5;

            do {
                if (!end_of_stream) {
                    // retrieve source image
                    cvcap >> image;
                    addWeighted( image.clone(), alpha, *app_streams.at(1)->image_ctx->image, beta, 0.0, image);
                    cv::imshow("master", image);
                    if (cv::waitKey(33) == 0x1b)
                        end_of_stream = true;
                }
                if (!end_of_stream) {
                    // convert cv::Mat(OpenCV) to AVFrame(FFmpeg)
                    const int stride[] = {static_cast<int>(image.step[0])};
                    sws_scale(swsctx, &image.data, stride, 0, image.rows, frame->data, frame->linesize);
                    frame->pts = frame_pts++;
                }
                // encode video frame
                AVPacket pkt;
                pkt.data = nullptr;
                pkt.size = 0;
                av_init_packet(&pkt);
                ret = avcodec_encode_video2(vstrm->codec, &pkt, end_of_stream ? nullptr : frame, &got_pkt);
                if (ret < 0) {
                    std::cerr << "fail to avcodec_encode_video2: ret=" << ret << "\n";
                    break;
                }
                if (got_pkt) {
                    // rescale packet timestamp
                    pkt.duration = 1;
                    av_packet_rescale_ts(&pkt, vstrm->codec->time_base, vstrm->time_base);
                    // write packet
                    pkt.stream_index = vstrm->index;
                    ret = av_write_frame(outctx, &pkt);
                    if (ret < 0) {
                        logging("Error write video frame: %s", av_err2str(ret));
                    }

                    std::cout << nb_frames << '\r' << std::flush;  // dump progress
                    ++nb_frames;
                }
                av_free_packet(&pkt);

//---------------------------------------------------------------------------------------------------
//Audio
                if (!end_of_stream) {
                    if (av_read_frame(input_format_context, pPacket) >= 0) {
                        logging("AVPacket->pts %" PRId64, pPacket->pts);
                        av_packet_rescale_ts(pPacket, input_format_context->streams[0]->codec->time_base,
                                             vstrm->time_base);

                        pPacket->stream_index = 1;
                        ret = av_write_frame(outctx, pPacket);
                        if (ret < 0) {
                            logging("Error write video frame: %s", av_err2str(ret));
                        }

                        //        av_interleaved_write_frame(outctx, pPacket);
                    }
                }
//---------------------------------------------------------------------------------------------------
            } while (ctx->is_active);//(!end_of_stream || got_pkt);
//            av_write_trailer(outctx);
//            std::cout << nb_frames << " frames encoded" << std::endl;
//
//            av_frame_free(&frame);
//            avcodec_close(vstrm->codec);
//            avio_close(outctx->pb);
//            avformat_free_context(outctx);

                delete_sdp(&sdp_vector);
                cout << " [x] Master has been finished" << endl;
            return 0;
        }

        static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame) {
            // Supply raw packet data as input to a decoder
            // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
            int response = avcodec_send_packet(pCodecContext, pPacket);

            if (response < 0) {
                logging("Error while sending a packet to the decoder: %s", response);
                return response;
            }

            while (response >= 0) {
                // Return decoded output data (into a frame) from a decoder
                // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
                response = avcodec_receive_frame(pCodecContext, pFrame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                } else if (response < 0) {
                    logging("Error while receiving a frame from the decoder: %s", response);
                    return response;
                }

                if (response >= 0) {
                    logging(
                            "Frame %d (type=%c, size=%d bytes) pts %d key_frame %d [DTS %d]",
                            pCodecContext->frame_number,
                            av_get_picture_type_char(pFrame->pict_type),
                            pFrame->pkt_size,
                            pFrame->pts,
                            pFrame->key_frame,
                            pFrame->coded_picture_number
                    );
                }
            }
            return 0;
        }

        static void add_audio_stream(AVStream *astrm, AVFormatContext *oc, enum AVCodecID codec_id) {
            AVCodecContext *c;
            AVCodec *codec;
            int ret;
            /* find the audio encoder */
            codec = avcodec_find_encoder(codec_id);
            if (!codec) {
                fprintf(stderr, "codec not found\n");
                exit(1);
            }
            astrm = avformat_new_stream(oc, NULL);
            if (!astrm) {
                fprintf(stderr, "Could not alloc stream\n");
                exit(1);
            }
            astrm->id = 1;

            cout << "oc->nb_streams=" << oc->nb_streams << endl;
            c = avcodec_alloc_context3(codec);
            if (!c) {
                fprintf(stderr, "Could not alloc an encoding context\n");
                exit(1);
            }

            /* put sample parameters */
            c->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_S16;
            c->sample_rate = codec->supported_samplerates ? codec->supported_samplerates[0] : 44100;
            c->channel_layout = codec->channel_layouts ? codec->channel_layouts[0] : AV_CH_LAYOUT_STEREO;
            c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
            c->bit_rate = 64000;
            c->codec_type = AVMEDIA_TYPE_AUDIO;
//    ost->st->time_base = (AVRational){ 1, c->sample_rate };
            // some formats want stream headers to be separate
            if (oc->oformat->flags & AVFMT_GLOBALHEADER)
                c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            oc->streams[1] = astrm;

            astrm->codec = c;
            return;
        }
}}