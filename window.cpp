#include "window.hpp"

void Window::init_emulator()
{
    init_screen();
    SDL_ShowOpenFileDialog(load_rom_callback, this, NULL, NULL, 0, SDL_GetCurrentDirectory(), false);
}

void Window::init_screen()
{
    uint8_t* pixels = (uint8_t*)surface->pixels;
    memset(pixels, 0, surface->pitch * surface->h);
}

void Window::init_window()
{
    SDL_CreateWindowAndRenderer(APP_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_INPUT_FOCUS, &window, &renderer);

    SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);

    palette = SDL_CreatePalette(2);
    SDL_SetPaletteColors(palette, colors, 0, 2);

    surface = SDL_CreateSurface(64, 32, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(surface, palette);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    SDL_SetTexturePalette(texture, palette);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void Window::init_cpu()
{
    cpu = new Cpu();
    CpuThreadPayload *payload = new CpuThreadPayload {cpu, Cpu::cpu_main};

    cpu_thread = SDL_CreateThread(Cpu::genericThreadCallback, "CPU Thread", payload);

}

void SDLCALL Window::load_rom_callback(void *userdata, const char *const *filelist, int filter)
{
    Window *self = static_cast<Window*>(userdata);
    self->load_rom(filelist, filter);
}

void Window::load_rom(const char *const *filelist, int filter)
{
    if (filelist == NULL)
    {
        SDL_Log("Error occurred while loading file: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    else if(filelist[0] == NULL)
    {
        SDL_Log("No ROM was selected");
        exit(EXIT_FAILURE);
    }

    size_t data_size;
    void *data = SDL_LoadFile(filelist[0], &data_size);
    if (data == NULL)
    {
        SDL_Log("Could not load ROM: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    memcpy(&mem[0x200], data, data_size);
    SDL_free(data);
    pc = 0x200;

    input_mutex = SDL_CreateMutex();
    
    init_cpu();
}

void Window::init()
{
    init_window();
    init_emulator();
}

void Window::quit()
{
    // Will have to kill al the threads (maybe with running bool is enough, idk)
    running = false;

    delete cpu;
}

void Window::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    
    SDL_RenderPresent(renderer);
}

void Window::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            quit();
            break;

        case SDL_EVENT_KEY_DOWN:
            SDL_LockMutex(input_mutex);
            if (awaiting_input)
                send_pressed_key(event);
            SDL_UnlockMutex(input_mutex);
            break;
        }
    }
}

void Window::send_pressed_key(SDL_Event event)
{
    switch (event.key.scancode)
    {
    case SDL_SCANCODE_1:
        recieved_input = true;
        key_pressed = 0x1;
        break;
    case SDL_SCANCODE_2:
        recieved_input = true;
        key_pressed = 0x2;
        break;
    case SDL_SCANCODE_3:
        recieved_input = true;
        key_pressed = 0x3;
        break;
    case SDL_SCANCODE_4:
        recieved_input = true;
        key_pressed = 0xc;
        break;
    case SDL_SCANCODE_Q:
        recieved_input = true;
        key_pressed = 0x4;
        break;
    case SDL_SCANCODE_W:
        recieved_input = true;
        key_pressed = 0x5;
        break;
    case SDL_SCANCODE_E:
        recieved_input = true;
        key_pressed = 0x6;
        break;
    case SDL_SCANCODE_R:
        recieved_input = true;
        key_pressed = 0xd;
        break;
    case SDL_SCANCODE_A:
        recieved_input = true;
        key_pressed = 0x7;
        break;
    case SDL_SCANCODE_S:
        recieved_input = true;
        key_pressed = 0x8;
        break;
    case SDL_SCANCODE_D:
        recieved_input = true;
        key_pressed = 0x9;
        break;
    case SDL_SCANCODE_F:
        recieved_input = true;
        key_pressed = 0xe;
        break;
    case SDL_SCANCODE_Z:
        recieved_input = true;
        key_pressed = 0xa;
        break;
    case SDL_SCANCODE_X:
        recieved_input = true;
        key_pressed = 0x0;
        break;
    case SDL_SCANCODE_C:
        recieved_input = true;
        key_pressed = 0xb;
        break;
    case SDL_SCANCODE_V:
        recieved_input = true;
        key_pressed = 0xf;
        break;
    }
}