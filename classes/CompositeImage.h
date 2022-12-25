#ifndef COMPOSITEIMAGE_H
#define COMPOSITEIMAGE_H

#include <string>
#include <color.h>
#include <vips/vips8>

class CompositeImage {
    public:
        CompositeImage();

        CompositeImage(int parts, std::string path);

        virtual ~CompositeImage();

        void compose();

        int get_num_parts();

        color get_avg_color();
        color crop_avg_color(int left, int top, int width, int height);

        std::vector<CompositeImage*> get_grid();

        void push_to_grid(CompositeImage* image);

        float get_distance_to_img(CompositeImage* img2);

        void compute_avg();

        static color image_average(vips::VImage image);

    protected:

    private:
        //TODO CIE-L*A*B* implementation in case color change is annoying
        int num_parts;
        color average;
        std::string name;

        std::vector<CompositeImage*> images_grid;

        vips::VImage original;
        vips::VImage compiled;

};

#endif // COMPOSITEIMAGE_H
