#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>
#include "battle/ac/ac_button_down.h"

static void actionCommandDisp(s32 type, f32 x, f32 y);

// The seven button masks, used for the "pick a unique button" modes.
u32 _button_table[7] = {0x100, 0x200, 0x400, 0x800, 0x40, 0x20, 0x10};


// Button-down action-command work area at _battleWorkPointer + 0x1F4C. No header declares it yet;
// two parallel per-button arrays and a timer.
typedef struct ButtonDownWork {
    s32 countdown;   // 0x00, presses left to make, from acParams[0]
    s32 prevOutput;  // 0x04, previous outputParams[0] (SFX edge detect)
    u32 buttons[13]; // 0x08, button masks
    s32 states[13];  // 0x3C, 0 = waiting, 1 = pressed
    s32 endTimer;    // 0x70, post-AC timer
} ButtonDownWork;


// The two unique-button loops must be `while (!found)` (a do-while leaves `pick` unspilled and drops
// the frame to 0x30 with only 10 callee-saved GPRs). `bd->states[i] = 0;` must be written inside both
// arms of `case -3` (duplicated, not once after the if/else): a shared store lets MWCC merge the two
// tails, and that alone changes every callee-saved colouring. `i` and `allDone` must be declared here
// at function scope (declaring them in their case blocks leaves work in r21 instead of r22).
// Tried on top of that: all 7! declaration orders, every relevant pragma (opt_lifetimes/propagation/
// dead_assignments/common_subs/peephole/scheduling/global_optimizer/optimize_for_size), types/`register`/
// scope for i, sfx, allDone, smooth, statement order, an inline helper, case order, do-while vs while,
// and pool/data placement. Everything else (work r22, smooth r25, disp r28, bd r29, cont r27, offset
// r26, table r21, found r20, pick r24/r23) matches.
// The last two callee-saved GPRs are i (r30) and sfx (r31). With the plain assignment MWCC starts sfx's
// live range at the assignment and colours it r30, giving i r31 plus a `mr` when the string address is
// materialised; a comma operator before the literal sequences an extra operand into the statement, which
// pushes sfx above i in the allocator's order (r31, direct `addi r31, r3, @l`) and matches exactly.
s32 battleAcMain_ButtonDown(BattleWork* work) {
    const char* sfx;
    ButtonDownWork* bd = (ButtonDownWork*)&work->actionCommands.acWork[0];
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    s32 cont;
    s32 i;
    s32 allDone;
    s32 smooth = 0;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        smooth = 1;
    }
    // Literal, not a named str_SFX_AC_BUTTON_PUSH1: the compiler pools it after actionCommandDisp's
    // generated int->float double, which is what puts the double at .rodata+0x40 (see below).
#ifdef MUST_MATCH
    // (0, ...) is the allocator nudge described above; the value of the left operand is discarded.
    sfx = (0, "SFX_AC_BUTTON_PUSH1");
#else
    sfx = "SFX_AC_BUTTON_PUSH1";
