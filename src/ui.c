#include <stdio.h>
#include "ui.h"
#include "projectile.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "defines.h"
#include "game.h"
#include "gem.h"





void UI_InitLayout(GameData *game){
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    float leftColX = sw * 0.05f;    //  soldan %5 boşluk
    float btnWidth = sw * 0.25f;    //  ekran genişliğinin %25 i genişlik
    float btnHeight = sh * 0.08f;   //  ekran yüksekliğinin %8 i yükseklik
    float gap = sh * 0.02f;         //  butonlar arası boşluk
    float startY = sh * 0.45f;      //  çizmeye başlanama yüksekliği

    //  başlık pozisyonu
    game->uilayout.titlePos = (Vector2){ leftColX, startY - (sh * 0.15f) }; 

    //  butonarın hitboxları
    game->uilayout.playButton = (Rectangle){ leftColX, startY, btnWidth, btnHeight };
    game->uilayout.settingsButton = (Rectangle){ leftColX, startY + (btnHeight + gap), btnWidth, btnHeight };
    game->uilayout.exitButton = (Rectangle){ leftColX, startY + (2 * (btnHeight + gap)), btnWidth, btnHeight };

    //  küçük butonlar
    float sBtnSize = sh * 0.06f; // Yüksekliğin %6'sı
    float margin = sh * 0.02f;   // %2 kenar boşluğu

    //  github butonu
    game->uilayout.githubButton = (Rectangle){ 
        leftColX,                // sol sütun hızasında
        sh - sBtnSize - margin,  // ekranın en altından biraz yukarıda
        sBtnSize * 1.5f,    //  buton genişliği 
        sBtnSize    //  buton yüksekliği
    };

    //  mute button
    game->uilayout.muteButton = (Rectangle){ 
        leftColX + sBtnSize * 1.5f,   //  githubın bittiği yerden 10 pixel sağda
        sh - sBtnSize - margin, //  aynı hizada
        sBtnSize * 1.5f,    //  buton genişliği
        sBtnSize    //  buton yüksekliği
    };
}




