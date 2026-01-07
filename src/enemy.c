#include "game.h"
#include "enemy.h"
#include <math.h>
#include "spatial_grid.h"
#include "raymath.h"


//  tüm düşman istatistikleri (kayıt defteri)
const EnemyDef ENEMY_DEFS[ENEMY_TYPE_COUNT] = {
    [ENEMY_BASIC] = {
        .baseHealth = 30.0f,
        .baseSpeed = 120.0f,
        .scale = 1.0f,
        .radius = 12.0f,
        .tint = WHITE,
        .xpValue = 1000
    },
    [ENEMY_RUSHER] = {
        .baseHealth = 10.0f,
        .baseSpeed = 250.0f,
        .scale = 0.7f,
        .radius = 8.0f,
        .tint = (Color){255, 100, 100, 255},
        .xpValue = 2000
    },
    [ENEMY_TANK] = {
        .baseHealth = 150.0f,
        .baseSpeed = 60.0f,
        .scale = 1.5f,
        .radius = 20.0f,
        .tint = (Color){100, 255, 100, 255},
        .xpValue = 5000
    }
};


//  başlangıçta enemy dizisini temizleme
void Enemy_Init(GameData *game){
    //  tüm düşmanlar başlangıçta kapalı olucak
    for (int i = 0; i < MAX_ENEMIES; i++){
        game->enemies[i].scale = 1.0f;  //  düşman büyüklüğü ölçeği
        game->enemies[i].active = false;    //  aktifliğini sıfırlıyoruz

         //  animasyon başlangıç ayarları
        game->enemies[i].currentFrame = 0;  //  o anki frame ini sıfırlıyoruz
        game->enemies[i].frameCounter = 0;  //  frame sayacını sıfırlıyoruz
        game->enemies[i].facingRight = true;    //  yön kontolcüsünü sıfırlıyoruz

        //  alttaki rastgele düşman seçicide hata çıkarsa oyunun çökmemesi için   
        EnemyType selectedType = ENEMY_BASIC;   //  default olarak tüm düşmanlar basic yapıyoruz
        const EnemyDef *def = &ENEMY_DEFS[selectedType];    //  yukarıdaki tablodan düşman verilerini çekmek için isim kısaltması yapıyoruz
        game->enemies[i].health = def->baseHealth;  //  düşman canını seçilen düşman tipinin canı olarak güncelliyoruz
        game->enemies[i].maxHealth = def->baseHealth;   //  ilerisi için düşmanları giderek güçlendirme vs
        game->enemies[i].type = selectedType;   //  yine ilerisi için düşman tipi ataması yapıyoruz
        game->enemies[i].xpReward = def->xpValue;   //  düşman ölünce çıkacak xp değeri

        Spatial_Clear();    //  düşman çarpışmaları için olan ızgarayı temizliyoruz
        
    }
}

//  tek bir düşman yaratma
void Enemy_Spawn(GameData *game){
    int roll = GetRandomValue(1, 100);
    EnemyType selectedType = ENEMY_BASIC;

    if(roll > 90) selectedType = ENEMY_TANK;    //  %10 ihtimalle tank
    else if(roll > 70) selectedType = ENEMY_RUSHER; //  %20 ihtimalle rusher
    else selectedType = ENEMY_BASIC;    //  %70 basic

    //  gelen düşmanın texturünü ayarla
    Texture2D currentTex = game->assets.enemyTextures[selectedType];

    //  yukardaki taslağı al
    const EnemyDef *def = &ENEMY_DEFS[selectedType];

    //  boş slot bul ve doldur 
    for (int i = 0; i < MAX_ENEMIES; i++){
        if(!game->enemies[i].active){
            //  temel durumu değiştir
            game->enemies[i].active = true;
            game->enemies[i].type = selectedType;

            //  aldığımız taslaktaki verileri kopyala
            game->enemies[i].health = def->baseHealth;
            game->enemies[i].maxHealth = def->baseHealth;
            game->enemies[i].speed = def->baseSpeed;
            game->enemies[i].scale = def->scale;
            game->enemies[i].color = def->tint;
            game->enemies[i].xpReward = def->xpValue;

            //  texture bağlama
            game->enemies[i].texture = game->assets.enemyTextures[selectedType];

            //  halka olarak spawnla
            //  DEG2RAD degree to radian yani dereceyi radyana çeviriyoruz çünkü bilgisayar öyle istiyor
            float angle = GetRandomValue(0, 360) * DEG2RAD;
            float margin = 200.0f;

            //  ekranın yarısından biraz fazla uzağa koyuyuoruz
            float distance = (SCREEN_WIDTH / 2) + margin;
            
            game->enemies[i].pos.x = game->player.pos.x + cosf(angle) * distance;
            game->enemies[i].pos.y = game->player.pos.y + sinf(angle) * distance;
            
            //  animasyon hazırlığı
            game->enemies[i].currentFrame = 0;
            game->enemies[i].frameCounter = 0;
            game->enemies[i].facingRight = true;
            game->enemies[i].frameSpeed = ENEMY_FRAME_COUNT;


            //  texture karesi hesapla
            float frameWidth = (float)currentTex.width / ENEMY_FRAME_COUNT;
            game->enemies[i].frameRec = (Rectangle){0.0f, 0.0f, frameWidth, (float)currentTex.height};
            break;
        }
    }
}


