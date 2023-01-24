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

        ImageBuilder(int parts, int w, int h, int fin_up, int prune, TextureLoader* texl);

        virtual ~ImageBuilder();

        void load_images(std::string path);

        void build_images();

        void create_final(int ind);

        int get_num_images();

        static int calculate_small_dim(int dim, int parts, float upscale);

        static void concat_all(int rows, int cols, int width, int height, float final_upscale,
                                std::unordered_map<CompositeImage*, cv::Mat>& resized_images,
                                std::vector<CompositeImage*>* grid, cv::Mat& full);

        std::vector<CompositeImage>* get_images();

        void prune(int ind, std::unordered_map<CompositeImage*, std::vector<pos>> positions,
                        std::unordered_map<CompositeImage*, int>& amounts,
                        std::vector<CompositeImage*>* imgs_abv_thrsh);

    protected:

    private:
        /* Finds closest image to specified clr. Ignores image under ind.
            TODO: refactor, make ind a set probably, generally make better system */
        static CompositeImage* find_closest_image(int ind, color clr, std::vector<CompositeImage*>* imgs);

        std::unordered_map<CompositeImage*, cv::Mat> resized_images;
        void build_image(int ind);

        std::vector<CompositeImage> images;
        std::vector<CompositeImage*> pointers_to_images;

        int num_parts;

        int width;
        int height;

        float final_upscale;

        int prune_threshold = 0;

        TextureLoader* tex_loader;

};

#endif // IMAGEBUILDER_H
