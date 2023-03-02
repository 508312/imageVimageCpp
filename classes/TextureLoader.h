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
        TextureLoader(std :: initializer_list <int> resoluts);
        virtual ~TextureLoader();

        TexType& get_texture(CompositeImage* image, int width);

        TexType& get_full_texture(CompositeImage* image);

    protected:
        int find_closest_res(int width);

        // arr of tables
        std::vector<std::unordered_map<std::string, TexType>> mipmaps;

    private:
};

#endif // TEXTURELOADER_H
