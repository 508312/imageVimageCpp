#include "ImageBuilderCimplementation.h"
#include "CompositeImageCimplementation.h"
#include <filesystem>
#include <iostream>
#include <Timer.h>
#include <unordered_map>

ImageBuilderCimplementation::ImageBuilderCimplementation() {
    //ctor
    num_parts = 120;
    width = 1600;
    height = 1600;
}

ImageBuilderCimplementation::ImageBuilderCimplementation(int parts) {
    num_parts = parts;
}

ImageBuilderCimplementation::~ImageBuilderCimplementation() {
    //dtor
}

void ImageBuilderCimplementation::build_images() {
    for (int i = 0; i < get_num_images(); i++) {
        images[i].load_into_mem();
    }
    std::unordered_map<std::string, VipsImage*> mymap;

    for (int i = 0; i < get_num_images(); i++) {
        build_image(i, &mymap);
    }
}

void ImageBuilderCimplementation::build_image(int ind, std::unordered_map<std::string, VipsImage*>* memo) {
    CompositeImageCimplementation* closest;
    int parts = images[ind].get_num_parts();
    color crop_clr;
    int left, top;

    //images[ind].load_into_mem();
    Timer t;
    t.start();

    for (int i=0; i < parts; i++) {
        top = i * (height/parts);
        for (int j = 0; j < parts; j++) {
            left = j * (width/parts);

            closest = find_closest_image(ind, images[ind].crop_avg_color(left, top, width/parts, height/parts));
            //std::cout << "found closest for " << i << " " << j << std::endl;

            images[ind].push_to_grid(closest);
        }
    }
    std::cout << "composing " << ind << " done " << t.get() << std::endl;


    t.start();
    images[ind].create_final(memo);
    std::cout << "image " << ind << " done " << t.get() << std::endl;
}

CompositeImageCimplementation* ImageBuilderCimplementation::find_closest_image(int ind, color clr) {
    int best_index = 0;
    float best_distance = 2000000;
    float distance;

    for (int i=0; i < get_num_images(); i++) {
        if (i == ind) {
            continue;
        }
        distance = images[i].get_distance_to_color(clr);
        if (distance < best_distance) {
            best_index = i;
            best_distance = distance;
        }
    }
    return &images[best_index];
}


int ImageBuilderCimplementation::get_num_images() {
    return images.size();
}

void ImageBuilderCimplementation::load_images(std::string path) {
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path().string() << std::endl;
        images.push_back(CompositeImageCimplementation(num_parts, entry.path().string(), width, height));
    }
}

std::vector<CompositeImageCimplementation>* ImageBuilderCimplementation::get_images() {
    return &images;
}
