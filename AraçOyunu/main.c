#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_image.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PLAYER_CAR_WIDTH 120
#define PLAYER_CAR_HEIGHT 120
#define MAX_RANDOM_CARS 2

#define VERTICAL_LANE_WIDTH 10
#define VERTICAL_LANE_HEIGHT WINDOW_HEIGHT
#define VERTICAL_LANE_X 200
#define VERTICAL_LANE_X2 400
#define VERTICAL_LANE_X3 600
#define VERTICAL_LANE_SPEED 1

typedef struct {
    int x;
    int y;
    SDL_Texture* texture;
} RandomCar;

void generateRandomCars(RandomCar* randomCars, int* carSpawnPositions, int numPositions, SDL_Renderer* renderer) {
    srand(time(NULL));

    for (int i = 0; i < MAX_RANDOM_CARS; i++) {
        int positionIndex = rand() % numPositions;
        randomCars[i].x = carSpawnPositions[positionIndex];
        randomCars[i].y = -i * 200;

        // Araba resmini yükleme
        SDL_Surface* carSurface = IMG_Load("resimler/randomAraba.png");
        randomCars[i].texture = SDL_CreateTextureFromSurface(renderer, carSurface);
        SDL_FreeSurface(carSurface);
    }
}

void cleanupRandomCars(RandomCar* randomCars) {
    for (int i = 0; i < MAX_RANDOM_CARS; i++) {
        SDL_DestroyTexture(randomCars[i].texture);
    }
}

int checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if (x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2) {
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // SDL başlatma
    SDL_Init(SDL_INIT_VIDEO);

    // Pencere oluşturma
    SDL_Window* window = SDL_CreateWindow("Beautiful Car Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    // Renderer oluşturma
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Boş ekranı render etme
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Siyah renk
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Oyuncu arabasının resmini yükleme
    SDL_Surface* playerCarSurface = IMG_Load("resimler/kullaniciAraba.png");
    SDL_Texture* playerCarTexture = SDL_CreateTextureFromSurface(renderer, playerCarSurface);
    SDL_FreeSurface(playerCarSurface);

    // Game over resmini yükleme
    SDL_Surface* gameOverSurface = IMG_Load("resimler/gameOver.jpg");
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_FreeSurface(gameOverSurface);

    // Yeniden dene resmini yükleme
    SDL_Surface* retrySurface = IMG_Load("resimler/tekrarDene.jpg");
    SDL_Texture* retryTexture = SDL_CreateTextureFromSurface(renderer, retrySurface);
    SDL_FreeSurface(retrySurface);

    // Oyuncu arabasının başlangıç konumu
    int playerCarX = 250;
    int playerCarY = 400;

    // Rastgele arabaları oluşturma
    RandomCar randomCars[MAX_RANDOM_CARS];
    int carSpawnPositions[] = { 50, 250, 450, 650 };
    int numPositions = sizeof(carSpawnPositions) / sizeof(carSpawnPositions[0]);
    generateRandomCars(randomCars, carSpawnPositions, numPositions, renderer);

    // Oyun durumu
    int gameRunning = 1;
    int gameOver = 0;

    // Kullanıcı pencereyi kapattığında çıkış yapma
    SDL_Event event;
    while (gameRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = 0;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT && playerCarX > 0) {
                    playerCarX -= 200; // Sola hareket için x koordinatını azalt
                }
                else if (event.key.keysym.sym == SDLK_RIGHT && playerCarX < WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
                    playerCarX += 200; // Sağa hareket için x koordinatını artır
                }
            }
        }

        // Oyun devam ediyorsa
        if (!gameOver) {
            // Boş ekranı temizleme
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            SDL_RenderClear(renderer);

            // Dikey şeritleri render etme
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk
            for (int i = -WINDOW_HEIGHT; i < WINDOW_HEIGHT; i += 60) {
                SDL_Rect verticalLaneRect = { VERTICAL_LANE_X, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
                SDL_RenderFillRect(renderer, &verticalLaneRect);

                SDL_Rect verticalLaneRect2 = { VERTICAL_LANE_X2, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
                SDL_RenderFillRect(renderer, &verticalLaneRect2);

                SDL_Rect verticalLaneRect3 = { VERTICAL_LANE_X3, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
                SDL_RenderFillRect(renderer, &verticalLaneRect3);
            }

            // Arabaları hareket ettirme ve render etme
            for (int i = 0; i < MAX_RANDOM_CARS; i++) {
                randomCars[i].y += VERTICAL_LANE_SPEED;

                // Arabanın ekrandan çıkmasını engelleme
                if (randomCars[i].y >= WINDOW_HEIGHT) {
                    randomCars[i].x = carSpawnPositions[rand() % numPositions];
                    randomCars[i].y = -PLAYER_CAR_HEIGHT;
                }

                // Araba ile oyuncu araba çarpışmasını kontrol etme
                int collision = checkCollision(playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT, randomCars[i].x, randomCars[i].y, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT);
                if (collision) {
                    gameOver = 1; // Oyunu bitir
                    break;
                }

                // Arabayı render etme
                SDL_Rect randomCarRect = { randomCars[i].x, randomCars[i].y, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
                SDL_RenderCopy(renderer, randomCars[i].texture, NULL, &randomCarRect);
            }

            // Arabayı ekrandan çıkmasını engelleme
            if (playerCarX < 0) {
                playerCarX = 0;
            }
            else if (playerCarX > WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
                playerCarX = WINDOW_WIDTH - PLAYER_CAR_WIDTH;
            }

            // Oyuncu arabasını render etme
            SDL_Rect playerCarRect = { playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
            SDL_RenderCopy(renderer, playerCarTexture, NULL, &playerCarRect);

            // Ekranı güncelleme
            SDL_RenderPresent(renderer);
        }
        else {
            // Oyun bittiğinde sadece game over ekranını render et
            SDL_Rect gameOverRect = { (WINDOW_WIDTH - PLAYER_CAR_WIDTH) / 2, (WINDOW_HEIGHT - PLAYER_CAR_HEIGHT) / 2, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
            SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
            SDL_RenderPresent(renderer);

            // 3 saniye beklet
            SDL_Delay(3000);

            // Oyunu yeniden başlat
            gameOver = 0;
            playerCarX = 250;
            playerCarY = 400;
            generateRandomCars(randomCars, carSpawnPositions, numPositions, renderer);
        }

        // 500 ms gecikme
        SDL_Delay(1);
    }

    // Kaynakları temizleme
    cleanupRandomCars(randomCars);
    SDL_DestroyTexture(playerCarTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(retryTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // SDL'yi kapatma
    SDL_Quit();

    return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include "src/include/SDL2/SDL.h"
// #include "src/include/SDL2/SDL_image.h"

// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600
// #define PLAYER_CAR_WIDTH 120
// #define PLAYER_CAR_HEIGHT 120
// #define MAX_RANDOM_CARS 2

// #define VERTICAL_LANE_WIDTH 10
// #define VERTICAL_LANE_HEIGHT WINDOW_HEIGHT
// #define VERTICAL_LANE_X 200
// #define VERTICAL_LANE_X2 400
// #define VERTICAL_LANE_X3 600
// #define VERTICAL_LANE_SPEED 1

// typedef struct {
//     int x;
//     int y;
//     SDL_Texture* texture;
// } RandomCar;

// void generateRandomCars(RandomCar* randomCars, int* carSpawnPositions, int numPositions, SDL_Renderer* renderer) {
//     srand(time(NULL));

//     for (int i = 0; i < MAX_RANDOM_CARS; i++) {
//         int positionIndex = rand() % numPositions;
//         randomCars[i].x = carSpawnPositions[positionIndex];
//         randomCars[i].y = -i * 200;

//         // Araba resmini yükleme
//         SDL_Surface* carSurface = IMG_Load("resimler/randomAraba.png");
//         randomCars[i].texture = SDL_CreateTextureFromSurface(renderer, carSurface);
//         SDL_FreeSurface(carSurface);
//     }
// }

// void cleanupRandomCars(RandomCar* randomCars) {
//     for (int i = 0; i < MAX_RANDOM_CARS; i++) {
//         SDL_DestroyTexture(randomCars[i].texture);
//     }
// }

// int checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
//     if (x1 < x2 + w2 &&
//         x1 + w1 > x2 &&
//         y1 < y2 + h2 &&
//         y1 + h1 > y2) {
//         return 1;
//     }
//     return 0;
// }

// int main(int argc, char* argv[]) {
//     // SDL başlatma
//     SDL_Init(SDL_INIT_VIDEO);

//     // Pencere oluşturma
//     SDL_Window* window = SDL_CreateWindow("Beautiful Car Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

//     // Renderer oluşturma
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     // Boş ekranı render etme
//     SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Siyah renk
//     SDL_RenderClear(renderer);
//     SDL_RenderPresent(renderer);

//     // Oyuncu arabasının resmini yükleme
//     SDL_Surface* playerCarSurface = IMG_Load("resimler/kullaniciAraba.png");
//     SDL_Texture* playerCarTexture = SDL_CreateTextureFromSurface(renderer, playerCarSurface);
//     SDL_FreeSurface(playerCarSurface);

//     // Game over resmini yükleme
//     SDL_Surface* gameOverSurface = IMG_Load("resimler/gameOver.jpg");
//     SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
//     SDL_FreeSurface(gameOverSurface);

//     // Oyuncu arabasının başlangıç konumu
//     int playerCarX = 250;
//     int playerCarY = 400;

//     // Rastgele arabaları oluşturma
//     RandomCar randomCars[MAX_RANDOM_CARS];
//     int carSpawnPositions[] = { 70, 270, 470, 670 };
//     int numPositions = sizeof(carSpawnPositions) / sizeof(carSpawnPositions[0]);
//     generateRandomCars(randomCars, carSpawnPositions, numPositions, renderer);

//     // Oyun durumu
//     int gameRunning = 1;
//     int gameOver = 0;

//     // Kullanıcı pencereyi kapattığında çıkış yapma
//     SDL_Event event;
//     while (gameRunning) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 gameRunning = 0;
//             }
//             else if (event.type == SDL_KEYDOWN) {
//                 if (event.key.keysym.sym == SDLK_LEFT && playerCarX > 0) {
//                     playerCarX -= 200; // Sola hareket için x koordinatını azalt
//                 }
//                 else if (event.key.keysym.sym == SDLK_RIGHT && playerCarX < WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//                     playerCarX += 200; // Sağa hareket için x koordinatını artır
//                 }
//             }
//         }

//         // Oyun devam ediyorsa
//         if (!gameOver) {
//             // Boş ekranı temizleme
//             SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
//             SDL_RenderClear(renderer);

//             // Dikey şeritleri render etme
//             SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk
//             for (int i = -WINDOW_HEIGHT; i < WINDOW_HEIGHT; i += 60) {
//                 SDL_Rect verticalLaneRect = { VERTICAL_LANE_X, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//                 SDL_RenderFillRect(renderer, &verticalLaneRect);

//                 SDL_Rect verticalLaneRect2 = { VERTICAL_LANE_X2, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//                 SDL_RenderFillRect(renderer, &verticalLaneRect2);

//                 SDL_Rect verticalLaneRect3 = { VERTICAL_LANE_X3, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//                 SDL_RenderFillRect(renderer, &verticalLaneRect3);
//             }

//             // Arabaları hareket ettirme ve render etme
//             for (int i = 0; i < MAX_RANDOM_CARS; i++) {
//                 randomCars[i].y += VERTICAL_LANE_SPEED;

//                 // Arabanın ekrandan çıkmasını engelleme
//                 if (randomCars[i].y >= WINDOW_HEIGHT) {
//                     randomCars[i].x = carSpawnPositions[rand() % numPositions];
//                     randomCars[i].y = -PLAYER_CAR_HEIGHT;
//                 }

//                 // Araba ile oyuncu araba çarpışmasını kontrol etme
//                 int collision = checkCollision(playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT, randomCars[i].x, randomCars[i].y, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT);
//                 if (collision) {
//                     gameOver = 1; // Oyunu bitir
//                     break;
//                 }

//                 // Arabayı render etme
//                 SDL_Rect randomCarRect = { randomCars[i].x, randomCars[i].y, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//                 SDL_RenderCopy(renderer, randomCars[i].texture, NULL, &randomCarRect);
//             }

//             // Arabayı ekrandan çıkmasını engelleme
//             if (playerCarX < 0) {
//                 playerCarX = 0;
//             }
//             else if (playerCarX > WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//                 playerCarX = WINDOW_WIDTH - PLAYER_CAR_WIDTH;
//             }

//             // Oyuncu arabasını render etme
//             SDL_Rect playerCarRect = { playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//             SDL_RenderCopy(renderer, playerCarTexture, NULL, &playerCarRect);

//             // Ekranı güncelleme
//             SDL_RenderPresent(renderer);
//         }
//         else {
//             // Oyun bittiğinde sadece game over ekranını render et
//             SDL_Rect gameOverRect = { (WINDOW_WIDTH - PLAYER_CAR_WIDTH) / 2, (WINDOW_HEIGHT - PLAYER_CAR_HEIGHT) / 2, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//             SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
//             SDL_RenderPresent(renderer);
//         }

//         // 500 ms gecikme
//         SDL_Delay(1);
//     }

//     // Kaynakları temizleme
//     cleanupRandomCars(randomCars);
//     SDL_DestroyTexture(playerCarTexture);
//     SDL_DestroyTexture(gameOverTexture);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);

//     // SDL'yi kapatma
//     SDL_Quit();

//     return 0;
// }


// // çalışan son hal
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include "src/include/SDL2/SDL.h"
// #include "src/include/SDL2/SDL_image.h"

// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600
// #define PLAYER_CAR_WIDTH 120
// #define PLAYER_CAR_HEIGHT 140
// #define MAX_RANDOM_CARS 4

// #define VERTICAL_LANE_WIDTH 10
// #define VERTICAL_LANE_HEIGHT WINDOW_HEIGHT
// #define VERTICAL_LANE_X 200
// #define VERTICAL_LANE_X2 400
// #define VERTICAL_LANE_X3 600
// #define VERTICAL_LANE_SPEED 1

// typedef struct {
//     int x;
//     int y;
//     SDL_Texture* texture;
// } RandomCar;

// void generateRandomCars(RandomCar* randomCars, int* carSpawnPositions, int numPositions, SDL_Renderer* renderer) {
//     srand(time(NULL));

//     for (int i = 0; i < MAX_RANDOM_CARS; i++) {
//         int positionIndex = rand() % numPositions;
//         randomCars[i].x = carSpawnPositions[positionIndex];
//         randomCars[i].y = -i * 200;

//         // Araba resmini yükleme
//         SDL_Surface* carSurface = IMG_Load("resimler/randomAraba.png");
//         randomCars[i].texture = SDL_CreateTextureFromSurface(renderer, carSurface);
//         SDL_FreeSurface(carSurface);
      
//     }
// }

// void cleanupRandomCars(RandomCar* randomCars) {
//     for (int i = 0; i < MAX_RANDOM_CARS; i++) {
//         SDL_DestroyTexture(randomCars[i].texture);
        
     
//     }
// }

// int main(int argc, char* argv[]) {
//     // SDL başlatma
//     SDL_Init(SDL_INIT_VIDEO);

//     // Pencere oluşturma
//     SDL_Window* window = SDL_CreateWindow("Boş Ekran", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

//     // Renderer oluşturma
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     // Boş ekranı render etme
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Siyah renk
//     SDL_RenderClear(renderer);
//     SDL_RenderPresent(renderer);

//     // Oyuncu arabasının resmini yükleme
//     SDL_Surface* playerCarSurface = IMG_Load("resimler/kullaniciAraba.png");
//     SDL_Texture* playerCarTexture = SDL_CreateTextureFromSurface(renderer, playerCarSurface);
//     SDL_FreeSurface(playerCarSurface);

//     // Oyuncu arabasının başlangıç konumu
//     int playerCarX = 250;
//     int playerCarY = WINDOW_HEIGHT - PLAYER_CAR_HEIGHT;

//     // Rastgele arabaları oluşturma
//     RandomCar randomCars[MAX_RANDOM_CARS];
//     int carSpawnPositions[] = { 70, 270, 470, 670 };
//     int numPositions = sizeof(carSpawnPositions) / sizeof(carSpawnPositions[0]);
//     generateRandomCars(randomCars, carSpawnPositions, numPositions, renderer);
  

//     // Kullanıcı pencereyi kapattığında çıkış yapma
//     SDL_Event event;
//     int running = 1;
//     while (running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 running = 0;
//             }
//             else if (event.type == SDL_KEYDOWN) {
//                 if (event.key.keysym.sym == SDLK_LEFT && playerCarX > 0) {
//                     playerCarX -= 200; // Sola hareket için x koordinatını azalt
//                 }
//                 else if (event.key.keysym.sym == SDLK_RIGHT && playerCarX < WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//                     playerCarX += 200; // Sağa hareket için x koordinatını artır
//                 }
//             }
//         }

//         // Boş ekranı temizleme
//         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//         SDL_RenderClear(renderer);

//         // Dikey şeritleri render etme
//         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk
//         for (int i = -WINDOW_HEIGHT; i < WINDOW_HEIGHT; i += 60) {
//             SDL_Rect verticalLaneRect = { VERTICAL_LANE_X, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//             SDL_RenderFillRect(renderer, &verticalLaneRect);

//             SDL_Rect verticalLaneRect2 = { VERTICAL_LANE_X2, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//             SDL_RenderFillRect(renderer, &verticalLaneRect2);

//             SDL_Rect verticalLaneRect3 = { VERTICAL_LANE_X3, i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//             SDL_RenderFillRect(renderer, &verticalLaneRect3);
//         }

//         // Arabaları hareket ettirme ve render etme
//         for (int i = 0; i < MAX_RANDOM_CARS; i++) {
//             randomCars[i].y += VERTICAL_LANE_SPEED;
         

//             // Arabanın ekrandan çıkmasını engelleme
//             if (randomCars[i].y >= WINDOW_HEIGHT) {
//                 randomCars[i].x = carSpawnPositions[rand() % numPositions];
//                 randomCars[i].y = -PLAYER_CAR_HEIGHT;
//             }

//             // Arabayı render etme
//             SDL_Rect randomCarRect = { randomCars[i].x, randomCars[i].y, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//             SDL_RenderCopy(renderer, randomCars[i].texture, NULL, &randomCarRect);
//         }

//         // Arabayı ekrandan çıkmasını engelleme
//         if (playerCarX < 0) {
//             playerCarX = 0;
//         }
//         else if (playerCarX > WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//             playerCarX = WINDOW_WIDTH - PLAYER_CAR_WIDTH;
//         }

//         // Oyuncu arabasını render etme
//         SDL_Rect playerCarRect = { playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//         SDL_RenderCopy(renderer, playerCarTexture, NULL, &playerCarRect);

//         // Ekranı güncelleme
//         SDL_RenderPresent(renderer);
//                    // 500 ms gecikme
//             SDL_Delay(1);

//     }

//     // Kaynakları temizleme
//     cleanupRandomCars(randomCars);
//     SDL_DestroyTexture(playerCarTexture);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);

//     // SDL'yi kapatma
//     SDL_Quit();

//     return 0;
// }

// // son kod
// #include <stdio.h>
// #include "src/include/SDL2/SDL.h"
// #include "src/include/SDL2/SDL_image.h"

// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600
// #define PLAYER_CAR_WIDTH 120
// #define PLAYER_CAR_HEIGHT 140

// #define VERTICAL_LANE_WIDTH 10
// #define VERTICAL_LANE_HEIGHT WINDOW_HEIGHT
// #define VERTICAL_LANE_X 200
// #define VERTICAL_LANE_X2 400
// #define VERTICAL_LANE_X3 600
// #define VERTICAL_LANE_SPEED 1

// int main(int argc, char* argv[])
// {
//     // SDL başlatma
//     SDL_Init(SDL_INIT_VIDEO);

//     // Pencere oluşturma
//     SDL_Window* window = SDL_CreateWindow("Boş Ekran", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

//     // Renderer oluşturma
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     // Boş ekranı render etme
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Siyah renk
//     SDL_RenderClear(renderer);
//     SDL_RenderPresent(renderer);

//     // Oyuncu arabasının resmini yükleme
//     SDL_Surface* playerCarSurface = IMG_Load("resimler/kullaniciAraba.png");
//     SDL_Texture* playerCarTexture = SDL_CreateTextureFromSurface(renderer, playerCarSurface);
//     SDL_FreeSurface(playerCarSurface);

//     // Oyuncu arabasının başlangıç konumu
//     int playerCarX = (WINDOW_WIDTH - PLAYER_CAR_WIDTH) / 2;
//     int playerCarY = WINDOW_HEIGHT - PLAYER_CAR_HEIGHT;

//     // Dikey şeritlerin başlangıç konumu
//     int verticalLaneY = -2 * WINDOW_HEIGHT;
//     int verticalLaneY2 = -2 * WINDOW_HEIGHT;
//     int verticalLaneY3 = -2 * WINDOW_HEIGHT;

//     // Kullanıcı pencereyi kapattığında çıkış yapma
//     SDL_Event event;
//     int running = 1;
//     while (running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 running = 0;
//             }
//             else if (event.type == SDL_KEYDOWN) {
//                 if (event.key.keysym.sym == SDLK_LEFT && playerCarX > 0) {
//                     playerCarX -= 10; // Sola hareket için x koordinatını azalt
//                 }
//                 else if (event.key.keysym.sym == SDLK_RIGHT && playerCarX < WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//                     playerCarX += 10; // Sağa hareket için x koordinatını artır
//                 }
//             }
//         }

//         // Boş ekranı temizleme
//         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//         SDL_RenderClear(renderer);

//         // Dikey şeritleri hareket ettirme
//         verticalLaneY += VERTICAL_LANE_SPEED;
//         verticalLaneY2 += VERTICAL_LANE_SPEED;
//         verticalLaneY3 += VERTICAL_LANE_SPEED;

//         if (verticalLaneY >= WINDOW_HEIGHT)
//             verticalLaneY = -WINDOW_HEIGHT;
//         if (verticalLaneY2 >= WINDOW_HEIGHT)
//             verticalLaneY2 = -WINDOW_HEIGHT;
//         if (verticalLaneY3 >= WINDOW_HEIGHT)
//             verticalLaneY3 = -WINDOW_HEIGHT;

//         // Dikey şeritleri render etme
//         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk

//         for (int i = -WINDOW_HEIGHT; i < WINDOW_HEIGHT; i += 60)
//         {
//             SDL_Rect verticalLaneRect = { VERTICAL_LANE_X, verticalLaneY + i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//             SDL_RenderFillRect(renderer, &verticalLaneRect);

//             SDL_Rect verticalLaneRect2 = { VERTICAL_LANE_X2, verticalLaneY2 + i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//             SDL_RenderFillRect(renderer, &verticalLaneRect2);

//             SDL_Rect verticalLaneRect3 = { VERTICAL_LANE_X3, verticalLaneY3 + i, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//             SDL_RenderFillRect(renderer, &verticalLaneRect3);
//         }

//         // Arabayı ekrandan çıkmasını engelleme
//         if (playerCarX < 0) {
//             playerCarX = 0;
//         }
//         else if (playerCarX > WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//             playerCarX = WINDOW_WIDTH - PLAYER_CAR_WIDTH;
//         }

//         // Oyuncu arabasını render etme
//         SDL_Rect playerCarRect = { playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//         SDL_RenderCopy(renderer, playerCarTexture, NULL, &playerCarRect);

//         // Ekranı güncelleme
//         SDL_RenderPresent(renderer);
//     }

//     // Kaynakları temizleme
//     SDL_DestroyTexture(playerCarTexture);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);

//     // SDL'yi kapatma
//     SDL_Quit();

//     return 0;
// }


// #include <stdio.h>
// #include "src/include/SDL2/SDL.h"
// #include "src/include/SDL2/SDL_image.h"

// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600
// #define PLAYER_CAR_WIDTH 120
// #define PLAYER_CAR_HEIGHT 140

// #define VERTICAL_LANE_WIDTH 10
// #define VERTICAL_LANE_HEIGHT WINDOW_HEIGHT
// #define VERTICAL_LANE_X 200
// #define VERTICAL_LANE_X2 400
// #define VERTICAL_LANE_WIDTH2 10
// #define VERTICAL_LANE_X3 600
// #define VERTICAL_LANE_WIDTH3 10

// int main(int argc, char* argv[])
// {
//     // SDL başlatma
//     SDL_Init(SDL_INIT_VIDEO);

//     // Pencere oluşturma
//     SDL_Window* window = SDL_CreateWindow("Boş Ekran", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

//     // Renderer oluşturma
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     // Boş ekranı render etme
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Siyah renk
//     SDL_RenderClear(renderer);
//     SDL_RenderPresent(renderer);

//     // Oyuncu arabasının resmini yükleme
//     SDL_Surface* playerCarSurface = IMG_Load("resimler/kullaniciAraba.png");
//     SDL_Texture* playerCarTexture = SDL_CreateTextureFromSurface(renderer, playerCarSurface);
//     SDL_FreeSurface(playerCarSurface);

//     // Oyuncu arabasının başlangıç konumu
//     int playerCarX = (WINDOW_WIDTH - PLAYER_CAR_WIDTH) / 14;
//     int playerCarY = WINDOW_HEIGHT - PLAYER_CAR_HEIGHT;

//     // Kullanıcı pencereyi kapattığında çıkış yapma
//     SDL_Event event;
//     int running = 1;
//     while (running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 running = 0;
//             }
//             else if (event.type == SDL_KEYDOWN) {
//                 if (event.key.keysym.sym == SDLK_LEFT && playerCarX > 0) {
//                     playerCarX -= 200; // Sola hareket için x koordinatını azalt
//                 }
//                 else if (event.key.keysym.sym == SDLK_RIGHT && playerCarX < WINDOW_WIDTH - PLAYER_CAR_WIDTH) {
//                     playerCarX += 200; // Sağa hareket için x koordinatını artır
//                 }
//             }
//         }

//         // Boş ekranı temizleme
//         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//         SDL_RenderClear(renderer);

//         // Dikey şeridi render etme
//         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk
//         SDL_Rect verticalLaneRect = { VERTICAL_LANE_X, 0, VERTICAL_LANE_WIDTH, VERTICAL_LANE_HEIGHT };
//         SDL_RenderFillRect(renderer, &verticalLaneRect);

//                 // Dikey2 şeridi render etme
//         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk
//         SDL_Rect verticalLaneRect2 = { VERTICAL_LANE_X2, 0, VERTICAL_LANE_WIDTH2, VERTICAL_LANE_HEIGHT };
//         SDL_RenderFillRect(renderer, &verticalLaneRect2);

//                 // Dikey3 şeridi render etme
//         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Beyaz renk
//         SDL_Rect verticalLaneRect3 = { VERTICAL_LANE_X3, 0, VERTICAL_LANE_WIDTH3, VERTICAL_LANE_HEIGHT };
//         SDL_RenderFillRect(renderer, &verticalLaneRect3);

//         // Oyuncu arabasını render etme
//         SDL_Rect playerCarRect = { playerCarX, playerCarY, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT };
//         SDL_RenderCopy(renderer, playerCarTexture, NULL, &playerCarRect);

//         // Ekranı güncelleme
//         SDL_RenderPresent(renderer);
//     }

//     // Kaynakları temizleme
//     SDL_DestroyTexture(playerCarTexture);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);

//     // SDL'yi kapatma
//     SDL_Quit();

//     return 0;
// }




//bos ekran

// #include <stdio.h>
// #include "src/include/SDL2/SDL.h"

// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600

// int main(int argc, char* argv[])
// {
//     // SDL başlatma
//     SDL_Init(SDL_INIT_VIDEO);

//     // Pencere oluşturma
//     SDL_Window* window = SDL_CreateWindow("Boş Ekran", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

//     // Renderer oluşturma
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     // Boş ekranı render etme
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Siyah renk
//     SDL_RenderClear(renderer);
//     SDL_RenderPresent(renderer);

//     // Kullanıcı pencereyi kapattığında çıkış yapma
//     SDL_Event event;
//     int running = 1;
//     while (running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 running = 0;
//             }
//         }
//     }

//     // Pencere ve rendereri yok etme
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);

//     // SDL'yi kapatma
//     SDL_Quit();

//     return 0;
// }

