#include <windows.h>
#include <winuser.h>

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <Timer.h>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/hal/interface.h>

#include "SDLTextureLoader.h"
#include "ImageBuilder.h"
#include "SDLGuimage.h"
#include "StatsCounter.h"
#include "ImageSyncer.h"

#include <stdlib.h>

#include <cstdlib>

bool init_sdl(SDL_Window** window_ptr, SDL_Surface** surface_ptr, SDL_Renderer** renderer_ptr,
               int x_win_res, int y_win_res);
void init_vars(int argc, char** args, int& x_window, int& y_window,
               int& x_image, int& y_image, int& parts_x, int& parts_y,
               int& detail_width, int& local_transition_width, char** render_type,
               int& prune_threshold, int& closeness_thresh, float& final_upscale,
               std::vector<int>& resolutions, char** folder);
void closeSDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** screenSurface);
void clearScreen(SDL_Renderer* renderer);

int main( int argc, char* args[] ) {
    SetProcessDPIAware();

    int x_win_res, y_win_res, x_img_res, y_img_res, x_img_parts, y_img_parts,
    detail_width, local_transition_threshold, prune_threshold, closeness_threshold;
    float final_upscale;
    std::vector<int> resolutions;
    char* render_type;
    char* folder = "qats_small";

    Timer t1;
    Timer t2;

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool running = true;

    init_vars(argc, args, x_win_res, y_win_res, x_img_res, y_img_res, x_img_parts, y_img_parts,
          detail_width, local_transition_threshold, &render_type, prune_threshold, closeness_threshold,
          final_upscale, resolutions, &folder);

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, render_type);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    if( !init_sdl(&window, &screenSurface, &renderer, x_win_res, y_win_res) ) {
        std::cout << "FAILED TO CREATE A WINDOW" << std::endl;
        return 0;
    }

    ImageBuilder builder(x_img_parts, y_img_parts, x_img_res, y_img_res, final_upscale,
                          prune_threshold, closeness_threshold);
    builder.load_images(folder);

    t1.start();
    builder.build_images();
    SDLTextureLoader test_loader(&builder, resolutions, renderer, 2);
    std::vector<CompositeImage*> ptrs_to_imgs = builder.get_pointers_to_images();
    test_loader.load_set(ptrs_to_imgs);

    std::cout << "whole process " << t1.get() << std::endl;

    std::vector<CompositeImage>* images = builder.get_images();

    t1.start();
    StatsCounter stats_counter(images);
    std::cout << "stats took " << t1.get() << std::endl;

    std::cout << "total used: " << stats_counter.get_total()
     << " total images: " << images->size() <<
     " reachable from start: " << stats_counter.calc_reachable_from_img(&(*images)[0]) << std::endl;

    SDLGuimage test(x_win_res, y_win_res, detail_width, &test_loader, &(*images)[0], renderer, &stats_counter);

    float zoom = 1;
    test.change_zoom(zoom);

    int crash = 0;
    std::vector<SDLGuimage*> bottom_level;

    clearScreen(renderer);
    test.generate_image();

    Timer t;
    while(running) {
        while(SDL_PollEvent(&event) != 0)
        {
            t.start();
            if(event.type == SDL_QUIT)
            {
                running = false;
            } else if (event.type == SDL_MOUSEWHEEL) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                zoom = zoom + event.wheel.preciseY;
                test.move_cam_pos_based_on_mouse(x, y, event.wheel.preciseY * 0.1 + 1);
                test.increment_zoom(event.wheel.preciseY * 0.1 + 1);

                clearScreen(renderer);
                test.generate_image();
                std::cout << "FPS " << 1000000.0/t.get() << std::endl;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f) {
                    std::cout << "Seen " << stats_counter.get_seen() <<
                    " images out of " <<  stats_counter.get_total() << std::endl;
                }

            }
        }

        /* for memory checks
        test.increment_zoom(1.12);
        test.generate_image();
        SDL_RenderPresent( renderer );


        if (crash > 200) {
            running = false;
        }

        crash++;
        */
        SDL_RenderPresent(renderer);
        SDL_Delay(0);
    }

    //test_loader.free_textures();
    std::cout << "here" << std::endl;
    closeSDL(&window, &renderer, &screenSurface);
    std::cout << "here2" << std::endl;

    return 0;
}


