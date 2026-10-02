#include "battle/ac/ac_gauge_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
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
const f32 float_100_80421874 = 100.0f;
const f32 float_1_80421878 = 1.0f;
const f32 float_neg1_8042187c = -1.0f;
const f32 float_16_80421880 = 16.0f;
const f32 float_176_80421884 = 176.0f;
const f32 float_neg288_80421888 = -288.0f;
const f32 float_18_8042188c = 18.0f;
const f32 float_0_80421890 = 0.0f;
const f32 float_neg200_80421894 = -200.0f;
const f32 float_88_80421898 = 88.0f;
const f32 float_173_8042189c = 173.0f;
const f32 float_49_804218a0 = 49.0f;
const f32 float_10_804218a4 = 10.0f;
const f32 float_20_804218a8 = 20.0f;
const f32 float_0p8_804218ac = 0.8f;
const f32 float_neg228_804218b0 = -228.0f;
const f32 float_55_804218b4 = 55.0f;
const f32 float_56_804218b8 = 56.0f;
const f32 float_28_804218bc = 28.0f;
const f32 float_neg300_804218c0 = -300.0f;
const f32 float_30_804218c4 = 30.0f;
const f32 float_50_804218c8 = 50.0f;
const f32 float_90_804218cc = 90.0f;
const f32 float_180_804218d0 = 180.0f;
const f32 float_270_804218d4 = 270.0f;

