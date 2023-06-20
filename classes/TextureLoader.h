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

        TexType& get_texture(CompositeImage* image, int width);

        TexType& get_full_texture(CompositeImage* image);

        void resize_to(int amnt);

        virtual void free_textures();

    protected:
        int find_closest_res(int width);

        virtual void free_texture(int res_index, int img_index) = 0;
        virtual void free_texture_above_thresh(int img_index);

        // first array is of resolution, second array is of image itself
        std::vector<std::vector<TexType>> mipmaps;

        LFUcache texture_cache{100};

    private:
};

#endif // TEXTURELOADER_H
