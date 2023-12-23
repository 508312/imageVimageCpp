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
        int get_mNumWidthidth();
        /** Returns how much images compose this image vertically **/
        int get_mNumHeighteight();

        /** Returns width resolution at which image was generated **/
        int get_width();
        /** Returns height resolution at which image was generated **/
        int get_height();

        /** Returns average color of original image. **/
        color get_avg_color();
        /** Returns average color of a crop from original image. **/
        color crop_avg_color(int left, int top, int width, int height);

        /** Returns image grid of this image **/
        std::vector<uint16_t>* get_grid();

        /** Pushes image index to last open space in the grid. **/
        void push_to_grid(uint16_t image);

        /** Gets color distance to another image. **/
        int get_distance_to_img(CompositeImage* img2);

        /** Computes average color of this image. **/
        void compute_avg();

        /** Loads original image to memory. **/
        void load_to_mem();

        /** Unloads original image from memory. **/
        void unload_from_mem();

        /** Fills the grid with empty indexes. **/
        void fill_grid_with_empty();

        /** Returns average color of the passed image **/
        static color image_average(cv::Mat* image);

        /** Gets disatnce between average color of the image and passed color. **/
        int get_distance_to_color(const color& clr);

        /** Returns image name **/
        std::string get_name();

        cv::Mat load_image();

        cv::Mat* get_image();

        void set_num_unique_images(int num);

        /** Returns original extension of the image. **/
        std::string get_extension();

        /** Returns pointer to image located at x,y position in the grid. **/
        CompositeImage* get_image_at(int x, int y);

        /** Returns image index as in ImageBuilder at the given x,y in the grid. **/
        uint16_t get_image_index_at(int x, int y);

        /** Changes selected image index in the grid. **/
        void set_image_at(int x, int y, uint16_t image);

        /** Returns image between two colors. **/
        static int distance(const color& c1, const color& c2);

        /** Return image index of this image. **/
        uint16_t get_ind();

        /** Coalesces images into blocks. **/
        void coalesce_blocks(int max_size);

    protected:

    private:
        /** Number of parts to divide the image horizontally. **/
        uint16_t mNumWidth;
        /** Number of parts to divide the image vertically. **/
        uint16_t mNumHeight;
        /** Resolution to load images at. **/
        uint16_t mWidth, mHeight;
        /** Number of unique images in the grid **/
        uint16_t num_unique_images = 0;
        /** Index of an image as in ImageBuilder **/
        uint16_t index;
        /** Mapping from indexes of image builder vector to pointers to images **/
        std::vector<CompositeImage*>* ind_img_map;

        /** Flag if the original image is loaded in this composite image **/
        bool is_loaded = false;

        /** Average color of this composite image. **/
        color average;

        /** Grid of images of which this image consists. **/
        std::vector<uint16_t> images_grid;
        /** Name of the image as in file it was loaded from. **/
        std::string name;
        /** Extension of the image as in file it was loaded from. **/
        std::string extension;
        /** Path to this image. **/
        std::string path;

        /** Original image is stored here if it is to be loaded. **/
        cv::Mat stored_image;
};

#endif // COMPOSITEIMAGE_H
