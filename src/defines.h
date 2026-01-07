#ifndef DEFINES_H
#define DEFINES_H


#include <stdbool.h>
#include "raylib.h"

//  sabitler
#define MAX_BULLETS 2000    //  mermi havuzu
#define MAX_ENEMIES 2000    //  düşman havuzu
#define MAX_HIGHSCORE 20    //  tabloda ilk kaç skor gözükücek
#define MAX_GEM 1000    //  ekrandaki maksimum gem
#define SCREEN_WIDTH GetScreenWidth()   //  ekran genişlik
#define SCREEN_HEIGHT GetScreenHeight()   //  ekran yükseklik


//  oyun sabitleri
#define ENEMY_RADIUS 12.0f   // düşman büyüklüğü
#define PLAYER_FRAME_COUNT 8    //  oyuncu frame sayısı
#define ENEMY_FRAME_COUNT 6 //  düşman frame sayısı
#define PROJECTILE_FRAME_COUNT 6    //   mermi frame sayısı
#define PLAYER_BOUNDARY 20  //  oyuncu sınırı
#define INITIAL_SPAWN_INTERVAL 30.0f //  oyun zorluğu
#define XP_INTERVAL 1.1f    //  level atlamak gittikçe ne kadar zorlaşsın değeri
#define MAX_PLAYER_SPEED 350.0f //  maksimum oyuncu hızı
#define MAX_SHOOT_COOLDOWN 0.1f //  maksimum mermi cooldownı
#define MAX_PLAYER_HEALTH 100.0f    //  maximum can değeri
#define SCROLL_SPEED 35.0f  //  mouse tekerleği kaydırma hızı

//  spatial system sabitleri
#define CELL_SIZE 100 //    her hücre 100x100 pixel
#define GRID_BUCKETS 1024   //  toplam hücre sayısı
#define MAX_PER_CELL 128    //  bir hücrede olabilecek maximum düşman





//  enum kendi değişken türümüzü yaratmamızı sağlıyor
typedef enum {
    MENU,       //  giriş ekranı
    GAMEPLAY,       //  oyun ekranı
    LEVEL_UP,   //  level atlama ekranı
    SETTINGS,   //  ayarlar ekranı
    HIGHSCORES, //  skor tablosu ekranı
    GAME_OVER      //  lose ekranı
} GameState;


//  upgrade türleri
typedef enum {
    UPGRADE_ATTACK_SPEED,   //  saldırı hızı upgradei
    UPGRADE_MOVEMENT_SPEED, //  hareket hızı upgradei
    UPGRADE_DAMAGE, //  saldırı gücü upgradei
    UPGRADE_HEALTH //  can yükseltme veya can alma upgradei
} UpgradeType;

//  düşman türleri
typedef enum {
    ENEMY_BASIC,    //  dengeli düşman 0
    ENEMY_RUSHER,   //  hızlı ama zayıf 1
    ENEMY_TANK, //  güçlü ama yavaş 2
    ENEMY_TYPE_COUNT    // kaç tane düşman türü var otomatik 3 oluyor üste bir tane daha tür eklersek otomatik 4 olucak
}EnemyType;


//  düşman fabrika ayaralrı
//  oyun sırasında değişmeyen değerler
typedef struct{
    float baseHealth;   //  başlangıç canı
    float baseSpeed;    //  başlangıç hızı
    float scale;   //  boyut çarpanı
    float radius;   //  çarpışma yarıçapı
    Color tint; //  ayırt edici renk
    int xpValue;    //  öldüğünde vereği xp
}EnemyDef;


//  yükseltme kartı yapısı
typedef struct {
    const char* title;  //  yükseltme kartı başlığı
    const char* description;    //  yükseltme kartı açıklaması kart ne işe yarıyor
    UpgradeType type;   //  ne işe yarayacak
    float value; //  ugrade değeri ne kadar olucak
} UpgradeDef;




typedef struct {
    Vector2 pos;    //  gem pozisyonu
    Texture2D texture;  //  gem texture
    int value;  //  gem değeri (kaç xp veya kaç altın)
    bool active;    //  gemin aktifliği
    bool isMagnetized;  //  şuanda çekiliyor mu
    Color color;    //  gem rengi
    float rotation; //  kendi etrafında dönme efekti için
} Gem;



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
    float health;   //  mevcut can
    float maxHealth;    //  maximum can
} Player;


