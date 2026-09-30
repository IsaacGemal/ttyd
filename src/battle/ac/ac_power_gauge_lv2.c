#include "battle/ac/ac_power_gauge_lv2.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// --- .rodata literals of this unit ---
// The five Vec3 literals are one contiguous array; the original names them individually at 0xC
// strides. Referencing them as an array is what makes mwcc keep a single base register.
const Vec vec3_803001c0[5] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};
const f64 double_to_int_80300200 = 4503601774854144.0;

// --- .sdata literals of this unit ---
// _power_table is local to this unit in the original; _power_table2 is global. They are non-const,
// which is what puts them in .sdata (and makes mwcc use the SDA21 address form).
static s32 _power_table[7] = {0x12C, 0xC8, 0x96, 0x64, 0x50, 0x3C, 0x28};
s32 _power_table2[7] = {0x64, 0x64, 0x64, 0x64, 0x6E, 0x78, 0x82};

// --- .sdata2 literals ---
extern const s32 dat_80421c20;
extern const f32 float_neg200_80421c24;
extern const f32 float_25_80421c28;
extern const f32 float_1_80421c2c;
extern const f32 float_100_80421c30;
extern const f32 float_70_80421c34;
extern const f32 float_120_80421c38;
extern const f32 float_neg300_80421c3c;
extern const f32 float_30_80421c40;
extern const f32 float_0_80421c44;
extern const f32 float_0p01_80421c48;

// Work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC). No header declares it.
typedef struct PowerGaugeLv2DecWork {
    s32 frame;        // 0x00
    s32 frameLimit;   // 0x04
    f32 gaugePercent; // 0x08
    s32 button;       // 0x0C, passed to BattlePadCheckNow
    f32 field_0x10;   // 0x10
    f32 field_0x14;   // 0x14
    s32 flag;         // 0x18
    s32 flag2;        // 0x1C
    s16 timing[3];    // 0x20
    s16 pad_0x26;     // 0x26
    s32 field_0x28;   // 0x28
} PowerGaugeLv2DecWork;

static void actionCommandDisp(f32, f32);

