#ifndef CHIP8_CPP
#define CHIP8_CPP

#include "chip8.h"
#include <iostream>
using namespace std;

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

unsigned char fontset[FONTSET_SIZE] =
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


Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Initialize registers
    pc = START_ADDRESS;
    I = START_ADDRESS;
    opcode = 0;
    delay_timer = 0;
    sound_timer = 0;
    sp = 0;

    // Initalize arrays
    memset(V, 0, sizeof(V));
    memset(key, 0, sizeof(key));
    memset(video, 0, sizeof(video));
    memset(stack, 0, sizeof(stack));

    // Load fontset
    for (unsigned int i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<int>(0, 255);

    // Initalize function pointers
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;

}

void Chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL()
{}

void Chip8::ShowState()
{
    
    cout << "PC: 0x" << std::hex << pc << endl;
    cout << "SP: 0x" << std::hex << sp << endl;
    cout << "I: 0x" << std::hex << I << endl;
    cout << "DT: 0x" << std::hex << delay_timer << endl;
    cout << "ST: 0x" << std::hex << sound_timer << endl;
    cout << "OPCODE: 0x" << std::hex << opcode << endl;
    cout << "V[0]: 0x" << std::hex << V[0] << endl;
    cout << "V[1]: 0x" << std::hex << V[1] << endl;
    cout << "V[2]: 0x" << std::hex << V[2] << endl;
    cout << "V[3]: 0x" << std::hex << V[3] << endl;
    cout << "V[4]: 0x" << std::hex << V[4] << endl;
    cout << "V[5]: 0x" << std::hex << V[5] << endl;
    cout << "V[6]: 0x" << std::hex << V[6] << endl;
    cout << "V[7]: 0x" << std::hex << V[7] << endl;
    cout << "V[8]: 0x" << std::hex << V[8] << endl;
    cout << "V[9]: 0x" << std::hex << V[9] << endl;
    cout << "V[A]: 0x" << std::hex << V[0xA] << endl;
    cout << "V[B]: 0x" << std::hex << V[0xB] << endl;
    cout << "V[C]: 0x" << std::hex << V[0xC] << endl;
    cout << "V[D]: 0x" << std::hex << V[0xD] << endl;
    cout << "V[E]: 0x" << std::hex << V[0xE] << endl;
    cout << "V[F]: 0x" << std::hex << V[0xF] << endl;
    cout << endl;
}

void Chip8::LoadROM(const char* filename)
{
    // Open the file
    ifstream file(filename, ios::binary | ios::ate);
    // If file opened allocate buffer and read the file into the buffer
    if (file.is_open())
    {
        streampos size = file.tellg();
        char* buffer = new char[size];
        file.seekg(0, ios::beg);
        file.read(buffer, size);

        // Close file and copy buffer to memory
        file.close();

        for (int i = 0; i < size; i++)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        

        // Free buffer
        delete[] buffer;
    }
    else {
        cout << "rom failed to load." << endl;

        exit(1);
    }
}

void Chip8::Cycle()
{
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc+=2;

    // Decode and execute opcode
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Update timers
    if (delay_timer > 0)
    {
        delay_timer--;
    }

    if (sound_timer > 0)
    {
        sound_timer--;
    }
}

void Chip8::OP_00E0() 
{
    // Clear the display
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    // Return from a subroutine
    pc = stack[sp];
    sp--;
}   

void Chip8::OP_1nnn()
{
    // Jump to location nnn
    short nnn = opcode & 0x0FFFu; // lowest 12 bits
    pc = nnn;
}

void Chip8::OP_2nnn()
{
    // Call subroutine at nnn
    sp++;
    stack[sp] = pc;
    short nnn = opcode & 0x0FFFu;
    pc = nnn;
}

void Chip8::OP_3xkk() 
{
    // Skip next instruction if Vx == kk
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char kk = opcode & 0x00FF;

    if (V[x] == kk) pc+=2;
}

void Chip8::OP_4xkk() 
{
    // Skip next instruction if Vx != kk
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char kk = opcode & 0x00FF;

    if (V[x] != kk) pc+=2;
}

void Chip8::OP_5xy0() 
{
    // Skip next instruction if Vx = Vy
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    if (V[x] == V[y]) pc+=2;
}

void Chip8::OP_6xkk() 
{
    // Put kk in Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char kk = opcode & 0x00FF;

    V[x] = kk;
}

void Chip8::OP_7xkk() 
{
    // Vx += kk
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char kk = opcode & 0x00FF;

    V[x] += kk;
}

void Chip8::OP_8xy0() 
{
    // Vx = Vy
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    V[x] = V[y];
}

void Chip8::OP_8xy1() 
{
    // Vx = Vx OR Vy
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    V[x] |= V[y] ;

    // reset flags register to 0
    V[0xF] = 0;
}

void Chip8::OP_8xy2() 
{
    // Vx = Vx AND Vy
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    V[x] &= V[y];

    // reset flags register to 0
    V[0xF] = 0;
}

void Chip8::OP_8xy3() 
{
    // Vx = Vx XOR Vy
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    V[x] ^= V[y];

    // reset flags register to 0
    V[0xF] = 0;
}

void Chip8::OP_8xy4() 
{
    // Vx = Vx + Vy, VF = carry
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    uint16_t sum = V[x] + V[y];

    if (sum > 255U)
    {
        V[0xF] = 1;
    }
    else
    {
        V[0xF] = 0;
    }

    V[x] = sum & 0xFFu;

}

