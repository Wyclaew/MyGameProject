#include "defines.h"
#include "spatial_grid.h"
#include "enemy.h"


//  grid veri yapısı
int grid[GRID_BUCKETS][MAX_PER_CELL];
int gridCounts[GRID_BUCKETS];    //  her hücrede kaç düşman var 


//  ızgarayı temizle
void Spatial_Clear() {
    for (int i = 0; i < GRID_BUCKETS; i++){
        gridCounts[i] = 0;
    }
}




//  dünya koordinatını grid indesine çevirir
int Spatial_GetGridHash(int x, int y) {
    //  negatif koordinatları pozitife çevirmek için büyük asal sayılarla çarpıyoruz
    //  bu sonsuz haritada çakışmaları minimize eder
    unsigned int h1 = (unsigned int)(x * 73856093);
    unsigned int h2 = (unsigned int)(y * 19349663);
    return (h1 ^ h2) % GRID_BUCKETS;

}


//  düşmanı ızgaraya ekle
void Spatial_Add(const Enemy *e, int enemyIndex) {
    int cellX = (int)(e->pos.x / CELL_SIZE);
    int cellY = (int)(e->pos.y / CELL_SIZE);

    int hash = Spatial_GetGridHash(cellX, cellY);

    //  eğer hücrede yer varsa ekle
    if (gridCounts[hash] < MAX_PER_CELL) {
        grid[hash][gridCounts[hash]] = enemyIndex;
        gridCounts[hash]++;
    }
}