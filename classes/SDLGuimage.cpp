#include "SDLGuimage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include "ImageBuilder.h"
#include "Timer.h"
#include "string"
#include <stack>
#include <queue>

SDLGuimage::SDLGuimage( int w, int h, int row, int col, int detail_thresh, int local_transition_thresh,
                        SDLTextureLoader* texloader, CompositeImage* starting_image,
                         SDL_Renderer* renderer, SDLGuimage* parent, StatsCounter* stats_cntr) {
    //ctor
    width = w;
    height = h;
    self_row = 0;
    self_col = 0;
    zoom = 0;

    detail_threshold = detail_thresh;

    texture_loader = texloader;

    composite_image = starting_image;

    local_transition_zoom = composite_image->get_width()/calculate_small_x();

    this->renderer = renderer;
    this->parent = parent;
    this->stats_counter = stats_cntr;
    if (stats_cntr != nullptr) {
        this->stats_counter->add_seen_image(starting_image);
    }

    self_row = row;
    self_col = col;

    change_cam_pos(width/2, height/2);
}
SDLGuimage::SDLGuimage( int w, int h, int row, int col, int detail_thresh, int local_transition_thresh,
                        SDLTextureLoader* texloader, CompositeImage* starting_image,
                         SDL_Renderer* renderer, SDLGuimage* parent)
        : SDLGuimage(w, h, row, col, detail_thresh, local_transition_thresh, texloader, starting_image, renderer, parent, nullptr) {
}

SDLGuimage::SDLGuimage( int w, int h, int row, int col, int detail_thresh, int local_transition_thresh,
                        SDLTextureLoader* texloader, CompositeImage* starting_image,
                         SDL_Renderer* renderer)
        : SDLGuimage(w, h, row, col, detail_thresh, local_transition_thresh, texloader, starting_image, renderer, nullptr, nullptr) {

}
SDLGuimage::SDLGuimage( int w, int h, int detail_thresh, int local_transition_thresh,
                        SDLTextureLoader* texloader,
                        CompositeImage* starting_image, SDL_Renderer* renderer)
        : SDLGuimage(w, h, 0, 0, detail_thresh, local_transition_thresh, texloader, starting_image, renderer, nullptr, nullptr) {
}

SDLGuimage::SDLGuimage( int w, int h, int detail_thresh, int local_transition_thresh, SDLTextureLoader* texloader,
                        CompositeImage* starting_image, SDL_Renderer* renderer, StatsCounter* stats_cntr)
        : SDLGuimage(w, h, 0, 0, detail_thresh, local_transition_thresh, texloader, starting_image, renderer, nullptr, stats_cntr) {
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
    return composite_image->get_width() / (float)composite_image->get_num_parts_width();
}

float SDLGuimage::calculate_small_y() {
    return composite_image->get_height() / (float)composite_image->get_num_parts_height();
}

