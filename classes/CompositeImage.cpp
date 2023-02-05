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

CompositeImage::CompositeImage() {

}

CompositeImage::CompositeImage(int parts, std::string path, int w, int h) {
    //std::string name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    extension = path.substr(path.find_last_of("."), path.length() - path.find_last_of("."));
    this->path = path;
    this->num_parts = parts;

    width = w;
    height = h;

    //compute_avg();
}

CompositeImage::~CompositeImage() {
    //dtor
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
    if (pix.cols < width) {
        inter = cv::INTER_CUBIC;
    } else {
        inter = cv::INTER_AREA;
    }
    cv::resize(pix, pix, cv::Size(width, height), 0, 0, inter);
    cv::cvtColor(pix, pix, cv::COLOR_BGRA2BGR);
    return pix;
}

std::string CompositeImage::get_name() {
    return name;
}

cv::Mat* CompositeImage::get_image() {
    if (is_loaded){
        return &stored_image;
    } else {
        return NULL;
    }
}

std::vector<CompositeImage*>* CompositeImage::get_grid() {
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
    return images_grid[x * num_parts + y];
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

void CompositeImage::change_grid(int x, int y, CompositeImage* image) {
    images_grid[x * num_parts + y] = image;
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

void CompositeImage::push_to_grid(CompositeImage* image) {
    images_grid.push_back(image);
}

void CompositeImage::set_num_unique_images(int num) {
    num_unique_images = num;
}

int CompositeImage::get_width() {
    return width;
}

int CompositeImage::get_height() {
    return height;
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

int CompositeImage::get_num_parts() {
    return num_parts;
}
