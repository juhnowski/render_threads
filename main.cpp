#include <iostream>
#include <unistd.h>
#include <thread>

#include <vector>
#include <mutex>
#include "ImageContext.h"
#include "StreamContext.h"
#include "ApplicationContext.h"

#define slave_stream_max_count 1
using namespace std;

//------------- storage -----------------------------
static mutex mu[slave_stream_max_count+1];
static string names[slave_stream_max_count+1];
static int video_ports[slave_stream_max_count+1];
static int audio_ports[slave_stream_max_count+1];

//------------- context -----------------------------
static ApplicationContext *app = new ApplicationContext();

void master(StreamContext *ctx) {
    while(ctx->is_active) {

        for (int i = 1; i < app->streams.size(); ++i) {
            app->streams.at(i)->image_ctx->mtx->lock();
            *app->streams.at(i)->image_ctx->image-= 2;
            cout << "Master: " << *app->streams.at(i)->image_ctx->image << endl;
            app->streams.at(i)->image_ctx->mtx->unlock();
        }

        usleep(50000);
    }
}

void slave(StreamContext *ctx) {
    while(ctx->is_active) {

        ctx->image_ctx->mtx->lock();
        *ctx->image_ctx->image+= 2;
        cout << "Slave: " << *ctx->image_ctx->image << endl;
        ctx->image_ctx->mtx->unlock();

        usleep(50000);
    }
}


static StreamContext* add_StreamContext(int idx, string name, int videoPort, int audioPort, int *image) {
    ImageContext *im = new ImageContext(&mu[idx], image);

    names[idx] = string(name);
    video_ports[idx] = videoPort;
    audio_ports[idx] = audioPort;

    StreamContext *ctx = new StreamContext(&names[idx], &video_ports[idx], &audio_ports[idx], im,true);
    app->streams.push_back(ctx);
    cout<< "Stream '" << ctx << "' has been created"<< endl;
    return ctx;
}


void rabbit_control() {

    //------------------------ create master ---------------------------------
    int img_master = 0;
    add_StreamContext(0, "master", 50041, 50051, &img_master);

    //------------------------ create slave ---------------------------------

    int img_slave = 1;
    add_StreamContext(1, "slave", 51041, 51051, &img_master);

    thread t_master (master, ref(app->streams.at(0)));
    thread t_slave (slave, ref(app->streams.at(1)));

    // stop
    usleep(1500000);
    app->streams.at(0)->is_active = false;
    app->streams.at(1)->is_active = false;

    t_master.join();
    t_slave.join();
}

int main() {

    thread t_control(rabbit_control);


    t_control.join();

    return 0;
}
