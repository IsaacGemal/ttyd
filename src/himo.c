#include "himo.h"
#include "mario/mariost.h"
#include "driver/mapdrv.h"

#include <dolphin/mtx.h>
#include <math.h>

// Stack work area shared with vivihimoDisp; the layout is fixed by the original
// (points at 0x78, knots at 0xA8, tangents at 0xB8 relative to the base).
typedef struct HimoWork {
    Vec pos0;         // 0x00
    Vec pos1;         // 0x0C
    Mtx mtx0;         // 0x18
    Mtx mtx1;         // 0x48
    Vec points[4];    // 0x78
    f32 knots[4];     // 0xA8
    f32 tangents[12]; // 0xB8
} HimoWork;

extern s32 __float_nan[];

extern const f32 float_0p5_8041dec0;
extern const f32 float_0_8041decc;
extern const f32 float_0p1_8041dee8;
extern const f32 float_2p5_8041deec;
extern const f32 float_3p1416_8041ded8;
extern const f32 float_0p25_8041def0;
extern const f32 float_10_8041ded0;
extern const f32 float_0p33333_8041defc;
extern const f32 float_0p2_8041def4;
extern const f32 float_neg1_8041def8;

#define HIMO_FP_NAN 1
#define HIMO_FP_INFINITE 2
#define HIMO_FP_ZERO 3
#define HIMO_FP_NORMAL 4
#define HIMO_FP_SUBNORMAL 5

static inline s32 himo_fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return HIMO_FP_NAN;
            } else {
                return HIMO_FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return HIMO_FP_SUBNORMAL;
            } else {
                return HIMO_FP_ZERO;
            }
            break;
        default:
            return HIMO_FP_NORMAL;
    }
}

static inline f32 himo_sqrtf(f32 x) {
    if (x > float_0_8041decc) {
        f64 guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return *(f32*)__float_nan;
    } else if (himo_fpclassifyf(x) == HIMO_FP_NAN) {
        return *(f32*)__float_nan;
    }
    return x;
}

static inline Vec himo_makeVec(f32 x, f32 y, f32 z) {
    Vec v = (Vec){0.0f, 0.0f, 0.0f};
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

// NONMATCHING (93.03%): logic matches; register allocation differs (the original keeps the constant
// pool base in r31 and the work base in r30, ours rematerializes them) and the int->float / sqrt
// magic constants load from local @-pool symbols instead of `vec3_802f8068`+offset.
void vivihimo(Mtx mtxA, Mtx mtxB, f32 phase, f32 spread, f32 offA, f32 offB) {
    HimoWork work;
    HimoWork* wp = &work;
    Vec axis;
    Vec normal;
    Vec diff;
    Vec* point;
    Vec* points;
    f32 magA;
    f32 magB;
    f32 dist;
    f32 swing;
    s32 i;

    work.pos0.x = mtxA[0][3];
    work.pos0.y = mtxA[1][3] + offA;
    work.pos0.z = mtxA[2][3];
    work.pos1.x = mtxB[0][3];
    work.pos1.y = mtxB[1][3] + offB;
    work.pos1.z = mtxB[2][3];

    magA = float_0p5_8041dec0 * PSVECMag((Vec*)mtxA);
    magB = float_0p5_8041dec0 * PSVECMag((Vec*)((u8*)mtxA + 0x20));
    dist = himo_sqrtf(magA * magA + magB * magB);

    spread = spread * (float_0p5_8041dec0 * dist);
    PSMTXCopy(mtxA, wp->mtx0);
    PSMTXCopy(mtxB, wp->mtx1);

    axis = *(Vec*)mtxA;
    if (float_0_8041decc != axis.x || float_0_8041decc != axis.y || float_0_8041decc != axis.z) {
        PSVECNormalize(&axis, &axis);
        normal = himo_makeVec(-axis.x, -axis.y, -axis.z);
        swing = float_2p5_8041deec * (f32)sin(float_0p1_8041dee8 * ((f32)gp->retraceCount * phase));
        PSVECSubtract(&wp->pos1, &wp->pos0, &diff);

        points = wp->points;
        point = points;
        for (i = 0; i < 4; i++) {
            f32 t = float_10_8041ded0 * (f32)sin(float_3p1416_8041ded8 * (f32)i * float_0p25_8041def0) + swing;
            f32 off;

            if (i == 1) {
                off = spread * (float_0p2_8041def4 * t);
            } else if (i == 2) {
                off = spread * t;
            } else {
                f32 f = float_neg1_8041def8 * (f32)(i & 1);

                off = spread * (f * t);
            }

            point->x = work.pos0.x + float_0p33333_8041defc * (diff.x * (f32)i) + normal.x * off;
            point->y = work.pos0.y + float_0p33333_8041defc * (diff.y * (f32)i) + normal.y * off;
            point->z = work.pos0.z + float_0p33333_8041defc * (diff.z * (f32)i) + normal.z * off;
            point++;
        }

        wp->points[0] = wp->pos0;
        wp->points[3] = wp->pos1;
        spline_maketable(4, (s32)points, (s32*)wp->knots, (s32*)wp->tangents);
        vivihimoDisp(camGetCurNo(), wp);
    }
}
