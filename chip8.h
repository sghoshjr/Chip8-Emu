#ifndef CHIP8_H
#define CHIP8_H

#include <iostream>
#include <string>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <random>
#include <limits>
#include <functional>
#include <array>

constexpr uint32_t START_ADDRESS = 0x200;
constexpr uint32_t END_ADDRESS = 0xFFF;
constexpr uint32_t FONT_START_ADDRESS = 0x050;
constexpr uint32_t VIDEO_WIDTH = 64U;
constexpr uint32_t VIDEO_HEIGHT = 32U;

class Chip8 {
   private:
    uint8_t registers[16]{};    //Register V0...VF
    uint8_t memory[4096]{};
    uint16_t index{};           //Store Address during operations
    uint16_t pc{};              //Program Counter
    uint16_t stack[16]{};
    uint8_t sp{};               //Stack Pointer
    uint8_t delay_timer{};
    uint8_t sound_timer{};
    uint16_t opcode{};

    uint8_t Vx;                 //_X__
    uint8_t Vy;                 //__Y_
    uint16_t addr;              //_NNN
    uint8_t val;                //__KK
    uint8_t height;             //___N

    //Random Number Generator
    std::default_random_engine rngEngine;
    std::uniform_int_distribution<uint32_t> randomByte;

    //Function Pointer Table
    using f_ptr = void (Chip8::*)();

    std::array<f_ptr, 0x0F + 1> opcodeTableMaster{};
    std::array<f_ptr, 0x0E + 1> opcodeTable0{};
    std::array<f_ptr, 0x0E + 1> opcodeTable8{};
    std::array<f_ptr, 0x0E + 1> opcodeTableE{};
    std::array<f_ptr, 0x65 + 1> opcodeTableF{};

    std::string BUFFER;

   public:
    uint8_t keypad[16]{};
    uint32_t video_frame[VIDEO_HEIGHT][VIDEO_WIDTH]{};
    bool draw = true;

    Chip8();

    void loadROM(const char *);
    void loadFonts();
    void cycle();
    bool clock_tick();
    void reset();

   private:
    //OPCODES
    void OP_00E0();  //CLS
    void OP_00EE();  //RET from SUBROUTINE
    void OP_1NNN();  //JUMP ADDR
    void OP_2NNN();  //CALL ADDR
    void OP_3XKK();  //SE Vx, byte
    void OP_4XKK();  //SNE Vx, byte
    void OP_5XY0();  //SE Vx, Vy
    void OP_6XKK();  //LD Vx, byte
    void OP_7XKK();  //ADD Vx, byte
    void OP_8XY0();  //LD Vx, Vy
    void OP_8XY1();  //OR Vx, Vy
    void OP_8XY2();  //AND Vx, Vy
    void OP_8XY3();  //XOR Vx, Vy
    void OP_8XY4();  //ADD Vx, Vy - flag
    void OP_8XY5();  //SUB Vx, Vy - flag
    void OP_8XY6();  //SHR Vx
    void OP_8XY7();  //SUBN Vx, Vy
    void OP_8XYE();  //SHL Vx
    void OP_9XY0();  //SNE Vx, Vy
    void OP_ANNN();  //LD I, Addr
    void OP_BNNN();  //JP V0, Addr
    void OP_CXKK();  //RND Vx, byte
    void OP_DXYN();  //DRW Vx, Vy, nibble
    void OP_EX9E();  //SKP Vx
    void OP_EXA1();  //SKNP Vx
    void OP_FX07();  //LD Vx, DT
    void OP_FX0A();  //LD Vx, K
    void OP_FX15();  //LD DT, Vx
    void OP_FX18();  //LD ST, Vx
    void OP_FX1E();  //ADD I, Vx
    void OP_FX29();  //LD F, Vx
    void OP_FX33();  //LD B, Vx
    void OP_FX55();  //LD I, Vx
    void OP_FX65();  //LD Vx, I
    void OP_NULL();  //NOP

    void populateFunctionPtrTable();
    void decodeOpcode0();
    void decodeOpcode8();
    void decodeOpcodeE();
    void decodeOpcodeF();

};

#endif // CHIP8_H