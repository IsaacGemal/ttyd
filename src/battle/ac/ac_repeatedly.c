#include "battle/ac/ac_repeatedly.h"
#include "battle/battle.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>
#include <dolphin/os.h>
#include <dolphin/si.h>

static void actionCommandDisp(f32 x, f32 y);

// Float literals this unit emitted into .sdata2.
extern const f32 float_10_8041c7e0;
extern const f32 float_0_8041c7d8;
extern const f32 float_0p01_8041c7c0;
extern const f32 float_neg300_8041c7d0;
extern const f32 float_30_8041c7d4;
extern const f32 float_neg1_8041c7dc;
extern const f32 float_100_8041c7bc;
extern const f32 float_neg200_8041c7ac;
extern const f32 float_70_8041c7b0;
extern const f32 float_1_8041c7b4;
extern const f32 float_0p5_8041c7b8;
extern const f32 float_neg288_8041c7c4;
extern const f32 float_176_8041c7c8;
extern const f32 float_45_8041c7cc;

// .rodata zero vectors this unit emitted; the original pools every access through the vec3_802f3160 base.
const Vec vec3_802f3160 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f316c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f3178 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f3184 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f3190 = {0.0f, 0.0f, 0.0f};

// sdata2 word this unit emitted; the original symbol is undefined elsewhere.
__declspec(section ".sdata2") s32 dat_8041c7a8 = -1;

// int->float conversion magic the original unit emitted into .rodata. It must NOT be defined here:
// defining it forces MWCC to emit a second anonymous copy, which shifts actionCommandDisp's pooled
// `lfd 0x40(r30)` to 0x48. Left undefined, MWCC emits its own constant at the original offset and the
// symbol name difference is ignored by the scorer.
//const f64 double_to_int_802f31a0 = 4503601774854144.0;

// .sdata table of per-difficulty power values (local to this unit in the original asm).
static s32 _power_table[7] = {0x37, 0x32, 0x2D, 0x28, 0x23, 0x1E, 0x19};

// Repeatedly's action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct AcRepeatedlyWork {
    u32 field_0x00; // 0x0
    u32 field_0x04; // 0x4
    u32 field_0x08; // 0x8
    u32 field_0x0C; // 0xC
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    u16 field_0x20; // 0x20
    u16 field_0x22; // 0x22
    s16 field_0x24; // 0x24
    s16 field_0x26; // 0x26
    s16 field_0x28; // 0x28
    u8 field_0x2A;  // 0x2A
    s8 field_0x2B;  // 0x2B
    u8 field_0x2C;  // 0x2C
    u8 field_0x2D[3]; // 0x2D
    s32 field_0x30; // 0x30
} AcRepeatedlyWork;

