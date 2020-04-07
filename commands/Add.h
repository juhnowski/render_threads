#ifndef TEST_THREADS_ADD_H
#define TEST_THREADS_ADD_H

#include "Start.h"

namespace command {
    class Add {
    public:
        static void exec(Command *cmd);
        static StreamContext* add_StreamContext(string name, int videoPort, int audioPort);
    };
}

#endif //TEST_THREADS_ADD_H
