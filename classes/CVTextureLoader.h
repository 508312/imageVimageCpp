#ifndef CVTEXTURELOADER_H
#define CVTEXTURELOADER_H
#include "TextureLoader.h"


// WTF IS THIS LOL??? WHY DOESNT IT WORK WITHOUT IT???????? WHY IS IT NOT NEEDED IN OTHER FILES????????
// bruh what i think everything is linked alright
#include "TextureLoader.cpp"

class CVTextureLoader : public TextureLoader<cv::Mat>
{
    public:
        CVTextureLoader();
        CVTextureLoader(std :: initializer_list <int> resoluts);

        virtual ~CVTextureLoader();

        void set_texture(CompositeImage* image, cv::Mat& pixels);

    protected:

    private:
};

#endif // CVTEXTURELOADER_H
