#include "collision.h"
#include "game.h"



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

                    //  düşman yok oldu
                    game->enemies[j].active = false;
                    game->score += 1; // skora 1 puan ekle 
                    // !!!!! ileride xp ve altın düşürme kodu gelicek
                    //  !!!!!!! ileride sound effect gelicek (patlama sesi)
                    //  mermi bir düşmanı buldu ve yok oldu diğerlerine bakmaya gerek yok döngüden çıkıyoruz
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

                //  skoru kontrol et rekor mu diye
                Game_CheckAndSaveScore(game, game->score);

                //  çarpışma yaşandığı için oyunu bitirme
                game->currentState = GAME_OVER;
                
                //  oyun bittiği için döngüden çıkabiliriz
                break;
            }
                
      }
}

    
void Collisions_CheckAll(GameData *game){
        Collisions_BulletEnemy(game);   //  mermi ve düşman çarpıştı mı kontrolü
        Collisions_PlayerEnemy(game);   // oyuncu ve düşman çarpıştı mı kontrolü

}