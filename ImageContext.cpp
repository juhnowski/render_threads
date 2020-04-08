#include "ImageContext.h"

ImageContext::ImageContext(mutex *mtx, cv::Mat *image) : mtx(mtx), image(image) {
    this->mtx = mtx;
    this->image = image;
}

ImageContext::ImageContext(mutex *mtx) : mtx(mtx) {
    this->mtx = mtx;
}

string ImageContext::to_string() const {
    return "image";//std::to_string(*image);
}

ostream &operator<<(ostream &outs, const ImageContext *obj) {
    return outs << obj->to_string();
}
