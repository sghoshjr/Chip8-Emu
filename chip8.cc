#include "chip8.h"

// Constructor
Chip8::Chip8() : rngEngine(std::chrono::system_clock::now().time_since_epoch().count()) {
    pc = START_ADDRESS;
    loadFonts();

    randomByte = std::uniform_int_distribution<uint32_t>(0, (std::numeric_limits<uint8_t>::max)());
    populateFunctionPtrTable();
}

/**
 * Load CHIP8 ROM in 0x200-0xFFF memory section (2 byte OPCODE)
 */
void Chip8::loadROM(const char *fname) {
    std::ifstream file(fname, std::ios::binary);

    if (file.is_open()) {
        auto size = std::filesystem::file_size(fname);

        if (size > (END_ADDRESS - START_ADDRESS)) {
            std::cout << "ERROR : File size too big " << fname << std::endl;
            file.close();
            return;
        }

        BUFFER.resize(size);

        file.read(BUFFER.data(), size);
        std::cout << "ROM Size : " << size << " bytes" << std::endl;

        std::memcpy(&memory[START_ADDRESS], BUFFER.data(), size);
        file.close();
    } else {
        std::cout << "ERROR : Cannot load ROM " << fname << std::endl;
    }
}


/**
 * Loads Fonts into Memory [0x050-0x0A0]
 */
void Chip8::loadFonts() {
    constexpr uint32_t FONT_SZ = 16 * 5;

    uint8_t fonts[FONT_SZ] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // F
    };

    for (int i = 0; i < FONT_SZ; ++i) {
        memory[FONT_START_ADDRESS + i] = fonts[i];
    }
}

// Execute the Fetch, Decode, Execute cycle
void Chip8::cycle() {
    // Fetch
    opcode = ((uint16_t)memory[pc] << 8U) | (memory[pc + 1]);
    // printRed(opcode);

    Vx = (opcode & 0x0F00U) >> 0x08U;  //_X__
    Vy = (opcode & 0x00F0U) >> 0x04U;  //__Y_
    addr = (opcode & 0x0FFFU);         //_NNN
    val = (opcode & 0x00FFU);          //__KK
    height = (opcode & 0x000FU);       //___N

    // Increment Program Counter
    pc += 0x02U;

    // Decode and Execute
    std::invoke(opcodeTableMaster[(opcode & 0xF000U) >> 0x0CU], *this);
}

// Update Delay and Sound Timer
bool Chip8::clock_tick() {
    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        --sound_timer;
        if (sound_timer == 0) {
            // printRed("Beep!");
            return true;
        }
    }
    return false;
}

// Reset
void Chip8::reset() {
    std::memset(registers, 0, sizeof(registers));
    std::memset(memory, 0, sizeof(memory));
    loadFonts();
    std::memcpy(&memory[START_ADDRESS], BUFFER.data(), BUFFER.size());
    index = 0;
    std::memset(stack, 0, sizeof(stack));
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    std::memset(keypad, 0, sizeof(keypad));
    std::memset(video_frame, 0, sizeof(video_frame));
    opcode = 0;
    draw = true;
    pc = START_ADDRESS;
}

// OPCODES

// Clear the Display
void Chip8::OP_00E0() {
    std::memset(video_frame, 0, sizeof(video_frame));
    draw = true;
}

// Return from a subroutine
void Chip8::OP_00EE() {
    --sp;
    pc = stack[sp];
}

// Jump to Address
void Chip8::OP_1NNN() {
    pc = addr;
}

// Call Address
void Chip8::OP_2NNN() {
    stack[sp++] = pc;
    pc = addr;
}

// Skip Instruction if Vx == KK
void Chip8::OP_3XKK() {
    if (registers[Vx] == val) {
        pc += 0x02U;
    }
}

// Skip Instruction if Vx != KK
void Chip8::OP_4XKK() {
    if (registers[Vx] != val) {
        pc += 0x02U;
    }
}

// Skip Instruction if Vx == Vy
void Chip8::OP_5XY0() {
    if (registers[Vx] == registers[Vy]) {
        pc += 0x02U;
    }
}

// Set Vx = KK
void Chip8::OP_6XKK() {
    registers[Vx] = val;
}

// Set Vx = Vx + KK
void Chip8::OP_7XKK() {
    registers[Vx] += val;
}

// Set Vx = Vy
void Chip8::OP_8XY0() {
    registers[Vx] = registers[Vy];
}

