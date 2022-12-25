#ifndef COMPOSITEIMAGE_H
#define COMPOSITEIMAGE_H

#include <string>
#include <color.h>

class CompositeImage {
    public:
        CompositeImage();
        virtual ~CompositeImage();

        void compose();

        int get_num_parts();
        color get_avg_color();
        color get_crop_avg_color(int left, int top, int width, int height);

        void set_images_grid(CompositeImage* grid);

        float get_distance_to_img(CompositeImage* img2);


    protected:

    private:
        int num_parts;
        color average;

        std::string name;
        CompositeImage* images_grid[1000][1000];


};

#endif // COMPOSITEIMAGE_H
