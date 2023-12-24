#include "CompositeImage.h"
#include <cmath>
#include "color.h"
#include <iostream>
#include <vector>
#include "Timer.h"

#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <functional>
#include <unordered_map>
CompositeImage::CompositeImage(int parts_w, int parts_h, std::string path,
                                int w, int h, cid index,  std::vector<CompositeImage*>* ind_map) {
    //std::string name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    mName = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    mExtension = path.substr(path.find_last_of("."), path.length() - path.find_last_of("."));
    mPath = path;
    this->mNumWidth = parts_w;
    this->mNumHeight = parts_h;
    ind_img_map = ind_map;
    mId = index;

    mWidth = w;
    mHeight = h;

    //computeAvgColor();
}

CompositeImage::~CompositeImage() {
    //dtor
}

void CompositeImage::resizeGrid() {
    mGrid.resize(mNumHeight*mNumWidth);
}

void CompositeImage::unloadImageFromMemory() {
    mImgLoaded = false;
    mStoredImage.release();
}

void CompositeImage::loadImageToMemory() {
    mImgLoaded = true;

    mStoredImage = loadImage();
}

cv::Mat CompositeImage::loadImage() {
    cv::Mat pix = cv::imread(mPath, cv::IMREAD_COLOR);
    int inter;
    if (pix.cols < mWidth) {
        inter = cv::INTER_CUBIC;
    } else {
        inter = cv::INTER_AREA;
    }
    cv::resize(pix, pix, cv::Size(mWidth, mHeight), 0, 0, inter);
    cv::cvtColor(pix, pix, cv::COLOR_BGRA2BGR);
    return pix;
}

std::string CompositeImage::getName() {
    return mName;
}

cid CompositeImage::getId() {
    return mId;
}

cv::Mat* CompositeImage::getImage() {
    if (mImgLoaded){
        return &mStoredImage;
    } else {
        return NULL;
    }
}

std::vector<cid>* CompositeImage::getGrid() {
    return &mGrid;
}

/* Returns distance of average colors between two images. */
int CompositeImage::distanceToImage(CompositeImage* img2) {
    return distance(getAvgColor(), img2->getAvgColor());
}

int CompositeImage::distanceToColor(const color& clr) {
    return distance(getAvgColor(), clr);
}

int CompositeImage::distance(const color& c1, const color& c2) {
    int red = c2.r - c1.r;
    int green = c2.g - c1.g;
    int blue = c2.b - c1.b;
    return red * red + green * green + blue * blue;
}


color CompositeImage::getAvgColor() {
    return mAvgColor;
}

std::string CompositeImage::getExtension(){
    return mExtension;
}

CompositeImage* CompositeImage::getImageAt(int x, int y) {
    return (*ind_img_map)[mGrid[x * mNumWidth + y]];
}

cid CompositeImage::getImageIdAt(int x, int y) {
    return mGrid[x * mNumWidth + y];
}

color CompositeImage::getCropAvgColor(int left, int top, int width, int height) {
    cv::Rect crop(left, top, width, height);

    cv::Mat pix;

    bool free_flag = false;

    if (getImage() == NULL) {
        loadImageToMemory();
        free_flag = true;
    } else {
        pix = *getImage();
    }

    cv::Mat cropped(mStoredImage, crop);

    color clr = imageAvgColor(&cropped);

    if (free_flag) {
        unloadImageFromMemory();
    }

    return clr;
}

void CompositeImage::setImageAt(int x, int y, cid img) {
    mGrid[x * mNumWidth + y] = img;
}

color CompositeImage::imageAvgColor(cv::Mat* image) {
    color c;

    cv::Mat pixels = *image;
    cv::cvtColor(pixels, pixels, cv::COLOR_BGR2Lab);
    cv::Scalar avg = cv::mean(pixels);

    c.r = avg[0];
    c.g = avg[1];
    c.b = avg[2];

    /*
    cv::Scalar avg = cv::mean(*image);

    cv::Mat lab;
    cv::Mat bgr(1,1, CV_8UC3, avg);
    cv::cvtColor(bgr, lab, cv::COLOR_BGR2Lab);

    c.r = lab.data[0];
    c.g = lab.data[1];
    c.b = lab.data[2];
    */

    return c;
}

void CompositeImage::pushToGrid(cid image) {
    mGrid.push_back(image);
}

int CompositeImage::getNumWidth() {
    return mNumWidth;
}

int CompositeImage::getNumHeight() {
    return mNumHeight;
}

int CompositeImage::getWidth() {
    return mWidth;
}

int CompositeImage::getHeight() {
    return mHeight;
}

void CompositeImage::computeAvgColor() {
    cv::Mat pix;
    bool free_flag = false;
    if (getImage() == NULL) {
        loadImageToMemory();
        free_flag = true;
    }

    //cv::resize(mStoredImage, mStoredImage, cv::Size(width/num_parts, height/num_parts), 0, 0, cv::INTER_AREA);
    mAvgColor = imageAvgColor(&mStoredImage);

    if (free_flag) {
        unloadImageFromMemory();
    }
}

void CompositeImage::coalesceBlocks(int max_size) {
    int ii;
    int ij;
    cid start_ind;
    uint32_t max_i;
    uint32_t max_j;
    for (int pi = 0; pi < mNumHeight - 1; pi++) {
        for (int pj = 0; pj < mNumWidth - 1; pj++) {
            start_ind = getImageIdAt(pi, pj);
            if (start_ind == (cid)-1 || start_ind == (cid)-2) {
                continue;
            }
            ii = pi;
            ij = pj;
            max_i = UINT32_MAX;
            max_j = UINT32_MAX;
            while (true) {
                ij++;

                if (ij >= max_j) {
                    ii++;
                    ij = pj;
                }

                if (ij >= mNumWidth) {
                    int diag_i = pi + std::max(ii - pi, ij - pj);
                    int diag_j = pj + diag_i - pi;
                    if (diag_i < max_i) {
                        max_i = diag_i;
                        max_j = diag_j;
                    }

                    ii++;
                    ij = pj;
                }

                if (ii >= mNumHeight) {
                    int diag_i = pi + std::max(ii - pi, ij - pj);
                    int diag_j = pj + diag_i - pi;
                    if (diag_i < max_i) {
                        max_i = diag_i;
                        max_j = diag_j;
                    }
                    break;
                }

                if (ii >= max_i) {
                    break;
                }

                if (getImageIdAt(ii, ij) != start_ind) {
                    int diag_i = pi + std::max(ii - pi, ij - pj);
                    int diag_j = pj + diag_i - pi;
                    if (diag_i < max_i) {
                        max_i = diag_i;
                        max_j = diag_j;
                    }
                }
            }

            max_i = std::min((uint32_t)pi + max_size, max_i);
            max_j = std::min((uint32_t)pj + max_size, max_j);
            if (max_i - pi <= 1) {
                continue;
            }
            //std::cout << "max i j " << max_i << max_j << std::endl;
            //std::cout << "p i j " << pi << pj << std::endl;
            //std::cout << "doing last" << std::endl;
            for (int i = pi; i < max_i; i++) {
                for (int j = pj; j < max_j; j++) {
                    if (i == pi && j == pj) {
                        continue;
                    }
                    //if (i == pi + 1 || j == pj + 1) {
                    //    setImageAt(i, j, -3);
                    if (i == pi && j == max_j - 1) {
                        setImageAt(i, j, -2);
                    } else {
                        setImageAt(i, j, -1);
                    }
                }
            }
            //std::cout << "finished last" << std::endl;
        }
    }
}

/*
AAABAA
AAAAAA
ABAAAA

*/
