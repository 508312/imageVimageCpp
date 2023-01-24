#include "Guimage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include "ImageBuilder.h"
#include "Timer.h"
#include "string"

Guimage::Guimage(TextureLoader* texloader, CompositeImage* starting_image) {
    //ctor
    width = 1600;
    height = 1600;

    change_cam_pos(width/2, height/2);

    window_name = "Display window";

    detail_threshold = 5000;

    bounding_frame = cv::Rect(0, 0, width, height);

    texture_loader = texloader;

    composite_image = starting_image;
}

Guimage::~Guimage() {
    //dtor
}

// SUPER MEH MEH CODE
void Guimage::draw_detailed() {
    std::unordered_map<CompositeImage*, cv::Mat> resized;
    std::vector<CompositeImage*> to_concat;

    float small_img_size_x = composite_image->get_width() / (float)composite_image->get_num_parts();
    float small_img_size_y = composite_image->get_height() / (float)composite_image->get_num_parts();

    int min_x_ind = std::max((int)cam_min_x/(int)small_img_size_x,
                              0);
    int min_y_ind = std::max((int)cam_min_y/(int)small_img_size_y,
                              0);
    int max_x_ind = std::min((int)std::ceil(cam_max_x/small_img_size_x) + 1,
                              composite_image->get_num_parts());
    int max_y_ind = std::min((int)std::ceil(cam_max_y/small_img_size_y) + 1,
                              composite_image->get_num_parts());
    int num_x = max_x_ind - min_x_ind;
    int num_y = max_y_ind - min_y_ind;


    float off_x_start = cam_min_x - min_x_ind*small_img_size_x;
    int start_w = small_img_size_x - (cam_min_x - min_x_ind*small_img_size_x);
    float off_y_start = cam_min_y - min_y_ind*small_img_size_y;
    int start_h = small_img_size_y - (cam_min_y - min_y_ind*small_img_size_y);

    int end_w = small_img_size_x - (max_x_ind*small_img_size_x - cam_max_x);
    int end_h = small_img_size_y - (max_y_ind*small_img_size_y - cam_max_y);

    CompositeImage* img;
    Timer t;

    t.start();
    for (int i = min_x_ind; i < max_x_ind; i++) {
        for (int j = min_y_ind; j < max_y_ind; j++) {
            CompositeImage* img = composite_image->get_image_at(i, j);
            if (resized[img].empty()) {
                /*
                cv::resize(texture_loader->get_full_texture(img), resized[img],
                            cv::Size((texture_loader->get_full_texture(img).cols
                                      - small_img_size_x * zoom)/5,
                                      (texture_loader->get_full_texture(img).rows
                                      - small_img_size_y * zoom)/5));
                */
                cv::resize(texture_loader->get_full_texture(img), resized[img],
                            cv::Size(small_img_size_x * zoom, small_img_size_y * zoom),
                           0, 0, cv::INTER_NEAREST);
            }
            to_concat.push_back(img);
        }
    }
    std::cout << "resizing " << t.get() << " " << resized.size() << std::endl;
    ImageBuilder::concat_all(num_y, num_x,
                            small_img_size_x * zoom * num_x,
                            small_img_size_y * zoom * num_y,
                            1, resized, &to_concat, image_to_render);

    std::cout << "b4 crop " << off_x_start*zoom << " " << off_y_start*zoom
     << " " << image_to_render.rows << " " << image_to_render.cols << std::endl;
    cv::Mat crop(image_to_render, cv::Rect(off_x_start*zoom, off_y_start*zoom,
                                            std::min(width, int(small_img_size_x * zoom * num_x - off_x_start*zoom)),
                                            std::min(height, int(small_img_size_y * zoom * num_y - off_y_start*zoom))));
    image_to_render = crop;
    cv::resize(image_to_render, image_to_render, cv::Size(width, height));
}

void Guimage::generate_image(){
    if (should_be_detailed) {
        Timer t;
        t.start();
        draw_detailed();
        std::cout << "drawing detailed took " << t.get() << std::endl;
    } else {
        if (zoom >= 1) {

            cv::Mat image = texture_loader->get_full_texture(composite_image);
            int difference = image.rows / width;

            cv::Mat crop(image, cv::Rect(cam_min_x * difference, cam_min_y * difference,
                                          width * inv_zoom * difference, height * inv_zoom * difference));

            cv::resize(crop, image_to_render, cv::Size(width, height), 0, 0, cv::INTER_AREA); // TODO: LOOK PRETTY NOT FUCKING RESIZED

            //cv::resize(crop, image_to_render, cv::Size(width, height));
        }

    }
}

void Guimage::update_cam_bounds(){
    float half_w = width * inv_zoom * 0.5;
    float half_h = height * inv_zoom * 0.5;

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

void Guimage::increment_zoom(float zd) {
    change_zoom(zoom * zd);
}

void Guimage::change_zoom(float z) {
    zoom = std::max(z, (float)1);
    inv_zoom = 1/zoom;
    if (zoom * width > detail_threshold) {
        should_be_detailed = true;
    } else {
        should_be_detailed = false;
    }
    update_cam_bounds();
}

void Guimage::show() {
    cv::imshow(window_name, image_to_render);
}
