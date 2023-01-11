#include "ImageBuilder.h"
#include "CompositeImage.h"
#include <filesystem>
#include <iostream>

ImageBuilder::ImageBuilder() {
    //ctor
    num_parts = 40;
}

ImageBuilder::ImageBuilder(int parts) {
    num_parts = parts;
}

ImageBuilder::~ImageBuilder() {
    //dtor
}

void ImageBuilder::build_images() {
    for (int i = 0; i < get_num_images(); i++) {
        build_image(i);
    }
}

void ImageBuilder::build_image(int ind) {
    CompositeImage* closest;
    int parts = images[ind].get_num_parts();
    color crop_clr;
    int left, top;

    std::cout << "got here 1" << std::endl;
    for (int i=0; i < parts; i++) {
        top = i * (height/parts);
        for (int j = 0; j < parts; j++) {
            left = j * (width/parts);

            closest = find_closest_image(ind, images[ind].crop_avg_color(left, top, width/parts, height/parts));
            images[ind].push_to_grid(closest);
        }
    }
}

CompositeImage* ImageBuilder::find_closest_image(int ind, color clr) {
    int best_index = 0;
    float best_distance = 200000;
    float distance;

    for (int i=0; i < get_num_images(); i++) {
        if (i == ind) {
            //continue;
        }
        distance = images[i].get_distance_to_color(clr);
        if (distance < best_distance) {
            best_index = i;
        }
    }
    return &images[best_index];
}


int ImageBuilder::get_num_images() {
    return images.size();
}

void ImageBuilder::load_images(std::string path) {
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path().string() << std::endl;
        images.push_back(CompositeImage(num_parts, entry.path().string(), width, height));
    }
}

std::vector<CompositeImage>* ImageBuilder::get_images() {
    return &images;
}
