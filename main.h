//
// Created by ilya on 06.04.2020.
//

#ifndef TEST_THREADS_MAIN_H
#define TEST_THREADS_MAIN_H

#include <iostream>
#include <unistd.h>
#include <thread>

#include <vector>
#include <mutex>
#include "ImageContext.h"
#include "StreamContext.h"
#include "ApplicationContext.h"
#include "rabbit/Controller.h"
#include "settings.h"

//------------- storage -----------------------------
static mutex mu[slave_stream_max_count+1];
static string names[slave_stream_max_count+1];
static int video_ports[slave_stream_max_count+1];
static int audio_ports[slave_stream_max_count+1];
static int images[slave_stream_max_count+1];

static int stream_cnt = 0;
static mutex m_stream_cnt;
//------------- context -----------------------------
static ApplicationContext *app = new ApplicationContext();

static bool is_run = false;

#endif //TEST_THREADS_MAIN_H

