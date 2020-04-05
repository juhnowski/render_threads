#include "ImageContext.h"

ImageContext::ImageContext(mutex *mtx, int *image) : mtx(mtx), image(image) {
    this->mtx = mtx;
    this->image = image;
}

string ImageContext::to_string() const {
    return std::to_string(*image);
}

ostream &operator<<(ostream &outs, const ImageContext *obj) {
    return outs << obj->to_string();
}
