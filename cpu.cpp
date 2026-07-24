#include "cpu.hpp"

unsigned short Cpu::fetch()
{
    unsigned short instr;
    instr = mem[pc];
    instr = instr << 8;
    instr = instr | mem[pc + 1];

    pc += 2;

    return instr;
}

void Cpu::decode(unsigned short instr)
{
    unsigned char nib = instr >> 12; // First nibble

    unsigned char x = (instr >> 8) & 0x0f;
    unsigned char y = (instr >> 4) & 0x0f;
    unsigned char n = instr & 0x0f;
    unsigned char nn = instr;
    unsigned short nnn = instr & 0x0fff;

    switch (nib)
    {
    case 0x0:
        decode_0(instr);
        break;
    case 0x1:
        // jmp nnn
        pc = nnn;
        break;
    case 0x2:
        // jsr nnn - jump to subroutine at address nnn
        stack.push(pc);
        pc = nnn;
        break;
    case 0x3:
        // skeq vx, nn - skip if register vx == constant
        if (vreg[x] == nn)
            pc += 2;

        break;
    case 0x4:
        // skne vx, nn - skip if register vx <> constant
        if (vreg[x] != nn)
            pc += 2;

        break;
    case 0x5:
        // skeq vx,vy - skip if register vx == vy
        if (vreg[x] == vreg[y])
            pc += 2;

        break;
    case 0x6:
        // set vx to nn
        vreg[x] = nn;
        break;
    case 0x7:
        // add vx += nn
        vreg[x] += nn;
        break;
    case 0x8:
        decode_8(instr);
        break;
    case 0x9:
        // skne vx,vy - skip if register vx <> vy
        if (vreg[x] != vreg[y])
            pc += 2;

        break;
    case 0xa:
        // mvi nnn --- I := nnn
        I_reg = nnn;
        break;
    case 0xb:
#ifdef ORIGINAL_CHIP
        // jmi nnn --- pc := V0 + nnn
        pc = vreg[0x0] + nnn;

#else // SUPER_CHIP
      // jmi vx, nnn !!! pc := VX + nnn
        pc = vreg[x] + nnn;
#endif
        break;
    case 0xc:
        // rand vx, nn --- vx := random number [0-255] & nn
        vreg[x] = (rand() % 256) & nn;
        break;
    case 0xd:
        // sprite vx, vy, n --- Display. vf = 1 on collision
        display_sprite(vreg[x], vreg[y], n);
        break;
    case 0xe:
        decode_e(instr);
        break;
    case 0xf:
        decode_f(instr);
        break;

    default:
        undecodable("decode", instr);
        break;
    }
}

void Cpu::decode_0(unsigned short instr)
{
    switch (instr)
    {
    case 0x00e0:
        // cls - clear the screen
        {
            uint8_t *pixels = (uint8_t *)surface->pixels;
            memset(pixels, 0, surface->pitch * surface->h);
            // render();
            break;
        }

    case 0x00ee:
        // rts - return from subroutine call
        pc = stack.top();
        stack.pop();
        break;

    default:
        undecodable("decode_0", instr);
        break;
    }
}

void Cpu::decode_8(unsigned short instr)
{
    unsigned char x = (instr >> 8) & 0x0f;
    unsigned char y = (instr >> 4) & 0x0f;
    unsigned char op = instr & 0x0f;

    switch (op)
    {
    case 0x0:
        // mov vx, vy
        vreg[x] = vreg[y];
        break;
    case 0x1:
        // or vx, vy
        vreg[x] = vreg[x] | vreg[y];
        break;
    case 0x2:
        // and vx, vy
        vreg[x] = vreg[x] & vreg[y];
        break;
    case 0x3:
        // xor vx, vy
        vreg[x] = vreg[x] ^ vreg[y];
        break;
    case 0x4:
        // add vx, vy
        vreg[x] += vreg[y];
        vreg[0xf] = vreg[x] < vreg[y];
        break;
    case 0x5:
        // sub vx, vy --- sets vf if there's NOT a borrow
        {
            unsigned char res = vreg[x] - vreg[y];
            unsigned char borrow = res <= vreg[x];
            vreg[x] = res;
            vreg[0xf] = borrow;
            break;
        }
    case 0x6:
        // shr vx, vy
        vreg[x] = vreg[y];
        vreg[x] >>= 1;
        vreg[0xf] = vreg[y] & 1;
        break;

    case 0x7:
        // rsb vx, vy --- sets vf if there's NOT a borrow
        vreg[x] = vreg[y] - vreg[x];
        vreg[0xf] = vreg[x] <= vreg[y];
        break;
    case 0xe:
        // shl vx, vy

        vreg[x] = vreg[y];
        vreg[x] <<= 1;
        vreg[0xf] = vreg[y] & 1;
        break;

    default:
        undecodable("decode_8", instr);
        break;
    }
}

