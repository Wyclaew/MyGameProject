#include "game.h"
#include "enemy.h"
#include <math.h>



//  başlangıçta enemy dizisini temizleme
void Enemy_Init(GameData *game){
    //  tüm düşmanlar başlangıçta kapalı olucak
    for (int i = 0; i < MAX_ENEMIES; i++){
        game->enemies[i].scale = 1.0f;
        game->enemies[i].currentFrame = 0;
        game->enemies[i].active = false;

         //  animasyon başlangıç ayarları
        game->enemies[i].currentFrame = 0;
        game->enemies[i].frameCounter = 0;
        game->enemies[i].facingRight = true;


        //  texture genişliğini kare sayısıan bölüyoruz
        float frameWidth =(float)game->assets.enemyTexture.width / ENEMY_FRAME_COUNT;
        game->enemies[i].frameRec = (Rectangle) {0.0f, 0.0f, frameWidth, (float)game->assets.enemyTexture.height};
    }
}

//  tek bir düşman yaratma
void Enemy_Spawn(GameData *game){
    for (int i = 0; i < MAX_ENEMIES; i++){
        if (!game->enemies[i].active){
            game->enemies[i].active = true; //  düşmanı canlandır
            game->enemies[i].pos = Game_GetSafeSpawnPosition(game->player.pos, 240.0f); //  oyuncudan uzak bir yer bul
            game->enemies[i].speed = (float)GetRandomValue(100, 200);   //  rastgele bir hız değeri seç
            game->enemies[i].color = RED;   //  düşmanın rengini belirle
            break;  //  bir düşman doğurup döngüden çıkıyoruz
           
        }
        
    }
    
}


//  düşmanları ekrana çizme
void Enemy_Draw(const GameData *game){
            //  düşman çizimi
            for (int i = 0; i < MAX_ENEMIES; i++){  
                //  sadece aktif düşmanları çiz
                if(!game->enemies[i].active) continue;

                //  yüklediğimiz texture ün neresi kesilecek
                Rectangle source = game->enemies[i].frameRec;

                //  texture öçeklendirmesi
                float singleFrameWidth = game->assets.enemyTexture.width / ENEMY_FRAME_COUNT;
                float scaledWidth = singleFrameWidth * game->enemies[i].scale;
                float scaledHeight = game->assets.enemyTexture.height * game->enemies[i].scale;


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

                //  düşmanı cizdirme    (0.0f döndürme açısı) resmin orijinal renklerini korumak için renge white yazdık
                DrawTexturePro(game->assets.enemyTexture, source, dest, origin, 0.0f, WHITE);
            }
        
}



void Enemy_Update(GameData *game, float dt){
    

            //  döngü ile ekrandaki tüm düşmanları tek tek kontrol et
            for (int i = 0; i < MAX_ENEMIES; i++){

                //  eğer düşman aktif değilse çizmeye devam etme
                if(!game->enemies[i].active) continue;

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
                    if(game->enemies[i].currentFrame > 5){
                        game->enemies[i].currentFrame = 0;
                    }

                    game->enemies[i].frameRec.x = (float)game->enemies[i].currentFrame * (game->assets.enemyTexture.width / 6);

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