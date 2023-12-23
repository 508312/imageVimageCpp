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
                                int w, int h, uint16_t index,  std::vector<CompositeImage*>* ind_map) {
    //std::string name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    extension = path.substr(path.find_last_of("."), path.length() - path.find_last_of("."));
    this->path = path;
    this->mNumWidth = parts_w;
    this->mNumHeight = parts_h;
    ind_img_map = ind_map;
    this->index = index;

    mWidth = w;
    mHeight = h;

    //compute_avg();
}

CompositeImage::~CompositeImage() {
    //dtor
}

void CompositeImage::fill_grid_with_empty() {
    images_grid.resize(mNumHeight*mNumWidth);
}

void CompositeImage::unload_from_mem() {
    is_loaded = false;
    stored_image.release();
}

void CompositeImage::load_to_mem() {
    is_loaded = true;
    stored_image = load_image();
}

cv::Mat CompositeImage::load_image() {
    cv::Mat pix = cv::imread(path, cv::IMREAD_COLOR);
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

std::string CompositeImage::get_name() {
    return name;
}

uint16_t CompositeImage::get_ind() {
    return index;
}

cv::Mat* CompositeImage::get_image() {
    if (is_loaded){
        return &stored_image;
    } else {
        return NULL;
    }
}

std::vector<uint16_t>* CompositeImage::get_grid() {
    return &images_grid;
}

/* Returns distance of average colors between two images. */
int CompositeImage::get_distance_to_img(CompositeImage* img2) {
    return distance(get_avg_color(), img2->get_avg_color());
}

int CompositeImage::get_distance_to_color(const color& clr) {
    return distance(get_avg_color(), clr);
}

int CompositeImage::distance(const color& c1, const color& c2) {
    int red = c2.r - c1.r;
    int green = c2.g - c1.g;
    int blue = c2.b - c1.b;
    return red * red + green * green + blue * blue;
}


color CompositeImage::get_avg_color() {
    return average;
}

std::string CompositeImage::get_extension(){
    return extension;
}

CompositeImage* CompositeImage::get_image_at(int x, int y) {
    return (*ind_img_map)[images_grid[x * mNumWidth + y]];
}

uint16_t CompositeImage::get_image_index_at(int x, int y) {
    return images_grid[x * mNumWidth + y];
}

color CompositeImage::crop_avg_color(int left, int top, int width, int height) {
    cv::Rect crop(left, top, width, height);

    cv::Mat pix;

    bool free_flag = false;

    if (get_image() == NULL) {
        load_to_mem();
        free_flag = true;
    } else {
        pix = *get_image();
    }

    cv::Mat cropped(stored_image, crop);

    color clr = image_average(&cropped);

    if (free_flag) {
        unload_from_mem();
    }

    return clr;
}

void CompositeImage::set_image_at(int x, int y, uint16_t img) {
    images_grid[x * mNumWidth + y] = img;
}

color CompositeImage::image_average(cv::Mat* image) {
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

void CompositeImage::push_to_grid(uint16_t image) {
    images_grid.push_back(image);
}

void CompositeImage::set_num_unique_images(int num) {
    num_unique_images = num;
}
int CompositeImage::get_mNumWidthidth() {
    return mNumWidth;
}

int CompositeImage::get_mNumHeighteight() {
    return mNumHeight;
}

int CompositeImage::get_width() {
    return mWidth;
}

int CompositeImage::get_height() {
    return mHeight;
}

void CompositeImage::compute_avg() {
    cv::Mat pix;
    bool free_flag = false;
    if (get_image() == NULL) {
        load_to_mem();
        free_flag = true;
    }

    //cv::resize(stored_image, stored_image, cv::Size(width/num_parts, height/num_parts), 0, 0, cv::INTER_AREA);
    average = image_average(&stored_image);

    if (free_flag) {
        unload_from_mem();
    }
}

void CompositeImage::coalesce_blocks(int max_size) {
    int ii;
    int ij;
    uint16_t start_ind;
    uint32_t max_i;
    uint32_t max_j;
    for (int pi = 0; pi < mNumHeight - 1; pi++) {
        for (int pj = 0; pj < mNumWidth - 1; pj++) {
            start_ind = get_image_index_at(pi, pj);
            if (start_ind == (uint16_t)-1 || start_ind == (uint16_t)-2) {
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

                if (get_image_index_at(ii, ij) != start_ind) {
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
                    //    set_image_at(i, j, -3);
                    if (i == pi && j == max_j - 1) {
                        set_image_at(i, j, -2);
                    } else {
                        set_image_at(i, j, -1);
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
