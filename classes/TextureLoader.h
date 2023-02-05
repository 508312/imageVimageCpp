#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <unordered_map>
#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "CompositeImage.h"

class TextureLoader
{
    public:
        TextureLoader();
        TextureLoader(std :: initializer_list <int> resoluts);
        virtual ~TextureLoader();

        cv::Mat& get_texture(CompositeImage* image, int width);

        cv::Mat& get_full_texture(CompositeImage* image);

        void set_texture(CompositeImage* image, cv::Mat texture);

    protected:

    private:
        int find_closest_res(int width);
        // arr of tables
        std::unordered_map<std::string, cv::Mat> table;

        std::vector<std::unordered_map<std::string, cv::Mat>> mipmaps;
        std::vector<int> resolutions;
};

#endif // TEXTURELOADER_H
