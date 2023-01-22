#include "Guimage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include "ImageBuilder.h"
#include "Timer.h"

Guimage::Guimage(TextureLoader* texloader, CompositeImage* starting_image) {
    //ctor
    width = 1600;
    height = 1600;

    change_cam_pos(52, 52);

    window_name = "Display window";

    detail_threshold = 2000;

    bounding_frame = cv::Rect(0, 0, width, height);

    texture_loader = texloader;

    composite_image = starting_image;
}

Guimage::~Guimage() {
    //dtor
}

//make pretty later
void Guimage::draw_detailed() {
    std::unordered_map<CompositeImage*, cv::Mat> resized;
    std::vector<CompositeImage*> to_concat;

    int small_img_size_x = composite_image->get_width() / composite_image->get_num_parts();
    int small_img_size_y = composite_image->get_height() / composite_image->get_num_parts();

    int min_x_ind = cam_min_x/small_img_size_x;
    int min_y_ind = cam_min_y/small_img_size_y;
    int max_x_ind = std::ceil(cam_max_x/small_img_size_x) + 1;
    int max_y_ind = std::ceil(cam_max_y/small_img_size_y) + 1;

    int off_x_start = cam_min_x - min_x_ind*small_img_size_x;
    int start_w = small_img_size_x - (cam_min_x - min_x_ind*small_img_size_x);
    int off_y_start = cam_min_y - min_y_ind*small_img_size_y;
    int start_h = small_img_size_y - (cam_min_y - min_y_ind*small_img_size_y);

    int off_x_end = small_img_size_x - (max_x_ind*small_img_size_x - cam_max_x);
    int off_y_end = small_img_size_y - (max_y_ind*small_img_size_y - cam_max_y);

    for (int i = min_x_ind; i < max_x_ind; i++) {
        for (int j = min_y_ind; j < max_y_ind; j++) {
            to_concat.push_back(composite_image->get_image_at(i, j));
        }
    }

    ImageBuilder::concat_all(max_y_ind - min_y_ind, max_x_ind - min_x_ind,
                             small_img_size_x, small_img_size_y,
                             width/((float) small_img_size_x),
                             resized, &to_concat, image_to_render);

    //cv::Mat crop(image_to_render, cv::Rect(1));
    //image_to_render = crop;
}

void Guimage::generate_image(){
    if (should_be_detailed) {
        std::cout << "Should be detailed body " << std::endl;
        Timer t;
        t.start();
        for (int i = 0; i < 100; i++) {
            draw_detailed();
        }
        std::cout << "took " << t.get();
    } else {
        if (zoom >= 1) {

            cv::Mat image = texture_loader->get_full_texture(composite_image);
            cv::resize(image, image, cv::Size(width, height), 0, 0, cv::INTER_AREA); // TODO: LOOK PRETTY NOT FUCKING RESIZED

            cv::Mat crop(image, cv::Rect(cam_min_x, cam_min_y,
                                          width * inv_zoom, height * inv_zoom));
            cv::resize(crop, image_to_render, cv::Size(width, height));
        }

    }
}

void Guimage::update_cam_bounds(){
    int half_w = width * inv_zoom * 0.5;
    int half_h = height * inv_zoom * 0.5;

    cam_max_x = cam_x + half_w;
    cam_min_x = cam_x - half_w;
    cam_max_y = cam_y + half_h;
    cam_min_y = cam_y - half_h;
}

void Guimage::change_cam_pos(int x, int y) {
    //TODO: edge cases + negative + lol func isn't even finished
    cam_x = x;
    cam_y = y;

    update_cam_bounds();
}

void Guimage::change_zoom(float z) {
    zoom = z;
    inv_zoom = 1/z;
    if (z * width > detail_threshold) {
        should_be_detailed = true;
    } else {
        should_be_detailed = false;
    }
    update_cam_bounds();
}

void Guimage::show() {
    cv::imshow(window_name, image_to_render);
}
