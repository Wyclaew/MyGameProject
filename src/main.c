#include "defines.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "collision.h"
#include "ui.h"
#include <time.h>
 



//  programın başladığı yer
int main(void){
    //  işletim sisteminden grafik belleği isteme
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bug fixes and general improvements");
    SetTargetFPS(60);   //  fps limitleme

    //  esc tuşunun pencereyi kapatma özelliğini devre dışı bıraktık
    SetExitKey(0);

    //  üretilen random sayıların örüntü oluşturmasını bozmak için
    SetRandomSeed(time(NULL));
    
    //  oyun verilerini başlat
    GameData game = {0};

    //  tüm sistemleri başlat
    Game_Init(&game);

    //  oyun döngüsü
    while(!WindowShouldClose()){
        float dt = GetFrameTime();  //  geçen süre(delta time)

        //  f11 ile tam ekrana alma
        if(IsKeyPressed(KEY_F11)) ToggleFullscreen();

        //  oyun mantığı (her state için ayrı)
        Game_Update(&game, dt);

        //  çizim (her state için ayrı)
        Game_Draw(&game);

    }

    //  temizlik (textureleri unloadlama vs.)
    Game_Cleanup(&game);
    CloseWindow();

    return 0;   
}