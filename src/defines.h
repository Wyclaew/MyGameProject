#ifndef DEFINES_H
#define DEFINES_H


#include <stdbool.h>
#include "raylib.h"

//  sabitler
#define MAX_BULLETS 2000
#define MAX_ENEMIES 2000
#define MAX_HIGHSCORE 10
#define SCREEN_WIDTH 800   //  ekran genişlik
#define SCREEN_HEIGHT 600   //  ekran yükseklik


//  oyun sabitleri
#define ENEMY_RADIUS 12.0f   // düşman büyüklüğü
#define PLAYER_FRAME_COUNT 8    //  oyuncu frame sayısı
#define ENEMY_FRAME_COUNT 6 //  düşman frame sayısı
#define PROJECTILE_FRAME_COUNT 6    //   mermi frame sayısı
#define PLAYER_BOUNDARY 20  //  oyuncu sınırı
#define INITIAL_SPAWN_INTERVAL 20.0f //  oyun zorluğu


//  enum kendi değişken türümüzü yaratmamızı sağlıyor
typedef enum {
    MENU,       //  giriş ekranı
    GAMEPLAY,       //  oyun ekranı
    SETTINGS,   //  ayarlar ekranı
    HIGHSCORES, //  skor tablosu ekranı
    GAME_OVER      //  lose ekranı
} GameState;


//  oyuncu yapısı
typedef struct {
    Vector2 pos;    //  oyuncu konumu
    float speed;    //  oyuncu hızı
    float radius;   //  oyuncu büyüklüğü
    Texture2D texture;  //  oyuncunun resmi
    Rectangle frameRec; //  şu an gösterilen kare
    int currentFrame;   //  kaçıncı karedeyiz
    float frameSpeed;   //  ne kadar hızlı değişsin
    float framesCounter;    //  zaman sayacı animasyon hızı için
    bool facingRight;    //  karakter sağa mı bakıyor
    float scale;    //  karakter ölçeklendirmesi için
} Player;


//  düşman yapısı
typedef struct {
    Vector2 pos;
    Color color;
    float speed;
    float health;
    float frameSpeed;
    float frameCounter;
    float scale;
    bool facingRight;
    bool active;
    int type;
    int currentFrame;
    Texture2D texture;
    Rectangle frameRec;
} Enemy;



//  mermi yapısı
typedef struct{
    Vector2 pos;    //  merminin konumu
    Vector2 dir;    //  gideceği yön
    int type;
    int currentFrame;
    float radius;   //  mermi büyüklüğü
    float frameSpeed;
    float frameCounter;
    float scale;
    float speed;    //  hızı
    bool active;   //  mermi havada mı
    bool facingRight;
    Texture2D texture;
    Rectangle frameRec;
} Projectile;


//  skor tablosu yapısı
typedef struct{
    char player_name[20];
    int score;
} HighScore;


//  ayarlar yapısı
typedef struct {
    float masterVolume; //  genel ses
    float musicVolume;  //  müzik sesi
    float sfxVolume;    //  efekt sesi
    bool isMuted;   //  ses full kapalı mı full açık mı
} GameSettings;


// ui layoutu (düzeni)
typedef struct{
    Rectangle playButton;
    Rectangle settingsButton;
    Rectangle scoresButton;
    Rectangle exitButton;
    Rectangle githubButton;
    Rectangle muteButton;
    Vector2 titlePos;
}UILayout;


typedef struct{
    Texture2D playerTexture;
    Texture2D enemyTexture;
    Texture2D bulletTexture;
} GameAssets;



typedef struct {
    Player player;  //  oyuncu tanımlama
    GameAssets assets;
    Enemy enemies[MAX_ENEMIES]; //  düşmanlar için dizi
    Projectile bullets[MAX_BULLETS];    //  mermiler için dizi
    HighScore highScores[MAX_HIGHSCORE];    //  yüksek skor tablosu
    GameSettings settings;  //  ayarlar
    GameState currentState; //  oyunun mevcut menüsü
    UILayout uilayout;  //  butonlar vs
    int score; //   skor sayacı 0
    int letterCount;    //  skortablosu girdisi için harf sayacı
    int enemyWaveSize;  //  düşmanların wave büyüklüğü
    char inputName[20]; //  skor tablosu isim tutucu
    float waveTimer;    //  wave için zamanı tutucak 0
    float shootTimer;    //  saldırı zamanlayıcısı 0
    float spawnInterval;    //  ne sıklıkla düşman çıkıcak 1.5f
    float shootCooldown; //  yarım saniyede bir ateş et  0.5f
    bool isNewHighScore;    //  gameover ekranında eğer ilk 10 a giren skor varsa input kutusu açmak için
} GameData;




#endif