void Chip8::OP_8xy5() 
{
    // Vx = Vx - Vy, VF = NOT borrow
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = ( opcode & 0x00F0u ) >> 4u;
    V[0xF] = 0;
    if (V[x] > V[y])
    {
        V[0xF] = 1;
    }

    V[x] -= V[y];

}

void Chip8::OP_8xy6() 
{
    /*
    It put the value of VY into VX, and then shifted the value in VX 
    1 bit to the right (8XY6) or left (8XYE). 
    VY was not affected, but the flag register 
    VF would be set to the bit that was shifted out.
    */
    // Vx = Vx SHR 1
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = (opcode & 0x00F0u) >> 4u;

    unsigned char val = V[y];
    V[0xF] = val & 0x01u;
    val >>= 1;
    V[x] = val;
}

void Chip8::OP_8xy7() 
{
    // Vx = Vy - Vx, VF = NOT borrow
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = ( opcode & 0x00F0u ) >> 4u;
    V[0xF] = 0;
    if (V[y] > V[x]) { V[0xF] = 1; }

    V[x] = (V[y] - V[x]);
}

void Chip8::OP_8xyE()
{
    // Vx = Vx SHL 1.
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char y = (opcode & 0x00F0u) >> 4u;

    unsigned char val = V[y];
    V[0xF] = val & 0x01u;
    val <<= 1;

    V[x] = val;

}

void Chip8::OP_9xy0()
{
    // Skip if Vx != Vy
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char y = ( opcode & 0x00F0u ) >> 4u;

    if (V[x] != V[y]) pc+=2;
}

void Chip8::OP_Annn()
{
    // I = nnn
    short nnn = opcode & 0x0FFFu; // lowest 12 bits
    I = nnn;
}

void Chip8::OP_Bnnn()
{
    // PC = nnn + V0
    short nnn = opcode & 0x0FFFu; // lowest 12 bits
    pc = nnn + V[0];
}

void Chip8::OP_Cxkk()
{
    // Vx = random AND kk
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    unsigned char kk = opcode & 0x00FF;

    V[x] = ( static_cast<int>( randByte(randGen) ) & kk) ;
}

void Chip8::OP_Dxyn()
{
    unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    unsigned char Vy = (opcode & 0x00F0u) >> 4u;
    unsigned char height = opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    unsigned char xPos = V[Vx] % VIDEO_WIDTH;
    unsigned char yPos = V[Vy] % VIDEO_HEIGHT;

    V[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        unsigned char spriteByte = memory[I + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            unsigned char spritePixel = spriteByte & (0x80u >> col);
            unsigned int* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            // Sprite pixel is on
            if (spritePixel)
            {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFFFFFFFF)
                {
                    V[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF; 
            }
        }
    }


}

void Chip8::OP_Ex9E()
{
    // Skip if key press of Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    if (key[ V[x] ]) pc+=2;
}

void Chip8::OP_ExA1()
{
    // Skip if no key press of Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    if (!key[ V[x] ]) pc+=2;
}

void Chip8::OP_Fx07()
{
    // Vx = delay timer
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    V[x] = delay_timer;
}

void Chip8::OP_Fx0A()
{
    // Wait for a key press, store the value of the key in Vx.
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    
	if (key[0])
	{
		V[x] = 0;
	}
	else if (key[1])
	{
		V[x] = 1;
	}
	else if (key[2])
	{
		V[x] = 2;
	}
	else if (key[3])
	{
		V[x] = 3;
	}
	else if (key[4])
	{
		V[x] = 4;
	}
	else if (key[5])
	{
		V[x] = 5;
	}
	else if (key[6])
	{
		V[x] = 6;
	}
	else if (key[7])
	{
		V[x] = 7;
	}
	else if (key[8])
	{
		V[x] = 8;
	}
	else if (key[9])
	{
		V[x] = 9;
	}
	else if (key[10])
	{
		V[x] = 10;
	}
	else if (key[11])
	{
		V[x] = 11;
	}
	else if (key[12])
	{
		V[x] = 12;
	}
	else if (key[13])
	{
		V[x] = 13;
	}
	else if (key[14])
	{
		V[x] = 14;
	}
	else if (key[15])
	{
		V[x] = 15;
	}
	else
	{
		pc -= 2;
	}

}

void Chip8::OP_Fx15()
{
    // DT = Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    delay_timer = V[x];
}

void Chip8::OP_Fx18()
{
    // ST = Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    sound_timer = V[x];
}

void Chip8::OP_Fx1E()
{
    // I += Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    I += V[x];
}

void Chip8::OP_Fx29()
{
    // I = sprite address for Vx
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
    I = FONTSET_START_ADDRESS + V[x] * 5;
}

void Chip8::OP_Fx33()
{
    // I = BCD(Vx)
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;
  unsigned char value = V[x];
    // Ones-place
    memory[I + 2] = value % 10;
    value /= 10;

    // Tens-place
    memory[I + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    memory[I] = value % 10;

}

void Chip8::OP_Fx55()
{
    // Store V(0->x) in memory starting from I
  unsigned char x = ( opcode & 0x0F00u ) >> 8u;

    for (int i=0; i<=x; i++)
    {
        memory[I+i] = V[i];
    }

    // increment index register
    I++;
}

void Chip8::OP_Fx65()
{
    // Store mem(I->I+x) in registers starting from V0
    unsigned char x = ( opcode & 0x0F00u ) >> 8u;

    for (int i=0; i<=x; i++)
    {
        V[i] = memory[I + i];
    }

    I++;
}


#endif // CHIP8_CPP