void UI_DrawMenu(const GameData *game){
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    
    ClearBackground(RAYWHITE);

    //  başlık
    DrawText("Circle\nSurvivor", (int)game->uilayout.titlePos.x, (int)game->uilayout.titlePos.y, 60, MAROON);
    
    //  butonlar
    DrawResponsiveButton(game->uilayout.playButton, "PLAY", 30, RED);
    DrawResponsiveButton(game->uilayout.settingsButton, "SETTINGS", 30, GRAY);
    DrawResponsiveButton(game->uilayout.exitButton, "QUIT", 30, BLACK);
    
    //  ikonların çizimi
    //  github butonu
    DrawRectangleRec(game->uilayout.githubButton, BLUE);
    DrawResponsiveButton(game->uilayout.githubButton, "GIT", 20, LIGHTGRAY);
    UI_DrawCenteredText("GIT", game->uilayout.githubButton, 20, WHITE);

    //  mute butonu
    Color muteColor = game->settings.isMuted ? RED : GREEN;
    DrawRectangleRec(game->uilayout.muteButton, muteColor);
    DrawResponsiveButton(game->uilayout.muteButton, game->settings.isMuted ? "OFF" : "ON", 20, LIGHTGRAY);

    UI_DrawCenteredText(game->settings.isMuted ? "OFF" : "ON", game->uilayout.muteButton, 20, muteColor);

    //  orta sütun karakter preview
    Texture2D pTex = game->assets.playerTexture;
    float scale = 4.0f; 
    
    //  animasyon frame hesaplaması
    int frame = (int)(GetTime() * PLAYER_FRAME_COUNT) % PLAYER_FRAME_COUNT;
    float frameW = pTex.width / PLAYER_FRAME_COUNT;
    
    Rectangle source = { frame * frameW, 0, frameW, (float)pTex.height };
    
    //  zamanla yön değiştir
    if ((int)GetTime() % 8 > 4) source.width *= -1; 

    Rectangle dest = { 
        sw * 0.51f,     // ekranın biraz ortasında başla
        sh * 0.4f,      // yükseklik
        frameW * scale, 
        (float)pTex.height * scale 
    };
    Vector2 origin = { dest.width/2, dest.height/2 };

    //  gölge efekti
    DrawEllipse((int)dest.x, (int)(dest.y + dest.height/2 - 10), dest.width/2, 10, Fade(BLACK, 0.3f));
    
    //  karakter çiz
    DrawTexturePro(pTex, source, dest, origin, 0.0f, WHITE);
    
    const char* charName = "The Survivor";
    int textW = MeasureText(charName, 30);
    DrawText(charName, (int)(dest.x - textW/2), (int)(dest.y + dest.height/2 + 20), 30, DARKGRAY);


    //  sağ sütun skor tablosu
    Rectangle scoreBox = { 
        sw * 0.70f,   // sağ taraf
        sh * 0.25f,   // biraz aşağıdan başla
        sw * 0.25f,   //    genişlik
        sh * 0.60f    //    yükseklik
    };

    //  panel arka planı
    DrawRectangleRec(scoreBox, Fade(BLACK, 0.1f));
    DrawRectangleLinesEx(scoreBox, 2, DARKGRAY);
    
    // başlık
    DrawText("TOP HUNTERS", (int)scoreBox.x, (int)scoreBox.y - 30, 25, GOLD);
    
    //  scroll bar
    DrawRectangle((int)(scoreBox.x + scoreBox.width - 5), (int)scoreBox.y, 5, (int)scoreBox.height, Fade(GRAY, 0.3f));

    //  makas modu
    //  sadece bu kutunun içine çizim yapılsın
    BeginScissorMode((int)scoreBox.x, (int)scoreBox.y, (int)scoreBox.width, (int)scoreBox.height);
        
        float contentHeight = MAX_HIGHSCORE * 40.0f; // listenin toplam yüksekliği

        for(int i=0; i < MAX_HIGHSCORE; i++){
            //  pozisyon ayarlaması
            float yPos = scoreBox.y + (i * 40) - game->scoreScrollOffset;

            //  skor tablosu sonsuz döngüsü
            //  eğer bir yazı yukarıdan çıkarsa, aşağıdan tekrar girmeli
            //  veya aşağıdaysa ve kaydırma azsa yukarıda görünmeli
            //  eğer ypos kutunun çok üstündeyse contentheight kadar aşağı at
            if (yPos < scoreBox.y - 40) {
                yPos += contentHeight;
            }

            //  sadece görünür alandakileri çiz
            if(yPos > scoreBox.y - 40 && yPos < scoreBox.y + scoreBox.height){
                
                //  sıra no ve isim
                DrawText(TextFormat("#%d %s", i+1, game->highScores[i].player_name), 
                         (int)scoreBox.x + 15, (int)yPos + 10, 20, DARKGRAY);
                
                //  skor sağa yaslı
                const char* sText = TextFormat("%d", game->highScores[i].score);
                int sW = MeasureText(sText, 20);
                DrawText(sText, (int)(scoreBox.x + scoreBox.width - sW - 15), (int)yPos + 10, 20, MAROON);
                
                //  alt çizgi
                DrawLine((int)scoreBox.x + 10, (int)yPos + 35, (int)(scoreBox.x + scoreBox.width - 10), (int)yPos + 35, Fade(GRAY, 0.3f));
            }
        }

    EndScissorMode();   //  scissor modunu kapat
}



