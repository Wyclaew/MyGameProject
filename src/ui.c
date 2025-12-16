#include <stdio.h>
#include "ui.h"
#include "projectile.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "defines.h"
#include "game.h"











void UI_InitLayout(GameData *game){   //  butonları yerleştirme
    //  ekranın merkezini bul (sonradan boyut değişince otomatik ayarlanması için)
    float centerX = (float)SCREEN_WIDTH / 2;
    float startY = 250.0f;  //  ilk butonun y konumu
    float buttonWidth = 220.0f; //  buton genişlikleri
    float buttonHeight = 50.0f;  //  buton yükseklikleri
    float gap = 20.0f;   //  butonlar arası boşluk

    //  play butonu
    game->uilayout.playButton = (Rectangle){
        centerX - (buttonWidth / 2),    //  butonun tam ortalanması için buton genişliğinin yarısını ekran genişliğinin yarısından çıkarıyoruz
        startY,
        buttonWidth,
        buttonHeight
    };

    //  settings butonu
    game->uilayout.settingsButton = (Rectangle){
        centerX - (buttonWidth / 2),
        startY + buttonHeight + gap,
        buttonWidth,
        buttonHeight
    };


    //  skor butonu
    game->uilayout.scoresButton = (Rectangle){
        centerX - (buttonWidth / 2),
        startY + (buttonHeight + gap) * 2,
        buttonWidth,
        buttonHeight
    };

    //  exit butonu
    game->uilayout.exitButton = (Rectangle){
        centerX - (buttonWidth / 2),
        startY + (buttonHeight + gap) * 3,
        buttonWidth,
        buttonHeight
    };

    //  github butonu
    game->uilayout.githubButton = (Rectangle){
        20, SCREEN_HEIGHT -40, 100, 30
    };
    
    //  mute butonu
    game->uilayout.muteButton = (Rectangle) {
        SCREEN_WIDTH - 130,
        SCREEN_HEIGHT -40,
        100,
        30
    };

    //  oyun ismi
    game->uilayout.titlePos = (Vector2){
        centerX,
        100
    };

}




void UI_DrawMenu(const GameData *game){

    //  ana başlık
    const char* title = "Circle Survivor";
    int titleSize = 60;
    int titleWidth = MeasureText(title, titleSize);
    int titleHeight = 80;
    Color titleColor = GOLD;
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, titleHeight, titleSize, titleColor);

    //  alt başlık
        DrawText("beta v1.0", SCREEN_WIDTH - 100, SCREEN_HEIGHT - 30, 15, LIGHTGRAY);

    //  butonları çizme basit dikdörtgenler
    //  play butonu
    DrawRectangleRec(game->uilayout.playButton, GetColor(0xF5F5DCFF) );
    UI_DrawCenteredText("PLAY", game->uilayout.playButton, 20, BLACK);

    //  settings butonu
    DrawRectangleRec(game->uilayout.settingsButton, GetColor(0xF5F5DCFF)); 
    UI_DrawCenteredText("SETTINGS", game->uilayout.settingsButton, 20, BLACK);

    //  skor butonu
    DrawRectangleRec(game->uilayout.scoresButton, GetColor(0xF5F5DCFF)); 
    UI_DrawCenteredText("HIGH SCORES", game->uilayout.scoresButton, 20, BLACK);

    //  exit butonu
    DrawRectangleRec(game->uilayout.exitButton, GetColor(0xF5F5DCFF)); 
    UI_DrawCenteredText("EXIT GAME", game->uilayout.exitButton, 20, BLACK);

    //  github butonu
    DrawRectangleRec(game->uilayout.githubButton, BLUE);
    UI_DrawCenteredText("GitHub", game->uilayout.githubButton, 20, WHITE);

    //  mute butonu
    DrawRectangleRec(game->uilayout.muteButton, LIGHTGRAY);
    UI_DrawCenteredText("MUTE", game->uilayout.muteButton, 10, DARKGRAY);
}



