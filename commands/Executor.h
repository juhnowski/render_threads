#ifndef TEST_THREADS_EXECUTOR_H
#define TEST_THREADS_EXECUTOR_H

#include "Command.h"
#include <map>
#include "Add.h"
#include "Start.h"
#include "Stop.h"

namespace command {

    class Executor {
    public:
        static void exec(string *str_cmd);
    };

}
#endif //TEST_THREADS_EXECUTOR_H
