#include "Stop.h"
#include "../main.h"

namespace command {

    void Stop::exec(Command *cmd) {
        is_run = false;
    }
}