void UI_UpdateMenu(GameData *game){
    Vector2 mousePoint = GetMousePosition();

    //  eğer mouse pozisyonu play butonuyla çakışıyorsa ve sol mouse butonuna basıldıysa oyunu başlat
    if(CheckCollisionPointRec(mousePoint, game->uilayout.playButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        Game_Reset(game);
        game->currentState = GAMEPLAY;
    }

    //  eğer mouse pozisyonu settings butonuyla çakışıyorsa ve sol mouse butonuna basıldıysa oyun durumunu ayarlara al
    if(CheckCollisionPointRec(mousePoint, game->uilayout.settingsButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        game->currentState = SETTINGS;
    }

    //  eğer mouse pozisyonu ve exit butonu pozisyonları çakışıyorsa ve sol mouse butonuna basıldıysa oyun penceresini kapat
    if(CheckCollisionPointRec(mousePoint, game->uilayout.exitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        CloseWindow();
    }

    //  skor tablosu scroll mantığı
    float dt = GetFrameTime();
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    
    // listenin toplam piksel yüksekliği
    // eksi görünen skor kutusunun yüksekliği
    // böylece listenin en altı görününce durur
    float visibleHeight = sh * 0.62f;
    float contentHeight = MAX_HIGHSCORE * 40.0f;
    float maxScroll = contentHeight - visibleHeight + 13.0f;

    //  eğer liste kutudan küçükse kaydırmaya gerek yok
    if (maxScroll < 0) maxScroll = 0;

    //  manuel kaydırma
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        //  kullanıcı tekerleği kaydırıyorsa
        game->scoreScrollOffset -= wheel * SCROLL_SPEED;
        
        //  sınırla
        if (game->scoreScrollOffset < 0) game->scoreScrollOffset = 0;
        if (game->scoreScrollOffset > maxScroll) game->scoreScrollOffset = maxScroll;
        
        //  kullanıcı elle kaydırdıktan sonra yönü sıfırla
        game->isScrollingDown = true; 
    } 
    else {
        //  otomatik kaydırma
        if (game->isScrollingDown) {
            //  yavaşça aşağı in
            game->scoreScrollOffset += (SCROLL_SPEED - 15.0f) * dt;   //  yavaşça inmez hızı 

            //  listenin sonuna geldi mi
            if (game->scoreScrollOffset >= maxScroll) {
                game->scoreScrollOffset = maxScroll; // sınırda tut
                //  belki biraz beklenip geri sarılabilir
                game->isScrollingDown = false; //   yönü değiştir
                
            }
        } 
        else {
            //  hızlıca yukarı çık
            game->scoreScrollOffset -= 150.0f * dt;

            //  en başa gelid mi
            if (game->scoreScrollOffset <= 0) {
                game->scoreScrollOffset = 0;    //  sınırda tut
                game->isScrollingDown = true;   //  yönü değiştir
            }
        }
    }

    //  github linki
    if(CheckCollisionPointRec(mousePoint, game->uilayout.githubButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        OpenURL("https://github.com/Wyclaew");
    }

    //  mute butonu işlevi
    if(CheckCollisionPointRec(mousePoint, game->uilayout.muteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        game->settings.isMuted = !game->settings.isMuted;
        
        SetMasterVolume(game->settings.isMuted ? 0.0f : game->settings.masterVolume);
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

    //  paneli çiz
    DrawRectangleRec(panel, RAYWHITE);
    DrawRectangleLinesEx(panel, 3.0f, DARKGRAY);

    //  başlık
    int headerSize = (int)(sh * 0.05f); //  %5 font
    const char* text = "SETTINGS";
    int textW = MeasureText(text, headerSize);
    DrawText(text, panel.x + (panelW - textW) / 2, panel.y + (panelH * 0.05f), headerSize , DARKGRAY);

    //  panel içinde slider
    float sliderW = panelW* 0.8f;
    float sliderH = panelH * 0.08f;
    Rectangle sliderRect = {
        panel.x + (panelW - sliderW) / 2,   //  panelin içinde ortala
        panel.y + 160,  //  panelin tepesinde %30 aşağıda
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
    int labelSize = (int)(sh * 0.03f);
    DrawText("Master Volume", sliderRect.x, sliderRect.y - labelSize - 5, labelSize, DARKGRAY);
    DrawText(TextFormat("%d%%", (int)game->settings.masterVolume * 100), sliderRect.x + sliderRect.width + 10, sliderRect.y, labelSize, WHITE);

    //  back butonu panelin altında
    const char* backText = "Press B to back";
    int backTextSize = (int)(sh * 0.03f);
    int backTextW = MeasureText(backText, backTextSize);
    DrawText(backText, panel.x + (panelW - backTextW) / 2, panel.y + panelH - (panelH * 0.1f), backTextSize, GRAY);

}


void UI_UpdateSettings(GameData *game){
    //  kaydırıcı bar yapımı
    Vector2 mouse = GetMousePosition();
    
    //  panel ve slider konumları draw fonksiyonu ile aynı olmalı
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float panelW = sw * 0.6f;
    float panelH = sh * 0.6f;
    Rectangle panel = {(sw - panelW) / 2, (sh - panelH) / 2, panelW, panelH};
    
    float sliderW = panelW * 0.8f;
    float sliderH = panelH * 0.08f;

    //  slider konumu draw ile aynı olucak
    Rectangle sliderRect = {
        panel.x + (panelW - sliderW) / 2,
        panel.y + 160, 
        sliderW,
        sliderH
    };
    
    //  ayarlardan menüye gelme
    if(IsKeyPressed(KEY_B)) game->currentState = MENU;

    //  slider mantığı
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        if(mouse.x >= sliderRect.x && mouse.x <= sliderRect.x + sliderRect.width &&
           mouse.y >= sliderRect.y - 10 && mouse.y <= sliderRect.y + sliderRect.height + 10){
            
            float value = (mouse.x - sliderRect.x) / sliderRect.width;
            
            //  değeri 0 ile 1 arasına sıkıştır
            if (value < 0.0f) value = 0.0f;
            if (value > 1.0f) value = 1.0f;

            game->settings.masterVolume = value;
            
            //  ses ayarını uygula
            SetMasterVolume(game->settings.masterVolume);
        }
    }
}



void UI_DrawGameplay(const GameData *game){

    //  debug ekranı
    //UI_DrawDebugInfo(game);

    //  skor tablosu
    DrawText(TextFormat("SCORE : %i", game->score), SCREEN_WIDTH - MeasureText(TextFormat("SCORE : %i", game->score), 60) + 100, 40, 35, GOLD); 
    

    //  xp barı
    UI_DrawXPBar(game);
}

void UI_DrawGameOver(const GameData *game){

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    //  başlık
    const char* title = "GAME OVER";
    int titleSize = (int)(sh * 0.06f); //   ekran boyuna göre font büyüklüğü
    if (titleSize < 40) titleSize = 40; //  minimum boyut koruması
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (sw - titleW) / 2, (int)(sh * 0.2f), titleSize, RED);

    //  skor
    const char* scoreText = TextFormat("Your Score: %i", game->score);
    int scoreSize = (int)(sh * 0.04f);
    if (scoreSize < 30) scoreSize = 30;
    int scoreW = MeasureText(scoreText, scoreSize);
    DrawText(scoreText, (sw - scoreW)/2, (int)(sh * 0.3f), scoreSize, BLACK);

    //  yeni rekor mu
    if (game->isNewHighScore){
        //  yeni rekor yazısı
        const char* newRecText = "NEW RECORD! What's your name, warrior?";
        int recSize = 20;
        int recW = MeasureText(newRecText, recSize);
        DrawText(newRecText, (sw - recW) / 2, (int)(sh * 0.45f), recSize, GOLD);

        //  isim giriş kutusu
        int boxWidth = sw * 0.3f; //  kutunun genişliği
        int boxHeight = 40;
        int boxX = (sw - boxWidth) / 2; //  Kutuyu ortala
        int boxY = (int)(sh * 0.52f);   //  Yüksekliği ayarla

        //  kutuyu çiz
        DrawRectangle(boxX, boxY, boxWidth, boxHeight, LIGHTGRAY);
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, DARKGRAY);  // kutu çerçevesi

        // girilen ismi çiz
        DrawText(game->inputName, boxX + 10, boxY + 10, 20, BLACK);

        // kaydetme talimati
        const char* saveText = "Press ENTER To Save";
        int saveSize = 30;
        int saveW = MeasureText(saveText, saveSize);
        DrawText(saveText, (sw - saveW) / 2, (int)(sh * 0.65f), saveSize, GRAY);
    }

    //  eğer yüksek skor değilse
    else {
        //  tekrar Oyna
        const char* playText = "Press ENTER to Play Again";
        int playSize = 20;
        int playW = MeasureText(playText, playSize);
        DrawText(playText, (sw - playW) / 2, (int)(sh * 0.5f), playSize, DARKGRAY);

        //  menüye Dön
        const char* menuText = "Press M For Main Menu";
        int menuW = MeasureText(menuText, 20);
        DrawText(menuText, (sw - menuW) / 2, (int)(sh * 0.58f), 20, DARKGRAY);
    }
}



void UI_DrawDebugInfo(const GameData *game){
            DrawText("Hareket için W, A, S, D", 10, 120, 20, LIGHTGRAY);

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
            
            //  debug yazısı
            sprintf(debugText, "pos : %.0f, %.0f | FPS : %d | Bullet : %d | Enemy : %d | speed : %.0f | health : %.0f | cooldown : %.0f ", 
                game->player.pos.x, 
                game->player.pos.y, 
                fps, 
                activeBullets, 
                activeEnemies, 
                game->player.speed, 
                game->player.health,
                game->shootCooldown
                );
            DrawText(debugText, 10, 90, 20, GREEN);
}



void UI_DrawHighScores(const GameData *game){
                 DrawText("TOP 10", 350, 50, 30, GOLD);
             for (int i = 0; i < MAX_HIGHSCORE; i++){
                 DrawText(TextFormat("%i. %s  %i", i+1, game->highScores[i].player_name, game->highScores[i].score), 300, 100 + (i*40), 20, DARKBROWN);
            }
            DrawText("B For Back", 350, 550, 20, GRAY);
    
}

//  mouse konumuna duyarlı buton yapmak için yardımcı fonksiyon
void DrawResponsiveButton(Rectangle rect, const char* text, int fontSize, Color baseColor){
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);

    // hover ise rengi aç ve biraz büyütmüş gibi göster (border ekle)
    if (isHover) {
        DrawRectangleRec(rect, ColorBrightness(baseColor, 0.2f)); //    %20 daha parlak
        DrawRectangleLinesEx(rect, 3, GOLD); // altın çerçeve
    } else {
        DrawRectangleRec(rect, baseColor);
    }

    // yazıyı Ortala
    UI_DrawCenteredText(text, rect, fontSize, WHITE);
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
    int sh = GetScreenHeight();
    int barHeight = sh * 0.03f;
    Color barColor = GREEN;

    // barın arkası boş bar
    DrawRectangle(0, 0, sw, (int)barHeight, Fade(DARKGRAY, 0.5f));

    //  dolu bar 
    float ratio = (float)game->currentXP / (float)game->requiredXP;

    if(game->currentState == LEVEL_UP) {
        ratio = 1.0f;
    }

    //  barın taşmasını önle
    if(ratio > 1.0f) ratio = 1.0f;

    //  bar efektleri
    //  barın dolması
    //  eğer bar dolduysa veya level up ekranındaysak rainbow efekti ver
    if(ratio >= 1.0f || game->currentState == LEVEL_UP){
        //  zamanı kullanrak sürekli değişen bir hue değeri üret
        //  rainbow efekt hızı gettime * 800 hızı belirler
        //  360 değeri ile mod alarak renk çemberinde dönmeyi sağla
        float hue = (float)((int)(GetTime() * 400) % 360);
        barColor = ColorFromHSV(hue, 0.8f, 1.0f);   //  parlak canlı renkler
    }
    //  barın dolu kısmı
    DrawRectangle(0, 0, (sw * ratio), barHeight, barColor);

    //  barın çerçevisi
    DrawRectangleLines(0, 0, sw, barHeight, BLACK);

    //  level yazısı
    const char* levelText = TextFormat("LEVEL : %d", game->level);
    int fontSize = (int)(sh * 0.03f);
    
    //  yazının ekrandaki genişkiği
    int textWidth = MeasureText(levelText, fontSize);
    int posX = (int)(sw * 0.02f);
    int posY = (int)(barHeight + 5);

    // çiz
    DrawText(levelText, posX - 20, posY, fontSize, GOLD);
}


void UI_UpdateLevelUp(GameData *game){
    //  oyunu arkada durdurmamız lazım unutma mainde hallet
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();

    //  kartların dinamik boyut hesabı
    float cardW = sw * 0.22f;   //  kartların genişliği
    float cardH = sh * 0.50f;   //  kartların yüksekliği
    float gap = sw * 0.03f; //  kartlar arası boşluk
    
    //  başlngıç x noktası
    float totalWidth = (3 * cardW) + (2 * gap);
    float startX = (sw - totalWidth) / 2.0f;    //  başlangıç x pozisyonu
    float startY = (sh - cardH) / 2.0f; //  başlngıç y pozisyonu

    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < 3; i++){
        //  her kartın o ankki pozisyonu
        Rectangle cardRect = {
            startX + i *(cardW + gap),
            startY,
            cardW,
            cardH
        };

        //  tıklama kontrolü
        if(CheckCollisionPointRec(mousePos, cardRect)){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            //  upgrade kartı seçme efekti eklenebilir
            Game_ApplyUpgrade(game, i);

            //  upgrade i uyguladıktan sonra düşmanları güvenli bir yere koy
            Enemy_PushAwayFromPlayer(game, 150.0f);

            //  oyunu tekrar başlatıyoruz
            game->currentState = GAMEPLAY;
            }
        }
    }
}



