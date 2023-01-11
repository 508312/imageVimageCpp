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
    name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    path_to_img = path;
    //name = path.substr(path.find_last_of("\\") + 1, path.length() - path.find_last_of("\\") - 1);
    this->num_parts = parts;
    original = vips_image_new_from_file(path.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, NULL);
    if (vips_image_hasalpha(original)) {
        std::cout << "FLATTENED" << std::endl;
        vips_flatten(original, &original, NULL);
    }
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

void CompositeImageCimplementation::create_final(std::unordered_map<std::string, VipsImage*>* memo){
    int n = images_grid.size();
    float factor = (vips_image_get_width(original)/(float)num_parts)/vips_image_get_width(original);
    int small_width = vips_image_get_width(original)/num_parts;
    int small_height = vips_image_get_height(original)/num_parts;
    VipsImage* full = NULL;

    VipsImage* iarr[n];

    /*
    std::unordered_map<std::string, int> contains;
    for (int i = 0; i < n; i++) {
        if(contains[images_grid[i]->name] == 0) {
            (*memo)[images_grid[i]->name] = *images_grid[i]->get_img();
            vips_resize((*memo)[images_grid[i]->name], &(*memo)[images_grid[i]->name], factor, NULL);

            vips_image_write_to_file((*memo)[images_grid[i]->name], ("folder2\\" + images_grid[i]->name).c_str(), NULL);

            iarr[0] = (*memo)[images_grid[i]->name];
            iarr[1] = (*memo)[images_grid[i]->name];

            //vips_arrayjoin(iarr, &full, 2, NULL);

            contains[images_grid[i]->name] = 1;
            if (full == NULL) {
                std::cout << images_grid[i]->name << std::endl;
            }
        }
        std::cout << "reg" << images_grid[i]->name << std::endl;

    }
    */

    for (int i = 0; i < n; i++) {
        if((*memo)[images_grid[i]->name] == 0) {
            (*memo)[images_grid[i]->name] = *images_grid[i]->get_img();
            vips_resize((*memo)[images_grid[i]->name], &(*memo)[images_grid[i]->name], factor, NULL);

        }
        iarr[i] = (*memo)[images_grid[i]->name];

        //iarr[i] = images_grid[i]->get_real_img();
    }

    std::cout << "joining started" << std::endl;
    vips_arrayjoin(iarr, &full, num_parts * num_parts, "across", num_parts, NULL);
    std::cout << "joining ended" << std::endl;

    vips_image_write_to_file(full, ("folder2\\" + name + ".jpg").c_str(), NULL);
    //vips_image_write_to_file(full, ("folder2\\" + name + ".jpg").c_str(), NULL);
}

/*
    int n = images_grid.size();
    float factor = (vips_image_get_width(original)/(float)num_parts)/vips_image_get_width(original);
    int small_width = vips_image_get_width(original)/num_parts;
    int small_height = vips_image_get_height(original)/num_parts;
    VipsImage* full = vips_image_new_from_file("bg.png", "access", VIPS_ACCESS_SEQUENTIAL, NULL);
    VipsImage* iarr[n];
    VipsImage* kek;

    for (int i = 0; i < num_parts; i++) {
        for (int j = 0; j < num_parts; j++) {
            if((*memo)[images_grid[i * num_parts + j]->name] == 0) {
                (*memo)[images_grid[i * num_parts + j]->name] = *images_grid[i * num_parts + j]->get_img();
                vips_resize((*memo)[images_grid[i * num_parts + j]->name], &(*memo)[images_grid[i * num_parts + j]->name], factor, NULL);
                std::cout << "resized and memoed " << std::endl;
            }
            kek = (*memo)[images_grid[i * num_parts + j]->name];
            vips_insert(full, (*memo)[images_grid[i * num_parts + j]->name], &full, j * small_width, i * small_height, NULL);
        }
        //iarr[i] = images_grid[i]->get_real_img();
    }

    //vips_arrayjoin(iarr, &full, num_parts*num_parts, "across", num_parts, NULL);

    vips_image_write_to_file(full, ("folder2\\" + name).c_str(), NULL);


int n = images_grid.size();
float factor = (vips_image_get_width(original)/(float)num_parts)/vips_image_get_width(original);
int small_width = vips_image_get_width(original)/num_parts;
int small_height = vips_image_get_height(original)/num_parts;
VipsImage* iarr[n];

for (int i = 0; i < n; i++) {
    iarr[i] = images_grid[i]->get_real_img();
}

int x[num_parts * num_parts];
int y[num_parts * num_parts];
int blend[num_parts * num_parts];
for (int i = 0; i < num_parts; i++) {
    for (int j = 0; j < num_parts; j++) {
        x[i * num_parts + j] = small_width * j;
        y[i * num_parts + j] = small_height * i;
        blend[i * num_parts + j] = VIPS_BLEND_MODE_SOURCE;
    }
}

VipsArrayInt* xarr = vips_array_int_new(x, num_parts*num_parts);
VipsArrayInt* yarr = vips_array_int_new(y, num_parts*num_parts);

VipsImage* full;
vips_composite(iarr, &full, n, blend,
                "compositing_space", VIPS_INTERPRETATION_RGB, "x", xarr, "y", yarr, NULL);

for(int i=0; i<16; i++)
    std::cout << vips_image_get_width(iarr[i]);

vips_image_write_to_file(full, ("folder2\\" + name).c_str(), NULL);

g_object_unref(xarr);
g_object_unref(yarr);
*/

