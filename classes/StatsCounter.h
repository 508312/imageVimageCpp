#ifndef STATSCOUNTER_H
#define STATSCOUNTER_H
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "CompositeImage.h"
/** Class to keep track of seen images and other fun stats of used and unused images **/
class StatsCounter
{
    public:
        StatsCounter(std::vector<CompositeImage>* images);
        virtual ~StatsCounter();
        /** Adds image which was seen on canvas **/
        void add_seen_image(CompositeImage* img);
        /** Returns images seen on canvas **/
        int get_seen();
        /** Gets total amount of images in the set **/
        int get_total();
        /** Calculates total amount of images **/
        void calc_totals();

        /** Calculates how many unique images are reachable from the specified image **/
        int calc_reachable_from_img(CompositeImage* img);
    protected:

    private:
        /** Keeps track of unique images in each image **/
        std::unordered_map<int, std::unordered_set<int>> unique_images;
        /** Keeps track of all images **/
        std::unordered_set<int> all_images;

        /** Name to index mapping **/
        std::unordered_map<std::string, int> str_index;

        /** Total images in the set **/
        int total = 0;

        /** Seen images in the set **/
        int seen = 0;
};

#endif // STATSCOUNTER_H
