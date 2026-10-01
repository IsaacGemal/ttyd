#include "battle/ac/ac_power_gauge.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// actionCommandDisp is this unit's local helper (.fn actionCommandDisp, local).
static void actionCommandDisp(f32 x, f32 y);

// .sdata2 literals referenced by battleAcMain_PowerGauge and battleAcDisp_PowerGauge
// (offsets in REPORT.md).
#ifdef MUST_MATCH
// The original keeps the `lwz dat_80421588` late, after the disp stores; letting MWCC see the
// object as mutable stops it hoisting that load above them. __declspec(section) keeps the object
// in .sdata2, where the original const object lives.
__declspec(section ".sdata2") s32 dat_80421588 = -1;
#else
const s32 dat_80421588 = -1;
#endif
extern const f32 float_100_8042158c;
extern const f32 float_1_80421590;
extern const f32 float_neg288_80421594;
extern const f32 float_45_80421598;
extern const f32 float_120_8042159c;
extern const f32 float_neg200_804215a0;
extern const f32 float_12_804215a4;
extern const f32 float_18_804215a8;
extern const f32 float_neg225_804215ac;
extern const f32 float_80_804215b0;
extern const f32 float_56_804215b4;
extern const f32 float_28_804215b8;
extern const f32 float_neg300_804215bc;
extern const f32 float_30_804215c0;
extern const f32 float_0_804215c4;
extern const f32 float_101_804215c8;
extern const f64 double_to_int_802fea30;

// .rodata zero vectors used as icon positions by actionCommandDisp.
extern const Vec vec3_802fe958;
extern const Vec vec3_802fe964;
extern const Vec vec3_802fe970;
extern const Vec vec3_802fe97c;
extern const Vec vec3_802fe988;
extern const Vec vec3_802fe994;
extern const Vec vec3_802fe9a0;
extern const Vec vec3_802fe9ac;
extern const Vec vec3_802fe9b8;
extern const Vec vec3_802fe9c4;
extern const Vec vec3_802fe9d0;
extern const Vec vec3_802fe9dc;
extern const Vec vec3_802fe9e8;
extern const Vec vec3_802fe9f4;
extern const Vec vec3_802fea00;
extern const Vec vec3_802fea0c;
extern const Vec vec3_802fea18;
extern const Vec vec3_802fea24;

// Runtime work area of the power gauge action command, at BattleWork+0x1F4C (inside
// BattleActions.field_0x2BC). No header declares it yet.
typedef struct PowerGaugeDecWork {
    s32 frame;        // 0x0
    f32 gaugePercent; // 0x4
    s32 counter;      // 0x8, input-frame counter
    s32 value;        // 0xC
    s8 flag;          // 0x10
    s8 state;         // 0x11
    s8 flag2;         // 0x12
    u8 pad_0x13;      // 0x13
    u32 button;       // 0x14, BattlePad button mask
    s32 countdown;    // 0x18
} PowerGaugeDecWork;

// The original called intplGetValue through a prototype that lists the two float bounds
// before the step/duration words. The ABI is identical (ints->r3-r5, floats->f1-f2); only the
// source argument order differs, and MWCC then schedules the float loads first as retail does.
#ifdef MUST_MATCH
#define intplGetValueFM(mode, start, end, currStep, steps) \
    ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(mode, start, end, currStep, steps)
#else
#define intplGetValueFM(mode, start, end, currStep, steps) intplGetValue(mode, currStep, steps, start, end)
#endif