void SDLGuimage::create_detailed() {
    float theoretical_x = calculate_small_x();
    float theoretical_y = calculate_small_y();

    int min_x_ind, min_y_ind, max_x_ind, max_y_ind;

    calculate_bound_indexes(min_x_ind, min_y_ind, max_x_ind, max_y_ind);

    int num_x = max_x_ind - min_x_ind;
    int num_y = max_y_ind - min_y_ind;

    CompositeImage* img;
    SDL_Texture* tex;
    SDL_Rect texrect = {0, 0, std::round(theoretical_x*zoom) + 1, std::round(theoretical_y*zoom) + 1}; // TODO:DELETE GRID ARTIFACTS robust way
    SDL_Rect srcrect = {NULL, NULL, NULL, NULL};

    float x_start = 0;
    float x_end = 0;
    float y_start = 0;
    float y_end = 0;

    int tex_w;
    int tex_h;

    //Timer t;
    //int sum = 0;
    for (int i = min_y_ind; i < max_y_ind; i++) {
        texrect.y = std::max(0.0f, std::round((i*theoretical_y - cam_min_y) * zoom));
        texrect.h = (std::min((i+1)*theoretical_y, cam_max_y) - std::max(cam_min_y, i * theoretical_y)) * zoom;
        texrect.h += ((int)(texrect.y + texrect.h) != (int) std::max(0.0f, std::round(((i+1)*theoretical_y - cam_min_y) * zoom)));

        y_start = std::max((cam_min_y - i*theoretical_y) * zoom, 0.0f);
        y_end = std::min((cam_max_y - i*theoretical_y)*zoom, theoretical_y*zoom);

        for (int j = min_x_ind; j < max_x_ind; j++) {
            texrect.x = std::max(0.0f, std::round((j*theoretical_x - cam_min_x) * zoom));
            texrect.w = (std::min((j+1)*theoretical_x, cam_max_x) - std::max(cam_min_x, j * theoretical_x)) * zoom;
            texrect.w += ((int)(texrect.x + texrect.w) != (int) std::max(0.0f, std::round(((j+1)*theoretical_x - cam_min_x) * zoom)));

            x_start = std::max((cam_min_x - j*theoretical_x) * zoom, 0.0f);
            x_end = std::min((cam_max_x - j*theoretical_x)*zoom, theoretical_x*zoom);

            img = composite_image->get_image_at(i, j);
            //t.start();
            tex = texture_loader->get_texture(img, theoretical_x*zoom);
            //sum += t.get();

            SDL_QueryTexture(tex, NULL, NULL, &tex_w, &tex_h);

            float difference_x = tex_w/(theoretical_x*zoom);
            float difference_y = tex_h/(theoretical_y*zoom);

            srcrect.x = std::round(x_start * difference_x);
            srcrect.y = std::round(y_start * difference_y);
            srcrect.w = std::round((x_end - x_start) * difference_x);
            srcrect.h = std::round((y_end - y_start) * difference_y);

            //std::cout << " texres " << tex_w << " " << tex_h << std::endl;
            //std::cout << i << " " << j << " texrectx " << texrect.x << " " << texrect.y << " " << texrect.w << " " << texrect.h <<  std::endl;
            //std::cout << i << " " << j << " srcrect " << srcrect.x << " " << srcrect.y << " " << srcrect.w << " " << srcrect.h << std::endl;

            SDL_RenderCopy(renderer, tex, &srcrect, &texrect);

            //TODO: remove hardcode lol
            if (zoom > local_transition_zoom/8) {
                stats_counter->add_seen_image(img);
            }
        }
    }
    //std::cout << "GETTING TEXTURES TOOK " << sum << std::endl;

    if (zoom > local_transition_zoom) {
        add_next_images();
    }
}

void SDLGuimage::calculate_bound_indexes(int& min_x_ind, int& min_y_ind, int& max_x_ind, int& max_y_ind) {
    float theoretical_x = calculate_small_x();
    float theoretical_y = calculate_small_y();

    min_x_ind = std::max((int)(cam_min_x/theoretical_x),
                              0);
    min_y_ind = std::max((int)(cam_min_y/theoretical_y),
                              0);
    max_x_ind = std::min((int)std::ceil(cam_max_x/theoretical_x),
                              composite_image->get_num_parts_width());
    max_y_ind = std::min((int)std::ceil(cam_max_y/theoretical_y),
                              composite_image->get_num_parts_height());
}

void SDLGuimage::add_next_images() {
    int min_x_ind, min_y_ind, max_x_ind, max_y_ind;
    calculate_bound_indexes(min_x_ind, min_y_ind, max_x_ind, max_y_ind);

    add_next_images(min_x_ind, min_y_ind, max_x_ind, max_y_ind);
}