// Skip if key instructions
void Cpu::decode_e(unsigned short instr)
{
    unsigned char x = (instr >> 8) & 0x0f;
    unsigned char op = instr;

    const bool *key_states = SDL_GetKeyboardState(NULL);
    SDL_Scancode code = getScanCode(vreg[x]);
    switch (op)
    {
    case 0x9e:
        // skpr vx - skip if key in vx is pressed
        if (key_states[code])
            pc += 2;

        break;
    case 0xa1:
        // skup vx - skip if key in vx is not pressed
        if (!key_states[code])
            pc += 2;
        break;

    default:
        undecodable("decode_e", instr);
        break;
    }
}

// There are timer instructions
void Cpu::decode_f(unsigned short instr)
{
    unsigned char x = (instr >> 8) & 0x0f;
    unsigned char op = instr;
    switch (op)
    {
    case 0x07:
        SDL_LockMutex(timer_mutex);
        vreg[x] = delay_timer;
        SDL_UnlockMutex(timer_mutex);
        break;

    case 0x15:
        SDL_LockMutex(timer_mutex);
        delay_timer = vreg[x];
        SDL_UnlockMutex(timer_mutex);
        break;

    case 0x18:
        SDL_LockMutex(timer_mutex);
        sound_timer = vreg[x];
        SDL_UnlockMutex(timer_mutex);
        break;

    case 0x1e:
        // i reg += vx
        {
            unsigned short res = I_reg + vreg[x];
            vreg[0xf] = I_reg & 0x0fff < res & 0x0fff;
            I_reg = res;
            break;
        }

    case 0x0a:
        // Get key
        SDL_LockMutex(input_mutex);
        awaiting_input = true;
        if (!recieved_input)
        {
            pc -= 2;
        }
        else
        {
            vreg[x] = key_pressed;
            recieved_input = false;
            awaiting_input = false;
        }
        SDL_UnlockMutex(input_mutex);
        break;

    case 0x29:
        // font vx - sets register i to point to the sprite character vx
        I_reg = FONT_ADDRESS + vreg[x] * FONT_HEIGHT;
        break;

    case 0x33:
        // bcd vx - Decode vx into binary-coded decimal stored in I, I+1, I+2
        mem[I_reg] = vreg[x] / 100;
        mem[I_reg + 1] = (vreg[x] / 10) % 10;
        mem[I_reg + 2] = vreg[x] % 10;
        break;

    case 0x55:
        // Store registers from v0 to vx (included)
        for (int i = 0; i <= x; i++)
            mem[I_reg + i] = vreg[i];
        break;

    case 0x65:
        // Load registers from v0 to vx (included)
        for (int i = 0; i <= x; i++)
            vreg[i] = mem[I_reg + i];
        break;

    default:
        undecodable("decode_f", instr);
        break;
    }
}

