#include "slave.h"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

using namespace std;
using namespace cv;

namespace cvrender{
    namespace slave {
        int width = 0;
        int height = 0;
        int fps = 30;
        int bitrate = 300000;

        VideoCapture get_input_stream(std::string *inputSdp ) {

            setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "protocol_whitelist;file,rtp,udp", 1);

            VideoCapture cap(*inputSdp, cv::CAP_FFMPEG);

            if(!cap.isOpened()){
                cout << "Error opening "<< *inputSdp << endl;
                exit(1);
            }

            width = cap.get(CAP_PROP_FRAME_WIDTH);
            height = cap.get(CAP_PROP_FRAME_HEIGHT);

            cout << "Incoming width:" << width << endl;
            cout << "Incoming height:" << height << endl;

            return cap;
        }

        void initialize_avformat_context(AVFormatContext *&fctx, const char *format_name)
        {
            int ret = avformat_alloc_output_context2(&fctx, nullptr, format_name, nullptr);
            if (ret < 0)
            {
                std::cout << "Could not allocate output format context!" << std::endl;
                exit(1);
            }
        }

        void initialize_io_context(AVFormatContext *&fctx, const char *output)
        {
            if (!(fctx->oformat->flags & AVFMT_NOFILE))
            {
                int ret = avio_open2(&fctx->pb, output, AVIO_FLAG_WRITE, nullptr, nullptr);
                if (ret < 0)
                {
                    std::cout << "initialize_io_context: Could not open output IO context!" << std::endl;
                    exit(1);
                }
            }
        }


        void set_codec_params(AVFormatContext *&fctx, AVCodecContext *&codec_ctx)
        {
            const AVRational dst_fps = {fps, 1};

            codec_ctx->codec_tag = 0;
            codec_ctx->codec_id = AV_CODEC_ID_H264;
            codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
            codec_ctx->width = width;
            codec_ctx->height = height;
            codec_ctx->gop_size = 12;
            codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
            codec_ctx->framerate = dst_fps;
            codec_ctx->time_base = av_inv_q(dst_fps);
            codec_ctx->bit_rate = bitrate;
            if (fctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
        }

        void initialize_codec_stream(AVStream *&stream, AVCodecContext *&codec_ctx, AVCodec *&codec, std::string codec_profile)
        {
            int ret = avcodec_parameters_from_context(stream->codecpar, codec_ctx);
            if (ret < 0)
            {
                std::cout << "Could not initialize stream codec parameters!" << std::endl;
                exit(1);
            }

            AVDictionary *codec_options = nullptr;
            av_dict_set(&codec_options, "profile", codec_profile.c_str(), 0);
            av_dict_set(&codec_options, "preset", "superfast", 0);
            av_dict_set(&codec_options, "tune", "zerolatency", 0);

            // open video encoder
            ret = avcodec_open2(codec_ctx, codec, &codec_options);
            if (ret < 0)
            {
                std::cout << "Could not open video encoder!" << std::endl;
                exit(1);
            }
        }

        SwsContext *initialize_sample_scaler(AVCodecContext *codec_ctx, double width, double height)
        {
            SwsContext *swsctx = sws_getContext(width, height, AV_PIX_FMT_BGR24, width, height, codec_ctx->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
            if (!swsctx)
            {
                std::cout << "Could not initialize sample scaler!" << std::endl;
                exit(1);
            }

            return swsctx;
        }

        AVFrame *allocate_frame_buffer(AVCodecContext *codec_ctx, double width, double height)
        {
            AVFrame *frame = av_frame_alloc();

            std::vector<uint8_t> framebuf(av_image_get_buffer_size(codec_ctx->pix_fmt, width, height, 1));
            av_image_fill_arrays(frame->data, frame->linesize, framebuf.data(), codec_ctx->pix_fmt, width, height, 1);
            frame->width = width;
            frame->height = height;
            frame->format = static_cast<int>(codec_ctx->pix_fmt);

            return frame;
        }

//        void write_frame(AVCodecContext *codec_ctx, AVFormatContext *fmt_ctx, AVFrame *frame)
//        {
//            AVPacket pkt = {0};
//            av_init_packet(&pkt);
//
//            int ret = avcodec_send_frame(codec_ctx, frame);
//            if (ret < 0)
//            {
//                std::cout << "Error sending frame to codec context!" << std::endl;
//                exit(1);
//            }
//
//            ret = avcodec_receive_packet(codec_ctx, &pkt);
//            if (ret < 0)
//            {
//                std::cout << "Error receiving packet from codec context!" << std::endl;
//                exit(1);
//            }
//
//            av_interleaved_write_frame(fmt_ctx, &pkt);
//            av_packet_unref(&pkt);
//        }

        struct input_t;
        struct input_t {
            string audio;
            string video;
        };

        static void create_slave_sdp(int *audio_port, int *video_port, vector<input_t> &sdp_vector, int* index);

        static void create_slave_sdp(int *audio_port, int *video_port, vector<input_t> &sdp_vector, int* index) {

            struct input_t input;
            input.audio = to_string(*index) + "_" + to_string(getpid()) + "_a.sdp";
            input.video = to_string(*index) + "_" + to_string(getpid()) + "_v.sdp";

cout << "input.video = " << input.video;

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
            file_a << "a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=121056E500" << endl;
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

        static void delete_sdp(int, vector<input_t> *);

        static void delete_sdp(int ret, vector<input_t> *sdp_vector) {
            vector<input_t> *files = (vector<input_t> *) sdp_vector;

            for (input_t rtp: *(vector<input_t> *) sdp_vector) {
                remove(rtp.audio.c_str());
                remove(rtp.video.c_str());
            }
        }

        static vector<input_t> sdp_slave_vector = {};
        int slave(StreamContext *ctx) {

            create_slave_sdp(ctx->audio_port, ctx->video_port, sdp_slave_vector, &ctx->idx);

            std::string h264profile = "high444";

            av_log_set_level(AV_LOG_DEBUG);

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
            av_register_all();
#endif
            avformat_network_init();

            int ret;
cout << "Slave video  [" << &sdp_slave_vector.at(0).video << "] " << sdp_slave_vector.at(0).video << endl;
            VideoCapture cap = get_input_stream(&sdp_slave_vector.at(0).video);

            std::vector<uint8_t> imgbuf(height * width * 3 + 16);
            cv::Mat image(height, width, CV_8UC3, imgbuf.data(), width * 3);

            bool end_of_stream = false;

            do {
                cap >> image;
            } while (!end_of_stream);
            return 0;
        }

    }
}