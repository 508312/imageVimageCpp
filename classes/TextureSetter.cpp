#include "TextureSetter.h"
#include <iostream>

TextureSetter::TextureSetter(ImageBuilder* img_bldr) {
    resolutions.push_back(1600);
    load_threshold = 0;
    this->image_builder = img_bldr;
}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts)
                :TextureSetter(img_bldr, resoluts, 0)  {
}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts,
                              int load_threshold) {
    for (int res : resoluts) {
        resolutions.push_back(res);
    }
    this->load_threshold = load_threshold;
    image_builder = img_bldr;
}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts)
                            :TextureSetter(img_bldr, resoluts, 0) {

}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts,
                              int load_threshold) {
    for (int res : resoluts) {
        resolutions.push_back(res);
    }
    this->load_threshold = load_threshold;
    image_builder = img_bldr;
}

TextureSetter::~TextureSetter()
{
    //dtor
}


void TextureSetter::set_below_threshold(CompositeImage* image, cv::Mat& pixels) {
    set_texture(image, pixels, load_threshold, resolutions.size());
}

#include <windows.h>
void TextureSetter::set_above_threshold(CompositeImage* image, cv::Mat& pixels) {
    if (load_threshold <= 0) {
        return;
    }

    set_texture(image, pixels, 0, load_threshold);

    Sleep(5);
    texture_statuses[image->get_ind()] = TEXTURE_LOADED;
}

void TextureSetter::set_below_threshold(CompositeImage* image) {
    cv::Mat pixels;
    image_builder->create_final(image->get_ind(), pixels);
    set_below_threshold(image, pixels);
}

void TextureSetter::set_above_threshold(CompositeImage* image) {
    cv::Mat pixels;
    image_builder->create_final(image->get_ind(), pixels);
    set_above_threshold(image, pixels);
}
