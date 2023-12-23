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
    width = w;
    height = h;
    final_upscale = fin_up;
    prune_threshold = prune;
    closeness_threshold = closeness;
}

ImageBuilder::~ImageBuilder() {
    //dtor
}

void ImageBuilder::build_images() {
    Timer t;

    /*
    for (int i = 0; i < get_num_images(); i++) {
        build_image(i);
    }
    */

    std::vector<int> indexes;

    for (int i = 0; i < images.size(); i++) {
        images[i].resizeGrid();
        indexes.push_back(i);
    }

    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
              build_image(i);});

    // EASILY CAN BE MADE SO THAT IT RESIZES ONLY THOSE WHICH IT NEEDS TO(had this b4),
    // but this is just more readable
    fill_table(get_num_images(),
                calculate_small_dim(width, mNumWidth, final_upscale),
                calculate_small_dim(height, mNumHeight, final_upscale),
                final_upscale, resized_images, pointers_to_images);

    fill_table(get_num_images(), width/8, height/8, final_upscale, resized_big, pointers_to_images);

    cv::Mat tmp;
    std::for_each(std::execution::seq, indexes.begin(), indexes.end(), [&](int i){
                  //images[i].coalesceBlocks(30);
                  create_final(i, tmp);});


    /*
    for (int i = 0; i < get_num_images(); i++) {
        t.start();
        create_final(i);
        std::cout << "image " << i << " done " << t.get() << std::endl;
    } */

    //resized_images.clear();
}

void ImageBuilder::build_image(int ind) {
    int closest = -1;
    int parts_w = images[ind].getNumWidth();
    int parts_h = images[ind].getNumHeight();
    color crop_clr;
    int left, top;
    pos p;

    std::vector<int> counter;
    counter.resize(get_num_images(), 0);

    std::vector<std::vector<pos>> positions;
    positions.resize(get_num_images(), std::vector<pos>());
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
        top = i * (height/parts_h);
        for (int j = 0; j < parts_w; j++) {
            left = j * (width/parts_w);

            avg = images[ind].getCropAvgColor(left, top, width/parts_w, height/parts_h);

            if (i != 0 && j != 0) {
                if (CompositeImage::distance(avg, left_avg) <= closeness_threshold) {
                    closest = images[ind].getImageIdAt(i, j - 1);
                }
                else if (CompositeImage::distance(avg, top_avgs[j]) <= closeness_threshold)
                    closest = images[ind].getImageIdAt(i - 1, j);
            }
            t1.start();
            if (closest == -1)
                closest = find_closest_image(ind, avg, &pointers_to_images);
            t1tot += t1.get();

            if (prune_threshold > 0) {
                counter[closest] += 1;
                if (counter[closest] <= prune_threshold) {
                    p.x = i;
                    p.y = j;
                    positions[closest].push_back(p);
                } else if (positions[closest].size() != 0) {
                    positions[closest].clear();
                    images_above_threshold.push_back(pointers_to_images[closest]);
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
    if (prune_threshold > 0) {
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
    uint16_t closest;
    int top;
    int left;
    int parts_w;
    int parts_h;
    int total = 0;
    uint16_t image;
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

            top = p.x * (height/parts_h);
            left = p.y * (width/parts_w);
            color avg = images[ind].getCropAvgColor(left, top, width/parts_w, height/parts_h);
            closest = find_closest_image(-1, avg, imgs_abv_thrsh);
            images[ind].setImageAt(p.x, p.y, closest);
        }
    }

    std::cout << "PRUNED " << total << " images" << std::endl;

}

void ImageBuilder::create_final(int ind, cv::Mat& concatted_image) {
    std::vector<uint16_t>* grid = images[ind].getGrid();

    std::cout << "concat started " << std::endl;
    concat_all(mNumHeight, mNumWidth, final_upscale, resized_images, grid, concatted_image);

    int small_width = calculate_small_dim(width, mNumWidth, final_upscale);
    int small_height = calculate_small_dim(height, mNumHeight, final_upscale);
    cv::Mat tmp;
    cv::Mat destRoi;
    int li = 0;
    int lj = 0;
    Timer t;
    t.start();
    for (int i = 0; i < mNumHeight; i++) {
        for (int j = 0; j < mNumWidth; j++ ) {
            uint16_t idx = (*grid)[i * mNumWidth + j];
            if (idx == (uint16_t)-1) {
                continue;
            }
            if (idx == (uint16_t)-2) {
                uint16_t old_idx = (*grid)[li * mNumWidth + lj];
                //std::cout << "pasted " << small_width * (j - lj) << " " << small_height * (j - lj) << " to "
                // << small_width * j << " " << small_height * i << "\n";

                cv::resize(resized_big[old_idx], tmp,
                    cv::Size(small_width * (j - lj + 1), small_height * (j - lj + 1)), 0, 0);

                destRoi = concatted_image(cv::Rect(small_width * lj, small_height * li, small_width * (j - lj + 1), small_height * (j - lj + 1)));
                tmp.copyTo(destRoi);
            }
            li = i;
            lj = j;
        }
    }
    std::cout << "TOOOOK " << t.get() << std::endl;
    //cv::imwrite(("folderCoal\\" + images[ind].getName() + "_compiled" + images[ind].getExtension()).c_str(), concatted_image);

}

void ImageBuilder::fill_table(int num_images, int small_width, int small_height, float final_upscale,
                                std::vector<cv::Mat>& resized_images,
                                std::vector<CompositeImage*>& images_to_resize) {
    resized_images.resize(num_images);
    std::vector<int> indexes;
    for (int img = 0; img < num_images; img++) {
        resized_images[img] = NULL;
        indexes.push_back(img);
    }

    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
        CompositeImage* img = images_to_resize[i];
        if (img == NULL) {
            return;
        }

        if (img->getImage() == NULL) {
            resized_images[i] = img->loadImage();
        } else {
            resized_images[i] = *img->getImage();
        }
        cv::resize(resized_images[i], resized_images[i],
                cv::Size(small_width, small_height), 0, 0, cv::INTER_AREA);
        std::cout << "RESIZING TO SMALL SIZE, total " << resized_images.size() << std::endl;
    });
}

