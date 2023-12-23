#ifndef COMPOSITEIMAGE_H
#define COMPOSITEIMAGE_H

#include <string>
#include <color.h>
#include <opencv2/core/core.hpp>

class CompositeImage {
    public:

        CompositeImage(int parts_w, int parts_h, std::string path,
                        int w, int h, uint16_t index, std::vector<CompositeImage*>* ind_maps);

        virtual ~CompositeImage();

        /** Returns how much images compose this image horizontally **/
        int getNumWidth();
        /** Returns how much images compose this image vertically **/
        int getNumHeight();

        /** Returns width resolution at which image was generated **/
        int getWidth();
        /** Returns height resolution at which image was generated **/
        int getHeight();

        /** Returns average color of original image. **/
        color getAvgColor();
        /** Returns average color of a crop from original image. **/
        color getCropAvgColor(int left, int top, int width, int height);

        /** Returns image grid of this image **/
        std::vector<uint16_t>* getGrid();

        /** Pushes image index to last open space in the grid. **/
        void pushToGrid(uint16_t image);

        /** Gets color distance to another image. **/
        int distanceToImage(CompositeImage* img2);

        /** Computes average color of this image. **/
        void computeAvgColor();

        /** Loads original image to memory. **/
        void loadImageToMemory();

        /** Unloads original image from memory. **/
        void unloadImageFromMemory();

        /** Resized grid to width * parts size. **/
        void resizeGrid();

        /** Returns average color of the passed image **/
        static color imageAvgColor(cv::Mat* image);

        /** Gets disatnce between average color of the image and passed color. **/
        int distanceToColor(const color& clr);

        /** Returns image name **/
        std::string getName();

        cv::Mat loadImage();

        cv::Mat* getImage();

        /** Returns original extension of the image. **/
        std::string getExtension();

        /** Returns pointer to image located at x,y position in the grid. **/
        CompositeImage* getImageAt(int x, int y);

        /** Returns image index as in ImageBuilder at the given x,y in the grid. **/
        uint16_t getImageIdAt(int x, int y);

        /** Changes selected image index in the grid. **/
        void setImageAt(int x, int y, uint16_t image);

        /** Returns image between two colors. **/
        static int distance(const color& c1, const color& c2);

        /** Return image index of this image. **/
        uint16_t getId();

        /** Coalesces images into blocks. **/
        void coalesceBlocks(int max_size);

    protected:

    private:
        /** Number of parts to divide the image horizontally. **/
        uint16_t mNumWidth;
        /** Number of parts to divide the image vertically. **/
        uint16_t mNumHeight;
        /** Resolution to load images at. **/
        uint16_t mWidth, mHeight;
        /** Index of an image as in ImageBuilder **/
        uint16_t mId;
        /** Mapping from indexes of image builder vector to pointers to images **/
        std::vector<CompositeImage*>* ind_img_map;

        /** Flag if the original image is loaded in this composite image **/
        bool mImgLoaded = false;

        /** Average color of this composite image. **/
        color mAvgColor;

        /** Grid of images of which this image consists. **/
        std::vector<uint16_t> mGrid;
        /** Name of the image as in file it was loaded from. **/
        std::string mName;
        /** Extension of the image as in file it was loaded from. **/
        std::string mExtension;
        /** Path to this image. **/
        std::string mPath;

        /** Original image is stored here if it is to be loaded. **/
        cv::Mat mStoredImage;
};

#endif // COMPOSITEIMAGE_H
