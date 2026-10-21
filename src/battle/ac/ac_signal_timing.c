#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_signal_timing.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "battle/battle_disp.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>
#pragma pool_data on // the original addresses its .rodata through one base register

// .sdata2 constants used by this unit. Order matches the original .sdata2 (report).
const u32 dat_804216d0 = 0xFFFFFFFF;
const u32 dat_804216d4 = 0xFFFFFFFF;
const u32 dat_804216d8 = 0xFFFFFFFF;
const u32 dat_804216dc = 0xFFFFFFFF;

// The original unit's .rodata: 14 separate zero vectors, the int->float magic double, then the
// SFX names. The vectors must be separate `vec3_*` objects (not a `vec3[14]` array) so MWCC
// matches the original's 14 symbols; the named double and strings are the original's objects.
// The compiler still pools its own anonymous copy of the magic double at the end (+0xf8), which
// actionCommandDisp then loads instead of the named one at +0xa8 (see the note on that function).
const Vec vec3_802fedc8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fedd4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fede0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fedec = {0.0f, 0.0f, 0.0f};

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

    switch (work->actionCommands.acParams[1]) {
    case 1:
        return work->actionCommands.acParams[7] + ac_signaltiming_ok_frame_range_2[idx];
    default:
        return work->actionCommands.acParams[7] + ac_signaltiming_ok_frame_range[idx];
    }
}
#pragma dont_inline reset

