#include "ImageBuilder.h"
#include "CompositeImage.h"
#include <filesystem>
#include <iostream>
#include <Timer.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <execution>
#include "TextureLoader.h"

ImageBuilder::ImageBuilder() {
}

ImageBuilder::ImageBuilder(int parts_w, int parts_h, int w, int h, int fin_up, int prune,
                            int closeness) {
    mNumWidth = parts_w;
    mNumHeight = parts_h;
    mWidth = w;
    mHeight = h;
    mFinalUpscale = fin_up;
    mPruneThreshold = prune;
    mClosenessThreshold = closeness;
}

ImageBuilder::~ImageBuilder() {
    //dtor
}

void ImageBuilder::buildImages() {
    Timer t;

    /*
    for (int i = 0; i < getNumImages(); i++) {
        buildImage(i);
    }
    */

    std::vector<int> indexes;

    for (int i = 0; i < images.size(); i++) {
        images[i].resizeGrid();
        indexes.push_back(i);
    }

    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
              buildImage(i);});

    // EASILY CAN BE MADE SO THAT IT RESIZES ONLY THOSE WHICH IT NEEDS TO(had this b4),
    // but this is just more readable
    fillTable(getNumImages(),
                calculateSmallDim(mWidth, mNumWidth, mFinalUpscale),
                calculateSmallDim(mHeight, mNumHeight, mFinalUpscale),
                mFinalUpscale, mResizedImages, mPointersToImages);

    fillTable(getNumImages(), mWidth/8, mHeight/8, mFinalUpscale, mResizedBig, mPointersToImages);

    cv::Mat tmp;
    std::for_each(std::execution::seq, indexes.begin(), indexes.end(), [&](int i){
                  //images[i].coalesceBlocks(3000);
                  createFinal(i, tmp);});


    /*
    for (int i = 0; i < getNumImages(); i++) {
        t.start();
        createFinal(i);
        std::cout << "image " << i << " done " << t.get() << std::endl;
    } */

    //mResizedImages.clear();
}

void ImageBuilder::buildImage(int ind) {
    int closest = -1;
    int parts_w = images[ind].getNumWidth();
    int parts_h = images[ind].getNumHeight();
    color crop_clr;
    int left, top;
    pos p;

    std::vector<int> counter;
    counter.resize(getNumImages(), 0);

    std::vector<std::vector<pos>> positions;
    positions.resize(getNumImages(), std::vector<pos>());
    std::vector<CompositeImage*> images_above_threshold;

    Timer t;

    images[ind].loadImageToMemory();
    t.start();

    color left_avg;
    color* top_avgs = new color[parts_w];

    color avg;
    bool is_left, is_top;
    Timer t1;
    int t1tot = 0;
    for (int i=0; i < parts_h; i++) {
        top = i * (mHeight/parts_h);
        for (int j = 0; j < parts_w; j++) {
            left = j * (mWidth/parts_w);

            avg = images[ind].getCropAvgColor(left, top, mWidth/parts_w, mHeight/parts_h);

            if (i != 0 && j != 0) {
                if (CompositeImage::distance(avg, left_avg) <= mClosenessThreshold) {
                    closest = images[ind].getImageIdAt(i, j - 1);
                }
                else if (CompositeImage::distance(avg, top_avgs[j]) <= mClosenessThreshold)
                    closest = images[ind].getImageIdAt(i - 1, j);
            }
            t1.start();
            if (closest == -1)
                closest = findClosestImage(ind, avg, &mPointersToImages);
            t1tot += t1.get();

            if (mPruneThreshold > 0) {
                counter[closest] += 1;
                if (counter[closest] <= mPruneThreshold) {
                    p.x = i;
                    p.y = j;
                    positions[closest].push_back(p);
                } else if (positions[closest].size() != 0) {
                    positions[closest].clear();
                    images_above_threshold.push_back(mPointersToImages[closest]);
                }
            }

            left_avg = avg;
            top_avgs[j] = avg;
            //images[ind].pushToGrid(closest);
            images[ind].setImageAt(i, j, closest);
            closest = -1;
        }

    }
    delete[] top_avgs;
    if (mPruneThreshold > 0) {
        prune(ind, positions, counter, &images_above_threshold);
    }
    std::cout << "composing " << ind << " " << images[ind].getName() << " done " << t.get() << std::endl;
    std::cout << "finding closest total " << t1tot << std::endl;
    images[ind].unloadImageFromMemory();


    /*
    // STUPID COUNTING FOR FUTURE OPTIMIZATION
    std::unordered_map<int, int> counter_of_couters;

    for(std::unordered_map<CompositeImage*, int>::iterator iter = counter.begin(); iter != counter.end(); ++iter) {
        CompositeImage* key =  iter->first;

        std::cout << key->getName() << " --- " << counter[key] << std::endl;

        counter_of_couters[counter[key]]++;
    }

    std::cout << "----------------------------FULL_TOTAL_SCORE---------------------------------------" << std::endl;

    for(int i = 0; i < parts*parts; i++) {
        if (counter_of_couters[i] != 0)
            std::cout << i << " --- " << counter_of_couters[i] << std::endl;
    }
    */

}

