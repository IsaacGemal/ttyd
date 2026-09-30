#ifndef _UNIT_BARRIERN_Z_H_
#define _UNIT_BARRIERN_Z_H_

#include <dolphin/types.h>

typedef struct BarriernZanzouScale {
    f32 value[5];
} BarriernZanzouScale;

typedef struct BarriernSatelliteTo {
    s32 type;         // 0x00
    s32 unitId;       // 0x04
    s32 points[5];    // 0x08
    s32 field_1C[15]; // 0x1C
    s32 field_58;     // 0x58
} BarriernSatelliteTo;

typedef struct BarriernSatelliteToData {
    s32 field_0; // 0x00
    f32 x;       // 0x04
    f32 y;       // 0x08
    f32 z;       // 0x0C
    f32 scale;   // 0x10
} BarriernSatelliteToData;

#endif