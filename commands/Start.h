#ifndef TEST_THREADS_START_H
#define TEST_THREADS_START_H

#include "Command.h"

namespace command {
    class Start {
    public:
        static void exec(Command *cmd);
    };
};

#endif //TEST_THREADS_START_H
