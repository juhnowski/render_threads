#ifndef TEST_THREADS_GLOBAL_H
#define TEST_THREADS_GLOBAL_H

#include "settings.h"
#include "StreamContext.h"
#include <vector>

//------------- storage -----------------------------
extern mutex mu[slave_stream_max_count + 1];
extern string names[slave_stream_max_count + 1];
extern int video_ports[slave_stream_max_count + 1];
extern int audio_ports[slave_stream_max_count + 1];
extern int images[slave_stream_max_count + 1];
extern string rtmps[slave_stream_max_count + 1];

extern int stream_cnt;
extern mutex m_stream_cnt;

extern vector<StreamContext *> app_streams;
extern bool app_is_run;

extern string rtmp_url;

#endif //TEST_THREADS_GLOBAL_H
