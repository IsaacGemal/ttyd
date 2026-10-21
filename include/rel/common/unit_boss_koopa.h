#pragma once

#include <dolphin/mtx.h>

// Shared by the two Bowser units, unit_boss_koopa (las) and unit_boss_koopa_tou (tou2), whose whole code is the
// functions in rel/common/unit_boss_koopa/.

// The jump state _icon_jump_icon allocates for an icon and _disp_icon draws.
typedef struct IconJump {
    Vec start;     // 0x0
    Vec pos;       // 0xC
    Vec target;    // 0x18
    s32 frames;    // 0x24
    f32 speed;     // 0x28
    f32 fallAccel; // 0x2C
    f32 vy;        // 0x30
    u8 pad_0x34[0x7C - 0x34];
} IconJump;

// These units pass the icon id converted as u16 and evaluate the scale last; icondrv_gx.h has the definition's
// signature (see the iconDispGx entry in tools/local_prototype_exceptions.json).
void iconDispGx(Vec* position, s16 flags, u16 iconId, f32 scale);
