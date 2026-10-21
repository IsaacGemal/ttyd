#pragma once

#include <dolphin/types.h>

// Shared by the Pider units (unit_piders, all three versions) and unit_churantalar, with the functions in
// rel/common/unit_piders/.

// Work block allocated for a pider's yarn display (unit + 0x310).
typedef struct PidersYarnWork {
    f32 field_0x0;   // 0x0
    f32 field_0x4;   // 0x4
    f32 field_0x8;   // 0x8
    f32 field_0xC;   // 0xC
    f32 field_0x10;  // 0x10
    f32 field_0x14;  // 0x14
    u8 field_0x18;   // 0x18
} PidersYarnWork;

