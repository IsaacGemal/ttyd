#include "battle/ac/ac_power_gauge_lv.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// --- Declarations that belong in a header (no ac_power_gauge_lv.h exists yet) ---

// --- .rodata literals of this unit (offsets in REPORT.md) ---
// The seven Vec3 literals are one contiguous array; the original object names them individually at
// 0xC strides. Referencing them as an array is what makes mwcc keep a single base register.
const Vec vec3_802fea38[7] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};
const f64 double_to_int_802fea90 = 4503601774854144.0;

// --- .sdata2 literals of this unit (offsets in REPORT.md) ---
// Declared here and defined at the bottom so the compiler emits a reference to the named symbol
// instead of folding the value into its own literal pool.
extern const s32 dat_804215d0;
extern const f32 float_neg200_804215d4;
extern const f32 float_25_804215d8;
extern const f32 float_1_804215dc;
extern const f32 float_100_804215e0;
extern const f32 float_neg220_804215e4;
extern const f32 float_70_804215e8;
extern const f32 float_56_804215ec;
extern const f32 float_neg300_804215f0;
extern const f32 float_30_804215f4;
extern const f32 float_0_804215f8;
extern const f32 float_101_804215fc;

// The gauge runtime work area at BattleWork+0x1F4C. No header declares it yet.
typedef struct PowerGaugeLvDecWork {
    s32 frame;        // 0x0
    f32 gaugePercent; // 0x4
    u32 field_0x8;    // 0x8
    s32 value;        // 0xC
    s32 flag;         // 0x10
    s32 field_0x14;   // 0x14
} PowerGaugeLvDecWork;

static void actionCommandDisp(f32, f32);

// don't_inline: the original keeps actionCommandDisp as its own local function.
#pragma dont_inline on
// NONMATCHING (73.11%): 15 instructions differ, 14 missing, 25 extra, plus register differences.
static void actionCommandDisp(f32 f1, f32 f2) {
    BattleWork* work = _battleWorkPointer;
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    PowerGaugeLvDecWork* dec = (PowerGaugeLvDecWork*)((u8*)work + 0x1f4c);
    Vec pos;
    f32 ratio;

    camGetPtr(CAMERA_2D);

    {
        pos = vec3_802fea38[0];
        pos.x = float_neg200_804215d4 + f1;
        pos.y = float_25_804215d8 + f2;
        iconDispGx(&pos, 0x10, 0x8c, float_1_804215dc);
    }

    ratio = dec->gaugePercent / float_100_804215e0;
    if (ratio > float_1_804215dc) {
        ratio = float_1_804215dc;
    }
    disp->value = ratio;

    BattleAcDrawGauge((s32)f1, (s32)f2, 0xb2, *(s32*)((u8*)work + 0x1ccc), *(s32*)((u8*)work + 0x1cd0),
                      *(s32*)((u8*)work + 0x1cd4), *(s32*)((u8*)work + 0x1cd8), 0, disp->value);

    switch (work->actionCommands.mAcState) {
    case 0x63:
    case 0x64:
    case 0x3ea:
    case 0x3eb:
    case 0x3ec:
        f1 = float_neg220_804215e4 + f1;
        f2 = float_70_804215e8 + f2;
        {
            pos = vec3_802fea38[1];
            pos.x = f1;
            pos.y = f2;
            iconDispGx(&pos, 0x10, 0x7e, float_1_804215dc);
        }
        {
            pos = vec3_802fea38[2];
            pos.x = float_56_804215ec + f1;
            pos.y = f2;
            iconDispGx(&pos, 0x10, 0x80, float_1_804215dc);
        }
        break;
    case 0x3e8:
        if (dec->gaugePercent >= (f32)*(s32*)((u8*)work + 0x1cd0) &&
            dec->gaugePercent <= float_100_804215e0) {
            f1 = float_neg220_804215e4 + f1;
            f2 = float_70_804215e8 + f2;
            {
                pos = vec3_802fea38[3];
                pos.x = f1;
                pos.y = f2;
                iconDispGx(&pos, 0x10, 0x7f, float_1_804215dc);
            }
            {
                pos = vec3_802fea38[4];
                pos.x = float_56_804215ec + f1;
                pos.y = f2;
                iconDispGx(&pos, 0x10, 0x81, float_1_804215dc);
            }
        }
        else {
            f1 = float_neg220_804215e4 + f1;
            f2 = float_70_804215e8 + f2;
            {
                pos = vec3_802fea38[5];
                pos.x = f1;
                pos.y = f2;
                iconDispGx(&pos, 0x10, 0x7e, float_1_804215dc);
            }
            {
                pos = vec3_802fea38[6];
                pos.x = float_56_804215ec + f1;
                pos.y = f2;
                iconDispGx(&pos, 0x10, 0x80, float_1_804215dc);
            }
        }
        break;
    }
}
#pragma dont_inline reset

void battleAcDelete_PowerGaugeLv(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3eb;
    }
}

// NONMATCHING (98.86%): only `addi r31, r4, 0x1f20` differs; mwcc folds the disp pointer into
// the BattleWork base (`mr r31, r4` + 0x1fxx offsets) instead of materialising it.
void battleAcDisp_PowerGaugeLv(CameraId camId, void* param) {
    BattleWork* work = (BattleWork*)param;
    BattleACDispParams* ag = &work->actionCommands.dispParams;
    s32 state = work->actionCommands.mAcState;

    switch (state) {
    case 0x63:
        ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, float_neg300_804215f0, float_30_804215f4);
        // fallthrough
    case 0x64:
    case 0x3e8:
    case 0x3ea:
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter > 0) {
            ag->counter--;
        }
        break;
    case 0x3eb:
    case 0x3ec:
        if (ag->counter >= 0x28) {
            ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, float_30_804215f4, float_neg300_804215f0);
        }
        else {
            ag->iconX = float_30_804215f4;
        }
        actionCommandDisp(ag->iconX, ag->iconY);
        if (ag->counter < 0x3c) {
            ag->counter++;
        }
        break;
    }
}

