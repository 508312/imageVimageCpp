#ifndef TEXTURESETTER_H
#define TEXTURESETTER_H
#include "CompositeImage.h"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"

class TextureSetter
{
    public:
        TextureSetter();
        TextureSetter(std::initializer_list<int> resoluts);
        TextureSetter(std::initializer_list<int> resoluts, int load_threshold);
        TextureSetter(std::vector<int>& resoluts);
        TextureSetter(std::vector<int>& resoluts, int load_threshold);
        virtual ~TextureSetter();

        virtual void set_texture(CompositeImage* image, cv::Mat& pixels) = 0;
        virtual void set_below_threshold(CompositeImage* image, cv::Mat& pixels) = 0;
        virtual void set_above_threshold(CompositeImage* image, cv::Mat& pixels) = 0;

        virtual void resize_to(int amnt) = 0;

    protected:
        std::vector<int> resolutions;
        int load_threshold;

    private:
};

#endif // TEXTURESETTER_H