void init_vars(int argc, char** args, int& x_window, int& y_window,
               int& x_image, int& y_image, int& parts_x, int& parts_y,
               int& detail_width, int& local_transition_width, char** render_type,
               int& prune_threshold, int& closeness_thresh, float& final_upscale,
               std::vector<int>& resolutions, char** folder) {
    x_window = 1600;
    y_window = 1600;
    x_image = 1600;
    y_image = 1600;
    parts_x = 400;
    parts_y = 400;
    prune_threshold = 3;
    closeness_thresh = 0;
    final_upscale = 1;
    detail_width = 1900;
    local_transition_width = 1600;
    resolutions = {1500, 800, 400, 200, 100, 50, 25, 10, 5, 2};
    *render_type = "software";

    for (int i = 1; i < argc; i++) {
        std::cout << "here" << std::endl;
        if (!strcmp(args[i], "-winx")) {
            i++;
            x_window = atoi(args[i]);
        } if (!strcmp(args[i], "-winy")) {
            i++;
            y_window = atoi(args[i]);
        } if (!strcmp(args[i], "-imgx")) {
            i++;
            x_image = atoi(args[i]);
        } if (!strcmp(args[i], "-imgy")) {
            i++;
            y_image = atoi(args[i]);
        } if (!strcmp(args[i], "-parts_x")) {
            std::cout << "inside" << std::endl;
            i++;
            parts_x = atoi(args[i]);
        } if (!strcmp(args[i], "-parts_y")) {
            i++;
            parts_y = atoi(args[i]);
        } if (!strcmp(args[i], "-detail_width")) {
            i++;
            detail_width = atoi(args[i]);
        } if (!strcmp(args[i], "-prune")) {
            i++;
            prune_threshold = atoi(args[i]);
        } if (!strcmp(args[i], "-closeness")) {
            i++;
            closeness_thresh = atoi(args[i]);
        } if (!strcmp(args[i], "-render")) {
            i++;
            *render_type = args[i];
        } if (!strcmp(args[i], "-upscale")) {
            i++;
            final_upscale = atof(args[i]);
        } if (!strcmp(args[i], "-transition_width")) {
            i++;
            local_transition_width = atoi(args[i]);
        } if (!strcmp(args[i], "-resolutions")) {
            i++;
            int num_res = atoi(args[i]);
            resolutions.clear();
            for (int j = 0; j < num_res; j++) {
                i++;
                resolutions.push_back(atoi(args[i]));
            }
        } if (!strcmp(args[i], "-folder")) {
            i++;
            *folder = args[i];
        }
    }
}


/* Handles initializing SDL window. */
bool init_sdl(SDL_Window** window_ptr, SDL_Surface** surface_ptr, SDL_Renderer** renderer_ptr,
            int x_win_res, int y_win_res) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Initialization failure " << SDL_GetError() << std::endl;
        return false;
    }

    *window_ptr = SDL_CreateWindow("imageVimage", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, x_win_res,
                                y_win_res, SDL_WINDOW_SHOWN);
    if(*window_ptr == NULL) {
        std::cout << "Window creation failure " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer_ptr = SDL_CreateRenderer(*window_ptr, -1, SDL_RENDERER_ACCELERATED);

    *surface_ptr = SDL_GetWindowSurface(*window_ptr);

    return true;
}

/* Handles closing the window and deallocating the memory. */
void closeSDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** screenSurface) {
    // TODO: find out what causes crash of a program on line below
    SDL_DestroyRenderer(*renderer);
    std::cout << "destroyed renderer" << std::endl;
    *renderer = NULL;
    SDL_FreeSurface(*screenSurface);
    std::cout << "destroyed surface" << std::endl;
    *screenSurface = NULL;
    SDL_DestroyWindow(*window);
    std::cout << "destroyed window" << std::endl;
    *window = NULL;
    SDL_Quit();
}

/* Clears screen. */
void clearScreen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}


//800x800 12FPS
//800x800 8.9FPS
