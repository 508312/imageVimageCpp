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
    //TODO:: CHECK IF THIS IS SAFE, i mean it should be, right? But then calling free textures inside parent class doesnt work.
    free_textures();
}

void SDLTextureLoader::set_texture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind) {
    for (int i = start_ind; i < end_ind; i++) {
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

            if (mipmaps[i][image->get_ind()] == NULL) {
                std::cout << "ERROR ON TEXTURE CREATION " << SDL_GetError() << std::endl;
            }
        }

        SDL_UpdateTexture(mipmaps[i][image->get_ind()], NULL, (void*)pixels.data, pixels.step1());
    }
}

void SDLTextureLoader::set_texture(CompositeImage* image, cv::Mat& pixels) {
    set_texture(image, pixels, 0, resolutions.size());
}

void SDLTextureLoader::load_set(std::vector<CompositeImage*>& images) {
    resize_to(images.size());

    std::vector<int> indexes;

    for (int i = 0; i < images.size(); i++) {
        cv::Mat full;
        image_builder->create_final(images[i]->get_ind(), full);
        set_below_threshold(images[i], full);


        indexes.push_back(i);
    }


}

void SDLTextureLoader::free_textures() {
    int amnt = 0;

    SDL_Texture* texture_to_delete;
    for (int i = 0; i < resolutions.size(); i++) {
        for(int j = 0; j < mipmaps[i].size(); j++) {
            texture_to_delete = mipmaps[i][j];
            SDL_DestroyTexture(texture_to_delete);
            mipmaps[i][j] = NULL;
            amnt++;
        }
    }
    std::cout << "DELETED " << amnt << std::endl;
}
