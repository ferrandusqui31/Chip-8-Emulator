#include "window.hpp"


void Window::init_window()
{
    SDL_CreateWindowAndRenderer(APP_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_INPUT_FOCUS, &window, &renderer);

    SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);

    palette = SDL_CreatePalette(2);
    SDL_SetPaletteColors(palette, colors, 0, 2);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    SDL_SetTexturePalette(texture, palette);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
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

    cpu = new Cpu();
    cpu_thread = cpu->init(data, data_size);
}

void Window::init()
{
    init_window();

    // Init emulator
    SDL_ShowOpenFileDialog(load_rom_callback, this, NULL, NULL, 0, SDL_GetCurrentDirectory(), false);
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

    SDL_UpdateTexture(texture, NULL, cpu->getDisplay(), 64 * sizeof(uint8_t));
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
            SDL_LockMutex(cpu->input_mutex);
            if (cpu->awaiting_input)
                send_pressed_key(event);
            SDL_UnlockMutex(cpu->input_mutex);
            break;
        }
    }
}

void Window::send_pressed_key(SDL_Event event)
{
    switch (event.key.scancode)
    {
    case SDL_SCANCODE_1:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x1;
        break;
    case SDL_SCANCODE_2:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x2;
        break;
    case SDL_SCANCODE_3:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x3;
        break;
    case SDL_SCANCODE_4:
        cpu->recieved_input = true;
        cpu->key_pressed = 0xc;
        break;
    case SDL_SCANCODE_Q:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x4;
        break;
    case SDL_SCANCODE_W:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x5;
        break;
    case SDL_SCANCODE_E:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x6;
        break;
    case SDL_SCANCODE_R:
        cpu->recieved_input = true;
        cpu->key_pressed = 0xd;
        break;
    case SDL_SCANCODE_A:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x7;
        break;
    case SDL_SCANCODE_S:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x8;
        break;
    case SDL_SCANCODE_D:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x9;
        break;
    case SDL_SCANCODE_F:
        cpu->recieved_input = true;
        cpu->key_pressed = 0xe;
        break;
    case SDL_SCANCODE_Z:
        cpu->recieved_input = true;
        cpu->key_pressed = 0xa;
        break;
    case SDL_SCANCODE_X:
        cpu->recieved_input = true;
        cpu->key_pressed = 0x0;
        break;
    case SDL_SCANCODE_C:
        cpu->recieved_input = true;
        cpu->key_pressed = 0xb;
        break;
    case SDL_SCANCODE_V:
        cpu->recieved_input = true;
        cpu->key_pressed = 0xf;
        break;
    }
}

bool Window::isRunning()
{
    return running;
}

Cpu *Window::getCpu()
{
    return cpu;
}
