#include "TextureLoader.h"
#include <iostream>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

TextureLoader::TextureLoader()
{
    //ctor
}

TextureLoader::~TextureLoader()
{
    //dtor
}

cv::Mat& TextureLoader::get_texture(CompositeImage* image, int width) {
    return table[image->get_name()];
}

cv::Mat& TextureLoader::get_full_texture(CompositeImage* image) {
    return table[image->get_name()];
}

void TextureLoader::set_texture(CompositeImage* image, cv::Mat texture) {
    table[image->get_name()] = texture;
}
