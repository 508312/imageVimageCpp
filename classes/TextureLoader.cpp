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
    mMipmaps.push_back(std::vector<TexType>{});
}

template <typename TexType>
TextureLoader<TexType>::~TextureLoader() {
}

template <typename TexType>
void TextureLoader<TexType>::freeTextures() {
    int amnt = 0;

    for (int i = 0; i < mResolution.size(); i++) {
        for(int j = 0; j < mMipmaps[i].size(); j++) {
            freeTexture(i, j);
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
                                int mLoadThreshold) : TextureSetter(img_bldr, resoluts, mLoadThreshold) {
    for (int res : resoluts) {
        mMipmaps.push_back(std::vector<TexType>{});
    }
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts)
 : TextureLoader(img_bldr, resoluts, 0) {
}

template <typename TexType>
TextureLoader<TexType>::TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts,
                            int mLoadThreshold) : TextureSetter(img_bldr, resoluts, mLoadThreshold) {
    for (int res : resoluts) {
        mMipmaps.push_back(std::vector<TexType>{});
    }
}

template <typename TexType>
void TextureLoader<TexType>::resizeTo(int amnt) {
    for (int i = mTextureStatuses.size(); i < amnt; i++) {
        mTextureStatuses.push_back(TEXTURE_NOT_LOADED);
    }
    for (int i = 0; i < mMipmaps.size(); i++) {
        mMipmaps[i].resize(amnt);
    }
}

template <typename TexType>
int TextureLoader<TexType>::findClosestRes(int width) {

    int best = 99999;
    int best_ind = -1;
    for (int i = 0; i < mResolution.size(); i++) {
        if (abs(width - mResolution[i]) < best) {
            best = abs(width - mResolution[i]);
            best_ind = i;
        }
    }
    return best_ind;

    /*
    int best_ind = mResolution.size() - 1;
    while (width > mResolution[best_ind - 1] && best_ind > 0) {
        best_ind--;
    }
    return best_ind;
    */
}

template <typename TexType>
TexType& TextureLoader<TexType>::getTexture(CompositeImage* image, int width) {
    int index = findClosestRes(width);
    int del = -1;

    if (index <= mLoadThreshold) {
        //std::cout << "CACHE PUT " << image->getId() << std::endl;
        del = mTextureCache.put(image->getId());
        //std::cout << "CACHE PUT DONE " << std::endl;


        //if (del != -1) {
        //    std::cout << "DEL NOT ZERO " << del << " STATUS " << (int) mTextureStatuses[image->getId()] << std::endl;
        //}

        if (del != -1 && mTextureStatuses[image->getId()] == TEXTURE_LOADED) {
            std::cout << "STARTED UNLOADING " << del << std::endl;
            mTextureStatuses[image->getId()] = TEXTURE_STARTED_UNLOADING;
            std::thread dt([this,del](){freeTextureAboveThreshold(del);});
            dt.detach();
        }
    }

    if (index <= mLoadThreshold && mTextureStatuses[image->getId()] == TEXTURE_NOT_LOADED) {
        mTextureStatuses[image->getId()] = TEXTURE_STARTED_LOADING;
        std::thread ct([this,image](){setAboveThreshold(image);});
        ct.detach();
    }

    if (mTextureStatuses[image->getId()] == TEXTURE_LOADED || index >= mLoadThreshold) {
        return mMipmaps[index][image->getId()];
    }
    return mMipmaps[mLoadThreshold][image->getId()];
}

template <typename TexType>
TexType& TextureLoader<TexType>::getFullTexture(CompositeImage* image) {
    int index = 0;
    while (mMipmaps[index][image->getId()] == NULL) {
        index++;
    }
    return mMipmaps[0][image->getId()];
}

template <typename TexType>
void TextureLoader<TexType>::freeTextureAboveThreshold(int img_index) {
    for (int i = 0; i < mLoadThreshold; i++) {
        freeTexture(i, img_index);
    }
    std::cout << "UNLOADING SUCCESSFULL " << img_index << std::endl;
    mTextureStatuses[img_index] == TEXTURE_NOT_LOADED;
}
