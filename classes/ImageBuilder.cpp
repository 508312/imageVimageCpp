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
    //ctor
    num_parts = 320;
    width = 1600;
    height = 1600;
    final_upscale = 2;
    prune_threshold = 10;
    closeness_threshold = 1;
}

ImageBuilder::ImageBuilder(int parts, int w, int h, int fin_up, int prune, int closeness, TextureSetter* texl) {
    num_parts = parts;
    width = w;
    height = h;
    final_upscale = fin_up;
    prune_threshold = prune;
    closeness_threshold = closeness;
    tex_loader = texl;
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
        images[i].fill_grid_with_empty();
        indexes.push_back(i);
    }
    std::for_each(std::execution::par_unseq, indexes.begin(), indexes.end(), [&](int i){
              build_image(i);});

    for (int i = 0; i < get_num_images(); i++) {
        t.start();
        create_final(i);
        std::cout << "image " << i << " done " << t.get() << std::endl;
    }
    resized_images.clear();
}

void ImageBuilder::build_image(int ind) {
    CompositeImage* closest = nullptr;
    int parts = images[ind].get_num_parts();
    color crop_clr;
    int left, top;
    pos p;

    std::unordered_map<CompositeImage*, int> counter;

    std::unordered_map<CompositeImage*, std::vector<pos>> positions;
    std::vector<CompositeImage*> images_above_threshold;

    Timer t;

    images[ind].load_to_mem();
    t.start();

    // TEMP for debug. LATER will need to optimize finding closest by checking top and left neighbor.
    color left_avg;
    color* top_avgs = new color[parts];

    color avg;
    bool is_left, is_top;
    Timer t1;
    int t1tot = 0;
    for (int i=0; i < parts; i++) {
        top = i * (height/parts);
        for (int j = 0; j < parts; j++) {
            left = j * (width/parts);

            avg = images[ind].crop_avg_color(left, top, width/parts, height/parts);

            if (i != 0 && j != 0) {
                if (CompositeImage::distance(avg, left_avg) <= closeness_threshold) {
                    closest = images[ind].get_image_at(i, j - 1);
                }
                else if (CompositeImage::distance(avg, top_avgs[j]) <= closeness_threshold)
                    closest = images[ind].get_image_at(i - 1, j);
            }
            t1.start();
            if (closest == nullptr)
                closest = find_closest_image(ind, avg, &pointers_to_images);
            t1tot += t1.get();

            counter[closest] += 1;
            if (counter[closest] <= prune_threshold) {
                p.x = i;
                p.y = j;
                positions[closest].push_back(p);
            } else if (positions[closest].size() != 0) {
                positions[closest].clear();
                images_above_threshold.push_back(closest);
            }
            left_avg = avg;
            top_avgs[j] = avg;
            //images[ind].push_to_grid(closest);
            images[ind].set_image_at(i, j, closest);
            closest = nullptr;
        }

    }
    delete[] top_avgs;
    if (prune_threshold > 0) {
        prune(ind, positions, counter, &images_above_threshold);
    }
    std::cout << "composing " << ind << " " << images[ind].get_name() << " done " << t.get() << std::endl;
    std::cout << "finding closest total " << t1tot << std::endl;
    images[ind].unload_from_mem();


    /*
    // STUPID COUNTING FOR FUTURE OPTIMIZATION
    std::unordered_map<int, int> counter_of_couters;

    for(std::unordered_map<CompositeImage*, int>::iterator iter = counter.begin(); iter != counter.end(); ++iter) {
        CompositeImage* key =  iter->first;

        std::cout << key->get_name() << " --- " << counter[key] << std::endl;

        counter_of_couters[counter[key]]++;
    }

    std::cout << "----------------------------FULL_TOTAL_SCORE---------------------------------------" << std::endl;

    for(int i = 0; i < parts*parts; i++) {
        if (counter_of_couters[i] != 0)
            std::cout << i << " --- " << counter_of_couters[i] << std::endl;
    }
    */

}

void ImageBuilder::prune(int ind, std::unordered_map<CompositeImage*, std::vector<pos>> positions,
                        std::unordered_map<CompositeImage*, int>& amounts,
                        std::vector<CompositeImage*>* imgs_abv_thrsh) {
    if (positions.size() == 0) {
        return;
    }
    CompositeImage* closest;
    int top;
    int left;
    int parts;
    int total = 0;
    CompositeImage* image;
    for(std::unordered_map<CompositeImage*,std::vector<pos>>::iterator iter = positions.begin();
                                                                        iter != positions.end(); ++iter) {
        image = iter->first;
        parts = image->get_num_parts();
        if (positions[image].size() == 0) {
            continue;
        }
        total++;
        amounts[image] = 0;
        for(int k = 0; k < positions[image].size(); k++) {
            pos p = positions[image][k];

            top = p.x * (height/parts);
            left = p.y * (width/parts);
            color avg = images[ind].crop_avg_color(left, top, width/parts, height/parts);
            closest = find_closest_image(-1, avg, imgs_abv_thrsh);
            images[ind].change_grid(p.x, p.y, closest);
        }
    }

    std::cout << "PRUNED " << total << " images" << std::endl;

}

