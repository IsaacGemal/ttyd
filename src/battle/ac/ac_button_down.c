#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>
#include "battle/ac/ac_button_down.h"

static void actionCommandDisp(s32 type, f32 x, f32 y);

// Float literals the original unit emitted into .sdata2 (offsets listed in REPORT.md).
extern const f32 float_neg200_804204e8;
extern const f32 float_40_804204ec;
extern const f32 float_70_804204f0;
extern const f32 float_1_804204f4;
extern const f32 float_16_804204f8;
extern const f32 float_neg300_804204fc;
extern const f32 float_0_80420500;

// Vec literals the original unit emitted into .rodata (offsets listed in REPORT.md). They are one
// contiguous array; the individual obj names in the original .s are aliases at 0xC offsets.
const Vec vec3_802fd5d0[5] = {
    {0.0f, 0.0f, -2.0f},
    {0.0f, 0.0f, -2.0f},
    {0.0f, 0.0f, -2.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};

// The int->float magic constant (2^52) the original unit emitted into .rodata at +0x40. Declared so the
// unit's .rodata layout matches; the cast below still uses a compiler-generated pool copy (see NONMATCHING).
const f64 double_to_int_802fd610 = 4503601774854144.0;

// The seven button masks, used for the "pick a unique button" modes.
u32 _button_table[7] = {0x100, 0x200, 0x400, 0x800, 0x40, 0x20, 0x10};

// SFX name the original unit emitted into .rodata.
const char str_802FD618[] = "SFX_AC_BUTTON_PUSH1";

// Button-down action-command work area at _battleWorkPointer + 0x1F4C. No header declares it yet;
// two parallel per-button arrays and a timer.
typedef struct ButtonDownWork {
    s32 field_0x00;  // 0x00, countdown from ac field_34[4]
    s32 field_0x04;  // 0x04, previous outputParams[0] (SFX edge detect)
    u32 buttons[13]; // 0x08, button masks
    s32 states[13];  // 0x3C, 0 = waiting, 1 = pressed
    s32 field_0x70;  // 0x70, post-AC timer
} ButtonDownWork;

// The original ac_button_down unit was compiled with -O4,s (optimize for size): the prologues use
// stmw/lmw, which -O4,p never emits. The file has no per-Object flag in configure.py, so scope the
// pragma to the two functions that need it.
#pragma optimize_for_size on
#pragma opt_lifetimes off // needed: the default lifetime pass assigns limit/i/work to different callee-saved GPRs than the target
#pragma opt_propagation off // needed: with propagation on limit drops to r29 and iconX/iconY swap FPRs

// NONMATCHING (95.17%): the int->float conversion's 2^52 constant is emitted as an anonymous pool entry
// (lis/addi), where the target reuses the named double_to_int_802fd610 at 0x40(r31); the final loop's
// button/address temporaries are also swapped (button r4 vs the target's r3).
static void actionCommandDisp(s32 type, f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    ButtonDownWork* bd;
    s32 flags;
    s32 i;
    s32 j;
    s32 limit;
    f32 baseX = float_neg200_804204e8 + x;
    f32 iconY;
    f32 iconX;
    const Vec* vec = vec3_802fd5d0;

    iconX = baseX - float_40_804204ec;
    iconY = float_70_804204f0 + y;
    limit = *(s32*)&work->actionCommands.field_34[0x0C] * 3 + 2;
    bd = (ButtonDownWork*)((u8*)work + 0x1F4C);

    {
        Vec t = vec[0];
        t.x = iconX;
        t.y = iconY;
        iconDispGx(&t, 0x10, 0x8F, float_1_804204f4);
    }

    iconX += float_16_804204f8;
    for (j = 1; j < limit; j++) {
#ifdef MUST_MATCH
        // volatile: stops MWCC hoisting the const vec[1] load out of the loop, which the target does not do.
        Vec t = *(volatile Vec*)&vec[1];
#else
        Vec t = vec[1];
#endif
        t.x = iconX;
        t.y = iconY;
        iconDispGx(&t, 0x10, 0x8E, float_1_804204f4);
        iconX += float_16_804204f8;
    }

    {
        Vec t = vec[2];
        t.x = iconX;
        t.y = iconY;
        iconDispGx(&t, 0x10, 0x90, float_1_804204f4);
    }

    flags = *(s32*)&work->actionCommands.field_34[0x00];
    if (flags & 1) {
        limit = 0;
        for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
            if (bd->states[i] == 0) {
                break;
            }
            limit++;
        }
        if (limit < *(s32*)&work->actionCommands.field_34[0x0C]) {
            limit++;
        }
    } else {
        limit = *(s32*)&work->actionCommands.field_34[0x0C];
    }

    for (i = 0; i < limit; i++) {
        u32 button = bd->buttons[i];
        s16 pressed;
        s16 released;

        if (button == 0) {
            continue;
        }
        switch (button) {
            case 0x100:
                pressed = 0x65;
                released = 0x64;
                break;
            case 0x200:
                pressed = 0x67;
                released = 0x66;
                break;
            case 0x400:
                pressed = 0x69;
                released = 0x68;
                break;
            case 0x800:
                pressed = 0x6b;
                released = 0x6a;
                break;
            case 0x40:
                pressed = 0x7f;
                released = 0x7e;
                break;
            case 0x20:
                pressed = 0x81;
                released = 0x80;
                break;
            case 0x10:
                pressed = 0x83;
                released = 0x82;
                break;
            case 0x40000:
            default:
                pressed = 0x78;
                released = 0x79;
                break;
        }
        if (bd->states[i] != 0) {
            Vec v = vec[3];
            v.x = baseX + (f32)(i * 0x30);
            v.y = iconY;
            iconDispGx(&v, 0x10, pressed, float_1_804204f4);
        } else {
            Vec v = vec[4];
            v.x = baseX + (f32)(i * 0x30);
            v.y = iconY;
            iconDispGx(&v, 0x10, released, float_1_804204f4);
        }
    }
}
#pragma optimize_for_size reset

