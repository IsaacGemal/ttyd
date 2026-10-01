#include "battle/ac/ac_stick_rotate.h"

#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// StickRotate action-command work area at _battleWorkPointer + 0x1F4C.
typedef struct StickRotateWork {
    s8 field_0x00;    // 0x00, _GetInputDir result
    s8 field_0x01;    // 0x01, previous direction
    s8 field_0x02;    // 0x02, last accepted direction
    s8 field_0x03;    // 0x03, clamped direction
    s8 field_0x04;    // 0x04, roll direction
    s8 field_0x05;    // 0x05, roll counter
    s8 field_0x06;    // 0x06, icon id
    u8 field_0x07;    // 0x07, icon timer
    u16 field_0x08;   // 0x08, roll step count
    u8 field_0x0A[2]; // 0x0A
    s32 field_0x0C;   // 0x0C, work timer
    u16 field_0x10;   // 0x10, roll counter
    u8 field_0x12[2]; // 0x12
    s32 field_0x14;   // 0x14, post-AC timer
} StickRotateWork;

// Float literals the original unit emitted into .sdata2.
extern const f32 float_neg200_804204d0;
extern const f32 float_70_804204d4;
extern const f32 float_1_804204d8;
extern const f32 float_neg300_804204dc;
extern const f32 float_30_804204e0;
extern const f32 float_0_804204e4;

// The original emits nine separate zero Vecs into .rodata (vec3_802fd560 ... vec3_802fd5c0), one per
// `Vec v = {0}` initializer, and reads them from one base register; one array reproduces that.
#ifdef MUST_MATCH
#define STICK_VEC_BASE const Vec* vec = vec3_802fd560;
#define STICK_ZERO_VEC(i) (vec[i])
const Vec vec3_802fd560[9] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};
#else
#define STICK_VEC_BASE
#define STICK_ZERO_VEC(i) ((Vec){ 0.0f, 0.0f, 0.0f })
#endif

void battleAcDelete_StickRotate(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}

// Match-only: the header declares _get_icon_id as s32(s32), but the original passes an s8 field raw
// (`lbz r3`) and passes the s16 icon id straight to iconDispGx (no `extsh`). The cast keeps the call a
// direct `bl _get_icon_id`; same workaround as ac_gauge_timing/ac_lst_rc_keep.
#ifdef MUST_MATCH
#define GET_ICON_ID(id) (((s16 (*)(s8))_get_icon_id)((id)))
#else
#define GET_ICON_ID(id) _get_icon_id((id))
#endif

// opt_lifetimes/opt_propagation: without both, MWCC folds the StickRotateWork base (work+0x1F4C) into
// the BattleWork base instead of materialising it in its own register as the original does.
#pragma opt_lifetimes off // needed: the default lifetime pass folds the StickRotateWork base into the BattleWork base
#pragma opt_propagation off // needed: with propagation on the same fold happens
void actionCommandDisp2(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    StickRotateWork* sr = (StickRotateWork*)&ac->field_0x2BC[0];
    STICK_VEC_BASE

    switch (work->actionCommands.mAcState) {
        case 0x63:
        case 0x64: {
            Vec v = STICK_ZERO_VEC(6);
            v.x = float_neg200_804204d0 + x;
            v.y = float_70_804204d4 + y;
            iconDispGx(&v, 0x10, 0x79, float_1_804204d8);
            break;
        }
        case 0x3E8:
        case 0x3E9: {
            Vec v;
            sr->field_0x07++;
            if (sr->field_0x07 >=
                (*(s32*)&work->actionCommands.field_34[0x14] +
                 *(s32*)&work->actionCommands.field_34[0x18]) /
                    2) {
                s32 roll;
                sr->field_0x07 = 0;
                roll = sr->field_0x04;
                if (roll == 0) {
                    roll = -1;
                }
                if (sr->field_0x06 == 0) {
                    sr->field_0x06 = 1;
                } else {
                    sr->field_0x06 += roll;
                    if (sr->field_0x06 >= 9) {
                        sr->field_0x06 = 1;
                    } else if (sr->field_0x06 <= 0) {
                        sr->field_0x06 = 8;
                    }
                }
            }
            v = STICK_ZERO_VEC(7);
            v.x = float_neg200_804204d0 + x;
            v.y = float_70_804204d4 + y;
            iconDispGx(&v, 0x10, GET_ICON_ID(sr->field_0x06), float_1_804204d8);
            break;
        }
        case 0x3EA:
        case 0x3EB:
        case 0x3EC:
        case 0x3ED:
        case 0x3EE: {
            Vec v = STICK_ZERO_VEC(8);
            v.x = float_neg200_804204d0 + x;
            v.y = float_70_804204d4 + y;
            iconDispGx(&v, 0x10, 0x79, float_1_804204d8);
            break;
        }
        default:
            break;
    }
}