void UI_UpdateMenu(GameData *game){

        Vector2 mousePoint = GetMousePosition();

    //  play butonu basış kontrolü
    if(CheckCollisionPointRec(mousePoint, game->uilayout.playButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        //  oyunu sıfırla ve başlat
        Game_Reset(game);
        game->currentState = GAMEPLAY;
    }

    //  settings butonu basış kontrolü
    if(CheckCollisionPointRec(mousePoint, game->uilayout.settingsButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        game->currentState = SETTINGS;
    }

    //  high score butonu basış kontrolü
    if(CheckCollisionPointRec(mousePoint, game->uilayout.scoresButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        game->currentState = HIGHSCORES;
    }

    //  githuba butonu basış kontrolü
    if(CheckCollisionPointRec(mousePoint, game->uilayout.githubButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        OpenURL("https://github.com/Wyclaew/MyGameProject");
    }

    //  mute butonu basış kontolü
    if(CheckCollisionPointRec(mousePoint, game->uilayout.muteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        game->settings.isMuted = !game->settings.isMuted;
    }

    //  exit butonu basış kontolü
    if(CheckCollisionPointRec(mousePoint, game->uilayout.exitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        CloseWindow();
    }




            //  enter tuşuna bastığımızda oyun durumu menüden gameplaye geçicek 
            //  IsKeyPressed: tuşa basıldığı anda true döner
                if (IsKeyPressed(KEY_ENTER)){
                Game_Reset(game);

                game->currentState = GAMEPLAY;
            }
}



void UI_DrawSettings(const GameData *game){
    DrawText("SETTINGS", 350, 100, 30, DARKGRAY);

    //  slider çizimi
    DrawRectangle(300, 200, 200, 20, LIGHTGRAY);    //  arka çubuk

    //  doluluk oranı
    DrawRectangle(300, 200, (int)(game->settings.masterVolume * 200), 20, GREEN);
    DrawText("Master Volume", 300, 180, 20, GRAY);

    DrawText("Press B to go back", 300, 500, 20, DARKGRAY);
}


void UI_UpdateSettings(GameData *game){
    //  kaydırıcı bar yapımı
    Vector2 mouse = GetMousePosition();
    Rectangle sliderMaster = {300, 200, 200, 20,};
    
    //  ayarlardan menüye gelme
    if(IsKeyPressed(KEY_B)) game->currentState = MENU;

    //  slider mantığı
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        if(CheckCollisionPointRec(mouse, sliderMaster)){
            //  farenin konumuna göre 0.0 ile 1.0 arasında bir değer hesaplar
            float value = (mouse.x - sliderMaster.x) / sliderMaster.width;
            game->settings.masterVolume = value;
            SetMasterVolume(game->settings.masterVolume); //  raylibin ses ayarı
        }
    }
}



void UI_DrawGameplay(const GameData *game){

    UI_DrawDebugInfo(game);

    Player_Draw(&game->player);

    Projectile_DrawBullet(game);

            //  skor tablosu
            DrawText(TextFormat("SCORE : %i", game->score), 10, 10, 20, GOLD); 

    Enemy_Draw(game);




}

void UI_DrawGameOver(const GameData *game){

    DrawText("Game Over", 300, 100, 40, RED);
    DrawText(TextFormat("Your Score : %i", game->score), 320, 160, 30, BLACK);

    if (game->isNewHighScore){
        DrawText("NEW RECORD! What's your name, warrior?", 250, 250, 20, GOLD);
        //  input kutusu
        DrawRectangle(300, 280, 200, 40, LIGHTGRAY);
        DrawText(game->inputName, 310, 290, 20, BLACK);
        DrawText("Press ENTER To Save", 300, 350, 15, GRAY);
    }
    else {
        DrawText("Press [ENTER] to Play Again", 280, 300, 20, DARKGRAY);
        DrawText("Press [M] For Main Menu", 320, 340, 20, DARKGRAY);
    }

}



void UI_DrawDebugInfo(const GameData *game){
                DrawText("Hareket için W, A, S, D", 10, 60, 20, LIGHTGRAY);

            //  debug ekranı ekleme

            int activeBullets = 0;
            for (int i = 0; i < MAX_BULLETS; i++){
                if(game->bullets[i].active) activeBullets++;

            }

            int activeEnemies = 0;
            for (int i = 0; i < MAX_ENEMIES; i++){
                if(game->enemies[i].active) activeEnemies++;
            }
            
            int fps = GetFPS();

            char debugText[100];
            
            // sprintf(debugText, "Fps : %d  |  Mermiler : %d  |  Dusmanlar : %d", fps, activeBullets, activeEnemies);
            sprintf(debugText, "pos : %.0f, %.0f | FPS : %d | Bullet : %d | Enemy : %d", game->player.pos.x, game->player.pos.y, fps, activeBullets, activeEnemies);
            DrawText(debugText, 10, 40, 20, GREEN);
}



void UI_DrawHighScores(const GameData *game){
                DrawText("TOP 10", 350, 50, 30, GOLD);
            for (int i = 0; i < MAX_HIGHSCORE; i++){
                DrawText(TextFormat("%i. %s ..... %i", i+1, game->highScores[i].player_name, game->highScores[i].score), 300, 100 + (i*40), 20, DARKBROWN);
            }
            DrawText("B For Back", 350, 550, 20, GRAY);
    
}

//  istediğimiz kutuya istediğimiz yazıyı ortalı şekilde koyma
void UI_DrawCenteredText(const char* text, Rectangle rect, int fontSize, Color color){
    int textWidth = MeasureText(text, fontSize);
    int textX = rect.x + (rect.width - textWidth) / 2;
    int textY = rect.y + (rect.height - fontSize) / 2;
    DrawText(text, textX, textY, fontSize, color);
}