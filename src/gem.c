#include "defines.h"
#include "gem.h"
#include "game.h"
#include <math.h>
#include "raymath.h"




void Gem_Init(GameData *game){

    for (int i = 0; i < MAX_GEM; i++){
        if(!game->gems[i].active) continue;
        game->gems[i].active = false;
        game->gems[i].rotation = 0.0f;
        game->gems[i].isMagnetized = false;
        game->gems[i].texture = game->assets.gemTexture;
    }
}



void Gem_UpdateGems(GameData *game, float dt){
    for (int i = 0; i < MAX_GEM; i++){
        //  gem aktif değilse döngüye girme
        if(!game->gems[i].active) continue;

        //  sürekli yavaşça dönsün
        game->gems[i].rotation += 2.0f * dt;

        float distance = Vector2Distance(game->gems[i].pos, game->player.pos);

        //  oyuncu taşı topladı mı
        float dx = game->player.pos.x - game->gems[i].pos.x;
        float dy = game->player.pos.y - game->gems[i].pos.y;
        float dist = sqrt(dx * dx + dy * dy);

        //  oyuncuya doğru çekilme manyetiklik efekti
        if(dist < 150) {
            game->gems[i].pos.x += (dx / dist) * 5.0f;
            game->gems[i].pos.y += (dy / dist) * 5.0f;
            game->gems[i].isMagnetized = true;
        }

        else {
            game->gems[i].isMagnetized = false;
        }

        //  toplama anı
        if(dist < game->player.radius) {
            game->gems[i].active = false;   //  xp yi yoket
            game->currentXP += game->gems[i].value; //  mevcut xp ye alınan xp değeri kadar ekle

        }

    }
    
}


void Gem_SpawnGem(GameData *game, Vector2 position, int value){
    for (int i = 0; i < MAX_GEM; i++){
        if(!game->gems[i].active){
            game->gems[i].active = true;
            game->gems[i].pos = position;
            game->gems[i].value = value;
            game->gems[i].rotation = 0.0f;
            game->gems[i].isMagnetized = false;
            game->gems[i].texture = game->assets.gemTexture;
            break;
        }
    }
    
}



void Gem_DrawGem(const GameData *game){
    Texture2D texture = game->assets.gemTexture;
    
    //  texture merkezini bul
    Vector2 origin = {(float)texture.width / 2, (float)texture.height / 2};
    float scale = 0.3f;

    for (int i = 0; i < MAX_GEM; i++){
        //  aktif olmayanları geç
        if(!game->gems[i].active) continue;

        const Gem *g = &game->gems[i];

        if(g->isMagnetized){
            
            //  parlama efekti içinn
            BeginBlendMode(BLEND_ADDITIVE);

            //  oyuncuya giden yönün tersini bul efekt için
            Vector2 dirToPlayer = Vector2Subtract(game->player.pos, g->pos);
            dirToPlayer = Vector2Normalize(dirToPlayer);
            Vector2 trailDir = Vector2Scale(dirToPlayer, -1.0f);    // Ters yön

            //  arkasına 3 tane azalarak giden kopya çiz
            for (int k = 1; k <= 3; k++){
                Vector2 trailPos = Vector2Add(g->pos, Vector2Scale(trailDir, k * 0.8f));

                float trailAlpha = 0.6f - (k * 0.15f);
                float trailScale = scale * (1.0f - k * 0.2f);

                DrawTexturePro(texture, 
                    (Rectangle){0, 0, texture.width, texture.height},
                    (Rectangle){trailPos.x, trailPos.y, texture.width * trailScale, texture.height * trailScale},
                    origin,
                    g->rotation - (k * 10),
                    Fade(GOLD, trailAlpha));
            }
            
            //  moddan çık
            EndBlendMode();
        }


        //  ana gem çizimi
        Rectangle dest = { g->pos.x, g->pos.y, texture.width * scale, texture.height * scale};

        //  gem texture çizimi
        DrawTexturePro(texture,
            (Rectangle){0, 0, texture.width, texture.height},   
            dest,
            origin,
            g->rotation,
            WHITE); 
    }
}