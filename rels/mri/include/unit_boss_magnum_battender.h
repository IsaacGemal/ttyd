#pragma once

#include "driver/effdrv.h"
#include "evt/evt_cmd.h"

// Per-arm scratch allocated by arm_init and drawn by arm_main.
typedef struct MagnumBattenderWork {
    Vec field_0x00;      // 0x00
    Vec field_0x0C;      // 0x0C
    f32 field_0x18;      // 0x18
    f32 field_0x1C;      // 0x1C
    EffectEntry* effect; // 0x20
} MagnumBattenderWork;

USER_FUNC(eff_hokori);
EVT_DEF(stamp_attack_event);
