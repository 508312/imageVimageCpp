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

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1600;

bool init(SDL_Window** window_ptr, SDL_Surface** surface_ptr, SDL_Renderer** renderer_ptr);
void close(SDL_Window** window);
void clearScreen(SDL_Renderer* renderer);

int main( int argc, char* args[] ) {
    SetProcessDPIAware();

    //SDL stuff
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool running = true;

    if( !init(&window, &screenSurface, &renderer) ) {
        printf( "Failed to initialize!\n" );
        return 0;
    }

    Timer t1;
    Timer t2;

    SDLTextureLoader test_loader({1600, 800, 400, 200, 100, 50, 25}, renderer);

    ImageBuilder builder(320, 1600, 1600, 1, 3, 1, &test_loader);
    builder.load_images("hemtai");
    builder.build_images();

    std::vector<CompositeImage> images = *(builder.get_images());
    SDLGuimage test(&test_loader, &images[0], renderer);

    float zoom = 1;
    test.change_zoom(zoom);
    test.change_cam_pos(800, 800);
    test.generate_image();
    SDL_RenderPresent( renderer );

    while(running) {
        // Events
        while( SDL_PollEvent( &event ) != 0 )
        {
            if( event.type == SDL_QUIT )
            {
                running = false;
            } else if (event.type == SDL_MOUSEWHEEL) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                zoom = zoom + event.wheel.preciseY;
                test.move_cam_pos_based_on_mouse(x, y, event.wheel.preciseY + 1);
                test.increment_zoom(event.wheel.preciseY + 1);
                //test.change_zoom(zoom);

                test.generate_image();
                SDL_RenderPresent( renderer );
            }
        }
    }
    close(&window);

    return 0;
}


/* Handles initializing SDL window. */
bool init(SDL_Window** window_ptr, SDL_Surface** surface_ptr, SDL_Renderer** renderer_ptr) {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    *window_ptr = SDL_CreateWindow( "imageVimage", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( *window_ptr == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    *renderer_ptr = SDL_CreateRenderer( *window_ptr, -1, SDL_RENDERER_ACCELERATED );

    SDL_SetRenderDrawColor( *renderer_ptr, 0xFF, 0xFF, 0xFF, 0xFF );

    *surface_ptr = SDL_GetWindowSurface( *window_ptr );

    return true;
}

/* Handles closing the window and deallocating the memory. */
void close(SDL_Window** window) {
    SDL_DestroyWindow( *window );
    *window = NULL;
    SDL_Quit();
}

/* Clears screen. */
void clearScreen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor( renderer, 0x66, 0x66, 0x99, 0xFF );
    SDL_RenderClear( renderer );
}
