#ifndef TEST_THREADS_IMAGECONTEXT_H
#define TEST_THREADS_IMAGECONTEXT_H
#include <mutex>
#include <iostream>
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

class ImageContext {
public:
    ImageContext(mutex *mtx, cv::Mat *image);
    ImageContext(mutex *mtx);
    string to_string() const;
    friend ostream& operator << (ostream& outs, const ImageContext* obj);

    cv::Mat *image;
    std::mutex *mtx;
};


#endif //TEST_THREADS_IMAGECONTEXT_H
