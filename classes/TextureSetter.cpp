#include "TextureSetter.h"

TextureSetter::TextureSetter() {
    resolutions.push_back(1600);
}

TextureSetter::TextureSetter(std :: initializer_list <int> resoluts) {
    for (int res : resoluts) {
        resolutions.push_back(res);
    }
}

TextureSetter::~TextureSetter()
{
    //dtor
}