// The target keeps the work-area base `work + 0x1F4C` in r5 (`addi r5, r4, 0x1f4c`) and
// addresses the fields off it; MWCC folds every derived base into the BattleWork r4
// (`lbz r3, 0x1f4e(r4)`), so that one instruction and its six displacements differ. This is the
// same register-allocation wall as battleAcMain_CraneTiming / battleAcDisp_LstRcKeep.
// NONMATCHING (99.08%): work-area base folded into BattleWork; everything else matches.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    StickRotateWork* sr = (StickRotateWork*)((u8*)work + 0x1F4C);
    STICK_VEC_BASE

    switch (work->actionCommands.mAcState) {
        case 0x63:
        case 0x64: {
            Vec v = STICK_ZERO_VEC(0);
            v.x = float_neg200_804204d0 + x;
            v.y = float_70_804204d4 + y;
            iconDispGx(&v, 0x10, 0x79, float_1_804204d8);
            break;
        }
        case 0x3E8: {
            Vec v = STICK_ZERO_VEC(1);
            v.x = float_neg200_804204d0 + x;
            v.y = float_70_804204d4 + y;
            iconDispGx(&v, 0x10, _get_icon_id(sr->field_0x02), float_1_804204d8);
            break;
        }
        case 0x3E9:
            if (sr->field_0x05 <
                *(s32*)&work->actionCommands.field_34[0x14]) {
                if (sr->field_0x08 <= 1) {
                    Vec v = STICK_ZERO_VEC(2);
                    v.x = float_neg200_804204d0 + x;
                    v.y = float_70_804204d4 + y;
                    iconDispGx(&v, 0x10, _get_icon_id(sr->field_0x02),
                               float_1_804204d8);
                } else {
                    Vec v = STICK_ZERO_VEC(3);
                    v.x = float_neg200_804204d0 + x;
                    v.y = float_70_804204d4 + y;
                    iconDispGx(&v, 0x10, _get_icon_id(sr->field_0x01),
                               float_1_804204d8);
                }
            } else {
                Vec v = STICK_ZERO_VEC(4);
                v.x = float_neg200_804204d0 + x;
                v.y = float_70_804204d4 + y;
                iconDispGx(&v, 0x10, _get_icon_id(sr->field_0x02), float_1_804204d8);
            }
            break;
        case 0x3EA:
        case 0x3EB:
        case 0x3EC:
        case 0x3ED:
        case 0x3EE: {
            Vec v = STICK_ZERO_VEC(5);
            v.x = float_neg200_804204d0 + x;
            v.y = float_70_804204d4 + y;
            iconDispGx(&v, 0x10, 0x79, float_1_804204d8);
            break;
        }
        default:
            break;
    }
}

// dont_inline: the original calls _get_icon_id from both display functions.
#pragma dont_inline on
s16 _get_icon_id(u8 id) {
    switch ((s8)id) {
        case 1:
            return 0x7C;
        case 2:
            return 0x7D;
        case 3:
            return 0x7A;
        case 4:
            return 0x77;
        case 5:
            return 0x76;
        case 6:
            return 0x75;
        case 7:
            return 0x78;
        case 8:
            return 0x7B;
        default:
            return 0x79;
    }
}
#pragma dont_inline reset

