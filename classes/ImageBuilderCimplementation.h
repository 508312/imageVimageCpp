#ifndef ImageBuilderCimplementation_H
#define ImageBuilderCimplementation_H

#include"CompositeImageCimplementation.h"
#include<vector>
#include<string>

class ImageBuilderCimplementation {
    public:
        ImageBuilderCimplementation();

        ImageBuilderCimplementation(int parts);

        virtual ~ImageBuilderCimplementation();

        void load_images(std::string path);

        void build_images();

        int get_num_images();

        std::vector<CompositeImageCimplementation>* get_images();

        CompositeImageCimplementation* find_closest_image(int ind, color clr);

    protected:

    private:
        void build_image(int ind, std::unordered_map<std::string, VipsImage*>* memo);
        void load_image_original(std::string name);

        std::vector<CompositeImageCimplementation> images;


        int num_parts;

        int width;
        int height;

};

#endif // ImageBuilderCimplementation_H
