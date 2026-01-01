#ifndef ENEMY_H
#define ENEMY_H


#include "defines.h"

void Enemy_Init(GameData *game);    //  düşmanların başlangıcı
void Enemy_Update(GameData *game, float dt);    //  düşman yapay zekası
void Enemy_UpdateAll(GameData *game, float dt);   //  düşmanların hareketlerinin hesaplanması
void Enemy_Draw(const GameData *game);   //  düşman çizdirme
void Enemy_DrawAll(const GameData *game);
void Enemy_Spawn(GameData *game);   //  düşman spawnlama
void Enemy_Reset(GameData *game);    //  düşmanları resetleme
void Enemy_PushAwayFromPlayer(GameData *game, float safeRadius);    //  upgrad aldıktan sonra direkt ölmeyi engellemek içim




#endif