// optimize_for_size: the original saves r28-r31 with `stmw`; the default model uses individual saves.
#pragma optimize_for_size on
// The two near-misses this function had: MWCC hoists `lwz dat_80421588` above the disp stores
// (fixed by the mutable dat declaration above) and schedules `lwz dec->counter` before
// intplGetValue's `lfs` pair (fixed by intplGetValueFM above).
s32 battleAcMain_PowerGauge(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    PowerGaugeDecWork* dec = (PowerGaugeDecWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 autoCommand = 0;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (work->actionCommands.mAcState) {
    case 0x63:
        break;
    case 0x0:
        memset(disp, 0, 0x2C);
        disp->iconX = float_neg300_804215bc;
        disp->iconY = float_0_804215c4;
        disp->counter = 0x14;
        disp->value = float_0_804215c4;
        disp->field_0xC = dat_80421588;
        dec->frame = 0;
        dec->counter = 0;
        work->actionCommands.outputParams[0] = 0;
        work->actionCommands.mAcState = 0x63;
        work->actionCommands.result = 1;
        dec->value = *(s32*)((u8*)work + 0x1CCC) + *(s32*)((u8*)work + 0x1CD0);
        dec->gaugePercent = float_0_804215c4;
        switch (*(s32*)((u8*)work + 0x1CC8)) {
        case 0:
            dec->flag2 = 1;
            dec->button = 0x60;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 1:
            dec->flag2 = 1;
            dec->button = 0x100;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 2:
            dec->flag2 = 0;
            dec->button = 0x100;
            dec->state = 0;
            break;
        case 3:
            dec->flag2 = 0;
            dec->button = 0x20;
            dec->state = 0;
            break;
        default:
            dec->flag2 = 1;
            dec->button = 0x100;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        }
        break;
    case 0x64:
        if ((work->actionCommands.field_0x4 & 1) &&
            (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
            if ((s32)irand(100) >= 0) {
                work->actionCommands.field_0x8 = 0x1E;
                dec->flag = 0;
            }
            else {
                dec->flag = 1;
                work->actionCommands.field_0x8 = 0x1E;
            }
            work->actionCommands.outputParams[0] = 1;
            work->actionCommands.mAcState = 0x3EA;
            break;
        }
        if (dec->flag2 != 0) {
            work->actionCommands.mAcState = 0x3E9;
        }
        else {
            work->actionCommands.mAcState = 0x3E8;
        }
        if (dec->flag2 != 0) {
            BattleAcGaugeSeInit();
        }
        // fallthrough
    case 0x3E8:
    case 0x3E9:
        dec->frame++;
        switch (dec->state) {
        case 0:
            if (dec->flag2 == 0 && (BattlePadGetNow() == dec->button || autoCommand != 0)) {
                dec->state = 1;
                work->actionCommands.mAcState = 0x3E9;
                dec->frame = 0;
                work->actionCommands.outputParams[0] = 1;
                BattleAcGaugeSeInit();
            }
            break;
        case 1:
            if (dec->flag2 != 0) {
                if (BattlePadGetNow() == dec->button) {
                    if (BattlePadCheckTrigger(dec->button) != 0) {
                        dec->state = 2;
                    }
                }
            }
            else {
                if ((s32)BattlePadCheckNow(dec->button) == 0) {
                    if ((s32)BattlePadCheckUp(dec->button) != 0) {
                        dec->state = 2;
                    }
                }
            }
            break;
        }
        if ((*(u32*)((u8*)work + 0x1CC4) & 1) == 0) {
            dec->counter++;
        }
        else if (dec->state == 1) {
            dec->counter++;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            if (dec->frame > 0x78) {
                dec->frame = 0;
                work->actionCommands.result = 0;
                dec->gaugePercent = float_0_804215c4;
                work->actionCommands.mAcState = 0x3EB;
                work->actionCommands.outputParams[0] = 2;
                break;
            }
        }
        else {
            if (dec->frame > dec->value) {
                dec->frame = 0;
                work->actionCommands.result = 0;
                dec->gaugePercent = float_101_804215c8;
                work->actionCommands.mAcState = 0x3EB;
                break;
            }
        }
        if (dec->frame <= *(s32*)((u8*)work + 0x1CCC)) {
            dec->gaugePercent = intplGetValueFM(*(s32*)((u8*)work + 0x1CD4), float_0_804215c4, float_100_8042158c, dec->counter, *(s32*)((u8*)work + 0x1CCC));
        }
        else {
            dec->gaugePercent = float_100_8042158c;
        }
        BattleAcGaugeSeUpdate(dec->gaugePercent);
        if (autoCommand != 0) {
            if (dec->gaugePercent >= (f32)*(s32*)((u8*)work + 0x1CD8)) {
                dec->state = 2;
            }
            else {
                dec->state = 1;
            }
        }
        if (dec->state == 2) {
            if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1CD8)) {
                work->actionCommands.result = 0;
                work->actionCommands.result &= ~2;
            }
            else {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            work->actionCommands.mAcState = 0x3EB;
        }
        break;
    case 0x3EA:
        work->actionCommands.field_0x8--;
        if (work->actionCommands.field_0x8 < 0) {
            if (dec->flag != 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            else {
                work->actionCommands.result = 0;
            }
            work->actionCommands.field_0x30 |= 1;
            dec->countdown = 0x3C;
            work->actionCommands.mAcState = 0x3ED;
            return 0;
        }
        break;
    case 0x3EB:
        work->actionCommands.mAcState = 0x3EC;
        // fallthrough
    case 0x3EC:
        work->actionCommands.field_0x30 |= 1;
        BattleAcGaugeSeDelete();
        dec->countdown = 0x3C;
        work->actionCommands.mAcState = 0x3ED;
        // fallthrough
    case 0x3ED:
        dec->countdown--;
        if (dec->countdown <= 0) {
            work->actionCommands.mAcState = 0x3EE;
        }
        break;
    case 0x3EE:
        work->actionCommands.maincb = NULL;
        work->actionCommands.dispcb = NULL;
        work->actionCommands.resultcb = NULL;
        work->actionCommands.deletecb = NULL;
        return 0;
    }
    return 1;
}
#pragma optimize_for_size reset

s32 battleAcResult_PowerGauge(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_PowerGauge(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    s32 state = ac->mAcState;

    switch (state) {
    case 0x63:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_804215bc, float_30_804215c0);
        // fallthrough
    case 0x64:
    case 0x3e8:
    case 0x3e9:
    case 0x3eb:
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3ec:
    case 0x3ed:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_804215c0, float_neg300_804215bc);
        } else {
            disp->iconX = float_30_804215c0;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3c) {
            disp->counter++;
        }
        break;
    }
}

