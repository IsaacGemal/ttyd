#pragma once

#include <dolphin/mtx.h>

// Shared by the Pider units (unit_piders, all three versions) and unit_churantalar, with the functions in
// rel/common/unit_piders/.

// Work block allocated for a pider's yarn display (unit + 0x310). piders_draw_yarn fills it each frame and
// piders_draw_yarn_sub draws the yarn as a quad between the two ends.
typedef struct PidersYarnWork {
    Vec top;    // 0x0, the home x/z at y 400
    Vec bottom; // 0xC, the home x/z at y = unit work 2 (the scripts set 0, or ease it from 400 to 0 on entry)
    u8 alpha;   // 0x18, piders_draw_yarn's argument; the material colour's alpha
} PidersYarnWork;

