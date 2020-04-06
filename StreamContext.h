#ifndef TEST_THREADS_STREAMCONTEXT_H
#define TEST_THREADS_STREAMCONTEXT_H

#include <string>
#include "ImageContext.h"

using namespace std;

class StreamContext {
public:
    StreamContext(int idx, string *name, int *videoPort, int *audioPort, ImageContext *imageCtx, bool isActive);

    string to_string() const;
    friend ostream& operator << (ostream& outs, const StreamContext* obj);
    int idx;
    string *name;
    int *video_port;
    int *audio_port;
    ImageContext *image_ctx;
    bool is_active;
};


#endif //TEST_THREADS_STREAMCONTEXT_H
