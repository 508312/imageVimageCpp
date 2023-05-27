#include "TextureSetter.h"

TextureSetter::TextureSetter() {
    resolutions.push_back(1600);
}

TextureSetter::TextureSetter(std::initializer_list<int> resoluts):TextureSetter(resoluts, 0)  {

}

TextureSetter::TextureSetter(std::initializer_list<int> resoluts, int load_threshold) {
    for (int res : resoluts) {
        resolutions.push_back(res);
    }
    this->load_threshold = load_threshold;
}

TextureSetter::TextureSetter(std::vector<int>& resoluts):TextureSetter(resoluts, 0) {

}

TextureSetter::TextureSetter(std::vector<int>& resoluts, int load_threshold) {
    for (int res : resoluts) {
        resolutions.push_back(res);
    }
    this->load_threshold = load_threshold;
}

TextureSetter::~TextureSetter()
{
    //dtor
}
