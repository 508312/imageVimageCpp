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
    mAnchorX = 0;
    mAnchorY = 0;
    mAnchorW = 0;
    mAnchorH = 0;
    mAnchorRow = -1;
    mAnchorCol = -1;
}

ImageSyncer::~ImageSyncer()
{
    //dtor
}

void ImageSyncer::reset() {
    mAnchorX = 0;
    mAnchorY = 0;
    mAnchorW = 0;
    mAnchorH = 0;
    mAnchorRow = -1;
    mAnchorCol = -1;
}

void ImageSyncer::syncImage(SDLGuimage& img) {
    if (mAnchorW = 0) {
        mAnchorX = img.get_cam_x();
        mAnchorY = img.get_cam_y();
        mAnchorW = img.getWidth();
        mAnchorH = img.getHeight();
        mAnchorRow = img.get_row();
        mAnchorCol = img.get_col();
        mAnchorNumCols = img.get_max_col();
        mAnchorNumRows = img.get_max_row();
    } else {
        //img.change_cam_pos(mAnchorX + mAnchorW * calculate_difference(mAnchorCol, img->get_col()),
        //                    mAnchorY + mAnchorH * calculate_difference(mAnchorRow, img->get_row());
    }
}
