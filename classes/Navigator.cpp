#include "Navigator.h"

Navigator::Navigator(SDLGuimage* guimage, std::vector<int> path) {
    mGuimage = guimage;
    mPath = path;
}

void Navigator::step(float scale) {

}

Navigator::~Navigator()
{
    //dtor
}
