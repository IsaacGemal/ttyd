#include "battle/ac/ac_shot.h"
#include "battle/battle.h"
#include <string.h>

// .sdata2 constants used by _ac_disp_init; the original emits them as named symbols.
extern const f32 float_neg200_8041e4d0;
extern const f32 float_0_8041e4b8;
extern u32 dat_8041e4a8;

s32 battleAcResult_Shot(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_Shot(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}

static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = float_neg200_8041e4d0;
    disp->iconY = float_0_8041e4b8;
    disp->counter = 0x14;
    disp->field_0xC = dat_8041e4a8;
    disp->field_0x10[0] = 0;
    *(s8*)&disp->field_0x10[1] = -0x10;
    *(s8*)&disp->field_0x10[2] = -0x10;
    disp->field_0x10[3] = 0;
    disp->value = float_0_8041e4b8;
}

const f32 float_neg200_8041e4d0 = -200.0f;
const f32 float_0_8041e4b8 = 0.0f;
u32 dat_8041e4a8 = 0xFFFFFFFF;
