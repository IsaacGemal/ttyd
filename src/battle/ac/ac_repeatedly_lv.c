#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/battle.h"
#include "system.h"

extern const f32 float_neg300_8041e82c;
extern const f32 float_30_8041e830;

extern const f32 float_0p01_8041e814;
extern const f32 float_0_8041e828;
extern const f32 float_100_8041e80c;
extern const f32 float_0p4_8041e834;
extern const f32 float_1p5_8041e838;
extern s32 dat_8041e7f0;

static void actionCommandDisp(f32 x, f32 y);

// RepeatedlyLv's action-command work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC).
typedef struct RepeatedlyLvWork {
    s32 field_0x00; // 0x00
    s32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    u32 padCheck;   // 0x0C, BattlePadCheck mask
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    s16 field_0x18; // 0x18
    s16 field_0x1A; // 0x1A
    s16 field_0x1C; // 0x1C
    s16 field_0x1E; // 0x1E
    s16 field_0x20; // 0x20
    s16 field_0x22; // 0x22
    u8 field_0x24;  // 0x24
    s8 field_0x25;  // 0x25
    u8 field_0x26;  // 0x26
    u8 field_0x27;  // 0x27
    u8 pad_0x28[0x30 - 0x28]; // 0x28
    s32 field_0x30; // 0x30, delay timer
} RepeatedlyLvWork;

// Local to this file (.fn _init_param, local in ac_repeatedly_lv.s).
// Materialising the work-area pointer in r4 (as the original does with `addi r4, r3, 0x1f4c` and then
// `0x8(r4)` accesses) needs the BattleActions* intermediate: with `dec` derived straight from
// `&work->actionCommands...` MWCC folds it into the BattleWork base (94.94%); going through a local
// `ac = &work->actionCommands` keeps `work` in r3 and makes the allocator materialise `dec` in r4, as
// battleAcMain_RepeatedlyLv/battleAcMain_TimingA do. The limit temp must be `s16` (the field width):
// as `s32` the zero constant and the limit value swap registers (zero r6/limit r7, target zero r7/
// limit r6). Both together match byte-for-byte.
static void _init_param(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    RepeatedlyLvWork* dec = (RepeatedlyLvWork*)&ac->field_0x2BC[0];

    dec->padCheck = 0;
    dec->field_0x27 = 1;

    switch (*(s32*)&ac->field_34[4]) {
    case 2:
        dec->field_0x08 = 0x200;
        dec->field_0x04 = 0x200;
        dec->field_0x00 = 0x200;
        break;
    case 3:
        dec->field_0x08 = 0x400;
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        break;
    case 4:
        dec->field_0x08 = 0x800;
        dec->field_0x04 = 0x800;
        dec->field_0x00 = 0x800;
        break;
    case 5:
        dec->field_0x08 = 0x20;
        dec->field_0x04 = 0x20;
        dec->field_0x00 = 0x20;
        break;
    case 8:
        dec->padCheck = 0x100;
        // fall through
    case 7:
        dec->field_0x27 = 0;
        // fall through
    case 6:
        dec->field_0x08 = 0x40000;
        dec->field_0x04 = 0x40000;
        dec->field_0x00 = 0x40000;
        break;
    case 11:
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        dec->field_0x08 = 0x200;
        break;
    case 12:
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        dec->field_0x08 = 0x800;
        break;
    case 13:
        dec->field_0x04 = 0x10000;
        dec->field_0x00 = 0x10000;
        dec->field_0x08 = 0x20000;
        break;
    case 14:
        dec->field_0x04 = 0x40000;
        dec->field_0x00 = 0x40000;
        dec->field_0x08 = 0x80000;
        break;
    case 15:
        dec->field_0x04 = 0x40;
        dec->field_0x00 = 0x40;
        dec->field_0x08 = 0x20;
        break;
    default:
        dec->field_0x08 = 0x100;
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        break;
    }

    dec->field_0x22 = 0;
    {
        s16 limit = *(s32*)&ac->field_34[0x08];
        dec->field_0x20 = limit;
        dec->field_0x1E = limit;
    }
    dec->field_0x18 = 0;
    dec->field_0x14 = float_0_8041e828;
    dec->field_0x1C = 0;
    dec->field_0x24 = *(s32*)&ac->field_34[0x0C];
    dec->field_0x1A = *(s32*)&ac->field_34[0x10];
    dec->field_0x10 = float_0p01_8041e814 * (f32)*(s32*)&ac->field_34[0x14];
    dec->field_0x26 = *(s32*)&ac->field_34[0x18];
    if (*(s16*)&ac->field_78[0x10] > 0) {
        *(s16*)&ac->field_78[0x10] -= 1;
        dec->field_0x25 = 1;
    } else {
        dec->field_0x25 = 0;
    }
    ac->outputParams[2] = 0;
    ac->outputParams[0] = 0;
    ac->outputParams[1] = 0;
}

