#include "battle/ac/ac_power_gauge_lv.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>

// --- Declarations that belong in a header (no ac_power_gauge_lv.h exists yet) ---

// --- .rodata literals of this unit (offsets in REPORT.md) ---
// The original object names the seven Vec3 literals individually at 0xC strides (0x802fea38..0x802fea80),
// followed by the int->float magic double at 0x802fea90.
const Vec vec3_802fea38 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea44 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea50 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea5c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea68 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea74 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fea80 = {0.0f, 0.0f, 0.0f};

// --- .sdata2 literals of this unit (offsets in REPORT.md) ---
// The float constants are declared here and defined at the bottom so the compiler emits a reference to
// the named symbol instead of folding the value into its own literal pool.
#ifdef MUST_MATCH
// A `const` object here lets MWCC hoist this function's `lwz r6, dat` above the disp stores; the
// original schedules the load at its store. Non-const defeats the alias analysis, and the section
// directive keeps it in .sdata2 where the original has it.
__declspec(section ".sdata2") s32 dat_804215d0 = -1;
#else
const s32 dat_804215d0 = -1;
#endif
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

// MWCC folds dec (and disp) into the BattleWork base when both are derived directly from
// `work`; the MUST_MATCH `ac` intermediate below keeps work=r30, dec=r31 and disp=r29 as the original.
s32 battleAcMain_PowerGaugeLv(BattleWork* work) {
#ifdef MUST_MATCH
    // MWCC folds dec (and disp) into the BattleWork base when both are derived directly from `work`;
    // deriving both from the `ac` intermediate keeps work, dec and disp in their own registers, as the
    // original does (same fix as battleAcMain_CraneTiming).
    BattleActions* ac = &work->actionCommands;
    PowerGaugeLvDecWork* dec = (PowerGaugeLvDecWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
#else
    PowerGaugeLvDecWork* dec = (PowerGaugeLvDecWork*)&work->actionCommands.field_0x2BC[0];
    BattleACDispParams* disp = &work->actionCommands.dispParams;
#endif

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
                s32 r = (s32)irand(work->actionCommands.acParams[1] - 1) + 1;
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
            if (dec->gaugePercent < (f32)work->actionCommands.acParams[2]) {
                work->actionCommands.result = 0;
                work->actionCommands.result &= ~2;
            }
            else {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
                if (work->actionCommands.acParams[1] <= 1) {
                    work->actionCommands.outputParams[0] = 1;
                }
                else if (dec->gaugePercent < (f32)work->actionCommands.acParams[3]) {
                    work->actionCommands.outputParams[0] = 1;
                }
                else if (work->actionCommands.acParams[1] <= 2) {
                    work->actionCommands.outputParams[0] = 2;
                }
                else if (dec->gaugePercent < (f32)work->actionCommands.acParams[4]) {
                    work->actionCommands.outputParams[0] = 2;
                }
                else if (work->actionCommands.acParams[1] <= 3) {
                    work->actionCommands.outputParams[0] = 3;
                }
                else if (dec->gaugePercent < (f32)work->actionCommands.acParams[5]) {
                    work->actionCommands.outputParams[0] = 3;
                }
                else if (work->actionCommands.acParams[1] <= 4) {
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
s32 battleAcResult_PowerGaugeLv(BattleWork* work) {
    return work->actionCommands.result;
}

// The `BattleActions*` intermediate is what makes MWCC materialise `disp` in r31
// (`addi r31, r4, 0x1f20`) instead of folding it into the BattleWork base
// (`mr r31, r4` + 0x1fxx offsets); the state is read through `ac` and folds back to r4.
void battleAcDisp_PowerGaugeLv(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    s32 state = ac->mAcState;

    switch (state) {
    case 0x63:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_804215f0, float_30_804215f4);
        // fallthrough
    case 0x64:
    case 0x3e8:
    case 0x3ea:
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3eb:
    case 0x3ec:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_804215f4, float_neg300_804215f0);
        }
        else {
            disp->iconX = float_30_804215f4;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3c) {
            disp->counter++;
        }
        break;
    }
}
#pragma opt_common_subs reset
#pragma dont_inline reset
#pragma optimize_for_size reset
void battleAcDelete_PowerGaugeLv(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 0x3eb;
    }
}

