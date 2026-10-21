#include "battle/ac/ac_stick_rotate.h"

#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>

// StickRotate action-command work area at _battleWorkPointer + 0x1F4C.
typedef struct StickRotateWork {
    s8 inputDir;       // 0x00, _GetInputDir result: 0 neutral, 1-8 compass, -1 nothing held
    s8 prevDir;        // 0x01, direction on the previous frame
    s8 expectDir;      // 0x02, direction the roll must reach next (the icon shows it)
    s8 nextDir;        // 0x03, the direction after expectDir (inputDir + rollDir, wrapped 1-8)
    s8 rollDir;        // 0x04, roll direction: +1 or -1, 0 before the first move
    s8 stepFrames;     // 0x05, frames since the last accepted step
    s8 iconDir;        // 0x06, direction the rotating icon points at (actionCommandDisp2)
    u8 iconTimer;      // 0x07, frames until the icon moves on
    u16 steps;         // 0x08, accepted direction changes (outputParams[0] is (steps + 7) / 8)
    u8 pad_0x0A[2];    // 0x0A
    s32 timer;         // 0x0C, frames left to finish the roll (starts at 120, then acParams[1])
    u16 fastSteps;     // 0x10, steps made in less than acParams[4] frames
    u8 pad_0x12[2];    // 0x12
    s32 endTimer;      // 0x14, frames the result display waits after the action command
} StickRotateWork;

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


static void actionCommandDisp(f32 x, f32 y);

