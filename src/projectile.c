
#include "game.h"
#include "collision.h"
#include <math.h>
#include "projectile.h"


// mermi mekaniği boş mermi varmı diye kontrol edip ateşleme
void Projectile_FireBullet(GameData *game, Vector2 targetPos){

    for (int i = 0; i < MAX_BULLETS; i++){
        if(!game->bullets[i].active){   //  mermi aktif kullanılıyor mu

            game->bullets[i].active = true;   //  artık kullanılıyor
            game->bullets[i].pos = game->player.pos; //  oyuncunun olduğu yerden çıkıcak
            game->bullets[i].speed = 300.0f;   //  mermi hızı
            game->bullets[i].radius = 4.0f;   //  mermi büyüklüğü
            game->bullets[i].scale = 1.0f;

            //  animasyon başlangıç değerleri
            game->bullets[i].currentFrame = 0;
            game->bullets[i].frameCounter = 0;
            game->bullets[i].facingRight = (targetPos.x > game->player.pos.x);
            game->bullets[i].frameSpeed = 10;   //  animasyon hızı

            //  hedef yönü hesaplama(düşman takip mantığının aynısı)
            float dx = targetPos.x - game->player.pos.x;
            float dy = targetPos.y - game->player.pos.y;
            float distance = sqrt(dx * dx + dy * dy);


            // texture genişliğini kare sayısına böl
            float frameWidth = (float)game->assets.bulletTexture.width / PROJECTILE_FRAME_COUNT;
            game->bullets[i].frameRec = (Rectangle) {0.0f, 0.0f, frameWidth, (float)game->assets.bulletTexture.height};

            
            //  sıfıra bölünme hatasını engelleme
            if(distance > 0){
                //  birim vektöre dönüştürme
                game->bullets[i].dir.x = dx / distance;
                game->bullets[i].dir.y = dy / distance;

                //  mermi sağa mı gidiyo
                game->bullets[i].facingRight = (dx > 0);
            }
            else {
                game->bullets[i].dir = (Vector2){1, 0};
                game->bullets[i].facingRight = true;
            }
            break;  //  bir mermi için yapıp döngüden çıkıyoruz
        }

    }
    
}



void Projectile_UpdateBullet(GameData *game, float dt){
            //  yeni mermileri güncelleme
            for (int i = 0; i < MAX_BULLETS; i++){
                if(!game->bullets[i].active) continue;
                   
                    Projectile_UpdatePysics(&game->bullets[i], dt);

                    Projectile_UpdateAnimation(&game->bullets[i], game->assets.bulletTexture.width);

            }
                //  mermi ve düşman çarpışması kontrol etme fonksiyonu
            Collisions_BulletEnemy(game);   
}


void Projectile_DrawBullet(const GameData *game){


             //  mermilerin çizimi
            for (int i = 0; i < MAX_BULLETS; i++){
                if(!game->bullets[i].active) continue;


                    Rectangle source = game->bullets[i].frameRec; //  mevcut pencereyi kopyala


                    //  texture ölçeklendirmesi
                    float singleFrameWidth = game->assets.bulletTexture.width / PROJECTILE_FRAME_COUNT ;
                    float scaledWidth = singleFrameWidth * game->bullets[i].scale;
                    float scaledHeight = game->assets.bulletTexture.height * game->bullets[i].scale;


                    //  yön kontrolü
                    if(game->bullets[i].facingRight){

                        if(source.width < 0) source.width *= -1;
                    }

                    else {
                        if(source.width > 0) source.width *= -1;
                    }



                    //  mermi ekranda nereye çizilecek
                    Rectangle dest = {
                        game->bullets[i].pos.x,
                        game->bullets[i].pos.y,
                        (float)scaledHeight,
                        (float)scaledWidth
                    };

                    //  texture ü merkeze al
                    Vector2 origin = {scaledWidth / 2, scaledHeight / 2};

                    DrawTexturePro(game->assets.bulletTexture, source, dest, origin, 0.0f, WHITE);
                

            }
}






void Projectile_UpdateAnimation(Projectile *bullet, int textureWidth){
    bullet->frameCounter++;

    //  frame speede göre değiştir
    if(bullet->frameCounter >= (60 / bullet->frameSpeed)){
        bullet->frameCounter = 0;
        bullet->currentFrame++;
    }

    //  maks frame sayısına gelince başa döndür
    if(bullet->frameCounter >= PROJECTILE_FRAME_COUNT){
        bullet->frameCounter = 0;
    }

    //  sprite sheet üzerinde pencereryi kaydır
    int frameWidth = textureWidth / PROJECTILE_FRAME_COUNT;
    bullet->frameRec.x = (float)bullet->currentFrame * frameWidth;

}


void Projectile_UpdatePysics(Projectile *bullet, float dt){

    //  mermiyi ilerlet
    bullet->pos.x += bullet->dir.x * bullet->speed * dt;
    bullet->pos.y += bullet->dir.y * bullet->speed * dt;

    //  mermi ekran dışına çıktı mı kontrol et
    if(bullet->pos.x < 0 || bullet->pos.x > SCREEN_WIDTH || bullet->pos.y < 0 || bullet->pos.y > SCREEN_HEIGHT){
        bullet->active = false; //  mermiyi yok et
    }
}

