#ifndef GUIMAGE_H
#define GUIMAGE_H
#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "CompositeImage.h"
#include "TextureLoader.h"
#include "CompositeImage.h"


class Guimage
{
    public:
        Guimage(TextureLoader* texloader, CompositeImage* starting_image);
        virtual ~Guimage();

        void show();

        void change_zoom(float z);

        void change_cam_pos(int x, int y);

        void update_cam_bounds();

        void generate_image();

        void draw_detailed();

    protected:

    private:
        float zoom;
        float inv_zoom;

        bool should_be_detailed;

        // cam's pos
        float cam_x, cam_y;
        int cam_max_x, cam_max_y, cam_min_x, cam_min_y;

        int width, height;

        int detail_threshold;

        cv::Rect bounding_frame;

        std::string window_name;

        cv::Mat image_to_render;

        CompositeImage* composite_image;

        TextureLoader* texture_loader;
};

#endif // GUIMAGE_H