// NONMATCHING (98.44%): `BattleActionCommandGetDifficulty` returns s8 in the header, so the
// assignment emits `extsb r31, r3`; the original references the named double_to_int_80300200
// instead of an anonymous @NNN; and a few scheduler/register choices differ.
s32 battleAcMain_PowerGaugeLv2(BattleWork* work) {
    s32 difficulty;
    PowerGaugeLv2DecWork* dec;
    BattleACDispParams* disp;
    s32 autoCommand;
    s32 smooth;
    s32 window;
    s32 i;

    dec = (PowerGaugeLv2DecWork*)((u8*)work + 0x1F4C);
    disp = &work->actionCommands.dispParams;
    difficulty = BattleActionCommandGetDifficulty(work);
    autoCommand = 0;
    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (work->actionCommands.mAcState) {
        case 0x0:
            memset(disp, 0, 0x2C);
            disp->iconX = float_neg300_80421c3c;
            disp->iconY = float_0_80421c44;
            disp->counter = 0x14;
            disp->value = float_0_80421c44;
            disp->field_0xC = dat_80421c20;
            ((u8*)disp)[0x10] = 0;
            ((s8*)disp)[0x11] = -0x10;
            ((s8*)disp)[0x12] = -0x10;
            ((u8*)disp)[0x13] = 0;
            dec->frame = 0;
            work->actionCommands.outputParams[0] = 0;
            work->actionCommands.mAcState = 0x63;
            work->actionCommands.result = 1;
            dec->frameLimit = sysMsec2Frame(0x1B58);
            dec->gaugePercent = float_0_80421c44;
            dec->field_0x10 = (f32)*(s32*)((u8*)work + 0x1CCC) * float_0p01_80421c48;
            dec->field_0x14 = (f32)*(s32*)((u8*)work + 0x1CD0) * float_0p01_80421c48;
            dec->flag = 0;
            dec->flag2 = 0;
            dec->button = 0x100;
            sysMsec2Frame(0x3E8);
            for (i = 0; i < 3; i++) {
                dec->timing[i] = (s16)(irand(sysMsec2Frame(0xFA)) + (i + 1) * sysMsec2Frame(0x3E8));
            }
            break;
        case 0x63:
            break;
        case 0x64:
            if ((work->actionCommands.field_0x4 & 1) &&
                (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
                if ((s32)irand(100) >= 0) {
                    work->actionCommands.field_0x8 = 0x1E;
                    work->actionCommands.outputParams[0] = 0;
                }
                else {
                    work->actionCommands.outputParams[0] =
                        (s32)irand(*(s32*)((u8*)work + 0x1CD4) - 1) + 1;
                    work->actionCommands.field_0x8 = 0x1E;
                }
                work->actionCommands.mAcState = 0x3E9;
                break;
            }
            BattleAcGaugeSeInit();
            work->actionCommands.mAcState = 0x3E8;
            // fallthrough
        case 0x3E8:
            dec->frame++;
            if (dec->frame > dec->frameLimit) {
                if (*(s32*)((u8*)work + 0x1CE8) < *(s32*)((u8*)work + 0x1CD8)) {
                    work->actionCommands.result = 0;
                    work->actionCommands.result &= ~2;
                }
                else {
                    work->actionCommands.result = 2;
                    work->actionCommands.mResultCount++;
                }
                work->actionCommands.mAcState = 0x3EA;
                break;
            }
            smooth = BattlePadCheckNow(dec->button) != 0;
            window = sysMsec2Frame(0x2EE);
            dec->flag2 = 1;
            for (i = 0; i < 3; i++) {
                if (dec->frame >= dec->timing[i] && dec->frame < dec->timing[i] + window) {
                    dec->flag2 = 0;
                    break;
                }
            }
            if (autoCommand) {
                if (dec->flag2 != 0) {
                    smooth = 1;
                }
                else {
                    smooth = 0;
                }
            }
            if (dec->flag == 0) {
                if (smooth) {
                    if (dec->flag2 != 0) {
                        dec->gaugePercent += float_0p01_80421c48 *
                                             (dec->field_0x10 * (f32)_power_table[difficulty]);
                    }
                    else {
                        dec->gaugePercent -= float_0p01_80421c48 *
                                             (dec->field_0x10 * (f32)_power_table2[difficulty]);
                    }
                }
                if (dec->gaugePercent >= float_100_80421c30) {
                    dec->gaugePercent = float_100_80421c30;
                    dec->flag = 1;
                }
                else {
                    dec->gaugePercent -= dec->field_0x14;
                }
                if (dec->gaugePercent <= float_0_80421c44) {
                    dec->gaugePercent = float_0_80421c44;
                }
            }
            BattleAcGaugeSeUpdate(dec->gaugePercent);
            work->actionCommands.outputParams[0] =
                (s32)(dec->gaugePercent * (f32)*(s32*)((u8*)work + 0x1CD4) / float_100_80421c30);
            if (work->actionCommands.outputParams[0] >= *(s32*)((u8*)work + 0x1CD4)) {
                work->actionCommands.outputParams[0] = *(s32*)((u8*)work + 0x1CD4) - 1;
            }
            if (!(*(u32*)((u8*)work + 0x1CC4) & 1) && dec->gaugePercent >= float_100_80421c30) {
                work->actionCommands.outputParams[0]++;
            }
            break;
        case 0x3E9:
            work->actionCommands.field_0x8--;
            if (work->actionCommands.field_0x8 < 0) {
                if (work->actionCommands.outputParams[0] >= *(s32*)((u8*)work + 0x1CD8)) {
                    work->actionCommands.result = 2;
                    work->actionCommands.mResultCount++;
                }
                else {
                    work->actionCommands.result = 0;
                }
                return 0;
            }
            break;
        case 0x3EA:
            work->actionCommands.mAcState = 0x3EB;
            // fallthrough
        case 0x3EB:
            work->actionCommands.field_0x30 |= 1;
            BattleAcGaugeSeDelete();
            dec->field_0x28 = 0x3C;
            work->actionCommands.mAcState = 0x3EC;
            // fallthrough
        case 0x3EC:
            dec->field_0x28--;
            if (dec->field_0x28 <= 0) {
                work->actionCommands.mAcState = 0x3ED;
            }
            break;
        case 0x3ED:
            work->actionCommands.maincb = NULL;
            work->actionCommands.dispcb = NULL;
            work->actionCommands.resultcb = NULL;
            work->actionCommands.deletecb = NULL;
            return 0;
    }

    return 1;
}

s32 battleAcResult_PowerGaugeLv2(BattleWork* work) {
    return work->actionCommands.result;
}

// NONMATCHING (98.86%): mwcc folds `disp` into the `work` base and keeps work in r31; the original
// materialises work+0x1F20 in r31 and accesses 0x14/0x18/0x20.
void battleAcDisp_PowerGaugeLv2(CameraId camId, void* param) {
    BattleWork* work = (BattleWork*)param;
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    switch (work->actionCommands.mAcState) {
        case 0x63:
            disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_80421c3c,
                                        float_30_80421c40);
            // fallthrough
        case 0x64:
        case 0x3E8:
        case 0x3EA:
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter > 0) {
                disp->counter--;
            }
            break;
        case 0x3EB:
        case 0x3EC:
            if (disp->counter >= 0x28) {
                disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_80421c40,
                                            float_neg300_80421c3c);
            }
            else {
                disp->iconX = float_30_80421c40;
            }
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter < 0x3C) {
                disp->counter++;
            }
            break;
    }
}

