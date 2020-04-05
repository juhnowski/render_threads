#ifndef TEST_THREADS_IMAGECONTEXT_H
#define TEST_THREADS_IMAGECONTEXT_H
#include <mutex>
#include <iostream>

using namespace std;

class ImageContext {
public:
    ImageContext(mutex *mtx, int *image);
    string to_string() const;
    friend ostream& operator << (ostream& outs, const ImageContext* obj);

    int *image;
    std::mutex *mtx;
};


#endif //TEST_THREADS_IMAGECONTEXT_H
