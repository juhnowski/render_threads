#include "Handler.h"

namespace rabbit {

    void Handler::onConnected(AMQP::TcpConnection *connection) {
        TcpHandler::onConnected(connection);
        std::cout << "connected" << std::endl;
    }

    void Handler::onError(AMQP::TcpConnection *connection, const char *message) {
        TcpHandler::onError(connection, message);
        std::cout << "error: " << message << std::endl;
    }

    void Handler::onReady(AMQP::TcpConnection *connection) {
        TcpHandler::onReady(connection);
        std::cout << "ready" << std::endl;
    }

    void Handler::onClosed(AMQP::TcpConnection *connection) {
        TcpHandler::onClosed(connection);
        std::cout << "closed" << std::endl;
    }

    void Handler::onDetached(AMQP::TcpConnection *connection) {
        LibEvHandler::onDetached(connection);
        std::cout << "detached" << std::endl;
    }
}