//
// Created by ilya on 06.04.2020.
//

#ifndef TEST_THREADS_TIMER_H
#define TEST_THREADS_TIMER_H
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include "../libh/json.hpp"

namespace rabbit {

    class Timer {
    public:
        Timer(struct ev_loop *loop, AMQP::TcpChannel *channel, std::string queue);

    private:
        /**
        *  The actual watcher structure
        *  @var struct ev_io
        */
        struct ev_timer _timer;

        /**
         *  Pointer towards the AMQP channel
         *  @var AMQP::TcpChannel
         */
        AMQP::TcpChannel *_channel;

        /**
         *  Name of the queue
         *  @var std::string
         */
        std::string _queue;

        /**
         *  Callback method that is called by libev when the timer expires
         *  @param  loop        The loop in which the event was triggered
         *  @param  timer       Internal timer object
         *  @param  revents     The events that triggered this call
         */
        static void callback(struct ev_loop *loop, struct ev_timer *timer, int revents);
    };

}
#endif //TEST_THREADS_TIMER_H
