#ifndef UI_H
#define UI_H


#include "defines.h"
#include "game.h"





//  menü fonksiyonları
void UI_InitLayout(GameData *game);
void UI_DrawMenu(const GameData *game);
void UI_UpdateMenu(GameData *game); //  menü içindeki hesaplamalar

//  diğer ekranlar
void UI_DrawSettings(const GameData *game);
void UI_UpdateSettings(GameData *game);
void UI_DrawGameplay(const GameData *game);
void UI_DrawGameOver(const GameData *game);
void UI_UpdateGameOver(GameData *game, char *inputName, int *letterCount);
void UI_DrawHighScores(const GameData *game);
void UI_DrawDebugInfo(const GameData *game);
void UI_DrawCenteredText(const char* text, Rectangle rect, int fontSize, Color color);
void UI_DrawXPBar(const GameData *game);
void UI_UpdateLevelUp(GameData *game);
void UI_DrawLevelUp(const GameData *game);




#endif