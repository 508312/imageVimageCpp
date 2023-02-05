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

    local_transition_threshold = width;

    window_name = "Display window";

    detail_threshold = 1600;

    texture_loader = texloader;

    composite_image = starting_image;
}

Guimage::~Guimage() {
    //dtor
}

bool Guimage::should_be_drawn() {
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

void Guimage::set_local_transition_threshold(int thresh) {
    local_transition_threshold = thresh;
}

// SUPER MEH MEH CODE
// PASS RESIZED AS AN ARGUMENT
void Guimage::create_detailed(std::unordered_map<CompositeImage*, cv::Mat>& resized) {
    std::vector<CompositeImage*> to_concat;

    float small_img_size_x = composite_image->get_width() / (float)composite_image->get_num_parts();
    float small_img_size_y = composite_image->get_height() / (float)composite_image->get_num_parts();

    int min_x_ind = std::max((int)cam_min_x/(int)small_img_size_x,
                              0);
    int min_y_ind = std::max((int)cam_min_y/(int)small_img_size_y,
                              0);
    int max_x_ind = std::min((int)std::ceil(cam_max_x/small_img_size_x),
                              composite_image->get_num_parts());
    int max_y_ind = std::min((int)std::ceil(cam_max_y/small_img_size_y),
                              composite_image->get_num_parts());
    int num_x = max_x_ind - min_x_ind;
    int num_y = max_y_ind - min_y_ind;


    float off_x_start = cam_min_x - min_x_ind*small_img_size_x;
    float start_w = small_img_size_x - off_x_start;
    float off_y_start = cam_min_y - min_y_ind*small_img_size_y;
    float start_h = small_img_size_y - off_y_start;

    float end_w = (-(max_x_ind-1)*small_img_size_x + cam_max_x);
    float end_h = (-(max_y_ind-1)*small_img_size_y + cam_max_y);

    CompositeImage* img;
    Timer t;

    t.start();
    for (int i = min_y_ind; i < max_y_ind; i++) {
        for (int j = min_x_ind; j < max_x_ind; j++) {
            //std::cout << "I " << i << " J " << j << std::endl;
            img = composite_image->get_image_at(i, j);
            if (resized[img].empty()) {
                cv::resize(texture_loader->get_texture(img, small_img_size_x * zoom), resized[img],
                            cv::Size(small_img_size_x * zoom, small_img_size_y * zoom),
                           0, 0, cv::INTER_LINEAR_EXACT);
            }
            to_concat.push_back(img);
        }
    }
    //std::cout << "resizing " << t.get() << " " << small_img_size_x * zoom * num_x << " to_concat size " << to_concat.size() << std::endl;

    float error_x = small_img_size_x * zoom * num_x -
                        ImageBuilder::calculate_small_dim(small_img_size_x * zoom * num_x, num_y, 1) * num_x;

    float error_y = small_img_size_y * zoom * num_y -
                        ImageBuilder::calculate_small_dim(small_img_size_y * zoom * num_y, num_x, 1) * num_y;

    ImageBuilder::concat_all(num_y, num_x,
                            1, resized, &to_concat, image_to_render);

    /*
    std::cout << "offset " << off_x_start*zoom
      << " \tmin " << std::min((int) (image_to_render.cols - off_x_start*zoom), int(small_img_size_x * zoom * num_x - off_x_start*zoom + end_w*zoom))
      << " \tsmallimgENDW " << (small_img_size_x * num_x - off_x_start + end_w)* zoom << std::endl
      << " \tsmall_img_size_x " << small_img_size_x
      << " \toff_x_start " << off_x_start
      << " \tend_w " << end_w << std::endl
      << " \tzoom " << zoom
      << " \tcols-off " << image_to_render.cols - off_x_start*zoom << std::endl
      << " \tcols " << image_to_render.cols
      << " \toff_y_start*zoom " << off_y_start*zoom
      << " \tminHeight " << std::min((int) (image_to_render.rows - off_y_start*zoom), int(small_img_size_y * zoom * num_y - off_y_start*zoom + end_h*zoom))
      << " \theight " << image_to_render.cols << std::endl;
    */

    cv::Mat crop(image_to_render, cv::Rect(off_x_start*zoom, off_y_start*zoom,
                std::min((float) (image_to_render.cols - off_x_start*zoom),
                          (small_img_size_x * (num_x - 2) + start_w + end_w) * zoom),
                std::min((float) (image_to_render.rows - off_y_start*zoom),
                          (small_img_size_y * (num_y - 2) + start_h + end_h) * zoom)));
    image_to_render = crop;
    //std::cout << " width " << image_to_render.cols << " height " << image_to_render.rows << std::endl;

    //std::cout << "error " << error_x << std::endl;
    if (small_img_size_x * zoom >= local_transition_threshold ) {//&& error_x < 1) {
        int k = 0;

        num_cols_in_next = num_x;
        num_rows_in_next = num_y;

        float new_zoom = (small_img_size_x * zoom)/(width);

        for (int i = min_y_ind; i < max_y_ind; i++) {
            for (int j = min_x_ind; j < max_x_ind; j++) {
                    // USES IDEAL THEORETICAL WIDTH AND NOT THE ACTUAL ONE, FIX FIX FIX!!!!
                    int new_cam_x = cam_x - j * small_img_size_x;
                    int new_cam_y = cam_y - i * small_img_size_y;

                    //float new_width = std::min((float) (image_to_render.cols - off_x_start*zoom),
                    //      (small_img_size_x * (num_x - 2) + start_w + end_w) * zoom);

                    std::cout << "I " << i << " J " << j << " k " << k << std::endl;
                    std::cout << "NEW CAM X CAM Y " << new_cam_x*composite_image->get_num_parts()
                     << " " << new_cam_y*composite_image->get_num_parts() << " ZOOM " << new_zoom<< std::endl;

                    next_image_exists = true;
                    next_images.push_back(Guimage(texture_loader, to_concat[k]));
                    next_images[k].change_cam_pos(new_cam_x*composite_image->get_num_parts(),
                                                  new_cam_y*composite_image->get_num_parts());
                    next_images[k].change_zoom(new_zoom);
                    k++;
            }
        }
    }
}

void Guimage::generate_image() {
    std::unordered_map<CompositeImage*, cv::Mat> resized;
    generate_image(resized);
}

void Guimage::generate_image(std::unordered_map<CompositeImage*, cv::Mat>& resized) {
    if(next_image_exists) {

        std::vector<int> indexes_to_erase;

        /*   tl    t  X   tr
             l     X  X   r
             X     X  X   X
             bl    b  X   X */
        bool del_top, del_left, del_right, del_bot;

        int tr = num_cols_in_next - 1;
        int tl = 0;
        int t = std::min(1, num_cols_in_next - 1);
        int l = num_cols_in_next * std::min(1, num_rows_in_next - 1);
        int bl = num_cols_in_next * (num_rows_in_next - 1);
        int b = num_cols_in_next * (num_rows_in_next - 1) + std::min(1, num_cols_in_next - 1);
        int r = num_cols_in_next - 1 + num_cols_in_next * std::min(num_rows_in_next - 1, 1);

        //std::cout << "size " << next_images.size() << " t:" << t << " l:" << l << " b:" << b << " r:" << r << " br:" << br << " tl:" << tl << std::endl;

        del_top = !(next_images[tl].should_be_drawn()) && !(next_images[t].should_be_drawn());
        del_left = !(next_images[tl].should_be_drawn()) && !(next_images[l].should_be_drawn());
        del_bot = !(next_images[bl].should_be_drawn()) && !(next_images[b].should_be_drawn());
        del_right = !(next_images[tr].should_be_drawn()) && !(next_images[r].should_be_drawn());
        //std::cout << "dt " << del_top << " dl " << del_left << " dr " << del_right << " db " << del_bot << std::endl;
        for (int i = 0; i < next_images.size(); i++) {
            if (next_images[i].should_be_drawn())
                continue;
            indexes_to_erase.push_back(i);
        }

        // std::cout << "B4 " << num_cols_in_next << " " << num_rows_in_next << std::endl;

        num_cols_in_next = num_cols_in_next - del_left - del_right;
        num_rows_in_next = num_rows_in_next - del_bot - del_top;

        // std::cout << "AFT " << num_cols_in_next << " " << num_rows_in_next << std::endl;

        for (int i = 0; i < indexes_to_erase.size(); i++) {
            next_images.erase(next_images.begin() + indexes_to_erase[i] - i);
        }

        cv::Mat harr[num_cols_in_next];
        cv::Mat varr[num_rows_in_next];

        for (int i = 0; i < num_rows_in_next; i++) {
            for (int j = 0; j < num_cols_in_next; j++) {
                int ind = i * num_cols_in_next + j;
                int prev_x = std::max(int(i * num_cols_in_next), int(ind - 1));
                int prev_y = std::max(j, int(ind - 1 * num_cols_in_next));

                next_images[ind].generate_image(resized);

                /*

                std::cout << ind << " "
                 << next_images[ind].get_image().cols <<
                 next_images[ind].get_image().rows << std::endl;

                std::cout << prev_x << "<- prev x   prev y -> " << prev_y << std::endl;

                std::cout << "PREV COLS " << next_images[prev_y].get_image().cols <<
                    " PREV ROWS " << next_images[prev_x].get_image().rows << std::endl;

                */


                cv::resize(next_images[ind].get_image(), next_images[ind].get_image(),
                            cv::Size(next_images[prev_y].get_image().cols,
                                     next_images[prev_x].get_image().rows));

                harr[j] = next_images[ind].get_image();

                //std::cout << ind << " CURRENT COLS: " << harr[j].cols << " ROWS " << harr[j].rows << std::endl;
            }
            cv::hconcat(harr, num_cols_in_next, varr[i]);
        }
        cv::vconcat(varr, num_rows_in_next, image_to_render);
        //std::cout << "DRAWING GENERAL TOOK " << t.get() << " total in concat " << total << std::endl;
        return;
    }

    if (should_be_detailed) {
        Timer t;
        //t.start();
        create_detailed(resized);
        //std::cout << "drawing detailed took " << t.get() << std::endl;
    } else {
        cv::Mat image = texture_loader->get_full_texture(composite_image);
        int difference = image.rows / width;


        /*
        std::cout << "img " << image.cols << " y " << image.rows << std::endl;
        std::cout << "max_x " << cam_max_x << " min_x " << cam_min_x << std::endl;
        std::cout << "max_y " << cam_max_y << " min_y " << cam_min_y << std::endl;
        std::cout << "width " << width << " height " << height << std::endl;
        std::cout << "cam_x " << cam_x << " cam_y " << cam_y << std::endl;
        */

        cv::Mat crop(image, cv::Rect(cam_min_x * difference, cam_min_y * difference,
                                      (cam_max_x - cam_min_x) * difference, (cam_max_y - cam_min_y) * difference));

        cv::resize(crop, image_to_render,
                    cv::Size((cam_max_x - cam_min_x) * zoom,
                            (cam_max_y - cam_min_y) * zoom), 0, 0, cv::INTER_AREA);

        //cv::resize(crop, image_to_render, cv::Size(width, height));
    }
}

cv::Mat& Guimage::get_image() {
    return image_to_render;
}

void Guimage::update_cam_bounds(){
    float half_w = 0.5 * width / zoom;
    float half_h = 0.5 * height / zoom;

    cam_max_x = std::min(cam_x + half_w, (float) width);
    cam_min_x = std::max((float) 0, cam_x - half_w);
    cam_max_y = std::min(cam_y + half_h, (float) width);
    cam_min_y = std::max((float) 0, cam_y - half_h);

    //EEEH POGRESHNOST' V 1 PIXEL?????
    if (cam_max_x - cam_min_x < 1 || cam_max_y - cam_min_y < 1) {
        std::cout << "WENT OFF SCREEN" << std::endl;
        std::cout << "maX " << cam_max_x
         << " miX " << cam_min_x
         << " maY " << cam_max_y
         << " miY " << cam_min_y
        << std::endl;
        off_screen = true;
    }
}

void Guimage::move_cam_pos_based_on_mouse(int cur_x, int cur_y, float delta_z) {
    if (next_image_exists){
        for (int i = 0; i < next_images.size(); i++)
            next_images[i].move_cam_pos_based_on_mouse(cur_x, cur_y, delta_z);
        return;
    }
    std::cout << "new_cur_x " << cur_x/(zoom*delta_z) << " old x corrected " << cur_x/zoom << std::endl;
    std::cout << "old min x " << cam_x - width/(2*zoom) << " new min x "
    << (cam_x - width/(2*zoom)) + cur_x/zoom - cur_x/(zoom*delta_z) << std::endl;

    double new_min_x = (cam_x - width/(2*zoom)) + cur_x/zoom - cur_x/(zoom*delta_z); // 0 + 800 - 400  400
    double new_min_y = (cam_y - height/(2*zoom)) + cur_y/zoom - cur_y/(zoom*delta_z);

    float new_x = (new_min_x + width / (zoom * delta_z * 2));
    float new_y = (new_min_y + height / (zoom * delta_z * 2));

    std::cout << "X: " << new_x << " Y: " << new_y << std::endl;

    change_cam_pos(new_x, new_y);
}

void Guimage::change_cam_pos(int x, int y) {
    //TODO: edge cases + negative + lol func isn't even finished
    cam_x = x;
    cam_y = y;

    update_cam_bounds();
}

void Guimage::increment_zoom(float zd) {
    if (next_image_exists){
        for (int i = 0; i < next_images.size(); i++)
            next_images[i].increment_zoom(zd);
        return;
    }

    change_zoom(zoom * zd);
}

void Guimage::change_zoom(float z) {
    zoom = std::max(z, (float)-1000);
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
