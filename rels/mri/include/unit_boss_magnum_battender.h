#pragma once

#include "driver/effdrv.h"
#include "evt/evt_cmd.h"

// Per-arm scratch allocated by arm_init and drawn by arm_main.
typedef struct MagnumBattenderWork {
    Vec prevPos;         // 0x00, the unit's position last frame (arm_main)
    Vec pos;             // 0x0C, the unit's position; the effect sits 18 below it
    f32 rotZ;            // 0x18, eased toward targetRotZ and passed to BtlUnit_SetRotate as z
    f32 targetRotZ;      // 0x1C, the angle of the pos - prevPos movement
    EffectEntry* effect; // 0x20
} MagnumBattenderWork;

USER_FUNC(eff_hokori);
EVT_DEF(stamp_attack_event);
