#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_signal_timing.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "battle/battle_disp.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// .sdata2 constants used by this unit. Order matches the original .sdata2 (report).
const u32 dat_804216d0 = 0xFFFFFFFF;
const u32 dat_804216d4 = 0xFFFFFFFF;
const u32 dat_804216d8 = 0xFFFFFFFF;
const u32 dat_804216dc = 0xFFFFFFFF;
const f32 float_neg200_804216e0 = -200.0f;
const f32 float_70_804216e4 = 70.0f;
const f32 float_1_804216e8 = 1.0f;
const f32 float_neg288_804216ec = -288.0f;
const f32 float_25_804216f0 = 25.0f;
const f32 float_0_804216f4 = 0.0f;
const f32 float_1p5_804216f8 = 1.5f;
const f32 float_0p5_804216fc = 0.5f;
const f32 float_15_80421700 = 15.0f;
const f32 float_0p8_80421704 = 0.8f;
const f32 float_0p6_80421708 = 0.6f;
const f32 float_20p8_8042170c = 20.800001f;
const f32 float_neg300_80421710 = -300.0f;
const f32 float_30_80421714 = 30.0f;

// The original unit's .rodata: 14 separate zero vectors, the int->float magic double, then the
// SFX names. The vectors must be separate `vec3_*` objects (not a `vec3[14]` array) so MWCC
// matches the original's 14 symbols; the named double and strings are the original's objects.
// The compiler still pools its own anonymous copy of the magic double at the end (+0xf8), which
// actionCommandDisp then loads instead of the named one at +0xa8 (see the note on that function).
const Vec vec3_802fedc8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fedd4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fede0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fedec = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fedf8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee04 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee10 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee1c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee28 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee34 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee40 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee4c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee58 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fee64 = {0.0f, 0.0f, 0.0f};
const f64 double_to_int_802fee70 = 4503601774854144.0;
const char str_802FEE78[] = "SFX_AC_PI1";
const char str_802FEE84[] = "SFX_AC_PONE1";
const char str_802FEE94[] = "SFX_AC_COMMAND_OK1";
const char str_802FEEA8[] = "SFX_AC_COMMAND_NG1";

// Local to this file in the original (`.fn actionCommandDisp, local`).
static void actionCommandDisp(f32 x, f32 y);

// Tables defined in the original unit's .sdata. Non-const so mwcc places them in .sdata and uses
// the SDA21 address form (`li r3, sym@sda21`).
u32 _button_table_4dir[4] = {0x00010000, 0x00020000, 0x00040000, 0x00080000};
u32 _button_table_abxy[4] = {0x00000100, 0x00000200, 0x00000400, 0x00000800};
s32 ac_signaltiming_ok_frame_range[7] = {0x14, 0x10, 0x0C, 0x0A, 0x06, 0x04, 0x02};
s32 ac_signaltiming_ok_frame_range_2[7] = {0x1E, 0x1E, 0x1E, 0x1E, 0x19, 0x14, 0x0F};

// SignalTiming's action-command work area at BattleWork+0x1F4C (see the unit header).

// dont_inline: the original keeps _get_ok_frame_range as its own function and battleAcMain calls it
// through a `bl`; without this MWCC's deferred inliner folds the two switch arms into the caller.
#pragma dont_inline on
s32 _get_ok_frame_range(s32 idx) {
    BattleWork* work = _battleWorkPointer;

    switch (*(s32*)((u8*)work + 0x1ccc)) {
    case 1:
        return *(s32*)((u8*)work + 0x1ce4) + ac_signaltiming_ok_frame_range_2[idx];
    default:
        return *(s32*)((u8*)work + 0x1ce4) + ac_signaltiming_ok_frame_range[idx];
    }
}
#pragma dont_inline reset

