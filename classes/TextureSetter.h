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
        TextureSetter(std :: initializer_list <int> resoluts);
        virtual ~TextureSetter();

        virtual void set_texture(CompositeImage* image, cv::Mat& pixels) = 0;

    protected:
        std::vector<int> resolutions;

    private:
};

#endif // TEXTURESETTER_H