void battleAcDelete_PowerGaugeLv2(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EB;
    }
}

// NONMATCHING (84.40%): the frame/layout now match, but the Vec BaseRemat, the integer register
// allocation in the two threshold loops, and the icon argument setup still differ.
static void actionCommandDisp(f32 x, f32 y) {
    BattleActions* ac = (BattleActions*)((u8*)_battleWorkPointer + 0x1C90);
    const Vec* vec = vec3_803001c0;
    f32 f31;
    f32 f30;
    f32 ratio;
    s32 thresholds[4];
    s32 i;
    s32 count;
    s32 icon0;
    s32 icon1;

    camGetPtr(CAMERA_2D);

    f31 = float_neg200_80421c24 + x;
    f30 = float_25_80421c28 + y;

    {
        Vec pos = vec[0];
        pos.x = f31;
        pos.y = f30;
        iconDispGx(&pos, 0x10, 0x8C, float_1_80421c2c);
    }

    {
        s32 flags = 0;

        ratio = *(f32*)((u8*)ac + 0x2C4) / float_100_80421c30;
        if (ratio > float_1_80421c2c) {
            ratio = float_1_80421c2c;
        }
        *(f32*)((u8*)ac + 0x2B8) = ratio;

        if (*(s32*)((u8*)ac + 0x2D4) != 0) {
            ((u8*)ac)[0x29D] = (u8)(((u8*)ac)[0x29D] + ((u8*)ac)[0x2A1]);
            ((u8*)ac)[0x29E] = (u8)(((u8*)ac)[0x29E] + ((u8*)ac)[0x2A2]);

            if (ratio >= float_1_80421c2c) {
                flags |= 1;
            }
            if (((u8*)ac)[0x29D] <= 0x80 && (s8)((u8*)ac)[0x2A1] < 0) {
                ((s8*)ac)[0x2A2] = 0x10;
                ((s8*)ac)[0x2A1] = 0x10;
            }
            if (((u8*)ac)[0x29D] >= 0xF5 && (s8)((u8*)ac)[0x2A1] > 0) {
                ((s8*)ac)[0x2A2] = -0x10;
                ((s8*)ac)[0x2A1] = -0x10;
            }
        }

        for (i = 0; i < 4; i++) {
            thresholds[i] = 100;
        }
        count = 0;
        for (i = 0; i < 4; i++) {
            thresholds[i] = *(s32*)((u8*)ac + 0x78 + i * 4);
            if (i != 0 && (thresholds[i] - thresholds[i - 1]) <= 0) {
                break;
            }
            count++;
        }

        BattleAcDrawGauge((s32)x, (s32)y, 0xB2, count, thresholds[0], thresholds[1], thresholds[2], flags,
                          *(f32*)((u8*)ac + 0x2B8));
    }

    icon1 = BattleACGetButtonIcon(*(s32*)((u8*)ac + 0x2C8), 1);
    icon0 = BattleACGetButtonIcon(*(s32*)((u8*)ac + 0x2C8), 0);

    switch (ac->mAcState) {
        case 0x63:
        case 0x64:
        case 0x3EA:
        case 0x3EB:
        case 0x3EC:
        {
            Vec pos = vec[1];
            pos.x = f31;
            pos.y = float_70_80421c34 + y;
            iconDispGx(&pos, 0x10, icon0, float_1_80421c2c);
            break;
        }
        case 0x3E8:
            if (*(s32*)((u8*)ac + 0x2D8) != 0) {
                Vec pos = vec[2];
                pos.x = f31;
                pos.y = float_70_80421c34 + y;
                iconDispGx(&pos, 0x10, icon1, float_1_80421c2c);
            }
            else {
                Vec pos = vec[3];
                pos.x = f31;
                pos.y = float_70_80421c34 + y;
                iconDispGx(&pos, 0x10, icon0, float_1_80421c2c);
            }
            break;
    }

    {
        Vec pos = vec[4];
        pos.x = float_120_80421c38 + f31;
        pos.y = f30;
        iconDispGx(&pos, 0x10, *(s32*)((u8*)ac + 0x2D8) != 0 ? 0x95 : 0x91, float_1_80421c2c);
    }
}

// --- .sdata2 definitions ---
const s32 dat_80421c20 = -1;
const f32 float_neg200_80421c24 = -200.0f;
const f32 float_25_80421c28 = 25.0f;
const f32 float_1_80421c2c = 1.0f;
const f32 float_100_80421c30 = 100.0f;
const f32 float_70_80421c34 = 70.0f;
const f32 float_120_80421c38 = 120.0f;
const f32 float_neg300_80421c3c = -300.0f;
const f32 float_30_80421c40 = 30.0f;
const f32 float_0_80421c44 = 0.0f;
const f32 float_0p01_80421c48 = 0.01f;
