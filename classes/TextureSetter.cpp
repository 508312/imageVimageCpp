#include "TextureSetter.h"
#include <iostream>

TextureSetter::TextureSetter(ImageBuilder* img_bldr) {
    mResolution.push_back(1600);
    mLoadThreshold = 0;
    this->mImageBuilder = img_bldr;
}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts)
                :TextureSetter(img_bldr, resoluts, 0)  {
}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts,
                              int mLoadThreshold) {
    for (int res : resoluts) {
        mResolution.push_back(res);
    }
    this->mLoadThreshold = mLoadThreshold;
    mImageBuilder = img_bldr;
}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts)
                            :TextureSetter(img_bldr, resoluts, 0) {

}

TextureSetter::TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts,
                              int mLoadThreshold) {
    for (int res : resoluts) {
        mResolution.push_back(res);
    }
    this->mLoadThreshold = mLoadThreshold;
    mImageBuilder = img_bldr;
}

TextureSetter::~TextureSetter()
{
    //dtor
}


void TextureSetter::setBelowThreshold(CompositeImage* image, cv::Mat& pixels) {
    setTexture(image, pixels, mLoadThreshold, mResolution.size());
}

void TextureSetter::setAboveThreshold(CompositeImage* image, cv::Mat& pixels) {
    if (mLoadThreshold <= 0) {
        return;
    }

    setTexture(image, pixels, 0, mLoadThreshold);

    mTextureStatuses[image->getId()] = TEXTURE_LOADED;
}

void TextureSetter::setBelowThreshold(CompositeImage* image) {
    cv::Mat pixels;
    mImageBuilder->createFinal(image->getId(), pixels);
    setBelowThreshold(image, pixels);
}

void TextureSetter::setAboveThreshold(CompositeImage* image) {
    cv::Mat pixels;
    mImageBuilder->createFinal(image->getId(), pixels);
    setAboveThreshold(image, pixels);
}