// The function-pointer selection must be a single conditional expression (not `if`), otherwise MWCC
// materialises the address through a scratch register (`addi r3, r3, sym@l; mr r31, r3`) instead of
// directly into r31. Deriving disp from a BattleActions* keeps it in r30 instead of folding it into param.
void battleAcDisp_StickRotate(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    void (*disp_fn)(f32, f32);
    BattleACDispParams* disp;

    disp = &ac->dispParams;
    disp_fn = (*(s32*)&ac->field_34[0x00] & 2) ? actionCommandDisp : actionCommandDisp2;

    switch (ac->mAcState) {
        case 0x63:
            disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14,
                                        float_neg300_804204dc, float_30_804204e0);
            disp_fn(disp->iconX, disp->iconY);
            if (disp->counter > 0) {
                disp->counter--;
            }
            break;
        case 0x64:
        case 0x3E8:
        case 0x3E9:
        case 0x3EA:
        case 0x3EB:
            disp->iconX = float_30_804204e0;
            disp_fn(disp->iconX, disp->iconY);
            break;
        case 0x3EC:
        case 0x3ED:
        case 0x3EE:
            if (disp->counter > 0) {
                disp->counter--;
            }
            if (disp->counter <= 0x28) {
                disp->iconX = intplGetValue(4, disp->counter, 0x28,
                                            float_neg300_804204dc, float_30_804204e0);
            } else {
                disp->iconX = float_30_804204e0;
            }
            disp_fn(disp->iconX, disp->iconY);
            break;
        default:
            break;
    }
}

s32 battleAcResult_StickRotate(BattleWork* work) {
    return work->actionCommands.result;
}

