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
        game->gems[i].texture = LoadTexture("xp_gem.png");
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
        if(dist < 100) {
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

            //  level atlama kontrolü
            if(game->currentXP >= game->requiredXP){
                game->level++;
                game->currentXP -= game->requiredXP; //  artan leveli sonraki levele aktarma
                game->requiredXP = (int)game->requiredXP * XP_INTERVAL; //  level atlamayı zorlaştır

                //  upgrade kartlarını hazırla
                Game_GenerateUpgrades(game);    //  kartları hazırla
                game->currentState = LEVEL_UP;  //  oyunu dondur ve menüyü aç
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
            game->gems[i].rotation = 0.0f;
            game->gems[i].isMagnetized = false;
            break;
        }
    }
    
}


void Gem_DrawGem(const GameData *game){
    Texture2D texture = game->assets.gemTexture;
    
    //  texture merkezini bul
    Vector2 origin = {(float)texture.width / 2, (float)texture.height / 2};
    float scale = 1.0f;

    //  parlama efekti 
    //  üst üste binen renkleri toplayarak ışık gibi görünmesini sağlıyor
    BeginBlendMode(BLEND_ADDITIVE);

    for (int i = 0; i < MAX_GEM; i++){
        if(!game->gems[i].active) continue;

        const Gem *g = &game->gems[i];

        //  iz efekti
        if(g->isMagnetized){
            //  oyuncuya giden yönün tersini bul
            Vector2 dirToPlayer = Vector2Subtract(game->player.pos, g->pos);
            dirToPlayer = Vector2Normalize(dirToPlayer);
            Vector2 trailDir = Vector2Scale(dirToPlayer, -1.0f);    //  ters yön

            //  arkasına 3 tane azalarak sönen kopya çiz
            for (int k = 1; k <= 3; k++){
            Vector2 trailPos = Vector2Add(g->pos, Vector2Scale(trailDir, k * 0.8f));    //  8 piksel arayla

            //  gittikçe şeffaflaşan ve küçülen kopyalar
            float trailAlpha = 0.6f - (k * 0.15f);
            float trailScale = scale * (1.0f - k * 0.2f);

            //  izi çiz
            DrawTexturePro(texture, 
            (Rectangle){0, 0, texture.width, texture.height},
            (Rectangle){trailPos.x, trailPos.y, texture.width * trailScale, texture.height * trailScale},
            origin,
            g->rotation - (k * 10), //  izin dönüşü hafif arkadan geliyor
            Fade(GOLD, trailAlpha));

            }

            //  ana gem çizimi
            //  ekranda nereye ne boyutta çizilecek
            Rectangle dest = { g->pos.x, g->pos.y, texture.width * scale, texture.height * scale};

            //  kendisini çiz
            DrawTexturePro(texture,
            (Rectangle){0, 0, texture.width, texture.height},   //  kaynak tüm resim
            dest,
            origin,
            g->rotation,
            WHITE); //  orijinal renk
            
        }
    }
    //  planlama modunu kapat
    EndBlendMode();
}
