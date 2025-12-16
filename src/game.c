#include "game.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "collision.h"
#include "ui.h"
#include <string.h>
#include <math.h>
#include <stdio.h>




void Game_Init(GameData *game){
    //  gamedata structını sıfırla
    memset(game, 0, sizeof(GameData));

    //  oyun başlangıç değerleri
    game->currentState = MENU;  //  başlangıçta hangi menüde olucak
    game->spawnInterval = INITIAL_SPAWN_INTERVAL; //  oyun zorluk değeri
    game->shootCooldown = 0.5f; //  atış hızımız
    game->enemyWaveSize = 5;    //  başlangıçta 5 düşman 
    game->assets.playerTexture = LoadTexture("player_cat_walk.png");
    game->assets.enemyTexture = LoadTexture("enemy_walk.png");
    game->assets.bulletTexture = LoadTexture("bullet_fired.png");

    //  ses ayarları
    game->settings.masterVolume = 1.0f; //  ana ses seviyesi
    game->settings.musicVolume = 1.0f;  //  müzik sesi seviyesi
    game->settings.sfxVolume = 1.0f;    //  efekt sesi sevyesi
    game->settings.isMuted = false; //  menüdeki ses kapatma butonu durumu


    //  texture yükleme kontrolü
    if(game->assets.playerTexture.id == 0){
        TraceLog(LOG_WARNING, "Player texture failed to load!");
    }

    if(game->assets.bulletTexture.id == 0){
        TraceLog(LOG_WARNING, "Bullet texture failed to load!");
    }

        if(game->assets.enemyTexture.id == 0){
        TraceLog(LOG_WARNING, "Enemy texture failed to load!");
    }

    //  alt sistemleri başlat
    //  oyuncuyu başlat
    Player_Init(&game->player, &game->assets);

    //  uilayout pozisyonları
    UI_InitLayout(game);

    // high score tablosunu yükle
    Game_LoadHighScores(game);

    //  düşmanları hazırla
    Enemy_Init(game);

}



void Game_Update(GameData *game, float dt){

    switch(game->currentState){
            case MENU:
            UI_UpdateMenu(game);
            break;

            case GAMEPLAY:
            Player_Update(&game->player, dt);   //  oyuncu hareketi
            Game_WaveSpawner(game, dt); //  wave spawner
            Enemy_Update(game, dt); //  düşman ai update
            Projectile_UpdateBullet(game, dt);   //  mermi hareketi
            Game_ShootingSystem(game, dt);  //  otomatik ateş sistemi
            Collisions_CheckAll(game);  //  carpışma kontrolü
            break;


            case SETTINGS:
            UI_UpdateSettings(game);
            break;


            case HIGHSCORES:
            if(IsKeyPressed(KEY_B)) game->currentState = MENU;
            break;


            case GAME_OVER:
            Game_UpdateGameOver(game);
            break;

        }
}


void Game_Draw(const GameData *game){
         BeginDrawing();    //  çizmeye başla

        //  arkaplanı her karede temizleme temizlemezsek önceki karedeki çizim kalır ve arkamızda iz bırakırız
        ClearBackground(RAYWHITE);

        switch(game->currentState){

            //  menüdeki yazılar
            case MENU:
            UI_DrawMenu(game);
            break;

            case SETTINGS:
            UI_DrawSettings(game);
            break;

            case HIGHSCORES:
            UI_DrawHighScores(game);
            break;
            
            // oyun içi çizimler oyuncu düşmanlar vs.
            case GAMEPLAY:
            UI_DrawGameplay(game);
            break;

            //  oyun bitişi
            case GAME_OVER:
            UI_DrawGameOver(game);
            break;
         }
        EndDrawing();   // çizim işlemini bitir
    }




//  OYUN MANTIKLARI

//  oyuncunun içinde düşman doğmama mekaniği
Vector2 Game_GetSafeSpawnPosition (Vector2 playerPos, float minDistance){
    Vector2 spawnPos;
    float distance = 0.0f;

    do{
        spawnPos.x = (float)GetRandomValue(0, SCREEN_WIDTH);
        spawnPos.y = (float)GetRandomValue(0, SCREEN_HEIGHT);

        float dx = playerPos.x - spawnPos.x;
        float dy = playerPos.y - spawnPos.y;
        distance = sqrt(dx * dx + dy * dy);

        
    } 
    while(distance < minDistance);
    return spawnPos;
}



void Game_ShootingSystem(GameData *game, float dt){
    //  oto saldırı mantığı en yakındakini bulup ateş et

            game->shootTimer += dt;   //  sayacı arttırma
            
            if(game->shootTimer >= game->shootCooldown){
                int nearestEnemyIndex = -1; //  en yakın düşmanı bul
                float minDistance = 9999999.0f;
                for (int i = 0; i < MAX_ENEMIES; i++){  //  sadece aktif ve varsayılan düşmanlara bakacağız
                    if(!game->enemies[i].active) continue;

                    float dx = game->enemies[i].pos.x - game->player.pos.x;
                    float dy = game->enemies[i].pos.y - game->player.pos.y;
                    float dist = sqrt(dx * dx + dy * dy);
                
                    if(dist < minDistance){
                        minDistance = dist;
                        nearestEnemyIndex = i;
                    }
                }
                //  eğer düşman bulduysan ateşle
                if(nearestEnemyIndex != -1){
                    //  mermiyi player pozisyonundan düşman pozisyonuna ateşle
                    Projectile_FireBullet(game, game->enemies[nearestEnemyIndex].pos);
                    game->shootTimer = 0.0f;   //  sayacı sıfırla                
                
            }
        }    
}