void battleAcDelete_ButtonDown(BattleWork* work) {
    work->actionCommands.mAcState = 1003;
}

// Deriving both the state and disp parameters from one BattleActions* (rather than straight from
// the BattleWork*) is what makes MWCC materialise the disp base: `addi r4, r4, 0x1f20`, then 0x20(r4).
void battleAcDisp_ButtonDown(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    s32 state = ac->mAcState;
    BattleACDispParams* disp = &ac->dispParams;
    f32 x;
    f32 y;

    switch (state) {
        case 0:
        case 99:
            if (disp->counter > 0) {
                disp->counter--;
            }
            x = intplGetValue(4, 20 - disp->counter, 20, float_neg300_804204fc, float_0_80420500);
            y = float_0_80420500;
            break;
        default:
            y = float_0_80420500;
            x = y;
            break;
    }
    actionCommandDisp(0, x, y);
}

s32 battleAcResult_ButtonDown(BattleWork* work) {
    return work->actionCommands.result;
}

// optimize_for_size: battleAcMain's prologue uses stmw/lmw.
#pragma optimize_for_size on
// NONMATCHING (82.01%): the state machine and all logic match, but MWCC assigns the callee-saved
// registers differently (work is r24 instead of r22, smooth r28 instead of r25, ...), so almost every
// instruction that touches a pointer/loop variable differs in its register field.
s32 battleAcMain_ButtonDown(BattleWork* work) {
    ButtonDownWork* bd = (ButtonDownWork*)((u8*)work + 0x1F4C);
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    const char* sfx;
    s32 smooth = 0;
    s32 cont;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        smooth = 1;
    }
    sfx = str_802FD618;

    do {
        cont = 0;
        switch (work->actionCommands.mAcState) {
            case 0: {
                s32 i;
                work->actionCommands.result = 1;
                memset(disp, 0, 0x2C);
                disp->iconX = float_neg300_804204fc;
                disp->iconY = float_0_80420500;
                disp->counter = 0x14;
                memset(bd->buttons, 0, 0x34);
                memset(bd->states, 1, 0x34);
                for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
                    switch (*(s32*)&work->actionCommands.field_34[0x10]) {
                        case -3:
                            if (*(s32*)&work->actionCommands.field_34[0x00] & 4) {
                                bd->buttons[i] = _button_table[irand(4)];
                            } else {
                                u32 pick;
                                s32 found = 0;
                                do {
                                    s32 j;
                                    s32 unique = 1;
                                    pick = _button_table[irand(4)];
                                    for (j = i - 1; j >= 0; j--) {
                                        if (bd->buttons[j] == pick) {
                                            unique = 0;
                                        }
                                    }
                                    if (unique) {
                                        found = 1;
                                    }
                                } while (!found);
                                bd->buttons[i] = pick;
                            }
                            bd->states[i] = 0;
                            break;
                        case -4:
                            bd->buttons[i] = (irand(100) < 50) ? 0x200 : 0x400;
                            bd->states[i] = 0;
                            break;
                        case -2:
                            bd->buttons[i] = (irand(100) < 50) ? 0x100 : 0x200;
                            bd->states[i] = 0;
                            break;
                        default: {
                            u32 pick;
                            s32 found = 0;
                            do {
                                s32 j;
                                s32 unique = 1;
                                pick = _button_table[irand(7)];
                                for (j = i - 1; j >= 0; j--) {
                                    if (bd->buttons[j] == pick) {
                                        unique = 0;
                                    }
                                }
                                if (unique) {
                                    found = 1;
                                }
                            } while (!found);
                            bd->buttons[i] = pick;
                            bd->states[i] = 0;
                            break;
                        }
                    }
                }
                work->actionCommands.mAcState = 99;
                break;
            }
            case 100:
                work->actionCommands.mAcState = 1000;
                bd->field_0x04 = 0;
                bd->field_0x00 = *(s32*)&work->actionCommands.field_34[4];
                // fallthrough
            case 1000:
                if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
                    if (work->actionCommands.field_0x4 & 2) {
                        work->actionCommands.field_0x8--;
                        if (work->actionCommands.field_0x8 >= 0) {
                            break;
                        }
                    }
                    if (irand(100) >= 0) {
                        work->actionCommands.result = 0;
                    } else {
                        work->actionCommands.result = 2;
                        work->actionCommands.mResultCount++;
                    }
                    return 0;
                }
                if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
                    (*(s16*)&work->actionCommands.field_78[0x10])--;
                    work->actionCommands.result = 2;
                    work->actionCommands.mAcState = 1001;
                    cont = 1;
                    break;
                }
                {
                    s32 i;
                    s32 allDone = 1;
                    work->actionCommands.outputParams[0] = 0;
                    for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
                        u32 button = bd->buttons[i];

                        if (button == 0) {
                            continue;
                        }
                        if (bd->states[i] != 0) {
                            work->actionCommands.outputParams[0]++;
                            continue;
                        }
                        if (BattlePadGetTrigger() == button || smooth) {
                            bd->states[i] = 1;
                            work->actionCommands.outputParams[0]++;
                            if (*(s32*)&work->actionCommands.field_34[8] == 1) {
                                if (i + 1 < *(s32*)&work->actionCommands.field_34[0x0C]) {
                                    allDone = 0;
                                }
                                goto check;
                            }
                        } else {
                            if (BattlePadGetTrigger() != 0) {
                                work->actionCommands.result = 0;
                                work->actionCommands.mAcState = 1001;
                                if ((*(s32*)&work->actionCommands.field_34[0x00] & 2) &&
                                    work->actionCommands.outputParams[0] > 0) {
                                    work->actionCommands.result |= 2;
                                    work->actionCommands.result &= ~1;
                                    work->actionCommands.mAcState = 1001;
                                    goto check;
                                }
                                return 1;
                            }
                            allDone = 0;
                            if (*(s32*)&work->actionCommands.field_34[8] == 1) {
                                goto check;
                            }
                        }
                    }
                check:
                    if (bd->field_0x04 != work->actionCommands.outputParams[0]) {
                        psndSFXOn(sfx);
                        bd->field_0x04 = work->actionCommands.outputParams[0];
                    }
                    if (allDone) {
                        work->actionCommands.result |= 2;
                        work->actionCommands.result &= ~1;
                        work->actionCommands.mAcState = 1001;
                        cont = 1;
                    } else if (--bd->field_0x00 < 0) {
                        work->actionCommands.result = 0;
                        work->actionCommands.mAcState = 1001;
                        cont = 1;
                        if ((*(s32*)&work->actionCommands.field_34[0x00] & 2) &&
                            work->actionCommands.outputParams[0] > 0) {
                            work->actionCommands.result |= 2;
                            work->actionCommands.result &= ~1;
                            work->actionCommands.mAcState = 1001;
                        }
                    }
                }
                break;
            case 1001:
                work->actionCommands.mAcState = 1002;
                // fallthrough
            case 1002:
                return 0;
            case 1003:
                bd->field_0x70 = 0x3C;
                work->actionCommands.mAcState = 1004;
                break;
            case 1004:
                bd->field_0x70--;
                if (bd->field_0x70 <= 0) {
                    work->actionCommands.mAcState = 1005;
                }
                break;
            case 1005:
                work->actionCommands.maincb = NULL;
                work->actionCommands.dispcb = NULL;
                work->actionCommands.resultcb = NULL;
                work->actionCommands.deletecb = NULL;
                break;
            case 99:
            default:
                break;
        }
    } while (cont);

    return 1;
}
#pragma optimize_for_size reset
