#ifndef CPU_HPP
#define CPU_HPP

#include <SDL3/SDL.h>

#include <atomic>
#include <ctime>
#include <random>
#include <stack>

#include "macros.hpp"

class Cpu
{
private:
    struct CpuThreadPayload
    {
        Cpu *self = nullptr;
        int (Cpu::*func)();
    };

    std::atomic<bool> running = true;
    SDL_Mutex *timer_mutex = nullptr;
    SDL_Thread *timers_thread = nullptr;

    // Frequency should be 1MHz
    static constexpr double PERIOD_NS = 1600000;

    // 4K memory
    unsigned char mem[0xfff];

    // Display
    uint8_t display[BASE_WIDTH * BASE_HEIGHT];

    // Registers
    unsigned char vreg[16];
    unsigned short I_reg, pc;

    // Stack (for call and ret instructions)
    std::stack<unsigned short> stack;

    // Timers
    unsigned char delay_timer, sound_timer;

public:
    // Keypad
    SDL_Mutex *input_mutex = nullptr;
    bool awaiting_input = false;
    bool recieved_input = false;
    unsigned char key_pressed;

private:
    unsigned short fetch();
    void decode(unsigned short instr);
    void decode_0(unsigned short instr);
    void decode_8(unsigned short instr);
    void decode_e(unsigned short instr);
    void decode_f(unsigned short instr);

    SDL_Scancode getScanCode(unsigned char key);
    void display_sprite(unsigned char x, unsigned char y, unsigned char h);

    int timers_func();
    static int SDLCALL genericThreadCallback(void *data);

    void init_fonts();

public:
    ~Cpu();

    int cpu_main();
    SDL_Thread *init(void *data, size_t data_size);
    void stop();

    // Consultors
    uint8_t *getDisplay();
};

#endif