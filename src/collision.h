#ifndef COLLISIONS_H
#define COLLISIONS_H


#include "defines.h"

//  çarpışma kontrolleri
void Collisions_CheckAll(GameData *game); //  çarpışmaların olup olmadığının kontrolü
void Collisions_BulletEnemy(GameData *game);
void Collisions_PlayerEnemy(GameData *game);



#endif