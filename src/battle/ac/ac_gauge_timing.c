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

// .rodata: 17 separate zero Vecs (not one array), then the compiler's conversion pool. As
// separate objects they make MWCC anchor the int->float pool entry to the first vec's base.
const Vec vec3_802ff860 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff86c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff878 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff884 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff890 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff89c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8a8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8b4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8c0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8cc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8d8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8e4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8f0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff8fc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff908 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff914 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff920 = {0.0f, 0.0f, 0.0f};

// MATCH_F64: read the constant through a volatile pointer so mwcc loads it from its own symbol.
#ifdef MUST_MATCH
#define MATCH_F64(x) (*(volatile const f64*)&(x))
#else
#define MATCH_F64(x) (x)
#endif


// GaugeTiming work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC). No header declares it.
typedef struct GaugeTimingWork {
    s32 frame;      // 0x00
    f32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    s32 field_0x0C; // 0x0C
    s32 field_0x10; // 0x10
    s8 field_0x14;  // 0x14
    s8 field_0x15;  // 0x15
    s8 field_0x16;  // 0x16
    u8 pad_0x17;    // 0x17
    s32 field_0x18; // 0x18
    s32 field_0x1C; // 0x1C
    s32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
} GaugeTimingWork;

// Local to this file in the original (`.fn actionCommandDisp, local`).
static void actionCommandDisp(f32 x, f32 y);