/*
void CompositeImageCimplementation::create_final(std::unordered_map<std::string, VipsImage*>* memo){
    VipsImage* row;
    VipsImage* full;

    VipsImage* img;

    float factor = (vips_image_get_width(original)/(float)num_parts)/vips_image_get_width(original);

    std::cout << "num parts" << num_parts << std::endl;

    for (int i = 0; i < num_parts; i++) {
        for (int j = 0; j < num_parts; j++) {
            if (j == 0) {
                //row = vips_image_new_from_file(images_grid[i*num_parts]->get_path().c_str(), NULL);
                if((*memo)[images_grid[i*num_parts]->name] == 0) {
                    (*memo)[images_grid[i*num_parts]->name] = *images_grid[i*num_parts]->get_img();
                    vips_resize((*memo)[images_grid[i*num_parts]->name], &(*memo)[images_grid[i*num_parts]->name], factor, NULL);
                }
                row = (*memo)[images_grid[i*num_parts]->name];
                //vips_image_write_to_file(*memo[images_grid[i*num_parts]->name], "testy.jpg", NULL);
                //vips_resize(row, &row, images_grid[i*num_parts + j]->get_factor_w(),
                //             "vscale", images_grid[i*num_parts + j]->get_factor_h(), NULL);
                continue;
            }

            if((*memo)[images_grid[i*num_parts + j]->name] == 0) {
                (*memo)[images_grid[i*num_parts + j]->name] = *images_grid[i*num_parts + j]->get_img();
                vips_resize((*memo)[images_grid[i*num_parts + j]->name], &(*memo)[images_grid[i*num_parts + j]->name], factor, NULL);
                std::cout << "im inside" << std::endl;
            }
            img = (*memo)[images_grid[i*num_parts + j]->name];

            vips_merge(row, img, &row, VIPS_DIRECTION_HORIZONTAL, -vips_image_get_width(row), 0, NULL);
            //vips_resize(img, &img, images_grid[i*num_parts + j]->get_factor_w(),
            //             "vscale", images_grid[i*num_parts + j]->get_factor_h(), NULL);

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

    vips_image_write_to_file(full, ("folder2\\" + name).c_str(), NULL);

}
*/

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
    //return (std::pow((c2.r-c1.r)*0.3, 2) + std::pow((c2.g-c1.g)*0.59, 2) + std::pow((c2.b-c1.b)*0.11, 2));
    //return (std::pow((c2.r-c1.r), 2) + std::pow((c2.g-c1.g), 2) + std::pow((c2.b-c1.b), 2));
    return (c2.r-c1.r) * (c2.r-c1.r) + (c2.g-c1.g) * (c2.g-c1.g) + (c2.b-c1.b) * (c2.b-c1.b);
}

color CompositeImageCimplementation::get_avg_color() {
    return average;
}

void CompositeImageCimplementation::load_into_mem() {
    original = vips_image_new_from_file(path_to_img.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, "memory", TRUE, NULL);
    if (vips_image_hasalpha(original)) {
        vips_flatten(original, &original, NULL);
    }
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
    VipsImage* lab_image;

    vips_colourspace(*image, &lab_image, VIPS_INTERPRETATION_LAB, NULL);

    vips_stats(lab_image, &stats, NULL);
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

VipsImage* CompositeImageCimplementation::get_real_img() {
    return original;
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
