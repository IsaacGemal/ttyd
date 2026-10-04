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

static inline f32 effStarPointSinf(f32 rad, f32 b, f32 one, f32 pi) {
    f32 v = MATCH_F32(float_0p00761_804209d8);
        f32 t, u;

    if (rad <= pi) {
        if (rad <= float_1p5708_804209e8) {
            t = rad * rad;
            v = v * t - b;
            v = v * t + one;
            v = v * rad;
        } else {
            t = rad - float_1p5708_804209e8;
            u = float_1p5708_804209e8 - t;
            t = u * u;
            v = v * t - b;
            v = v * t + one;
            v = v * u;
        }
    } else if (rad < float_4p7124_804209ec) {
        t = rad - pi;
        u = t * t;
        v = v * u - b;
        v = v * u + one;
        v = v * t;
        v = -v;
    } else {
        t = rad - float_4p7124_804209ec;
        u = float_1p5708_804209e8 - t;
        t = u * u;
        v = v * t - b;
        v = v * t + one;
        v = v * u;
        v = -v;
    }
    return v;
}

static inline f32 effStarPointCosf(f32 rad, f32 b, f32 one, f32 pi) {
    f32 v = MATCH_F32(float_0p03705_804209f4);
            f32 t, u;

    if (rad <= pi) {
        if (rad <= float_1p5708_804209e8) {
            t = rad * rad;
            v = v * t - b;
            v = v * t + one;
        } else {
            t = rad - float_1p5708_804209e8;
            u = float_1p5708_804209e8 - t;
            t = u * u;
            v = v * t - b;
            v = v * t + one;
            v = -v;
        }
    } else if (rad < float_4p7124_804209ec) {
        t = rad - pi;
        t = t * t;
        v = v * t - b;
        v = v * t + one;
        v = -v;
    } else {
        t = rad - float_4p7124_804209ec;
        u = float_1p5708_804209e8 - t;
        t = u * u;
        v = v * t - b;
        v = v * t + one;
    }
    return v;
}

// opt_common_subs off: with CSE on MWCC merges the loop-invariant float pool loads, which
// shifts the loop constants (k16605/cOne/k4967/14) one FPR too high versus the target.
#pragma opt_common_subs off
// opt_lifetimes off: the default lifetime analysis schedules the seed/temp FPRs differently
// (0.00761 lands in f4 vs f2); this recovers the target's instruction scheduling.
#pragma opt_lifetimes off
// The sin/cos polynomial is written as the two static inline helpers above; expanding it inline
// loses the target's seed/temp/rad registers (0.00761 in f2, rad in f3). Passing the sin helper's
// first coefficient (`b`) plus `one` and `pi` as arguments is what places the loop-invariant
// constants in the target's callee-saved registers. The preheader assignment order
// (cPi, c0, c6, c360, cTwo, cOne) is load-bearing for the schedule of the pool loads (it is worth
// two instructions); `i = 1` before `lowAlpha = 4` matches the target's init order. Only FPR
// colouring differs: the ten loop-invariant values match except that the compiler's i2f magic and
// cTwo are swapped (target magic=f26/two=f31, ours two=f26/magic=f31), and the inlined polynomial's
// volatile `1.5707964` scratch is in f0 instead of f1, which cascades through the sin/cos branches.
// Across every named/literal subset of the nine constants (2^9), every caller-local declaration and
// assignment permutation (720 each), a 13k-iteration random walk, mwcceppc 1.3-2.7 and every
// optimization-flag combination the magic only ever lands in f22/f23/f24/f25/f31, never f26; the
// fused `fsubs` can only be produced by the compiler's own int->float idiom, so a named double
// cannot steer its register. This is allocator luck; a permuter job is the remaining route.
// NONMATCHING (97.96%): the i2f magic/cTwo f26/f31 swap and the polynomial's f0/f1 scratch.
EffectEntry* effStarPointEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
    s32 lowAlpha;
    EffectEntry* entry;
    StarPointData* data;
    s32 i;
    f32 cPi, c360, c6, c0, cTwo, cOne;

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

    cPi = float_3p1416_804209e4;
    c0 = float_0_804209cc;
    c6 = float_6p2832_804209d0;
    c360 = float_360_804209d4;
    cTwo = float_2_804209f0;
    cOne = float_1_804209e0;

    i = 1;
    lowAlpha = 4;
    data = &data[1];
    for (; i < entry->count; i++, data++) {
        s32 angle = rand() % 360;
        f32 rad = (c6 * (f32)angle) / c360;
        f32 cSin1 = float_0p16605_804209dc;
        f32 cCos1 = float_0p4967_804209f8;

        data->position.x = c0;
        data->position.y = c0;
        data->position.z = c0;

        data->field_0x18 = cTwo * effStarPointSinf(rad, cSin1, cOne, cPi);

        // cos approximation
        data->field_0x1C = cTwo * effStarPointCosf(rad, cCos1, cOne, cPi);

        data->field_0x20 = float_14_804209fc;
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
