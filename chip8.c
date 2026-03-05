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

        case 0x0000:
            if(opcode == 0x00E0){
                //0x00E0: ekrani temizler 
                memset(cpu->display, 0, sizeof(cpu->display));
            }
            break;

        case 0x1000:
            //1NNN: program counter'i NNN adresine eşitle
            cpu->PC = opcode & 0x0FFF;
            break;

        case 0xA000:
            cpu->I = opcode & 0x0FFF;
            break;
        
        //6XNN: VX register ina NN degerini ata (Ornek: V0 = 25)
        case 0x6000:
            cpu->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;

        //7XNN: VX register ina NN degerini ekle (ornek: V0 = V0 + 5)
        case 0x7000:
            cpu->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;

        //DXYN: Ekran grafik (sprite) cizme komutu
        case 0xD000: {
            // X ve Y kordinatlarini registerlardan al.
            // %64 ve %32 ile ekran sinirleari icinde kalmasini sagla.
            uint8_t x = cpu->V[(opcode & 0xF000) >> 8] % 64;
            uint8_t y = cpu->V[(opcode & 0xF000) >> 4] % 32;
            uint8_t height = opcode & 0x000F; //Seklin yuksekligi (satir sayisi)

            //VF registerini (Carpisma bayragi) once 0 yap
            cpu->V[0xF] = 0;

            //Satirlari tek tek don
            for(int row = 0; row < height; row++){
                //I registerinin gosterdigi adresten 1 baytlik piksel verisini ak
                uint8_t sprite_byte = cpu->memory[cpu->I + row];

                //1 bayt in icindeki 8 biti (sutun tektek don)
                for(int col = 0; col < 8 ; col++){
                    //eger o anki bir 1 ise (yani cizilecek bir pikselse)
                    if((sprite_byte & (0x80 >> col)) != 0){
                        //cizilecek yer ekranin icinde mi diye kontrol et
                        if(x + col < 64 && y +row < 32){
                            //Hedefteki piksel zaten 1 (acik) ise carpisma var demektir
                            if(cpu->display[(y + row) * 64 + (x + col)]== 1) {
                                cpu->V[0xF] = 1; //VF bayragini 1 yap
                            }
                            // XOR mantigi ile pikseli ciz veya sil (^= 1)
                            cpu->display[(y + row) * 64 + (x + col)] ^= 1;
                        }
                    }
                }
            }
            break;
        }

        //EXXX: Klavye (Input) kontrolleri
        case 0xE000:
            if((opcode & 0x00FF) == 0x009E){
                //EX9E: Eger VX register indaki tus basiliysa, bir sonraki komutu atla
                uint8_t key = cpu->V[(opcode & 0x0F00) >> 8];
                if(cpu->keypad[key]){
                    cpu->PC += 2;
                }
            } else if ((opcode & 0x00FF) == 0x00A1) {
                // EXA1: Eger VX register'indaki tus basili DEGILSE, bir sonraki komutu atla
                uint8_t key = cpu->V[(opcode & 0x0F00) >> 8];
                if(!cpu->keypad[key]){
                    cpu->PC += 2;
                }
            }
            break;
        
        //FXXX: Zamanlayicilar ve bellek
        case 0xF000:
            switch(opcode & 0x00FF){
                //FX07: VX = delay_timer(gecikme degerini oku)
                case 0x0007:
                    cpu->V[(opcode & 0x0F00) >> 8] = cpu->delay_timer;
                    break;
                //FX15: delay_timer = VX (geceikme degerini ayarla)
                case 0x0015:
                    cpu->delay_timer = cpu->V[(opcode & 0x0F00) >> 8];
                    break;
                //FX18: sound_timer =VX (Ses degerini ayarla)
                case 0x0018:
                    cpu->sound_timer = cpu->V[(opcode & 0x0F00) >> 8];
                    break;
                }
            break;


        
        default:
            printf("bilinmeyen ve ya henuz yazilmayan opcode: 0x%X\n", opcode);
        break;        
    }
}

void update_timers(Chip8CPU* cpu) {
    // Eger gecikme zamanlayicisi sifirdan buyukse, bir azlat
    if(cpu->delay_timer > 0){
        cpu->delay_timer--;
    }

    //Eger ses zamanlayicisi sifirdan buyukse, bir azalt
    if(cpu->sound_timer > 0){
        if(cpu->sound_timer == 1){
            // Sifira ulastiginde kisa bir "BEEP" sesi cikar
            printf("BEEP!\n");
        }
        cpu->sound_timer--;
    }
}
