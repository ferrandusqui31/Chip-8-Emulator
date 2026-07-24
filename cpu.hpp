#ifndef CPU_HPP
#define CPU_HPP

#include <ctime>
#include <iostream>
#include <random>

#include <SDL3/SDL.h>

#include "lots.hpp"
#include "macros.hpp"

class Cpu
{
private:
    SDL_Mutex *timer_mutex = nullptr;

    unsigned short fetch();
    void decode(unsigned short instr);

    void decode_0(unsigned short instr);
    void decode_8(unsigned short instr);
    void decode_e(unsigned short instr);
    void decode_f(unsigned short instr);

    SDL_Scancode getScanCode(unsigned char key);
    void display_sprite(unsigned char x, unsigned char y, unsigned char h);
    void init_fonts();

public:
    int cpu_main();

private:
    int timers_func();

public:
    static int SDLCALL genericThreadCallback(void *data);
};

struct CpuThreadPayload
{
    Cpu *self = nullptr;
    int (Cpu::*func)();
};
#endif