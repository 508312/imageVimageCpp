#include "StatsCounter.h"
#include <execution>
#include <stack>

StatsCounter::StatsCounter(std::vector<CompositeImage>* images)
{
    std::vector<int> indexes;
    for (int i = 0; i < images->size(); i++) {
        mStrIndex[(*images)[i].getName()] = i;
        mUniqueImages[i].clear();
        indexes.push_back(i);
    }


    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
                    CompositeImage* img = &((*images)[i]);
                    for (int j = 0; j < img->getNumHeight(); j++) {
                        for (int k = 0; k < img->getNumWidth(); k++) {
                            if (img->getImageIdAt(j, k) == (uint16_t)-1 ||
                                img->getImageIdAt(j, k) == (uint16_t)-2) {
                                continue;
                            }
                            mUniqueImages[i].insert(mStrIndex[img->getImageAt(j, k)->getName()]);
                        }
                    }});

    calculateTotals();
}

StatsCounter::~StatsCounter()
{
    //dtor
}

int StatsCounter::getTotal() {
    return mTotal;
}

int StatsCounter::getSeen() {
    return mTotal - mAllImages.size();
}

int StatsCounter::calculateReachableFromImage(CompositeImage* img) {
    int cur_img;
    std::unordered_set<int> alr_seen;
    std::stack<int> imgstack;
    imgstack.push(mStrIndex[img->getName()]);
    while (imgstack.size() > 0) {
        cur_img = imgstack.top();
        imgstack.pop();

        if (alr_seen.count(cur_img))
            continue;

        for (int img_ind : mUniqueImages[cur_img]) {
            imgstack.push(img_ind);
        }

        alr_seen.insert(cur_img);
    }
    return alr_seen.size();
}

void StatsCounter::addSeenImage(CompositeImage* img) {
    int ind = mStrIndex[img->getName()];
    mAllImages.erase(ind);

    //for (int img_ind : mUniqueImages[ind]) {
    //    mAllImages.erase(img_ind);
    //}

}

void StatsCounter::calculateTotals() {
    for (auto const& [key, ind] : mStrIndex) {
        for (int img_ind : mUniqueImages[ind]) {
            mAllImages.insert(img_ind);
        }
    }
    mTotal = mAllImages.size();
}
