#include "eff/eff_starpoint.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>
#include "matching.h"

typedef struct StarPointData {
    s32 type;       // 0x00
    s32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    Vec position;   // 0x0C
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
} StarPointData;

extern const f32 float_deg2rad_804209c8;
extern const f32 float_0_804209cc;
extern const f32 float_6p2832_804209d0;
extern const f32 float_360_804209d4;
extern const f32 float_0p00761_804209d8;
extern const f32 float_0p16605_804209dc;
extern const f32 float_1_804209e0;
extern const f32 float_3p1416_804209e4;
extern const f32 float_1p5708_804209e8;
extern const f32 float_4p7124_804209ec;
extern const f32 float_2_804209f0;
extern const f32 float_0p03705_804209f4;
extern const f32 float_0p4967_804209f8;
extern const f32 float_14_804209fc;
const f64 double_to_int_802fdbf8 = 4503601774854144.0;
const char str_802FDBEC[] = "StarPoint";

// opt_common_subs off: with CSE on MWCC merges the loop-invariant float pool loads, which
// shifts the loop constants (k16605/cOne/k4967/14) one FPR too high versus the target.
#pragma opt_common_subs off
// opt_lifetimes off: the default lifetime analysis schedules the seed/temp FPRs differently
// (0.00761 lands in f4 vs f2); this recovers the target's instruction scheduling.
#pragma opt_lifetimes off
// Loop constants/seed FPRs match; only the int->float magic is f31 instead of f26, rotating the six
// top-block values (target cTwo..c0=f31..f27,magic=f26; ours magic=f31,cTwo..c0=f30..f26), and the
// loop temp rad is f1 not f3. Reordering the outer declarations/assignments, moving them inside the
// loop, naming the conversion or inlining it, and opt_common_subs/opt_lifetimes/scheduling/
// opt_propagation variants all leave the magic at f31. All 120 permutations of the five outer
// declarations also keep it at f31; with opt_common_subs on it lands at f22 (polynomial constants
// f23-f26) instead of f26. global_optimizer/opt_loop_invariants/opt_dead_assignments/
// optimize_for_size/optimization_level/pool_data/fp_contract change nothing (or worse). Assigning a
// loop-invariant constant inside the loop (the eff_stageclear/eff_recovery trick) makes mwcceppc 2.6
// loop forever here, with or without opt_lifetimes. A permuter job is the remaining route.
// NONMATCHING (96.94%): `i = 1` before `lowAlpha = 4` matches the target's init order (+0.06%).
EffectEntry* effStarPointEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
    s32 lowAlpha;
    EffectEntry* entry;
    StarPointData* data;
    s32 i;
    f32 cTwo, cPi, c360, c6, c0;

    entry = effEntry();
    entry->type = str_802FDBEC;
    entry->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(StarPointData));
    entry->userdata = data;
    entry->callback = effStarPointMain;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x04 = 60;
    data->field_0x08 = 0;

    c0 = float_0_804209cc;
    c6 = float_6p2832_804209d0;
    c360 = float_360_804209d4;
    cPi = float_3p1416_804209e4;
    cTwo = float_2_804209f0;

    i = 1;
    lowAlpha = 4;
    data = &data[1];
    for (; i < entry->count; i++, data++) {
        s32 angle = rand() % 360;
        f32 rad = (c6 * (f32)angle) / c360;
        f32 sinv, cosv, f0, f1;
        const f32 cOne = float_1_804209e0;
        f32 cSin1 = float_0p16605_804209dc;
        f32 cCos1 = float_0p4967_804209f8;
        f32 c14 = float_14_804209fc;

        data->position.x = c0;
        data->position.y = c0;
        data->position.z = c0;

        // sin approximation
        sinv = MATCH_F32(float_0p00761_804209d8);
        if (rad <= cPi) {
            if (rad <= float_1p5708_804209e8) {
                f0 = rad * rad;
                sinv = sinv * f0 - cSin1;
                sinv = sinv * f0 + cOne;
                sinv = sinv * rad;
            } else {
                f0 = rad - float_1p5708_804209e8;
                f1 = float_1p5708_804209e8 - f0;
                f0 = f1 * f1;
                sinv = sinv * f0 - cSin1;
                sinv = sinv * f0 + cOne;
                sinv = sinv * f1;
            }
        } else if (rad < float_4p7124_804209ec) {
            f1 = rad - cPi;
            f0 = f1 * f1;
            sinv = sinv * f0 - cSin1;
            sinv = sinv * f0 + cOne;
            sinv = sinv * f1;
            sinv = -sinv;
        } else {
            f0 = rad - float_4p7124_804209ec;
            f1 = float_1p5708_804209e8 - f0;
            f0 = f1 * f1;
            sinv = sinv * f0 - cSin1;
            sinv = sinv * f0 + cOne;
            sinv = sinv * f1;
            sinv = -sinv;
        }
        data->field_0x18 = cTwo * sinv;

        // cos approximation
        cosv = MATCH_F32(float_0p03705_804209f4);
        if (rad <= cPi) {
            if (rad <= float_1p5708_804209e8) {
                f0 = rad * rad;
                cosv = cosv * f0 - cCos1;
                cosv = cosv * f0 + cOne;
            } else {
                f0 = rad - float_1p5708_804209e8;
                f0 = float_1p5708_804209e8 - f0;
                f0 = f0 * f0;
                cosv = cosv * f0 - cCos1;
                cosv = cosv * f0 + cOne;
                cosv = -cosv;
            }
        } else if (rad < float_4p7124_804209ec) {
            f0 = rad - cPi;
            f0 = f0 * f0;
            cosv = cosv * f0 - cCos1;
            cosv = cosv * f0 + cOne;
            cosv = -cosv;
        } else {
            f0 = rad - float_4p7124_804209ec;
            f0 = float_1p5708_804209e8 - f0;
            f0 = f0 * f0;
            cosv = cosv * f0 - cCos1;
            cosv = cosv * f0 + cOne;
        }
        data->field_0x1C = cTwo * cosv;

        data->field_0x20 = c14;
        data->field_0x24 = cTwo;
        data->field_0x2C = 0;
        data->field_0x30 = lowAlpha;
        lowAlpha += 3;
    }

    return entry;
}

