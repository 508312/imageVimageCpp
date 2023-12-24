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
        mAnchorX = img.getCamX();
        mAnchorY = img.getCamY();
        mAnchorW = img.getWidth();
        mAnchorH = img.getHeight();
        mAnchorRow = img.getRow();
        mAnchorCol = img.getCol();
        mAnchorNumCols = img.getMaxCol();
        mAnchorNumRows = img.getMaxRow();
    } else {
        //img.changeCamPos(mAnchorX + mAnchorW * calculate_difference(mAnchorCol, img->getCol()),
        //                    mAnchorY + mAnchorH * calculate_difference(mAnchorRow, img->getRow());
    }
}