void ImageBuilder::prune(int ind, std::vector<std::vector<pos>> positions,
                        std::vector<int>& amounts,
                        std::vector<CompositeImage*>* imgs_abv_thrsh) {
    if (positions.size() == 0) {
        return;
    }
    cid closest;
    int top;
    int left;
    int parts_w;
    int parts_h;
    int total = 0;
    cid image;
    for(int i = 0; i < positions.size(); ++i) {
        image = i;
        if (positions[image].size() == 0) {
            continue;
        }
        parts_w = images[image].getNumWidth();
        parts_h = images[image].getNumHeight();
        total++;
        amounts[image] = 0;
        for(int k = 0; k < positions[image].size(); k++) {
            pos p = positions[image][k];

            top = p.x * (mHeight/parts_h);
            left = p.y * (mWidth/parts_w);
            color avg = images[ind].getCropAvgColor(left, top, mWidth/parts_w, mHeight/parts_h);
            closest = findClosestImage(-1, avg, imgs_abv_thrsh);
            images[ind].setImageAt(p.x, p.y, closest);
        }
    }

    std::cout << "PRUNED " << total << " images" << std::endl;

}

void ImageBuilder::createFinal(int ind, cv::Mat& concatted_image) {
    std::vector<cid>* grid = images[ind].getGrid();

    std::cout << "concat started " << std::endl;
    concatAll(mNumHeight, mNumWidth, mFinalUpscale, mResizedImages, grid, concatted_image);

    int small_width = calculateSmallDim(mWidth, mNumWidth, mFinalUpscale);
    int small_height = calculateSmallDim(mHeight, mNumHeight, mFinalUpscale);
    cv::Mat tmp;
    cv::Mat destRoi;
    int li = 0;
    int lj = 0;
    Timer t;
    t.start();

    /*
    for (int i = 0; i < mNumHeight; i++) {
        for (int j = 0; j < mNumWidth; j++ ) {
            cid idx = (*grid)[i * mNumWidth + j];
            if (idx < CID_RESERVED)
                std::cout << idx << " ";
            else
                std::cout << "X ";
        }
        std::cout << "\n";
    }
    */



    std::vector<std::vector<int>> used;
    used.resize(mNumHeight);

    for (int i = 0; i < mNumHeight; i++) {
        for (int j = 0; j < mNumWidth; j++ ) {
            cid idx = (*grid)[i * mNumWidth + j];
            if (idx == CID_CORNER) {
                cid old_idx = (*grid)[li * mNumWidth + lj];
                //std::cout << "pasted " << small_width * (j - lj) << " " << small_height * (j - lj) << " to "
                // << small_width * j << " " << small_height * i << "\n";
                uint16_pair start_pos = images[ind].findStart(i, j);
                if (std::find(used[start_pos.i].begin(), used[start_pos.i].end(), start_pos.j) != used[start_pos.i].end()) {
                    continue;
                }
                used[start_pos.i].push_back(start_pos.j);

                int side = std::max(i - start_pos.i, j - start_pos.j);

                cv::resize(mResizedBig[old_idx], tmp,
                    cv::Size(small_width * (side + 1), small_height * (side + 1)), 0, 0);

                destRoi = concatted_image(cv::Rect(small_width * start_pos.j, small_height * start_pos.i,
                                                    small_width * (side + 1), small_height * (side + 1)));
                tmp.copyTo(destRoi);
            }
            if (idx >= CID_RESERVED) {
                continue;
            }
            li = i;
            lj = j;
        }
    }
    std::cout << "TOOOOK " << t.get() << std::endl;
    //cv::imwrite(("folderCoal\\" + images[ind].getName() + "_compiled" + images[ind].getExtension()).c_str(), concatted_image);

}