void SDLGuimage::add_next_images(int min_x_ind, int min_y_ind, int max_x_ind, int max_y_ind) {
    float theoretical_x = calculate_small_x();
    float theoretical_y = calculate_small_y();

    float new_zoom = (theoretical_x * zoom)/composite_image->get_width();
    double new_x, new_y;
    CompositeImage* img;

    for (int i = min_y_ind; i < max_y_ind; i++) {
        for (int j = min_x_ind; j < max_x_ind; j++) {
            std::cout << "ADDING " << i << " " << j << std::endl;
            img = composite_image->get_image_at(i, j);
            next_images.push_back(SDLGuimage(width, height, i, j, detail_threshold, local_transition_zoom * calculate_small_x(),
                                              texture_loader, img, renderer, this, stats_counter));
            next_image_exists = true;

            new_x = (cam_x - j*theoretical_x) * local_transition_zoom;
            new_y = (cam_y - i*theoretical_y) * local_transition_zoom;

            next_images[next_images.size() - 1].change_zoom(new_zoom);
            next_images[next_images.size() - 1].change_cam_pos(new_x, new_y);
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
            (next_images.begin() + indexes_to_erase[i] - i)->clear_next_images();
            next_images.erase(next_images.begin() + indexes_to_erase[i] - i);
        }
        return;
    }

    if (should_be_detailed) {
        create_detailed();
    } else {
        int img_w, img_h;
        float difference_x;
        float difference_y;
        SDL_Texture* image = texture_loader->get_texture(composite_image, composite_image->get_width() * zoom);
        SDL_QueryTexture(image, NULL, NULL, &img_w, &img_h);
        difference_x = (float) img_w / (float) composite_image->get_width();
        difference_y = (float) img_h / (float) composite_image->get_height();
        float real_w = std::min((float)composite_image->get_width(), cam_max_x) - std::max(0.0f, cam_min_x);
        float real_h = std::min((float)composite_image->get_height(), cam_max_y) - std::max(0.0f, cam_min_y);

        SDL_Rect rect{std::max(std::round(-cam_min_x * zoom), 0.0f),
                        std::max(std::round(-cam_min_y * zoom), 0.0f),
                        std::round(real_w * zoom), std::round(real_h * zoom)};
        SDL_Rect srcrect{std::round(cam_min_x * difference_x), std::round(cam_min_y * difference_y),
         std::round((cam_max_x - cam_min_x) * difference_x), std::round((cam_max_y - cam_min_y)  * difference_y)};
        SDL_RenderCopy(renderer, image, &srcrect, &rect);
    }
}


bool SDLGuimage::switch_to_parent() {
    if (parent != nullptr) {
        parent->make_active();
        return 1;
    }
    return 0;
}

void SDLGuimage::make_active() {
    clear_next_images();
}

