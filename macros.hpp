#ifndef MACROS_HPP
#define MACROS_HPP

// Window definition macros
#define APP_NAME "CHIP-8 Emulator"
#define WINDOW_SCALE 20
#define WINDOW_WIDTH WINDOW_SCALE * 64
#define WINDOW_HEIGHT WINDOW_SCALE * 32

// Chip configurations
#define ORIGINAL_CHIP
// #define SUPER_CHIP

#define FONT_ADDRESS 0x50
constexpr int FONT_HEIGHT = 5;

#define MAX_ROM_SIZE 4096 - 0x200

// Debug
#define undecodable(func, instr) printf("%s: Unrecognized instruction %X\n", func, instr);exit(EXIT_FAILURE)

#endif