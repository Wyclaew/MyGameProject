#ifndef PLAYER_H
#define PLAYER_H


#include "defines.h"


void Player_Update(Player *player, float dt);    //  oyuncu hareketi hesaplanması
void Player_Draw(const Player *player);  //  oyuncuyu çizdirme
void Player_Init(Player *player, GameAssets *assets);    //  oyuncunun başlama kodları 
void Player_Camera_Update(GameData *game);  //  kameranın konumunu güncelle



#endif