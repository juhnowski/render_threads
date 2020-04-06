#ifndef TEST_THREADS_HANDLER_H
#define TEST_THREADS_HANDLER_H

#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

namespace rabbit {
    class Handler : public AMQP::LibEvHandler{
    public:
        Handler(struct ev_loop *loop) : AMQP::LibEvHandler(loop) {}
        virtual ~Handler() = default;

    private:
        virtual void onError(AMQP::TcpConnection *connection, const char *message) override;
        virtual void onConnected(AMQP::TcpConnection *connection) override;
        virtual void onReady(AMQP::TcpConnection *connection) override;
        virtual void onClosed(AMQP::TcpConnection *connection) override;
        virtual void onDetached(AMQP::TcpConnection *connection) override;
    };
}

#endif //TEST_THREADS_HANDLER_H
