#ifndef COMPOSITEIMAGE_H
#define COMPOSITEIMAGE_H

#include <string>
#include <color.h>
#include <vips/vips8>

class CompositeImage {
    public:
        CompositeImage();

        CompositeImage(int parts, std::string path, int w, int h);

        virtual ~CompositeImage();

        void compose();

        int get_num_parts();

        color get_avg_color();
        color crop_avg_color(int left, int top, int width, int height);

        std::vector<CompositeImage*> get_grid();

        void push_to_grid(CompositeImage* image);

        float get_distance_to_img(CompositeImage* img2);

        void compute_avg();

        static color image_average(vips::VImage* image);

        float get_distance_to_color(color clr);

    protected:

    private:
        //TODO CIE-L*A*B* implementation in case color change is annoying
        static float distance(color c1, color c2);
        int num_parts;

        color average;


        //TODO MIGRATE TO FULL C++, on the time of writing the bindings or smth are fucked and half the functions don't work.
        std::vector<CompositeImage*> images_grid;
        std::string name;


        vips::VImage original;
        vips::VImage compiled;

};

#endif // COMPOSITEIMAGE_H
