#include <SDL3/SDL_main.h>
#include "window.hpp"

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    
    Window win;

    win.init();

    while (running)
    {
        win.handleEvents();
        win.render();
    }

    SDL_Quit();

    return 0;
}