// Set Vx = Vx OR Vy
void Chip8::OP_8XY1() {
    registers[Vx] |= registers[Vy];
}

// Set Vx = Vx AND Vy
void Chip8::OP_8XY2() {
    registers[Vx] &= registers[Vy];
}

// Set Vx = Vx XOR Vy
void Chip8::OP_8XY3() {
    registers[Vx] ^= registers[Vy];
}

// Set Vx = Vx + Vy, VF = 1 if overflow
void Chip8::OP_8XY4() {
    uint16_t sum = registers[Vx] + registers[Vy];

    registers[0x0F] = (sum > 255U) ? 1 : 0;
    registers[Vx] = sum & 0x00FFU;
}

// Set Vx = Vx - Vy, VF = 1 if not borrow
void Chip8::OP_8XY5() {
    registers[0x0F] = (registers[Vx] > registers[Vy]) ? 1 : 0;
    registers[Vx] -= registers[Vy];
}

// Set Vx = Vx SHR 1
void Chip8::OP_8XY6() {
    registers[0x0F] = registers[Vx] & 0x01U;
    registers[Vx] >>= 1;
}

// Set Vx = Vy - Vx, VF = 1 if not borrow
void Chip8::OP_8XY7() {
    registers[0x0F] = (registers[Vy] > registers[Vx]) ? 1 : 0;
    registers[Vx] = registers[Vy] - registers[Vx];
}

// Set Vx = Vx SHL 1
void Chip8::OP_8XYE() {
    registers[0x0F] = (registers[Vx] & 0x80) >> 0x07U;
    registers[Vx] <<= 1;
}

// Skip Instruction if Vx != Vy
void Chip8::OP_9XY0() {
    if (registers[Vx] != registers[Vy]) {
        pc += 0x02U;
    }
}

// Set I = Addr
void Chip8::OP_ANNN() {
    index = addr;
}

// Jump to Addr + V0
void Chip8::OP_BNNN() {
    pc = (addr) + registers[0x00];
}

// Set Vx = RND AND KK
void Chip8::OP_CXKK() {
    registers[Vx] = static_cast<uint8_t>(randomByte(rngEngine)) & val;
}

/**
 * Draw at position Vx, Vy
 * Read from Memory[i] (N bytes)
 * 8-Bit-Encoded Sprites of height N, col=8
 */
void Chip8::OP_DXYN() {
    registers[0x0F] = 0;
    uint8_t x_pos = registers[Vx] % VIDEO_WIDTH;
    uint8_t y_pos = registers[Vy] % VIDEO_HEIGHT;

    for (int r = 0; r < height; ++r) {
        uint8_t sprite_row = memory[index + r];
        for (int c = 0; c < 8; ++c) {
            uint8_t sprite_pixel = sprite_row & (0x80U >> c);
            uint32_t *pixel = &video_frame[(y_pos + r) % VIDEO_HEIGHT][(x_pos + c) % VIDEO_WIDTH];

            if (sprite_pixel) {
                if (*pixel) {
                    registers[0x0F] = 1;
                }
                *pixel ^= 0xFFFFFFFF;
            }
        }
    }
    draw = true;
}

// Skip Instruction if Key with val(Vx) is pressed
void Chip8::OP_EX9E() {
    if (keypad[registers[Vx]]) {
        pc += 0x02U;
    }
}

// Skip Instruction if key with val(Vx) is not pressed
void Chip8::OP_EXA1() {
    if (!keypad[registers[Vx]]) {
        pc += 0x02U;
    }
}

// Set Vx = Delay Timer
void Chip8::OP_FX07() {
    registers[Vx] = delay_timer;
}

// Wait for keypress - store in Vx
void Chip8::OP_FX0A() {
    std::cout << "Waiting for Keypress...\n";

    for (int i = 0; i < 16; ++i) {
        if (keypad[i]) {
            registers[Vx] = i;
            return;
        }
    }
    pc -= 0x02U;
}

// Set Delay Timer = Vx
void Chip8::OP_FX15() {
    delay_timer = registers[Vx];
}

// Set Sound Timer = Vx
void Chip8::OP_FX18() {
    sound_timer = registers[Vx];
}

// Set I = I + Vx
void Chip8::OP_FX1E() {
    registers[0x0F] = (index + registers[Vx] > 0x0FFFU) ? 1 : 0;
    index += registers[Vx];
}

// Set I = Address sprite of digit Vx
void Chip8::OP_FX29() {
    index = FONT_START_ADDRESS + registers[Vx] * 0x05U;
}

