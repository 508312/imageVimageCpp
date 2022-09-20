/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>


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
    SDL_Delay(100);
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

class Timer {
    std::chrono::time_point<std::chrono::steady_clock> startT;
    public:
        void start();
        int get();
        Timer();
};

Timer::Timer() { }

void Timer::start() {
    startT = std::chrono::steady_clock::now();
}

int Timer::get() {
    std::chrono::time_point<std::chrono::steady_clock> endT = std::chrono::steady_clock::now();

    std::chrono::microseconds diff = std::chrono::duration_cast<std::chrono::microseconds>(endT - startT);

    return diff.count();
}


int main( int argc, char* args[] ) {
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //The window renderer
    SDL_Renderer* renderer = NULL;

    //Event handler
    SDL_Event e;

    SDL_Texture* images[700];

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

	int i = 0;
    for (const auto & entry : std::filesystem::directory_iterator("images"))
    {
        if (i == 700)
            break;
        images[i] = IMG_LoadTexture(renderer, entry.path().string().c_str());
        std::cout << i++ << std::endl;
    }

    Timer t;
	int avg = 0;
	int n = 1;
    float scale = 0;
    //Main game loop.
    while(!quit) {

        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            } else if (e.type == SDL_MOUSEWHEEL) {
                if(e.wheel.y > 0){ // scroll up
                    scale += e.wheel.preciseX;
                }
                else if(e.wheel.y < 0){ // scroll down
                    scale -= e.wheel.preciseY;
                }

            }
        }

        clearScreen( renderer );

        t.start();
        for (int i=0; i<150; i++) {
            for (int j=0; j<150; j++){
                texr.x = SCREEN_WIDTH/150 * j;
                SDL_RenderCopy(renderer, images[std::rand()%700], NULL, &texr);
            }
            texr.y = SCREEN_HEIGHT/150 * i;
        }

        std::cout << t.get() << std::endl;

        // copy the texture to the rendering context
        SDL_RenderCopy(renderer, img, NULL, &texr);

        SDL_RenderPresent( renderer );
        SDL_Delay(10);
    }
    close(&window);

    return 0;
}