#pragma opt_lifetimes reset
#pragma opt_common_subs reset

void effStarPointMain(EffectEntry* effect) {
    Vec position;
    StarPointData* data = effect->userdata;
    Vec disp = { 0 };
    s32 i;

    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;

    data->field_0x04--;
    data->field_0x08++;
    if (data->field_0x04 < 0) {
        effDelete(effect);
        return;
    }

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x30 != 0) {
            data->field_0x30--;
            data->field_0x2C = 0;
        } else {
            data->field_0x20 -= data->field_0x24;
            data->position.x += data->field_0x18;
            data->position.y += data->field_0x20;
            data->position.z += data->field_0x1C;
            if (data->field_0x20 < float_0_804209cc) {
                data->field_0x2C -= 2;
                if (data->field_0x2C < 0) {
                    data->field_0x2C = 0;
                }
            } else {
                data->field_0x2C = 0xff;
            }
        }
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effStarPointDisp, effect);
}

void effStarPointDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    StarPointData* data = effect->userdata;
    StarPointData* elem;
    Mtx mtx;
    Mtx rot;
    Mtx elemMtx;
    s32 i;

    PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', float_deg2rad_804209c8 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(mtx, rot, mtx);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x2C != 0) {
            PSMTXTrans(elemMtx, data->position.x, data->position.y, data->position.z);
            PSMTXConcat(mtx, elemMtx, elemMtx);
            iconDispGx2(elemMtx, 0, 0x18C);
        }
    }
}
