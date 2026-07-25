#ifndef MACROS_HPP
#define MACROS_HPP

// Chip configurations
#define FONT_ADDRESS 0x50
#define FONT_HEIGHT 5

#define BASE_WIDTH 64
#define BASE_HEIGHT 32

// Window definition macros
#define APP_NAME "CHIP-8 Emulator"
#define WINDOW_SCALE 20
#define WINDOW_WIDTH WINDOW_SCALE * BASE_WIDTH
#define WINDOW_HEIGHT WINDOW_SCALE * BASE_HEIGHT

// Debug
#define undecodable(func, instr) printf("%s: Unrecognized instruction %X\n", func, instr);exit(EXIT_FAILURE)

#endif