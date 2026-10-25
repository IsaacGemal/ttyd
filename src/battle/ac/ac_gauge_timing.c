#include "battle/ac/ac_gauge_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "math_ext.h"
#include "matching.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// sdata2 constants of this unit.
#ifdef MUST_MATCH
__declspec(section ".sdata2") s32 dat_80421870 = -1;
#else
const s32 dat_80421870 = -1;
#endif
// All-literal pool (AGENT_BRIEF "Pool recipes"): the target's .sdata2 is MWCC's literal pool in
// first-use order over the .text function order. Defining these as named objects emits the objects
// and pools a duplicate at every folded read (200 bytes vs the target's 104). Mapping the names to
// literals and letting the compiler pool them reproduces the target's 25 entries exactly.
#define float_100_80421874 100.0f
#define float_1_80421878 1.0f
#define float_neg1_8042187c -1.0f
#define float_16_80421880 16.0f
#define float_176_80421884 176.0f
#define float_neg288_80421888 -288.0f
#define float_18_8042188c 18.0f
#define float_0_80421890 0.0f
#define float_neg200_80421894 -200.0f
#define float_88_80421898 88.0f
#define float_173_8042189c 173.0f
#define float_49_804218a0 49.0f
#define float_10_804218a4 10.0f
#define float_20_804218a8 20.0f
#define float_0p8_804218ac 0.8f
#define float_neg228_804218b0 -228.0f
#define float_55_804218b4 55.0f
#define float_56_804218b8 56.0f
#define float_28_804218bc 28.0f
#define float_neg300_804218c0 -300.0f
#define float_30_804218c4 30.0f
#define float_50_804218c8 50.0f
#define float_90_804218cc 90.0f
#define float_180_804218d0 180.0f
#define float_270_804218d4 270.0f

// GaugeTiming work area at BattleWork+0x1F4C (inside BattleActions.acWork). No header declares it.
typedef struct GaugeTimingWork {
    s32 frame;      // 0x00, frames since the state started
    f32 gauge;      // 0x04, needle position 0..100 (battleAcDisp reads it / 100 as the value)
    s32 sweep;      // 0x08, frames the needle has moved; scales the angle in the sweep modes
    s32 timeLimit;  // 0x0C, frames allowed before the sweep fails (copied from acParams[1])
    s32 inZone;     // 0x10, needle is inside the target; drives the icon and the result
    s8 tokenHit;    // 0x14, outcome rolled up front when the unit has the token badge
    s8 phase;       // 0x15, 0 waiting for the button, 1 button down, 2 input done
    s8 tapMode;     // 0x16, 1: a press stops the needle (state 0x3E9); 0: hold the button, release stops it (0x3E8 until held)
    u8 pad_0x17;    // 0x17
    s32 buttonMask; // 0x18, BattlePad mask for the button (acParams[0] picks it)
    s32 flashTimer; // 0x1C, frames the stopped needle flashes before the sweep resumes
    s32 laps;       // 0x20, whole hundreds passed by the needle (the 100-step modes)
    s32 sfxId;      // 0x24, looping gauge sound handle, -1 when none
    s32 endTimer;   // 0x28, frames before the action command ends
} GaugeTimingWork;

// Local to this file in the original (`.fn actionCommandDisp, local`).
static void actionCommandDisp(f32 x, f32 y);