int ImageBuilder::calculate_small_dim(int dim, int parts, float upscale) {
    return dim/(float)parts * upscale;
}

void ImageBuilder::concat_all(int rows, int cols, float final_upscale,
                                std::vector<cv::Mat>& resized_images,
                                std::vector<uint16_t>* grid, cv::Mat& full) {
    cv::Mat* harr = new cv::Mat[cols];
    cv::Mat* varr = new cv::Mat[rows];
    uint16_t cur_img;

    Timer t;
    t.start();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            uint16_t img = (*grid)[i * cols + j];
            if (img == (uint16_t)-1 || img == (uint16_t)-2) {
                img = cur_img;
            } else {
                cur_img = img;
            }

            harr[j] = resized_images[img];
        }
        cv::hconcat(harr, cols, varr[i]);
    }
    cv::vconcat(varr, rows, full);

    delete[] harr;
    delete[] varr;
    std::cout << "actual combining " << t.get() << std::endl;
}

int ImageBuilder::find_closest_image(int ind, color& clr, std::vector<CompositeImage*>* imgs) {
    int best_index = 0;
    int best_distance = INT_MAX;
    int distance;

    for (int i = 0; i < imgs->size(); i++) {
        /*
        if (i == ind) {
            continue;
        }
        */
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


int ImageBuilder::get_num_images() {
    return images.size();
}

void ImageBuilder::load_images(std::string path) {
    Timer t;
    t.start();
    int ind = 0;

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        images.push_back(CompositeImage(mNumWidth, mNumHeight, entry.path().string(), width, height, ind, &pointers_to_images));
        ind++;
    }

    std::for_each(std::execution::par_unseq, images.begin(), images.end(), [](CompositeImage& image){
                  std::cout << image.getName() << std::endl;
                  image.computeAvgColor();});

    pointers_to_images.resize(images.size());
    for (int i = 0; i < images.size(); i++) {
        pointers_to_images[i] = &images[i];
    }
    std::cout << "loading and computing averages of all images took " << t.get() << std::endl;
}

std::vector<CompositeImage>* ImageBuilder::get_images() {
    return &images;
}

std::vector<CompositeImage*>& ImageBuilder::get_pointers_to_images() {
    return pointers_to_images;
}
