#include "battle/ac/ac_stick_keep_left.h"

#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// sdata2 float constants used by the disp function.
extern const f32 float_neg300_8041c418;
extern const f32 float_30_8041c41c;
extern const f32 float_0_8041c420;

// sdata table of per-difficulty hit windows, indexed by BattleActionCommandGetDifficulty.
s32 ac_stickkeepleft_ok_frame[7] = {0x50, 0x46, 0x3C, 0x28, 0x14, 0x0A, 0x05};

// rodata sound-effect names, passed to psndSFXOn (0x802EFBA8/0x802EFBB4 in the original).
const char str_802EFBA8[] = "SFX_AC_PI1";
const char str_802EFBB4[] = "SFX_AC_PONE1";

// Local to this file (.fn actionCommandDisp, local in ac_stick_keep_left.s).
// The original's caller passes a third argument in r3 even though the body
// derives everything from the battle work, so keep the 3-argument signature.
static void actionCommandDisp(f32 x, f32 y, s32 type);

// StickKeepLeft action-command work area at BattleWork + 0x1F4C. No header declares it.
typedef struct StickKeepLeftWork {
    s32 field_0x00; // 0x00, countdown to the next note
    s32 field_0x04; // 0x04, current note index
    s32 field_0x08; // 0x08, post-command timer
    u32 sfx[12];    // 0x0C, psndSFXOn handles for the notes (u32 so `== -1` compares unsigned)
} StickKeepLeftWork;