// Required: without this pragma the callee-saved allocation rotates (the SFX pointers drop from
// r31-r28 to r25-r28) and the function falls to ~95%. The rest of the shape is load-bearing too: the
// `cont`/`pressed` resets at the top of the do/while, the selection `while (!found)` loop with
// `unique = 1` after the `irand` call, the `switch`es for the range and in-window tests, the
// `field_34[4] + field_34[0x1C]` association, and reading `bd->field_0x08` directly at each
// `bd->states[...]` index (a cached `idx` makes MWCC keep it in a register and miss).
#pragma optimize_for_size on // needed for the original's callee-saved register assignment
s32 battleAcMain_SignalTiming(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    const char* sfxPi1;
    const char* sfxPone1;
    const char* sfxOk;
    const char* sfxNg;
    s32 pressed;
    AcSignalTimingWork* bd;
    BattleACDispParams* disp;
    s32 cont;
    s32 autoCommand = 0;
    s32 k;
    bd = (AcSignalTimingWork*)&ac->field_0x2BC[0];
    disp = &ac->dispParams;
    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }
    sfxNg = str_802FEEA8;
    sfxPi1 = str_802FEE78;
    sfxPone1 = str_802FEE84;
    sfxOk = str_802FEE94;
    do {
        cont = 0;
        pressed = 0;
        switch (ac->mAcState) {
        case 0: {
            s32 i;
            ac->result = 1;
            memset(disp, 0, 0x2C);
            disp->iconX = float_neg300_80421710;
            disp->iconY = float_0_804216f4;
            disp->counter = 0x14;
            bd->field_0x00 = 0;
            bd->field_0x04 = 0;
            bd->field_0x10 = 0;
            bd->field_0x08 = 0;
            bd->field_0x0C = 0;
            bd->field_0x2C = ((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work);
            bd->field_0x30 = 0;
            *(s32*)&ac->field_34[0x28] = 0;
            for (k = 0; k < 5; k++) {
                bd->states[k] = 0;
            }
            switch (*(s32*)&ac->field_34[0x14]) {
            case 1:
            case 2:
                for (i = 0; i < *(s32*)&ac->field_34[4];) {
                    s32 found = 0;
                    u32 pick;
                    while (!found) {
                        s32 j;
                        s32 unique;
                        if (*(s32*)&ac->field_34[0x14] == 1) {
                            pick = _button_table_4dir[irand(4)];
                        } else {
                            pick = _button_table_abxy[irand(4)];
                        }
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
                    i++;
                }
                bd->field_0x34 = bd->buttons[0];
                break;
            default:
                bd->field_0x34 = 0x100;
                for (i = 0; i < *(s32*)&ac->field_34[4]; i++) {
                    bd->buttons[i] = 0x100;
                }
                bd->field_0x34 = bd->buttons[0];
                break;
            }
            ac->mAcState = 0x63;
            break;
        }
        case 0x63:
            break;
        case 0x64:
            if (*(s16*)&ac->field_78[0x10] > 0) {
                (*(s16*)&ac->field_78[0x10])--;
                ac->result = 2;
                cont = 1;
                ac->mAcState = 0x3ea;
                break;
            }
            bd->field_0x14 = bd->field_0x08 * *(s32*)&ac->field_34[0xC];
            bd->field_0x18 = (bd->field_0x08 + 1) * *(s32*)&ac->field_34[0xC] - 1;
            bd->field_0x1C = bd->field_0x14;
            bd->field_0x20 = bd->field_0x1C + _get_ok_frame_range(bd->field_0x2C) - 1;
            ac->mAcState = 0x3e8;
            // fallthrough
        case 0x3e8:
            bd->field_0x28 = (bd->field_0x0C + 1) * *(s32*)&ac->field_34[0xC] - 1;
            bd->field_0x0C = bd->field_0x0C + 1;
            if (bd->field_0x0C < *(s32*)&ac->field_34[0x1C] + 1) {
                if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                    psndSFXOn(sfxPi1);
                }
            } else {
                if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                    psndSFXOn(sfxPone1);
                }
            }
            bd->field_0x24 = 0;
            ac->mAcState = 0x3e9;
            // fallthrough
        case 0x3e9: {
            s32 inWindow;
            bd->field_0x10 = bd->field_0x10 + 1;
            if (bd->field_0x24 != 0) {
                bd->field_0x30 = 0;
            } else if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C]) {
                if (bd->field_0x10 >= bd->field_0x1C && bd->field_0x10 <= bd->field_0x20) {
                    bd->field_0x30 = 1;
                } else {
                    bd->field_0x30 = 0;
                }
            } else {
                bd->field_0x30 = 0;
            }
            inWindow = 0;
            if (BattlePadCheckTrigger(bd->field_0x34) != 0) {
                pressed = 1;
            }
            if (pressed != 0) {
                switch (*(s32*)&ac->field_34[0x14]) {
                case 2:
                    if (bd->field_0x34 == BattlePadGetTrigger()) {
                        inWindow = 1;
                    } else {
                        inWindow = 0;
                    }
                    break;
                default:
                    inWindow = 1;
                    break;
                }
            }
            if (autoCommand != 0 && bd->field_0x30 != 0) {
                pressed = 1;
                inWindow = 1;
            }
            if (bd->field_0x00 + bd->field_0x04 >= *(s32*)&ac->field_34[4]) {
                pressed = 0;
            }
            if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C] && pressed != 0 && bd->field_0x24 == 0) {
                bd->field_0x24 = 1;
                if (bd->field_0x10 >= bd->field_0x1C && bd->field_0x10 <= bd->field_0x20 && inWindow != 0) {
                    s32 idx;
                    bd->field_0x00 = bd->field_0x00 + 1;
                    *(s32*)&ac->field_34[0x28] = 1;
                    idx = bd->field_0x08;
                    if (idx >= *(s32*)&ac->field_34[0x1C] &&
                        idx < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                        bd->states[idx - *(s32*)&ac->field_34[0x1C]] = 1;
                        if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                            psndSFXOn(sfxOk);
                        }
                    }
                    if (*(s32*)&ac->field_34[0] & 0x10) {
                        *(s32*)&ac->field_34[0x24] = bd->field_0x00;
                        ac->result &= ~1;
                        if (bd->field_0x00 >= *(s32*)&ac->field_34[0x10]) {
                            ac->result |= 2;
                        }
                        cont = 1;
                        ac->mAcState = 0x3ea;
                    }
                } else {
                    if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C] &&
                        bd->field_0x08 < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                        bd->field_0x04 = bd->field_0x04 + 1;
                        if (bd->field_0x10 >= bd->field_0x14) {
                            bd->states[bd->field_0x08 - *(s32*)&ac->field_34[0x1C]] = -1;
                            if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                                psndSFXOn(sfxNg);
                            }
                            *(s32*)&ac->field_34[0x28] = -1;
                        } else {
                            bd->field_0x24 = 0;
                        }
                    }
                }
            }
            if (bd->field_0x24 == 0 && bd->field_0x10 > bd->field_0x20) {
                bd->field_0x24 = 1;
                *(s32*)&ac->field_34[0x28] = -1;
                if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C] &&
                    bd->field_0x08 < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                    bd->field_0x04 = bd->field_0x04 + 1;
                    bd->states[bd->field_0x08 - *(s32*)&ac->field_34[0x1C]] = -1;
                    if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                        psndSFXOn(sfxNg);
                    }
                }
            }
            if (bd->field_0x24 != 0) {
                if (bd->field_0x08 < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                    bd->field_0x08 = bd->field_0x08 + 1;
                    if (bd->field_0x08 < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                        bd->field_0x14 = bd->field_0x08 * *(s32*)&ac->field_34[0xC] - 0xa;
                        bd->field_0x18 = (bd->field_0x08 + 1) * *(s32*)&ac->field_34[0xC] - 1;
                        bd->field_0x1C = bd->field_0x08 * *(s32*)&ac->field_34[0xC];
                        bd->field_0x20 = bd->field_0x1C + _get_ok_frame_range(bd->field_0x2C) - 1;
                        bd->field_0x24 = 0;
                        if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C] &&
                            bd->field_0x08 < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                            bd->field_0x34 = bd->buttons[bd->field_0x08 - *(s32*)&ac->field_34[0x1C]];
                        }
                    }
                }
            }
            if (bd->field_0x10 >= bd->field_0x0C * *(s32*)&ac->field_34[0xC]) {
                if (bd->field_0x0C < *(s32*)&ac->field_34[4] + *(s32*)&ac->field_34[0x1C]) {
                    ac->mAcState = 0x3e8;
                } else {
                    *(s32*)&ac->field_34[0x24] = bd->field_0x00;
                    ac->result &= ~1;
                    if (bd->field_0x00 >= *(s32*)&ac->field_34[0x10]) {
                        ac->result |= 2;
                    }
                    cont = 1;
                    ac->mAcState = 0x3ea;
                }
            }
            break;
        }
        case 0x3ea:
            ac->mAcState = 0x3eb;
            break;
        case 0x3eb:
            break;
        case 0x3ec:
            ac->field_0x30 |= 1;
            bd->field_0x30 = 0;
            bd->field_0x54 = 0x3c;
            ac->mAcState = 0x3ed;
            break;
        case 0x3ed:
            bd->field_0x54 = bd->field_0x54 - 1;
            if (bd->field_0x54 <= 0) {
                ac->mAcState = 0x3ee;
            }
            break;
        case 0x3ee:
            ac->maincb = NULL;
            ac->dispcb = NULL;
            ac->resultcb = NULL;
            ac->deletecb = NULL;
            break;
        default:
            break;
        }
    } while (cont);

    return 1;
}
#pragma optimize_for_size reset
s32 battleAcResult_SignalTiming(BattleWork* work) {
    return work->actionCommands.result;
}
void battleAcDisp_SignalTiming(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_80421710, float_30_80421714);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E8:
    case 0x3E9:
    case 0x3EA:
    case 0x3EB:
        actionCommandDisp(float_30_80421714, float_0_804216f4);
        break;
    case 0x3EC:
    case 0x3ED:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_80421714, float_neg300_80421710);
        } else {
            disp->iconX = float_30_80421714;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3C) {
            disp->counter++;
        }
        break;
    default:
        break;
    }
}
void battleAcDelete_SignalTiming(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}

