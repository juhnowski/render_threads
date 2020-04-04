#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

struct image_t;
struct image_t {
    int image; // <---- cv
    mutex mtx;
};

struct stream_t;
struct stream_t {
    string name;
    int video_port;
    struct image_t image_ctx;
    bool is_active;
};

struct ctx_t;
struct ctx_t{
    struct stream_t master;
    vector<stream_t> slaves;
};

void master(ctx_t &ctx) {
    while(ctx.master.is_active) {
        ctx.master.image_ctx.mtx.lock();

        if (ctx.master.image_ctx.image > 2) {
            ctx.master.image_ctx.image-= 2;
            cout << "Master: " << ctx.master.image_ctx.image << endl;
        }

        ctx.master.image_ctx.mtx.unlock();

        usleep(50000);
    }
}

void slave(stream_t &ctx) {
    while(ctx.is_active) {
        ctx.image_ctx.mtx.lock();
        ctx.image_ctx.image -= 2;
        cout << "Bar: " << ctx.image_ctx.image << endl;
        ctx.image_ctx.mtx.unlock();

        usleep(50000);
    }
}

void rabbit_control(ctx_t &ctx) {

    // add slave 1
    image_t img_ctx = {2};
    stream_t test_slave_ctx = {"test", 50041, {2}, true};
    thread t_slave (slave, ref(test_slave_ctx));
    ctx.slaves.push_back(ref(test_slave_ctx));

    // stop
    usleep(150000);
    ctx.master.is_active = false;
    ctx.slaves[0].is_active = false;

}

int main() {
    int num = 10;
    bool is_active = true;
    mutex mtx;
    struct ctx_t app_context;

    thread t_master (master, ref(app_context));
    thread t_control(rabbit_control, ref(app_context));

    t_master.join();
    t_control.join();

    return 0;
}
