#include "collision.h"
#include "game.h"
#include "gem.h"
#include "enemy.h"


 //  mermi ve düşman çarpışma güncelleyicisi
void Collisions_BulletEnemy(GameData *game){
        //  mermileri tek tek gez
        for (int i = 0; i < MAX_BULLETS; i++){
            //  eğer mermi aktif ekranda değilse o mermiyi direkt geçiyor (optimizasyon)
            if(!game->bullets[i].active) continue;

            //  düşmanları gez
            for (int j = 0; j < MAX_ENEMIES; j++){

                //  eğer düşman ölüyse mermi ona çarpmaz
                if(!game->enemies[j].active) continue;

                //  çarpışma anı 
                //  mermi ile düşman birbirlerine değdi mi ?
                if(CheckCollisionCircles(game->bullets[i].pos, game->bullets[i].radius, game->enemies[j].pos, ENEMY_RADIUS)){

                    //  vurulma anı mermiyi yoket
                    game->bullets[i].active = false;

                    //  ilerdide bullet damage eklenebilir şimdilik sabit 10
                    game->enemies[j].health -= 10.0f;

                    //  ölüm kontrolü
                    if(game->enemies[j].health <= 0){
                        //  düşmanı yoket
                        game->enemies[j].active = false;

                        //  düşmanın türüne göre tablodan xp değerini çek
                        EnemyType type = game->enemies[j].type;
                        
                        //  düşmanı spawn ederken direkt olarak xp değeri verdiğimiz için burda bişey yapmicaz daha
                        int xpReward = game->enemies[j].xpReward;

                        //  gem düşürme fonksiyonunu çağırıyoruz
                        Gem_SpawnGem(game, game->enemies[j].pos, xpReward);

                        //  tank öldürünce 2 puan gelsin
                        //  sonradan her düşman için özel puan ekleyebiliriz
                        game->score += (type == ENEMY_TANK) ? 2 : 1;
                    }
                    break;
                }
         }   
    }
}

void Collisions_PlayerEnemy(GameData *game){
            //  düşman çarpışma kontrolü
            for (int i = 0; i < MAX_ENEMIES; i++){

                //  düşman aktif değilse kontrol etme
                if(!game->enemies[i].active) continue;

                //  oyuncu ile düşman biribirine değdi mi
                if (CheckCollisionCircles(game->player.pos, game->player.radius, game->enemies[i].pos, ENEMY_RADIUS)){

                game->player.health -= 10;
                
                //  oyuncunun canı bitti mi
                if (game->player.health <= 0) {
                //  skoru kontrol et rekor mu diye
                Game_CheckAndSaveScore(game, game->score);

                //  mevcut oyun modu game over
                game->currentState = GAME_OVER;
                }
                //  oyun bittiği için döngüden çıkabiliriz
                break;
            }
                
      }
}

    
void Collisions_CheckAll(GameData *game){
        Collisions_BulletEnemy(game);   //  mermi ve düşman çarpıştı mı kontrolü
        Collisions_PlayerEnemy(game);   // oyuncu ve düşman çarpıştı mı kontrolü

}