// NONMATCHING (85.92%): 3 instructions differ, 25 missing, 21 extra, plus register differences.
s32 battleAcMain_StickRotate(BattleWork* work) {
    StickRotateWork* sr = (StickRotateWork*)((u8*)work + 0x1F4C);
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    s32 cont;

    do {
        cont = 0;
        switch (work->actionCommands.mAcState) {
            case 0:
                work->actionCommands.result = 1;
                memset(disp, 0, 0x2C);
                disp->iconX = float_neg300_804204dc;
                disp->iconY = float_0_804204e4;
                disp->counter = 0x14;
                sr->field_0x02 = 0;
                sr->field_0x03 = 0;
                sr->field_0x06 = 0;
                sr->field_0x07 = 0;
                sr->field_0x10 = 0;
                sr->field_0x05 = 0;
                work->actionCommands.mAcState = 0x63;
                break;
            case 0x64:
                sr->field_0x0C = 0x78;
                work->actionCommands.mAcState = 0x3E8;
                sr->field_0x04 = 0;
                sr->field_0x08 = 0;
                work->actionCommands.outputParams[0] = 0;
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    sr->field_0x02 = 3;
                    sr->field_0x03 = 2;
                }
                // fallthrough
            case 0x3E8:
                if ((work->actionCommands.field_0x4 & 1) &&
                    (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
                    if (work->actionCommands.field_0x4 & 2) {
                        work->actionCommands.field_0x8--;
                        if (work->actionCommands.field_0x8 >= 0) {
                            break;
                        }
                    }
                    if ((s32)irand(100) >= 0) {
                        work->actionCommands.result = 0;
                    } else {
                        work->actionCommands.result = 2;
                        work->actionCommands.mResultCount++;
                    }
                    return 0;
                }
                sr->field_0x00 = _GetInputDir();
                if ((sr->field_0x00 == -1 ||
                     (*(s32*)&work->actionCommands.field_34[0x0C] == 0 &&
                      sr->field_0x00 <= 0)) &&
                    *(s16*)&work->actionCommands.field_78[0x10] <= 0) {
                    if (--sr->field_0x0C <= 0) {
                        work->actionCommands.result = 0;
                        work->actionCommands.mAcState = 0x3EA;
                        cont = 1;
                        break;
                    }
                    break;
                }
                work->actionCommands.mAcState = 0x3E9;
                sr->field_0x01 = 0;
                work->actionCommands.outputParams[0]++;
                sr->field_0x0C = *(s32*)&work->actionCommands.field_34[0x08];
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    sr->field_0x04 = -1;
                    sr->field_0x01 = 4;
                }
                break;
            case 0x3E9: {
                s8 roll;
                if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
                    (*(s16*)&work->actionCommands.field_78[0x10])--;
                    work->actionCommands.result = 2;
                    work->actionCommands.mAcState = 0x3EA;
                    work->actionCommands.outputParams[0] =
                        *(s32*)&work->actionCommands.field_34[0x04];
                    cont = 1;
                }
                sr->field_0x0C--;
                sr->field_0x05++;
                if (*(s32*)&work->actionCommands.field_34[0x0C] != 0 &&
                    !BattlePadCheckNow(
                        *(s32*)&work->actionCommands.field_34[0x0C])) {
                    sr->field_0x0C = 0;
                }
                if (sr->field_0x0C < 0) {
                    if (work->actionCommands.outputParams[0] >=
                            *(s32*)&work->actionCommands.field_34[0x10] &&
                        !BattlePadCheckNow(
                            *(s32*)&work->actionCommands.field_34[0x0C])) {
                        work->actionCommands.result |= 2;
                    }
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                sr->field_0x00 = _GetInputDir();
                if (sr->field_0x00 == -1) {
                    if (work->actionCommands.outputParams[0] >=
                        *(s32*)&work->actionCommands.field_34[0x10]) {
                        work->actionCommands.result |= 2;
                    }
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                if (sr->field_0x00 == 0) {
                    sr->field_0x04 = 0;
                    if ((*(s32*)&work->actionCommands.field_34[0x00] & 1) &&
                        sr->field_0x01 != 0) {
                        work->actionCommands.result &= ~1;
                        work->actionCommands.mAcState = 0x3EA;
                        cont = 1;
                        break;
                    }
                    goto L_801FD700;
                }
                if (sr->field_0x01 == 0) {
                    goto L_801FD700;
                }
                roll = _CalcRollDir();
                if (roll == 0) {
                    if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                        sr->field_0x04 = 0;
                        if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                            work->actionCommands.result &= ~1;
                            work->actionCommands.mAcState = 0x3EA;
                            cont = 1;
                            break;
                        }
                    } else if (sr->field_0x04 != 0) {
                        sr->field_0x04 = 0;
                    }
                    goto L_801FD700;
                }
                if (sr->field_0x04 == 0) {
                    s8 dir;
                    sr->field_0x04 = roll;
                    dir = sr->field_0x00 + sr->field_0x04;
                    if (dir >= 9) {
                        dir = 1;
                    }
                    if (dir <= 0) {
                        dir = 8;
                    }
                    sr->field_0x02 = sr->field_0x00;
                    sr->field_0x03 = dir;
                } else if (sr->field_0x04 != roll) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                if (sr->field_0x01 == sr->field_0x00) {
                    goto L_801FD700;
                }
                sr->field_0x08++;
                work->actionCommands.outputParams[0] = (sr->field_0x08 + 7) / 8;
                if (work->actionCommands.outputParams[0] >
                    *(s32*)&work->actionCommands.field_34[0x04]) {
                    work->actionCommands.outputParams[0] =
                        *(s32*)&work->actionCommands.field_34[0x04];
                }
                if (*(s32*)&work->actionCommands.field_34[0x00] & 1) {
                    if (sr->field_0x08 > 1) {
                        if (sr->field_0x05 >
                            *(s32*)&work->actionCommands.field_34[0x18]) {
                            work->actionCommands.result &= ~1;
                            work->actionCommands.mAcState = 0x3EA;
                            cont = 1;
                            break;
                        }
                        if (sr->field_0x05 <
                            *(s32*)&work->actionCommands.field_34[0x14]) {
                            sr->field_0x10++;
                            if (sr->field_0x10 >
                                *(s32*)&work->actionCommands.field_34[0x1C]) {
                                work->actionCommands.result &= ~1;
                                work->actionCommands.mAcState = 0x3EA;
                                cont = 1;
                                break;
                            }
                        }
                    }
                }
                sr->field_0x05 = 0;
                if (sr->field_0x04 != 0 && sr->field_0x04 != roll) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                if (sr->field_0x00 != sr->field_0x02) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                {
                    s8 dir = sr->field_0x03 + sr->field_0x04;
                    if (dir >= 9) {
                        dir = 1;
                    }
                    if (dir <= 0) {
                        dir = 8;
                    }
                    sr->field_0x01 = sr->field_0x00;
                    sr->field_0x02 = sr->field_0x03;
                    sr->field_0x03 = dir;
                }
                if (sr->field_0x08 <
                    *(s32*)&work->actionCommands.field_34[0x10] * 8) {
                    goto L_801FD700;
                }
                work->actionCommands.result |= 2;
                work->actionCommands.result &= ~1;
                work->actionCommands.mAcState = 0x3EA;
                cont = 1;
                break;
            L_801FD700:
                sr->field_0x01 = sr->field_0x00;
                break;
            }
            case 0x3EA:
                work->actionCommands.mAcState = 0x3EB;
                break;
            case 0x3EC:
                sr->field_0x14 = 0x3C;
                disp->counter = 0x3C;
                work->actionCommands.mAcState = 0x3ED;
                // fallthrough
            case 0x3ED:
                sr->field_0x14--;
                if (sr->field_0x14 <= 0) {
                    work->actionCommands.mAcState = 0x3EE;
                }
                break;
            case 0x3EE:
                work->actionCommands.maincb = NULL;
                work->actionCommands.dispcb = NULL;
                work->actionCommands.resultcb = NULL;
                work->actionCommands.deletecb = NULL;
                break;
            default:
                break;
        }
    } while (cont);

    return 1;
}