// .rodata: 17 consecutive zero Vecs, then the two conversion doubles.
const Vec vec3_802ff860[17] = {
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};
const f64 double_to_int_802ff930 = 4503601774854144.0;
const f64 double_12_802ff938 = 12.0;

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
// The two `field_34[0] & 2` tests emit a single `beq skip` where the original emits a redundant
// `bne` to the next instruction before `beq skip` (the pattern occurs nowhere else in the tree and
// does not reproduce from any source shape tried); the default-case conversion
// field_34[0x14]/100*field_0x08+50 loads field_34[0x14] into r0 instead of r4, which shifts the
// A-conversion / B-load / hundred-load schedule (fdivs/fsubs/fmadds themselves already match).
// NONMATCHING (99.01%): two `field_34[0] & 2` tests miss a redundant `bne` before `beq skip`;
// default-case int->float puts field_34[0x14] in r0 instead of r4.
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
            neg = MATCH_F32(float_neg300_804218c0);
            zero = MATCH_F32(float_0_80421890);
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
            dec->field_0x04 = MATCH_F32(float_0_80421890);
            *(s32*)&work->actionCommands.field_34[0x14] &= ~1;
            break;
        default:
            dec->field_0x04 = MATCH_F32(float_50_804218c8);
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
                dec->field_0x04 = MATCH_F32(float_0_80421890);
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
                    intplGetValue(5, (s32)angle, 0x5A, MATCH_F32(float_0_80421890), MATCH_F32(float_100_80421874));
            } else if (angle <= float_180_804218d0) {
                dec->field_0x04 = intplGetValue(2, (s32)(angle - float_90_804218cc), 0x5A, MATCH_F32(float_100_80421874),
                                                MATCH_F32(float_0_80421890));
            } else if (angle <= float_270_804218d4) {
                dec->field_0x04 = intplGetValue(5, (s32)(angle - float_180_804218d0), 0x5A, MATCH_F32(float_0_80421890),
                                                MATCH_F32(float_100_80421874));
            } else {
                dec->field_0x04 = intplGetValue(2, (s32)(angle - float_270_804218d4), 0x5A, MATCH_F32(float_100_80421874),
                                                MATCH_F32(float_0_80421890));
            }
            if (dec->field_0x1C > 0) {
                dec->field_0x1C--;
                if (dec->field_0x1C > 0) {
                    dec->field_0x04 = MATCH_F32(float_0_80421890);
                    dec->field_0x10 = 0;
                    goto tail;
                }
            } else if ((*(s32*)&work->actionCommands.field_34[0x00] & 2) &&
                       ((s32)dec->field_0x04 <= 0)) {
                dec->field_0x20++;
                dec->field_0x04 = MATCH_F32(float_0_80421890);
                dec->field_0x10 = 0;
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    dec->field_0x1C = 0xF;
                } else {
                    dec->field_0x1C = 0x1E;
                }
                goto tail;
            }
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
                    dec->field_0x04 = MATCH_F32(float_0_80421890);
                    dec->field_0x10 = 0;
                    goto tail;
                }
            }
            {
                s32 a = *(s32*)&work->actionCommands.field_34[0x14];
                f32 f = (f32)a / MATCH_F32(float_100_80421874);
                dec->field_0x04 = f * (f32)dec->field_0x08 + MATCH_F32(float_50_804218c8);
            }
            if (*(s32*)&work->actionCommands.field_34[0x00] & 1) {
                s32 v = (s32)dec->field_0x04;
                if ((*(s32*)&work->actionCommands.field_34[0x00] & 2) && (v / 100 > dec->field_0x20)) {
                    dec->field_0x20 = v / 100;
                    dec->field_0x04 = MATCH_F32(float_0_80421890);
                    dec->field_0x10 = 0;
                    if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                        dec->field_0x1C = 0xF;
                    } else {
                        dec->field_0x1C = 0x1E;
                    }
                    goto tail;
                }
                v %= 100;
                dec->field_0x04 = (f32)v;
            }
            if (__fabsf(dec->field_0x04 - MATCH_F32(float_50_804218c8)) <= double_12_802ff938) {
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
#define ICON_DISP(v, f, id, s) (((void (*)(Vec*, s16, s32, f32))iconDispGx)((v), (f), (id), (s)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((v), (f), (id), (s))
#endif

// optimize_for_size: the original saves r25-r31 with `stmw` and keeps the 2D camera in r27.
#pragma optimize_for_size on
// Remaining diffs (99.32%): MWCC leaves the int->float pool double (the anonymous @NNN that
// backs `(f32)int`) on its own `lis` base unless the function references enough distinct doubles
// for it to build a shared .rodata pool.  Two MUST_MATCH seed reads of the existing conversion
// doubles are enough to make it pool, so the `lfd`s become `lfd fN, 0xe0(r28)` instead of
// `lis @NNN` (the target has `lfd fN, 0xd0(r30)`).  The remaining magic diffs are just that:
// the pool base is r28 = `...rodata.0` rather than the vec3 pointer r30, and the anonymous magic
// sits at +0xe0 (after the two named doubles) rather than the target's +0xd0, which would require
// the named `double_to_int_802ff930` / `double_12_802ff938` to be compiler pool entries instead of
// globals (out of scope, they are shared with battleAcMain_GaugeTiming).  An unused `f64 temp =
// <magic>` local makes MWCC drop the pool base and use `vec3_802ff860` (the target's prologue) but
// then it does not fold the magic (98.71%), so the seed form is kept.
// The gauge loop's `176 * (100/(count+1) * (i+1)) / 100` is split into `f32 t`/`u` steps that
// straddle the icon test, which reproduces the target's `fdivs; fmuls; beq; fmuls; fdivs`
// schedule; separating `u` from `t` keeps the intermediate in the target's f0; naming
// `bx = float_neg288 + x` forces `fadds f0, bx, u` (the target's operand order).
// The ELSE rasta's two cross-call values live in `f32 q[2]` (not two scalars): as array elements
// MWCC colours them f27/f28, exactly the target's registers (`fadds f28, f0, f31` / `fadds f27,
// f2, f1`); with four scalars it used f25/f26 (the loop's v.x/v.y).
// NONMATCHING (99.32%): the top `if (field_34[0x0C] == 1)` is a switch (case 1 / default) that
// reproduces the target's `cmpwi; beq then; b else`; only the int->float double pool base/offset
// differ (see above).
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    const Vec* vec = vec3_802ff860;
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

#ifdef MUST_MATCH
    (void)*(volatile f64*)&double_to_int_802ff930;
    (void)*(volatile f64*)&double_12_802ff938;
#endif

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
            Vec v = vec[0];
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
                // volatile: stops MWCC hoisting the vec[1] copy out of the loop (the target reloads it each pass).
                v = *(volatile Vec*)&vec[1];
#else
                v = vec[1];
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
                Vec v = vec[2];
                v.x = baseX;
                v.y = baseY;
                ICON_DISP(&v, 0x10, icon1, float_1_80421878);
            }
            {
                Vec v = vec[3];
                v.x = float_56_804218b8 + baseX;
                v.y = baseY;
                ICON_DISP(&v, 0x10, icon3, float_1_80421878);
            }
        } else {
            Vec v = vec[4];
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
                        Vec v = vec[5];
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                    }
                    {
                        Vec v = vec[6];
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon2, float_1_80421878);
                    }
                } else {
                    Vec v = vec[7];
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                }
            } else {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = vec[8];
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                    }
                    {
                        Vec v = vec[9];
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon3, float_1_80421878);
                    }
                } else {
                    Vec v = vec[10];
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
                        Vec v = vec[11];
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                    }
                    {
                        Vec v = vec[12];
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon3, float_1_80421878);
                    }
                } else {
                    Vec v = vec[13];
                    v.x = float_28_804218bc + (float_neg228_804218b0 + x);
                    v.y = float_55_804218b4 + y;
                    ICON_DISP(&v, 0x10, icon1, float_1_80421878);
                }
            } else {
                if ((icon2 & 0xffff) != 0) {
                    baseX = float_neg228_804218b0 + x;
                    baseY = float_55_804218b4 + y;
                    {
                        Vec v = vec[14];
                        v.x = baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon0, float_1_80421878);
                    }
                    {
                        Vec v = vec[15];
                        v.x = float_56_804218b8 + baseX;
                        v.y = baseY;
                        ICON_DISP(&v, 0x10, icon2, float_1_80421878);
                    }
                } else {
                    Vec v = vec[16];
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