// Required: without this pragma the callee-saved allocation rotates (the SFX pointers drop from
// r31-r28 to r25-r28) and the function falls to ~95%. The rest of the shape is load-bearing too: the
// `cont`/`pressed` resets at the top of the do/while, the selection `while (!found)` loop with
// `unique = 1` after the `irand` call, the `switch`es for the range and in-window tests, the
// `acParams[0] + acParams[6]` association, and reading `bd->curSignal` directly at each
// `bd->states[...]` index (a cached `idx` makes MWCC keep it in a register and miss).
#pragma optimize_for_size on // needed: without it battleAcMain_SignalTiming 95.49%
#ifdef MUST_MATCH
// The target's string pool is PI,PONE,OK,NG while its four address loads in the entry block run
// NG,PI,PONE,OK. Reassigning after the initializers gives that emission order; the initializers
// keep the pool entries in the target's order, which needs the dead-store elimination off.
#pragma opt_dead_assignments off
#endif
s32 battleAcMain_SignalTiming(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
#ifdef MUST_MATCH
    const char* sfxPi1 = "SFX_AC_PI1";
    const char* sfxPone1 = "SFX_AC_PONE1";
    const char* sfxOk = "SFX_AC_COMMAND_OK1";
    const char* sfxNg = "SFX_AC_COMMAND_NG1";
#else
    const char* sfxPi1;
    const char* sfxPone1;
    const char* sfxOk;
    const char* sfxNg;
#endif
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
#ifdef MUST_MATCH
    sfxNg = "SFX_AC_COMMAND_NG1";
    sfxPi1 = "SFX_AC_PI1";
    sfxPone1 = "SFX_AC_PONE1";
    sfxOk = "SFX_AC_COMMAND_OK1";
#else
    sfxPi1 = "SFX_AC_PI1";
    sfxPone1 = "SFX_AC_PONE1";
    sfxOk = "SFX_AC_COMMAND_OK1";
    sfxNg = "SFX_AC_COMMAND_NG1";
#endif
    do {
        cont = 0;
        pressed = 0;
        switch (ac->mAcState) {
        case 0: {
            s32 i;
            ac->result = 1;
            memset(disp, 0, 0x2C);
            disp->iconX = -300.f;
            disp->iconY = 0.0f;
            disp->counter = 0x14;
            bd->hits = 0;
            bd->misses = 0;
            bd->frame = 0;
            bd->curSignal = 0;
            bd->signalCount = 0;
            bd->difficulty = ((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work);
            bd->inWindow = 0;
            ac->outputParams[1] = 0;
            for (k = 0; k < 5; k++) {
                bd->states[k] = 0;
            }
            switch (ac->acParams[4]) {
            case 1:
            case 2:
                for (i = 0; i < ac->acParams[0];) {
                    s32 found = 0;
                    u32 pick;
                    while (!found) {
                        s32 j;
                        s32 unique;
                        if (ac->acParams[4] == 1) {
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
                bd->expected = bd->buttons[0];
                break;
            default:
                bd->expected = 0x100;
                for (i = 0; i < ac->acParams[0]; i++) {
                    bd->buttons[i] = 0x100;
                }
                bd->expected = bd->buttons[0];
                break;
            }
            ac->mAcState = 0x63;
            break;
        }
        case 0x63:
            break;
        case 0x64:
            if (ac->guardFrames > 0) {
                (ac->guardFrames)--;
                ac->result = 2;
                cont = 1;
                ac->mAcState = 0x3ea;
                break;
            }
            bd->windowStart = bd->curSignal * ac->acParams[2];
            bd->windowEnd = (bd->curSignal + 1) * ac->acParams[2] - 1;
            bd->okStart = bd->windowStart;
            bd->okEnd = bd->okStart + _get_ok_frame_range(bd->difficulty) - 1;
            ac->mAcState = 0x3e8;
            // fallthrough
        case 0x3e8:
            bd->signalEnd = (bd->signalCount + 1) * ac->acParams[2] - 1;
            bd->signalCount = bd->signalCount + 1;
            if (bd->signalCount < ac->acParams[6] + 1) {
                if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                    psndSFXOn(sfxPi1);
                }
            } else {
                if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                    psndSFXOn(sfxPone1);
                }
            }
            bd->latched = 0;
            ac->mAcState = 0x3e9;
            // fallthrough
        case 0x3e9: {
            s32 inWindow;
            bd->frame = bd->frame + 1;
            if (bd->latched != 0) {
                bd->inWindow = 0;
            } else if (bd->curSignal >= ac->acParams[6]) {
                if (bd->frame >= bd->okStart && bd->frame <= bd->okEnd) {
                    bd->inWindow = 1;
                } else {
                    bd->inWindow = 0;
                }
            } else {
                bd->inWindow = 0;
            }
            inWindow = 0;
            if (BattlePadCheckTrigger(bd->expected) != 0) {
                pressed = 1;
            }
            if (pressed != 0) {
                switch (ac->acParams[4]) {
                case 2:
                    if (bd->expected == BattlePadGetTrigger()) {
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
            if (autoCommand != 0 && bd->inWindow != 0) {
                pressed = 1;
                inWindow = 1;
            }
            if (bd->hits + bd->misses >= ac->acParams[0]) {
                pressed = 0;
            }
            if (bd->curSignal >= ac->acParams[6] && pressed != 0 && bd->latched == 0) {
                bd->latched = 1;
                if (bd->frame >= bd->okStart && bd->frame <= bd->okEnd && inWindow != 0) {
                    s32 idx;
                    bd->hits = bd->hits + 1;
                    ac->outputParams[1] = 1;
                    idx = bd->curSignal;
                    if (idx >= ac->acParams[6] &&
                        idx < ac->acParams[0] + ac->acParams[6]) {
                        bd->states[idx - ac->acParams[6]] = 1;
                        if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                            psndSFXOn(sfxOk);
                        }
                    }
                    if (ac->acFlag & 0x10) {
                        ac->outputParams[0] = bd->hits;
                        ac->result &= ~1;
                        if (bd->hits >= ac->acParams[3]) {
                            ac->result |= 2;
                        }
                        cont = 1;
                        ac->mAcState = 0x3ea;
                    }
                } else {
                    if (bd->curSignal >= ac->acParams[6] &&
                        bd->curSignal < ac->acParams[0] + ac->acParams[6]) {
                        bd->misses = bd->misses + 1;
                        if (bd->frame >= bd->windowStart) {
                            bd->states[bd->curSignal - ac->acParams[6]] = -1;
                            if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                                psndSFXOn(sfxNg);
                            }
                            ac->outputParams[1] = -1;
                        } else {
                            bd->latched = 0;
                        }
                    }
                }
            }
            if (bd->latched == 0 && bd->frame > bd->okEnd) {
                bd->latched = 1;
                ac->outputParams[1] = -1;
                if (bd->curSignal >= ac->acParams[6] &&
                    bd->curSignal < ac->acParams[0] + ac->acParams[6]) {
                    bd->misses = bd->misses + 1;
                    bd->states[bd->curSignal - ac->acParams[6]] = -1;
                    if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                        psndSFXOn(sfxNg);
                    }
                }
            }
            if (bd->latched != 0) {
                if (bd->curSignal < ac->acParams[0] + ac->acParams[6]) {
                    bd->curSignal = bd->curSignal + 1;
                    if (bd->curSignal < ac->acParams[0] + ac->acParams[6]) {
                        bd->windowStart = bd->curSignal * ac->acParams[2] - 0xa;
                        bd->windowEnd = (bd->curSignal + 1) * ac->acParams[2] - 1;
                        bd->okStart = bd->curSignal * ac->acParams[2];
                        bd->okEnd = bd->okStart + _get_ok_frame_range(bd->difficulty) - 1;
                        bd->latched = 0;
                        if (bd->curSignal >= ac->acParams[6] &&
                            bd->curSignal < ac->acParams[0] + ac->acParams[6]) {
                            bd->expected = bd->buttons[bd->curSignal - ac->acParams[6]];
                        }
                    }
                }
            }
            if (bd->frame >= bd->signalCount * ac->acParams[2]) {
                if (bd->signalCount < ac->acParams[0] + ac->acParams[6]) {
                    ac->mAcState = 0x3e8;
                } else {
                    ac->outputParams[0] = bd->hits;
                    ac->result &= ~1;
                    if (bd->hits >= ac->acParams[3]) {
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
            bd->inWindow = 0;
            bd->endTimer = 0x3c;
            ac->mAcState = 0x3ed;
            break;
        case 0x3ed:
            bd->endTimer = bd->endTimer - 1;
            if (bd->endTimer <= 0) {
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
#ifdef MUST_MATCH
#pragma opt_dead_assignments reset
#endif
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
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.f, 30.f);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E8:
    case 0x3E9:
    case 0x3EA:
    case 0x3EB:
        actionCommandDisp(30.f, 0.0f);
        break;
    case 0x3EC:
    case 0x3ED:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.f, -300.f);
        } else {
            disp->iconX = 30.f;
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

#ifdef MUST_MATCH
// Match-only: passing the icon id through a u16 slot (instead of s32) makes MWCC schedule the
// 0x92-loop's int->float conversion exactly as the original (lis/xoris/lwz hoisted, addi/clrlwi
// last). The stored value and the call ABI are unchanged.
#define ICON_DISP_B(v, f, id, s) (((void (*)(f32, Vec*, s16, u16))iconDispGx)((s), (v), (f), (id)))
#else
#define ICON_DISP_B(v, f, id, s) iconDispGx((s), (v), (f), (id))
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
//  - the int->float magic double has to stay the compiler's own anonymous pool entry, which this
//    all-literal form places at +0xa8 and actionCommandDisp loads as `lfd fN, 0xa8(r30)`;
//    defining the double as a source object does not make MWCC use it for `(f32)int` and shifts
//    the load off +0xa8.
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
    num = ac->acParams[0];
    bd = (AcSignalTimingWork*)&ac->field_0x2BC[0];
    count = bd->signalCount;

    if (!(ac->acFlag & 1)) {
        if (bd->inWindow != 0) {
            Vec t = vec3_802fedc8;
            t.x = -200.f + x;
            t.y = 70.f + y;
            iconDispGx(1.0f, &t, 0x10, 0x65);
        } else {
            Vec t = vec3_802fedd4;
            t.x = -200.f + x;
            t.y = 70.f + y;
            iconDispGx(1.0f, &t, 0x10, 0x64);
        }
    }

    {
        s32 limit;
        switch (num) {
        case 1:
            limit = ac->acParams[6] * 2 + 5;
            break;
        case 3:
            limit = ac->acParams[6] * 2 + 0xB;
            break;
        case 4:
            limit = ac->acParams[6] * 2 + 0xD;
            break;
        case 5:
            limit = ac->acParams[6] * 2 + 0xF;
            break;
        default:
            limit = ac->acParams[6] * 2 + 0xB;
            break;
        }
        {
            Vec t = vec3_802fede0;
            t.x = -288.0f + x;
            f31 = 25.0f + y;
            t.y = f31;
            iconDispGx(1.0f, &t, 0x10, 0x8F);
        }
        for (i = 1; i < limit; i++) {
#ifdef MUST_MATCH
            // Volatile copy: retail reloads the template Vec every iteration instead of hoisting it.
            Vec t = *(volatile Vec*)&vec3_802fedec;
#else
            Vec t = vec3_802fedec;
#endif
            t.x = x + (f32)(i * 0x10 - 0x120);
            t.y = f31;
            iconDispGx(1.0f, &t, 0x10, 0x8E);
        }
        {
            Vec t = (Vec){0.0f, 0.0f, 0.0f};
            t.x = x + (f32)(i * 0x10 - 0x120);
            t.y = f31;
            iconDispGx(1.0f, &t, 0x10, 0x90);
        }
    }

    if (!(ac->acFlag & 4)) {
        Mtx mtx2;
        Mtx mtx;
        Mtx scale;
        s32 number = bd->hits + ac->acParams[5];
        {
            s32 ioff;
            if (ac->acFlag & 1) {
                ioff = (num * 0x30 - 0x1a + (ac->acParams[6] * 32));
            } else {
                ioff = (num * 0x30 - 0x1a + (ac->acParams[6] * 32));
            }
            {
                f32 fneg = -200.f + x;
                PSMTXTrans(mtx, fneg + (f32)ioff, f31, 0.0f);
            }
        }
        if (ac->acFlag & 8) {
            PSMTXScale(scale, 1.5f, 1.5f, 1.5f);
        } else {
            PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
        }
        PSMTXConcat(mtx, scale, mtx2);
        iconNumberDispGx(mtx2, number, 0, *(GXColor*)&dat_804216d0);
    }

    for (i = 0; i < ac->acParams[6]; i++) {
        if (i < bd->signalCount) {
            if (i < ac->acParams[6]) {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                u16 id = (u16)(0x92 + i);
                t.y = f31;
                t.x = x + (f32)(i * 0x21 - 0x102);
                ICON_DISP_B(&t, 0x10, id, 1.0f);
            } else {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                t.y = f31;
                t.x = x + (f32)(i * 0x21 - 0x102);
                iconDispGx(1.0f, &t, 0x10, 0x94);
            }
        } else {
            Vec t = (Vec){0.0f, 0.0f, 0.0f};
            t.y = f31;
            t.x = x + (f32)(i * 0x21 - 0x102);
            iconDispGx(0.5f, &t, 0x10, 0x91);
        }
    }

    if (!(ac->acFlag & 1)) {
        for (i = 0; i < num; i++) {
            if (i + ac->acParams[6] < count) {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                t.x = x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa)));
                t.y = f31;
                iconDispGx(1.0f, &t, 0x10, 0x95);
            } else {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                t.x = x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa)));
                t.y = f31;
                iconDispGx(1.0f, &t, 0x10, 0x91);
            }
        }
    } else if (!(ac->acFlag & 2)) {
        f28 = 15.0f + y;
        for (i = 0; i < num; i++) {
            f32 scale = 0.8f;
#ifdef MUST_MATCH
            // The leading comma (a discarded second read of `i`) nudges the allocator to colour
            // t0 in r4 as the target; the natural form below colours it r7 and costs ~12 diffs.
            s32 t0 = (i, i + ac->acParams[6]);
#else
            s32 t0 = i + ac->acParams[6];
#endif
            if (bd->signalCount == t0 && bd->frame >= ac->acParams[2] * t0 - 5 &&
                bd->frame < ac->acParams[2] * t0) {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                t.y = f28;
                t.x = x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa)));
                iconDispGx(0.8f, &t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 0));
            } else {
                s32 delta;
                Vec t;
                if (bd->signalCount <= t0) {
                    delta = -2;
                    scale = 0.6f;
                } else {
                    delta = -0xa;
                }
                t = (Vec){0.0f, 0.0f, 0.0f};
                t.x = x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa)));
                t.y = (f32)(delta + 0x19) + y;
                iconDispGx(scale, &t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 1));
            }
        }
    } else {
        f28 = 20.800001f + f31;
        for (i = 0; i < num; i++) {
#ifdef MUST_MATCH
            s32 t0 = (i, i + ac->acParams[6]); // see the note on the first t0 loop
#else
            s32 t0 = i + ac->acParams[6];
#endif
            if (bd->signalCount == t0 && bd->frame >= ac->acParams[2] * t0 - 0x14 &&
                bd->frame <= ac->acParams[2] * t0) {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                t.x = x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa)));
                t.y = f31;
                iconDispGx(0.8f, &t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 0));
            } else if (bd->signalCount <= t0) {
                if (i >= num - 1) {
                    // Match-only: the original sets r5=0 before every btlDispTexPlane call (the
                    // callee's GXColor is the 2nd GPR); the extra trailing s32 reproduces it, as in
                    // battle_seq_end.c. The natural 7-argument call is under #else.
#ifdef MUST_MATCH
                    ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
                        x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa))),
                        f28, 0.0f, 0.8f, 0.8f,
                        0x40, *(GXColor*)&dat_804216d4, 0);
