#include "chip8.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Chip-8'in 0'dan F'ye kadar olan karakterlerinin pikselleri (Fontset)
uint8_t fontset[80] = {
    0xF0,0x90,0x90,0x90,0xF0, 0x20,0x60,0x20,0x20,0x70, 0xF0,0x10,0xF0,0x80,0xF0, 0xF0,0x10,0xF0,0x10,0xF0,
    0x90,0x90,0xF0,0x10,0x10, 0xF0,0x80,0xF0,0x10,0xF0, 0xF0,0x80,0xF0,0x90,0xF0, 0xF0,0x10,0x20,0x40,0x40,
    0xF0,0x90,0xF0,0x90,0xF0, 0xF0,0x90,0xF0,0x10,0xF0, 0xF0,0x90,0xF0,0x90,0x90, 0xE0,0x90,0xE0,0x90,0xE0,
    0xF0,0x80,0x80,0x80,0xF0, 0xE0,0x90,0x90,0x90,0xE0, 0xF0,0x80,0xF0,0x80,0xF0, 0xF0,0x80,0xF0,0x80,0x80
};


void init_chip8(Chip8CPU* cpu){
    // Tum sistemi sifirlar. (Registers, bellek, ekran)
    memset(cpu, 0, sizeof(Chip8CPU));

    cpu->PC = 0x200;    //program counter'ı 0x200 adresine ayarla

    //Fontset'i belleğin başlarına (0x050 adresinden itibaren) kopyala
    for(int i = 0; i < 80; i++){
        cpu->memory[0x050 + i] = fontset[i];
    }
}

bool load_roam(Chip8CPU* cpu, const char* filename){
    //Dosyayi binary formatta okuma modunda (rb-read binary) ac.
    FILE* file = fopen(filename, "rb");
    if(file == NULL){
        printf("Hata: %s dosyasi acilmadi!\n", filename);
        return false;
    }

    //Dosya boyutu hesaplama
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    //dosya bellege sigar mi? kontrol
    //bellek 4096bayt, programlar 0x200(512) adresinden basliyor.
    if(file_size > 4096- 0x200){
        printf("Hata: ROM dosyasi hafiza icin cok buyuk!\n");
        fclose(file);
        return false;
    }

    //dosyadaki verileri bellege kopyala
    //fread(veriyi nereye yazacagim, her bir verinin boyutu, toplam kac veri okuyacagim, hangi dosyadan okuyacagim)
    fread(&cpu->memory[0x200], 1, file_size, file);
    fclose(file);
    return true;
}

void emulate_cycle(Chip8CPU* cpu) {
    //1)FETCH (getir): Bellekten 2 byte'lik komutu oku
    //memory[PC] ilk byte'ı, memory[PC+1] ikinci byte'ı gosterir
    uint16_t opcode =(cpu->memory[cpu->PC] << 8) | cpu->memory[cpu->PC + 1];
    
    //sonraki komut icin program counter ı 2 arttır
    cpu->PC += 2;

    //2)DECODE & EXECUTE (coz ve calistir)
    //opcode un sadece ilk hanesi(kategori) okumak icin 0xF00 ile maskele
    //bitwise AND (maskeleme) 0xF000 sadece ilk haneyi göster, gerisini gizle.
    switch (opcode & 0xF000) {
        case 0xA000:
            cpu->I = opcode &0x0FFF;
            break;
        default:
        printf("bilinmeyen ve ya henuz yazilmayan opcode: 0x%X\n", opcode);
        break;        
    }
}

