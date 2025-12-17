#include "defines.h"
#include "gem.h"
#include "game.h"
#include <math.h>




void Gem_Init(GameData *game){
    game->gems->active = false;
    game->gems->color = YELLOW;
}


void Gem_UpdateGems(GameData *game){
    for (int i = 0; i < MAX_GEM; i++){
        //  gem aktif değilse döngüye girme
        if(!game->gems[i].active) continue;

        //  oyuncu taşı topladı mı
        float dx = game->player.pos.x - game->gems[i].pos.x;
        float dy = game->player.pos.y - game->gems[i].pos.y;
        float dist = sqrt(dx * dx + dy * dy);

        //  oyuncuya doğru çekilme manyetiklik efekti
        if(dist < 100) {
            game->gems[i].pos.x += (dx / dist) * 5.0f;
            game->gems[i].pos.y += (dy / dist) * 5.0f;
        }
        //  toplama anı
        if(dist < game->player.radius) {
            game->gems[i].active = false;   //  xp yi yoket
            game->currentXP += game->gems[i].value; //  mevcut xp ye alınan xp değeri kadar ekle

            //  level atlama kontrolü
            if(game->currentXP >= game->requiredXP){
                game->level++;
                game->currentXP -= game->requiredXP; //  artan leveli sonraki levele aktarma
                game->requiredXP = (int)game->requiredXP * XP_INTERVAL; //  level atlamayı zorlaştır
                // buraya level atlama ödülü olarak can yinelem veya başka bişey gelebilir
                //  level atlama sesi de gelebilir
            }

        }

    }
    
}


void Gem_SpawnGem(GameData *game, Vector2 position, int value){
    for (int i = 0; i < MAX_GEM; i++){
        if(!game->gems[i].active){
            game->gems[i].active = true;
            game->gems[i].pos = position;
            game->gems[i].value = value;
            break;
        }
    }
    
}
