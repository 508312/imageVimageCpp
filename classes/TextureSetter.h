#ifndef TEXTURESETTER_H
#define TEXTURESETTER_H
#include "CompositeImage.h"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "ImageBuilder.h"
#include "TextureStatus.h"

class TextureSetter
{
    public:
        TextureSetter(ImageBuilder* img_bldr);
        TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts);
        TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, int load_threshold);
        TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts);
        TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts, int load_threshold);
        virtual ~TextureSetter();

        virtual void set_texture(CompositeImage* image, cv::Mat& pixels) = 0;
        virtual void set_texture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind) = 0;
        virtual void set_below_threshold(CompositeImage* image, cv::Mat& pixels);
        virtual void set_above_threshold(CompositeImage* image, cv::Mat& pixels);
        virtual void set_below_threshold(CompositeImage* image);
        virtual void set_above_threshold(CompositeImage* image);

        virtual void resize_to(int amnt) = 0;

    protected:
        std::vector<int> resolutions;
        std::vector<TextureStatus> texture_statuses;
        int load_threshold;
        ImageBuilder* image_builder;

    private:
};

#endif // TEXTURESETTER_H
