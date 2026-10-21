#include "battle/ac/ac_power_gauge_lv2.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
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
// The int->float magic double (0x4330000080000000) is the compiler's own pool entry at
// 0x80300200; naming it in the source duplicates it and the function then relocates to the
// anonymous @NNN instead of the target's double_to_int_80300200. Leave it to the pool.

// --- .sdata literals of this unit ---
// _power_table is local to this unit in the original; _power_table2 is global. They are non-const,
// which is what puts them in .sdata (and makes mwcc use the SDA21 address form).
static s32 _power_table[7] = {0x12C, 0xC8, 0x96, 0x64, 0x50, 0x3C, 0x28};
s32 _power_table2[7] = {0x64, 0x64, 0x64, 0x64, 0x6E, 0x78, 0x82};

// --- .sdata2 literals ---
#ifdef MUST_MATCH
// Match-only: MWCC hoists a load of a `const` global above the surrounding stores, changing
// the register allocation; a non-const definition keeps the load where the target has it.
// __declspec keeps it in .sdata2 so the unit's data still matches.
__declspec(section ".sdata2") s32 dat_80421c20 = -1;
#else
const s32 dat_80421c20 = -1;
#endif

// Work area at BattleWork+0x1F4C (inside BattleActions.field_0x2BC). No header declares it.
typedef struct PowerGaugeLv2DecWork {
    s32 frame;        // 0x00
    s32 frameLimit;   // 0x04
    f32 gaugePercent; // 0x08
    s32 button;       // 0x0C, passed to BattlePadCheckNow
    f32 rate;         // 0x10, 0.01 * acParams[1]: the gain (outside a timing window) or loss (inside) per frame, times the power table
    f32 drain;        // 0x14, 0.01 * acParams[2]: lost per frame
    s32 flag;         // 0x18
    s32 flag2;        // 0x1C
    s16 timing[3];    // 0x20
    s16 pad_0x26;     // 0x26
    s32 endTimer;     // 0x28
} PowerGaugeLv2DecWork;

static void actionCommandDisp(f32, f32);

