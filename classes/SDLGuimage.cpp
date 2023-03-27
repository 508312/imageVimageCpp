#include "SDLGuimage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include "ImageBuilder.h"
#include "Timer.h"
#include "string"

SDLGuimage::SDLGuimage( int w, int h, int row, int col, int detail_thresh,
                        SDLTextureLoader* texloader, CompositeImage* starting_image,
                         SDL_Renderer* renderer, SDLGuimage* parent) {
    //ctor
    width = w;
    height = h;
    self_row = 0;
    self_col = 0;

    change_cam_pos(width/2, height/2);

    window_name = "Display window";

    detail_threshold = detail_thresh;

    texture_loader = texloader;

    composite_image = starting_image;

    local_transition_zoom = width/calculate_small_x();

    this->renderer = renderer;
    this->parent = parent;

    self_row = row;
    self_col = col;
}


SDLGuimage::SDLGuimage( int w, int h, int row, int col, int detail_thresh,
                        SDLTextureLoader* texloader, CompositeImage* starting_image,
                         SDL_Renderer* renderer)
        : SDLGuimage(w, h, row, col, detail_thresh, texloader, starting_image, renderer, nullptr) {

}
SDLGuimage::SDLGuimage( int w, int h, int detail_thresh,
                        SDLTextureLoader* texloader,
                        CompositeImage* starting_image, SDL_Renderer* renderer)
        : SDLGuimage(w, h, 0, 0, detail_thresh, texloader, starting_image, renderer, nullptr) {
}


SDLGuimage::~SDLGuimage() {
    //dtor
}

bool SDLGuimage::should_be_drawn() {
    if (next_image_exists) {
        for (int i = 0; i < next_images.size(); i++) {
            if (next_images[i].should_be_drawn()) {
                return true;
            }
        }
        return false;
    }
    return !off_screen;
}

/** Threshold in width **/
void SDLGuimage::set_local_transition_threshold(int thresh_width) {
    local_transition_zoom = thresh_width/calculate_small_x();
}

float SDLGuimage::calculate_small_x() {
    return composite_image->get_width() / (float)composite_image->get_num_parts();
}

float SDLGuimage::calculate_small_y() {
    return composite_image->get_height() / (float)composite_image->get_num_parts();
}

void SDLGuimage::create_detailed() {
    float theoretical_x = calculate_small_x();
    float theoretical_y = calculate_small_y();

    int min_x_ind = std::max((int)(cam_min_x/theoretical_x),
                              0);
    int min_y_ind = std::max((int)(cam_min_y/theoretical_y),
                              0);
    int max_x_ind = std::min((int)std::ceil(cam_max_x/theoretical_x),
                              composite_image->get_num_parts());
    int max_y_ind = std::min((int)std::ceil(cam_max_y/theoretical_y),
                              composite_image->get_num_parts());

    int num_x = max_x_ind - min_x_ind;
    int num_y = max_y_ind - min_y_ind;

    CompositeImage* img;
    SDL_Texture* tex;
    SDL_Rect texrect = {0, 0, theoretical_x*zoom + 1, theoretical_y*zoom + 1}; // TODO:DELETE GRID ARTIFACTS
                                // tbh can be done by just adding 1 to w/h, not noticable and fixes grid bug

    for (int i = min_y_ind; i < max_y_ind; i++) {
        texrect.y = (i*theoretical_y - cam_min_y) * zoom;
        for (int j = min_x_ind; j < max_x_ind; j++) {
            img = composite_image->get_image_at(i, j);
            tex = texture_loader->get_texture(img, theoretical_x*zoom);
            texrect.x = (j*theoretical_x - cam_min_x) * zoom;
            SDL_RenderCopy(renderer, tex, NULL, &texrect);
        }
    }

    if (zoom > local_transition_zoom) {
        float new_zoom = (theoretical_x * zoom)/width;
        float new_x, new_y;

        for (int i = min_y_ind; i < max_y_ind; i++) {
            for (int j = min_x_ind; j < max_x_ind; j++) {
                img = composite_image->get_image_at(i, j);
                next_images.push_back(SDLGuimage(width, height, i, j, detail_threshold,
                                                  texture_loader, img, renderer, this));
                next_image_exists = true;

                new_x = (cam_x - j*theoretical_x) * local_transition_zoom;
                new_y = (cam_y - i*theoretical_y) * local_transition_zoom;

                next_images[next_images.size() - 1].change_cam_pos(new_x, new_y);
                next_images[next_images.size() - 1].change_zoom(new_zoom);
            }
        }
    }
}

