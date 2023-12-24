#ifndef IMAGESYNCER_H
#define IMAGESYNCER_H
#include "SDLGuimage.h"

/** Class to lineup images not to have growing pixel gaps when zooming in the same direction.
    Images to sync must have same original width and height.
    This class also keeps track of there being images needed to be filled in.
    This class may be redundant but I felt it would be easier to have it.
    **/
class ImageSyncer
{
    public:
        ImageSyncer();
        virtual ~ImageSyncer();

        /** Resets images to sync */
        void reset();

        /** Syncs image to the anchor image. First image passed after reset becomes anchor image. */
        void syncImage(SDLGuimage& img);

    protected:

    private:
        /** Anchor image x pos. **/
        int mAnchorX;
        /** Anchor image y pos. **/
        int mAnchorY;
        /** Anchor image width. 0 value indicates image has been reset. **/
        int mAnchorW;
        /** Anchor image height. 0 value indicates image has been reset. **/
        int mAnchorH;
        /** Anchor image row. -1 value indicates image has been reset. **/
        int mAnchorRow;
        /** Anchor image col. -1 value indicates image has been reset. **/
        int mAnchorCol;
        /** Num columns in anchor. -1 value indicates image has been reset. **/
        int mAnchorNumCols;
        /** Num columns in rows. -1 value indicates image has been reset. **/
        int mAnchorNumRows;
};

#endif // IMAGESYNCER_H
