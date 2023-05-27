#ifndef COMPOSITEIMAGE_H
#define COMPOSITEIMAGE_H

#include <string>
#include <color.h>
#include <opencv2/core/core.hpp>

class CompositeImage {
    public:

        CompositeImage(int parts_w, int parts_h, std::string path,
                        int w, int h, uint16_t index, std::vector<CompositeImage*>* ind_maps);

        virtual ~CompositeImage();

        /** Returns how much images compose this image horizontally **/
        int get_num_parts_width();
        /** Returns how much images compose this image vertically **/
        int get_num_parts_height();

        /** Returns width resolution at which image was generated **/
        int get_width();
        /** Returns height resolution at which image was generated **/
        int get_height();

        color get_avg_color();
        color crop_avg_color(int left, int top, int width, int height);

        std::vector<uint16_t>* get_grid();

        void push_to_grid(uint16_t image);

        void change_grid(int x, int y, uint16_t image);

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
        uint16_t get_image_index_at(int x, int y);

        void set_image_at(int x, int y, uint16_t image);

        static int distance(const color& c1, const color& c2);

        uint16_t get_ind();

    protected:

    private:
        uint16_t num_parts_w;
        uint16_t num_parts_h;
        uint16_t width, height;
        uint16_t num_unique_images = 0;
        uint16_t index;
        std::vector<CompositeImage*>* ind_img_map;

        bool is_loaded = false;

        color average;

        std::vector<uint16_t> images_grid;
        std::string name;
        std::string extension;
        std::string path;

        cv::Mat stored_image;
};

#endif // COMPOSITEIMAGE_H
