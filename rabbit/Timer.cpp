#include "Timer.h"

namespace rabbit {

    Timer::Timer(struct ev_loop *loop, AMQP::TcpChannel *channel, std::string queue) {
        ev_timer_init(&_timer, callback, 0.005, 1.005);
        _timer.data = this;
        ev_timer_start(loop, &_timer);
    }

    void Timer::callback(struct ev_loop *loop, struct ev_timer *timer, int revents) {
        Timer *self = static_cast<Timer*>(timer->data);
    }
}