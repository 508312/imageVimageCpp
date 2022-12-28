#ifndef COMPOSITEIMAGECIMPLEMENTATION_H
#define COMPOSITEIMAGECIMPLEMENTATION_H

#include <string>
#include <color.h>
#include <vips/vips8>
#include <unordered_map>

class CompositeImageCimplementation {
    public:
        CompositeImageCimplementation();

        CompositeImageCimplementation(int parts, std::string path, int w, int h);

        virtual ~CompositeImageCimplementation();

        void compose();

        int get_num_parts();

        color get_avg_color();
        color crop_avg_color(int left, int top, int width, int height);

        std::vector<CompositeImageCimplementation*> get_grid();

        void create_final(std::unordered_map<std::string, VipsImage**> memo);

        void push_to_grid(CompositeImageCimplementation* image);

        float get_distance_to_img(CompositeImageCimplementation* img2);

        std::string get_path();

        void compute_avg();

        static color image_average(VipsImage** image);

        float get_distance_to_color(color clr);

        float get_factor_h();
        float get_factor_w();

        void load_into_mem();

        VipsImage** get_img();
        std::string name;

    protected:

    private:
        //TODO CIE-L*A*B* implementation in case color change is annoying
        static float distance(color c1, color c2);
        int num_parts;

        std::vector<CompositeImageCimplementation*> images_grid;

        std::string path_to_img;

        float factor_width = 1;
        float factor_height = 1;

        color average;


        //TODO MIGRATE TO FULL C++, on the time of writing the bindings or smth are fucked and half the functions don't work.



        VipsImage* original;
        VipsImage* compiled;

};

#endif // COMPOSITEIMAGE_H
