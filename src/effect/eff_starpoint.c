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

static inline f32 effStarPointSinf(f32 rad, f32 b, f32 one, f32 pi) {
    f32 v = MATCH_F32(float_0p00761_804209d8);

    if (rad <= pi) {
        if (rad <= float_1p5708_804209e8) {
            f32 t = rad * rad;
            v = v * t - b;
            v = v * t + one;
            v = v * rad;
        } else {
            f32 t = rad - float_1p5708_804209e8;
            f32 u = float_1p5708_804209e8 - t;
            f32 s = u * u;
            v = v * s - b;
            v = v * s + one;
            v = v * u;
        }
    } else if (rad < float_4p7124_804209ec) {
        f32 t = rad - pi;
        f32 u = t * t;
        v = v * u - b;
        v = v * u + one;
        v = v * t;
        v = -v;
    } else {
        f32 t = rad - float_4p7124_804209ec;
        f32 u = float_1p5708_804209e8 - t;
        f32 s = u * u;
        v = v * s - b;
        v = v * s + one;
        v = v * u;
        v = -v;
    }
    return v;
}

static inline f32 effStarPointCosf(f32 rad, f32 b, f32 one, f32 pi) {
    f32 v = MATCH_F32(float_0p03705_804209f4);

    if (rad <= pi) {
        if (rad <= float_1p5708_804209e8) {
            f32 t = rad * rad;
            v = v * t - b;
            v = v * t + one;
        } else {
            f32 t = rad - float_1p5708_804209e8;
            f32 u = float_1p5708_804209e8 - t;
            f32 s = u * u;
            v = v * s - b;
            v = v * s + one;
            v = -v;
        }
    } else if (rad < float_4p7124_804209ec) {
        f32 t = rad - pi;
        f32 s = t * t;
        v = v * s - b;
        v = v * s + one;
        v = -v;
    } else {
        f32 t = rad - float_4p7124_804209ec;
        f32 u = float_1p5708_804209e8 - t;
        f32 s = u * u;
        v = v * s - b;
        v = v * s + one;
    }
    return v;
}

// opt_lifetimes off: the default lifetime analysis schedules the seed/temp FPRs differently
// (0.00761 lands in f3 vs f2, rad in f0 vs f3); this recovers the target's instruction scheduling.
#pragma opt_lifetimes off
// The sin/cos polynomial is written as the two static inline helpers above. Block-scoping each
// branch's transformed-angle and square temps is load-bearing: declared at helper scope the
// polynomial's volatile f0/f1 work registers came out swapped in all four branches, while a
// per-branch scope reproduces the target's register for every one of them. Passing the helper's
// first coefficient (`b`) plus `one` and `pi` as arguments places the loop-invariant constants in
// the target's callee-saved registers; the preheader assignment order and `i = 1` before
// `lowAlpha = 4` match the target's init order.
// NONMATCHING (99.25%): the compiler's own int->float magic double (double_to_int_802fdbf8) and
// the named cTwo (2.0f) hold swapped callee-saved FPRs (target magic=f26/cTwo=f31, ours
// magic=f31/cTwo=f26). The replay model shows the whole gap is exactly this vreg swap: moving the
// magic's sweep slot between cOne and c0 and cTwo's above cPi reproduces every target register.
// The magic is a backend-materialised web, so a named local cannot outrank it. Rewriting the
// conversion with the unit's manual-union idiom (win_log.c) does reference a named double, but MWCC
// emits fsub+frsp instead of the fused fsubs the original has, and every arrangement that puts the
// f64 local below the constants makes mwcceppc loop forever. Making the constants literals lets
// their temps interleave with the magic by first-use order and pushes it all the way down to f28
// (cPi/c0/c360 land above it), but cTwo then drops to f22-f26 instead of rising to f31 and the
// exact order never co-occurs. Allocator luck; a permuter route.
EffectEntry* effStarPointEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
    s32 lowAlpha;
    EffectEntry* entry;
    StarPointData* data;
    s32 i;
    f32 cPi, c360, c6, c0, cTwo, cOne;

    entry = effEntry();
    entry->type = "StarPoint";
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

// The named .sdata2 objects, defined after the code so they are emitted before the function
// literal pool in the original's order.
const f32 float_deg2rad_804209c8 = 0.017453292f;
const f32 float_0_804209cc = 0.0f;
const f32 float_6p2832_804209d0 = 6.2831855f;
const f32 float_360_804209d4 = 360.0f;
const f32 float_0p00761_804209d8 = 0.00761f;
const f32 float_0p16605_804209dc = 0.16605f;
const f32 float_1_804209e0 = 1.0f;
const f32 float_3p1416_804209e4 = 3.1415927f;
const f32 float_1p5708_804209e8 = 1.5707964f;
const f32 float_4p7124_804209ec = 4.712389f;
const f32 float_2_804209f0 = 2.0f;
const f32 float_0p03705_804209f4 = 0.03705f;
const f32 float_0p4967_804209f8 = 0.4967f;
const f32 float_14_804209fc = 14.0f;
