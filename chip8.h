#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t V[16];          //16 register
    uint16_t I;             //index register
    uint16_t PC;            //program counter
    uint16_t SP;            //stack pointer
    uint16_t stack[16];     //call stack
    uint8_t delay_timer;    //60hz gecikme
    uint8_t sound_timer;    //60hz ses gecikmesi    

    uint8_t memory[4096];       // 4KB ram
    uint8_t display[64 * 32];   // 64x32 piksel ekran
    bool keypad[16];            // 16 tane keys

} Chip8CPU; 

void init_chip8(Chip8CPU* cpu);
bool load_roam(Chip8CPU* cpu, const char* filename);

#endif 