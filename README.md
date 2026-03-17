# 🕹️ CHIP-8 Emulator in C (with SDL2)

Bu proje, 1970'lerin sonunda **COSMAC VIP** gibi mikrobilgisayarlar için geliştirilen **CHIP-8** sanal makinesinin (VM) C dili ve SDL2 kütüphanesi kullanılarak geliştirilmiş tam kapsamlı bir emülatörüdür. Bu emülatör, donanım bileşenlerini (CPU, RAM, Ekran, Ses) yazılımsal olarak simüle ederek klasik Chip-8 oyunlarını (Pong, Tetris, Space Invaders vb.) günümüz bilgisayarlarında çalıştırabilmek için tasarlanmıştır.

## 🎮 Özellikler

*   **Tam Opcode Desteği:** Chip-8 işlemcisine ait tüm komut setleri (Grafik, Ses, Aritmetik, Akış Kontrolü) uygulanmıştır.
*   **Grafik Ekranı:** Orijinal 64x32 çözünürlük, SDL2 ile modern ekranlara ölçeklenerek aktarılmıştır.
*   **Ses Sistemi:** `SDL_AudioSpec` ve callback fonksiyonları kullanılarak "BEEP" sesleri (Kare dalga) üretilir.
*   **Giriş Sistemi:** Orijinal 16 tuşlu Hex klavye, standart QWERTY klavyeye haritalanmıştır.
*   **Zamanlayıcılar:** 60Hz hızında çalışan Gecikme (Delay) ve Ses (Sound) zamanlayıcıları.

## 📂 Proje Yapısı

*   **`main.c`**: Programın giriş noktasıdır. SDL2 penceresini, ses sistemini başlatır ve sonsuz döngü içinde işlemciyi çalıştırır (`emulate_cycle`).
*   **`chip8.c`**: Chip-8 işlemcisinin beynidir. Bellek yönetimi, register'lar ve tüm Opcode (komut) mantığı burada bulunur.
*   **`chip8.h`**: `Chip8CPU` veri yapısını ve fonksiyon prototiplerini tanımlayan başlık dosyasıdır.
*   **`include/ & lib/`**: SDL2 kütüphanesi için gerekli başlık (.h) ve kütüphane (.a/.lib) dosyaları.
*   **`SDL2.dll`**: Derlenmiş programın çalışması için gerekli dinamik kütüphane dosyası.


## 🚀 Kurulum ve Derleme

### Gereksinimler
* GCC Derleyicisi (MinGW veya benzeri)
* **SDL2** Geliştirme Kütüphanesi (include ve lib klasörleri proje dizininde olmalıdır)

### 🛠️ Nasıl Derlenir?
Terminal üzerinden aşağıdaki komutla projeyi derleyebilirsiniz:
```bash
gcc main.c chip8.c -o chip8_emu -I./include -L./lib -lmingw32 -lSDL2main -lSDL2
```

### Çalıştırma
Derleme sonrası oluşan `.exe` dosyasını bir ROM dosyası ile çalıştırın:
```bash
.\chip8_emu.exe PONG.ch8
```


## 🎮 Klavye Haritalaması (Input Mapping)

Orijinal Chip-8 klavyesi (0-F), klavyenizde aşağıdaki gibi eşleştirilmiştir:

| Chip-8 Tuş Takımı | Klavye Karşılığı |
| :---: | :---: |
| **1 2 3 C** | **1 2 3 4** |
| **4 5 6 D** | **Q W E R** |
| **7 8 9 E** | **A S D F** |
| **A 0 B F** | **Z X C V** |

## 📝 Notlar
*   **ROM Dosyaları:** Projede test için kullanılan **PONG** oyunu [kripod/chip8-roms](https://github.com/kripod/chip8-roms) reposundan temin edilmiştir. Diğer klasik oyunları (Tetris, Space Invaders vb.) da benzer kaynaklardan bulabilirsiniz.
*   **Hata Çözümü:** Eğer "ROM dosyası bulunamadı" hatası alırsanız, oyun dosyasının `chip8_emu.exe` ile *tamamen aynı klasörde* olduğundan emin olun.
*   **Ses Sorunları:** Ses duyamıyorsanız bilgisayarınızın ses seviyesini kontrol edin; emülatör basit bir kare dalga (beep) sesi üretir.
*   **Zamanlama:** Bu emülatör basit bir zamanlama döngüsü kullanır (`SDL_Delay`). Orijinal donanımın %100 hassas hızını taklit etmeyebilir ancak oyunları oynanabilir hızda çalıştırır.

## 📚 Kaynaklar ve Teşekkür
Bu emülatör geliştirilirken şu kaynaklardan faydalanılmıştır:
*   [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH.HTM) - Chip-8 için dokümantasyon.
*   [Kristóf Poduszló](https://github.com/kripod) - PONG ROM ve diğer test ROM'ları için. 
## 📄 Lisans
Bu proje MIT Lisansı ile lisanslanmıştır. İstediğiniz gibi kullanabilir, değiştirebilir ve dağıtabilirsiniz.
