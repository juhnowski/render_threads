#include "StreamContext.h"

StreamContext::StreamContext(int idx, string *name, int *videoPort, int *audioPort, string *rtmp, ImageContext *imageCtx, bool isActive)
        : name(name), video_port(videoPort), audio_port(audioPort), image_ctx(imageCtx), is_active(isActive) {
    this->idx = idx;
    this->name = name;
    this->video_port = videoPort;
    this->audio_port = audioPort;
    this->image_ctx = imageCtx;
    this->is_active = isActive;
    this->rtmp = rtmp;
}

string StreamContext::to_string() const {
    return std::string(*name);
}

ostream &operator<<(ostream &outs, const StreamContext *obj) {
    return outs << obj->to_string();
}

