#ifndef GAME_H
#define GAME_H


#include "defines.h"



//  kullanacağımız fonksiyonları önceden tanıtma (prototipler)

//  oyun durumu yönetimi
void Game_Init(GameData *game); //  oyunu ve tüm alt sistemleri başlatır
void Game_Update(GameData *game, float dt);  //  oyun döngüsü fizikler ve mantıklar
void Game_Draw(const GameData *game);    //  oyunun çizim döngüsü
void Game_Reset(GameData *game);    //  oyun bitince skoru ve zorluğu sıfırlama vs.
void Game_Cleanup(GameData *game);  //  kapanış ve belleği temizleme


//  spesifik durum update fonksiyonları
void Game_UpdateGameOver(GameData *game);  //  ölüm ekranı içi hesaplama işleri
void Game_WaveSpawner(GameData *game, float dt); //  düşmanlar için wave kontrolü
void Game_ShootingSystem(GameData *game, float dt);    //  en yakın düşmana ateş etme vs. mekanikleri
void Game_GeneratesUpgrade(GameData *game); //  rastgele upgrade seç
void Game_ApplyUpgrade(GameData *game, int index);  //  seçilen upgrade i onayla
void Game_ApplyAttackSpeed(GameData *game, float percentage);   //  saldırı hızını oyuncuya işle
void Game_ApplyMoveSpeed(Player *player, float percentage); //  hareket hızını oyuncuya işle
void Game_GenerateUpgrades(GameData *game); //  3 tane farklı upgrade seç
void Game_DrawWorld(const GameData *game);  //  objeleri çiz
void Game_CheckLevelUp(GameData *game); //  level atlama kontrolcsü
void Game_DrawFloor(const GameData *game);   //  zemin çizimi

//  yardımcı fonskiyonlar
Vector2 Game_GetSafeSpawnPosition(Vector2 playerPos, float minDistance); //  düşmanların oyuncunun içinde doğmaması için safespawn noktası alma




//  skor yönetimi
void Game_LoadHighScores(GameData *game);  //  high score dosyasını okuma
void Game_SaveHighScores(const GameData *game);  //  high score dosyasına yazma
void Game_CheckAndSaveScore(GameData *game, int newScore);   //  yapılan skor ilk 10 skor arasına girdi mi kontrol et


#endif