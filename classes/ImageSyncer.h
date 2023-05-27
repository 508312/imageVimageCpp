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
        void sync_image(SDLGuimage& img);

    protected:

    private:
        /** Anchor image x pos. **/
        int p_anchor_x;
        /** Anchor image y pos. **/
        int p_anchor_y;
        /** Anchor image width. 0 value indicates image has been reset. **/
        int p_anchor_w;
        /** Anchor image height. 0 value indicates image has been reset. **/
        int p_anchor_h;
        /** Anchor image row. -1 value indicates image has been reset. **/
        int p_anchor_row;
        /** Anchor image col. -1 value indicates image has been reset. **/
        int p_anchor_col;
        /** Num columns in anchor. -1 value indicates image has been reset. **/
        int p_anchor_num_cols;
        /** Num columns in rows. -1 value indicates image has been reset. **/
        int p_anchor_num_rows;
};

#endif // IMAGESYNCER_H