s32 battleAcMain_PowerGaugeLv2(BattleWork* work) {
    s32 difficulty;
    PowerGaugeLv2DecWork* dec;
    BattleACDispParams* disp;
    s32 autoCommand;
    s32 smooth;
    s32 window;
    s32 i;

    dec = (PowerGaugeLv2DecWork*)&work->actionCommands.field_0x2BC[0];
    disp = &work->actionCommands.dispParams;
#ifdef MUST_MATCH
    // Match-only: the callee really returns 4 bytes (no extsb in the original); the `s8` header
    // would sign-extend the `slwi r0, r31, 2` index.
    difficulty = ((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work);
#else
    difficulty = BattleActionCommandGetDifficulty(work);
#endif
    autoCommand = 0;
    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (work->actionCommands.mAcState) {
        case 0x0:
            memset(disp, 0, 0x2C);
            disp->iconX = -300.0f;
            disp->iconY = 0.0f;
            disp->counter = 0x14;
            disp->value = 0.0f;
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
            dec->gaugePercent = 0.0f;
            dec->rate = (f32)work->actionCommands.acParams[1] * 0.01f;
            dec->drain = (f32)work->actionCommands.acParams[2] * 0.01f;
            dec->flag = 0;
            dec->flag2 = 0;
            dec->button = 0x100;
            sysMsec2Frame(0x3E8);
            for (i = 0; i < 3; i++) {
                dec->timing[i] = (s16)(irand(sysMsec2Frame(0xFA)) + (i * 2 + 1) * sysMsec2Frame(0x3E8));
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
                        (s32)irand(work->actionCommands.acParams[3] - 1) + 1;
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
                if (work->actionCommands.outputParams[0] < work->actionCommands.acParams[4]) {
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
                        dec->gaugePercent += 0.01f *
                                             (dec->rate * (f32)_power_table[difficulty]);
                    }
                    else {
                        dec->gaugePercent -= 0.01f *
                                             (dec->rate * (f32)_power_table2[difficulty]);
                    }
                }
                if (dec->gaugePercent >= 100.0f) {
                    dec->gaugePercent = 100.0f;
                    dec->flag = 1;
                }
                else {
                    dec->gaugePercent -= dec->drain;
                }
#ifdef MUST_MATCH
                {
                    // Match-only: the volatile read forces the reload into f1 the target has;
                    // the natural form reuses f0 and swaps the compare operands.
                    f32 g = *(volatile f32*)&dec->gaugePercent;
                    if (g <= 0.0f) {
                        dec->gaugePercent = 0.0f;
                    }
                }
#else
                if (dec->gaugePercent <= 0.0f) {
                    dec->gaugePercent = 0.0f;
                }
#endif
            }
            BattleAcGaugeSeUpdate(dec->gaugePercent);
            work->actionCommands.outputParams[0] =
                (s32)(dec->gaugePercent * (f32)work->actionCommands.acParams[3] / 100.0f);
            if (work->actionCommands.outputParams[0] >= work->actionCommands.acParams[3]) {
                work->actionCommands.outputParams[0] = work->actionCommands.acParams[3] - 1;
            }
            if (!(*(u32*)&work->actionCommands.acFlag & 1) && dec->gaugePercent >= 100.0f) {
                work->actionCommands.outputParams[0]++;
            }
            break;
        case 0x3E9:
            work->actionCommands.field_0x8--;
            if (work->actionCommands.field_0x8 < 0) {
                if (work->actionCommands.outputParams[0] >= work->actionCommands.acParams[4]) {
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
            dec->endTimer = 0x3C;
            work->actionCommands.mAcState = 0x3EC;
            // fallthrough
        case 0x3EC:
            dec->endTimer--;
            if (dec->endTimer <= 0) {
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

// The original keeps disp (work+0x1F20) in r31 and reads mAcState once from work+0x1C9C.
// Binding it through `ac`/`disp` and a local `state` is what makes mwcc materialise the disp base.
void battleAcDisp_PowerGaugeLv2(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    s32 state = ac->mAcState;

    switch (state) {
        case 0x63:
            disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.0f,
                                        30.0f);
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
                disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.0f,
                                            -300.0f);
            }
            else {
                disp->iconX = 30.0f;
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

// Match-only: the header declares BattleAcDrawGauge's float ratio last, but the original's
// prototype lists it between `count` and the three thresholds: the ratio `lfs f1` is scheduled
// between the two `fctiwz` conversions only when the float parameter is evaluated before the
// threshold arguments. The ABI is identical (ints -> r3-r10, float -> f1).
#ifdef MUST_MATCH
#define BattleAcDrawGaugeFM(x, y, type, count, ratio, t0, t1, t2, flags)                                  \
    ((void (*)(s32, s32, s32, s32, f32, s32, s32, s32, s32))BattleAcDrawGauge)((x), (y), (type), (count), \
                                                                               (ratio), (t0), (t1), (t2), (flags))
#else
#define BattleAcDrawGaugeFM(x, y, type, count, ratio, t0, t1, t2, flags) \
    BattleAcDrawGauge((x), (y), (type), (count), (t0), (t1), (t2), (flags), (ratio))
#endif

// Match-only: the original is the FP division followed by the clamp, but writing the numerator
// through a temp (`g`) and the clamp bound as the literal 1.0f colours the three FP registers as
// the original does (numerator f2, 100.f f1, 1.f f0); the natural form rotates them.
// MATCHING (100%): all instructions agree; the anonymous `@NNN` for the 1.0f literal is the
// symbol-only difference noted above and disappears when the unit links.
static void actionCommandDisp(f32 x, f32 y) {
    Vec* vec = (Vec*)vec3_803001c0;
    BattleActions* ac = &_battleWorkPointer->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    PowerGaugeLv2DecWork* dec = (PowerGaugeLv2DecWork*)&ac->field_0x2BC[0];
    f32 f31;
    f32 f30;
    f32 ratio;
    s32 thresholds[4];
    s32 i;
    s32 count;
    s32 icon0;
    s32 icon1;

    camGetPtr(CAMERA_2D);

    f31 = -200.0f + x;
    f30 = 25.0f + y;

    {
        Vec pos = vec[0];
        pos.x = f31;
        pos.y = f30;
        iconDispGx(1.0f, &pos, 0x10, 0x8C);
    }

    {
        s32 flags = 0;

        f32 g = dec->gaugePercent;
        ratio = g / 100.0f;
        if (ratio > 1.0f) {
            ratio = 1.0f;
        }
        disp->value = ratio;

        if (dec->flag != 0) {
            ((u8*)ac)[0x29D] = (u8)(((u8*)ac)[0x29D] + ((u8*)ac)[0x2A1]);
            ((u8*)ac)[0x29E] = (u8)(((u8*)ac)[0x29E] + ((u8*)ac)[0x2A2]);

            if (disp->value >= 1.0f) {
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

        count = 0;
        for (i = 0; i < 4; i++) {
            thresholds[i] = 100;
        }
        for (i = 0; i < 4; i++, count++) {
            thresholds[i] = *(s32*)((u8*)ac + 0x78 + i * 4);
            if (i != 0 && (thresholds[i] - thresholds[i - 1]) <= 0) {
                break;
            }
        }

        BattleAcDrawGaugeFM((s32)x, (s32)y, 0xB2, count, disp->value, thresholds[0], thresholds[1],
                            thresholds[2], flags);
    }

    icon1 = BattleACGetButtonIcon(dec->button, 1);
    icon0 = BattleACGetButtonIcon(dec->button, 0);

    switch (ac->mAcState) {
        case 0x63:
        case 0x64:
        case 0x3EA:
        case 0x3EB:
        case 0x3EC:
        {
            Vec pos = vec[1];
            pos.x = f31;
            pos.y = 70.0f + y;
            iconDispGx(1.0f, &pos, 0x10, icon0);
            break;
        }
        case 0x3E8:
            if (dec->flag2 != 0) {
                Vec pos = vec[2];
                pos.x = f31;
                pos.y = 70.0f + y;
                iconDispGx(1.0f, &pos, 0x10, icon1);
            }
            else {
                Vec pos = vec[3];
                pos.x = f31;
                pos.y = 70.0f + y;
                iconDispGx(1.0f, &pos, 0x10, icon0);
            }
            break;
    }

    {
        s32 id = dec->flag2 != 0 ? 0x95 : 0x91;
        Vec pos = vec[4];
        pos.x = 120.0f + f31;
        pos.y = f30;
        iconDispGx(1.0f, &pos, 0x10, id);
    }
}

// --- .sdata2 definitions ---

// the 24 zero bytes after the unit's last .sdata object (gap_07_804179E8_sdata)
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") static u32 gap_end_sdata[6] = { 0, 0, 0, 0, 0, 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
