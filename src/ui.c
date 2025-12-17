#include <stdio.h>
#include "ui.h"
#include "projectile.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "defines.h"
#include "game.h"











void UI_InitLayout(GameData *game){   //  butonları yerleştirme

    Vector2 mousePoint = GetMousePosition();

    
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    //  butonları merkeze dizme
    float btnWidth = sw * 0.29f; //  buton genişlikleri ölçeklendirmesi
    float btnHeight = sh * 0.09f;   //  buton yüksekliği ölçeklendirmesi
    float startY = sh * 0.4f;   //  butonların başlayacağı yükseklik
    float gap = sh * 0.02f; //  butonlar arası boşluk
    float margin = 20.0f;    //  köşelerden bırakılacak boşluk
    float sBtnSize = 50.0f;  //  küçük ikonların buton genişliği

    game->uilayout.playButton = (Rectangle){(float)(sw - btnWidth) / 2, startY, btnWidth, btnHeight};
    game->uilayout.settingsButton =(Rectangle){(float)(sw - btnWidth) / 2, startY + (btnHeight + gap), btnWidth, btnHeight};
    game->uilayout.scoresButton =(Rectangle){(float)(sw - btnWidth) / 2, startY + (2 * (btnHeight + gap)), btnWidth, btnHeight};
    game->uilayout.exitButton =(Rectangle){(float)(sw -btnWidth) / 2, startY + (3 *(btnHeight + gap)), btnWidth, btnHeight};
    game->uilayout.githubButton =(Rectangle){margin, (float)sh - sBtnSize - margin, sBtnSize, sBtnSize};
    game->uilayout.muteButton =(Rectangle){margin, margin, sBtnSize, sBtnSize};

}




void UI_DrawMenu(const GameData *game){

    int sw = SCREEN_WIDTH;
    int sh = SCREEN_HEIGHT;
    Vector2 mousePoint = GetMousePosition();

    //  ana başlık
    const char* title = "Circle Survivor";
    int titleSize = (int)(sh * 0.1f);   //  oyun ismi yüksekliğin %10 u kadar
    int titleWidth = MeasureText(title, titleSize);
    Color titleColor = GOLD;
    DrawText(title, (sw - titleWidth) / 2, sh * 0.15f, titleSize, titleColor);

    //  alt başlık
    DrawText("beta v2.0", sw - 100, sh - 30, 15, LIGHTGRAY);

    float btnWidth = sw * 0.1f; //  buton genişlikleri ölçeklendirmesi
    float btnHeight = sh * 0.1f;   //  buton yüksekliği ölçeklendirmesi
    float startY = sh * 0.4f;   //  butonların başlayacağı yükseklik
    float gap = sh * 0.02f; //  butonlar arası boşluk
    float margin = 20.0f;    //  köşelerden bırakılacak boşluk
    float sBtnSize = 50.0f;  //  küçük ikonların buton genişliği

    Color colorPlay = CheckCollisionPointRec(mousePoint, game->uilayout.playButton) ? BEIGE : LIGHTGRAY;
    Color colorSetting = CheckCollisionPointRec(mousePoint, game->uilayout.settingsButton) ?  BEIGE : LIGHTGRAY;
    Color colorScore = CheckCollisionPointRec(mousePoint, game->uilayout.scoresButton) ? BEIGE : LIGHTGRAY;
    Color colorExit = CheckCollisionPointRec(mousePoint, game->uilayout.exitButton) ? BEIGE : LIGHTGRAY;
    Color colorGithub = CheckCollisionPointRec(mousePoint, game->uilayout.githubButton) ? BEIGE : LIGHTGRAY;
    Color colorMute = CheckCollisionPointRec(mousePoint, game->uilayout.muteButton) ? BEIGE : LIGHTGRAY;


    //  butonları çizme basit dikdörtgenler
    //  play butonu
    DrawRectangleRec(game->uilayout.playButton, colorPlay);
    UI_DrawCenteredText("PLAY", game->uilayout.playButton, (int)(btnHeight * 0.5f), BLACK);

    //  settings butonu
    DrawRectangleRec(game->uilayout.settingsButton, colorSetting);
    UI_DrawCenteredText("SETTINGS", game->uilayout.settingsButton, (int)(btnHeight * 0.5f), BLACK);

    //  skor tablosu butonu
    DrawRectangleRec(game->uilayout.scoresButton, colorScore);
    UI_DrawCenteredText("HIGH SCORES", game->uilayout.scoresButton, (int)(btnHeight * 0.5f), BLACK);

    //  oyundan çıkma butonu
    DrawRectangleRec(game->uilayout.exitButton, colorExit);
    UI_DrawCenteredText("EXIT GAME", game->uilayout.exitButton, (int)(btnHeight * 0.5f), BLACK);

    //  github butonu
    DrawRectangleRec(game->uilayout.githubButton, colorGithub);
    UI_DrawCenteredText("GitHub", game->uilayout.githubButton, (int)(btnHeight * 0.5f), WHITE);

    //  mute butonu
    DrawRectangleRec(game->uilayout.muteButton, colorMute);
    UI_DrawCenteredText("MUTE", game->uilayout.muteButton, (int)(btnHeight * 0.2f), DARKGRAY);
}



