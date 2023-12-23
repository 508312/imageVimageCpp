#ifndef NAVIGATOR_H
#define NAVIGATOR_H
#include <vector>
#include <SDLGuimage.h>

struct xy {
    int x;
    int y;
};

class Navigator {
    public:
        Navigator(SDLGuimage* guimage, std::vector<int> path);
        virtual ~Navigator();

        void step(float scale);

    protected:

    private:
        SDLGuimage* mGuimage;
        std::vector<int> mPath;
        int mPos;

        xy getCurPos();
};

#endif // NAVIGATOR_H
