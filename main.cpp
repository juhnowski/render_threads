
#include "main.h"

using namespace std;
using namespace rabbit;

mutex mu[slave_stream_max_count + 1];
string names[slave_stream_max_count + 1];
int video_ports[slave_stream_max_count + 1];
int audio_ports[slave_stream_max_count + 1];
int images[slave_stream_max_count + 1];

int stream_cnt = 0;
mutex m_stream_cnt;

vector<StreamContext *> app_streams;
bool app_is_run = false;


void master(StreamContext *ctx) {
    while(ctx->is_active) {

        for (int i = 1; i < app_streams.size(); ++i) {
            app_streams.at(i)->image_ctx->mtx->lock();
            *app_streams.at(i)->image_ctx->image-= 2;
            cout << "Master: " << *app_streams.at(i)->image_ctx->image << endl;
            app_streams.at(i)->image_ctx->mtx->unlock();
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


void rabbit_control(Controller *ctrl) {
    ctrl->start();
}

void run_control() {
    bool flag = false;

    while(!app_is_run) {
        if (!flag) {
            usleep(50000);
        }
    }

    app_streams.at(0)->is_active = true;
    app_streams.at(1)->is_active = true;
    thread t_master (master, ref(app_streams.at(0)));
    thread t_slave (slave, ref(app_streams.at(1)));
    cout << " [x] Started" << endl;

    flag = false;
    while(app_is_run) {

        flag = app_is_run;

        if (!flag) {
            usleep(50000);
        }

    }


    app_streams.at(0)->is_active = false;
    app_streams.at(1)->is_active = false;

    t_master.join();
    t_slave.join();

    cout << " [x] Finished" << endl;
}

int main() {
    cout << "main: app->is_run["<<&app_is_run<<"]" << app_is_run << endl;
    Controller *ctrl = new Controller();
    thread t_control(rabbit_control, ref(ctrl));
    thread t_run(run_control);

    t_run.join();
    t_control.join();

    return 0;
}
