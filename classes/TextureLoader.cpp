#include "TextureLoader.h"
#include <iostream>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <thread>
#include <future>


template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr) : TextureSetter(img_bldr) {
    mipmaps.push_back(std::vector<TexType>{});
}

template <typename TexType>
TextureLoader<TexType>::~TextureLoader() {
}

template <typename TexType>
void TextureLoader<TexType>::free_textures() {
    int amnt = 0;

    for (int i = 0; i < resolutions.size(); i++) {
        for(int j = 0; j < mipmaps[i].size(); j++) {
            free_texture(i, j);
            amnt++;
        }
    }
    std::cout << "DELETED " << amnt << std::endl;
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts)
 : TextureLoader(img_bldr, resoluts, 0) {
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts,
                                int load_threshold) : TextureSetter(img_bldr, resoluts, load_threshold) {
    for (int res : resoluts) {
        mipmaps.push_back(std::vector<TexType>{});
    }
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts)
 : TextureLoader(img_bldr, resoluts, 0) {
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts,
                            int load_threshold) : TextureSetter(img_bldr, resoluts, load_threshold) {
    for (int res : resoluts) {
        mipmaps.push_back(std::vector<TexType>{});
    }
}

template <typename TexType>
void TextureLoader<TexType>::resize_to(int amnt) {
    for (int i = texture_status.size(); i < amnt; i++) {
        texture_status.push_back(TEXTURE_NOT_LOADED);
        std::cout << (int)texture_status[i] << std::endl;
    }
    for (int i = 0; i < mipmaps.size(); i++) {
        mipmaps[i].resize(amnt);
    }
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

    /*
    int best_ind = resolutions.size() - 1;
    while (width > resolutions[best_ind - 1] && best_ind > 0) {
        best_ind--;
    }
    return best_ind;
    */
}

template <typename TexType>
TexType& TextureLoader<TexType>::get_texture(CompositeImage* image, int width) {
    int index = find_closest_res(width);
    if (index == load_threshold && texture_status[image->get_ind()] == TEXTURE_NOT_LOADED) {
        texture_status[image->get_ind()] = TEXTURE_STARTED_LOADING;
        std::thread t1([this,image](){set_above_threshold(image);});
        t1.detach();
    }

    while (mipmaps[index][image->get_ind()] == NULL) {
        index++;
    }
    return mipmaps[index][image->get_ind()];
}

template <typename TexType>
TexType& TextureLoader<TexType>::get_full_texture(CompositeImage* image) {
    int index = 0;
    while (mipmaps[index][image->get_ind()] == NULL) {
        index++;
    }
    return mipmaps[0][image->get_ind()];
}
