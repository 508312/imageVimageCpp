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
        float get_cam_x();
        /** Returns center of camera on the y axis. **/
        float get_cam_y();
        /** Returns width of the window. **/
        float get_width();
        /** Returns height of the window. **/
        float get_height();
        /** Returns the row of this image as in its row in parent's grid. **/
        int get_row();
        /** Returns the col of this image as in its col in parent's grid. **/
        int get_col();
        /** Changes zoom to the @z value. **/
        void change_zoom(float z);
        /** Changes cam pos to @x and @y. **/
        void change_cam_pos(double x, double y);

        /** Generates and renders image. **/
        void generate_image();

        /** Renders subdivided root image. **/
        void create_detailed();

        /** Increments zoom by zd. **/
        void increment_zoom(float zd);

        void set_local_transition_threshold(int thresh);

        /** Pans camera towards cursor **/
        void move_cam_pos_based_on_mouse(int cur_x, int cur_y, float delta_z);

        /** Calculates width of subdivided image. **/
        float calculate_small_x();
        /** Calculates height of subdivided image. **/
        float calculate_small_y();

        /** Switches to parent. Returns 1 on success or 0 if there is no parent to switch to. **/
        bool switch_to_parent();

        /** Deletes the following images, in turn becomes last image(hence, active)**/
        void make_active();

        /** Clears next images and images of children. **/
        void clear_next_images();

        int get_max_row();
        int get_max_col();

        /** Returns images being rendered. Leafs of the tree. Children at the bottom level. **/
        void get_bottom_level(std::vector<SDLGuimage*>& res_vec);

        /** Adds next images captured by cam bounds. **/
        void add_next_images();
        /** Adds child images from min indexes to max indexes. **/
        void add_next_images(int min_x_ind, int min_y_ind, int max_x_ind, int max_y_ind);

        /** Calculates which indexes are captured by camera (non inclusive). **/
        void calculate_bound_indexes(int& min_x_ind, int& min_y_ind, int& max_x_ind, int& max_y_ind);

        /** Returns child images. **/
        SDLGuimage* get_next(int row, int col);

        /** Adds missing images caused by unzooming **/
        // TODO: WIP FUNCTION
        void add_missing(int diff_row, int diff_col);

    protected:
        /** Updates camera bounds. Camera is placed on width x height image. **/
        void update_cam_bounds();

        /** Aligns a little bit when zooming back. **/
        void adjust_back_transition(float zd, int row, int col, float cam_x, float cam_y);

        /** Returns if the image is on screen and should be drawn. **/
        bool should_be_drawn();

    private:
        /** Calculates factor by which images should be warped on x. **/
        float calculate_warp_scale_x();
        /** Calculates factor by which images should be warped on y. **/
        float calculate_warp_scale_y();

        /** Current zoom of an image.**/
        float zoom = 1;

        /** List of children Guimages.**/
        std::vector<SDLGuimage> next_images;
        /** Flag if this image has children.**/
        bool next_image_exists=false;

        /** Bool indicating if an image should be drawn from it's parts.**/
        bool should_be_detailed;

        /** Pointer to a parent Guimage. **/
        SDLGuimage* parent;

        /** Position on the grid of the parent composite image.**/
        int self_row, self_col;

        /** Threshold of a zoom for when to change to rendering children guimages instead of parent one. **/
        float local_transition_zoom;

        /** Cam's center's position. **/
        double cam_x, cam_y;

        /** Cam's borders positions. **/
        float cam_max_x, cam_max_y, cam_min_x, cam_min_y;

        /** Windows width and height. Persistent across all childen. **/
        int width, height;

        /** Threshold for when to start rendering image from its parts.
            Doesn't have to be persistent, but I don't see cases when it's not. **/
        int detail_threshold;

        int num_rows_in_next;
        int num_cols_in_next;

        /** Is the image off screen? **/
        bool off_screen = false;

        /** Composite image belonging to the current Guimage. **/
        CompositeImage* composite_image;

        /** Dynamically loads and unloads images of correct resolution. Persistent across all children.**/
        SDLTextureLoader* texture_loader;

        /** SDL renderer to draw images on window. Persistent across all children. **/
        SDL_Renderer* renderer;

        /** Counts stats for how many images have been seen. Persistent across all children. **/
        StatsCounter* stats_counter;
};

#endif // SDLGUIMAGE_H
