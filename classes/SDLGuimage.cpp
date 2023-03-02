#include "SDLGuimage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include "ImageBuilder.h"
#include "Timer.h"
#include "string"

SDLGuimage::SDLGuimage(SDLTextureLoader* texloader, CompositeImage* starting_image, SDL_Renderer* renderer) {
    //ctor
    width = 1600;
    height = 1600;

    change_cam_pos(width/2, height/2);

    local_transition_threshold = width;

    window_name = "Display window";

    detail_threshold = 2000;

    texture_loader = texloader;

    composite_image = starting_image;

    this->renderer = renderer;
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

void SDLGuimage::set_local_transition_threshold(int thresh) {
    local_transition_threshold = thresh;
}

void SDLGuimage::create_detailed() {
    float theoretical_x = composite_image->get_width() / (float)composite_image->get_num_parts();
    float theoretical_y = composite_image->get_height() / (float)composite_image->get_num_parts();

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
    SDL_Rect texrect = {0, 0, theoretical_x*zoom, theoretical_y*zoom};

    for (int i = min_y_ind; i < max_y_ind; i++) {
        texrect.y = (i*theoretical_y - cam_min_y) * zoom;
        for (int j = min_x_ind; j < max_x_ind; j++) {
            img = composite_image->get_image_at(i, j);
            tex = texture_loader->get_texture(img, theoretical_x*zoom);
            texrect.x = (j*theoretical_x - cam_min_x) * zoom;
            SDL_RenderCopy(renderer, tex, NULL, &texrect);
        }
    }

    if (theoretical_x * zoom >= local_transition_threshold) {
        float transition_zoom = local_transition_threshold/theoretical_x;
        float new_zoom = (theoretical_x * zoom)/width;
        float new_x, new_y;

        for (int i = min_y_ind; i < max_y_ind; i++) {
            for (int j = min_x_ind; j < max_x_ind; j++) {
                img = composite_image->get_image_at(i, j);
                next_images.push_back(SDLGuimage(texture_loader, img, renderer));
                next_image_exists = true;

                new_x = (cam_x - j*theoretical_x) * transition_zoom;
                new_y = (cam_y - i*theoretical_y) * transition_zoom;

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

//Theoretical cam, becomes smaller, over the image
void SDLGuimage::update_cam_bounds(){
    float half_w = 0.5 * width / zoom;
    float half_h = 0.5 * height / zoom;

    cam_max_x = cam_x + half_w;
    cam_min_x = cam_x - half_w;
    cam_max_y = cam_y + half_h;
    cam_min_y = cam_y - half_h;
    if (cam_max_x < 0 || cam_min_x > width || cam_max_y < 0 || cam_min_y > height) {
        std::cout << " WENT OFF SCREEN " << std::endl;
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
    //TODO: edge cases + negative + lol func isn't even finished
    cam_x = x;
    cam_y = y;

    update_cam_bounds();
}

void SDLGuimage::increment_zoom(float zd) {
    if (next_image_exists){
        for (int i = 0; i < next_images.size(); i++)
            next_images[i].increment_zoom(zd);
        return;
    }

    change_zoom(zoom * zd);
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