void SDLGuimage::clear_next_images() {
    if (next_image_exists) {
        for (int i=0; i < next_images.size(); i++) {
            next_images[i].clear_next_images();
        }
    }
    next_images.clear();
    next_image_exists = false;
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

/** Moves camera based on position of cursor, as if on canvas. **/
void SDLGuimage::move_cam_pos_based_on_mouse(int cur_x, int cur_y, float delta_z) {
    if (next_image_exists){
        for (int i = 0; i < next_images.size(); i++)
            next_images[i].move_cam_pos_based_on_mouse(cur_x, cur_y, delta_z);
        return;
    }

    float new_min_x = (cam_x - width/(2*zoom)) + cur_x/zoom - cur_x/(zoom*delta_z); // 0 + 800 - 400  400
    float new_min_y = (cam_y - height/(2*zoom)) + cur_y/zoom - cur_y/(zoom*delta_z);

    double new_x = (new_min_x + width / (zoom * delta_z * 2));
    double new_y = (new_min_y + height / (zoom * delta_z * 2));

    change_cam_pos(new_x, new_y);

    // TODO: just b careful with this, put more thought.
    if (parent != nullptr) {
        parent->change_cam_pos(self_col * calculate_small_x() + cam_x / local_transition_zoom,
                self_row * calculate_small_y() + cam_y / local_transition_zoom);
    }
}

/** Changes cam position and updates bounds **/
void SDLGuimage::change_cam_pos(double x, double y) {
    cam_x = x;
    cam_y = y;

    update_cam_bounds();
}

void SDLGuimage::increment_zoom(float zd) {
    if (next_image_exists){
        int len_next = next_images.size();
        for (int i = 0; i < len_next; i++) {
            if (!next_image_exists) {
                return;
            }
            next_images[i].increment_zoom(zd);
        }
        return;
    }
    if (zoom * zd < (local_transition_zoom*calculate_small_x())/composite_image->get_width()) {
        if (switch_to_parent()) {
            parent->adjust_back_transition(zoom * zd, self_row, self_col, cam_x, cam_y);
        }
        return;
    }

    //std::cout << self_row << " " << self_col << "x " << cam_x << " y " << cam_y << std::endl;

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

int SDLGuimage::get_max_row() {
    return composite_image->get_num_parts_height();
}

int SDLGuimage::get_max_col() {
    return composite_image->get_num_parts_width();
}

SDLGuimage* SDLGuimage::get_next(int row, int col) {
    for (int i = 0; i < next_images.size(); i++) {
        if (next_images[i].self_row == row && next_images[i].self_col == col) {
            return &next_images[i];
        }
    }
    return nullptr;
}

void SDLGuimage::get_bottom_level(std::vector<SDLGuimage*>& res_arr) {
    if (next_images.size() == 0) {
        res_arr.push_back(this);
    } else {
        for (int i = 0; i < next_images.size(); i++) {
            next_images[i].get_bottom_level(res_arr);
        }
    }
}

float SDLGuimage::get_cam_x() {
    return cam_x;
}

float SDLGuimage::get_cam_y() {
    return cam_y;
}

float SDLGuimage::get_width() {
    return width;
}

float SDLGuimage::get_height() {
    return height;
}

int SDLGuimage::get_row() {
    return self_row;
}

int SDLGuimage::get_col() {
    return self_col;
}

int proper_mod(int x, int m) {
    return (x%m + m)%m;
}

void SDLGuimage::add_missing(int diff_row, int diff_col) {
    SDLGuimage* cur = parent;
    SDLGuimage* prev = nullptr;
    std::stack<SDLGuimage*> path;
    path.push(this);

    if (cur == nullptr) {
        return;
    }

    //check in parent first
    for (int i = 0; i < cur->next_images.size(); i++) {
        if((cur->next_images[i].self_row == self_row + diff_row) &&
           (cur->next_images[i].self_col == self_col + diff_col)) {
            return;
        }
    }

    while (cur->parent != nullptr &&
           ((cur->self_row + diff_row) < 0 || (cur->self_row + diff_row) >= cur->get_max_col() ||
           (cur->self_row + diff_col) < 0 || (cur->self_col + diff_col) >= cur->get_max_col())) {
        path.push(cur);
        prev = cur;
        cur = cur->parent;
    }

    int n_x;
    int n_y;
    SDLGuimage* cur_orig = cur;
    SDLGuimage* next;

    if (cur->parent == nullptr &&
        (cur->self_col + diff_col < 0 || cur->self_col + diff_col >= cur->get_max_col() ||
        cur->self_row + diff_row < 0 || cur->self_row + diff_row >= cur->get_max_row())) {
            return;
        }

    while (!path.empty()) {
        cur_orig = path.top();
        path.pop();
        n_x = proper_mod(cur_orig->self_col + diff_col, cur_orig->get_max_col());
        n_y = proper_mod(cur_orig->self_row + diff_row, cur_orig->get_max_row());

        next = cur->get_next(n_y, n_x);

        std::cout << "path" << path.size() << " N_x " << n_x << " n_y " << n_y << " zoom " << cur_orig->zoom << std::endl;

        if (next == nullptr) {
            cur->add_next_images(n_x, n_y, n_x + 1, n_y + 1);
            next = cur->get_next(n_y, n_x);
            next->change_zoom(cur_orig->zoom);
            next->change_cam_pos(cur_orig->cam_x - cur_orig->width * diff_col,
                                cur_orig->cam_y - cur_orig->height * diff_row);
        }

        cur = next;
        if (cur == nullptr) {
            std::cout << "YOU FUCKED UP MORON" << std::endl;
        }
    }
}