#endif

    do {
        cont = 0;
        switch (work->actionCommands.mAcState) {
            case 0: {
                work->actionCommands.result = 1;
                memset(disp, 0, 0x2C);
                disp->iconX = -300.0f;
                disp->iconY = 0.0f;
                disp->counter = 0x14;
                memset(bd->buttons, 0, 0x34);
                memset(bd->states, 1, 0x34);
                for (i = 0; i < work->actionCommands.acParams[2]; i++) {
                    switch (work->actionCommands.acParams[3]) {
                        case -2:
                            bd->buttons[i] = (irand(100) < 50) ? 0x100 : 0x200;
                            bd->states[i] = 0;
                            break;
                        case -4:
                            bd->buttons[i] = (irand(100) < 50) ? 0x200 : 0x400;
                            bd->states[i] = 0;
                            break;
                        case -3:
                            if (work->actionCommands.acFlag & 4) {
                                bd->buttons[i] = _button_table[irand(4)];
                                bd->states[i] = 0;
                            } else {
                                u32 pick;
                                s32 found = 0;
                                while (!found) {
                                    s32 j;
                                    s32 unique;
                                    pick = _button_table[irand(4)];
                                    unique = 1;
                                    for (j = i - 1; j >= 0; j--) {
                                        if (bd->buttons[j] == pick) {
                                            unique = 0;
                                        }
                                    }
                                    if (unique) {
                                        found = 1;
                                    }
                                }
                                bd->buttons[i] = pick;
                                bd->states[i] = 0;
                            }
                            break;
                        default: {
                            u32 pick;
                            s32 found = 0;
                            while (!found) {
                                s32 j;
                                s32 unique;
                                pick = _button_table[irand(7)];
                                unique = 1;
                                for (j = i - 1; j >= 0; j--) {
                                    if (bd->buttons[j] == pick) {
                                        unique = 0;
                                    }
                                }
                                if (unique) {
                                    found = 1;
                                }
                            }
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
                bd->prevOutput = 0;
                bd->countdown = work->actionCommands.acParams[0];
                // fallthrough
            case 1000:
                if ((work->actionCommands.autoFlags & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
                    if (work->actionCommands.autoFlags & 2) {
                        work->actionCommands.autoTimer--;
                        if (work->actionCommands.autoTimer >= 0) {
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
                if (work->actionCommands.guardFrames > 0) {
                    (work->actionCommands.guardFrames)--;
                    work->actionCommands.result = 2;
                    work->actionCommands.mAcState = 1001;
                    cont = 1;
                    break;
                }
                {
                    allDone = 1;
                    work->actionCommands.outputParams[0] = 0;
                    for (i = 0; i < work->actionCommands.acParams[2]; i++) {
                        if (bd->buttons[i] == 0) {
                            continue;
                        }
                        if (bd->states[i] != 0) {
                            work->actionCommands.outputParams[0]++;
                            continue;
                        }
                        if (BattlePadGetTrigger() == bd->buttons[i] || smooth) {
                            bd->states[i] = 1;
                            work->actionCommands.outputParams[0]++;
                            if (work->actionCommands.acParams[1] == 1) {
                                if (i + 1 < work->actionCommands.acParams[2]) {
                                    allDone = 0;
                                }
                                break;
                            }
                        } else {
                            if (BattlePadGetTrigger() != 0) {
                                work->actionCommands.result = 0;
                                work->actionCommands.mAcState = 1001;
                                if ((work->actionCommands.acFlag & 2) &&
                                    work->actionCommands.outputParams[0] > 0) {
                                    cont = 1;
                                    work->actionCommands.result |= 2;
                                    work->actionCommands.result &= ~1;
                                    work->actionCommands.mAcState = 1001;
                                    break;
                                }
                                return 1;
                            }
                            allDone = 0;
                            if (work->actionCommands.acParams[1] == 1) {
                                break;
                            }
                        }
                    }
                    if (bd->prevOutput != work->actionCommands.outputParams[0]) {
                        psndSFXOn(sfx);
                        bd->prevOutput = work->actionCommands.outputParams[0];
                    }
                    if (allDone) {
                        cont = 1;
                        work->actionCommands.result |= 2;
                        work->actionCommands.result &= ~1;
                        work->actionCommands.mAcState = 1001;
                    } else {
                        bd->countdown--;
                        if (bd->countdown >= 0) {
                            break;
                        }
                        work->actionCommands.result = 0;
                        cont = 1;
                        work->actionCommands.mAcState = 1001;
                        if ((work->actionCommands.acFlag & 2) &&
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
                bd->endTimer = 0x3C;
                work->actionCommands.mAcState = 1004;
                break;
            case 1004:
                bd->endTimer--;
                if (bd->endTimer <= 0) {
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

s32 battleAcResult_ButtonDown(BattleWork* work) {
    return work->actionCommands.result;
}

// Deriving both the state and disp parameters from one BattleActions* (rather than straight from
// the BattleWork*) is what makes MWCC materialise the disp base: `addi r4, r4, 0x1f20`, then 0x20(r4).
void battleAcDisp_ButtonDown(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
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
            x = intplGetValue(4, 20 - disp->counter, 20, -300.0f, 0.0f);
            y = 0.0f;
            break;
        default:
            y = 0.0f;
            x = y;
            break;
    }
    actionCommandDisp(0, x, y);
}

void battleAcDelete_ButtonDown(BattleWork* work) {
    work->actionCommands.mAcState = 1003;
}

// The templates of the five Vec literals below open .rodata, so MWCC places its generated int->float
// 2^52 pool double at .rodata+0x40 and reaches it as `lfd f1, 0x40(r31)`. The banner string
// battleAcMain uses must pool after that double, so battleAcMain writes it as a literal (a declared
// str_SFX_AC_BUTTON_PUSH1 emits first and pushes the double to +0x50, 99.99%).
// Using bd->buttons[k] directly in the zero test and the switch (no `button` temp) is what gives the
// target's r3 = button / r4 = row base split through the switch.
// The icon count (`limit`) and the drawn-button count (`count`) are separate locals, as are the two loop
// indices (`i` counts, `k` draws): one shared `limit`/`i` colours bd and the draw index differently.
static void actionCommandDisp(s32 type, f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    ButtonDownWork* bd;
    s32 k;
    s32 count;
    s32 flags;
    s32 i;
    s32 j;
    s32 limit;
    f32 baseX = -200.0f + x;
    f32 iconY;
    f32 iconX;

    iconX = baseX - 40.0f;
    iconY = 70.0f + y;
    limit = work->actionCommands.acParams[2] * 3 + 2;
    bd = (ButtonDownWork*)&work->actionCommands.acWork[0];

    iconDispGx(1.0f, &(Vec){iconX, iconY, -2.0f}, 0x10, 0x8F);

    iconX += 16.0f;
    for (j = 1; j < limit; j++) {
        iconDispGx(1.0f, &(Vec){iconX, iconY, -2.0f}, 0x10, 0x8E);
        iconX += 16.0f;
    }

    iconDispGx(1.0f, &(Vec){iconX, iconY, -2.0f}, 0x10, 0x90);

    flags = work->actionCommands.acFlag;
    if (flags & 1) {
        count = 0;
        for (i = 0; i < work->actionCommands.acParams[2]; i++) {
            if (bd->states[i] == 0) {
                break;
            }
            count++;
        }
        if (count < work->actionCommands.acParams[2]) {
            count++;
        }
    } else {
        count = work->actionCommands.acParams[2];
    }

    for (k = 0; k < count; k++) {
        s32 pressed;
        s32 released;

        if (bd->buttons[k] == 0) {
            continue;
        }
        switch (bd->buttons[k]) {
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
        if (bd->states[k] != 0) {
            iconDispGx(1.0f, &(Vec){baseX + (f32)(k * 0x30), iconY, 0.0f}, 0x10, pressed);
        } else {
            iconDispGx(1.0f, &(Vec){baseX + (f32)(k * 0x30), iconY, 0.0f}, 0x10, released);
        }
    }
}

