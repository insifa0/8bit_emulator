#include <stdio.h>
#include "chip8.h"
#include <SDL2/SDL.h>


// SDL2 kullanirken main fonksiyonu tam olarak bu parametrelerle yazilmalidir
int main(int argc, char* argv[]) {
    // 1. Kullanici komut satirindan bir ROM dosyasi girmediyse uyar
    if (argc < 2) {
        printf("Kullanim: %s <rom_dosyasi>\n", argv[0]);
        return 1;
    }

    // 2. SDL'in Gorsel (Video) alt sistemini baslat
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Baslatilamadi! Hata: %s\n", SDL_GetError());
        return 1;
    }

    // 3. 640x320 boyutlarinda bir pencere olustur
    // Orijinal 64x32 cozunurluk gunumuz ekranlarinda karinca kadar gorunur, bu yuzden 10 kat buyuttuk.
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          640, 320, SDL_WINDOW_SHOWN);

    // 4. Ekrana pikselleri cizecek olan donanim hizlandirmali Renderer'i (Cizici) olustur
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Simdilik pencerenin basariyla acildigini gormek icin 3 saniye (3000 ms) bekle
    SDL_Delay(3000);

    // 5. Program kapanirken RAM'de cop birakmamak icin SDL bilesenlerini yok et
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}