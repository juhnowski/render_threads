#ifndef TEST_THREADS_STOP_H
#define TEST_THREADS_STOP_H

#include "Command.h"
#include "../global.h"

namespace command {
    class Stop {
    public:
        static void exec(Command *cmd);
    };
}

#endif //TEST_THREADS_STOP_H