//  düşmanları ekrana çizme
void Enemy_Draw(const GameData *game){
            //  düşman çizimi
            for (int i = 0; i < MAX_ENEMIES; i++){  
                //  sadece aktif düşmanları çiz
                if(!game->enemies[i].active) continue;
                Texture2D currentTex = game->assets.enemyTextures[game->enemies[i].type];

                //  gölge çizimi
                float shadowW = 20.0f * game->enemies[i].scale;
                float shadowH = 8.0f * game->enemies[i].scale;

                //  ayaklarının tam altında
                Vector2 shadowPos = {
                    game->enemies[i].pos.x,
                    game->enemies[i].pos.y + (currentTex.height * game->enemies[i].scale / 2.0f) - 5.0f
                };

                DrawEllipse((int)shadowPos.x, shadowPos.y, shadowW, shadowH, Fade(BLACK, 0.4f));

                //  yüklediğimiz texture ün neresi kesilecek
                Rectangle source = game->enemies[i].frameRec;

                //  texture öçeklendirmesi
                float singleFrameWidth = (float)currentTex.width / ENEMY_FRAME_COUNT;
                float scaledWidth = singleFrameWidth * game->enemies[i].scale;
                float scaledHeight = currentTex.height * game->enemies[i].scale;


                if(game->enemies[i].facingRight){ //  oyuncu sağa bakıyorsa
                    if(source.width < 0) source.width *= -1;    //  ve oyuncunun sağındaysa değeri ters çevir
                }
                
                else{
                    if(source.width > 0) source.width *= -1;
                }

                Rectangle dest = {
                    game->enemies[i].pos.x, //  texture oyun içinde hangi x kordinatında olucak
                    game->enemies[i].pos.y, //  texture oyun içinde hangi y kordinatında olucak
                    (float)scaledWidth, //  texture ün oyun içindeki genişliği ne kadar olucak
                    (float)scaledHeight  //  texture ün oyun içindeki yüksekliği ne kadar olucak
                };

                //  texture ü merkeze al
                Vector2 origin = {scaledWidth / 2, scaledHeight / 2};

                //  düşmanı cizdirme (0.0f döndürme açısı) resmin orijinal renklerini korumak için renge white yazdık
                DrawTexturePro(currentTex, source, dest, origin, 0.0f, WHITE);

            }
}



