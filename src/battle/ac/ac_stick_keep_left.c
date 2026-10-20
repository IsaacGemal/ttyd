#include "battle/ac/ac_stick_keep_left.h"

#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// sdata2 float constants used by actionCommandDisp.
extern const f32 float_neg200_8041c3e8;
extern const f32 float_70_8041c3ec;
extern const f32 float_1_8041c3f0;
extern const f32 float_16_8041c3f4;
extern const f32 float_12_8041c3f8;
extern const f32 float_neg288_8041c3fc;
extern const f32 float_25_8041c400;

// actionCommandDisp's .rodata: the five "_AcSklN" name strings are the compiler's zero-vector
// literal pool anchor, so they must be .rodata (not the default small-data .sdata).
__declspec(section ".rodata") const char str_802EFAD8[] = "_AcSkl1";
__declspec(section ".rodata") const char str_802EFAE0[] = "_AcSkl2";
__declspec(section ".rodata") const char str_802EFAE8[] = "_AcSkl3";
__declspec(section ".rodata") const char str_802EFAF0[] = "_AcSkl4";
__declspec(section ".rodata") const char str_802EFAF8[] = "_AcSkl5";

// sdata table of per-difficulty hit windows, indexed by BattleActionCommandGetDifficulty.
s32 ac_stickkeepleft_ok_frame[7] = {0x50, 0x46, 0x3C, 0x28, 0x14, 0x0A, 0x05};

// The two sound-effect names are written as string literals in battleAcMain rather than as named
// globals: the literals land in .rodata after the int->float double (the original's order), while a
// named object would be emitted before it and shift the double away from its 0xC8 slot.

// Local to this file (.fn actionCommandDisp, local in ac_stick_keep_left.s).
// The original's caller passes a third argument in r3 even though the body
// derives everything from the battle work, so keep the 3-argument signature.
static void actionCommandDisp(f32 x, f32 y, s32 type);

// StickKeepLeft action-command work area at BattleWork + 0x1F4C. No header declares it.
typedef struct StickKeepLeftWork {
    s32 countdown;  // 0x00, frames until the next note (acParams[1] between notes)
    s32 note;       // 0x04, current note index; the notes start at 1
    s32 endTimer;   // 0x08, post-command timer
    u32 sfx[12];    // 0x0C, psndSFXOn handles for the notes (u32 so `== -1` compares unsigned)
} StickKeepLeftWork;

