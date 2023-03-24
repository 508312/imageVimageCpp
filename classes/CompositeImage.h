#ifndef COMPOSITEIMAGE_H
#define COMPOSITEIMAGE_H

#include <string>
#include <color.h>
#include <opencv2/core/core.hpp>

class CompositeImage {
    public:
        CompositeImage();

        CompositeImage(int parts, std::string path, int w, int h);

        virtual ~CompositeImage();

        void compose();

        int get_num_parts();

        int get_width();
        int get_height();

        color get_avg_color();
        color crop_avg_color(int left, int top, int width, int height);

        std::vector<CompositeImage*>* get_grid();

        void push_to_grid(CompositeImage* image);

        void change_grid(int x, int y, CompositeImage* image);

        int get_distance_to_img(CompositeImage* img2);

        void compute_avg();

        void load_to_mem();

        void unload_from_mem();

        void fill_grid_with_empty();

        static color image_average(cv::Mat* image);

        int get_distance_to_color(const color& clr);

        std::string get_name();

        cv::Mat load_image();

        cv::Mat* get_image();

        void set_num_unique_images(int num);

        std::string get_extension();

        CompositeImage* get_image_at(int x, int y);

        void set_image_at(int x, int y, CompositeImage* image);

        static int distance(const color& c1, const color& c2);

    protected:

    private:
        int num_parts;

        bool is_loaded = false;

        color average;

        int num_unique_images = 0;

        int width, height;

        std::vector<CompositeImage*> images_grid;
        std::string name;
        std::string extension;
        std::string path;

        cv::Mat stored_image;
};

#endif // COMPOSITEIMAGE_H
