#include "game.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "collision.h"
#include "ui.h"
#include "gem.h"
#include "upgrade_pool.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



void Game_Init(GameData *game){
    //  gamedata structını sıfırla
    memset(game, 0, sizeof(GameData));

    //  oyun başlangıç değerleri
    game->currentState = MENU;  //  başlangıçta hangi menüde olucak
    game->spawnInterval = INITIAL_SPAWN_INTERVAL - 20.0f; //  oyun zorluk değeri
    game->shootCooldown = 1.0f; //  atış hızımız
    game->enemyWaveSize = 2;    //  başlangıçta kaç düşman gelicek 
    game->assets.playerTexture = LoadTexture("player_cat_walk.png");
    game->assets.enemyTextures[ENEMY_BASIC] = LoadTexture("enemy_basic.png");
    game->assets.enemyTextures[ENEMY_RUSHER] = LoadTexture("enemy_rusher.png");
    game->assets.enemyTextures[ENEMY_TANK] = LoadTexture("enemy_tank.png");
    game->assets.bulletTexture = LoadTexture("bullet_fired.png");
    game->assets.gemTexture = LoadTexture("xp_gem.png");
    game->assets.floorTexture = LoadTexture("floor1.png");
    SetTextureWrap(game->assets.floorTexture,TEXTURE_WRAP_REPEAT);  //  texture ün bittiği yerden tekrarlanmasını sağlar
    game->requiredXP = 100; //  seviye atlamak için başlangıçta gereken xp
    game->level = 1;    //  başlangıç leveli
    game->currentXP = 0;    //  başlangıçta sahip olduğumuz xp
    game->isScrollingDown = true;   //  skor tablosu aşağı doğru kayıyor mu kontrolü

    //  ses ayarları
    game->settings.masterVolume = 1.0f; //  ana ses seviyesi
    game->settings.musicVolume = 1.0f;  //  müzik sesi seviyesi
    game->settings.sfxVolume = 1.0f;    //  efekt sesi sevyesi
    game->settings.isMuted = false; //  menüdeki ses kapatma butonu durumu

    //  kamera ayarları
    game->camera.target = (Vector2){0, 0};  //  kameranın ilk başta bakacağı pozisyon
    game->camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};   //  kameranın odaklanacağı nokta
    game->camera.rotation = 0.0f;   //  kameranın dönüş açısı
    game->camera.zoom = 1.0f;   //  yakınlaştırma ölçeği şuan 1 e 1


    //  texture yükleme kontrolü
    if(game->assets.playerTexture.id == 0){
        TraceLog(LOG_WARNING, "Player texture failed to load!");
    }

    if(game->assets.bulletTexture.id == 0){
        TraceLog(LOG_WARNING, "Bullet texture failed to load!");
    }

    if(game->assets.enemyTextures[ENEMY_BASIC].id == 0){
    TraceLog(LOG_WARNING, "enemy_basic texture failed to load!");
    }

    //  alt sistemleri başlat
    //  oyuncuyu başlat
    Player_Init(&game->player, &game->assets);

    //  uilayout pozisyonları
    UI_InitLayout(game);

    // high score tablosunu yükle
    Game_LoadHighScores(game);

    //  gemleri hazırla
    Gem_Init(game);

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
            Player_Camera_Update(game); //  kamera hareketi
            Gem_UpdateGems(game,dt);    //  gem matematiği efektleri
            Game_WaveSpawner(game, dt); //  wave spawner
            Enemy_Update(game, dt); //  düşman ai update
            Projectile_UpdateBullet(game, dt);   //  mermi hareketi
            Game_ShootingSystem(game, dt);  //  otomatik ateş sistemi
            Collisions_CheckAll(game);  //  carpışma kontrolü
            Game_CheckLevelUp(game);    //  level atlama kontrolü
            break;

            case LEVEL_UP:
            UI_UpdateLevelUp(game);
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

        //  çizmeye başla
         BeginDrawing();

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
            //  önce dünyayı çiz
            Game_DrawWorld(game);

            //  sonra oyunu çiz
            UI_DrawGameplay(game);
            break;

            //  level atlama ekranı çizimi
            case LEVEL_UP:
            //  arkada donmuş olarak gözükmesi için önce dünyayı çiz
            Game_DrawWorld(game);

            //  sonra oyun çi ui
            UI_DrawGameplay(game);
            UI_DrawLevelUp(game);
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
            
             //  en yakın düşmanı bul
            if(game->shootTimer >= game->shootCooldown){
                int nearestEnemyIndex = -1; //  en yakın düşman kimliği, henüz düşman bulunmadı (-1 geçersiz sayı)
                float minDistance = 9999999.0f; //  başlangıçta en kısa mesafeyi çok büyük yapıyoruz
                for (int i = 0; i < MAX_ENEMIES; i++){  //  sadece aktif ve varsayılan düşmanlara bakacağız
                    if(!game->enemies[i].active) continue;

                    float dx = game->enemies[i].pos.x - game->player.pos.x;
                    float dy = game->enemies[i].pos.y - game->player.pos.y;
                    float dist = sqrt(dx * dx + dy * dy);
                
                    if(dist < minDistance){
                        minDistance = dist; //  yeni en kısa mesafeyi bulduk
                        nearestEnemyIndex = i;  //  en yakın düşmanın kimliği
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
            game->enemyWaveSize += 1;

            //  wavesize limitle sonsuza kadar artmasın
            if(game->enemyWaveSize > 50) game->enemyWaveSize = 50;

            //  zamanlayıcıyı tekrar sıfırla
            game->waveTimer = 0;

            //  zaman ilerledikçe zorlaşması için her waveden sonra diğer wave için gereken süreyi kısaltma
            game->spawnInterval *= 0.99f;

            //  oyun zorluğuna minimum limit
            if(game->spawnInterval < 15.0f){
                game->spawnInterval = 15.0f;
            }
            
        }
}




void Game_UpdateGameOver(GameData *game){
    //  eğer yeni bi yüksek skor varsa isim girişi bekle
    if (game->isNewHighScore){
        //  karakter yakalama mantığı
        //  getcharpressed o karede basılan tuşun ASCII kodunu verir
        int key = GetCharPressed();

        //  tuşlara çok hızlı basılırken de algılayabilmek için while kullanıyoruz
        while (key > 0) {

            //  32 125 aralığı ASCII tablosunda yazılabilir karakterlerdir
            if ((key >= 32) && (key <= 125) && (game->letterCount < 10)){   //  ve isim kutudan taşmasın diye 10 karakter sınırı koyuyoruz
                game->inputName[game->letterCount] = (char) key;
                //  \0 (string sonlandırıcı) eklenerek c dilinin metni nerede bitireceğini bilmesi sağlıyoruz
                game->inputName[game->letterCount+1] ='\0';
                game->letterCount++;
            }
            key = GetCharPressed(); //  sıradaki tuşu al
        }

        //  harf silme (backspace)
        if (IsKeyPressed(KEY_BACKSPACE)){
            game->letterCount--;
            if (game->letterCount < 0) game->letterCount = 0;

            //  geri geldiğimiz noktaya hemen \0 koyarız ki önceki harf artık okunmasın
            game->inputName[game->letterCount] = '\0';
        }

        //  ismi kaydetme
        if (IsKeyPressed(KEY_ENTER) && game->letterCount > 0){

            //  skoru listeye ekle ve kaydet
            //  yeni skoru direkt listenin sonuna yazıyoruz ama zaten birazdan sıralanacak
            game->highScores[MAX_HIGHSCORE -1].score = game->score;  //  globl skor değişkeni

            //  high score içindeki player name dizine girilen ismi kopyalıyoruz
            TextCopy(game->highScores[MAX_HIGHSCORE-1].player_name, game->inputName);

            //  skor sırlama (bubble sort) listeyi tarayarak en yükseği 0. indekse çıkarıyoruz
            for (int i = 0; i < MAX_HIGHSCORE; i++){
                for (int j = 0; j < MAX_HIGHSCORE-i-1; j++){
                    if (game->highScores[j].score < game->highScores[j+1].score){
                        HighScore temp = game->highScores[j];
                        game->highScores[j] = game->highScores[j+1];
                        game->highScores[j+1] = temp;
                    }
                }
            }

            //  yeni listeyi kalıcı olarak kaydediyoruz
            Game_SaveHighScores(game);
            game->isNewHighScore = false;
            game->currentState = MENU;
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

    //  başlar başlamaz düşman doğması için sayacı ilerden başlatıyoruz
    game->waveTimer = game->spawnInterval;
    
    game->shootTimer = 0;

    //  wavesizeı sıfırla
    game->enemyWaveSize = 5;

    //  başlangıç zorluğuna dön
    game->spawnInterval = INITIAL_SPAWN_INTERVAL;

    // skor sıfırla
    game->score = 0;

    //  eski gemleri silmek için
    Game_Init(game);
    
}



void Game_LoadHighScores(GameData *game){
    //  program bazı bytlerı satır sonu karakteri sanmasın diye rb kullandık 0 ve 1 olarak olduğu gibi okuyor
    FILE *scoreFile = fopen("high_scores.dat", "rb");   //  rb ile dosyayı byte byte okuyoruz
    if(scoreFile == NULL){
        //  dosya yoksa listeyi sıfırla 
        for (int i = 0; i < MAX_HIGHSCORE; i++){
            //  skorların hepsini 0 a eşitliyoruz
            game->highScores[i].score = 0;

            //  tüm isimlerin yerine ------- koyuyoruz
            TextCopy(game->highScores[i].player_name, "---- "); 
        }
        return;
    }
    //  tüm diziyi tek seferde oku
    //  1. veri nerede
    //  2. her okumada kaç byte atlayacağım yani 1 blok kaç byte
    //  3. bu bloklardan kaç tane okuyayım
    //  4. hangi kaynaktan okuyacağım
    fread(game->highScores, sizeof(HighScore), MAX_HIGHSCORE, scoreFile);
    fclose(scoreFile);
}



void Game_SaveHighScores(const GameData *game){
    //  rb yi kullanabilmek için direkt wb şekilde yazmamız gerekiyor
    FILE *scoreFile = fopen("high_scores.dat", "wb");

    //  dosya başarılı şekilde açıldıysa
    //  bu kontolü yapmadan dosyaya bişeyler yazmaya çalışırsak program çökebilir
    if(scoreFile != NULL){
        //  1. den veriyi al, 2. her bir skor paketi boyutu, 3. kaç paket yazılacak, 4. hangi dosyaya yazılacak
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

    for(int i=0; i < ENEMY_TYPE_COUNT; i++){
    UnloadTexture(game->assets.enemyTextures[i]);
    }
    
    UnloadTexture(game->assets.bulletTexture);
    UnloadTexture(game->assets.gemTexture);
    UnloadTexture(game->assets.floorTexture);
}


void Game_GenerateUpgrades(GameData *game){

    //  sayıları karıştırmak için geçici dizi
    int randomize[UPGRADE_POOL_SIZE];

    //  diziyi doldur
    for (int i = 0; i < UPGRADE_POOL_SIZE; i++){
        randomize[i] = i;
    }
    
    //  random 3 farklı kart seç
    //  diziyi karıştır 
    for (int i = 0; i < UPGRADE_POOL_SIZE; i++){
        int randomPos = GetRandomValue(0, UPGRADE_POOL_SIZE - 1);

        //  sayıların yerlerini değişiriyoruz
        int temp = randomize[i];
        randomize[i] = randomize[randomPos];
        randomize[randomPos] = temp;
    }
    

    for (int i = 0; i < 3; i++){
        int selectedID = randomize[i];
        game->activeUpgrades[i] = upgradePool[selectedID];
    }    
}


//  hız ekleme yardımcı fonksiyonu
void Game_ApplyMoveSpeed(Player *player, float percentage){
    player->speed += (player->speed * percentage);

    //  yürüme hızını limitle
    if(player->speed > MAX_PLAYER_SPEED) player->speed = MAX_PLAYER_SPEED;
}

//  atış hızı azaltma yardımcı fonksiyonu
void Game_ApplyAttackSpeed(GameData *game, float percentage){
    game->shootCooldown *= (1.0f - percentage);

    //  atış hızını limitle
    if(game->shootCooldown < MAX_SHOOT_COOLDOWN) game->shootCooldown = MAX_SHOOT_COOLDOWN;
}


void Game_ApplyUpgrade(GameData *game, int index){
    UpgradeDef selected = game->activeUpgrades[index];

    switch (selected.type){
        case UPGRADE_ATTACK_SPEED:
        Game_ApplyAttackSpeed(game, selected.value);
        break;

        case UPGRADE_MOVEMENT_SPEED:
        Game_ApplyMoveSpeed(&game->player, selected.value);
        break;


        // damage vs gelicek --------------------
    
    }
}

//  sadece oyun dünyasındaki varklıkları çizer
void Game_DrawWorld(const GameData *game) {
    //  kamerayı aç
    BeginMode2D(game->camera);

        //  zemin çizimi
        Game_DrawFloor(game);

        //  objeler
        Gem_DrawGem(game);
        Projectile_DrawBullet(game);
        Enemy_Draw(game);
        Player_Draw(&game->player);

    EndMode2D(); // kamreayı kapat
}

void Game_CheckLevelUp(GameData *game) {
            //  level atlama kontrolü
            if(game->currentXP >= game->requiredXP){
                game->level++;  //  leveli arttır
                game->currentXP -= game->requiredXP; //  artan leveli sonraki levele aktarma
                game->requiredXP = (int)game->requiredXP * XP_INTERVAL; //  level atlamayı zorlaştır

                //  upgrade kartlarını hazırla
                Game_GenerateUpgrades(game);    //  kartları hazırla
                game->currentState = LEVEL_UP;  //  oyunu dondur ve menüyü aç
                // buraya level atlama ödülü olarak can yinelem veya başka bişey gelebilir
                //  level atlama sesi de gelebilir
            }
}


void Game_DrawFloor(const GameData *game) {
    Texture2D floorTexture = game->assets.floorTexture;

    //  resmin hangi parçalarını alıyoruz ?(source)
    //  x ve y değerlerini oyunucun konumuna eşitlersek desen hareket eder
    Rectangle source = {
        game->player.pos.x,
        game->player.pos.y,
        (float)SCREEN_WIDTH,
        (float)SCREEN_HEIGHT
    };

    //  ekranda nereye çiziyorsun yani hedef neresi(dest)
    //  oyuncuyu hep ortaladığımız için zemini de oyunucunun etrafına kuruyoruz
    Rectangle dest = {
        game->player.pos.x,
        game->player.pos.y,
        (float)SCREEN_WIDTH,
        (float)SCREEN_HEIGHT
    };

    //  merkezi ayarlıyoruz ki tam otursun
    Vector2 origin = {
        (float)SCREEN_WIDTH / 2.0f,
        (float)SCREEN_HEIGHT / 2.0f
    };

    //  çizim   (resim çok parlaksa renk yerine lightgray yazılabilir)
    DrawTexturePro(floorTexture, source, dest, origin, 0.0f, WHITE);
}