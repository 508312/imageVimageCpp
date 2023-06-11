#ifndef SDLTEXTURELOADER_H
#define SDLTEXTURELOADER_H
#include "TextureLoader.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ImageBuilder.h"

// WTF IS THIS LOL??? WHY DOESNT IT WORK WITHOUT IT???????? WHY IS IT NOT NEEDED IN OTHER FILES????????
// bruh what i think everything is linked alright
#include "TextureLoader.cpp"

class SDLTextureLoader : public TextureLoader<SDL_Texture*>
{
    public:
        SDLTextureLoader(ImageBuilder* img_bldr);
        SDLTextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, SDL_Renderer* renderer);
        SDLTextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, SDL_Renderer* renderer, int load_threshold);
        SDLTextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts, SDL_Renderer* renderer);
        SDLTextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts, SDL_Renderer* renderer, int load_threshold);
        virtual ~SDLTextureLoader();

        void set_texture(CompositeImage* image, cv::Mat& pixels);
        void set_texture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind);

        void load_set(std::vector<CompositeImage*>& images);

        virtual void free_texture(int res_index, int img_index);

    protected:

    private:
        SDL_Renderer* renderer;
};

#endif // SDLTEXTURELOADER_H
