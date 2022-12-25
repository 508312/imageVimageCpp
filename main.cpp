/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <vips/vips8>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <Timer.h>

#define VIPS_DEBUG
#define VIPS_DEBUG_VERBOSE



//Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 680;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

/* Handles initializing SDL window. */
bool init(SDL_Window** window_ptr, SDL_Surface** surface_ptr, SDL_Renderer** renderer_ptr) {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    //Create window
    *window_ptr = SDL_CreateWindow( "imageVimage", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( *window_ptr == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    *renderer_ptr = SDL_CreateRenderer( *window_ptr, -1, SDL_RENDERER_ACCELERATED );

    SDL_SetRenderDrawColor( *renderer_ptr, 0xFF, 0xFF, 0xFF, 0xFF );
    //Get window surface
    *surface_ptr = SDL_GetWindowSurface( *window_ptr );

    return true;
}

/* Handles closing the window and deallocating the memory. */
void close(SDL_Window** window) {
    //Destroy window
    SDL_DestroyWindow( *window );
    *window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

/* Clears screen. */
void clearScreen(SDL_Renderer* renderer) {
    //Clear screen
    SDL_SetRenderDrawColor( renderer, 0x66, 0x66, 0x99, 0xFF );
    SDL_RenderClear( renderer );
}

int main( int argc, char* args[] ) {
    std::cout << VIPS_INIT(args[0]) << std::endl << std::endl;


    vips::VImage in = vips_image_new_from_file ("loaded.png", "access", VIPS_ACCESS_SEQUENTIAL, NULL);

    //vips::VImage::new_from_file( "loaded.png", vips::VImage::option() ->set( "access", "sequential" ) );

    std::cout << "Is image null: " << in.is_null() << std::endl;

    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //The window renderer
    SDL_Renderer* renderer = NULL;

    //Event handler
    SDL_Event e;

    SDL_Texture* images[700];

    vips::VImage vimages[700];

    //Main loop flag
    bool quit = false;

    if( !init(&window, &screenSurface, &renderer) ) {
        printf( "Failed to initialize!\n" );
        return 0;
    }

    SDL_Texture* img = NULL;
    int w, h; // texture width & height
    // load our image
	img = IMG_LoadTexture(renderer, "loaded2.jpg");
	SDL_QueryTexture(img, NULL, NULL, &w, &h);
	SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = SCREEN_WIDTH/150; texr.h = SCREEN_HEIGHT/150;

    Timer t;
    Timer t2;
	int avg = 0;
	int n = 1;
    float scale = 1;

    t.start();
    int i = 0;
    for (const auto & entry : std::filesystem::directory_iterator("images"))
    {
        if (i == 620) //626
            break;
        //images[i] = IMG_LoadTexture(renderer, entry.path().string().c_str());
        vimages[i] = vips_image_new_from_file (entry.path().string().c_str(), "access", VIPS_ACCESS_SEQUENTIAL, NULL);
        //vimages[i].resize(0.99);
        // std::cout << i++ << std::endl;
    }
    std::cout << "time " << t.get() << std::endl;
    std::cout << in.width() << std::endl;

    size_t res_size;

    void* data = NULL;
    std::cout << "here1";
    t.start();

    /*
    SDL_Surface* sur = SDL_CreateRGBSurfaceFrom(data, in.width(),
                                                 in.height(), 4 * 8, in.width() * 4 * 8,
                                                 0xFF0000, 0x00FF00, 0x0000FF, 0x000000);
    std::cout << "here2";

    std::cout << std::endl << SDL_GetError() << std::endl;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, sur);

    std::cout << "here3";
    */


    SDL_Texture* buffer = SDL_CreateTexture(renderer,
                           SDL_PIXELFORMAT_RGB24,
                           SDL_TEXTUREACCESS_STREAMING,
                           in.width(),
                           in.height());

    int pitch = NULL;

    SDL_LockTexture(buffer, NULL, &data, &pitch);

    void* data2 = in.write_to_memory(&res_size);

    memcpy(data, data2, res_size);

    SDL_UnlockTexture(buffer);

    std::cout << "time " <<t.get() << std::endl;

    //Main game loop.
    quit = false;
    while(!quit) {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            } else if (e.type == SDL_MOUSEWHEEL) {
                scale = scale + e.wheel.preciseY;
                texr.w = SCREEN_WIDTH/150 * scale;
                texr.h = SCREEN_HEIGHT/150 * scale;
                //std::cout << scale << std::endl;

            }
        }
        clearScreen( renderer );

        t.start();
        /*
        for (int i=0; i<150; i++) {
            for (int j=0; j<150; j++){
                texr.x = SCREEN_WIDTH/150 * scale * j;
                //SDL_RenderCopy(renderer, images[(i*j*163)%626], NULL, &texr);
                SDL_RenderCopy(renderer, images[1], NULL, &texr);
            }
            texr.y = SCREEN_HEIGHT/150 * scale * i;
        }
        */


        SDL_RenderCopy(renderer, buffer, NULL, NULL);

        std::cout << t.get() << std::endl;

        t2.start();
        SDL_RenderPresent( renderer );

        std::cout << "render " << t2.get() << std::endl;

    }
    close(&window);

    return 0;
}
