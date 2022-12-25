#include "ImageBuilder.h"

ImageBuilder::ImageBuilder() {
    //ctor
}

ImageBuilder::~ImageBuilder() {
    //dtor
}

void ImageBuilder::build_images() {
    for (int i = 0; i < get_num_images(); i++) {
        build_image(i);
    }
}

void ImageBuilder::build_image(int i) {

}

int ImageBuilder::get_num_images() {
    return images.size();
}
