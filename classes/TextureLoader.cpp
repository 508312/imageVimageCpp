#include "TextureLoader.h"
#include <iostream>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

template <typename TexType>
TextureLoader<TexType>::TextureLoader() : TextureSetter() {
    mipmaps.push_back(std::unordered_map<std::string, TexType>{});
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(std::initializer_list<int> resoluts) : TextureSetter(resoluts) {
    for (int res : resoluts) {
        mipmaps.push_back(std::unordered_map<std::string, TexType>{});
    }
}

template <typename TexType>
TextureLoader<TexType>::~TextureLoader() {
    //dtor
}
template <typename TexType>
int TextureLoader<TexType>::find_closest_res(int width) {
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

template <typename TexType>
TexType& TextureLoader<TexType>::get_texture(CompositeImage* image, int width) {
    return mipmaps[find_closest_res(width)][image->get_name()];
}

template <typename TexType>
TexType& TextureLoader<TexType>::get_full_texture(CompositeImage* image) {
    return mipmaps[0][image->get_name()];
}
