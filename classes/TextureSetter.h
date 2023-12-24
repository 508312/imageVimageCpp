#ifndef TEXTURESETTER_H
#define TEXTURESETTER_H
#include "CompositeImage.h"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "ImageBuilder.h"
#include "TextureStatus.h"

/** Base class to set textures. **/
class TextureSetter
{
    public:
        TextureSetter(ImageBuilder* img_bldr);
        TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts);
        TextureSetter(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, int mLoadThreshold);
        TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts);
        TextureSetter(ImageBuilder* img_bldr, std::vector<int>& resoluts, int mLoadThreshold);
        virtual ~TextureSetter();

        /** Sets textures for resolutions from start_ind to end_ind(non inclusive) **/
        virtual void setTexture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind) = 0;

        /** Sets texture for images lower than load thresold(in width).
            Index wise it will set all resolutions higher than the load thresold index. **/
        virtual void setBelowThreshold(CompositeImage* image);

        /** Sets texture for images above load thresold(in width).
            Index wise it will set all resolutions lower than the load thresold index. **/
        virtual void setAboveThreshold(CompositeImage* image);

        /** Resizes this texture loader to hold @amnt of images. **/
        virtual void resizeTo(int amnt) = 0;

    protected:
        std::vector<int> mResolution;
        std::vector<TextureStatus> mTextureStatuses;
        int mLoadThreshold;
        ImageBuilder* mImageBuilder;

        /** Sets texture for images lower than load thresold(in width).
            Index wise it will set all resolutions higher than the load thresold index. **/
        virtual void setBelowThreshold(CompositeImage* image, cv::Mat& pixels);
        /** Sets texture for images above load thresold(in width).
            Index wise it will set all resolutions lower than the load thresold index. **/
        virtual void setAboveThreshold(CompositeImage* image, cv::Mat& pixels);

    private:
};

#endif // TEXTURESETTER_H
