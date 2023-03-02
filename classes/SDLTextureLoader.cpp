#include "SDLTextureLoader.h"

SDLTextureLoader::SDLTextureLoader() : TextureLoader()
{
}

SDLTextureLoader::SDLTextureLoader(std :: initializer_list <int> resoluts) : TextureLoader(resoluts)
{
    std::cout << "YOU ARE CREATING TEXTURE WITHOUT SETTING RENDERER FIRST, EXPECT ERRORS" << std::endl;
}

SDLTextureLoader::SDLTextureLoader(std :: initializer_list <int> resoluts, SDL_Renderer* renderer) : TextureLoader(resoluts)
{
    this->renderer = renderer;
}

SDLTextureLoader::~SDLTextureLoader()
{
    //TODO:FREE ALL TEXTURES LMAO
}

void SDLTextureLoader::set_texture(CompositeImage* image, cv::Mat& pixels) {
    for (int i = 0; i < resolutions.size(); i++) {
        float scale = (float)resolutions[i]/pixels.cols;
        cv::resize(pixels, pixels, cv::Size(0, 0),
                    scale, scale, cv::INTER_AREA);

        mipmaps[i][image->get_name()] = SDL_CreateTexture(renderer,
                                           SDL_PIXELFORMAT_BGR24,
                                           SDL_TEXTUREACCESS_STATIC,
                                           pixels.cols,
                                           pixels.rows);

        SDL_UpdateTexture(mipmaps[i][image->get_name()], NULL, (void*)pixels.data, pixels.step1());
    }
}
