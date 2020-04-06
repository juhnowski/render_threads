//
// Created by ilya on 06.04.2020.
//

#include "Command.h"

namespace command {

    using namespace std;

    Command::Command(string *json_str) {
        auto j = json::parse(*json_str);
        string s_cmd = j[STR_MSG_COMMAND];
        cmd = cmd_from_string(&s_cmd);
        auto p = j[STR_MSG_PARAMS];
        string name = p[STR_PARAM_ID];
        int video = p[STR_PARAM_VIDEO];
        int audio = p[STR_PARAM_AUDIO];
        name.erase(std::remove(name.begin(), name.end(), '"'), name.end());
        params_str.insert(make_pair(ParamEnum::ID, name));
        params_int.insert(make_pair(ParamEnum::VIDEO, video));
        params_int.insert(make_pair(ParamEnum::AUDIO, audio));
    }

    CmdEnum Command::cmd_from_string(string *value) {
        static const std::map<std::string, CmdEnum> enumStringsMap = {
                { STR_CMD_ADD, CmdEnum::ADD },
                { STR_CMD_START, CmdEnum::START },
                { STR_CMD_STOP, CmdEnum::STOP } };

        auto it = enumStringsMap.find(*value);

        if(it==enumStringsMap.end())
            return CmdEnum::UNDEFINED;

        return it->second;
    }

    ParamEnum Command::param_from_string(string *value) {
        static const std::map<std::string, ParamEnum> enumStringsMap = {
                { STR_PARAM_ID, ParamEnum::ID },
                { STR_PARAM_AUDIO, ParamEnum::AUDIO },
                { STR_PARAM_VIDEO, ParamEnum::VIDEO },
                { STR_PARAM_RTMP, ParamEnum::RTMP },
        };

        auto it = enumStringsMap.find(*value);

        if(it==enumStringsMap.end())
            return ParamEnum::UNDEFINED;

        return it->second;
    }


}
