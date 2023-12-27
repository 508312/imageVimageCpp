#include <windows.h>
#include <winuser.h>

#include <SDL.h>
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
#include "starting_variables.h"

#include <stdlib.h>

#include <cstdlib>

bool init_sdl(SDL_Window** window_ptr, SDL_Surface** surface_ptr, SDL_Renderer** renderer_ptr,
               int x_win_res, int y_win_res);
void initialize_starting_variables(int argc, char** args, starting_variables& vars);
void closeSDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** screen_surface);
void clearScreen(SDL_Renderer* renderer);

int main( int argc, char* args[] ) {
    SetProcessDPIAware();

    starting_variables starting_vars;

    Timer t1;
    Timer t2;

    SDL_Window* window = NULL;
    SDL_Surface* screen_surface = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool running = true;

    initialize_starting_variables(argc, args, starting_vars);

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, starting_vars.render_type);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    if( !init_sdl(&window, &screen_surface, &renderer, starting_vars.window_width, starting_vars.window_height) ) {
        std::cout << "FAILED TO CREATE A WINDOW" << std::endl;
        return 0;
    }

    ImageBuilder builder(starting_vars.image_number_of_cols, starting_vars.image_number_of_rows, starting_vars.image_width, starting_vars.image_height,
                        starting_vars.compiled_upscale, starting_vars.prune_threshold, starting_vars.closeness_threshold);
    builder.loadImages(starting_vars.folder);

    t1.start();
    builder.buildImages();
    SDLTextureLoader test_loader(&builder, starting_vars.resolutions, renderer, 2);
    std::vector<CompositeImage*> ptrs_to_imgs = builder.getPointersToImages();
    test_loader.loadSet(ptrs_to_imgs);

    std::cout << "whole process " << t1.get() << std::endl;

    std::vector<CompositeImage>* images = builder.getImages();

    t1.start();
    StatsCounter stats_counter(images);
    std::cout << "stats took " << t1.get() << std::endl;

    std::cout << "total used: " << stats_counter.getTotal()
     << " total images: " << images->size() <<
     " reachable from start: " << stats_counter.calculateReachableFromImage(&(*images)[0]) << std::endl;

    SDLGuimage test(starting_vars.image_width, starting_vars.image_height, starting_vars.detailed_image_width,
                    starting_vars.local_transition_width, &test_loader, &(*images)[1], renderer, &stats_counter);

    float zoom = 1;
    test.changezoom(zoom);

    int crash = 0;
    std::vector<SDLGuimage*> bottom_level;

    clearScreen(renderer);
    test.generateImage();

    Timer t;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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
                test.moveCamPosBasedOnMouse(x, y, event.wheel.preciseY * 0.1 + 1);
                test.incrementzoom(event.wheel.preciseY * 0.1 + 1);

                clearScreen(renderer);
                test.generateImage();

                std::cout << "FPS " << 1000000.0/t.get() << std::endl;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f) {
                    std::cout << "Seen " << stats_counter.getSeen() <<
                    " images out of " <<  stats_counter.getTotal() << std::endl;
                }

            }
        }

        /*
        test.incrementzoom(1.05);
        test.generateImage();
        SDL_RenderPresent( renderer );


        if (crash > 10000) {
            running = false;
        }

        crash++;
        */

        SDL_RenderPresent(renderer);
        SDL_Delay(0);
    }
    //test_loader.freeTextures();
    std::cout << "here" << std::endl;
    closeSDL(&window, &renderer, &screen_surface);
    std::cout << "here2" << std::endl;

    return 0;
}


void initialize_starting_variables(int argc, char** args, starting_variables& vars) {
    vars.window_width = 1600;
    vars.window_height = 1600;
    vars.image_width = 1600;
    vars.image_height = 1600;
    vars.image_number_of_cols = 400;
    vars.image_number_of_rows = 400;
    vars.prune_threshold = 3;
    vars.closeness_threshold = 0;
    vars.compiled_upscale = 1;
    vars.detailed_image_width = 3000;
    vars.local_transition_width = 1600;
    vars.resolutions = {1600, 800, 400, 200, 100, 50, 25, 10, 5};
    vars.render_type = "software";
    vars.folder = "imgsmall";


    for (int i = 1; i < argc; i++) {
        if (!strcmp(args[i], "--winx")) {
            i++;
            vars.window_width = atoi(args[i]);
        } if (!strcmp(args[i], "--winy")) {
            i++;
            vars.window_height = atoi(args[i]);
        } if (!strcmp(args[i], "--imgx")) {
            i++;
            vars.image_width = atoi(args[i]);
        } if (!strcmp(args[i], "--imgy")) {
            i++;
            vars.image_height = atoi(args[i]);
        } if (!strcmp(args[i], "--parts_x")) {
            std::cout << "inside" << std::endl;
            i++;
            vars.image_number_of_cols = atoi(args[i]);
        } if (!strcmp(args[i], "--parts_y")) {
            i++;
            vars.image_number_of_rows = atoi(args[i]);
        } if (!strcmp(args[i], "--detail_width")) {
            i++;
            vars.detailed_image_width = atoi(args[i]);
        } if (!strcmp(args[i], "--prune")) {
            i++;
            vars.prune_threshold = atoi(args[i]);
        } if (!strcmp(args[i], "--closeness")) {
            i++;
            vars.closeness_threshold = atoi(args[i]);
        } if (!strcmp(args[i], "--render")) {
            i++;
            vars.render_type = args[i];
            std::cout << "renderer " << *vars.render_type << std::endl;
        } if (!strcmp(args[i], "--upscale")) {
            i++;
            vars.compiled_upscale = atof(args[i]);
        } if (!strcmp(args[i], "--transition_width")) {
            i++;
            vars.local_transition_width = atoi(args[i]);
        } if (!strcmp(args[i], "--resolutions")) {
            i++;
            int num_res = atoi(args[i]);
            vars.resolutions.clear();
            for (int j = 0; j < num_res; j++) {
                i++;
                vars.resolutions.push_back(atoi(args[i]));
            }
        } if (!strcmp(args[i], "--folder")) {
            i++;
            vars.folder = args[i];
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
    // TODO: find out what causes freeze of a program on line below. It only happens when a few images have been loaded in.
    //                                      Doesn't happen when only a few image textures have been loaded into memory.
    SDL_ClearError();
    SDL_DestroyRenderer(*renderer);
    std::cout << "destroyed renderer " << SDL_GetError() << std::endl;
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
