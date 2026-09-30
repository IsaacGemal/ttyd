#pragma once

#include "evt/evt_cmd.h"

typedef struct MapAttrData {
    const char* name; // 0x0
    s32 field_0x4;    // 0x4
    s32 field_0x8;    // 0x8
} MapAttrData;

void setupDataBase(const char* areaName, const char* mapName);
BOOL setupDataCheck(void);
void setupDataLoad(const char* mapName);
