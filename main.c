#include <stdio.h>
#include "chip8.h"

int main() {
    Chip8CPU cpu;

    //1 islemciyi baslat (PC'yi 0x200'e ayarla)
    init_chip8(&cpu);

    //2 hafizanin baslangic noktasina test kodu
    //komut: 0xA123 yani Index register ına 0x123 degerini ata
    cpu.memory[0x200] = 0xA1;
    cpu.memory[0x201] = 0x23;
    //registerlara elle yazdık 0xA123 

    printf("calismadan once:\n");
    printf("PC: 0x%X\n\n", cpu.PC, cpu.I);
    printf("PC: 0x%X, I: 0x%X\n\n", cpu.PC, cpu.I);

    //3 islemci dongusu sadece 1 kez calistir
    emulate_cycle(&cpu);

    //4 sonuclari kontrol et
    printf("calistiktan sonra:\n");
    printf("PC: 0x%X (Beklenen: 0x202)\n", cpu.PC);
    printf("I: 0x%X (Beklenen: 0x123)\n", cpu.I);

    return 0;
}