// dont_inline: the original calls _CalcRollDir/_GetInputDir; MWCC inline-expanded them.
#pragma dont_inline on
s32 _GetInputDir(void) {
    BattleWork* work = _battleWorkPointer;
    u32 mask = *(u32*)&work->actionCommands.acParams[2];
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

s8 _CalcRollDir(void) {
    BattleWork* work = _battleWorkPointer;
    StickRotateWork* sr = (StickRotateWork*)&work->actionCommands.field_0x2BC[0];
    s8 input = sr->inputDir;
    s8 prev = sr->prevDir;

    if (input <= 0) {
        return 0;
    }
    if (prev <= 0) {
        return 0;
    }
    if (input == prev) {
        return sr->rollDir;
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
#pragma dont_inline reset

s32 battleAcMain_StickRotate(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    StickRotateWork* sr = (StickRotateWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 cont;

    do {
        cont = 0;
        switch (work->actionCommands.mAcState) {
            case 0:
                work->actionCommands.result = 1;
                memset(disp, 0, 0x2C);
                disp->iconX = -300.0f;
                disp->iconY = 0.0f;
                disp->counter = 0x14;
                sr->expectDir = 0;
                sr->nextDir = 0;
                sr->iconDir = 0;
                sr->iconTimer = 0;
                sr->fastSteps = 0;
                sr->stepFrames = 0;
                work->actionCommands.mAcState = 0x63;
                break;
            case 0x63:
                break;
            case 0x64:
                sr->timer = 0x78;
                work->actionCommands.mAcState = 0x3E8;
                sr->rollDir = 0;
                sr->steps = 0;
                work->actionCommands.outputParams[0] = 0;
                if (work->actionCommands.acFlag & 2) {
                    sr->expectDir = 3;
                    sr->nextDir = 2;
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
                sr->inputDir = _GetInputDir();
                if ((sr->inputDir == -1 ||
                     (work->actionCommands.acParams[2] == 0 &&
                      sr->inputDir <= 0)) &&
                    work->actionCommands.guardFrames <= 0) {
                    if (--sr->timer <= 0) {
                        work->actionCommands.result = 0;
                        work->actionCommands.mAcState = 0x3EA;
                        cont = 1;
                        break;
                    }
                    break;
                }
                work->actionCommands.mAcState = 0x3E9;
                sr->prevDir = 0;
                work->actionCommands.outputParams[0]++;
                sr->timer = work->actionCommands.acParams[1];
                if (work->actionCommands.acFlag & 2) {
                    sr->rollDir = -1;
                    sr->prevDir = 4;
                }
                break;
            case 0x3E9: {
                s8 roll;
                if (work->actionCommands.guardFrames > 0) {
                    (work->actionCommands.guardFrames)--;
                    work->actionCommands.result = 2;
                    work->actionCommands.mAcState = 0x3EA;
                    work->actionCommands.outputParams[0] =
                        work->actionCommands.acParams[0];
                    cont = 1;
                }
                sr->timer--;
                sr->stepFrames++;
                if (work->actionCommands.acParams[2] != 0 &&
                    !BattlePadCheckNow(
                        work->actionCommands.acParams[2])) {
                    sr->timer = 0;
                }
                if (sr->timer < 0) {
                    if (work->actionCommands.outputParams[0] >=
                            work->actionCommands.acParams[3] &&
                        !BattlePadCheckNow(
                            work->actionCommands.acParams[2])) {
                        work->actionCommands.result |= 2;
                    }
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                sr->inputDir = _GetInputDir();
                if (sr->inputDir == -1) {
                    if (work->actionCommands.outputParams[0] >=
                        work->actionCommands.acParams[3]) {
                        work->actionCommands.result |= 2;
                    }
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                if (sr->inputDir == 0) {
                    sr->rollDir = 0;
                    if ((work->actionCommands.acFlag & 1) &&
                        sr->prevDir != 0) {
                        work->actionCommands.result &= ~1;
                        work->actionCommands.mAcState = 0x3EA;
                        cont = 1;
                        break;
                    }
                    goto L_801FD700;
                }
                if (sr->prevDir == 0) {
                    goto L_801FD700;
                }
                roll = _CalcRollDir();
                if (roll != 0) {
                if (sr->rollDir == 0) {
                    s8 dir;
                    sr->rollDir = roll;
                    dir = sr->inputDir + sr->rollDir;
                    if (dir >= 9) {
                        dir = 1;
                    }
                    if (dir <= 0) {
                        dir = 8;
                    }
                    sr->expectDir = sr->inputDir;
                    sr->nextDir = dir;
                } else if (sr->rollDir != roll) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                if (sr->prevDir == sr->inputDir) {
                    goto L_801FD700;
                }
                sr->steps++;
                work->actionCommands.outputParams[0] = (sr->steps + 7) / 8;
                if (work->actionCommands.outputParams[0] >
                    work->actionCommands.acParams[0]) {
                    work->actionCommands.outputParams[0] =
                        work->actionCommands.acParams[0];
                }
                if (work->actionCommands.acFlag & 1) {
                    if (sr->steps > 1) {
                        if (sr->stepFrames >
                            work->actionCommands.acParams[5]) {
                            work->actionCommands.result &= ~1;
                            work->actionCommands.mAcState = 0x3EA;
                            cont = 1;
                            break;
                        }
                        if (sr->stepFrames <
                            work->actionCommands.acParams[4]) {
                            sr->fastSteps++;
                            if (sr->fastSteps >
                                work->actionCommands.acParams[6]) {
                                work->actionCommands.result &= ~1;
                                work->actionCommands.mAcState = 0x3EA;
                                cont = 1;
                                break;
                            }
                        }
                    }
                } else {
                    goto L_801FD700;
                }
                sr->stepFrames = 0;
                if (sr->rollDir != 0 && sr->rollDir != roll) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                if (sr->inputDir != sr->expectDir) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EA;
                    cont = 1;
                    break;
                }
                {
                    s8 dir = sr->nextDir + sr->rollDir;
                    if (dir >= 9) {
                        dir = 1;
                    }
                    if (dir <= 0) {
                        dir = 8;
                    }
                    sr->prevDir = sr->inputDir;
                    sr->expectDir = sr->nextDir;
                    sr->nextDir = dir;
                }
                if (sr->steps <
                    work->actionCommands.acParams[3] * 8) {
                    goto L_801FD700;
                }
                work->actionCommands.result |= 2;
                work->actionCommands.result &= ~1;
                work->actionCommands.mAcState = 0x3EA;
                cont = 1;
                break;
                }
                if (work->actionCommands.acFlag & 2) {
                    sr->rollDir = 0;
                    if (work->actionCommands.acFlag & 2) {
                        work->actionCommands.result &= ~1;
                        work->actionCommands.mAcState = 0x3EA;
                        cont = 1;
                        break;
                    }
                } else if (sr->rollDir != 0) {
                    sr->rollDir = 0;
                }
            L_801FD700:
                sr->prevDir = sr->inputDir;
                break;
            }
            case 0x3EA:
                work->actionCommands.mAcState = 0x3EB;
                break;
            case 0x3EC:
                sr->endTimer = 0x3C;
                disp->counter = 0x3C;
                work->actionCommands.mAcState = 0x3ED;
                // fallthrough
            case 0x3ED:
                sr->endTimer--;
                if (sr->endTimer <= 0) {
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


s32 battleAcResult_StickRotate(BattleWork* work) {
    return work->actionCommands.result;
}


// The function-pointer selection must be a single conditional expression (not `if`), otherwise MWCC
// materialises the address through a scratch register (`addi r3, r3, sym@l; mr r31, r3`) instead of
// directly into r31. Deriving disp from a BattleActions* keeps it in r30 instead of folding it into param.
void battleAcDisp_StickRotate(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    void (*disp_fn)(f32, f32);
    BattleACDispParams* disp;

    disp = &ac->dispParams;
    disp_fn = (ac->acFlag & 2) ? actionCommandDisp : actionCommandDisp2;

    switch (ac->mAcState) {
        case 0x63:
            disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14,
                                        -300.0f, 30.0f);
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
            disp->iconX = 30.0f;
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
                                            -300.0f, 30.0f);
            } else {
                disp->iconX = 30.0f;
            }
            disp_fn(disp->iconX, disp->iconY);
            break;
        default:
            break;
    }
}


// dont_inline: the original calls _get_icon_id from both display functions.
#pragma dont_inline on
s32 _get_icon_id(s8 id) {
    switch (id) {
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

#pragma opt_propagation off // needed: with propagation on MWCC folds the StickRotateWork base (work+0x1F4C) into the BattleWork base
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    StickRotateWork* sr = (StickRotateWork*)&ac->field_0x2BC[0];
    STICK_VEC_BASE

    switch (work->actionCommands.mAcState) {
        case 0x63:
        case 0x64: {
            Vec v = STICK_ZERO_VEC(0);
            v.x = -200.0f + x;
            v.y = 70.0f + y;
            iconDispGx(1.0f, &v, 0x10, 0x79);
            break;
        }
        case 0x3E8: {
            Vec v = STICK_ZERO_VEC(1);
            v.x = -200.0f + x;
            v.y = 70.0f + y;
            iconDispGx(1.0f, &v, 0x10, _get_icon_id(sr->expectDir));
            break;
        }
        case 0x3E9:
            if (sr->stepFrames <
                work->actionCommands.acParams[4]) {
                if (sr->steps <= 1) {
                    Vec v = STICK_ZERO_VEC(2);
                    v.x = -200.0f + x;
                    v.y = 70.0f + y;
                    iconDispGx(1.0f, &v, 0x10,
                               _get_icon_id(sr->expectDir));
                } else {
                    Vec v = STICK_ZERO_VEC(3);
                    v.x = -200.0f + x;
                    v.y = 70.0f + y;
                    iconDispGx(1.0f, &v, 0x10,
                               _get_icon_id(sr->prevDir));
                }
            } else {
                Vec v = STICK_ZERO_VEC(4);
                v.x = -200.0f + x;
                v.y = 70.0f + y;
                iconDispGx(1.0f, &v, 0x10, _get_icon_id(sr->expectDir));
            }
            break;
        case 0x3EA:
        case 0x3EB:
        case 0x3EC:
        case 0x3ED:
        case 0x3EE: {
            Vec v = STICK_ZERO_VEC(5);
            v.x = -200.0f + x;
            v.y = 70.0f + y;
            iconDispGx(1.0f, &v, 0x10, 0x79);
            break;
        }
        default:
            break;
    }
}
#pragma opt_propagation reset


#pragma opt_propagation off // needed: with propagation on MWCC folds the StickRotateWork base (work+0x1F4C) into the BattleWork base
void actionCommandDisp2(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    StickRotateWork* sr = (StickRotateWork*)&ac->field_0x2BC[0];
    STICK_VEC_BASE

    switch (work->actionCommands.mAcState) {
        case 0x63:
        case 0x64: {
            Vec v = STICK_ZERO_VEC(6);
            v.x = -200.0f + x;
            v.y = 70.0f + y;
            iconDispGx(1.0f, &v, 0x10, 0x79);
            break;
        }
        case 0x3E8:
        case 0x3E9: {
            Vec v;
            sr->iconTimer++;
            if (sr->iconTimer >=
                (work->actionCommands.acParams[4] +
                 work->actionCommands.acParams[5]) /
                    2) {
                s32 roll;
                sr->iconTimer = 0;
                roll = sr->rollDir;
                if (roll == 0) {
                    roll = -1;
                }
                if (sr->iconDir == 0) {
                    sr->iconDir = 1;
                } else {
                    sr->iconDir += roll;
                    if (sr->iconDir >= 9) {
                        sr->iconDir = 1;
                    } else if (sr->iconDir <= 0) {
                        sr->iconDir = 8;
                    }
                }
            }
            v = STICK_ZERO_VEC(7);
            v.x = -200.0f + x;
            v.y = 70.0f + y;
            iconDispGx(1.0f, &v, 0x10, _get_icon_id(sr->iconDir));
            break;
        }
        case 0x3EA:
        case 0x3EB:
        case 0x3EC:
        case 0x3ED:
        case 0x3EE: {
            Vec v = STICK_ZERO_VEC(8);
            v.x = -200.0f + x;
            v.y = 70.0f + y;
            iconDispGx(1.0f, &v, 0x10, 0x79);
            break;
        }
        default:
            break;
    }
}
#pragma opt_propagation reset


void battleAcDelete_StickRotate(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}