// optimize_for_size: the original saves r28-r31 with `stmw` and keeps `work` in r29.
#pragma optimize_for_size on
// The default-case field_34[0x14]/100*field_0x08+50 conversion matches r4 (not r0) once the /100
// constant is read plainly instead of through MATCH_F32's volatile (float_50 stays volatile).
s32 battleAcMain_GaugeTiming(BattleWork* work) {
#ifdef MUST_MATCH
    // MWCC folds two bases derived directly from `work` into one register; deriving both from
    // the `ac` intermediate keeps work=r29, dec=r31 and disp=r28 as the original does.
    BattleActions* ac = &work->actionCommands;
    GaugeTimingWork* dec = (GaugeTimingWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
#else
    GaugeTimingWork* dec = (GaugeTimingWork*)((u8*)work + 0x1F4C);
    BattleACDispParams* disp = &work->actionCommands.dispParams;
#endif
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
        disp->field_0xC = dat_80421870;
        dec->frame = 0;
        dec->field_0x08 = 0;
        work->actionCommands.outputParams[0] = 0;
        work->actionCommands.mAcState = 0x63;
        work->actionCommands.result = 1;
        dec->field_0x0C = *(s32*)&work->actionCommands.field_34[0x08];
        dec->field_0x10 = 0;
        dec->field_0x1C = 0;
        dec->field_0x20 = 0;
        dec->field_0x24 = -1;
        switch (*(s32*)&work->actionCommands.field_34[0x04]) {
        case 0:
            dec->field_0x16 = 1;
            dec->field_0x18 = 0x20;
            dec->field_0x15 = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 2:
            dec->field_0x16 = 0;
            dec->field_0x18 = 0x40000;
            dec->field_0x15 = 0;
            break;
        default:
            dec->field_0x16 = 1;
            dec->field_0x18 = 0x100;
            dec->field_0x15 = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        }
        switch (*(s32*)&work->actionCommands.field_34[0x0C]) {
        case 1:
            dec->field_0x04 = 0.0f;
            *(s32*)&work->actionCommands.field_34[0x14] &= ~1;
            break;
        default:
            dec->field_0x04 = 50.0f;
            break;
        }
        break;
    case 0x64:
        if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
            if ((s32)irand(100) >= 0) {
                work->actionCommands.field_0x8 = 0x1E;
                dec->field_0x14 = 0;
            } else {
                dec->field_0x14 = 1;
                work->actionCommands.field_0x8 = 0x1E;
            }
            work->actionCommands.mAcState = 0x3EA;
            break;
        }
        if (dec->field_0x16 != 0) {
            work->actionCommands.mAcState = 0x3E9;
        } else {
            work->actionCommands.mAcState = 0x3E8;
        }
        // fallthrough
    case 0x3E8:
    case 0x3E9:
        dec->frame++;
        switch (dec->field_0x15) {
        case 0:
            if (dec->field_0x16 == 0) {
                if ((s32)BattlePadCheckNow(dec->field_0x18) != 0) {
                    dec->field_0x15 = 1;
                    work->actionCommands.mAcState = 0x3E9;
                    dec->frame = 0;
                    work->actionCommands.outputParams[0] = 1;
                }
            }
            break;
        case 1:
            if (dec->field_0x16 != 0) {
                if ((u32)BattlePadGetNow() == (u32)dec->field_0x18) {
                    if (BattlePadCheckTrigger(dec->field_0x18) != 0) {
                        dec->field_0x15 = 2;
                    }
                }
            } else {
                if ((s32)BattlePadCheckNow(dec->field_0x18) == 0) {
                    if ((s32)BattlePadCheckUp(dec->field_0x18) != 0) {
                        dec->field_0x15 = 2;
                    }
                }
            }
            break;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            if (dec->frame > 0x78) {
                work->actionCommands.result = 0;
                dec->field_0x04 = 0.0f;
                work->actionCommands.mAcState = 0x3EB;
                work->actionCommands.outputParams[0] = 2;
                break;
            }
        } else if (dec->frame > dec->field_0x0C) {
            work->actionCommands.result = 0;
            work->actionCommands.mAcState = 0x3EB;
            break;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            break;
        }
        if (dec->field_0x1C <= 0) {
            dec->field_0x08++;
        }
        switch (*(s32*)&work->actionCommands.field_34[0x0C]) {
        case 1: {
            f32 angle = reviseAngle(
                (f32)(dec->field_0x08 * 0x168 / *(s32*)&work->actionCommands.field_34[0x14]));
            if (angle <= float_90_804218cc) {
                dec->field_0x04 =
                    intplGetValue(5, (s32)angle, 0x5A, 0.0f, 100.0f);
            } else if (angle <= float_180_804218d0) {
                dec->field_0x04 = intplGetValue(2, (s32)(angle - float_90_804218cc), 0x5A, 100.0f,
                                                0.0f);
            } else if (angle <= float_270_804218d4) {
                dec->field_0x04 = intplGetValue(5, (s32)(angle - float_180_804218d0), 0x5A, 0.0f,
                                                100.0f);
            } else {
                dec->field_0x04 = intplGetValue(2, (s32)(angle - float_270_804218d4), 0x5A, 100.0f,
                                                0.0f);
            }
            if (dec->field_0x1C > 0) {
                dec->field_0x1C--;
                if (dec->field_0x1C > 0) {
                    dec->field_0x04 = 0.0f;
                    dec->field_0x10 = 0;
                    goto tail;
                }
#ifdef MUST_MATCH
            } else {
                // The natural `A && B` (see the #else form) emits a single `beq skip`.  The original
                // tests `field_34[0] & 2` twice, positive then negative, so MWCC emits the first as a
                // `bne` to the fallthrough label and the second as the `beq` skip.
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    goto gaugeCheck1;
                }
                if (!(*(s32*)&work->actionCommands.field_34[0x00] & 2)) {
                    goto gaugeSkip1;
                }
            gaugeCheck1:
                if ((s32)dec->field_0x04 <= 0) {
                    dec->field_0x20++;
                    dec->field_0x04 = 0.0f;
                    dec->field_0x10 = 0;
                    if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                        dec->field_0x1C = 0xF;
                    } else {
                        dec->field_0x1C = 0x1E;
                    }
                    goto tail;
                }
            }
        gaugeSkip1:
#else
            } else if ((*(s32*)&work->actionCommands.field_34[0x00] & 2) &&
                       ((s32)dec->field_0x04 <= 0)) {
                dec->field_0x20++;
                dec->field_0x04 = 0.0f;
                dec->field_0x10 = 0;
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    dec->field_0x1C = 0xF;
                } else {
                    dec->field_0x1C = 0x1E;
                }
                goto tail;
            }
#endif
            if (dec->field_0x04 >= (f32)*(s32*)&work->actionCommands.field_34[0x10]) {
                dec->field_0x10 = 1;
            } else {
                dec->field_0x10 = 0;
            }
            break;
        }
        default: {
            if (dec->field_0x1C > 0) {
                dec->field_0x1C--;
                if (dec->field_0x1C > 0) {
                    dec->field_0x04 = 0.0f;
                    dec->field_0x10 = 0;
                    goto tail;
                }
            }
            {
                s32 a = *(s32*)&work->actionCommands.field_34[0x14];
                f32 f = (f32)a / float_100_80421874;
                dec->field_0x04 = f * (f32)dec->field_0x08 + 50.0f;
            }
            if (*(s32*)&work->actionCommands.field_34[0x00] & 1) {
                s32 v = (s32)dec->field_0x04;
#ifdef MUST_MATCH
                // Same double-test trick as gaugeCheck1 above.
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    goto gaugeCheck2;
                }
                if (!(*(s32*)&work->actionCommands.field_34[0x00] & 2)) {
                    goto gaugeMod2;
                }
            gaugeCheck2:
                if (v / 100 > dec->field_0x20) {
                    dec->field_0x20 = v / 100;
                    dec->field_0x04 = 0.0f;
                    dec->field_0x10 = 0;
                    if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                        dec->field_0x1C = 0xF;
                    } else {
                        dec->field_0x1C = 0x1E;
                    }
                    goto tail;
                }
            gaugeMod2:
#else
                if ((*(s32*)&work->actionCommands.field_34[0x00] & 2) && (v / 100 > dec->field_0x20)) {
                    dec->field_0x20 = v / 100;
                    dec->field_0x04 = 0.0f;
                    dec->field_0x10 = 0;
                    if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                        dec->field_0x1C = 0xF;
                    } else {
                        dec->field_0x1C = 0x1E;
                    }
                    goto tail;
                }
#endif
                v %= 100;
                dec->field_0x04 = (f32)v;
            }
            if (__fabsf(dec->field_0x04 - 50.0f) <= 12.0) {
                dec->field_0x10 = 1;
            } else {
                dec->field_0x10 = 0;
            }
            break;
        }
        }
    tail:
        if (autoFlag != 0 && dec->field_0x10 != 0) {
            dec->field_0x15 = 2;
        }
        if (dec->field_0x15 == 2) {
            if (dec->field_0x10 != 0) {
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
        work->actionCommands.field_0x8--;
        if (work->actionCommands.field_0x8 < 0) {
            if (dec->field_0x14 != 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            } else {
                work->actionCommands.result = 0;
            }
            work->actionCommands.field_0x30 |= 1;
            return 0;
        }
        break;
    case 0x3EB:
        work->actionCommands.mAcState = 0x3EC;
        // fallthrough
    case 0x3EC:
        work->actionCommands.field_0x30 |= 1;
        dec->field_0x28 = 0x3C;
        work->actionCommands.mAcState = 0x3ED;
        if ((u32)dec->field_0x24 != 0xFFFFFFFF) {
            psndSFXOff(dec->field_0x24);
        }
        // fallthrough
    case 0x3ED:
        dec->field_0x28--;
        if (dec->field_0x28 <= 0) {
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
#pragma optimize_for_size reset
s32 battleAcResult_GaugeTiming(BattleWork* work) {
    return work->actionCommands.result;
}
void battleAcDisp_GaugeTiming(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
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
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

// Match-only: iconDispGx's header declares the icon id `s16`, but the original passes the s32
// value returned by BattleACGetButtonIcon straight through (no `extsh`); a differently-typed
// function pointer keeps the call a direct `bl iconDispGx`. Same workaround as ac_lst_rc_keep.
#ifdef MUST_MATCH
#define ICON_DISP(v, f, id, s) (((void (*)(f32, Vec*, s16, s32))iconDispGx)((s), (v), (f), (id)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((s), (v), (f), (id))
#endif

// optimize_for_size: the original saves r25-r31 with `stmw` and keeps the 2D camera in r27.
#pragma optimize_for_size on
// 100%. The int->float magic must be MWCC's own anonymous pool entry: do not define
// `double_to_int_802ff930` in the source, or the compiler emits a second anonymous copy after the
// named one and the loads move to `0xe0(r28)`. The 17 zero Vecs must be separate objects (the
// original names each at a 0xc stride): that makes MWCC anchor the pool entry to the first vec and
// emit the target's `lfd fN, 0xd0(r30)` (with `double_12_802ff938` also left to the pool, which is
// why battleAcMain compares against the literal 12.0).
// The gauge loop's `176 * (100/(count+1) * (i+1)) / 100` is split into `f32 t`/`u` steps that
// straddle the icon test, which reproduces the target's `fdivs; fmuls; beq; fmuls; fdivs`
// schedule; separating `u` from `t` keeps the intermediate in the target's f0; naming
// `bx = float_neg288 + x` forces `fadds f0, bx, u` (the target's operand order).
// The ELSE rasta's two cross-call values live in `f32 q[2]` (not two scalars): as array elements
// MWCC colours them f27/f28, exactly the target's registers (`fadds f28, f0, f31` / `fadds f27,
// f2, f1`); with four scalars it used f25/f26 (the loop's v.x/v.y).
// The top `if (field_34[0x0C] == 1)` is a switch (case 1 / default) that reproduces the target's
// `cmpwi; beq then; b else`.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
#ifdef MUST_MATCH
    // Deriving disp before camGetPtr (and dec after it) keeps work=r31, vec=r30, disp=r27 and dec=r29,
    // as the original; the BattleActions intermediate keeps them from folding into the BattleWork base.
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    GaugeTimingWork* dec;
#else
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    GaugeTimingWork* dec;
#endif
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
#ifdef MUST_MATCH
    dec = (GaugeTimingWork*)&ac->field_0x2BC[0];
#else
    dec = (GaugeTimingWork*)((u8*)work + 0x1F4C);
#endif

    value = dec->field_0x04 / float_100_80421874;
    if (value > float_1_80421878) {
        value = float_1_80421878;
    }
    disp->value = value;

    switch (*(s32*)&ac->field_34[0x0C]) {
    case 1: {
        f32 ratio = disp->value * float_100_80421874 / (f32)*(s32*)&ac->field_34[0x10];
        if (ratio > float_100_80421874) {
            ratio = float_100_80421874;
        }
        if (dec->field_0x1C > 0) {
            BattleAcDrawGauge((s32)x, (s32)y, 0xB2, 0x64, 0x64, 0x64, 0x64, 0, float_neg1_8042187c);
        } else {
            BattleAcDrawGauge((s32)x, (s32)y, 0xB2, 0x64, 0x64, 0x64, 0x64, 0, ratio);
        }
        {
            s32 icon =
                (dec->field_0x04 >= (f32)*(s32*)&ac->field_34[0x10]) ? 0x95 : 0x91;
            Vec v = vec3_802ff860;
            v.x = float_16_80421880 + (float_176_80421884 + (float_neg288_80421888 + x));
            v.y = float_18_8042188c + y;
            ICON_DISP(&v, 0x10, icon, float_1_80421878);
        }
        break;
    }
    default: {
        BattleAcDrawGauge((s32)x, (s32)y, 0xB2, 0x64, 0x64, 0x64, 0x64, 0, float_0_80421890);
        if (dec->field_0x1C <= 0) {
            f32 a;
            f32 px;
            f32 q[2];
            cam = camGetPtr(CAMERA_2D);
            a = dec->field_0x04 * float_173_8042189c / float_100_80421874;
            px = (float_neg200_80421894 + x) - float_88_80421898;
            q[0] = px + a;
            q[1] = float_49_804218a0 + y;
            btlDispGXInit2DRasta();
            GXLoadPosMtxImm(cam->view, 0);
            btlDispGXQuads2DRasta(q[0], q[1], q[0] + float_10_804218a4, q[1] - float_16_80421880, 0, 0xFF, 0, 0xFF);
        }
        count = *(s32*)&ac->field_34[0x10];
        for (i = 0; i < count; i++) {
            {
                s32 icon;
                Vec v;
                f32 u;
                f32 bx;
                f32 t = float_100_80421874 / (f32)(count + 1);
                u = t * (f32)(i + 1);
                icon = (dec->field_0x10 != 0) ? 0x95 : 0x91;
                u = float_176_80421884 * u;
                u = u / float_100_80421874;
#ifdef MUST_MATCH
                // volatile: stops MWCC hoisting the vec3_802ff86c copy out of the loop (the target reloads it each pass).
                v = *(volatile Vec*)&vec3_802ff86c;
#else
                v = vec3_802ff86c;
#endif
                bx = float_neg288_80421888 + x;
                v.x = bx + u;
                v.y = float_20_804218a8 + y;
                ICON_DISP(&v, 0x10, icon, float_0p8_804218ac);
            }
        }
        break;
    }
    }

    switch (*(s32*)&ac->field_34[0x04]) {
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
                Vec v = vec3_802ff878;
                v.x = baseX;
                v.y = baseY;
                ICON_DISP(&v, 0x10, icon1, float_1_80421878);
            }
            {
                Vec v = vec3_802ff884;
                v.x = float_56_804218b8 + baseX;
                v.y = baseY;
                ICON_DISP(&v, 0x10, icon3, float_1_80421878);
            }
        } else {
            Vec v = vec3_802ff890;
            v.x = float_28_804218bc + (float_neg228_804218b0 + x);
            v.y = float_55_804218b4 + y;
            ICON_DISP(&v, 0x10, icon1, float_1_80421878);
        }
        break;
    case 0x3E8:
    case 0x3E9:
        if (dec->field_0x16 != 0) {
            if (dec->field_0x10 != 0) {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = vec3_802ff89c;
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                    }
                    {
                        Vec v = vec3_802ff8a8;
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon2, float_1_80421878);
                    }
                } else {
                    Vec v = vec3_802ff8b4;
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                }
            } else {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = vec3_802ff8c0;
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                    }
                    {
                        Vec v = vec3_802ff8cc;
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon3, float_1_80421878);
                    }
                } else {
                    Vec v = vec3_802ff8d8;
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                }
            }
        } else {
            if (dec->field_0x10 != 0) {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = vec3_802ff8e4;
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                    }
                    {
                        Vec v = vec3_802ff8f0;
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon3, float_1_80421878);
                    }
                } else {
                    Vec v = vec3_802ff8fc;
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                }
            } else {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = vec3_802ff908;
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                    }
                    {
                        Vec v = vec3_802ff914;
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon2, float_1_80421878);
                    }
                } else {
                    Vec v = vec3_802ff920;
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                }
            }
        }
        break;
    default:
        break;
    }
}
#pragma optimize_for_size reset