// The default-case acParams[4]/100*field_0x08+50 conversion matches r4 (not r0) once the /100
// constant is read plainly instead of through MATCH_F32's volatile (float_50 stays volatile).
s32 battleAcMain_GaugeTiming(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    GaugeTimingWork* dec = (GaugeTimingWork*)&ac->acWork[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 autoFlag = 0;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoFlag = 1;
    }

    switch (work->actionCommands.mAcState) {
    case 0x63:
        break;
    case 0:
        memset(disp, 0, 0x2C);
        {
            f32 zero;
            f32 neg;
            neg = -300.0f;
            zero = 0.0f;
            disp->iconX = neg;
            disp->iconY = zero;
            disp->counter = 0x14;
            disp->value = zero;
        }
        disp->gaugeColor.rgba = dat_80421870;
        dec->frame = 0;
        dec->sweep = 0;
        work->actionCommands.outputParams[0] = 0;
        work->actionCommands.mAcState = 0x63;
        work->actionCommands.result = 1;
        dec->timeLimit = work->actionCommands.acParams[1];
        dec->inZone = 0;
        dec->flashTimer = 0;
        dec->laps = 0;
        dec->sfxId = -1;
        switch (work->actionCommands.acParams[0]) {
        case 0:
            dec->tapMode = 1;
            dec->buttonMask = 0x20;
            dec->phase = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 2:
            dec->tapMode = 0;
            dec->buttonMask = 0x40000;
            dec->phase = 0;
            break;
        default:
            dec->tapMode = 1;
            dec->buttonMask = 0x100;
            dec->phase = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        }
        switch (work->actionCommands.acParams[2]) {
        case 1:
            dec->gauge = 0.0f;
            work->actionCommands.acParams[4] &= ~1;
            break;
        default:
            dec->gauge = 50.0f;
            break;
        }
        break;
    case 0x64:
        if ((work->actionCommands.autoFlags & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
            if (irand(100) >= 0) {
                work->actionCommands.autoTimer = 0x1E;
                dec->tokenHit = 0;
            } else {
                dec->tokenHit = 1;
                work->actionCommands.autoTimer = 0x1E;
            }
            work->actionCommands.mAcState = 0x3EA;
            break;
        }
        if (dec->tapMode != 0) {
            work->actionCommands.mAcState = 0x3E9;
        } else {
            work->actionCommands.mAcState = 0x3E8;
        }
        // fallthrough
    case 0x3E8:
    case 0x3E9:
        dec->frame++;
        switch (dec->phase) {
        case 0:
            if (dec->tapMode == 0) {
                if (BattlePadCheckNow(dec->buttonMask) != 0) {
                    dec->phase = 1;
                    work->actionCommands.mAcState = 0x3E9;
                    dec->frame = 0;
                    work->actionCommands.outputParams[0] = 1;
                }
            }
            break;
        case 1:
            if (dec->tapMode != 0) {
                if (BattlePadGetNow() == (u32)dec->buttonMask) {
                    if (BattlePadCheckTrigger(dec->buttonMask) != 0) {
                        dec->phase = 2;
                    }
                }
            } else {
                if (BattlePadCheckNow(dec->buttonMask) == 0) {
                    if (BattlePadCheckUp(dec->buttonMask) != 0) {
                        dec->phase = 2;
                    }
                }
            }
            break;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            if (dec->frame > 0x78) {
                work->actionCommands.result = 0;
                dec->gauge = 0.0f;
                work->actionCommands.mAcState = 0x3EB;
                work->actionCommands.outputParams[0] = 2;
                break;
            }
        } else if (dec->frame > dec->timeLimit) {
            work->actionCommands.result = 0;
            work->actionCommands.mAcState = 0x3EB;
            break;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            break;
        }
        if (dec->flashTimer <= 0) {
            dec->sweep++;
        }
        switch (work->actionCommands.acParams[2]) {
        case 1: {
            f32 angle = reviseAngle(
                (f32)(dec->sweep * 0x168 / work->actionCommands.acParams[4]));
            if (angle <= float_90_804218cc) {
                dec->gauge =
                    intplGetValue(5, (s32)angle, 0x5A, 0.0f, 100.0f);
            } else if (angle <= float_180_804218d0) {
                dec->gauge = intplGetValue(2, (s32)(angle - float_90_804218cc), 0x5A, 100.0f,
                                                0.0f);
            } else if (angle <= float_270_804218d4) {
                dec->gauge = intplGetValue(5, (s32)(angle - float_180_804218d0), 0x5A, 0.0f,
                                                100.0f);
            } else {
                dec->gauge = intplGetValue(2, (s32)(angle - float_270_804218d4), 0x5A, 100.0f,
                                                0.0f);
            }
            if (dec->flashTimer > 0) {
                dec->flashTimer--;
                if (dec->flashTimer > 0) {
                    dec->gauge = 0.0f;
                    dec->inZone = 0;
                    break;
                }
#ifdef MUST_MATCH
            // The natural `A && B` (the #else form) emits a single `beq skip`. The original tests
            // `acFlag & 2` twice, so MWCC emits the first as a `bne` past the second and the second
            // as the `beq` skip.
            } else if (((work->actionCommands.acFlag & 2) || (work->actionCommands.acFlag & 2)) &&
                       ((s32)dec->gauge <= 0)) {
#else
            } else if ((work->actionCommands.acFlag & 2) &&
                       ((s32)dec->gauge <= 0)) {
#endif
                dec->laps++;
                dec->gauge = 0.0f;
                dec->inZone = 0;
                if (work->actionCommands.acFlag & 2) {
                    dec->flashTimer = 0xF;
                } else {
                    dec->flashTimer = 0x1E;
                }
                break;
            }
            if (dec->gauge >= (f32)work->actionCommands.acParams[3]) {
                dec->inZone = 1;
            } else {
                dec->inZone = 0;
            }
            break;
        }
        default: {
            if (dec->flashTimer > 0) {
                dec->flashTimer--;
                if (dec->flashTimer > 0) {
                    dec->gauge = 0.0f;
                    dec->inZone = 0;
                    break;
                }
            }
            {
                s32 a = work->actionCommands.acParams[4];
                f32 f = (f32)a / float_100_80421874;
                dec->gauge = f * (f32)dec->sweep + 50.0f;
            }
            if (work->actionCommands.acFlag & 1) {
                s32 v = (s32)dec->gauge;
#ifdef MUST_MATCH
                // The same doubled `acFlag & 2` test as above.
                if (((work->actionCommands.acFlag & 2) || (work->actionCommands.acFlag & 2)) &&
                    (v / 100 > dec->laps)) {
#else
                if ((work->actionCommands.acFlag & 2) && (v / 100 > dec->laps)) {
#endif
                    dec->laps = v / 100;
                    dec->gauge = 0.0f;
                    dec->inZone = 0;
                    if (work->actionCommands.acFlag & 2) {
                        dec->flashTimer = 0xF;
                    } else {
                        dec->flashTimer = 0x1E;
                    }
                    break;
                }
                v %= 100;
                dec->gauge = (f32)v;
            }
            if (__fabsf(dec->gauge - 50.0f) <= 12.0) {
                dec->inZone = 1;
            } else {
                dec->inZone = 0;
            }
            break;
        }
        }
        if (autoFlag != 0 && dec->inZone != 0) {
            dec->phase = 2;
        }
        if (dec->phase == 2) {
            if (dec->inZone != 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            } else {
                work->actionCommands.result = 0;
                work->actionCommands.result &= ~2;
            }
            work->actionCommands.mAcState = 0x3EB;
        }
        break;
    case 0x3EA:
        work->actionCommands.autoTimer--;
        if (work->actionCommands.autoTimer < 0) {
            if (dec->tokenHit != 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            } else {
                work->actionCommands.result = 0;
            }
            work->actionCommands.endFlags |= 1;
            return 0;
        }
        break;
    case 0x3EB:
        work->actionCommands.mAcState = 0x3EC;
        // fallthrough
    case 0x3EC:
        work->actionCommands.endFlags |= 1;
        dec->endTimer = 0x3C;
        work->actionCommands.mAcState = 0x3ED;
        if ((u32)dec->sfxId != 0xFFFFFFFF) {
            psndSFXOff(dec->sfxId);
        }
        // fallthrough
    case 0x3ED:
        dec->endTimer--;
        if (dec->endTimer <= 0) {
            work->actionCommands.mAcState = 0x3EE;
        }
        break;
    case 0x3EE:
        work->actionCommands.maincb = NULL;
        work->actionCommands.dispcb = NULL;
        work->actionCommands.resultcb = NULL;
        work->actionCommands.deletecb = NULL;
        return 0;
    default:
        break;
    }
    return 1;
}
s32 battleAcResult_GaugeTiming(BattleWork* work) {
    return work->actionCommands.result;
}
void battleAcDisp_GaugeTiming(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_804218c0, float_30_804218c4);
        // falls through
    case 0x64:
    case 0x3E8:
    case 0x3E9:
    case 0x3EB:
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3EC:
    case 0x3ED:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_804218c4, float_neg300_804218c0);
        } else {
            disp->iconX = float_30_804218c4;
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
void battleAcDelete_GaugeTiming(BattleWork* work) {
    if (!(work->actionCommands.endFlags & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

// 100%. The int->float magic must be MWCC's own anonymous pool entry: do not define
// `double_to_int_802ff930` in the source, or the compiler emits a second anonymous copy after the
// named one and the loads move to `0xe0(r28)`. After the 17 zero Vec literal templates, MWCC anchors
// the pool entry to the first template and emits the target's `lfd fN, 0xd0(r30)` (with
// `double_12_802ff938` also left to the pool, which is why battleAcMain compares against the
// literal 12.0).
// The gauge loop's `176 * (100/(count+1) * (i+1)) / 100` is split into `f32 t`/`u` steps that
// straddle the icon test, which reproduces the target's `fdivs; fmuls; beq; fmuls; fdivs`
// schedule; separating `u` from `t` keeps the intermediate in the target's f0; naming
// `bx = float_neg288 + x` forces `fadds f0, bx, u` (the target's operand order).
// The ELSE rasta's two cross-call values live in `f32 q[2]` (not two scalars): as array elements
// MWCC colours them f27/f28, exactly the target's registers (`fadds f28, f0, f31` / `fadds f27,
// f2, f1`); with four scalars it used f25/f26 (the loop's v.x/v.y).
// The top `if (acParams[2] == 1)` is a switch (case 1 / default) that reproduces the target's
// `cmpwi; beq then; b else`.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    GaugeTimingWork* dec;
    CameraEntry* cam;
    f32 baseX;
    f32 baseY;
    f32 value;
    s32 i;
    s32 count;
    s32 icon0;
    s32 icon1;
    s32 icon2;
    s32 icon3;

    camGetPtr(CAMERA_2D);
    dec = (GaugeTimingWork*)&ac->acWork[0];

    value = dec->gauge / float_100_80421874;
    if (value > float_1_80421878) {
        value = float_1_80421878;
    }
    disp->value = value;

    switch (ac->acParams[2]) {
    case 1: {
        f32 ratio = disp->value * float_100_80421874 / (f32)ac->acParams[3];
        if (ratio > float_100_80421874) {
            ratio = float_100_80421874;
        }
        if (dec->flashTimer > 0) {
            BattleAcDrawGauge((s32)x, (s32)y, float_neg1_8042187c, 0xB2, 0x64, 0x64, 0x64, 0x64, 0);
        } else {
            BattleAcDrawGauge((s32)x, (s32)y, ratio, 0xB2, 0x64, 0x64, 0x64, 0x64, 0);
        }
        {
            s32 icon =
                (dec->gauge >= (f32)ac->acParams[3]) ? 0x95 : 0x91;
            Vec v = (Vec){0.0f, 0.0f, 0.0f};
            v.x = float_16_80421880 + (float_176_80421884 + (float_neg288_80421888 + x));
            v.y = float_18_8042188c + y;
            iconDispGx(float_1_80421878, &v, 0x10, icon);
        }
        break;
    }
    default: {
        BattleAcDrawGauge((s32)x, (s32)y, float_0_80421890, 0xB2, 0x64, 0x64, 0x64, 0x64, 0);
        if (dec->flashTimer <= 0) {
            f32 a;
            f32 px;
            f32 q[2];
            cam = camGetPtr(CAMERA_2D);
            a = dec->gauge * float_173_8042189c / float_100_80421874;
            px = (float_neg200_80421894 + x) - float_88_80421898;
            q[0] = px + a;
            q[1] = float_49_804218a0 + y;
            btlDispGXInit2DRasta();
            GXLoadPosMtxImm(cam->view, 0);
            btlDispGXQuads2DRasta(q[0], q[1], q[0] + float_10_804218a4, q[1] - float_16_80421880, 0, 0xFF, 0, 0xFF);
        }
        count = ac->acParams[3];
        for (i = 0; i < count; i++) {
            {
                s32 icon;
                Vec v;
                f32 u;
                f32 bx;
                f32 t = float_100_80421874 / (f32)(count + 1);
                u = t * (f32)(i + 1);
                icon = (dec->inZone != 0) ? 0x95 : 0x91;
                u = float_176_80421884 * u;
                u = u / float_100_80421874;
                v = (Vec){0.0f, 0.0f, 0.0f};
                bx = float_neg288_80421888 + x;
                v.x = bx + u;
                v.y = float_20_804218a8 + y;
                iconDispGx(float_0p8_804218ac, &v, 0x10, icon);
            }
        }
        break;
    }
    }

    switch (ac->acParams[0]) {
    case 0:
        // icon2/icon3 are set in every case (rather than once after the switch) because the target
        // materialises `li r28,0; li r27,0` in each arm; without it MWCC propagates the 0 and the
        // icon2 tests use a known-zero register instead.
        icon0 = BattleACGetButtonIcon(0x20, 1);
        icon1 = BattleACGetButtonIcon(0x20, 0);
        icon2 = 0;
        icon3 = 0;
        break;
    case 2:
        icon0 = BattleACGetButtonIcon(0x40000, 1);
        icon1 = BattleACGetButtonIcon(0x40000, 0);
        icon2 = 0;
        icon3 = 0;
        break;
    default:
        icon0 = BattleACGetButtonIcon(0x100, 1);
        icon1 = BattleACGetButtonIcon(0x100, 0);
        icon2 = 0;
        icon3 = 0;
        break;
    }

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3EB:
    case 0x3EC:
    case 0x3ED:
        if ((icon2 & 0xffff) != 0) {
            baseX = float_neg228_804218b0 + x;
            baseY = float_55_804218b4 + y;
            {
                Vec v = (Vec){0.0f, 0.0f, 0.0f};
                v.x = baseX;
                v.y = baseY;
                iconDispGx(float_1_80421878, &v, 0x10, icon1);
            }
            {
                Vec v = (Vec){0.0f, 0.0f, 0.0f};
                v.x = float_56_804218b8 + baseX;
                v.y = baseY;
                iconDispGx(float_1_80421878, &v, 0x10, icon3);
            }
        } else {
            Vec v = (Vec){0.0f, 0.0f, 0.0f};
            v.x = float_28_804218bc + (float_neg228_804218b0 + x);
            v.y = float_55_804218b4 + y;
            iconDispGx(float_1_80421878, &v, 0x10, icon1);
        }
        break;
    case 0x3E8:
    case 0x3E9:
        if (dec->tapMode != 0) {
            if (dec->inZone != 0) {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon0);
                    }
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon2);
                    }
                } else {
                    Vec v = (Vec){0.0f, 0.0f, 0.0f};
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    iconDispGx(float_1_80421878, &v, 0x10, icon0);
                }
            } else {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon1);
                    }
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon3);
                    }
                } else {
                    Vec v = (Vec){0.0f, 0.0f, 0.0f};
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    iconDispGx(float_1_80421878, &v, 0x10, icon1);
                }
            }
        } else {
            if (dec->inZone != 0) {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon1);
                    }
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon3);
                    }
                } else {
                    Vec v = (Vec){0.0f, 0.0f, 0.0f};
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    iconDispGx(float_1_80421878, &v, 0x10, icon1);
                }
            } else {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon0);
                    }
                    {
                        Vec v = (Vec){0.0f, 0.0f, 0.0f};
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        iconDispGx(float_1_80421878, &v, 0x10, icon2);
                    }
                } else {
                    Vec v = (Vec){0.0f, 0.0f, 0.0f};
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    iconDispGx(float_1_80421878, &v, 0x10, icon0);
                }
            }
        }
        break;
    default:
        break;
    }
}
