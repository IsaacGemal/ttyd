#pragma once

#include <dolphin/mtx.h>

typedef struct BattleCameraWork {
    s32 flags;              // 0x00
    s32 unk4;               // 0x04
    s32 priLimit;           // 0x08
    s32 mode;               // 0x0C
    s32 unk10;              // 0x10
    s32 unk14;              // 0x14
    s32 unk18;              // 0x18
    s32 unk1C;              // 0x1C
    s32 unk20;              // 0x20
    f32 zoom;               // 0x24
    s16 moveSpeedLv;        // 0x28
    s16 zoomSpeedLv;        // 0x2A
    u8 unk2C[0x5C - 0x2C];  // 0x2C
    Vec position;           // 0x5C
    Vec unk68;              // 0x68 (speed / delta)
    u8 unk74[0x80 - 0x74];  // 0x74
    s32 unk80;              // 0x80
    s32 unk84;              // 0x84
    s32 unk88;              // 0x88
    Vec unk8C;              // 0x8C
    Vec unk98;              // 0x98
    Vec unkA4;              // 0xA4
    Vec unkB0;              // 0xB0
    Vec unkBC;              // 0xBC
    Vec unkC8;              // 0xC8
    s32 unkD4;              // 0xD4
    s32 unkD8;              // 0xD8
    s32 unkDC;              // 0xDC
    s32 unkE0;              // 0xE0
    s32 unkE4;              // 0xE4
    s32 unkE8;              // 0xE8
    f32 unkEC;              // 0xEC
    f32 unkF0;              // 0xF0
    f32 unkF4;              // 0xF4
    f32 unkF8;              // 0xF8
    u8 unkFC[0x104 - 0xFC]; // 0xFC
} BattleCameraWork;

void battleCameraInit(void);

void battleCameraMain(void);
