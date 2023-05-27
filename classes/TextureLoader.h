#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <unordered_map>
#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "CompositeImage.h"
#include "TextureSetter.h"

template <class TexType>
class TextureLoader : public TextureSetter
{
    public:
        TextureLoader();
        TextureLoader(std::initializer_list<int> resoluts);
        TextureLoader(std::initializer_list<int> resoluts, int load_threshold);
        TextureLoader(std::vector<int>& resoluts);
        TextureLoader(std::vector<int>& resoluts, int load_threshold);
        virtual ~TextureLoader();

        TexType& get_texture(CompositeImage* image, int width);

        TexType& get_full_texture(CompositeImage* image);

        void resize_to(int amnt);

        virtual void free_textures();

    protected:
        int find_closest_res(int width);

        // first array is of resolution, second array is of image itself
        std::vector<std::vector<TexType>> mipmaps;

    private:
};

#endif // TEXTURELOADER_H