s32 battleAcResult_PowerGaugeLv(BattleWork* work) {
    return work->actionCommands.result;
}

// optimize_for_size: the original main saves r29-r31 with `stmw`; the size model reproduces its
// prologue/register choice for the state machine. The rest of the file does not need it.
#pragma optimize_for_size on
// NONMATCHING (96.38%): register allocation only. The original keeps work=r30, dec=r31, disp=r29;
// mwcc folds dec (and sometimes disp) into the BattleWork base instead of materialising them.
// All logic, switch tree, thresholds and the `result &= ~2` bit clear match.
s32 battleAcMain_PowerGaugeLv(BattleWork* work) {
    PowerGaugeLvDecWork* dec = (PowerGaugeLvDecWork*)((u8*)work + 0x1f4c);
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    switch (work->actionCommands.mAcState) {
    case 0x63:
        break;
    case 0:
        memset(disp, 0, 0x2c);
        disp->iconX = float_neg300_804215f0;
        disp->iconY = float_0_804215f8;
        disp->counter = 0x14;
        disp->value = float_0_804215f8;
        disp->field_0xC = dat_804215d0;
        dec->frame = 0;
        work->actionCommands.outputParams[0] = 0;
        work->actionCommands.mAcState = 0x63;
        work->actionCommands.result = 1;
        dec->value = 0x64;
        dec->gaugePercent = float_0_804215f8;
        break;
    case 0x64:
        if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
            if ((s32)irand(100) >= 0) {
                work->actionCommands.field_0x8 = 0x1e;
                work->actionCommands.outputParams[0] = 0;
            }
            else {
                s32 r = (s32)irand(*(s32*)((u8*)work + 0x1ccc) - 1) + 1;
                work->actionCommands.outputParams[0] = r;
                work->actionCommands.field_0x8 = 0x1e;
            }
            work->actionCommands.mAcState = 0x3e9;
            break;
        }
        work->actionCommands.mAcState = 0x3e8;
        // fallthrough
    case 0x3e8:
        dec->frame++;
        if (dec->frame > dec->value) {
            work->actionCommands.outputParams[0] = 0;
            work->actionCommands.result = 0;
            dec->gaugePercent = float_101_804215fc;
            return 0;
        }
        dec->gaugePercent = intplGetValue(2, dec->frame, dec->value, float_0_804215f8, float_100_804215e0);
        if (BattlePadGetNow() == 0x60) {
            if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cd0)) {
                work->actionCommands.result = 0;
                work->actionCommands.result &= ~2;
            }
            else {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
                if (*(s32*)((u8*)work + 0x1ccc) <= 1) {
                    work->actionCommands.outputParams[0] = 1;
                }
                else if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cd4)) {
                    work->actionCommands.outputParams[0] = 1;
                }
                else if (*(s32*)((u8*)work + 0x1ccc) <= 2) {
                    work->actionCommands.outputParams[0] = 2;
                }
                else if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cd8)) {
                    work->actionCommands.outputParams[0] = 2;
                }
                else if (*(s32*)((u8*)work + 0x1ccc) <= 3) {
                    work->actionCommands.outputParams[0] = 3;
                }
                else if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cdc)) {
                    work->actionCommands.outputParams[0] = 3;
                }
                else if (*(s32*)((u8*)work + 0x1ccc) <= 4) {
                    work->actionCommands.outputParams[0] = 4;
                }
                else {
                    work->actionCommands.outputParams[0] = 5;
                }
            }
            work->actionCommands.mAcState = 0x3ea;
        }
        break;
    case 0x3e9:
        work->actionCommands.field_0x8--;
        if (work->actionCommands.field_0x8 < 0) {
            if ((s32)work->actionCommands.outputParams[0] > 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            else {
                work->actionCommands.result = 0;
            }
            return 0;
        }
        break;
    case 0x3ea:
        work->actionCommands.mAcState = 0x3eb;
        // fallthrough
    case 0x3eb:
        work->actionCommands.field_0x30 |= 1;
        dec->flag = 0x3c;
        work->actionCommands.mAcState = 0x3ec;
        // fallthrough
    case 0x3ec:
        dec->flag--;
        if (dec->flag <= 0) {
            work->actionCommands.mAcState = 0x3ed;
        }
        break;
    case 0x3ed:
        work->actionCommands.maincb = NULL;
        work->actionCommands.dispcb = NULL;
        work->actionCommands.resultcb = NULL;
        work->actionCommands.deletecb = NULL;
        return 0;
    }
    return 1;
}
#pragma optimize_for_size reset

// --- .sdata2 definitions (kept after the code, see note above) ---
const s32 dat_804215d0 = -1;
const f32 float_neg200_804215d4 = -200.0f;
const f32 float_25_804215d8 = 25.0f;
const f32 float_1_804215dc = 1.0f;
const f32 float_100_804215e0 = 100.0f;
const f32 float_neg220_804215e4 = -220.0f;
const f32 float_70_804215e8 = 70.0f;
const f32 float_56_804215ec = 56.0f;
const f32 float_neg300_804215f0 = -300.0f;
const f32 float_30_804215f4 = 30.0f;
const f32 float_0_804215f8 = 0.0f;
const f32 float_101_804215fc = 101.0f;