void UI_UpdateMenu(GameData *game){

    Vector2 mousePoint = GetMousePosition();

    
    int sw = SCREEN_WIDTH;
    int sh = SCREEN_HEIGHT;


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
        OpenURL("https://github.com/Wyclaew");
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

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    //  yarı saydam arka plan
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));


    //  ana panel (ortadaki kutu)
    float panelW = sw * 0.6f;
    float panelH = sh * 0.6f;
    Rectangle panel = {(sw - panelW) / 2, (sh - panelH) / 2, panelW, panelH};

    const char* text = "SETTINGS";
    int fontSize = 40;
    int textW = MeasureText(text, fontSize);
    DrawText(text, panel.x + (panelW - textW) / 2, panel.y + 20, fontSize, WHITE);

    //  panel içinde slider
    float sliderW = panelW* 0.8f;
    float sliderH = 30;
    Rectangle sliderRect = {
        panel.x + (panelW - sliderW) / 2,
        panel.y + 160,
        sliderW,
        sliderH
    };

    //  slider arka planı
    DrawRectangleRec(sliderRect, LIGHTGRAY);

    //  slider doluluğu
    DrawRectangle(sliderRect.x, sliderRect.y, sliderRect.width * game->settings.masterVolume, sliderRect.height, GREEN);

    //  slider çerçevesi
    DrawRectangleLinesEx(sliderRect, 2.0f, GRAY);

    //  etiket
    DrawText("Master Volume", sliderRect.x, sliderRect.y - 25, 20, GRAY);
    DrawText(TextFormat("%d%%", (int)game->settings.masterVolume * 100), sliderRect.x + sliderRect.width + 10, sliderRect.y + 5, 20, BLACK);

    //  back butonu panelin altında
    DrawText("Press B to back", panel.x + (panelW - MeasureText("Press B to back", 20)) / 2, panel.y + panelH - 40, 20, GRAY);

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
        DrawText("NEW RECORD! What's your name, warrior?", 200, 250, 20, GOLD);
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


void UI_DrawXPBar(const GameData *game){
    int sw = GetScreenWidth();
    int barHeight = 20;

    // barın arkası boş bar
    DrawRectangle(0, 0, sw, barHeight, LIGHTGRAY);

    //  dolu bar 
    float ratio = (float)game->currentXP / (float)game->requiredXP;

    //  barın taşmasını önle
    if(ratio > 1.0f) ratio = 1.0f;

    //  barın dolması
    DrawRectangle(0, 0, (sw * ratio), barHeight, DARKGREEN);

    //  level yazısı
    const char* levelText = TextFormat("LEVEL : %d", game->level);
    int fontSize = 20;
    int margin = 20;    //  köşelerden ne kadar içerde duracağı

    //  yazının ekrandaki genişkiği
    int textWidth = MeasureText(levelText, fontSize);
    int posX = sw - textWidth - margin;
    int posY = margin;

    // çiz
    DrawText(levelText, posX, posY, fontSize, GOLD);
}