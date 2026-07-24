#ifndef LOTS_HPP
#define LOTS_HPP

#include <SDL3/SDL.h>

#include <atomic>
#include <stack>

// Related with emulator app logic tan chip-8 cpu
// inline std::atomic<bool> running = true;
// inline SDL_Surface *screen_surface = nullptr;

// 4K memory
// inline unsigned char mem[0xfff];

// Screen:
// implemented inside surface
// inline std::vector<std::vector<bool>> screen(64, std::vector<bool>(32));

// Registers
// inline unsigned char vreg[16];

// inline unsigned short I_reg, pc;

// Stack (for call and return instrs)
// inline std::stack<unsigned short> stack;

// Timers
// inline unsigned char delay_timer, sound_timer;

// Keypad
// inline SDL_Mutex *input_mutex = nullptr;
// inline bool awaiting_input = false;
// inline bool recieved_input = false;
// inline unsigned char key_pressed;

// Font

#endif