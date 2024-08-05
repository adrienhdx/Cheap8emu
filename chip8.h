#ifndef CHIP8_H
#define CHIP8_H

#include <chrono>
#include <random>
#include <fstream>

class Chip8 {
    public:
        Chip8();
        void LoadROM(const char *filename);
        void Cycle();
        unsigned char key[16];          // keypad
        unsigned int video[64 * 32];    // 64x32 monochrome display, 32 bits to work with SDL
        void ShowState();


    private:

        // short : 16 bits
        // char : 8 bits
        // int : 32 bits
        unsigned char memory[4096];     // 4KB memory
        unsigned char V[16];            // registers
        unsigned short I;               // index register
        unsigned char delay_timer;      // delay register, decreased at 60Hz ifnz
        unsigned char sound_timer;      // sound register, decreased at 60Hz ifnz
        unsigned short pc;              // program counter
        unsigned char sp;               // stack pointer
        unsigned short stack[16];       // stack
        unsigned short opcode;          // current opcode

        const unsigned int START_ADDRESS = 0x200; // 512
        const unsigned int FONTSET_SIZE = 80;
        const unsigned int FONTSET_START_ADDRESS = 0x50;

        std::default_random_engine randGen;
        std::uniform_int_distribution<int> randByte;



        void Table0();
        void Table8();
        void TableE();
        void TableF();

        typedef void (Chip8::*Chip8Func)();
        Chip8Func table[0xF + 1];
        Chip8Func table0[0xE + 1];
        Chip8Func table8[0xE + 1];
        Chip8Func tableE[0xE + 1];
        Chip8Func tableF[0x65 + 1];


        // Opcode functions
	    void OP_NULL(); // Do nothing
        void OP_00E0(); // Clear the display
        void OP_00EE(); // Return from a subroutine
        void OP_1nnn(); // Jump to location nnn
        void OP_2nnn(); // Call subroutine at nnn
        void OP_3xkk(); // Skip next instruction if Vx = kk
        void OP_4xkk(); // Skip next instruction if Vx != kk
        void OP_5xy0(); // Skip next instruction if Vx = Vy
        void OP_6xkk(); // Set Vx = kk
        void OP_7xkk(); // Set Vx = Vx + kk
        void OP_8xy0(); // Set Vx = Vy
        void OP_8xy1(); // Set Vx = Vx OR Vy
        void OP_8xy2(); // Set Vx = Vx AND Vy
        void OP_8xy3(); // Set Vx = Vx XOR Vy
        void OP_8xy4(); // Set Vx = Vx + Vy, set VF = carry
        void OP_8xy5(); // Set Vx = Vx - Vy, set VF = NOT borrow
        void OP_8xy6(); // Set Vx = Vx SHR 1
        void OP_8xy7(); // Set Vx = Vy - Vx, set VF = NOT borrow
        void OP_8xyE(); // Set Vx = Vx SHL 1
        void OP_9xy0(); // Skip next instruction if Vx != Vy
        void OP_Annn(); // Set I = nnn
        void OP_Bnnn(); // Jump to location nnn + V0
        void OP_Cxkk(); // Set Vx = random byte AND kk
        void OP_Dxyn(); // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        void OP_Ex9E(); // Skip next instruction if key with the value of Vx is pressed
        void OP_ExA1(); // Skip next instruction if key with the value of Vx is not pressed
        void OP_Fx07(); // Set Vx = delay timer value
        void OP_Fx0A(); // Wait for a key press, store the value of the key in Vx
        void OP_Fx15(); // Set delay timer = Vx
        void OP_Fx18(); // Set sound timer = Vx
        void OP_Fx1E(); // Set I = I + Vx
        void OP_Fx29(); // Set I = location of sprite for digit Vx
        void OP_Fx33(); // Store BCD representation of Vx in memory locations I, I+1, and I+2
        void OP_Fx55(); // Store registers V0 through Vx in memory starting at location I
        void OP_Fx65(); // Read registers V0 through Vx from memory starting at location I

        
};

#endif // CHIP8_H