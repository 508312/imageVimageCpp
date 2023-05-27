#include "SDLTextureLoader.h"

SDLTextureLoader::SDLTextureLoader() : TextureLoader()
{
}

SDLTextureLoader::SDLTextureLoader(std::initializer_list<int> resoluts, SDL_Renderer* renderer) : SDLTextureLoader(resoluts, renderer, 0)
{
}

SDLTextureLoader::SDLTextureLoader(std::initializer_list<int> resoluts,
                                    SDL_Renderer* renderer, int load_threshold) : TextureLoader(resoluts, load_threshold) {
    this->renderer = renderer;
}

SDLTextureLoader::SDLTextureLoader(std::vector<int>& resoluts, SDL_Renderer* renderer) : SDLTextureLoader(resoluts, renderer, 0)
{
}

SDLTextureLoader::SDLTextureLoader(std::vector<int>& resoluts,
                                    SDL_Renderer* renderer, int load_threshold) : TextureLoader(resoluts, load_threshold)
{
    this->renderer = renderer;
}

SDLTextureLoader::~SDLTextureLoader()
{
    //TODO:: CHECK IF THIS IS SAFE, i mean it should be, right? But then calling free textures inside parent class doesnt work.
    free_textures();
}

void SDLTextureLoader::set_texture(CompositeImage* image, cv::Mat& pixels) {
    for (int i = 0; i < resolutions.size(); i++) {
        float scale = (float)resolutions[i]/pixels.cols;
        cv::resize(pixels, pixels, cv::Size(0, 0),
                    scale, scale, cv::INTER_AREA);

        //done bcuz sometimes texture creation fails
        mipmaps[i][image->get_ind()] = NULL;
        while (mipmaps[i][image->get_ind()] == NULL) {
            mipmaps[i][image->get_ind()] = SDL_CreateTexture(renderer,
                                               SDL_PIXELFORMAT_BGR24,
                                               SDL_TEXTUREACCESS_STATIC,
                                               pixels.cols,
                                               pixels.rows);
        }

        SDL_UpdateTexture(mipmaps[i][image->get_ind()], NULL, (void*)pixels.data, pixels.step1());
    }
}

void SDLTextureLoader::free_textures() {
    int amnt = 0;

    SDL_Texture* texture_to_delete;
    for (int i = 0; i < resolutions.size(); i++) {
        for(int j = 0; j < mipmaps[i].size(); j++) {
            texture_to_delete = mipmaps[i][j];
            SDL_DestroyTexture(texture_to_delete);
            amnt++;
        }
    }
    std::cout << "DELETED " << amnt << std::endl;
}

void SDLTextureLoader::set_below_threshold(CompositeImage* image, cv::Mat& pixels) {
}

void SDLTextureLoader::set_above_threshold(CompositeImage* image, cv::Mat& pixels) {
}