void UI_DrawLevelUp(const GameData *game){
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();

    //  arka planı karart
    DrawRectangle(0, 0, (int)sw, (int)sh, Fade(WHITE, 0.0f));

    //  dinamik yazılar
    int titleSize = (int)(sh * 0.08f);    //  ekran yüksekliğinin %8 i kadar
    int subTitleSize = (int)(sh * 0.03f);   //  ekran yüksekliğinin %3 ü kadar

    //  başlıkları çiz
    UI_DrawCenteredText("LEVEL UP!", (Rectangle){0, sh * 0.1f, sw, (float)titleSize}, titleSize, GOLD);

    //  alt başlık biraz daha aşağısında
    UI_DrawCenteredText("Choose your destiny", (Rectangle){0, sh * 0.18f, sw, (float)subTitleSize},subTitleSize, LIGHTGRAY);

    //  kart boyutları update ile aynı olmalı
    float cardW = sw * 0.22f;
    float cardH = sh * 0.50f;
    float gap = sw * 0.03f;

    float totalWidth = (3 * cardW) + (2 * gap);
    float startX = (sw - totalWidth) / 2.0f;
    float startY = (sh - cardH) / 2.0f;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < 3; i++){
        float x = startX  + i *(cardW + gap);
        float y = startY;
        float w = cardW;
        float h = cardH;

        //  hover animasyonu
        // mouse üstündeyse kartı %5 büyüt
        bool isHover = CheckCollisionPointRec(mouse, (Rectangle){x, y, w, h});

        if(isHover){
            float scaleFactor = 1.05f;
            float newW = w * scaleFactor;
            float newH = h * scaleFactor;

            //  merkezden büyütme matematiği
            x -= (newW - w) / 2;
            y -= (newH - h) / 2;
            w = newW;
            h = newH;
        }

        Rectangle cardRect = {x, y, w, h};
        //  hover efektinde kartın arka plan renk değişimi
        Color cardBg = isHover ? RAYWHITE : LIGHTGRAY;
        Color borderColor = isHover ? GOLD : DARKGRAY;

        //  kart çizimi
        DrawRectangleRounded(cardRect, 0.1f, 10, cardBg);   //  rounded kenarlarını yuvarlaklı yapar
        DrawRectangleRoundedLines(cardRect, 0.1f, 10, borderColor);  //  kartın sınırlarının çizimi

        //  kart içeriği
        float iconRadius = w * 0.15f;   //  kart genişliğinin %15i kadar yarıçap
        Color iconColor = (game->activeUpgrades[i].type == UPGRADE_ATTACK_SPEED) ? RED : SKYBLUE;
        //  damage türü olursa rengi burda olucak
        DrawCircle(x + w/2, y + h * 0.2f, iconRadius, iconColor);

        // kart genişliğine göre font boyutu
        int nameSize = (int)(w * 0.1f); //  kartın %10 u kadar font

        //  ismi ortala ve yazdır
        int nameWidth = MeasureText(game->activeUpgrades[i].title, nameSize);
        DrawText(game->activeUpgrades[i].title, x + (w - nameWidth) / 2, y + h * 0.4f, nameSize, BLACK);

        //  açıklamayı yazdır
        int decSize = (int)(w * 0.08f); //  daha küçük font

        //  metni sığdırmak için rectangle sınırlarını belirliyoruz
        Rectangle textBound = {
            x + (w * 0.1f), //  soldan %10 boşluk
            y + (h * 0.55f),    //yarıdn biraz aşağıda başla
            w * 0.8f,   //  genişliğin %80 ini kullan
            h * 0.4f    //  alt kısıma kadar kullan
        };
        
        //  font spacing (harf aralığı)
        DrawTextEx(GetFontDefault(),
        game->activeUpgrades[i].description,
        (Vector2){textBound.x, textBound.y},
        (float)decSize,
        1.0f,
        DARKGRAY);
    }
}