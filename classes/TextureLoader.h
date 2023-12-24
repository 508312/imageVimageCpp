#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <unordered_map>
#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "CompositeImage.h"
#include "ImageBuilder.h"
#include "TextureSetter.h"
#include "LFUcache.h"

template <class TexType>
class TextureLoader : public TextureSetter
{
    public:
        TextureLoader(ImageBuilder* img_bldr);
        TextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts);
        TextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, int mLoadThreshold);
        TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts);
        TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts, int mLoadThreshold);
        virtual ~TextureLoader();

        /** Returns specified image at closest resolution available to width. **/
        TexType& getTexture(CompositeImage* image, int width);

        /** Returns texture at highest available resolution. **/
        TexType& getFullTexture(CompositeImage* image);

        /** Resizes data set to specified amount. **/
        void resizeTo(int amnt);

        /** Frees all textures. **/
        virtual void freeTextures();

    protected:
        /** Finds closest resolution index to specified width **/
        int findClosestRes(int width);

        /** Frees texture under specified res index and img index. **/
        virtual void freeTexture(int res_index, int img_index) = 0;

        /** Frees texture above load threshold for specified index. **/
        virtual void freeTextureAboveThreshold(int img_index);

        /** First array is of resolution, second array is of image itself **/
        std::vector<std::vector<TexType>> mMipmaps;

        /** LFU cache to keep track of last used images. **/
        LFUcache mTextureCache{100};

    private:
};

#endif // TEXTURELOADER_H