// optimize_for_size: the original _init_param saves its three callee-saved registers with stmw/lmw.
#pragma optimize_for_size on
static void _init_param(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 param;
    s32 timer;

    dec->field_0x2C = 0;
    dec->field_0x0C = 0;

    switch (*(s32*)&ac->field_34[0x04]) {
    case 10:
        work->flags |= 0x20000000;
        break;
    case 8:
        dec->field_0x0C = 0x100;
        // fallthrough
    case 6:
        dec->field_0x08 = 0x40000;
        dec->field_0x04 = 0x40000;
        dec->field_0x00 = 0x40000;
        break;
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
    case 11:
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        dec->field_0x08 = 0x200;
        break;
    case 16:
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        dec->field_0x08 = 0x200;
        dec->field_0x22 = irand(0x1E) + 0x3C;
        break;
    case 12:
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        dec->field_0x08 = 0x800;
        break;
    default:
        dec->field_0x08 = 0x100;
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        break;
    }

    param = *(s32*)&ac->field_34[0x08];
    dec->field_0x28 = param;
    dec->field_0x26 = param;
    dec->field_0x10 = (f32)*(s32*)&ac->field_34[0x0C];
    dec->field_0x14 = (f32)*(s32*)&ac->field_34[0x10] / float_10_8041c7e0;
    if (dec->field_0x14 != float_0_8041c7d8) {
        dec->field_0x10 = (f32)irand(100);
    }
    dec->field_0x18 = (f32)*(s32*)&ac->field_34[0x20];
    disp->value = float_0p01_8041c7c0 * dec->field_0x18;
    dec->field_0x20 = 0;
    dec->field_0x24 = *(s32*)&ac->field_34[0x18];
    dec->field_0x1C = float_0p01_8041c7c0 * (f32)*(s32*)&ac->field_34[0x1C];
    dec->field_0x2A = 0xB2;

    timer = *(s16*)&ac->field_78[0x10];
    if (timer > 0) {
        *(s16*)&ac->field_78[0x10] = timer - 1;
        dec->field_0x2B = 1;
    } else {
        dec->field_0x2B = 0;
    }
    ac->outputParams[0] = 0;
}
#pragma optimize_for_size reset
s32 battleAcMain_Repeatedly(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
#ifdef MUST_MATCH
    // Match-only: battle_ac.h declares the callee `s8`, but the original caller does not sign-extend
    // (its definition returns a zero-extended `lbz`). The cast suppresses MWCC's `extsb`.
    s32 difficulty = ((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work);
#else
    s32 difficulty = BattleActionCommandGetDifficulty(work);
#endif
    s32 stateChanged;
    s32 autoCommand = 0;
    f32 v;
    f32 target;

    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    stateChanged = 0;
    switch (ac->mAcState) {
    case 0:
        memset(disp, 0, 0x2C);
        disp->iconX = float_neg300_8041c7d0;
        disp->iconY = float_0_8041c7d8;
        disp->counter = 0x14;
        disp->field_0xC = dat_8041c7a8;
        disp->field_0x10[0] = 0;
        *(s8*)&disp->field_0x10[1] = -0x10;
        *(s8*)&disp->field_0x10[2] = -0x10;
        disp->field_0x10[3] = 0;
        disp->value = float_0_8041c7d8;
        ac->result = 1;
        dec->field_0x28 = 0;
        _init_param(work);
        ac->mAcState = 0x63;
        break;
    case 0x63:
        break;
    case 0x64:
        if (!((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & 0x10)) &&
            dec->field_0x0C != 0 &&
            (s32)BattlePadCheckNow(dec->field_0x0C) == 0) {
            break;
        }
        BattleAcGaugeSeInit();
        ac->mAcState = 0x3E8;
        ac->outputParams[0] = 1;
        // fallthrough
    case 0x3E8:
        switch (*(s32*)&ac->field_34[4]) {
        case 0xA:
            if (dec->field_0x2C != 0) {
                if (OSGetResetButtonState() == 0) {
                    dec->field_0x2C = 0;
                    stateChanged = 1;
                    dec->field_0x20++;
                }
            } else if (OSGetResetButtonState() != 0) {
                dec->field_0x2C = 1;
                stateChanged = 1;
                dec->field_0x20++;
            }
            break;
        case 9:
            if (dec->field_0x2C != 0) {
                if (SIProbe(0) == 8) {
                    dec->field_0x2C = 0;
                    stateChanged = 1;
                    dec->field_0x20++;
                }
            } else if (SIProbe(0) != 8 && SIProbe(0) != 0x80) {
                dec->field_0x2C = 1;
                stateChanged = 1;
                dec->field_0x20++;
            }
            break;
        case 0x10:
            dec->field_0x22--;
            if (dec->field_0x22 == 0) {
                dec->field_0x22 = irand(0x1E) + 0x3C;
                if (dec->field_0x00 == dec->field_0x04) {
                    dec->field_0x00 = dec->field_0x08;
                } else {
                    dec->field_0x00 = dec->field_0x04;
                }
            }
            if (BattlePadCheckTrigger(dec->field_0x00) != 0) {
                stateChanged = 1;
                dec->field_0x20++;
            }
            break;
        case 0xB:
        case 0xC:
            if (dec->field_0x20 & 1) {
                dec->field_0x00 = dec->field_0x08;
            } else {
                dec->field_0x00 = dec->field_0x04;
            }
            // fallthrough
        default:
            if (BattlePadCheckTrigger(dec->field_0x00) != 0) {
                stateChanged = 1;
                dec->field_0x20++;
            }
            break;
        }

        if (autoCommand && !stateChanged) {
            stateChanged = 1;
            dec->field_0x20++;
        }
        if (dec->field_0x2B == 0) {
            if (stateChanged) {
                dec->field_0x18 += float_0p01_8041c7c0 * (_power_table[difficulty] * (f32)dec->field_0x24);
            }
            target = *(volatile f32*)&dec->field_0x18;
            v = *(volatile f32*)&dec->field_0x1C;
            v = target - v;
            dec->field_0x18 = v;
#ifdef MUST_MATCH
            {
                // Match-only: the volatile read forces the reload into f1 the target has;
                // the natural form reuses f0 and swaps the compare operands.
                f32 g = *(volatile f32*)&dec->field_0x18;
                if (g <= float_0_8041c7d8) {
                    dec->field_0x18 = float_0_8041c7d8;
                }
            }
#else
            if (dec->field_0x18 <= float_0_8041c7d8) {
                dec->field_0x18 = float_0_8041c7d8;
            }
#endif
#ifdef MUST_MATCH
            {
                // Match-only: the volatile read forces the reload into f1 the target has;
                // the natural form reuses f0 and swaps the compare operands.
                f32 g = *(volatile f32*)&dec->field_0x18;
                if (g >= float_100_8041c7bc) {
                    dec->field_0x18 = float_100_8041c7bc;
                    dec->field_0x2B = 1;
                }
            }
#else
            if (dec->field_0x18 >= float_100_8041c7bc) {
                dec->field_0x18 = float_100_8041c7bc;
                dec->field_0x2B = 1;
            }
#endif
        }
        if (float_0_8041c7d8 != dec->field_0x14) {
            dec->field_0x10 += dec->field_0x14;
            if (dec->field_0x10 <= float_0_8041c7d8) {
                dec->field_0x10 = float_0_8041c7d8;
                dec->field_0x14 = dec->field_0x14 * float_neg1_8041c7dc;
            }
#ifdef MUST_MATCH
            {
                // Match-only: the volatile read forces the reload into f1 the target has;
                // the natural form reuses f0 and swaps the compare operands.
                f32 g = *(volatile f32*)&dec->field_0x10;
                if (g >= float_100_8041c7bc) {
                    dec->field_0x10 = float_100_8041c7bc;
                    dec->field_0x14 = dec->field_0x14 * float_neg1_8041c7dc;
                }
            }
#else
            if (dec->field_0x10 >= float_100_8041c7bc) {
                dec->field_0x10 = float_100_8041c7bc;
                dec->field_0x14 = dec->field_0x14 * float_neg1_8041c7dc;
            }
#endif
        }
        if (dec->field_0x0C != 0 && (s32)BattlePadCheckNow(dec->field_0x0C) == 0) {
            dec->field_0x28 = 0;
        }
        BattleAcGaugeSeUpdate(dec->field_0x18);
        if (dec->field_0x28 > 0) {
            dec->field_0x28--;
        } else {
            ac->result &= ~1;
            if (dec->field_0x18 >= dec->field_0x10 || dec->field_0x2B != 0) {
                ac->result |= 2;
            }
            battleAcDelete_Repeatedly(work);
            ac->mAcState = 0x3EA;
        }
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
s32 battleAcResult_Repeatedly(BattleWork* work) {
    return work->actionCommands.result;
}
void battleAcDisp_Repeatedly(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 99:
    case 100:
    case 1000:
    case 1001:
        disp->iconX = intplGetValue(4, 20 - disp->counter, 20, float_neg300_8041c7d0, float_30_8041c7d4);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 1002:
    case 1003:
        if (disp->counter >= 40) {
            disp->iconX = intplGetValue(4, disp->counter - 40, 20, float_30_8041c7d4, float_neg300_8041c7d0);
        } else {
            disp->iconX = float_30_8041c7d4;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 60) {
            disp->counter++;
        }
        break;
    }
}

// dont_inline: the original keeps this as a `bl battleAcDelete_Repeatedly` call instead of inlining it
// into battleAcMain_Repeatedly.
#pragma dont_inline on
void battleAcDelete_Repeatedly(BattleWork* work) {
    switch (*(s32*)&work->actionCommands.field_34[0x04]) {
    case 0xA:
        work->flags &= ~0x20000000;
        break;
    }
    BattleAcGaugeSeDelete();
    work->actionCommands.mAcState = 0x3EA;
}
#pragma dont_inline reset

// optimize_for_size: the original saves f28-f31 with stmw/psq_st and uses `divw` for /7, both
// -O4s hallmarks; the rest of this unit matches either way.
#pragma optimize_for_size on
// 100% matched.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    u8* p = (u8*)disp;
    f32 v;
    f32 target;
    s32 flags;

    camGetPtr(CAMERA_2D);

    if (*(s32*)&ac->field_34[4] != 0x10) {
        s32 iconA;
        s32 iconB;

        switch (dec->field_0x00) {
        case 0x100:
            iconA = 0x65;
            iconB = 0x64;
            break;
        case 0x200:
            iconA = 0x67;
            iconB = 0x66;
            break;
        case 0x400:
            iconA = 0x69;
            iconB = 0x68;
            break;
        case 0x800:
            iconA = 0x6B;
            iconB = 0x6A;
            break;
        case 0x40000:
        default:
            iconA = 0x78;
            iconB = 0x79;
            break;
        }
        if (((*(s32*)&disp->field_0x0[0] / 7) & 1) == 1) {
            Vec vec = vec3_802f3160;
            vec.x = float_neg200_8041c7ac + x;
            vec.y = float_70_8041c7b0 + y;
#ifdef MUST_MATCH
            ((void (*)(Vec*, s16, s32, f32))iconDispGx)(&vec, 0x10, iconB, float_1_8041c7b4);
#else
            iconDispGx(float_1_8041c7b4, &vec, 0x10, iconB);
#endif
        } else {
            Vec vec = vec3_802f316c;
            vec.x = float_neg200_8041c7ac + x;
            vec.y = float_70_8041c7b0 + y;
#ifdef MUST_MATCH
            ((void (*)(Vec*, s16, s32, f32))iconDispGx)(&vec, 0x10, iconA, float_1_8041c7b4);
#else
            iconDispGx(float_1_8041c7b4, &vec, 0x10, iconA);
#endif
        }
    } else {
        s32 alt = (*(s32*)&disp->field_0x0[0] / 7) & 1;
        f32 baseX = float_neg200_8041c7ac + x;
        f32 baseY = float_70_8041c7b0 + y;
        s32 i;
        s32 off;

        for (i = 0, off = 0; i < 2; i++, off += 0x28) {
            s32 iconA;
            s32 iconB;
            s32 match;

            if (i == 0) {
                iconA = 0x65;
                iconB = 0x64;
                if (dec->field_0x00 == dec->field_0x04) {
                    match = 1;
                } else {
                    match = 0;
                }
            } else {
                iconA = 0x67;
                iconB = 0x66;
                if (dec->field_0x00 == dec->field_0x08) {
                    match = 1;
                } else {
                    match = 0;
                }
            }
            if (match) {
                s32 icon = (alt == 1) ? iconB : iconA;
                Vec vec = vec3_802f3178;
                vec.x = baseX + (f32)off;
                vec.y = baseY;
#ifdef MUST_MATCH
                // Match-only: iconDispGx's iconId is s16 but the original passes the register directly.
                ((void (*)(Vec*, s16, s32, f32))iconDispGx)(&vec, 0x10, icon, float_1_8041c7b4);
#else
                iconDispGx(float_1_8041c7b4, &vec, 0x10, icon);
#endif
            } else {
                Vec vec = vec3_802f3184;
                vec.x = baseX + (f32)off;
                vec.y = baseY;
#ifdef MUST_MATCH
                ((void (*)(Vec*, s16, s32, f32))iconDispGx)(&vec, 0x10, iconB, float_0p5_8041c7b8);
#else
                iconDispGx(float_0p5_8041c7b8, &vec, 0x10, iconB);
#endif
            }
        }
    }

    target = dec->field_0x18;
    target = target / float_100_8041c7bc;
    v = disp->value;
    if (v <= target) {
        disp->value = v + float_0p01_8041c7c0;
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        disp->value = v - float_0p01_8041c7c0;
        if (disp->value < target) {
            disp->value = target;
        }
    }

    flags = 0;
    if ((s8)dec->field_0x2B == 1 && disp->value >= float_1_8041c7b4) {
        flags |= 1;
        p[0xD] += p[0x11];
        p[0xE] += p[0x12];
        if (p[0xD] <= 0x80 && (s8)p[0x11] < 0) {
            p[0x12] = 0x10;
            p[0x11] = 0x10;
        }
        if (p[0xD] >= 0xF5 && (s8)p[0x11] > 0) {
            *(s8*)&p[0x12] = -0x10;
            *(s8*)&p[0x11] = -0x10;
        }
    }
    if ((s8)dec->field_0x2B == -1) {
        flags |= 2;
    }

    v = disp->value;
    BattleAcDrawGauge((s32)x, (s32)y, dec->field_0x2A, 2,
                      (*(s32*)&ac->field_34[0] & 2) ? 100 : (s32)dec->field_0x10, 100, 100,
                      flags, v);
    {
        Vec vec = vec3_802f3190;
        vec.x = float_neg288_8041c7c4 + x + (float_176_8041c7c8 * dec->field_0x10) / float_100_8041c7bc;
        vec.y = float_45_8041c7cc + y;
        iconDispGx(float_1_8041c7b4, &vec, 0x10, 0x96);
    }
    (*(s32*)&disp->field_0x0[0])++;
    (*(s32*)&disp->field_0x0[4])++;
}
#pragma optimize_for_size reset

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_neg200_8041c7ac = -200.0f;
const f32 float_70_8041c7b0 = 70.0f;
const f32 float_1_8041c7b4 = 1.0f;
const f32 float_0p5_8041c7b8 = 0.5f;
const f32 float_100_8041c7bc = 100.0f;
const f32 float_0p01_8041c7c0 = 0.01f;
const f32 float_neg288_8041c7c4 = -288.0f;
const f32 float_176_8041c7c8 = 176.0f;
const f32 float_45_8041c7cc = 45.0f;
const f32 float_neg300_8041c7d0 = -300.0f;
const f32 float_30_8041c7d4 = 30.0f;
const f32 float_0_8041c7d8 = 0.0f;
const f32 float_neg1_8041c7dc = -1.0f;
const f32 float_10_8041c7e0 = 10.0f;

// 20 zero bytes that end the unit's .sdata: an all-zero object needs explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804117CC_sdata[5] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
