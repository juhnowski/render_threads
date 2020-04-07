#include "Add.h"


namespace command {

    StreamContext* Add::add_StreamContext(string name, int videoPort, int audioPort) {
        m_stream_cnt.lock();
        int idx = stream_cnt;

        if (idx>slave_stream_max_count) {
            cout << " [-] Maximum slave threads slave_stream_max_count="<<slave_stream_max_count<<" reached." << endl;
            return nullptr;
        }

        stream_cnt++;
        m_stream_cnt.unlock();

        ImageContext *im = new ImageContext(&mu[idx], &images[idx]);

        names[idx] = string(name);
        video_ports[idx] = videoPort;
        audio_ports[idx] = audioPort;

        StreamContext *ctx = new StreamContext(idx, &names[idx], &video_ports[idx], &audio_ports[idx], im,true);

        app_streams.push_back(ctx);
        cout << " [x] Add: "<< ctx << endl;
        return ctx;
    }

    void Add::exec(Command *cmd) {
        add_StreamContext(
                cmd->params_str[ParamEnum::ID],
                cmd->params_int[ParamEnum::VIDEO],
                cmd->params_int[ParamEnum::AUDIO]
        );
    }

}