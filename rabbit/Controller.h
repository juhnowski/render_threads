#ifndef TEST_THREADS_CONTROLLER_H
#define TEST_THREADS_CONTROLLER_H

#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include "Handler.h"
#include "Timer.h"
#include <iostream>
#include <mutex>

#include "../commands/Executor.h"


namespace rabbit {
    class Controller {
    public:
        void start();
//        static auto receiveMessageCallback(const AMQP::Message &message, uint64_t deliveryTag, bool redelivered);
    };
}

#endif //TEST_THREADS_CONTROLLER_H
