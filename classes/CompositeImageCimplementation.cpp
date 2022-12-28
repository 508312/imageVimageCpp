#include "CompositeImageCimplementation.h"
#include <cmath>
#include "color.h"
#include <iostream>
#include <vector>
#include <vips/vips8>
#include <unordered_map>

CompositeImageCimplementation::CompositeImageCimplementation() {

}

CompositeImageCimplementation::CompositeImageCimplementation(int parts, std::string path, int w, int h) {
    //std::string name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    path_to_img = path;
    name = path.substr(path.find_last_of("\\") + 1, path.length() - path.find_last_of("\\") - 1);
    this->num_parts = parts;
    original = vips_image_new_from_file(path.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, NULL);
    if (vips_image_get_width(original) != w || vips_image_get_height(original) != h) {
        factor_width = w/(float)vips_image_get_width(original);
        factor_height = h/(float)vips_image_get_height(original);
        vips_resize(original, &original, factor_width, "vscale", factor_height, NULL);
        std::cout << "resized " << name << std::endl;
    }
    compute_avg();
}

CompositeImageCimplementation::~CompositeImageCimplementation() {
    //dtor
}

VipsImage** CompositeImageCimplementation::get_img() {
    return &original;
}

void CompositeImageCimplementation::create_final(std::unordered_map<std::string, VipsImage**> memo){
    VipsImage* row;
    VipsImage* full;

    VipsImage* img;

    float factor = (vips_image_get_width(original)/(float)num_parts)/vips_image_get_width(original);

    std::cout << "num parts" << num_parts << std::endl;

    for (int i = 0; i < num_parts; i++) {
        for (int j = 0; j < num_parts; j++) {
            if (j == 0) {
                //row = vips_image_new_from_file(images_grid[i*num_parts]->get_path().c_str(), NULL);
                row = *images_grid[i*num_parts]->get_img();
                vips_resize(row, &row, factor, NULL);
                //vips_resize(row, &row, images_grid[i*num_parts + j]->get_factor_w(),
                //             "vscale", images_grid[i*num_parts + j]->get_factor_h(), NULL);
                continue;
            }
            img = *images_grid[i*num_parts + j]->get_img();
            vips_resize(img, &img, factor, NULL);
            //vips_resize(img, &img, images_grid[i*num_parts + j]->get_factor_w(),
            //             "vscale", images_grid[i*num_parts + j]->get_factor_h(), NULL);
            vips_merge(row, img, &row, VIPS_DIRECTION_HORIZONTAL, -vips_image_get_width(row), 0, NULL);
            //std::string path = "folder\\" + std::to_string(i) + "_" + std::to_string(j) + ".jpg";
            //std::cout << i << " " << j << "   " << images_grid[i*num_parts + j]->get_path() << std::endl;

            //vips_image_write_to_file(row, path.c_str(), NULL);

        }
        if (i == 0) {
            full = row;
            continue;
        }
        vips_merge(full, row, &full, VIPS_DIRECTION_VERTICAL, 0, -vips_image_get_height(full), NULL);

        //std::cout << "row " << i << " done" << std::endl;

    }

    vips_image_write_to_file(full, ("folder\\" + name).c_str(), NULL);

}

std::string CompositeImageCimplementation::get_path() {
    return path_to_img;
}

/* Returns distance of average colors between two images. */
float CompositeImageCimplementation::get_distance_to_img(CompositeImageCimplementation* img2) {
    return distance(get_avg_color(), img2->get_avg_color());
}

float CompositeImageCimplementation::get_distance_to_color(color clr) {
    return distance(get_avg_color(), clr);
}

float CompositeImageCimplementation::distance(color c1, color c2) {
    return (std::pow((c2.r-c1.r)*0.3, 2), 2 + std::pow((c2.g-c1.g)*0.59, 2) + std::pow((c2.b-c1.b)*0.11, 2));
    //(std::pow((c2.r-c1.r)*0.3, 2), 2 + std::pow((c2.g-c1.g)*0.59, 2) + std::pow((c2.b-c1.b)*0.11, 2));
}

color CompositeImageCimplementation::get_avg_color() {
    return average;
}

void CompositeImageCimplementation::load_into_mem() {
    original = vips_image_new_from_file(path_to_img.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, "memory", TRUE, NULL);
    vips_resize(original, &original, factor_width, "vscale", factor_height, NULL);
}

color CompositeImageCimplementation::crop_avg_color(int left, int top, int width, int height) {
    VipsImage* cropped;

    vips_crop(original, &cropped, left, top, width, height, NULL);

    return image_average(&cropped);
}

color CompositeImageCimplementation::image_average(VipsImage** image) {
    color c;
    VipsImage* stats;
    int n;
    double* vec;

    vips_stats(*image, &stats, NULL);
    vips_getpoint(stats, &vec, &n, 4, 1, NULL);
    c.r =  vec[0];

    vips_getpoint(stats, &vec, &n, 4, 2, NULL);
    c.g =  vec[0];

    vips_getpoint(stats, &vec, &n, 4, 3, NULL);
    c.b = vec[0];

    g_free(vec);
    g_object_unref(stats);

    return c;
}

float CompositeImageCimplementation::get_factor_w() {
    return factor_width;
}

float CompositeImageCimplementation::get_factor_h() {
    return factor_height;
}


void CompositeImageCimplementation::push_to_grid(CompositeImageCimplementation* image) {
    images_grid.push_back(image);
}

void CompositeImageCimplementation::compute_avg() {
    average = image_average(&original);
}

int CompositeImageCimplementation::get_num_parts() {
    return num_parts;
}
