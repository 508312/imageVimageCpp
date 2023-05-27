#include "ImageSyncer.h"

int calculate_difference(int from, int to, int bound) {
    if (to - from > bound/2) {
        return -(from + bound - to);
    } if (to - from < -bound/2) {
        return bound + to - from;
    }
    return to - from;
}


ImageSyncer::ImageSyncer()
{
    p_anchor_x = 0;
    p_anchor_y = 0;
    p_anchor_w = 0;
    p_anchor_h = 0;
    p_anchor_row = -1;
    p_anchor_col = -1;
}

ImageSyncer::~ImageSyncer()
{
    //dtor
}

void ImageSyncer::reset() {
    p_anchor_x = 0;
    p_anchor_y = 0;
    p_anchor_w = 0;
    p_anchor_h = 0;
    p_anchor_row = -1;
    p_anchor_col = -1;
}

void ImageSyncer::sync_image(SDLGuimage& img) {
    if (p_anchor_w = 0) {
        p_anchor_x = img.get_cam_x();
        p_anchor_y = img.get_cam_y();
        p_anchor_w = img.get_width();
        p_anchor_h = img.get_height();
        p_anchor_row = img.get_row();
        p_anchor_col = img.get_col();
        p_anchor_num_cols = img.get_max_col();
        p_anchor_num_rows = img.get_max_row();
    } else {
        //img.change_cam_pos(p_anchor_x + p_anchor_w * calculate_difference(p_anchor_col, img->get_col()),
        //                    p_anchor_y + p_anchor_h * calculate_difference(p_anchor_row, img->get_row());
    }
}