void ImageBuilder::fillTable(int num_images, int small_width, int small_height, float mFinalUpscale,
                                std::vector<cv::Mat>& mResizedImages,
                                std::vector<CompositeImage*>& images_to_resize) {
    mResizedImages.resize(num_images);
    std::vector<int> indexes;
    for (int img = 0; img < num_images; img++) {
        mResizedImages[img] = NULL;
        indexes.push_back(img);
    }

    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
        CompositeImage* img = images_to_resize[i];
        if (img == NULL) {
            return;
        }

        if (img->getImage() == NULL) {
            mResizedImages[i] = img->loadImage();
        } else {
            mResizedImages[i] = *img->getImage();
        }
        cv::resize(mResizedImages[i], mResizedImages[i],
                cv::Size(small_width, small_height), 0, 0, cv::INTER_AREA);
        std::cout << "RESIZING TO SMALL SIZE, total " << mResizedImages.size() << std::endl;
    });
}

int ImageBuilder::calculateSmallDim(int dim, int parts, float upscale) {
    return dim/(float)parts * upscale;
}

void ImageBuilder::concatAll(int rows, int cols, float mFinalUpscale,
                                std::vector<cv::Mat>& mResizedImages,
                                std::vector<cid>* grid, cv::Mat& full) {
    cv::Mat* harr = new cv::Mat[cols];
    cv::Mat* varr = new cv::Mat[rows];
    cid cur_img;

    Timer t;
    t.start();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cid img = (*grid)[i * cols + j];
            if (img >= CID_RESERVED) {
                img = cur_img;
            } else {
                cur_img = img;
            }

            harr[j] = mResizedImages[img];
        }
        cv::hconcat(harr, cols, varr[i]);
    }
    cv::vconcat(varr, rows, full);

    delete[] harr;
    delete[] varr;
    std::cout << "actual combining " << t.get() << std::endl;
}

int ImageBuilder::findClosestImage(int ind, color& clr, std::vector<CompositeImage*>* imgs) {
    int best_index = 0;
    int best_distance = INT_MAX;
    int distance;

    for (int i = 0; i < imgs->size(); i++) {
        //if (i == ind) {
        //    continue;
        //}

        distance = (*imgs)[i]->distanceToColor(clr);
        if (distance < best_distance) {
            best_index = i;
            best_distance = distance;
            if (distance == 0) {
                break;
            }
        }
    }
    return best_index;
}


int ImageBuilder::getNumImages() {
    return images.size();
}

void ImageBuilder::loadImages(std::string path) {
    Timer t;
    t.start();
    int ind = 0;

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        images.push_back(CompositeImage(mNumWidth, mNumHeight, entry.path().string(), mWidth, mHeight, ind, &mPointersToImages));
        ind++;
    }

    std::for_each(std::execution::par_unseq, images.begin(), images.end(), [](CompositeImage& image){
                  std::cout << image.getName() << std::endl;
                  image.computeAvgColor();});

    mPointersToImages.resize(images.size());
    for (int i = 0; i < images.size(); i++) {
        mPointersToImages[i] = &images[i];
    }
    std::cout << "loading and computing averages of all images took " << t.get() << std::endl;
}

std::vector<CompositeImage>* ImageBuilder::getImages() {
    return &images;
}

std::vector<CompositeImage*>& ImageBuilder::getPointersToImages() {
    return mPointersToImages;
}
