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

// sdata2 constants of this unit (offsets listed in REPORT.md).
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
// NONMATCHING (94.38%): newly written; remaining diffs are constant-pool labels and branch polarity, see REPORT.md.
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
        disp->iconX = float_neg300_804218c0;
        disp->iconY = float_0_80421890;
        disp->counter = 0x14;
        disp->value = float_0_80421890;
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
        if (*(s32*)&work->actionCommands.field_34[0x0C] == 1) {
            dec->field_0x04 = MATCH_F32(float_0_80421890);
            *(s32*)&work->actionCommands.field_34[0x14] &= ~1;
        } else {
            dec->field_0x04 = MATCH_F32(float_50_804218c8);
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
            }
            break;
        }
        if (dec->frame > dec->field_0x0C) {
            work->actionCommands.result = 0;
            work->actionCommands.mAcState = 0x3EB;
            break;
        }
        if (dec->field_0x1C <= 0) {
            dec->field_0x08++;
        }
        if (*(s32*)&work->actionCommands.field_34[0x0C] == 1) {
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
            }
            if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                if ((s32)dec->field_0x04 <= 0) {
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
            }
            if (dec->field_0x04 >= (f32)*(s32*)&work->actionCommands.field_34[0x14]) {
                dec->field_0x10 = 1;
            } else {
                dec->field_0x10 = 0;
            }
        } else {
            if (dec->field_0x1C > 0) {
                dec->field_0x1C--;
                if (dec->field_0x1C > 0) {
                    dec->field_0x04 = MATCH_F32(float_0_80421890);
                    dec->field_0x10 = 0;
                    goto tail;
                }
            }
            dec->field_0x04 = ((f32)*(s32*)&work->actionCommands.field_34[0x14] / MATCH_F32(float_100_80421874)) *
                                  (f32)dec->field_0x08 +
                              MATCH_F32(float_50_804218c8);
            if (*(s32*)&work->actionCommands.field_34[0x00] & 1) {
                if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                    if ((s32)dec->field_0x04 / 100 > dec->field_0x20) {
                        dec->field_0x20 = (s32)dec->field_0x04 / 100;
                        dec->field_0x04 = MATCH_F32(float_0_80421890);
                        dec->field_0x10 = 0;
                        if (*(s32*)&work->actionCommands.field_34[0x00] & 2) {
                            dec->field_0x1C = 0xF;
                        } else {
                            dec->field_0x1C = 0x1E;
                        }
                        goto tail;
                    }
                }
                dec->field_0x04 = (f32)((s32)dec->field_0x04 % 100);
            }
            if (__fabsf(dec->field_0x04 - MATCH_F32(float_50_804218c8)) <= double_12_802ff938) {
                dec->field_0x10 = 1;
            } else {
                dec->field_0x10 = 0;
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
        if (dec->field_0x24 != -1) {
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
        work->actionCommands.resultcb = NULL;
        work->actionCommands.dispcb = NULL;
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
// NONMATCHING (88.69%): newly written; remaining diffs are register allocation and constant-pool labels, see REPORT.md.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
#ifdef MUST_MATCH
    // Deriving both bases from the same `ac` intermediate keeps disp=r27 and dec=r29, as the original.
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    GaugeTimingWork* dec = (GaugeTimingWork*)&ac->field_0x2BC[0];
#else
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    GaugeTimingWork* dec = (GaugeTimingWork*)((u8*)work + 0x1F4C);
#endif
    const Vec* vec = vec3_802ff860;
    CameraEntry* cam;
    f32 baseX;
    f32 baseY;
    f32 value;
    s32 icon0;
    s32 icon1;
    s32 icon2;
    s32 icon3;
    s32 i;
    s32 count;

    camGetPtr(CAMERA_2D);

    value = dec->field_0x04 / float_100_80421874;
    if (value > float_1_80421878) {
        value = float_1_80421878;
    }
    disp->value = value;

    if (*(s32*)&work->actionCommands.field_34[0x0C] == 1) {
        f32 ratio = dec->field_0x04 * float_100_80421874 / (f32)*(s32*)&work->actionCommands.field_34[0x10];
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
                (dec->field_0x04 >= (f32)*(s32*)&work->actionCommands.field_34[0x10]) ? 0x95 : 0x91;
            Vec v = vec[0];
            v.x = float_176_80421884 + float_neg288_80421888 + x + float_16_80421880;
            v.y = float_18_8042188c + y;
            ICON_DISP(&v, 0x10, icon, float_1_80421878);
        }
    } else {
        BattleAcDrawGauge((s32)x, (s32)y, 0xB2, 0x64, 0x64, 0x64, 0x64, 0, float_0_80421890);
        if (dec->field_0x1C <= 0) {
            f32 a;
            f32 px;
            f32 py;
            f32 p1;
            cam = camGetPtr(CAMERA_2D);
            a = dec->field_0x04 * float_173_8042189c / float_100_80421874;
            px = (float_neg200_80421894 + x) - float_88_80421898;
            py = float_49_804218a0 + y;
            p1 = px + a;
            btlDispGXInit2DRasta();
            GXLoadPosMtxImm(cam->view, 0);
            btlDispGXQuads2DRasta(p1, py, p1 + float_10_804218a4, py - float_16_80421880, 0, 0xFF, 0, 0xFF);
        }
        count = *(s32*)&work->actionCommands.field_34[0x10];
        for (i = 0; i < count; i++) {
            s32 icon = (dec->field_0x10 != 0) ? 0x95 : 0x91;
            Vec v = vec[1];
            v.x = (float_neg288_80421888 + x) +
                  (float_176_80421884 * (float_100_80421874 / (f32)(count + 1) * (f32)(i + 1)) /
                   float_100_80421874);
            v.y = float_20_804218a8 + y;
            ICON_DISP(&v, 0x10, icon, float_0p8_804218ac);
        }
    }

    switch (*(s32*)&work->actionCommands.field_34[0x04]) {
    case 0:
        icon0 = BattleACGetButtonIcon(0x20, 1);
        icon1 = BattleACGetButtonIcon(0x20, 0);
        break;
    case 2:
        icon0 = BattleACGetButtonIcon(0x4, 1);
        icon1 = BattleACGetButtonIcon(0x4, 0);
        break;
    default:
        icon0 = BattleACGetButtonIcon(0x100, 1);
        icon1 = BattleACGetButtonIcon(0x100, 0);
        break;
    }
    icon2 = 0;
    icon3 = 0;

    switch (work->actionCommands.mAcState) {
    case 0x63:
    case 0x64:
    case 0x3EB:
    case 0x3EC:
    case 0x3ED:
        if (icon2 != 0) {
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
                if (icon2 != 0) {
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
                if (icon2 != 0) {
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
                if (icon2 != 0) {
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
                if (icon2 != 0) {
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
