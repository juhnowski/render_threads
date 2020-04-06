
#include "main.h"

using namespace std;
using namespace rabbit;



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


//static StreamContext* add_StreamContext(string name, int videoPort, int audioPort, int *image) {
//    m_stream_cnt.lock();
//    int idx = stream_cnt;
//    stream_cnt++;
//    m_stream_cnt.unlock();
//
//    ImageContext *im = new ImageContext(&mu[idx], image);
//
//    names[idx] = string(name);
//    video_ports[idx] = videoPort;
//    audio_ports[idx] = audioPort;
//
//    StreamContext *ctx = new StreamContext(idx, &names[idx], &video_ports[idx], &audio_ports[idx], im,true);
//
//    app->streams.push_back(ctx);
//
//    return ctx;
//}

void rabbit_control() {

    Controller *ctrl = new Controller();
    ctrl->start();
}

void run_control() {
    while(!is_run) {
        usleep(50000);
    }

    thread t_master (master, ref(app->streams.at(0)));
    thread t_slave (slave, ref(app->streams.at(1)));

    while(is_run) {
        usleep(50000);
    }

    app->streams.at(0)->is_active = false;
    app->streams.at(1)->is_active = false;

    t_master.join();
    t_slave.join();

}

int main() {
    thread t_control(rabbit_control);
    thread t_run(run_control);

    t_run.join();
    t_control.join();

    return 0;
}