// float_0_8041e828 must be defined (value visible to the compiler) immediately before battleAcMain:
// the gauge-clamp branches then put the zero constant in f0 and the running value in f1, matching the
// original. As an extern (or defined earlier/later in the file) the allocator rotates f0/f1.
const f32 float_0_8041e828 = 0.0f;

// optimize_for_size: the original saves r27-r31 with stmw/lmw and converts int->float via the
// double_to_int magic, both -O4,s hallmarks.
s32 battleAcMain_RepeatedlyLv(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    RepeatedlyLvWork* dec = (RepeatedlyLvWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 stateChanged = 0;
    s32 autoCommand = 0;

    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (ac->mAcState) {
    case 0:
        memset(disp, 0, 0x2C);
        disp->iconX = float_neg300_8041e82c;
        disp->iconY = float_0_8041e828;
        disp->counter = 0x14;
        disp->field_0xC = dat_8041e7f0;
        disp->field_0x10[0] = 0;
        *(s8*)&disp->field_0x10[1] = -0x10;
        *(s8*)&disp->field_0x10[2] = -0x10;
        disp->field_0x10[3] = 0;
        disp->value = float_0_8041e828;
        ac->result = 1;
        _init_param(work);
        ac->mAcState = 0x63;
        break;
    case 0x63:
        break;
    case 0x64:
        if (!((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) &&
            dec->padCheck != 0 &&
            (s32)BattlePadCheckNow(dec->padCheck) == 0 &&
            autoCommand == 0) {
            dec->field_0x22++;
            if (dec->field_0x22 >= 0x96) {
                ac->outputParams[2] = 0;
                ac->result &= ~1;
                ac->result |= 2;
                ac->mAcState = 0x3EA;
            }
            break;
        }
        BattleAcGaugeSeInit();
        ac->outputParams[0] = 1;
        ac->mAcState = 0x3E8;
        // fall through
    case 0x3E8:
        if ((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) {
            if (ac->field_0x4 & 2) {
                ac->field_0x8--;
                if (ac->field_0x8 >= 0) {
                    break;
                }
            }
            ac->result &= ~1;
            ac->result |= 2;
            {
                s32 r = irand(100);

                dec->field_0x18 = 0;
                while (r < 0) {
                    r -= dec->field_0x26;
                    if (r < 0) {
                        break;
                    }
                    dec->field_0x18++;
                }
            }
            ac->outputParams[2] = dec->field_0x18;
            ac->mResultCount++;
            battleAcDelete_RepeatedlyLv(work);
            return 0;
        }
        switch (*(s32*)&ac->field_34[4]) {
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            if (dec->field_0x1C & 1) {
                dec->field_0x00 = dec->field_0x08;
            } else {
                dec->field_0x00 = dec->field_0x04;
            }
            break;
        }
        if (dec->field_0x27 != 0) {
            if (*(s32*)&ac->field_34[0] & 0x20) {
                if ((s32)BattlePadCheckNow(dec->field_0x00) != 0) {
                    stateChanged = 1;
                    dec->field_0x1C++;
                }
            } else if ((s32)BattlePadCheckTrigger(dec->field_0x00) != 0) {
                stateChanged = 1;
                dec->field_0x1C++;
            }
        } else if ((s32)BattlePadCheckUp(dec->field_0x00) != 0) {
            stateChanged = 1;
            dec->field_0x1C++;
        }
        if (autoCommand && !stateChanged) {
            stateChanged = 1;
            dec->field_0x1C++;
        }
        if (dec->field_0x25 == 0) {
            if (stateChanged) {
                dec->field_0x14 += float_0p4_8041e834 * (f32)dec->field_0x1A;
            }
            {
                f32 v = dec->field_0x14;
                if (v >= float_100_8041e80c) {
                    dec->field_0x14 = float_100_8041e80c;
                    dec->field_0x25 = 1;
                    if (*(s32*)&ac->field_34[0] & 0x20) {
                        dec->field_0x25 = -1;
                    }
                } else {
                    dec->field_0x14 = v - dec->field_0x10;
                    if (dec->field_0x14 <= float_0_8041e828) {
                        dec->field_0x14 = float_0_8041e828;
                    }
                }
            }
        } else if (dec->field_0x25 == -1) {
            dec->field_0x14 = dec->field_0x14 - float_1p5_8041e838 * dec->field_0x10;
            if (dec->field_0x14 <= float_0_8041e828) {
                dec->field_0x14 = float_0_8041e828;
                dec->field_0x25 = 0;
            }
        }
        ac->outputParams[1] = (s32)dec->field_0x14;
        {
            s32 r = (s32)dec->field_0x14;
            if (dec->field_0x26 == 0) {
                dec->field_0x26 = 100;
            }
            dec->field_0x18 = 0;
            while (r > 0) {
                r -= dec->field_0x26;
                dec->field_0x18++;
                if (r <= 0) {
                    break;
                }
            }
        }
        if (dec->field_0x18 == 0 && (*(s32*)&ac->field_34[0] & 0x40)) {
            dec->field_0x18 = 1;
        }
        if (dec->field_0x25 != 0 && !(*(s32*)&ac->field_34[0] & 1)) {
            dec->field_0x18++;
        }
        ac->outputParams[2] = dec->field_0x18;
        if (dec->padCheck != 0 && (s32)BattlePadCheckNow(dec->padCheck) == 0) {
            dec->field_0x20 = 0;
        }
        BattleAcGaugeSeUpdate(dec->field_0x14);
        if (dec->field_0x20 > 0) {
            dec->field_0x20--;
            break;
        }
        ac->result &= ~1;
        if (dec->field_0x14 >= (f32)*(s32*)&ac->field_34[0x1C]) {
            ac->result |= 2;
        } else {
            ac->result &= ~2;
        }
        battleAcDelete_RepeatedlyLv(work);
        ac->mAcState = 0x3EA;
        break;
    case 0x3EA:
        dec->field_0x30 = 0x3C;
        ac->mAcState = 0x3EB;
        break;
    case 0x3EB:
        dec->field_0x30--;
        if (dec->field_0x30 <= 0) {
            ac->mAcState = 0x3EC;
        }
        break;
    case 0x3EC:
        ac->maincb = NULL;
        ac->dispcb = NULL;
        ac->resultcb = NULL;
        ac->deletecb = NULL;
        break;
    }
    return 1;
}

s32 battleAcResult_RepeatedlyLv(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_RepeatedlyLv(CameraId camId, void* param) {
    BattleWork* work = (BattleWork*)param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* ag = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3E8:
    case 0x3E9:
        ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, float_neg300_8041e82c, float_30_8041e830);
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter > 0) {
            ag->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
        if (ag->counter >= 0x28) {
            ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, float_30_8041e830, float_neg300_8041e82c);
        } else {
            ag->iconX = float_30_8041e830;
        }
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter < 0x3C) {
            ag->counter++;
        }
        break;
    }
}

// dont_inline: the original keeps the battleAcDelete_RepeatedlyLv call in battleAcMain instead of
// inlining its two stores.
#pragma dont_inline on
void battleAcDelete_RepeatedlyLv(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EA;
    BattleAcGaugeSeDelete();
}
#pragma dont_inline reset

// Defined after the code that uses them, so the references stay bound to these names.
const f32 float_neg300_8041e82c = -300.0f;
const f32 float_30_8041e830 = 30.0f;
