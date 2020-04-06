//
// Created by ilya on 06.04.2020.
//

#ifndef TEST_THREADS_COMMAND_H
#define TEST_THREADS_COMMAND_H

#include <string>
#include "../libh/json.hpp"
#include <map>

namespace command {
    using namespace std;
    using json = nlohmann::json;

    enum class CmdEnum {UNDEFINED, ADD, START, STOP};

    static const string STR_MSG_COMMAND = "command";
    static const string STR_MSG_PARAMS = "params";

    static const string  STR_CMD_ADD= "add";
    static const string  STR_CMD_START= "start";
    static const string  STR_CMD_STOP= "stop";

    static const string  STR_PARAM_ID= "id";
    static const string  STR_PARAM_ID_SLAVE= "slave";
    static const string  STR_PARAM_ID_MASTER= "master";
    static const string  STR_PARAM_AUDIO= "audio";
    static const string  STR_PARAM_VIDEO= "video";
    static const string  STR_PARAM_RTMP= "rtmp";

    enum class ParamEnum {UNDEFINED, ID, AUDIO, VIDEO, RTMP};

    class Command {
    public:
        Command(string *json_str);
        Command();
        string *json_str;
        CmdEnum cmd;

        map<ParamEnum,string> params_str;
        map<ParamEnum,int> params_int;
    private:
        static CmdEnum cmd_from_string(string *value);
        static ParamEnum param_from_string(string *value);
    };




}

#endif //TEST_THREADS_COMMAND_H
