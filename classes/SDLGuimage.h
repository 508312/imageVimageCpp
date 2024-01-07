#ifndef SDLGUIMAGE_H
#define SDLGUIMAGE_H
#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "CompositeImage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"
#include "SDLTextureLoader.h"
#include <vector>
#include <stack>
#include <queue>
#include "StatsCounter.h"

/** Graphical image class. Works by having an imaginary camera on root Composite image of constant size.
    When zooming camera borders shrink and we determine what is seen by the user.
**/
class SDLGuimage
{
        public:

        SDLGuimage( int w, int h, int detail_thresh, int local_transition_thresh,
                    SDLTextureLoader* texloader, CompositeImage* starting_image,
                    SDL_Renderer* renderer, StatsCounter* stats_cntr);

        SDLGuimage( int w, int h, int row, int col, int detail_thresh, int local_transition_thresh,
                    SDLTextureLoader* texloader, CompositeImage* starting_image,
                    SDL_Renderer* renderer);

        SDLGuimage( int w, int h, int row, int col, int detail_thresh, int local_transition_thresh,
                    SDLTextureLoader* texloader, CompositeImage* starting_image,
                    SDL_Renderer* renderer, SDLGuimage* parent);

        SDLGuimage(int w, int h, int detail_thresh, int local_transition_thresh, SDLTextureLoader* texloader,
                    CompositeImage* starting_image, SDL_Renderer* renderer);

        SDLGuimage( int w, int h, int row, int col, int detail_thresh, int local_transition_thresh,
                        SDLTextureLoader* texloader, CompositeImage* starting_image,
                         SDL_Renderer* renderer, SDLGuimage* parent, StatsCounter* stats_cntr);

        virtual ~SDLGuimage();

        /** Returns center of camera on the x axis. **/
        float getCamX();
        /** Returns center of camera on the y axis. **/
        float getCamY();
        /** Returns width of the window. **/
        float getWidth();
        /** Returns height of the window. **/
        float getHeight();
        /** Returns the row of this image as in its row in parent's grid. **/
        int getRow();
        /** Returns the col of this image as in its col in parent's grid. **/
        int getCol();
        /** Changes zoom to the @z value. **/
        void changezoom(float z);
        /** Changes cam pos to @x and @y. **/
        void changeCamPos(double x, double y);

        /** Generates and renders image. **/
        void generateImage();

        /** Renders subdivided root image. **/
        void createDetailed();

        void createDetailedCoalesced();

        /** Increments zoom by zd. **/
        void incrementzoom(float zd);

        void setLocalTransitionThreshold(int thresh);

        /** Pans camera towards cursor **/
        void moveCamPosBasedOnMouse(double cur_x, double cur_y, float delta_z);

        /** Calculates width of subdivided image. **/
        float calculateSmallX();
        /** Calculates height of subdivided image. **/
        float calculateSmallY();

        /** Switches to parent. Returns 1 on success or 0 if there is no parent to switch to. **/
        bool switchToParent();

        /** Deletes the following images, in turn becomes last image(hence, active)**/
        void makeActive();

        /** Clears next images and images of children. **/
        void clearNextImages();

        int getMaxRow();
        int getMaxCol();

        /** Returns images being rendered. Leafs of the tree. Children at the bottom level. **/
        void getBottomLevel(std::vector<SDLGuimage*>& res_vec);

        /** Adds next images captured by cam bounds. **/
        void addNextImages();
        /** Adds child images from min indexes to max indexes. **/
        void addNextImages(int min_x_ind, int min_y_ind, int max_x_ind, int max_y_ind);

        /** Calculates which indexes are captured by camera (non inclusive). **/
        void calculateBoundIndexes(int& min_x_ind, int& min_y_ind, int& max_x_ind, int& max_y_ind);

        /** Returns child images. **/
        SDLGuimage* getNext(int row, int col);

        /** Adds missing images caused by unzooming **/
        // TODO: WIP FUNCTION
        void addMissing(int diff_row, int diff_col);

    protected:
        /** Updates camera bounds. Camera is placed on width x height image. **/
        void updateCamBounds();

        /** Aligns a little bit when zooming back. **/
        void adjustBackTransition(float zd, int row, int col, float cam_x, float cam_y);

        /** Returns if the image is on screen and should be drawn. **/
        bool shouldBeDrawn();

    private:
        /** Calculates factor by which images should be warped on x. **/
        float calculateWarpScaleX();
        /** Calculates factor by which images should be warped on y. **/
        float calculateWarpScaleY();

        /** Current zoom of an image.**/
        double mZoom = 1;

        /** List of children Guimages.**/
        std::vector<SDLGuimage> mNextImages;
        /** Flag if this image has children.**/
        bool mNextImageExists=false;

        /** Bool indicating if an image should be drawn from it's parts.**/
        bool mShouldBeDetailed;

        /** Pointer to a parent Guimage. **/
        SDLGuimage* mParent;

        /** Position on the grid of the parent composite image.**/
        int mRow, mCol;

        /** Threshold of a zoom for when to change to rendering children guimages instead of parent one. **/
        double mLocalTransitionZoom;

        /** Cam's center's position. **/
        double mCamX, mCamY;

        /** Cam's borders positions. **/
        float mCamMaxX, mCamMaxY, mCamMinX, mCamMinY;

        /** Windows width and height. Persistent across all childen. **/
        int mWidth, mHeight;

        /** Threshold for when to start rendering image from its parts.
            Doesn't have to be persistent, but I don't see cases when it's not. **/
        int mDetailThreshold;
        int mBlendThreshold;

        int mNumRowsInNext;
        int mNumColsInNext;

        /** Is the image off screen? **/
        bool mIsOffScreen = false;

        /** Composite image belonging to the current Guimage. **/
        CompositeImage* mCompositeImage;

        /** Dynamically loads and unloads images of correct resolution. Persistent across all children.**/
        SDLTextureLoader* mTextureLoader;

        /** SDL renderer to draw images on window. Persistent across all children. **/
        SDL_Renderer* mRenderer;

        /** Counts stats for how many images have been seen. Persistent across all children. **/
        StatsCounter* mStatsCounter;
};

#endif // SDLGUIMAGE_H