void Enemy_Update(GameData *game, float dt){

            //  girdi temizle
            Spatial_Clear();
            for (int i = 0; i < MAX_ENEMIES; i++){
                if(game->enemies[i].active){
                    Spatial_Add(&game->enemies[i], i);
                }
            }
            

            //  döngü ile ekrandaki tüm düşmanları tek tek kontrol et
            for (int i = 0; i < MAX_ENEMIES; i++){

                //  eğer düşman aktif değilse çizmeye devam etme
                if(!game->enemies[i].active) continue;

                Enemy *e = &game->enemies[i];

                //  mevcut düşmanın texture ünü alıyoruz
                Texture2D currentTex = game->assets.enemyTextures[e->type];

                //  düşman çarpışma engelleyici
                Vector2 separationForce = {0, 0};
                int neighobrs = 0;

                //  hangi hücredeyim
                int cellX = (int)(e->pos.x / CELL_SIZE);
                int cellY = (int)(e->pos.y / CELL_SIZE);

                //  komşu hücreleri kontrol et (3x3 alandaki kendisi dahil)
                for(int offsetX = -1; offsetX <= 1; offsetX++) {
                    for (int offsetY = -1; offsetY < 1; offsetY++){
                        int hash = Spatial_GetGridHash(cellX + offsetX, cellY + offsetY);

                        //  bu hücredeki tüm düşmanları kontrol et
                        for (int k = 0; k < gridCounts[hash]; k++) {
                            int otherIdx = grid[hash][k];

                            if(i == otherIdx) continue;    //  kendinle çarpışma

                            Enemy *other = &game->enemies[otherIdx];

                            //  basit mesafe kontrolü
                            float dist = Vector2Distance(e->pos, other->pos);
                            float combinedRadius = (15.0f * e->scale) + (15.0f * other->scale);   //  radius tahmini

                            if(dist < combinedRadius) {
                                //  itme vektörü
                                Vector2 push = Vector2Subtract(e->pos, other->pos);
                                push = Vector2Normalize(push);

                                //  ne kadar yakınsa o kadar sert it
                                float strength = 1.0f - (dist / combinedRadius);
                                separationForce = Vector2Add(separationForce, Vector2Scale(push, strength));
                                neighobrs++;
                            }
                        } 
                    }
                }

                //  itme kuvvetini uygula
                if(neighobrs > 0) {
                    float separationSpeed = 150.0f; //  itme şiddeti
                    e->pos.x += separationForce.x * separationSpeed * dt;
                    e->pos.y += separationForce.y * separationSpeed * dt; 
                }

                //  düşman ve oyuncu arasındaki y mesafesi
                float dx = game->player.pos.x - game->enemies[i].pos.x;

                //  düşman ve oyuncu arasındaki x mesafesi
                float dy = game->player.pos.y - game->enemies[i].pos.y;

                //  düşman ve oyuncu arasındaki mesafeyi bul
                float enemyDistance = sqrt(dx * dx + dy * dy);

                /*  hareket normalizasyonu eğer mesafe sıfır ise yani üst üstelerse
                işlem yaptırmayacağız yoksa 0'a bölme hatası alırız */
                if(enemyDistance > 0){
                    /*  !! düşman pozisyonuna direkt olarak dx ve dy yi eklersek düşman bize ne kadar
                    yakın olursa o kadar yavaş ne kadar uzak olursa da o kadar hızlı gelirdi*/
                    //  birim vektör elde etme
                    game->enemies[i].pos.x += (dx / enemyDistance) * game->enemies[i].speed * dt;
                    game->enemies[i].pos.y += (dy / enemyDistance) * game->enemies[i].speed * dt;

                    //  animasyon mantığı
                    //  düşmanla oyuncu arasındaki x değeri pozitifse yani oyuncu sağda kalıyosa sağa bak
                    if(dx > 0) game->enemies[i].facingRight = true;
                    else game->enemies[i].facingRight = false;  //  oyuncu solda kalıyosa sağa bakma sola bak

                    //  kare değiştirme hız ayarı
                    game->enemies[i].frameCounter++;


                    if(game->enemies[i].frameCounter >= (60 / game->enemies[i].frameSpeed)){
                        game->enemies[i].frameCounter = 0;
                        game->enemies[i].currentFrame++;
                    }

                    //  6 frame olduğu için 5 dedik 8 frame olsa 7 diceyecektik
                    if(game->enemies[i].currentFrame > ENEMY_FRAME_COUNT - 1){
                        game->enemies[i].currentFrame = 0;
                    }

                    float frameWidth = (float)currentTex.width / ENEMY_FRAME_COUNT;
                    game->enemies[i].frameRec.x = (float)game->enemies[i].currentFrame * frameWidth;
                }
         }
}


void Enemy_UpdateAll(GameData *game, float dt){
        Enemy_Update(game, dt); //  düşman ai update
}



//  oyun bitip yeniden başlarken düşmanları sıfırlar
void Enemy_Reset(GameData *game){
    //  bu fonksiyonla ayn işi yapar yani tüm düşmanları siler
    Enemy_Init(game);
}

//  upgrade seçtikten sonra direkt ölünmemesi için düşmanları uzaklaştırma
void Enemy_PushAwayFromPlayer(GameData *game, float safeRadius) {
    for (int i = 0; i < MAX_ENEMIES; i++){
        if(!game->enemies[i].active) continue;

        //  düşmanın oyuncuya olan mesafesini ölç
        float dx = game->enemies[i].pos.x - game->player.pos.x;
        float dy = game->enemies[i].pos.y - game->player.pos.y;
        float dist = sqrt(dx * dx + dy * dy);

        //  eğer oyuncunun güvenli alanına çok yakınsa
        if(dist < safeRadius) {

            float dirX = dx / dist;
            float dirY = dy / dist;

            //  düşmanı güvenli alanın sınırına ışınla
            game->enemies[i].pos.x = game->player.pos.x + (dirX * safeRadius);
            game->enemies[i].pos.y = game->player.pos.y + (dirY * safeRadius);
        }
    }
    
}