SDL_Scancode Cpu::getScanCode(unsigned char key)
{
    // Layout:
    /*
    1 2 3 C
    4 5 6 D
    7 8 9 E
    A 0 B F
    */
    switch (key)
    {
    case 0x0:
        return SDL_SCANCODE_X;
        break;
    case 0x1:
        return SDL_SCANCODE_1;
        break;
    case 0x2:
        return SDL_SCANCODE_2;
        break;
    case 0x3:
        return SDL_SCANCODE_3;
        break;
    case 0x4:
        return SDL_SCANCODE_Q;
        break;
    case 0x5:
        return SDL_SCANCODE_W;
        break;
    case 0x6:
        return SDL_SCANCODE_E;
        break;
    case 0x7:
        return SDL_SCANCODE_A;
        break;
    case 0x8:
        return SDL_SCANCODE_S;
        break;
    case 0x9:
        return SDL_SCANCODE_D;
        break;
    case 0xa:
        return SDL_SCANCODE_Z;
        break;
    case 0xb:
        return SDL_SCANCODE_C;
        break;
    case 0xc:
        return SDL_SCANCODE_4;
        break;
    case 0xd:
        return SDL_SCANCODE_R;
        break;
    case 0xe:
        return SDL_SCANCODE_F;
        break;
    case 0xf:
        return SDL_SCANCODE_V;
        break;
    }

    printf("Unexpected decode value: %d\n", key);
    exit(EXIT_FAILURE);
    return SDL_Scancode();
}

void Cpu::display_sprite(unsigned char x, unsigned char y, unsigned char h)
{
    x %= 64;
    y %= 32;
    vreg[0xf] = 0;

    for (int i = 0; i < h && y + i < 32; i++)
    {
        const unsigned char byte = mem[I_reg + i];
        for (int j = 0; j < 8 && j + x < 64; j++)
        {
            const unsigned char bit = (byte >> (7 - j)) & 0x1; // Gets the bit for the position
            unsigned char x_fin = x + j, y_fin = y + i;
            uint8_t *pixels = (uint8_t *)surface->pixels;

            vreg[0xf] |= pixels[y_fin * surface->pitch + x_fin] & bit; // Collision detection
            pixels[y_fin * surface->pitch + x_fin] = pixels[y_fin * surface->pitch + x_fin] ^ bit;
            // screen[x + j][y + i] = screen[x + j][y + i] ^ bit;
        }
    }

    // render();
}

void Cpu::init_fonts()
{
    // Hope the compiler optimizes this mess
    unsigned char font[] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    const int fonts_size = 5 * 16;

    for (int i = 0; i < fonts_size; i++)
    {
        mem[FONT_ADDRESS + i] = font[i];
    }
}

int Cpu::cpu_main()
{
    init_fonts();
    srand(time(NULL));

    CpuThreadPayload *payload = new CpuThreadPayload {this, timers_func};
    SDL_Thread *timers_thread = SDL_CreateThread(genericThreadCallback, "timers_thread", payload);

    // Frequency should be 1MHz
    static constexpr double PERIOD_NS = 1600000;

    while (running)
    {
        Uint64 t0 = SDL_GetTicksNS();

        unsigned short instr = fetch();
        decode(instr);

        Uint64 t1 = SDL_GetTicksNS();
        double elapsed = static_cast<double>(t1 - t0);
        if (elapsed < PERIOD_NS)
            SDL_DelayNS(static_cast<Uint32>(PERIOD_NS - elapsed));
    }

    return 0;
}

int Cpu::timers_func()
{
    // They run at 60Hz
    static constexpr double PERIOD_NS = 1'000'000'000 / 60;
    timer_mutex = SDL_CreateMutex();

    while (running)
    {
        Uint64 t0 = SDL_GetTicksNS();

        SDL_LockMutex(timer_mutex);
        if (delay_timer > 0)
            delay_timer--;
        if (sound_timer > 0)
            sound_timer--;
        SDL_UnlockMutex(timer_mutex);

        Uint64 t1 = SDL_GetTicksNS();
        double elapsed = static_cast<double>(t1 - t0);
        if (elapsed < PERIOD_NS)
            SDL_DelayNS(static_cast<Uint32>(PERIOD_NS - elapsed));
    }

    SDL_DestroyMutex(timer_mutex);
    return 0;
}

int SDLCALL Cpu::genericThreadCallback(void *data)
{
    CpuThreadPayload *payload = static_cast<CpuThreadPayload*>(data);
    
    Cpu *self = payload->self;
    int (Cpu::*method)() = payload->func;
    
    delete payload;

    return (self->*method)();
}
