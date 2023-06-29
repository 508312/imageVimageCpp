#ifndef SDLTEXTURELOADER_H
#define SDLTEXTURELOADER_H
#include "TextureLoader.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ImageBuilder.h"
#include "LFUcache.h"

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

        /** Sets textures for resolutions starting at start_ind and ending at end_ind (not inclusive). **/
        void set_texture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind);

        /** Loads set of images. **/
        void load_set(std::vector<CompositeImage*>& images);

        /** Frees texture specified by res_index and img_index. **/
        virtual void free_texture(int res_index, int img_index); // TODO: move to protected, solve error ?

    protected:

    private:
        /** Renderer to which to load textures to. **/
        SDL_Renderer* renderer;
};

#endif // SDLTEXTURELOADER_H
