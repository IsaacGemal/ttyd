#include "battle/ac/ac_stick_keep_left.h"

#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "system.h"

// sdata2 float constants used by the disp function.
extern const f32 float_neg300_8041c418;
extern const f32 float_30_8041c41c;
extern const f32 float_0_8041c420;

// Local to this file (.fn actionCommandDisp, local in ac_stick_keep_left.s).
// The original's caller passes a third argument in r3 even though the body
// derives everything from the battle work, so keep the 3-argument signature.
static void actionCommandDisp(f32 x, f32 y, s32 type);

// StickKeepLeft action-command work area at BattleWork + 0x1F4C. No header declares it.
typedef struct StickKeepLeftWork {
    s32 field_0x00; // 0x00
    s32 field_0x04; // 0x04
} StickKeepLeftWork;

s32 battleAcResult_StickKeepLeft(BattleWork* work) {
    return work->actionCommands.result;
}

// optimize_for_size: the original prologue uses stmw/lmw (this file's other functions do too), i.e. built
// with -O4,s; without this pragma MWCC emits separate stw/lwz for r30/r31.
#pragma optimize_for_size on
void battleAcDisp_StickKeepLeft(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    StickKeepLeftWork* dec = (StickKeepLeftWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3E8:
        *(s32*)&disp->field_0x24[0] = -1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_8041c418,
                                    float_30_8041c41c);
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (*(s32*)&ac->field_34[0x10] == 0 && disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E9:
        *(s32*)&disp->field_0x24[0] = dec->field_0x04 - 1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_8041c418,
                                    float_30_8041c41c);
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (*(s32*)&ac->field_34[0x10] == 0 && disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
    case 0x3EC:
        actionCommandDisp(float_30_8041c41c, float_0_8041c420, *(s32*)&disp->field_0x24[0]);
        break;
    case 0x3ED:
    case 0x3EE:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_8041c41c,
                                        float_neg300_8041c418);
        }
        else {
            disp->iconX = float_30_8041c41c;
        }
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (*(s32*)&ac->field_34[0x10] == 0 && disp->counter < 0x3C) {
            disp->counter++;
        }
        break;
    default:
        break;
    }
}
#pragma optimize_for_size reset

void battleAcDelete_StickKeepLeft(BattleWork* work) {
    work->actionCommands.mAcState = 0x3ED;
}
