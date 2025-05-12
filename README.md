# 🏎️ 2D Araba Oyunu (SDL2)

Bu proje, C programlama dili ve SDL2 grafik kütüphanesi kullanılarak geliştirilmiş bir **2D araba kaçış oyunudur**. Oyuncu, aşağıdan yukarıya doğru hareket eden arabaları yön tuşlarıyla kontrol ederek çarpışmadan kaçınmaya çalışır.

---

## 🎮 Oyun Özellikleri

- ⬅️➡️ **Yön tuşları ile kontrol**
- 🚗 **Rastgele düşen arabalar**
- 💥 **Çarpışma algılama**
- 🏁 **Yüksek skor takibi (isteğe bağlı geliştirilebilir)**
- 🖼️ **SDL2 ile grafik çizimi**
- 📁 PNG dosyalarıyla sprite kullanımı

---

## 🛠️ Kullanılan Teknolojiler

| Teknoloji | Açıklama                   |
|----------|----------------------------|
| C        | Uygulama dili              |
| SDL2     | Grafik, görüntü ve yazı motoru |
| SDL_image | PNG formatında görseller için |
| SDL_ttf   | Yazı fontları için destek  |
| Makefile | Derleme otomasyonu         |

---

## 🏁 Kurulum

### 🔧 Gerekli Bağımlılıklar

- GCC (C derleyicisi)
- SDL2
- SDL2_image
- SDL2_ttf

### 🧱 Derleme

Proje dizininde şu komutu çalıştırarak oyunu derleyebilirsiniz:

```bash
make
