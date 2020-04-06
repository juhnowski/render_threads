#include "Executor.h"
#include "Add.h"
#include "Start.h"
#include "Stop.h"
#include <map>

namespace command {

    void Executor::exec(string *str_cmd) {
        Command *cmd = new Command(str_cmd);
        switch (cmd->cmd) {
            case CmdEnum::ADD:
                Add::exec(cmd);
                break;
            case CmdEnum::STOP:
                Stop::exec(cmd);
                break;
            case CmdEnum::START:
                Start::exec(cmd);
                break;
            default:
                //TODO: json format error
                break;
        }

    }
}