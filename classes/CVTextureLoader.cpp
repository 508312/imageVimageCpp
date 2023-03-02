#include "CVTextureLoader.h"
#include "TextureLoader.h"

CVTextureLoader::CVTextureLoader() : TextureLoader()
{
}

CVTextureLoader::~CVTextureLoader()
{
    //dtor
}

CVTextureLoader::CVTextureLoader(std :: initializer_list <int> resoluts) : TextureLoader(resoluts)
{
}

void CVTextureLoader::set_texture(CompositeImage* image, cv::Mat& pixels) {
    for (int i = 0; i < resolutions.size(); i++) {
        float scale = (float)resolutions[i]/pixels.cols;
        cv::resize(pixels, pixels, cv::Size(0, 0),
                    scale, scale, cv::INTER_AREA);
        mipmaps[i][image->get_name()] = pixels.clone();
    }
}
