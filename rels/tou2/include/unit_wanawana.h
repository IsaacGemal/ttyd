#pragma once

#include "evt/evt_cmd.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "memory.h"

// Work area for this unit's paper-texture capture (the 0x60-byte block __memAlloc'd in replace_evt).
typedef struct WanawanaWork {
    s32 captured;         // 0x00, set to 1 once replace_disp has captured a frame; replace_callback sets the paper texture only then
    s32 poseId;           // 0x04
    Mtx mtx;              // 0x08
    GXTexObj texObj;      // 0x38
    BattleUnitPart* part; // 0x58
    smartEntry* tex;      // 0x5C
} WanawanaWork;

USER_FUNC(replace_evt);
void replace_callback(BattleUnitPart* part, s32 arg);
void replace_disp(CameraId cameraId, void* param);
