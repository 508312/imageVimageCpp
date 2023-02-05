#include "TextureLoader.h"
#include <iostream>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

TextureLoader::TextureLoader()
{
    resolutions.push_back(1600);
    mipmaps.push_back(std::unordered_map<std::string, cv::Mat>{});
}

TextureLoader::TextureLoader(std::initializer_list<int> resoluts)
{
    for (int res : resoluts) {
        resolutions.push_back(res);
        mipmaps.push_back(std::unordered_map<std::string, cv::Mat>{});
    }
}

TextureLoader::~TextureLoader()
{
    //dtor
}

int TextureLoader::find_closest_res(int width) {
    int best = 99999;
    int best_ind = -1;
    for (int i = 0; i < resolutions.size(); i++) {
        if (abs(width - resolutions[i]) < best) {
            best = abs(width - resolutions[i]);
            best_ind = i;
        }
    }
    return best_ind;
}

cv::Mat& TextureLoader::get_texture(CompositeImage* image, int width) {
    return mipmaps[find_closest_res(width)][image->get_name()];
}

cv::Mat& TextureLoader::get_full_texture(CompositeImage* image) {
    return mipmaps[0][image->get_name()];
}

void TextureLoader::set_texture(CompositeImage* image, cv::Mat texture) {
    for (int i = 0; i < resolutions.size(); i++) {
        float scale = (float)resolutions[i]/texture.cols;
        cv::resize(texture, texture, cv::Size(0, 0),
                    scale, scale, cv::INTER_AREA);
        mipmaps[i][image->get_name()] = texture.clone();
    }
}
