#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <atomic>

#include "cpu.hpp"

class Window
{
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    const SDL_Color colors[2] = {
        {30, 26, 77, 255},
        {163, 179, 255, 255}};

    SDL_Palette *palette = nullptr;
    SDL_Texture *texture = nullptr;

    SDL_Thread *cpu_thread = nullptr;

    std::atomic<bool> running = true;
    // SDL_Surface *screen_surface = nullptr;

    Cpu *cpu = nullptr;

    void init_window();
    static void SDLCALL load_rom_callback(void *userdata, const char *const *filelist, int filter);
    void load_rom(const char *const *filelist, int filter);

public:
    void init();
    void quit();
    void render();
    void handleEvents();

private:
    void send_pressed_key(SDL_Event event);

public:
    // Consultors
    bool isRunning();
    Cpu *getCpu();
};
#endif