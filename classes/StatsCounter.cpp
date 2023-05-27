#include "StatsCounter.h"
#include <execution>
#include <stack>

StatsCounter::StatsCounter(std::vector<CompositeImage>* images)
{
    std::vector<int> indexes;
    for (int i = 0; i < images->size(); i++) {
        str_index[(*images)[i].get_name()] = i;
        unique_images[i].clear();
        indexes.push_back(i);
    }


    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
                    CompositeImage* img = &((*images)[i]);
                    for (int j = 0; j < img->get_num_parts_height(); j++) {
                        for (int k = 0; k < img->get_num_parts_width(); k++) {
                            unique_images[i].insert(str_index[img->get_image_at(j, k)->get_name()]);
                        }
                    }});

    calc_totals();
}

StatsCounter::~StatsCounter()
{
    //dtor
}

int StatsCounter::get_total() {
    return total;
}

int StatsCounter::get_seen() {
    return total - all_images.size();
}

int StatsCounter::calc_reachable_from_img(CompositeImage* img) {
    int cur_img;
    std::unordered_set<int> alr_seen;
    std::stack<int> imgstack;
    imgstack.push(str_index[img->get_name()]);
    while (imgstack.size() > 0) {
        cur_img = imgstack.top();
        imgstack.pop();

        if (alr_seen.count(cur_img))
            continue;

        for (int img_ind : unique_images[cur_img]) {
            imgstack.push(img_ind);
        }

        alr_seen.insert(cur_img);
    }
    return alr_seen.size();
}

void StatsCounter::add_seen_image(CompositeImage* img) {
    int ind = str_index[img->get_name()];
    all_images.erase(ind);

    //for (int img_ind : unique_images[ind]) {
    //    all_images.erase(img_ind);
    //}

}

void StatsCounter::calc_totals() {
    for (auto const& [key, ind] : str_index) {
        for (int img_ind : unique_images[ind]) {
            all_images.insert(img_ind);
        }
    }
    total = all_images.size();
}