//  düşman yapısı
//  oyun sırasında değişir
typedef struct {
    Vector2 pos;    //  düşman pozisyonu
    Color color;    //  düşman rengi
    int xpReward;   //  vereceği xp 
    int currentFrame;   //  düşmanın o anki gösterilen frame numarası
    int type;
    float speed;    //  düşman hızı
    float health;   //  düşman canı
    float maxHealth;    //  can barı çizimi için
    float frameSpeed;   //  düşmanın frame i ne kadar hızlı değişecek
    float frameCounter; //  frame sayacı
    float scale;    //  texture ölçeklendirme değeri
    bool facingRight;   //  düşmanın bize doğru bakmasının kontrolü
    bool active;    //  düşman aktifliği
    Texture2D texture;  //  düşman texture ü
    Rectangle frameRec; //  düşman için yüklediğimiz resmin tek karesi 
} Enemy;



//  mermi yapısı
typedef struct{
    Vector2 pos;    //  merminin konumu
    Vector2 dir;    //  gideceği yön
    int type;   //  mermi tipi ilerisi için
    int currentFrame;   //  merminin o anki frame numarası
    float radius;   //  mermi büyüklüğü
    float frameSpeed;   //  frame değiştirme hızı
    float frameCounter; //  frame sayacı
    float scale;    //  texture için ölçeklendirme
    float speed;    //  hızı
    bool active;   //  mermi havada mı
    bool facingRight;   //  merminin ne tarafa baktığı
    Texture2D texture;  //  mermi texture ü
    Rectangle frameRec; //  mermi texture ünün tek karesi
} Projectile;


//  skor tablosu yapısı
typedef struct{
    char player_name[20];   //  oyuncu ismini girmesi için dizi
    int score;  //  oyuncunun skoru
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
    Rectangle playButton;   //  oyna butonu
    Rectangle settingsButton;   //  ayarlar butonu
    Rectangle scoresButton; //  skor butonu (kalkacak)
    Rectangle exitButton;   //  çıkış butonu
    Rectangle githubButton; //  github butonu
    Rectangle muteButton;   //  mute butonu
    Vector2 titlePos;   //  başlık pozisyonu
}UILayout;


typedef struct{
    Texture2D playerTexture;    //  oyuncu texture ü
    Texture2D enemyTextures[ENEMY_TYPE_COUNT];  //  düşman texture leri
    Texture2D bulletTexture;    //  mermi texture ü
    Texture2D gemTexture;   //  gem texture ü
    Texture2D floorTexture; //  zemin texture ü 
} GameAssets;



typedef struct {
    Player player;  //  oyuncu tanımlama
    GameAssets assets;  //  texturler
    Enemy enemies[MAX_ENEMIES]; //  düşmanlar için dizi
    Projectile bullets[MAX_BULLETS];    //  mermiler için dizi
    HighScore highScores[MAX_HIGHSCORE];    //  yüksek skor tablosu
    Gem gems[MAX_GEM];  //  ekranda birden fazla gem gözükmesi için gem havuzu
    GameSettings settings;  //  ayarlar
    UpgradeDef activeUpgrades[3];   //  level atlama ekranında o an gösterilen 3 kart
    GameState currentState; //  oyunun mevcut menüsü
    UILayout uilayout;  //  butonlar vs
    int score; //   skor sayacı 0
    int letterCount;    //  skortablosu girdisi için harf sayacı
    int enemyWaveSize;  //  düşmanların wave büyüklüğü
    int currentXP;  //  sahip olunan xp değeri
    int requiredXP; //  level atlamak için gereken xp değeri
    int level;  //  levelimiz
    char inputName[20]; //  skor tablosu isim tutucu
    float waveTimer;    //  wave için zamanı tutucak 0
    float shootTimer;    //  saldırı zamanlayıcısı 0
    float spawnInterval;    //  ne sıklıkla düşman çıkıcak 1.5f
    float shootCooldown; //  yarım saniyede bir ateş et  0.5f
    float scoreScrollOffset;    //  skor tablosunun kayma miktarı
    bool isNewHighScore;    //  gameover ekranında eğer ilk 10 a giren skor varsa input kutusu açmak için
    bool isScrollingDown;   //  yüksek skor tablosu aşağı mı iniyor geri yukarı mı sarıyor
    Camera2D camera;    //  oyuncu kamerası
} GameData;




#endif