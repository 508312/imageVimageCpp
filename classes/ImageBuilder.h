#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include"CompositeImage.h"
#include<vector>
#include<string>
#include <unordered_map>
#include <variant>
#include "StatsCounter.h"

// TODO: move from weird ind-passing system to something more robust.
class ImageBuilder {
    public:
        struct pos {
            int x;
            int y;
        };

        ImageBuilder();

        ImageBuilder(int parts_w, int parts_h, int w, int h, int fin_up, int prune, int closeness);
        virtual ~ImageBuilder();

        /** Loads images from specified path. **/
        void load_images(std::string path);

        /** Subdivides all images. **/
        void build_images();

        /** Creates image from subdivided grid and outputs result in concatted_image. **/
        void create_final(int ind, cv::Mat& concatted_image);

        /** Returns number of images. **/
        int get_num_images();

        /** Calculates the size of a small image. **/
        static int calculate_small_dim(int dim, int parts, float upscale);

        /** Concats the grid using resized_images as the memo table. Outputs result in @full **/
        static void concat_all(int rows, int cols, float final_upscale,
                                std::vector<cv::Mat>& resized_images,
                                std::vector<uint16_t>* grid, cv::Mat& full);

        /** Returns vector with original images. **/
        std::vector<CompositeImage>* get_images();
        /** Returns vector of pointers to images. **/
        std::vector<CompositeImage*>& get_pointers_to_images();

        /** Fills memo table resized_images. **/
        static void fill_table(int num_images, int small_width, int small_height, float final_upscale,
                                std::vector<cv::Mat>& resized_images,
                                std::vector<CompositeImage*>& images_to_resize);

    protected:

    private:
        /** Prunes specified image. **/
        void prune(int ind, std::vector<std::vector<pos>> positions,
                        std::vector<int>& amounts,
                        std::vector<CompositeImage*>* imgs_abv_thrsh);

        /** Finds closest image to specified clr. Can ignore image under ind.
        *  TODO: refactor, make ind a set probably, make the Included_images vector or smth like that.
        */
        static int find_closest_image(int ind, color& clr, std::vector<CompositeImage*>* imgs);
        void build_image(int ind);

        /// Memo table for resized images.
        std::vector<cv::Mat> resized_images;

        std::vector<cv::Mat> resized_big;

        /// List of all images.
        std::vector<CompositeImage> images;
        /** List of pointers to images.
         *  Pointer to images[i] lies in pointers_to_images[i];
         *  One problem may arise from original list being resized due to adding new image,
         *  This should be taken in account when scaling function to support compiling on the fly images.
         */
         std::vector<CompositeImage*> pointers_to_images;

        /// Number of parts to divide the image on horizontally.
        int mNumWidth;
        /// Number of parts to divide the image on vertically.
        int mNumHeight;

        /// Width to resize original images to.
        int width;
        /// Height to resize original images to.
        int height;

        /// By how much to upscale @width and @height in the compiled image.
        float final_upscale;

        /** Replace images whose amount is less than this threshold.
            With a value of 10 image still looks good, and quite fast.
        */
        int prune_threshold = 0;

        /** Threshold below which to just take previous image when compiling composition.
            Good value to use is 1. 2 is acceptable.
            For 4 the artifacts are visible and not worth the speedup.
         */
        int closeness_threshold = 0;
};

#endif // IMAGEBUILDER_H
