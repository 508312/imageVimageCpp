#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include"CompositeImage.h"
#include<vector>
#include<string>
#include <unordered_map>
#include <TextureLoader.h>

// TODO: move from weird ind-passing system to something more robust.
class ImageBuilder {
    public:
        struct pos {
            int x;
            int y;
        };

        ImageBuilder();

        ImageBuilder(int parts, int w, int h, int fin_up, int prune, int closeness, TextureLoader* texl);

        virtual ~ImageBuilder();

        void load_images(std::string path);

        void build_images();

        void create_final(int ind);

        int get_num_images();

        static int calculate_small_dim(int dim, int parts, float upscale);

        static void concat_all(int rows, int cols, float final_upscale,
                                std::unordered_map<CompositeImage*, cv::Mat>& resized_images,
                                std::vector<CompositeImage*>* grid, cv::Mat& full);

        std::vector<CompositeImage>* get_images();

        void prune(int ind, std::unordered_map<CompositeImage*, std::vector<pos>> positions,
                        std::unordered_map<CompositeImage*, int>& amounts,
                        std::vector<CompositeImage*>* imgs_abv_thrsh);

        static void fill_table(int num_images, int small_width, int small_height, float final_upscale,
                                std::unordered_map<CompositeImage*, cv::Mat>* resized_images,
                                std::vector<CompositeImage*>* images);

    protected:

    private:
        /** Finds closest image to specified clr. Ignores image under ind.
        *  TODO: refactor, make ind a set probably, generally make better system
        */
        static CompositeImage* find_closest_image(int ind, color clr, std::vector<CompositeImage*>* imgs);
        void build_image(int ind);

        /// Memo table for resized images.
        std::unordered_map<CompositeImage*, cv::Mat> resized_images;

        /// List of all images.
        std::vector<CompositeImage> images;
        /** List of pointers to images.
         *  One problem may arise from original list being resized due to adding new image,
         *  This should be taken in account when scaling function to support compiling on the fly images.
         */
         std::vector<CompositeImage*> pointers_to_images;

        /// Number of parts to divide the image on.
        int num_parts;

        /// Width to resize original images to.
        int width;
        /// Height to resize original images to.
        int height;

        /// By how much to upscale @width and @height in the compiled image.
        float final_upscale;

        /*** Replace images whose amount is less than this threshold.
         With a value of 10 image still looks good, and quite fast.
        */
        int prune_threshold = 0;
        /***Threshold below which to just take previous image when compiling composition.
            Good value to use is 1. 2 is acceptable.
            For 4 the artifacts are visible and not worth the speedup.
         */
        int closeness_threshold = 0;

        /// Texture loader to which load textures after compiling.
        TextureLoader* tex_loader;

};

#endif // IMAGEBUILDER_H
