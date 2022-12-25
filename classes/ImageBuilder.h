#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include"CompositeImage.h"
#include<vector>
#include<string>

class ImageBuilder {
    public:
        ImageBuilder();

        ImageBuilder(int parts);

        virtual ~ImageBuilder();

        void load_images(std::string path);

        void build_images();

        int get_num_images();

        std::vector<CompositeImage>* get_images();

        CompositeImage* find_closest_image(int index);

    protected:

    private:
        void build_image(int index);
        void load_image_original(std::string name);

        std::vector<CompositeImage> images;

        int num_parts;

};

#endif // IMAGEBUILDER_H