// Match-only: iconDispGx declares the icon id `s16`, but the original passes the s32 value from
// BattleACGetButtonIcon straight through (no `extsh`); a differently-typed function pointer keeps
// the call a direct `bl iconDispGx`. Same workaround as ac_gauge_timing.c / ac_repeatedly_lv.c.
#ifdef MUST_MATCH
#define ICON_DISP(v, f, id, s) (((void (*)(Vec*, s16, s32, f32))iconDispGx)((v), (f), (id), (s)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((v), (f), (id), (s))
#endif

// Match-only: MWCC hoists the loop-invariant zero-vector copies out of the loops (the target
// reloads them each pass). A volatile copy keeps the load in the loop; the non-volatile reads
// of the other vectors are direct source-object reads.

// optimize_for_size: the original actionCommandDisp is size-optimized (its frame and register
// colouring only match at this level); every other function in the file matches at the default
// -O4,p, so the pragma is scoped to just this function.
#pragma optimize_for_size on
// The assignment order below (work, ac, camGetPtr, num, bd, count) is what makes MWCC colour
// work=r31, bd=r28, num=r27, count=r26 as the original; every zero vector is read directly by
// symbol and the offset expressions are inlined in the icon loops so the multiply lands where
// the target has it. The 3rd loop's copy stays a volatile read (the target reloads it each
// pass).
//  - the offset expressions must be written so the last constant binds to the loop-invariant
//    term: `(i * 0x30 + (field * 0x21 - 0xfa))` and `(num * 0x30 - 0x1a + (field * 32))`
//    (left-grouped or field-first forms reassociate and cost ~10-100 diffs); the t0 level tests
//    stay `field_0x10 >= field_c * t0 - 5`;
//  - `field * 32` (not `field << 5`) lands the Mtx-block temporaries in the target's registers;
//  - the t0 loops' `s32 t0 = (i, i + field)` comma nudge (MUST_MATCH; natural form below) colours
//    t0 in r4 and fixes ~16 diffs; `(i, i + field)` and `i + field` compile identically otherwise;
//  - the int->float magic double is the compiler's own anonymous pool entry (@NNN at +0xf8,
//    reached as `lfd fN, 0xf8(r30)`) where the target loads the named double from the
//    zero-vector base (`lfd fN, 0xa8(r30)`); defining the double as a source object does not make
//    MWCC use it for `(f32)int`, so the extra pool entry shifts the load. ~15 instructions;
//  - the `0x92 + i` icon block's store/conversion schedule still differs (the target delays the
//    `lis 0x4330` store and the magic load; ours hoists them).
// NONMATCHING (98.90%): the remaining diffs are the magic-double load above and that block's
// scheduling; the notes above list them.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work;
    BattleActions* ac;
    AcSignalTimingWork* bd;
    s32 num;
    s32 count;
    s32 i;
    f32 f31;
    f32 f28;

    work = _battleWorkPointer;
    ac = &work->actionCommands;
    camGetPtr(CAMERA_2D);
    num = *(s32*)&ac->field_34[4];
    bd = (AcSignalTimingWork*)&ac->field_0x2BC[0];
    count = bd->field_0x0C;

    if (!(*(s32*)&ac->field_34[0] & 1)) {
        if (bd->field_0x30 != 0) {
            Vec t = vec3_802fedc8;
            t.x = float_neg200_804216e0 + x;
            t.y = float_70_804216e4 + y;
            ICON_DISP(&t, 0x10, 0x65, float_1_804216e8);
        } else {
            Vec t = vec3_802fedd4;
            t.x = float_neg200_804216e0 + x;
            t.y = float_70_804216e4 + y;
            ICON_DISP(&t, 0x10, 0x64, float_1_804216e8);
        }
    }

    {
        s32 limit;
        switch (num) {
        case 1:
            limit = *(s32*)&ac->field_34[0x1C] * 2 + 5;
            break;
        case 3:
            limit = *(s32*)&ac->field_34[0x1C] * 2 + 0xB;
            break;
        case 4:
            limit = *(s32*)&ac->field_34[0x1C] * 2 + 0xD;
            break;
        case 5:
            limit = *(s32*)&ac->field_34[0x1C] * 2 + 0xF;
            break;
        default:
            limit = *(s32*)&ac->field_34[0x1C] * 2 + 0xB;
            break;
        }
        {
            Vec t = vec3_802fede0;
            t.x = float_neg288_804216ec + x;
            f31 = float_25_804216f0 + y;
            t.y = f31;
            ICON_DISP(&t, 0x10, 0x8F, float_1_804216e8);
        }
        for (i = 1; i < limit; i++) {
            Vec t = *(volatile Vec*)&vec3_802fedec;
            t.x = x + (f32)(i * 0x10 - 0x120);
            t.y = f31;
            ICON_DISP(&t, 0x10, 0x8E, float_1_804216e8);
        }
        {
            Vec t = vec3_802fedf8;
            t.x = x + (f32)(i * 0x10 - 0x120);
            t.y = f31;
            ICON_DISP(&t, 0x10, 0x90, float_1_804216e8);
        }
    }

    if (!(*(s32*)&ac->field_34[0] & 4)) {
        Mtx mtx2;
        Mtx mtx;
        Mtx scale;
        s32 number = bd->field_0x00 + *(s32*)&ac->field_34[0x18];
        {
            s32 ioff;
            if (*(s32*)&ac->field_34[0] & 1) {
                ioff = (num * 0x30 - 0x1a + (*(s32*)&ac->field_34[0x1C] * 32));
            } else {
                ioff = (num * 0x30 - 0x1a + (*(s32*)&ac->field_34[0x1C] * 32));
            }
            {
                f32 fneg = float_neg200_804216e0 + x;
                PSMTXTrans(mtx, fneg + (f32)ioff, f31, float_0_804216f4);
            }
        }
        if (*(s32*)&ac->field_34[0] & 8) {
            PSMTXScale(scale, float_1p5_804216f8, float_1p5_804216f8, float_1p5_804216f8);
        } else {
            PSMTXScale(scale, float_1_804216e8, float_1_804216e8, float_1_804216e8);
        }
        PSMTXConcat(mtx, scale, mtx2);
        iconNumberDispGx(mtx2, number, 0, *(GXColor*)&dat_804216d0);
    }

    for (i = 0; i < *(s32*)&ac->field_34[0x1C]; i++) {
        if (i < bd->field_0x0C) {
            if (i < *(s32*)&ac->field_34[0x1C]) {
                Vec t = vec3_802fee04;
                u16 id = (u16)(0x92 + i);
                t.y = f31;
                t.x = x + (f32)(i * 0x21 - 0x102);
                ICON_DISP(&t, 0x10, id, float_1_804216e8);
            } else {
                Vec t = vec3_802fee10;
                t.y = f31;
                t.x = x + (f32)(i * 0x21 - 0x102);
                ICON_DISP(&t, 0x10, 0x94, float_1_804216e8);
            }
        } else {
            Vec t = vec3_802fee1c;
            t.y = f31;
            t.x = x + (f32)(i * 0x21 - 0x102);
            ICON_DISP(&t, 0x10, 0x91, float_0p5_804216fc);
        }
    }

    if (!(*(s32*)&ac->field_34[0] & 1)) {
        for (i = 0; i < num; i++) {
            if (i + *(s32*)&ac->field_34[0x1C] < count) {
                Vec t = vec3_802fee28;
                t.x = x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa)));
                t.y = f31;
                ICON_DISP(&t, 0x10, 0x95, float_1_804216e8);
            } else {
                Vec t = vec3_802fee34;
                t.x = x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa)));
                t.y = f31;
                ICON_DISP(&t, 0x10, 0x91, float_1_804216e8);
            }
        }
    } else if (!(*(s32*)&ac->field_34[0] & 2)) {
        f28 = float_15_80421700 + y;
        for (i = 0; i < num; i++) {
            f32 scale = float_0p8_80421704;
#ifdef MUST_MATCH
            // The leading comma (a discarded second read of `i`) nudges the allocator to colour
            // t0 in r4 as the target; the natural form below colours it r7 and costs ~12 diffs.
            s32 t0 = (i, i + *(s32*)&ac->field_34[0x1C]);
#else
            s32 t0 = i + *(s32*)&ac->field_34[0x1C];
#endif
            if (bd->field_0x0C == t0 && bd->field_0x10 >= *(s32*)&ac->field_34[0xC] * t0 - 5 &&
                bd->field_0x10 < *(s32*)&ac->field_34[0xC] * t0) {
                Vec t = vec3_802fee40;
                t.y = f28;
                t.x = x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa)));
                ICON_DISP(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 0), float_0p8_80421704);
            } else {
                s32 delta;
                Vec t;
                if (bd->field_0x0C <= t0) {
                    delta = -2;
                    scale = float_0p6_80421708;
                } else {
                    delta = -0xa;
                }
                t = vec3_802fee4c;
                t.x = x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa)));
                t.y = (f32)(delta + 0x19) + y;
                ICON_DISP(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 1), scale);
            }
        }
    } else {
        f28 = float_20p8_8042170c + f31;
        for (i = 0; i < num; i++) {
#ifdef MUST_MATCH
            s32 t0 = (i, i + *(s32*)&ac->field_34[0x1C]); // see the note on the first t0 loop
#else
            s32 t0 = i + *(s32*)&ac->field_34[0x1C];
#endif
            if (bd->field_0x0C == t0 && bd->field_0x10 >= *(s32*)&ac->field_34[0xC] * t0 - 0x14 &&
                bd->field_0x10 <= *(s32*)&ac->field_34[0xC] * t0) {
                Vec t = vec3_802fee58;
                t.x = x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa)));
                t.y = f31;
                ICON_DISP(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 0), float_0p8_80421704);
            } else if (bd->field_0x0C <= t0) {
                if (i >= num - 1) {
                    // Match-only: the original sets r5=0 before every btlDispTexPlane call (the
                    // callee's GXColor is the 2nd GPR); the extra trailing s32 reproduces it, as in
                    // battle_seq_end.c. The natural 7-argument call is under #else.
#ifdef MUST_MATCH
                    ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
                        x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa))),
                        f28, float_0_804216f4, float_0p8_80421704, float_0p8_80421704,
                        0x40, *(GXColor*)&dat_804216d4, 0);