// optimize_for_size: the original prologue uses stmw/lmw (this file's other functions do too), i.e. built
// 100% via three match-only shapes (all under MUST_MATCH below/inside):
//  - an array for the two SFX names makes MWCC materialise each address straight into its callee-saved
//    register (`addi r31/r30, rX, @l`); two separate pointer locals give `addi rX, rX, @l` + `mr`;
//  - case 0x3EB keeps the count in a local `n` and re-tests it, adding the original's first `blt`;
//  - the ok_frame index casts the call through the callee's real 4-byte return type. battle_ac.h says
//    `s8`, but the callee is `lbz r3, 0x1cbd(r3); blr` (a whole word) and the original caller emits a
//    bare `slwi r0, r3, 2`; report: header/definition should be `s32` and `mAcDifficulty` a `u8`.
s32 battleAcMain_StickKeepLeft(BattleWork* work) {
#ifdef MUST_MATCH
    const char* sfxName[2];
#define STICK_PI_NAME sfxName[0]
#define STICK_PONE_NAME sfxName[1]
#else
    const char* piName;
    const char* poneName;
#define STICK_PI_NAME piName
#define STICK_PONE_NAME poneName
#endif
    StickKeepLeftWork* dec = (StickKeepLeftWork*)&work->actionCommands.field_0x2BC[0];
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    s32 stateChanged;
    s32 autoCommand = 0;
    s32 i;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    work->actionCommands.acParams[3] = 0;
    if (work->actionCommands.mAcState != 0) {
        if (work->actionCommands.acParams[2] != 0) {
            work->actionCommands.acParams[3] = 1;
        }
        else {
            work->actionCommands.acParams[3] = 0;
        }
    }

    STICK_PI_NAME = "SFX_AC_PI1";
    STICK_PONE_NAME = "SFX_AC_PONE1";

    for (;;) {
        stateChanged = 0;
        if (work->actionCommands.acParams[3] != 0) {
            break;
        }

        switch (work->actionCommands.mAcState) {
        case 0:
            work->actionCommands.result = 1;
            dec->note = 0;
            memset(disp, 0, 0x2C);
            disp->iconX = -300.0f;
            disp->iconY = 0.0f;
            disp->counter = 0x14;
            work->actionCommands.outputParams[0] = 0;
            work->actionCommands.acParams[2] = 0;
            work->actionCommands.acParams[3] = 0;
            work->actionCommands.acParams[4] = 0;
            work->actionCommands.outputParams[2] = 0;
            for (i = 0; i < 12; i++) {
                dec->sfx[i] = -1;
            }
            work->actionCommands.mAcState = 0x63;
            break;
        case 0x63:
            break;
        case 0x64:
            dec->countdown = 0x78;
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
            if ((s32)BattlePadCheckNow(0x40000) == 0 && work->actionCommands.guardFrames <= 0 && !autoCommand) {
                if (--dec->countdown <= 0) {
                    work->actionCommands.result = 0;
                    stateChanged = 1;
                    work->actionCommands.mAcState = 0x3EB;
                }
                break;
            }
            work->actionCommands.outputParams[0] = 1;
            work->actionCommands.mAcState = 0x3E9;
            dec->countdown = 1;
            dec->note = 1;
            work->actionCommands.outputParams[1] = 0;
            break;
        case 0x3E9:
            if (work->actionCommands.guardFrames > 0) {
                (work->actionCommands.guardFrames)--;
                work->actionCommands.result = 2;
                stateChanged = 1;
                work->actionCommands.mAcState = 0x3EB;
            }
            dec->countdown--;
            if (dec->countdown < 0) {
                dec->note++;
                work->actionCommands.outputParams[1]++;
                if (dec->note >= work->actionCommands.acParams[0] + 2) {
                    work->actionCommands.result &= ~1;
                    work->actionCommands.mAcState = 0x3EB;
                    work->actionCommands.outputParams[2] = 2;
                    if (work->actionCommands.acFlag & 2) {
                        work->actionCommands.result |= 2;
                        work->actionCommands.result &= ~1;
                    }
                    stateChanged = 1;
                }
                else {
                    s32 idx = dec->note - 2;
                    if (idx < work->actionCommands.acParams[0] - 1) {
                        if (dec->sfx[idx] == -1) {
                            dec->sfx[idx] = psndSFXOn(STICK_PI_NAME);
                        }
                    }
                    else {
                        if (dec->sfx[idx] == -1) {
                            dec->sfx[idx] = psndSFXOn(STICK_PONE_NAME);
                            work->actionCommands.acParams[4] = 1;
                        }
                    }
                    dec->countdown = work->actionCommands.acParams[1];
                }
                break;
            }
            if ((s32)BattlePadCheckNow(0x40000) != 0 && !autoCommand) {
                break;
            }
            {
                s32 hit = 0;
                if (dec->note != work->actionCommands.acParams[0] + 1) {
                    if (!(work->actionCommands.acFlag & 2)) {
                        work->actionCommands.outputParams[2] = 1;
                    }
                }
                else {
                    s32 remain = work->actionCommands.acParams[1] - dec->countdown;
#ifdef MUST_MATCH
                    // Match-only: the callee really returns 4 bytes; the `s8` header would emit `extsb`.
                    if (remain > ac_stickkeepleft_ok_frame[((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work)]) {
#else
                    if (remain > ac_stickkeepleft_ok_frame[BattleActionCommandGetDifficulty(work)]) {
#endif
                        if (!(work->actionCommands.acFlag & 2)) {
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
                else if (!(work->actionCommands.acFlag & 2)) {
                    work->actionCommands.result &= ~1;
                    stateChanged = 1;
                    work->actionCommands.mAcState = 0x3EB;
                }
            }
            break;
#ifdef MUST_MATCH
        case 0x3EB: {
            // Match-only: the extra `n >= 0` test reproduces the original's first `blt` before `mtctr`.
            s32 n = dec->note - 2;
            if (n >= 0) {
                for (i = 0; i <= n; i++) {
                }
            }
            work->actionCommands.mAcState = 0x3EC;
            break;
        }
#else
        case 0x3EB:
            for (i = 0; i <= dec->note - 2; i++) {
            }
            work->actionCommands.mAcState = 0x3EC;
            break;
#endif
        case 0x3ED:
            dec->endTimer = 0x3C;
            work->actionCommands.mAcState = 0x3EE;
            break;
        case 0x3EE:
            dec->endTimer--;
            if (dec->endTimer <= 0) {
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

#undef STICK_PI_NAME
#undef STICK_PONE_NAME
s32 battleAcResult_StickKeepLeft(BattleWork* work) {
    return work->actionCommands.result;
}

// optimize_for_size: the original prologue uses stmw/lmw (this file's other functions do too), i.e. built
void battleAcDisp_StickKeepLeft(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    StickKeepLeftWork* dec = (StickKeepLeftWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3E8:
        *(s32*)&disp->field_0x24[0] = -1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.0f,
                                    30.0f);
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (ac->acParams[3] == 0 && disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E9:
        *(s32*)&disp->field_0x24[0] = dec->note - 1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.0f,
                                    30.0f);
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (ac->acParams[3] == 0 && disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
    case 0x3EC:
        actionCommandDisp(30.0f, 0.0f, *(s32*)&disp->field_0x24[0]);
        break;
    case 0x3ED:
    case 0x3EE:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.0f,
                                        -300.0f);
        }
        else {
            disp->iconX = 30.0f;
        }
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (ac->acParams[3] == 0 && disp->counter < 0x3C) {
            disp->counter++;
        }
        break;
    default:
        break;
    }
}
void battleAcDelete_StickKeepLeft(BattleWork* work) {
    work->actionCommands.mAcState = 0x3ED;
}

// The original emits this last (the asm lists it first, and game code is compiled with
// -inline auto,deferred, which emits functions in reverse source order).
#ifdef MUST_MATCH
// match-only: going through an inline keeps the named 25.0f in its own FPR (f31), where the plain
// `yy = float_25 + y` reuses y's register (f30) and the whole loop body shifts one FPR down.
static inline f32 stickLeftAdd25(f32 v) { return float_25_8041c400 + v; }
#endif

// The `ac` intermediate (`BattleActions*`, rather than spelling `work->actionCommands.` at every use)
// is what makes MWCC colour `work` in r31 and the .rodata pool base in r30 as the original; with the
// direct form the two registers are swapped and ~55 loads differ (same shape as battleAcMain_PowerGaugeLv).
static void actionCommandDisp(f32 x, f32 y, s32 type) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    s32 count;
    s32 idx;
    s32 i;
    s32 n;
    f32 yy;
    f32 step;

    (void)type;
    camGetPtr(CAMERA_2D);

    count = ac->acParams[0];
    idx = ((StickKeepLeftWork*)&ac->field_0x2BC[0])->note - 1;

    switch (ac->mAcState) {
    case 0:
    case 0x63:
    case 0x64: {
        Vec v = ((Vec){0.0f, 0.0f, 0.0f});
        v.x = float_neg200_8041c3e8 + x;
        v.y = float_70_8041c3ec + y;
        iconDispGx(float_1_8041c3f0, &v, 0x10, 0x79);
        break;
    }
    case 0x3E8:
        if (gp->retraceCount & 8) {
            Vec v = ((Vec){0.0f, 0.0f, 0.0f});
            v.x = float_neg200_8041c3e8 + x;
            v.y = float_70_8041c3ec + y;
            iconDispGx(float_1_8041c3f0, &v, 0x10, 0x78);
        }
        else {
            Vec v = ((Vec){0.0f, 0.0f, 0.0f});
            v.x = float_neg200_8041c3e8 + x;
            v.y = float_70_8041c3ec + y;
            iconDispGx(float_1_8041c3f0, &v, 0x10, 0x79);
        }
        break;
    default:
        if (idx < count) {
            Vec v = ((Vec){0.0f, 0.0f, 0.0f});
            v.x = float_neg200_8041c3e8 + x;
            v.y = float_70_8041c3ec + y;
            iconDispGx(float_1_8041c3f0, &v, 0x10, 0x78);
        }
        else {
            Vec v = ((Vec){0.0f, 0.0f, 0.0f});
            v.x = float_neg200_8041c3e8 + x;
            v.y = float_70_8041c3ec + y;
            iconDispGx(float_1_8041c3f0, &v, 0x10, 0x79);
        }
        break;
    }

    n = 10;
    if (count >= 4) {
        n = count * 2 + 3;
        if (ac->acFlag & 1) {
            n += 2;
        }
    }

    step = float_16_8041c3f4;
    if (ac->acFlag & 1) {
        step = float_12_8041c3f8;
    }

    {
        Vec v = ((Vec){0.0f, 0.0f, 0.0f});
        v.x = float_neg288_8041c3fc + x;
#ifdef MUST_MATCH
        v.y = yy = stickLeftAdd25(y);
#else
        v.y = yy = float_25_8041c400 + y;
#endif
        iconDispGx(float_1_8041c3f0, &v, 0x10, 0x8F);
    }

    for (i = 1; i < n; i++) {
        Vec v = ((Vec){0.0f, 0.0f, 0.0f});
        v.x = x + (step * i + float_neg288_8041c3fc);
        v.y = yy;
        iconDispGx(float_1_8041c3f0, &v, 0x10, 0x8E);
    }

    {
        Vec v = ((Vec){0.0f, 0.0f, 0.0f});
        v.x = x + (step * i + float_neg288_8041c3fc);
        v.y = yy;
        iconDispGx(float_1_8041c3f0, &v, 0x10, 0x90);
    }

    step = 33.0f;
    if (ac->acFlag & 1) {
        step = 24.75f;
    }

    for (i = 0; i < count - 1; i++) {
        if (i < idx) {
            if (i < 3) {
                Vec v = ((Vec){0.0f, 0.0f, 0.0f});
                v.x = x + (step * i + (-258.0f));
                v.y = yy;
#ifdef MUST_MATCH
                // iconDispGx's header says s16 but the original passes a zero-extended u16 (clrlwi, no extsh).
                ((void (*)(Vec*, s16, u16, f32))iconDispGx)(&v, 0x10, (u16)(i + 0x92), float_1_8041c3f0);
#else
                iconDispGx(float_1_8041c3f0, &v, 0x10, (s16)(i + 0x92));
#endif
            }
            else {
                Vec v = ((Vec){0.0f, 0.0f, 0.0f});
                v.x = x + (step * i + (-258.0f));
                v.y = yy;
                iconDispGx(float_1_8041c3f0, &v, 0x10, 0x94);
            }
        }
        else {
            Vec v = ((Vec){0.0f, 0.0f, 0.0f});
            v.x = x + (step * i + (-258.0f));
            v.y = yy;
            iconDispGx(0.5f, &v, 0x10, 0x91);
        }
    }

    if (idx < count) {
        Vec v = ((Vec){0.0f, 0.0f, 0.0f});
        v.x = x + (step * (count - 1) + (-250.0f));
        v.y = yy;
        iconDispGx(float_1_8041c3f0, &v, 0x10, 0x91);
    }
    else {
        Vec v = ((Vec){0.0f, 0.0f, 0.0f});
        v.x = x + (step * (count - 1) + (-250.0f));
        v.y = yy;
        iconDispGx(float_1_8041c3f0, &v, 0x10, 0x95);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_neg200_8041c3e8 = -200.0f;
const f32 float_70_8041c3ec = 70.0f;
const f32 float_1_8041c3f0 = 1.0f;
const f32 float_16_8041c3f4 = 16.0f;
const f32 float_12_8041c3f8 = 12.0f;
const f32 float_neg288_8041c3fc = -288.0f;
const f32 float_25_8041c400 = 25.0f;
