#include "Controller.h"
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
    using json = nlohmann::json;
    using namespace std;



    void Controller::start() {
        // access to the event loop
        auto *loop = EV_DEFAULT;

        // handler for libev
        Handler handler(loop);

        // init the SSL library
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        SSL_library_init();
#else
        OPENSSL_init_ssl(0, NULL);
#endif

        // make a connection
        AMQP::Address address("amqp://localhost");

        AMQP::TcpConnection connection(&handler, address);

        // we need a channel too
        AMQP::TcpChannel myChannel(&connection);

        string topic_name = "topic_golive-renderer";
        myChannel.declareExchange(topic_name, AMQP::topic)

                .onSuccess([]() {
                    // by now the exchange is created
                    cout << " [x] Declare exchange: Success" << endl;
                })

                .onError([](const char *message) {
                    // something went wrong creating the exchange
                    cout << " [-]  Declare exchange error: "<< message << endl;
                });



        auto receiveMessageCallback =
                [](const AMQP::Message &message,
                   uint64_t deliveryTag,
                   bool redelivered)
                {
                    string json_str = string(message.body()).substr(0,message.bodySize());
                    command::Executor::exec(&json_str);
                };

        string queue_name = "my-queue";
        myChannel.declareQueue(queue_name, true)
                .onSuccess([](){
                    cout << " [x] Declare queue success"<< endl;
                })

                .onError([](const char *message) {
                    cout << " [-] Declare queue error: "<< message << endl;
                });

        string routing_key = "golive-renderer.command";
        myChannel.bindQueue(topic_name, queue_name, routing_key)
                .onSuccess([](){
                    cout << " [x] Bind queue : Success" << endl;
                })

                .onError([](const char *message){
                    cout << " [-]  Bind queue error: "<< message << endl;
                });

        myChannel.consume(queue_name,AMQP::noack).onReceived(receiveMessageCallback);

        // run the loop
        ev_run(loop, 0);

    }
}