void battleAcDelete_PowerGauge(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

// Match-only: iconDispGx's header declares the icon id `s16`, but the original passes the value
// returned by BattleACGetButtonIcon straight through (no `extsh`/`clrlwi`); a differently-typed
// function pointer keeps the call a direct `bl iconDispGx`. Same workaround as ac_gauge_timing.
#ifdef MUST_MATCH
#define ICON_DISP(v, f, id, s) (((void (*)(Vec*, s16, s32, f32))iconDispGx)((v), (f), (id), (s)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((v), (f), (id), (s))
#endif

// The only remaining difference is the FPR colouring of the ratio clamp: the original loads
// float_100 into f1, gaugePercent into f2 and float_1 into f0; MWCC assigns gaugePercent f0 and
// float_1 f2. Every instruction is otherwise identical, and the float_100/float_1 symbols are still
// referenced, so the data section is unchanged.
// NONMATCHING (99.95%): ratio clamp's FPRs are f0/f2 swapped.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
#ifdef MUST_MATCH
    // Deriving disp before camGetPtr (and dec after it) keeps work=r31, disp=r24 and dec=r29 as the
    // original; the BattleActions intermediate keeps them from folding into the BattleWork base.
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    PowerGaugeDecWork* dec;
#else
    BattleActions* ac = &work->actionCommands;
    PowerGaugeDecWork* dec;
    BattleACDispParams* disp = &ac->dispParams;
#endif
    s32 iconA;
    s32 iconB;
    s32 iconC;
    s32 iconD;
    f32 ratio;
    // Copies of the parameters: using x/y directly lets their argument live ranges (f1/f2) merge
    // with the body's float temporaries and costs the f30/f31 save (98.96%).
    f32 px = x;
    f32 py = y;

    camGetPtr(CAMERA_2D);
#ifdef MUST_MATCH
    dec = (PowerGaugeDecWork*)&ac->field_0x2BC[0];
#else
    dec = (PowerGaugeDecWork*)((u8*)work + 0x1f4c);
#endif

    ratio = float_100_8042158c;
    ratio = dec->gaugePercent / ratio;
    if (ratio > float_1_80421590) {
        ratio = float_1_80421590;
    }
    disp->value = ratio;

    if (*(s32*)&ac->field_34[0] & 2) {
        BattleAcDrawGauge((s32)px, (s32)py, 0xb2, 1, 100, 100, 100, 0, ratio);
    }
    else {
        BattleAcDrawGauge((s32)px, (s32)py, 0xb2, 2, *(s32*)&ac->field_34[0x14], 100, 100, 0, ratio);
    }

    if ((*(s32*)&ac->field_34[0] & 4) == 0) {
        Vec pos = vec3_802fe958;
        pos.x = float_neg288_80421594 + px +
                (f32)(*(s32*)&ac->field_34[0x14] * 0xb0) / float_100_8042158c;
        pos.y = float_45_80421598 + py;
        ICON_DISP(&pos, 0x10, 0x96, float_1_80421590);
    }
    else {
        s32 color;

        if (dec->gaugePercent >= (f32)*(s32*)&ac->field_34[0x14] &&
            dec->gaugePercent <= float_100_8042158c) {
            color = 0x95;
        }
        else {
            color = 0x91;
        }
        if (*(s32*)&ac->field_34[0] & 8) {
            Vec pos = vec3_802fe964;
            pos.x = float_neg200_804215a0 + px + float_120_8042159c - float_12_804215a4;
            pos.y = float_18_804215a8 + py;
            ICON_DISP(&pos, 0x10, color, float_1_80421590);
        }
        else {
            Vec pos = vec3_802fe970;
            pos.x = float_neg200_804215a0 + px + float_120_8042159c;
            pos.y = float_18_804215a8 + py;
            ICON_DISP(&pos, 0x10, color, float_1_80421590);
        }
    }

    switch (*(s32*)&ac->field_34[4]) {
    case 0:
        iconA = 0x7f;
        iconB = 0x7e;
        iconC = 0x81;
        iconD = 0x80;
        break;
    case 1:
        iconA = BattleACGetButtonIcon(0x100, 1);
        iconB = BattleACGetButtonIcon(0x100, 0);
        iconC = 0;
        iconD = 0;
        break;
    case 2:
        iconA = BattleACGetButtonIcon(0x100, 1);
        iconB = BattleACGetButtonIcon(0x100, 0);
        iconC = 0;
        iconD = 0;
        break;
    case 3:
        iconA = BattleACGetButtonIcon(0x20, 1);
        iconB = BattleACGetButtonIcon(0x20, 0);
        iconC = 0;
        iconD = 0;
        break;
    }

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3eb:
    case 0x3ec:
    case 0x3ed:
        if ((iconC & 0xffff) != 0) {
            px = float_neg225_804215ac + px;
            py = float_80_804215b0 + py;
            {
                Vec pos = vec3_802fe97c;
                pos.x = px;
                pos.y = py;
                ICON_DISP(&pos, 0x10, iconB, float_1_80421590);
            }
            {
                Vec pos = vec3_802fe988;
                pos.x = float_56_804215b4 + px;
                pos.y = py;
                ICON_DISP(&pos, 0x10, iconD, float_1_80421590);
            }
        }
        else {
            Vec pos = vec3_802fe994;
            pos.x = float_neg225_804215ac + px + float_28_804215b8;
            pos.y = float_80_804215b0 + py;
            ICON_DISP(&pos, 0x10, iconB, float_1_80421590);
        }
        break;
    case 0x3e8:
    case 0x3e9:
        if (dec->flag2 != 0) {
            if (dec->gaugePercent >= (f32)*(s32*)&ac->field_34[0x14] &&
                dec->gaugePercent <= float_100_8042158c) {
                if ((iconC & 0xffff) != 0) {
                    px = float_neg225_804215ac + px;
                    py = float_80_804215b0 + py;
                    {
                        Vec pos = vec3_802fe9a0;
                        pos.x = px;
                        pos.y = py;
                        ICON_DISP(&pos, 0x10, iconA, float_1_80421590);
                    }
                    {
                        Vec pos = vec3_802fe9ac;
                        pos.x = float_56_804215b4 + px;
                        pos.y = py;
                        ICON_DISP(&pos, 0x10, iconC, float_1_80421590);
                    }
                }
                else {
                    Vec pos = vec3_802fe9b8;
                    pos.x = float_neg225_804215ac + px + float_28_804215b8;
                    pos.y = float_80_804215b0 + py;
                    ICON_DISP(&pos, 0x10, iconA, float_1_80421590);
                }
            }
            else {
                if ((iconC & 0xffff) != 0) {
                    px = float_neg225_804215ac + px;
                    py = float_80_804215b0 + py;
                    {
                        Vec pos = vec3_802fe9c4;
                        pos.x = px;
                        pos.y = py;
                        ICON_DISP(&pos, 0x10, iconB, float_1_80421590);
                    }
                    {
                        Vec pos = vec3_802fe9d0;
                        pos.x = float_56_804215b4 + px;
                        pos.y = py;
                        ICON_DISP(&pos, 0x10, iconD, float_1_80421590);
                    }
                }
                else {
                    Vec pos = vec3_802fe9dc;
                    pos.x = float_neg225_804215ac + px + float_28_804215b8;
                    pos.y = float_80_804215b0 + py;
                    ICON_DISP(&pos, 0x10, iconB, float_1_80421590);
                }
            }
        }
        else if ((dec->gaugePercent >= (f32)*(s32*)&ac->field_34[0x14] &&
                  dec->gaugePercent <= float_100_8042158c) ||
                 ac->outputParams[0] == 0) {
            if ((iconC & 0xffff) != 0) {
                px = float_neg225_804215ac + px;
                py = float_80_804215b0 + py;
                {
                    Vec pos = vec3_802fe9e8;
                    pos.x = px;
                    pos.y = py;
                    ICON_DISP(&pos, 0x10, iconB, float_1_80421590);
                }
                {
                    Vec pos = vec3_802fe9f4;
                    pos.x = float_56_804215b4 + px;
                    pos.y = py;
                    ICON_DISP(&pos, 0x10, iconD, float_1_80421590);
                }
            }
            else {
                Vec pos = vec3_802fea00;
                pos.x = float_neg225_804215ac + px + float_28_804215b8;
                pos.y = float_80_804215b0 + py;
                ICON_DISP(&pos, 0x10, iconB, float_1_80421590);
            }
        }
        else {
            if ((iconC & 0xffff) != 0) {
                px = float_neg225_804215ac + px;
                py = float_80_804215b0 + py;
                {
                    Vec pos = vec3_802fea0c;
                    pos.x = px;
                    pos.y = py;
                    ICON_DISP(&pos, 0x10, iconA, float_1_80421590);
                }
                {
                    Vec pos = vec3_802fea18;
                    pos.x = float_56_804215b4 + px;
                    pos.y = py;
                    ICON_DISP(&pos, 0x10, iconC, float_1_80421590);
                }
            }
            else {
                Vec pos = vec3_802fea24;
                pos.x = float_neg225_804215ac + px + float_28_804215b8;
                pos.y = float_80_804215b0 + py;
                ICON_DISP(&pos, 0x10, iconA, float_1_80421590);
            }
        }
        break;
    }
}

// .rodata definitions of this unit, in the original's order.
const Vec vec3_802fe958 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe964 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe970 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe97c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe988 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe994 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9a0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9ac = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9b8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9c4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9d0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9dc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9e8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fe9f4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea00 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea0c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea18 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea24 = {0.0f, 0.0f, 0.0f};

// .sdata2 definitions of this unit, in the original's order.
const f32 float_100_8042158c = 100.0f;
const f32 float_1_80421590 = 1.0f;
const f32 float_neg288_80421594 = -288.0f;
const f32 float_45_80421598 = 45.0f;
const f32 float_120_8042159c = 120.0f;
const f32 float_neg200_804215a0 = -200.0f;
const f32 float_12_804215a4 = 12.0f;
const f32 float_18_804215a8 = 18.0f;
const f32 float_neg225_804215ac = -225.0f;
const f32 float_80_804215b0 = 80.0f;
const f32 float_56_804215b4 = 56.0f;
const f32 float_28_804215b8 = 28.0f;
const f32 float_neg300_804215bc = -300.0f;
const f32 float_30_804215c0 = 30.0f;
const f32 float_0_804215c4 = 0.0f;
const f32 float_101_804215c8 = 101.0f;
