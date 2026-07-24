#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "cpu.hpp"

class Window
{
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    SDL_Palette *palette = nullptr;
    SDL_Texture *texture = nullptr;

    SDL_Thread *cpu_thread = nullptr;

    Cpu *cpu = nullptr;

    void init_emulator();
    void init_screen();
    void init_window();
    void init_cpu();
    static void SDLCALL load_rom_callback(void *userdata, const char *const *filelist, int filter);
    void load_rom(const char *const *filelist, int filter);

public:
    void init();
    void quit();
    void render();
    void handleEvents();

private:
    void send_pressed_key(SDL_Event event);
};
#endif