// don't_inline: the original keeps actionCommandDisp as its own local function.
#pragma dont_inline on
// opt_common_subs off: with CSE on MWCC reuses the gauge-percent value it already has in f0 and
// emits `fdivs f0, f1, f0`; disabling common subexpression elimination gives the original operand
// registers (numerator f2, divisor f1).
#pragma opt_common_subs off
// The clamp keeps `ratio` in the original's divisor register (f1: `fdivs f1, f2, f1`, `fmr f1, f0`).
static void actionCommandDisp(f32 f1, f32 f2) {
    BattleWork* work = _battleWorkPointer;
#ifdef MUST_MATCH
    // Deriving disp/dec through the BattleActions intermediate makes MWCC materialise them in their
    // own registers (work=r30, disp=r29, dec=r28) instead of folding them into the BattleWork base.
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    PowerGaugeLvDecWork* dec;
#else
    BattleACDispParams* disp = &work->actionCommands.dispParams;
    PowerGaugeLvDecWork* dec;
#endif
    f32 ratio;

    camGetPtr(CAMERA_2D);
#ifdef MUST_MATCH
    dec = (PowerGaugeLvDecWork*)&ac->field_0x2BC[0];
#else
    dec = (PowerGaugeLvDecWork*)&work->actionCommands.field_0x2BC[0];
#endif

    {
        Vec pos = vec3_802fea38;
        pos.x = float_neg200_804215d4 + f1;
        pos.y = float_25_804215d8 + f2;
        iconDispGx(float_1_804215dc, &pos, 0x10, 0x8c);
    }

    ratio = dec->gaugePercent / float_100_804215e0;
    if (ratio > float_1_804215dc) {
        ratio = float_1_804215dc;
    }
    disp->value = ratio;

#ifdef MUST_MATCH
    // Match-only: the volatile read keeps `ratio` live across the draw call's argument setup, which
    // is what makes MWCC reuse the divisor register for the division (`fdivs f1, f2, f1`) instead of
    // coalescing the result with the numerator; it also produces the original's reload of disp->value
    // for the last argument. Plain `disp->value` (below) compiles to `fdivs f2, f2, f1`.
    ratio = *(volatile f32*)&disp->value;
    BattleAcDrawGauge((s32)f1, (s32)f2, 0xb2, work->actionCommands.acParams[1],
                      work->actionCommands.acParams[2],
                      work->actionCommands.acParams[3],
                      work->actionCommands.acParams[4], 0, ratio);
#else
    BattleAcDrawGauge((s32)f1, (s32)f2, 0xb2, work->actionCommands.acParams[1],
                      work->actionCommands.acParams[2],
                      work->actionCommands.acParams[3],
                      work->actionCommands.acParams[4], 0, disp->value);
#endif

    switch (work->actionCommands.mAcState) {
    case 0x63:
    case 0x64:
    case 0x3ea:
    case 0x3eb:
    case 0x3ec:
        f1 = float_neg220_804215e4 + f1;
        f2 = float_70_804215e8 + f2;
        {
            Vec pos = vec3_802fea44;
            pos.x = f1;
            pos.y = f2;
            iconDispGx(float_1_804215dc, &pos, 0x10, 0x7e);
        }
        {
            Vec pos = vec3_802fea50;
            pos.x = float_56_804215ec + f1;
            pos.y = f2;
            iconDispGx(float_1_804215dc, &pos, 0x10, 0x80);
        }
        break;
    case 0x3e8:
        if (dec->gaugePercent >= (f32)work->actionCommands.acParams[2] &&
            dec->gaugePercent <= float_100_804215e0) {
            f1 = float_neg220_804215e4 + f1;
            f2 = float_70_804215e8 + f2;
            {
                Vec pos = vec3_802fea5c;
                pos.x = f1;
                pos.y = f2;
                iconDispGx(float_1_804215dc, &pos, 0x10, 0x7f);
            }
            {
                Vec pos = vec3_802fea68;
                pos.x = float_56_804215ec + f1;
                pos.y = f2;
                iconDispGx(float_1_804215dc, &pos, 0x10, 0x81);
            }
        }
        else {
            f1 = float_neg220_804215e4 + f1;
            f2 = float_70_804215e8 + f2;
            {
                Vec pos = vec3_802fea74;
                pos.x = f1;
                pos.y = f2;
                iconDispGx(float_1_804215dc, &pos, 0x10, 0x7e);
            }
            {
                Vec pos = vec3_802fea80;
                pos.x = float_56_804215ec + f1;
                pos.y = f2;
                iconDispGx(float_1_804215dc, &pos, 0x10, 0x80);
            }
        }
        break;
    }
}
#pragma opt_common_subs reset
#pragma dont_inline reset

// --- .sdata2 definitions (kept after the code, see note above) ---

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
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
