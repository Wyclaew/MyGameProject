#include "upgrade_pool.h"
#include "defines.h"
#include <stdio.h>


//  level atlayınca önümüze rastgele gelen güçlendirmeler
const UpgradeDef upgradePool[] = {
    {"Quick Shot", "Attack speed increases\nby %5", UPGRADE_ATTACK_SPEED, 0.05f},
    {"Sun Of The Wind", "Movement speed increases\nby %5", UPGRADE_MOVEMENT_SPEED, 0.05f},
    {"Machine Gun", "Attack speed increases\nby %10", UPGRADE_ATTACK_SPEED, 0.1f},
    {"Athletic", "Movement speed increases\nby %10", UPGRADE_MOVEMENT_SPEED, 0.1f},
    {"Heavy Hitter", "Damage increases\nby %10", UPGRADE_DAMAGE, 0.1f}
    //  daha fazla çeşitli şeyler eklenebilir
};



// listenin boyutunu otomatik hesaplayıp pool size ı otomatik değiştirme
const int UPGRADE_POOL_SIZE = sizeof(upgradePool) / sizeof(upgradePool[0]);





