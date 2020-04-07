#ifndef TEST_THREADS_EXECUTOR_H
#define TEST_THREADS_EXECUTOR_H

#include <map>
#include "Add.h"

namespace command {

    class Executor {
    public:
        static void exec(string *str_cmd);
    };

}
#endif //TEST_THREADS_EXECUTOR_H