void ImageBuilder::create_final(int ind) {
    std::vector<CompositeImage*>* grid = images[ind].get_grid();
    cv::Mat full;

    // EASILY CAN BE MADE SO THAT IT RESIZES ONLY THOSE WHICH IT NEEDS TO(had this b4),
    // but this is just more readable
    fill_table(get_num_images(),
                calculate_small_dim(width, num_parts, final_upscale),
                calculate_small_dim(height, num_parts, final_upscale),
                final_upscale, &resized_images, &pointers_to_images);

    std::cout << "concat started " << std::endl;
    concat_all(num_parts, num_parts, final_upscale, resized_images, grid, full);

    tex_loader->set_texture(pointers_to_images[ind], full);

    //cv::imwrite(("folder2\\" + images[ind].get_name() + "_compiled" + images[ind].get_extension()).c_str(), full);
}

void ImageBuilder::fill_table(int num_images, int small_width, int small_height, float final_upscale,
                                std::unordered_map<CompositeImage*, cv::Mat>* resized_images,
                                std::vector<CompositeImage*>* images) {
    for (int i = 0; i < num_images; i++) {
        CompositeImage* img = (*images)[i];
        if((*resized_images)[img].empty()) {
            if (img->get_image() == NULL) {
                (*resized_images)[img] = img->load_image();
            } else {
                (*resized_images)[img] = *img->get_image();
            }
            cv::resize((*resized_images)[img], (*resized_images)[img],
                        cv::Size(small_width, small_height), 0, 0, cv::INTER_AREA);

            std::cout << "RESIZING TO SMALL SIZE, total " << (*resized_images).size() << std::endl;

        }
    }
}

int ImageBuilder::calculate_small_dim(int dim, int parts, float upscale) {
    return dim/(float)parts * upscale;
}

void ImageBuilder::concat_all(int rows, int cols, float final_upscale,
                                std::unordered_map<CompositeImage*, cv::Mat>& resized_images,
                                std::vector<CompositeImage*>* grid, cv::Mat& full) {
    cv::Mat* harr = new cv::Mat[cols];
    cv::Mat* varr = new cv::Mat[rows];

    Timer t;
    t.start();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            CompositeImage* img = (*grid)[i * cols + j];

            harr[j] = resized_images[img];
        }
        cv::hconcat(harr, cols, varr[i]);
    }
    cv::vconcat(varr, rows, full);

    delete[] harr;
    delete[] varr;
    // std::cout << "actual combining " << t.get() << std::endl;
}

CompositeImage* ImageBuilder::find_closest_image(int ind, color& clr, std::vector<CompositeImage*>* imgs) {
    int best_index = 0;
    int best_distance = INT_MAX;
    int distance;

    for (int i = 0; i < imgs->size(); i++) {
        /*
        if (i == ind) {
            continue;
        }
        */
        distance = (*imgs)[i]->get_distance_to_color(clr);
        if (distance < best_distance) {
            best_index = i;
            best_distance = distance;
            if (distance == 0) {

                break;
            }
        }
    }
    return (*imgs)[best_index];
}


int ImageBuilder::get_num_images() {
    return images.size();
}

void ImageBuilder::load_images(std::string path) {
    Timer t;
    t.start();

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        //std::cout << entry.path().string() << std::endl;
        images.push_back(CompositeImage(num_parts, entry.path().string(), width, height));
        // UNSAFE????? TODO: ????
        pointers_to_images.push_back(&images[images.size() - 1]);
        //images[images.size()-1].compute_avg();
    }

    std::for_each(std::execution::par_unseq, images.begin(), images.end(), [](CompositeImage& image){
                  std::cout << image.get_name() << std::endl;
                  image.compute_avg();});

    // UNSAFE????? TODO: ????
    for (int i = 0; i < pointers_to_images.size(); i++) {
        pointers_to_images[i] = &images[i];
    }

    std::cout << "loading and computing averages of all images took " << t.get() << std::endl;
}

std::vector<CompositeImage>* ImageBuilder::get_images() {
    return &images;
}
