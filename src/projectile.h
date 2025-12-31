#ifndef PROJECTILE_H
#define PROJECTILE_H


#include "defines.h"



void Projectile_FireBullet(GameData *game, Vector2 targetPos);  //  mermi ateşle
void Projectile_DrawBullet(const GameData *game);  //  mermi çizdir
void Projectile_UpdateBullet(GameData *game, float dt);   //  mermilerin hareketlerinin hesaplanması
void Projectile_UpdateAnimation(Projectile *bullet, int textureWidth);   //  animasyon matematiği
void Projectile_UpdatePysics(Projectile *bullet, float dt, Vector2 playerPos);  //  hareket matematiği





#endif