#else
                    btlDispTexPlane(0x40, *(GXColor*)&dat_804216d4,
                                    x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa))),
                                    f28, 0.0f, 0.8f, 0.8f);
#endif
                }
            } else {
                Vec t = (Vec){0.0f, 0.0f, 0.0f};
                t.x = x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa)));
                t.y = f31;
                iconDispGx(0.8f, &t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 1));
            }
        }
    }

    f28 = f31 - 25.0f;
    for (i = 0; i < num; i++) {
        if (bd->states[i] == 1) {
#ifdef MUST_MATCH
            ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
                x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa))), f28,
                0.0f, 1.0f, 1.0f,
                0x3E, *(GXColor*)&dat_804216d8, 0);
#else
            btlDispTexPlane(0x3E, *(GXColor*)&dat_804216d8,
                            x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa))), f28,
                            0.0f, 1.0f, 1.0f);
#endif
        } else if (bd->states[i] == -1) {
#ifdef MUST_MATCH
            ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
                x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa))), f28,
                0.0f, 1.0f, 1.0f,
                0x3F, *(GXColor*)&dat_804216dc, 0);
#else
            btlDispTexPlane(0x3F, *(GXColor*)&dat_804216dc,
                            x + (f32)((i * 0x30 + (ac->acParams[6] * 0x21 - 0xfa))), f28,
                            0.0f, 1.0f, 1.0f);
#endif
        }
    }
}
#pragma optimize_for_size reset

