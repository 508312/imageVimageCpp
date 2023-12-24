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
        void addSeenImage(CompositeImage* img);
        /** Returns images seen on canvas **/
        int getSeen();
        /** Gets total amount of images in the set **/
        int getTotal();
        /** Calculates total amount of images **/
        void calculateTotals();

        /** Calculates how many unique images are reachable from the specified image **/
        int calculateReachableFromImage(CompositeImage* img);
    protected:

    private:
        /** Keeps track of unique images in each image **/
        std::unordered_map<int, std::unordered_set<int>> mUniqueImages;
        /** Keeps track of all images **/
        std::unordered_set<int> mAllImages;

        /** Name to index mapping **/
        std::unordered_map<std::string, int> mStrIndex;

        /** Total images in the set **/
        int mTotal = 0;

        /** Seen images in the set **/
        int mSeen = 0;
};

#endif // STATSCOUNTER_H
