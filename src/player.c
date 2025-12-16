
#include "player.h"



void Player_Init(Player *player, GameAssets *assets){
    player->pos = (Vector2){400, 300};
    player->speed = 300.0f;
    player->radius = 20.0f;
    player->scale= 1.5f;
    player->currentFrame = 0;
    player->framesCounter = 0;
    player->frameSpeed = 8;
    player->facingRight = true;
    player->texture = assets->playerTexture;
    

    //  resmin toplam genişliğini kare sayısına bölüyoruz
    float frameWidth = (float) player->texture.width / 8;
    player->frameRec = (Rectangle){0.0f, 0.0f, frameWidth, (float)player->texture.height};


}

void Player_Update(Player *player, float dt){



            //  hareket ediyor mu kontrolü
            bool isMoving = false;

            //  yukarı
            if(IsKeyDown(KEY_W)) {player->pos.y-= player->speed * dt; isMoving = true;}

            // aşağı
            if(IsKeyDown(KEY_S)) {player->pos.y += player->speed * dt; isMoving = true;}

            // sağa
            if(IsKeyDown(KEY_D)) {player->pos.x += player->speed * dt; isMoving = true; player->facingRight = true;}

            //  sola
            if(IsKeyDown(KEY_A)) {player->pos.x -= player->speed * dt; isMoving = true; player->facingRight = false;}


            //  animasyon mantığı

            if(isMoving){
                player->framesCounter++; //  frame sayacını artırmaya başlayacağız

                //  sayaç kare sınırına geldi mi (yani mesela 8 karede bir değiştiriceksek 8 e geldi mi)
                if(player->framesCounter >= (60/player->frameSpeed)){

                    //  sayacı sıfırla
                    player->framesCounter = 0;

                    //  sonraki frame e geç
                    player->currentFrame++;
                    
                    //  8 frame olduğu çin 8 e geldi mi yani film şeridi bitti mi
                    if(player->currentFrame > 7) player->currentFrame = 0;

                    //  pencereyi sağa kaydır
                    player->frameRec.x = (float)player->currentFrame * player->texture.width / 8;
                }
            }

            else{
                //  eğer ismoving = false sa yani if in içine girmediyse duruyorsa ilk kareyi göster
                player->currentFrame = 0;
                player->frameRec.x = 0;

            }



            //  ekranın solundan dışarıya çıkamaması için
            if(player->pos.x < PLAYER_BOUNDARY) player->pos.x = PLAYER_BOUNDARY;

            //  sağından çıkamaması için
            if(player->pos.x > SCREEN_WIDTH - PLAYER_BOUNDARY) player->pos.x = SCREEN_WIDTH - PLAYER_BOUNDARY;

            //  yukardan çıkmaması için
            if(player->pos.y < PLAYER_BOUNDARY) player->pos.y = PLAYER_BOUNDARY;

            //  aşağıdan çıkmaması için
            if(player->pos.y > SCREEN_HEIGHT - PLAYER_BOUNDARY) player->pos.y = SCREEN_HEIGHT - PLAYER_BOUNDARY;
            //  hareket mantığı ve oyun içi
            //  IsKeyDown: tuşa basılı tutulduğu sürece true döner
}


void Player_Draw(const Player *player){
        //  oyuncumuzun çizimi
            //  yön kontrolü
            Rectangle source = player->frameRec;  //  mevcut pencereyi kopyala

            //  texture ölçeklendirmesi için
            float singleFrameWidth = (float)player->texture.width / PLAYER_FRAME_COUNT;
            float scaledWidth = singleFrameWidth * player->scale;
            float scaledHeight = player->texture.height * player->scale;

            if(player->facingRight){
                //  normal genişlik
                if(source.width < 0) source.width *= -1;
            } 

            else {
                //  aynalanmış genişlik yani sola bak
                if(source.width > 0) source.width *= -1;
            }

            // oyuncumuz ekranda nereye çizilecek
            
            Rectangle dest ={
                player->pos.x,
                player->pos.y,
                scaledHeight,
                scaledWidth
            };

            //  texture ü merkeze al
            Vector2 origin = {scaledWidth / 2, scaledHeight / 2};

            //  oyuncuyu çizdirme
            DrawTexturePro(player->texture, source, dest, origin, 0.0f, WHITE);
}

