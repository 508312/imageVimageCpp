#include "CompositeImage.h"
#include <cmath>
#include "color.h"
#include <iostream>
#include <vector>
#include <vips/vips8>

CompositeImage::CompositeImage() {

}

CompositeImage::CompositeImage(int parts, std::string path, int w, int h) {
    //std::string name = path.substr(path.find_last_of("\\") + 1, path.find_last_of(".") - path.find_last_of("\\") - 1);
    name = path.substr(path.find_last_of("\\") + 1, path.length() - path.find_last_of("\\") - 1);
    this->num_parts = parts;
    original = vips::VImage::new_from_file(path.c_str()); // TODO IS SEQ NEEDDED? vips::VImage::option()->set( "access", "sequential"));
    if (original.width() != w || original.height() != h) {
        float factor_width = w/original.width();
        float factor_height = h/original.height();
        original = original.resize(factor_width);// TODO UPDATE LIBRARY , vips::VImage::option()->set( "vscale", factor_height));
    }
    compute_avg();


}

CompositeImage::~CompositeImage() {
    //dtor
}


/* Returns distance of average colors between two images. */
float CompositeImage::get_distance_to_img(CompositeImage* img2) {
    return distance(get_avg_color(), img2->get_avg_color());
}

float CompositeImage::get_distance_to_color(color clr) {
    return distance(get_avg_color(), clr);
}

float CompositeImage::distance(color c1, color c2) {
    std::pow((c2.r-c1.r)*0.3, 2), 2 + std::pow((c2.g-c1.g)*0.59, 2) + std::pow((c2.b-c1.b)*0.11, 2);
}

color CompositeImage::get_avg_color() {
    return average;
}

color CompositeImage::crop_avg_color(int left, int top, int width, int height) {
    vips::VImage cropped = original.crop(left, top, width, height);

    return image_average(&cropped);
}

color CompositeImage::image_average(vips::VImage* image) {
    color c;
    vips::VImage stats = image->stats();

    std::cout << "got here lmao why so long" << std::endl;

    c.r = stats.getpoint(4, 1)[0];

    std::cout << "got here 3" << std::endl;
    c.g = stats.getpoint(4, 2)[0];
    c.b = stats.getpoint(4, 3)[0];

    return c;
}

void CompositeImage::push_to_grid(CompositeImage* image) {
    images_grid.push_back(image);
}

void CompositeImage::compute_avg() {
    average = image_average(&original);
}

int CompositeImage::get_num_parts() {
    return num_parts;
}
