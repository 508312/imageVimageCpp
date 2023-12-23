#include "SDLTextureLoader.h"
#include <execution>

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr) : TextureLoader(img_bldr){
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, SDL_Renderer* renderer)
    : SDLTextureLoader(img_bldr, resoluts, renderer, 0){
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts,
                                    SDL_Renderer* renderer, int load_threshold) : TextureLoader(img_bldr, resoluts, load_threshold) {
    this->renderer = renderer;
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts, SDL_Renderer* renderer)
 : SDLTextureLoader(img_bldr, resoluts, renderer, 0){
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts,
                                    SDL_Renderer* renderer, int load_threshold): TextureLoader(img_bldr, resoluts, load_threshold){
    this->renderer = renderer;
}

SDLTextureLoader::~SDLTextureLoader(){
    free_textures();
}

void SDLTextureLoader::set_texture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind) {
    for (int i = start_ind; i < end_ind; i++) {

        float scale = (float)resolutions[i]/pixels.cols;
        cv::resize(pixels, pixels, cv::Size(0, 0),
                    scale, scale, cv::INTER_AREA);

        //done bcuz sometimes texture creation fails
        mipmaps[i][image->getId()] = NULL;
        while (mipmaps[i][image->getId()] == NULL) {
            mipmaps[i][image->getId()] = SDL_CreateTexture(renderer,
                                               SDL_PIXELFORMAT_BGR24,
                                               SDL_TEXTUREACCESS_STATIC,
                                               pixels.cols,
                                               pixels.rows);

            if (mipmaps[i][image->getId()] == NULL) {
                std::cout << "ERROR ON TEXTURE CREATION " << SDL_GetError() << std::endl;
            }
        }

        if (SDL_UpdateTexture(mipmaps[i][image->getId()], NULL, (void*)pixels.data, pixels.step1()) != 0) {
            std::cout << "Writing pixels to texture failed, error " << SDL_GetError() << std::endl;
        }
    }
}

void SDLTextureLoader::load_set(std::vector<CompositeImage*>& images) {
    resize_to(images.size());

    std::vector<int> indexes;

    for (int i = 0; i < images.size(); i++) {
        indexes.push_back(i);
    }

    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
                                cv::Mat full;
                                image_builder->create_final(images[i]->getId(), full);
                                set_below_threshold(images[i], full); });
}

void SDLTextureLoader::free_texture(int res_index, int img_index) {
    if (mipmaps[res_index][img_index] != NULL) {
        SDL_DestroyTexture(mipmaps[res_index][img_index]);
        mipmaps[res_index][img_index] = NULL;
    }
}