void SDLGuimage::generate_image() {
    if(next_image_exists) {
        Timer t;
        t.start();
        std::vector<int> indexes_to_erase;
        for (int i = 0; i < next_images.size(); i++) {
            if (next_images[i].should_be_drawn())
                next_images[i].generate_image();
            else
                indexes_to_erase.push_back(i);
        }

        for (int i = 0; i < indexes_to_erase.size(); i++) {
            std::cout << "erased" << std::endl;
            next_images.erase(next_images.begin() + indexes_to_erase[i] - i);
        }
        return;
    }

    if (should_be_detailed) {
        create_detailed();
    } else {
        SDL_Texture* image = texture_loader->get_texture(composite_image, width * zoom);
        SDL_Rect rect{-cam_min_x * zoom, -cam_min_y * zoom, width * zoom, height * zoom};
        SDL_RenderCopy(renderer, image, NULL, &rect);
    }
}

/** Returns 1 if success or 0 if there is no parent **/
bool SDLGuimage::switch_to_parent() {
    if (parent != nullptr) {
        parent->make_active();
        return 1;
    }
    return 0;
}

void SDLGuimage::make_active() {
    clear_next_images();
    next_image_exists = false;
}

void SDLGuimage::clear_next_images() {
    if (next_image_exists) {
        for (int i=0; i < next_images.size(); i++) {
            next_images[i].clear_next_images();
        }
    }
    next_images.clear();
}

//Theoretical cam, becomes smaller, over the image
void SDLGuimage::update_cam_bounds(){
    float half_w = 0.5 * width / zoom;
    float half_h = 0.5 * height / zoom;

    cam_max_x = cam_x + half_w;
    cam_min_x = cam_x - half_w;
    cam_max_y = cam_y + half_h;
    cam_min_y = cam_y - half_h;
    if (cam_max_x < 0 || cam_min_x > width || cam_max_y < 0 || cam_min_y > height) {
        std::cout << " WENT OFF SCREEN " << self_row << " " << self_col << std::endl;
        off_screen = true;
    }
}

void SDLGuimage::move_cam_pos_based_on_mouse(int cur_x, int cur_y, float delta_z) {
    if (next_image_exists){
        for (int i = 0; i < next_images.size(); i++)
            next_images[i].move_cam_pos_based_on_mouse(cur_x, cur_y, delta_z);
        return;
    }

    float new_min_x = (cam_x - width/(2*zoom)) + cur_x/zoom - cur_x/(zoom*delta_z); // 0 + 800 - 400  400
    float new_min_y = (cam_y - height/(2*zoom)) + cur_y/zoom - cur_y/(zoom*delta_z);

    float new_x = (new_min_x + width / (zoom * delta_z * 2));
    float new_y = (new_min_y + height / (zoom * delta_z * 2));

    change_cam_pos(new_x, new_y);
}

void SDLGuimage::change_cam_pos(float x, float y) {
    cam_x = x;
    cam_y = y;

    update_cam_bounds();
}

void SDLGuimage::increment_zoom(float zd) {
    if (next_image_exists){
        for (int i = 0; i < next_images.size(); i++) {
            if (!next_image_exists) {
                return;
            }
            next_images[i].increment_zoom(zd);
        }
        return;
    }
    if (zoom * zd < 1) {
        if (switch_to_parent()) {
            parent->adjust_back_transition(zoom * zd, self_row, self_col, cam_x, cam_y);
        }
    }

    change_zoom(zoom * zd);
}

void SDLGuimage::adjust_back_transition(float z, int row, int col, float from_cam_x, float from_cam_y) {
    change_zoom(local_transition_zoom * z);
    change_cam_pos(col * calculate_small_x() + from_cam_x / local_transition_zoom,
                    row * calculate_small_y() + from_cam_y / local_transition_zoom);
}

void SDLGuimage::change_zoom(float z) {
    zoom = std::max(z, (float)-1000);

    if (zoom * width > detail_threshold) {
        should_be_detailed = true;
    } else {
        should_be_detailed = false;
    }
    update_cam_bounds();
}
