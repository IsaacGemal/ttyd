#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
// Only the fields effPturnDisp reads are named so far; the real block extends to at least 0x88.
typedef struct effPturnN64Data {
    s32 field_0x0; // 0x00
    s32 field_0x4; // 0x04
    f32 x;         // 0x08
    f32 y;         // 0x0C
    f32 z;         // 0x10
    u8 pad_14[0x14]; // 0x14
    s32 life;      // 0x28
    s32 field_0x2C; // 0x2C
    u8 pad_30[4];  // 0x30
    f32 field_0x34; // 0x34
    s32 field_0x38; // 0x38
    s32 field_0x3C; // 0x3C
    u8 pad_40[4];  // 0x40
    s32 field_0x44; // 0x44
    u8 pad_48[8];  // 0x48
    f32 field_0x50; // 0x50
    f32 field_0x54; // 0x54
    f32 field_0x58; // 0x58
    f32 field_0x5C; // 0x5C
    f32 field_0x60; // 0x60
    f32 field_0x64; // 0x64
    f32 field_0x68; // 0x68
    s32 field_0x6C; // 0x6C
    s32 field_0x70; // 0x70
    u8 pad_74[0x14]; // 0x74
    f32 field_0x88; // 0x88
    u8 pad_8C[4];  // 0x8C
} effPturnN64Data;
void effPturnDisp(CameraId camId, void* param);
void effPturnMain(EffectEntry* effect);
void pturn_main_dispp(CameraId camId, void* param);
void pturn_num_dispp(CameraId camId, void* param);
EffectEntry* effPturnN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z);
