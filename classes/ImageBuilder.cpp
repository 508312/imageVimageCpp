#include "ImageBuilder.h"
#include "CompositeImage.h"
#include <filesystem>

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

    for (int i=0; i < parts; i++) {
        for (int j = 0; j < parts; j++) {
            closest = find_closest_image(ind);
            images[ind].push_to_grid(closest);
        }
    }
}
// TODO WHAT THE FUCK DID I DO LMAO LOL FUCK CROPS CROPS CROPS AND AVERAGES N STUFF
CompositeImage* ImageBuilder::find_closest_image(int ind) {
    int best_index = 0;
    float best_distance = 10000;
    float distance;

    for (int i=0; i < get_num_images(); i++) {
        if (i == ind) {
            continue;
        }
        distance = images[i].get_distance_to_img(&images[ind]);
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
        images.push_back(CompositeImage(num_parts, entry.path().string()));
    }
}

std::vector<CompositeImage>* ImageBuilder::get_images() {
    return &images;
}
