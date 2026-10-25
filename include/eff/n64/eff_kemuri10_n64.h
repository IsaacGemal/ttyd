#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effKemuri10N64Entry(s32 a1, f32 a2, f32 a3, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8);
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
typedef struct effKemuri10Data {
    s32 field_0x0;    // 0x0 written at init, never read
    Vec position;     // 0x4
    f32 velX;         // 0x10 -4 * the direction, added to position each frame
    f32 velY;         // 0x14
    f32 velZ;         // 0x18
    Mtx orient;       // 0x1C columns: up (0), side (1), dir (2); the zero column 3 is the translation
    f32 texScroll;    // 0x4C texture-matrix U offset, advanced by texScrollSpeed each frame
    f32 texScrollSpeed; // 0x50
    f32 scale;        // 0x54
    s32 tevReg0A;     // 0x58 TEVREG0 alpha, ramps up for phases over 5 and down under 5
    s32 life;         // 0x5C frames left (16 at init); the effect is deleted below 0
    s32 timer;        // 0x60 frames elapsed
    u8 tevReg0R;      // 0x64 TEVREG0 red
    u8 tevReg0G;      // 0x65 TEVREG0 green
    u8 tevReg0B;      // 0x66 TEVREG0 blue
    u8 field_0x67[3]; // 0x67 a second copy of the TEVREG0 RGB, written at init and never read
    u8 pad_0x6A[2];   // 0x6A
    s32 camId;        // 0x6C dispEntry camera (CAMERA_3D)
} effKemuri10Data;
void effKemuri10Disp(CameraId camId, void* param);
void effKemuri10Main(EffectEntry* effect);
