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

class SDLGuimage
{
        public:

        SDLGuimage( int w, int h, int row, int col, int detail_thresh,
                    SDLTextureLoader* texloader, CompositeImage* starting_image,
                    SDL_Renderer* renderer);

        SDLGuimage( int w, int h, int row, int col, int detail_thresh,
                    SDLTextureLoader* texloader, CompositeImage* starting_image,
                    SDL_Renderer* renderer, SDLGuimage* parent);

        SDLGuimage(int w, int h, int detail_thresh, SDLTextureLoader* texloader,
                    CompositeImage* starting_image, SDL_Renderer* renderer);
        virtual ~SDLGuimage();

        void show();

        void change_zoom(float z);

        void change_cam_pos(float x, float y);

        void update_cam_bounds();

        void generate_image();

        void create_detailed();

        void increment_zoom(float zd);

        void set_local_transition_threshold(int thresh);

        bool should_be_drawn();

        void move_cam_pos_based_on_mouse(int cur_x, int cur_y, float delta_z);

        float calculate_small_x();
        float calculate_small_y();

        bool switch_to_parent();
        void make_active();

        void clear_next_images();

        void adjust_back_transition(float zd, int row, int col, float cam_x, float cam_y);

    protected:

    private:
        float zoom = 1;

        std::vector<SDLGuimage> next_images;
        bool next_image_exists=false;

        bool should_be_detailed;

        SDLGuimage* parent;

        int self_row;
        int self_col;

        float local_transition_zoom;

        // cam's pos
        float cam_x, cam_y;
        float cam_max_x, cam_max_y, cam_min_x, cam_min_y;

        int width, height;

        int detail_threshold;

        int num_rows_in_next;
        int num_cols_in_next;

        std::string window_name;

        bool off_screen = false;

        CompositeImage* composite_image;

        SDLTextureLoader* texture_loader;

        SDL_Renderer* renderer;
};

#endif // SDLGUIMAGE_H
