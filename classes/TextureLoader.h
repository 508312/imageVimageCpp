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
        TextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, int load_threshold);
        TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts);
        TextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts, int load_threshold);
        virtual ~TextureLoader();

        /** Returns specified image at closest resolution available to width. **/
        TexType& get_texture(CompositeImage* image, int width);

        /** Returns texture at highest available resolution. **/
        TexType& get_full_texture(CompositeImage* image);

        /** Resizes data set to specified amount. **/
        void resize_to(int amnt);

        /** Frees all textures. **/
        virtual void free_textures();

    protected:
        /** Finds closest resolution index to specified width **/
        int find_closest_res(int width);

        /** Frees texture under specified res index and img index. **/
        virtual void free_texture(int res_index, int img_index) = 0;

        /** Frees texture above load threshold for specified index. **/
        virtual void free_texture_above_thresh(int img_index);

        /** First array is of resolution, second array is of image itself **/
        std::vector<std::vector<TexType>> mipmaps;

        /** LFU cache to keep track of last used images. **/
        LFUcache texture_cache{100};

    private:
};

#endif // TEXTURELOADER_H