// optimize_for_size: the original prologue uses stmw/lmw (this file's other functions do too), i.e. built
// with -O4,s; without this pragma MWCC emits separate stw/lwz for r23-r31.
#pragma optimize_for_size on
// NONMATCHING (98.29%): string pointers copied through a `mr`; case 0x3EB empty loop lacks the second
// bound check (`blt` + `cmpwi r3, 0`); ok_frame index gets an `extsb` because battle_ac.h says the
// callee returns `s8` while its definition is `u8` (`lbz r3, 0x1cbd(r3)`).
s32 battleAcMain_StickKeepLeft(BattleWork* work) {
    const char* piName;
    const char* poneName;
    StickKeepLeftWork* dec = (StickKeepLeftWork*)((u8*)work + 0x1F4C);
    BattleACDispParams* disp = (BattleACDispParams*)((u8*)work + 0x1F20);
    s32 stateChanged;
    s32 autoCommand = 0;
    s32 i;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    *(s32*)&work->actionCommands.field_34[0x10] = 0;
    if (work->actionCommands.mAcState != 0) {
        if (*(s32*)&work->actionCommands.field_34[0xC] != 0) {
            *(s32*)&work->actionCommands.field_34[0x10] = 1;
        }
        else {
            *(s32*)&work->actionCommands.field_34[0x10] = 0;
        }
    }

    piName = str_802EFBA8;
    poneName = str_802EFBB4;

    for (;;) {
        stateChanged = 0;
        if (*(s32*)&work->actionCommands.field_34[0x10] != 0) {
            break;
        }

        switch (work->actionCommands.mAcState) {
        case 0:
            work->actionCommands.result = 1;
            dec->field_0x04 = 0;
            memset(disp, 0, 0x2C);
            disp->iconX = float_neg300_8041c418;
            disp->iconY = float_0_8041c420;
            disp->counter = 0x14;
            work->actionCommands.outputParams[0] = 0;
            *(s32*)&work->actionCommands.field_34[0xC] = 0;
            *(s32*)&work->actionCommands.field_34[0x10] = 0;
            *(s32*)&work->actionCommands.field_34[0x14] = 0;
            work->actionCommands.outputParams[2] = 0;
            for (i = 0; i < 12; i++) {
                dec->sfx[i] = -1;
            }
            work->actionCommands.mAcState = 0x63;
            break;
        case 0x63:
            break;
        case 0x64:
            dec->field_0x00 = 0x78;
            work->actionCommands.mAcState = 0x3E8;
            // fall through
        case 0x3E8:
            if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
                if (work->actionCommands.field_0x4 & 2) {
                    work->actionCommands.field_0x8--;
                    if (work->actionCommands.field_0x8 >= 0) {
                        break;
                    }
                }
                if (irand(100) >= 0) {
                    work->actionCommands.result = 0;
                }
                else {
                    work->actionCommands.result = 2;
                    work->actionCommands.mResultCount++;
                }
                return 0;
            }
            if ((s32)BattlePadCheckNow(0x40000) == 0 && *(s16*)&work->actionCommands.field_78[0x10] <= 0 && !autoCommand) {
                if (--dec->field_0x00 <= 0) {
                    work->actionCommands.result = 0;
                    stateChanged = 1;
                    work->actionCommands.mAcState = 0x3EB;
                }
                break;
            }
            work->actionCommands.outputParams[0] = 1;
            work->actionCommands.mAcState = 0x3E9;
            dec->field_0x00 = 1;
            dec->field_0x04 = 1;
            work->actionCommands.outputParams[1] = 0;
            break;
        case 0x3E9:
            if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
                (*(s16*)&work->actionCommands.field_78[0x10])--;
                work->actionCommands.result = 2;
                stateChanged = 1;
                work->actionCommands.mAcState = 0x3EB;
            }
            dec->field_0x00--;
            if (dec->field_0x00 < 0) {
                dec->field_0x04++;
                work->actionCommands.outputParams[1]++;
                if (dec->field_0x04 >= *(s32*)&work->actionCommands.field_34[4] + 2) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EB;
                    work->actionCommands.outputParams[2] = 2;
                    if (*(s32*)&work->actionCommands.field_34[0] & 2) {
                        work->actionCommands.result |= 2;
                        work->actionCommands.result &= ~1;
                    }
                    stateChanged = 1;
                }
                else {
                    s32 idx = dec->field_0x04 - 2;
                    if (idx < *(s32*)&work->actionCommands.field_34[4] - 1) {
                        if (dec->sfx[idx] == -1) {
                            dec->sfx[idx] = psndSFXOn(piName);
                        }
                    }
                    else {
                        if (dec->sfx[idx] == -1) {
                            dec->sfx[idx] = psndSFXOn(poneName);
                            *(s32*)&work->actionCommands.field_34[0x14] = 1;
                        }
                    }
                    dec->field_0x00 = *(s32*)&work->actionCommands.field_34[8];
                }
                break;
            }
            if ((s32)BattlePadCheckNow(0x40000) != 0 && !autoCommand) {
                break;
            }
            {
                s32 hit = 0;
                if (dec->field_0x04 != *(s32*)&work->actionCommands.field_34[4] + 1) {
                    if (!(*(s32*)&work->actionCommands.field_34[0] & 2)) {
                        work->actionCommands.outputParams[2] = 1;
                    }
                }
                else {
                    s32 remain = *(s32*)&work->actionCommands.field_34[8] - dec->field_0x00;
                    if (remain > ac_stickkeepleft_ok_frame[BattleActionCommandGetDifficulty(work)]) {
                        if (!(*(s32*)&work->actionCommands.field_34[0] & 2)) {
                            work->actionCommands.outputParams[2] = 1;
                        }
                    }
                    else {
                        work->actionCommands.outputParams[2] = 3;
                        hit = 1;
                    }
                }
                if (autoCommand) {
                    if (hit) {
                        work->actionCommands.result |= 2;
                        work->actionCommands.result &= ~1;
                        stateChanged = 1;
                        work->actionCommands.mAcState = 0x3EB;
                    }
                }
                else if (hit) {
                    work->actionCommands.result |= 2;
                    work->actionCommands.result &= ~1;
                    stateChanged = 1;
                    work->actionCommands.mAcState = 0x3EB;
                }
                else if (!(*(s32*)&work->actionCommands.field_34[0] & 2)) {
                    work->actionCommands.result &= ~1;
                    stateChanged = 1;
                    work->actionCommands.mAcState = 0x3EB;
                }
            }
            break;
        case 0x3EB:
            for (i = 0; i <= dec->field_0x04 - 2; i++) {
            }
            work->actionCommands.mAcState = 0x3EC;
            break;
        case 0x3ED:
            dec->field_0x08 = 0x3C;
            work->actionCommands.mAcState = 0x3EE;
            break;
        case 0x3EE:
            dec->field_0x08--;
            if (dec->field_0x08 <= 0) {
                work->actionCommands.mAcState = 0x3EF;
            }
            break;
        case 0x3EF:
            work->actionCommands.maincb = NULL;
            work->actionCommands.dispcb = NULL;
            work->actionCommands.resultcb = NULL;
            work->actionCommands.deletecb = NULL;
            break;
        default:
            break;
        }

        if (!stateChanged) {
            break;
        }
    }

    return 1;
}
#pragma optimize_for_size reset

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