// Store BCD representation of Vx in I, I+1, I+2
void Chip8::OP_FX33() {
    uint8_t val = registers[Vx];

    for (int i = 0; i < 3; ++i) {
        memory[index + 2 - i] = val % 10;
        val /= 10;
    }
}

// Store [V0-Vx] in memory[I]
void Chip8::OP_FX55() {
    for (int i = 0; i <= Vx; ++i) {
        memory[index + i] = registers[i];
    }
}

// Load [V0-Vx] from memory[I]
void Chip8::OP_FX65() {
    for (int i = 0; i <= Vx; ++i) {
        registers[i] = memory[index + i];
    }
}

// NOP
void Chip8::OP_NULL() {
    // printRed("NOP");
    return;
}

// Populate Function Pointer Table
void Chip8::populateFunctionPtrTable() {
    opcodeTableMaster.fill(&Chip8::OP_NULL);
    opcodeTable0.fill(&Chip8::OP_NULL);
    opcodeTable8.fill(&Chip8::OP_NULL);
    opcodeTableE.fill(&Chip8::OP_NULL);
    opcodeTableF.fill(&Chip8::OP_NULL);

    opcodeTableMaster[0x0] = &Chip8::decodeOpcode0;
    opcodeTableMaster[0x1] = &Chip8::OP_1NNN;
    opcodeTableMaster[0x2] = &Chip8::OP_2NNN;
    opcodeTableMaster[0x3] = &Chip8::OP_3XKK;
    opcodeTableMaster[0x4] = &Chip8::OP_4XKK;
    opcodeTableMaster[0x5] = &Chip8::OP_5XY0;
    opcodeTableMaster[0x6] = &Chip8::OP_6XKK;
    opcodeTableMaster[0x7] = &Chip8::OP_7XKK;
    opcodeTableMaster[0x8] = &Chip8::decodeOpcode8;
    opcodeTableMaster[0x9] = &Chip8::OP_9XY0;
    opcodeTableMaster[0xA] = &Chip8::OP_ANNN;
    opcodeTableMaster[0xB] = &Chip8::OP_BNNN;
    opcodeTableMaster[0xC] = &Chip8::OP_CXKK;
    opcodeTableMaster[0xD] = &Chip8::OP_DXYN;
    opcodeTableMaster[0xE] = &Chip8::decodeOpcodeE;
    opcodeTableMaster[0xF] = &Chip8::decodeOpcodeF;

    opcodeTable0[0x0] = &Chip8::OP_00E0;
    opcodeTable0[0xE] = &Chip8::OP_00EE;

    opcodeTable8[0x0] = &Chip8::OP_8XY0;
    opcodeTable8[0x1] = &Chip8::OP_8XY1;
    opcodeTable8[0x2] = &Chip8::OP_8XY2;
    opcodeTable8[0x3] = &Chip8::OP_8XY3;
    opcodeTable8[0x4] = &Chip8::OP_8XY4;
    opcodeTable8[0x5] = &Chip8::OP_8XY5;
    opcodeTable8[0x6] = &Chip8::OP_8XY6;
    opcodeTable8[0x7] = &Chip8::OP_8XY7;
    opcodeTable8[0xE] = &Chip8::OP_8XYE;

    opcodeTableE[0xE] = &Chip8::OP_EX9E;
    opcodeTableE[0x1] = &Chip8::OP_EXA1;

    opcodeTableF[0x07] = &Chip8::OP_FX07;
    opcodeTableF[0x0A] = &Chip8::OP_FX0A;
    opcodeTableF[0x15] = &Chip8::OP_FX15;
    opcodeTableF[0x18] = &Chip8::OP_FX18;
    opcodeTableF[0x1E] = &Chip8::OP_FX1E;
    opcodeTableF[0x29] = &Chip8::OP_FX29;
    opcodeTableF[0x33] = &Chip8::OP_FX33;
    opcodeTableF[0x55] = &Chip8::OP_FX55;
    opcodeTableF[0x65] = &Chip8::OP_FX65;
}

void Chip8::decodeOpcode0() {
    std::invoke(opcodeTable0[opcode & 0x000FU], this);
}

void Chip8::decodeOpcode8() {
    std::invoke(opcodeTable8[opcode & 0x000FU], this);
}

void Chip8::decodeOpcodeE() {
    std::invoke(opcodeTableE[opcode & 0x000FU], this);
}

void Chip8::decodeOpcodeF() {
    std::invoke(opcodeTableF[opcode & 0x00FFU], this);
}