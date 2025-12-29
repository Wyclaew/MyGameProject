#ifndef GEM_H
#define GEM_H

#include "defines.h"
#include "game.h"
#include "enemy.h"

void Gem_UpdateGems(GameData *game, float dt);
void Gem_Init(GameData *game);
void Gem_SpawnGem(GameData *game, Vector2 position, int value);
void Gem_DrawGem(const GameData *game);








#endif