// dont_inline: the original calls _CalcRollDir/_GetInputDir; MWCC inline-expanded them.
#pragma dont_inline on
s32 _CalcRollDir(void) {
    BattleWork* work = _battleWorkPointer;
    StickRotateWork* sr = (StickRotateWork*)((u8*)work + 0x1F4C);
    s8 input = sr->field_0x00;
    s8 prev = sr->field_0x01;

    if (input <= 0) {
        return 0;
    }
    if (prev <= 0) {
        return 0;
    }
    if (input == prev) {
        return (u8)sr->field_0x04;
    }
    if (input == 1 && prev == 8) {
        return 1;
    }
    if (prev == 1 && input == 8) {
        return -1;
    }
    {
        s8 diff = input - prev;
        if ((((diff >> 31) ^ diff) - (diff >> 31)) > 1) {
            return 0;
        }
        return diff;
    }
}

s32 _GetInputDir(void) {
    BattleWork* work = _battleWorkPointer;
    u32 mask = *(u32*)&work->actionCommands.field_34[0x0C];
    s32 p;

    if (mask != 0) {
        p = (s32)BattlePadCheckNow(mask);
        if (p == 0) {
            return -1;
        }
    }
    p = (s32)BattlePadCheckNow(0x10000);
    if (p) {
        p = (s32)BattlePadCheckNow(0x80000);
        if (p) {
            return 2;
        }
        p = (s32)BattlePadCheckNow(0x40000);
        return p ? 8 : 1;
    }
    p = (s32)BattlePadCheckNow(0x20000);
    if (p) {
        p = (s32)BattlePadCheckNow(0x80000);
        if (p) {
            return 4;
        }
        p = (s32)BattlePadCheckNow(0x40000);
        return p ? 6 : 5;
    }
    p = (s32)BattlePadCheckNow(0x80000);
    if (p) {
        return 3;
    }
    p = (s32)BattlePadCheckNow(0x40000);
    return p ? 7 : 0;
}
#pragma dont_inline reset
