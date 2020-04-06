#include "Start.h"
#include "../main.h"
namespace command {

    void Start::exec(Command *cmd) {
        is_run = true;
    }

}