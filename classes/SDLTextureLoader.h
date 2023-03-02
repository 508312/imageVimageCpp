#ifndef SDLTEXTURELOADER_H
#define SDLTEXTURELOADER_H
#include "TextureLoader.h"
#include <SDL.h>
#include <SDL_image.h>

// WTF IS THIS LOL??? WHY DOESNT IT WORK WITHOUT IT???????? WHY IS IT NOT NEEDED IN OTHER FILES????????
// bruh what i think everything is linked alright
#include "TextureLoader.cpp"

class SDLTextureLoader : public TextureLoader<SDL_Texture*>
{
    public:
        SDLTextureLoader();
        SDLTextureLoader(std :: initializer_list <int> resoluts);
        SDLTextureLoader(std :: initializer_list <int> resoluts, SDL_Renderer* renderer);
        virtual ~SDLTextureLoader();

        void set_texture(CompositeImage* image, cv::Mat& pixels);

    protected:

    private:
        SDL_Renderer* renderer;
};

#endif // SDLTEXTURELOADER_H
