#include <stdio.h>
#include "chip8.h"
#include <SDL2/SDL.h>
#include <math.h>

// Basit kare dalga (BEEP) sesi ureten fonksiyon
void audio_callback(void* userdata, Uint8* stream, int len) {
    Sint16* buffer = (Sint16*)stream;
    int length = len / 2; // 16-bit ses ornekleri (2 byte)
    static int sample_index = 0;

    for(int i = 0; i < length; i++, sample_index++) {
        // Kare dalga uretimi ve ses seviyesi (genlik) ayari
        // 3000 genliginde (cok yuksek olmayan) bir ses
        buffer[i] = ((sample_index / 20) % 2) ? 3000 : -3000;
    }
}


// SDL2 kullanirken main fonksiyonu tam olarak bu parametrelerle yazilmalidir
int main(int argc, char* argv[]) {
    // 1. Kullanici komut satirindan bir ROM dosyasi girmediyse uyar
    if (argc < 2) {
        printf("Kullanim: %s <rom_dosyasi>\n", argv[0]);
        return 1;
    }

    // 2. SDL'in Gorsel ve Ses alt sistemini baslat
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    // Ses cihazini ayarla
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audio_callback; // Yukarida yazdigimiz ses ureteci

    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    
    Chip8CPU cpu;
    init_chip8(&cpu);
    if(!load_rom(&cpu, argv[1])){
        printf("ROM dosyasi acilmadi!\n");
        return 1;
    }

    // 3. 640x320 boyutlarinda bir pencere olustur
    // Orijinal 64x32 cozunurluk gunumuz ekranlarinda karinca kadar gorunur, bu yuzden 10 kat buyuttuk. SDL ve Pencere ayarlari
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          640, 320, SDL_WINDOW_SHOWN);

    // 2. Ekrana pikselleri cizecek olan donanim hizlandirmali Renderer'i (Cizici) olustur
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //3.Ekranboyutlarına 64*32 bos tuval
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    // 4. SDL'in anlayacagi 32-bit (RGBA) renk formatinda piksellerimizi tutacak dizi
    uint32_t pixels[64 * 32];
    bool running = true;
    SDL_Event event; 

    // 5. Ana oyun dongumuz (Oyun kapanana kadar calisir)
    while(running){
        // Eger pencerenin carpisina (X) basilirsa donguyu bitir
        while (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }
            //Bir tusa basildiginda (1 yap)
            else if (event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym){
                    case SDLK_1: cpu.keypad[0x1] = true; break;
                    case SDLK_2: cpu.keypad[0x2] = true; break;
                    case SDLK_3: cpu.keypad[0x3] = true; break;
                    case SDLK_4: cpu.keypad[0xC] = true; break;
                    case SDLK_q: cpu.keypad[0x4] = true; break;
                    case SDLK_w: cpu.keypad[0x5] = true; break;
                    case SDLK_e: cpu.keypad[0x6] = true; break;
                    case SDLK_r: cpu.keypad[0xD] = true; break;
                    case SDLK_a: cpu.keypad[0x7] = true; break;
                    case SDLK_s: cpu.keypad[0x8] = true; break;
                    case SDLK_d: cpu.keypad[0x9] = true; break;
                    case SDLK_f: cpu.keypad[0xE] = true; break;
                    case SDLK_z: cpu.keypad[0xA] = true; break;
                    case SDLK_x: cpu.keypad[0x0] = true; break;
                    case SDLK_c: cpu.keypad[0xB] = true; break;
                    case SDLK_v: cpu.keypad[0xF] = true; break;
                }
            }

            // Tustan el cekildiginde (0 yap)
            else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_1: cpu.keypad[0x1] = false; break;
                    case SDLK_2: cpu.keypad[0x2] = false; break;
                    case SDLK_3: cpu.keypad[0x3] = false; break;
                    case SDLK_4: cpu.keypad[0xC] = false; break;
                    case SDLK_q: cpu.keypad[0x4] = false; break;
                    case SDLK_w: cpu.keypad[0x5] = false; break;
                    case SDLK_e: cpu.keypad[0x6] = false; break;
                    case SDLK_r: cpu.keypad[0xD] = false; break;
                    case SDLK_a: cpu.keypad[0x7] = false; break;
                    case SDLK_s: cpu.keypad[0x8] = false; break;
                    case SDLK_d: cpu.keypad[0x9] = false; break;
                    case SDLK_f: cpu.keypad[0xE] = false; break;
                    case SDLK_z: cpu.keypad[0xA] = false; break;
                    case SDLK_x: cpu.keypad[0x0] = false; break;
                    case SDLK_c: cpu.keypad[0xB] = false; break;
                    case SDLK_v: cpu.keypad[0xF] = false; break;
                }
            }
        }

        // a) Islemciyi 1 adim calistir (Fetch - Decode - Execute)
        emulate_cycle(&cpu);

        // b) 1 bitlik Chip-8 ekranini, 32 bitlik SDL2 RGBA formatina cevir
        for(int i = 0; i <64 *32; i++){
            // Eger piksel  1 ise Beyaz (0xFFFFFFFF), 0 ise Siyah (0x000000FF) yapılır.
            pixels[i] = cpu.display[i] ? 0xFFFFFFFF : 0x000000FF;
        }

        // c) pikselleri digital tuvale (texture) aktar
        SDL_UpdateTexture(texture,NULL, pixels, 64 * sizeof(uint32_t));

        //d)tuvali ekrana genisleterek ciz (640x320)
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);


        //e) Zamanlayicilari
        update_timers(&cpu);

        // Eger ses sayaci 0'dan buyukse sesi ac, degilse duraklat
        if (cpu.sound_timer > 0) {
            SDL_PauseAudioDevice(audio_device, 0); // Sesi baslat
        } else {
            SDL_PauseAudioDevice(audio_device, 1); // Sesi sustur
        }

        // F) Islemciyi yavaşlatmak icin delay
        SDL_Delay(2);
    }

    // 6. Program kapanirken RAM'de cop birakmamak icin SDL bilesenlerini yok et.
    SDL_DestroyTexture(texture);
    SDL_CloseAudioDevice(audio_device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

