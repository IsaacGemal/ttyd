#pragma once

#include "evt/evt_cmd.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

// Work entry behind the `work` array: two positions, an angle pair and a
// running effect handle, one per arm (0x24 bytes).
typedef struct ArmWork {
    Vec prevPos;          // 0x00
    Vec pos;              // 0x0C
    f32 rotZ;             // 0x18
    f32 targetRotZ;       // 0x1C
    EffectEntry* effect;  // 0x20
} ArmWork;

// Running target-marker state machine: state, timer and the marker effect.
typedef struct TargetWork {
    s32 state;            // 0x0
    s32 timer;            // 0x4
    EffectEntry* effect;  // 0x8
} TargetWork;

USER_FUNC(aud_target_mark);
USER_FUNC(aud_fire_no_hit);
USER_FUNC(aud_fire_hit);
void aud_fire_eff(f32 x, f32 y, f32 z);
void aud_get_pod_pos(BattleUnit* unit, f32* x, f32* y, f32* z);
USER_FUNC(aud_suikomi_fade);
USER_FUNC(aud_suikomi);
USER_FUNC(get_aud_num);
USER_FUNC(aud_jet_end);
USER_FUNC(aud_jet);
USER_FUNC(drill_jet);
USER_FUNC(drill_tor);
EVT_DEF(aud_hit_event);
EVT_DEF(aud_no_hit_event);
EVT_DEF(drill_attack_event);
EVT_DEF(boomerang_attack_event);
EVT_DEF(aud_attack_event);
EVT_DEF(aud_attack_event_sub);

// The original's callers disagree on the result and volume widths, so declare them
// with the widths this unit's asm uses.
u8 psndSFX_get_vol(s32 id);
void psndSFX_vol(s32 id, u8 vol);
