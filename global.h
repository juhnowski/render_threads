#ifndef TEST_THREADS_GLOBAL_H
#define TEST_THREADS_GLOBAL_H

#include "settings.h"
#include "StreamContext.h"
#include <vector>
#include <opencv2/core/mat.hpp>

//using namespace cv;

extern mutex mu[slave_stream_max_count + 1];
extern string names[slave_stream_max_count + 1];
extern int video_ports[slave_stream_max_count + 1];
extern int audio_ports[slave_stream_max_count + 1];
extern cv::Mat *images[slave_stream_max_count + 1]; //for master_stub, slave_stub should be int
extern string rtmps[slave_stream_max_count + 1];

extern int stream_cnt;
extern mutex m_stream_cnt;

extern vector<StreamContext *> app_streams;
extern bool app_is_run;

#endif //TEST_THREADS_GLOBAL_H
