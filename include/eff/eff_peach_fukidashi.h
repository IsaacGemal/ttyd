#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
typedef struct EffPeachFukidashiData {
    u32 flags;      // 0x0, low 2 bits are the type, bit 7 a flag
    Vec position;   // 0x4
    f32 angle;      // 0x10
    f32 radius;     // 0x14
    f32 rotZ;       // 0x18: Disp's z roll of the bubble icon, in degrees (copied from angle)
    s32 lifetime;   // 0x1C
    NpcEntry* npc;  // 0x20
} EffPeachFukidashiData;
void effSetPeachFukiBatu(const char* name);
EffectEntry* effPeachFukidashiEntry(s32 type, NpcEntry* npc, s32 lifetime, f32 a, f32 b, f32 c, f32 radius, f32 angle);
void effPeachFukidashiMain(EffectEntry* effect);
void effPeachFukidashiDisp(CameraId camId, void* param);
