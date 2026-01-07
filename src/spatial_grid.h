#ifndef SPATIAL_GRID_H
#define SPATIAL_GRID_H

#include "defines.h"


void Spatial_Clear();
void Spatial_Add(const Enemy *e, int enemyIndex);
int Spatial_GetGridHash(int x, int y);

extern int grid[GRID_BUCKETS][MAX_PER_CELL];
extern int gridCounts[GRID_BUCKETS];


#endif