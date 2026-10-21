#pragma once

#include <dolphin/types.h>

typedef struct MapData {
    const char* mapName;
    s32* eventAddr;
} MapData;

typedef struct AreaData {
    const char* areaName;
    const char* areaJpName;
    void* btlSetupTbl;   // 0x8, relSetBtlAddr: the area REL's btlsetup_<area>_tbl
    void* btlSetupNoTbl; // 0xC, relSetBtlAddr: the area REL's btl_<area>_setup_no_tbl
    MapData* maps[];
} AreaData;

typedef struct WorldData {
    const char* worldName;
    const char* worldJpName;
    AreaData* areas[64];
} WorldData;

MapData* mapDataPtr(const char* mapName);
AreaData* areaDataPtr(const char* areaName);
void relSetEvtAddr(const char* mapName, s32* eventAddr);
void relSetBtlAddr(const char* areaName, void* btlSetupTbl, void* btlSetupNoTbl);
int maxMapInArea(const char* areaName);
int maxAreaInWorld(void);
WorldData* worldDataPtr(void);