void Game_WaveSpawner(GameData *game, float dt){
        //  wave mantığı ekle
        //  zamanı ilerlet
        game->waveTimer += dt;
        if (game->waveTimer >= game->spawnInterval){

            //  düşmanları çağır wave size kadar
            for (int i = 0; i < game->enemyWaveSize; i++){
                Enemy_Spawn(game);
            }
            
            //  gittikçe dalga boyutunu artır
            game->enemyWaveSize += 2;

            //  wavesize limitle sonsuza kadar artmasın
            if(game->enemyWaveSize > 50) game->enemyWaveSize = 50;

            //  zamanlayıcıyı tekrar sıfırla
            game->waveTimer = 0;

            //  zaman ilerledikçe zorlaşması için her waveden sonra diğer wave için gereken süreyi kısaltma
            game->spawnInterval *= 0.99f;

            //  oyun zorluğuna minimum limit
            if(game->spawnInterval < 0.5f){
                game->spawnInterval = 0.5f;
            }
            
        }
}




void Game_UpdateGameOver(GameData *game){
    //  eğer yeni bi yüksek skor varsa isim girişi bekle
    if (game->isNewHighScore){
        //  karakter yakalama mantığı
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (game->letterCount < 10)){
                game->inputName[game->letterCount] = (char) key;
                game->inputName[game->letterCount+1] ='\0'; //  string sonlandırıcı
                game->letterCount++;
            }
            key = GetCharPressed(); //  sıradaki tuşu al
        }
        //  harf silme (backspace)
        if (IsKeyPressed(KEY_BACKSPACE)){
            game->letterCount--;
            if (game->letterCount < 0) game->letterCount = 0;
            game->inputName[game->letterCount] = '\0';
        }
        //  ismi kaydetme
        if (IsKeyPressed(KEY_ENTER) && game->letterCount > 0){
            //  skoru listeye ekle ve kaydet
            game->highScores[MAX_HIGHSCORE -1].score = game->score;  //  globl skor değişkeni
            TextCopy(game->highScores[MAX_HIGHSCORE-1].player_name, game->inputName);

            //  skor sırlama
            for (int i = 0; i < MAX_HIGHSCORE; i++){
                for (int j = 0; j < MAX_HIGHSCORE-i-1; j++){
                    if (game->highScores[j].score < game->highScores[j+1].score){
                        HighScore temp = game->highScores[j];
                        game->highScores[j] = game->highScores[j+1];
                        game->highScores[j+1] = temp;
                    }
                }
            }
            Game_SaveHighScores(game);
            game->isNewHighScore = false;
            game->currentState = HIGHSCORES;  //  kaydettikten sonra skorları gösteriyo
        }
    }
    else {
        //  yüksek skor değilse normal buton
        if (IsKeyPressed(KEY_ENTER)){
            Game_Reset(game);
            game->currentState = GAMEPLAY;
        }
        if (IsKeyPressed(KEY_M)){
            game->currentState = MENU;
        }
    }
}




void Game_Reset(GameData *game){
    //  oyuncuyu merkeze koy
    game->player.pos = (Vector2){(float) SCREEN_WIDTH / 2.0f, (float) SCREEN_HEIGHT / 2.0f};

    //  düşmanları resetleme
    Enemy_Reset(game);

    //  tüm mermileri sil
    for (int i = 0; i < MAX_BULLETS; i++){
        game->bullets[i].active = false;
    }

    //  başlar başlamaz düşman doğması için
    game->waveTimer = game->spawnInterval;
    
    game->shootTimer = 0;

    //  wavesizeı sıfırla
    game->enemyWaveSize = 5;

    //  başlangıç zorluğuna dön
    game->spawnInterval = INITIAL_SPAWN_INTERVAL;

    // skor sıfırla
    game->score = 0;
    
}



void Game_LoadHighScores(GameData *game){
    FILE *scoreFile = fopen("high_scores.dat", "rb");
    if(scoreFile == NULL){
        //  dosya yoksa listeyi sıfırla 
        for (int i = 0; i < MAX_HIGHSCORE; i++){
            game->highScores[i].score = 0;
            TextCopy(game->highScores[i].player_name, "----"); 
        }
        return;
    }
    //  tüm diziyi tek seferde oku
    fread(game->highScores, sizeof(HighScore), MAX_HIGHSCORE, scoreFile);
    fclose(scoreFile);

}



void Game_SaveHighScores(const GameData *game){
    FILE *scoreFile = fopen("high_scores.dat", "wb");
    if(scoreFile != NULL){
        fwrite(game->highScores, sizeof(HighScore), MAX_HIGHSCORE, scoreFile);
        fclose(scoreFile);
    }
}


void Game_CheckAndSaveScore(GameData *game, int newScore){

    //  skor ilk 10 skora giriyor mu kontrol et
    if(newScore > game->highScores[MAX_HIGHSCORE-1].score){
        game->isNewHighScore = true;
        //  stringi temizle
        game->letterCount = 0;
        game->inputName[0] = '\0';

    }

}


void Game_Cleanup(GameData *game){
    UnloadTexture(game->assets.playerTexture);
    UnloadTexture(game->assets.enemyTexture);
    UnloadTexture(game->assets.bulletTexture);
}