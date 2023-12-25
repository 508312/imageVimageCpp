#include "SDLTextureLoader.h"
#include <execution>

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr) : TextureLoader(img_bldr){
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts, SDL_Renderer* renderer)
    : SDLTextureLoader(img_bldr, resoluts, renderer, 0){
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::initializer_list<int> resoluts,
                                    SDL_Renderer* renderer, int mLoadThreshold) : TextureLoader(img_bldr, resoluts, mLoadThreshold) {
    this->mRenderer = renderer;
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts, SDL_Renderer* renderer)
 : SDLTextureLoader(img_bldr, resoluts, renderer, 0){
}

SDLTextureLoader::SDLTextureLoader(ImageBuilder* img_bldr, std::vector<int>& resoluts,
                                    SDL_Renderer* renderer, int mLoadThreshold): TextureLoader(img_bldr, resoluts, mLoadThreshold){
    this->mRenderer = renderer;
}

SDLTextureLoader::~SDLTextureLoader(){
    freeTextures();
}

void SDLTextureLoader::setTexture(CompositeImage* image, cv::Mat& pixels, uint8_t start_ind, uint8_t end_ind) {
    for (int i = start_ind; i < end_ind; i++) {

        float scale = (float)mResolution[i]/pixels.cols;
        cv::resize(pixels, pixels, cv::Size(0, 0),
                    scale, scale, cv::INTER_AREA);

        //done bcuz sometimes texture creation fails
        mMipmaps[i][image->getId()] = NULL;
        while (mMipmaps[i][image->getId()] == NULL) {
            mMipmaps[i][image->getId()] = SDL_CreateTexture(mRenderer,
                                               SDL_PIXELFORMAT_BGR24,
                                               SDL_TEXTUREACCESS_STATIC,
                                               pixels.cols,
                                               pixels.rows);

            if (mMipmaps[i][image->getId()] == NULL) {
                std::cout << "ERROR ON TEXTURE CREATION " << SDL_GetError() << std::endl;
            }
        }

        if (SDL_UpdateTexture(mMipmaps[i][image->getId()], NULL, (void*)pixels.data, pixels.step1()) != 0) {
            std::cout << "Writing pixels to texture failed, error " << SDL_GetError() << std::endl;
        }
        if (SDL_SetTextureAlphaMod(mMipmaps[i][image->getId()], 16) != 0) {
            std::cout << "Error on changing alpha" << std::endl;
        }
    }
}

void SDLTextureLoader::loadSet(std::vector<CompositeImage*>& images) {
    resizeTo(images.size());

    std::vector<int> indexes;

    for (int i = 0; i < images.size(); i++) {
        indexes.push_back(i);
    }

    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
                                cv::Mat full;
                                mImageBuilder->createFinal(images[i]->getId(), full);
                                setBelowThreshold(images[i], full); });
}

void SDLTextureLoader::freeTexture(int res_index, int img_index) {
    if (mMipmaps[res_index][img_index] != NULL) {
        SDL_DestroyTexture(mMipmaps[res_index][img_index]);
        mMipmaps[res_index][img_index] = NULL;
    }
}