#else
                    btlDispTexPlane(0x40, *(GXColor*)&dat_804216d4,
                                    x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa))),
                                    f28, float_0_804216f4, float_0p8_80421704, float_0p8_80421704);
#endif
                }
            } else {
                Vec t = vec3_802fee64;
                t.x = x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa)));
                t.y = f31;
                ICON_DISP(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 1), float_0p8_80421704);
            }
        }
    }

    f28 = f31 - float_25_804216f0;
    for (i = 0; i < num; i++) {
        if (bd->states[i] == 1) {
#ifdef MUST_MATCH
            ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
                x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa))), f28,
                float_0_804216f4, float_1_804216e8, float_1_804216e8,
                0x3E, *(GXColor*)&dat_804216d8, 0);
#else
            btlDispTexPlane(0x3E, *(GXColor*)&dat_804216d8,
                            x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa))), f28,
                            float_0_804216f4, float_1_804216e8, float_1_804216e8);
#endif
        } else if (bd->states[i] == -1) {
#ifdef MUST_MATCH
            ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
                x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa))), f28,
                float_0_804216f4, float_1_804216e8, float_1_804216e8,
                0x3F, *(GXColor*)&dat_804216dc, 0);
#else
            btlDispTexPlane(0x3F, *(GXColor*)&dat_804216dc,
                            x + (f32)((i * 0x30 + (*(s32*)&ac->field_34[0x1C] * 0x21 - 0xfa))), f28,
                            float_0_804216f4, float_1_804216e8, float_1_804216e8);
#endif
        }
    }
}
#pragma optimize_for_size reset

