#include "himo.h"
#include "matching.h"
#include "mario/mariost.h"
#include "driver/mapdrv.h"

#include <dolphin/mtx.h>
#include "math_ext.h"

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
extern const f32 float_0p066667_8041dec4;
extern const f32 float_neg1p2881_8041dec8;
extern const f32 float_1_8041ded4;
extern const f32 float_0p82_8041dedc;
extern const f32 float_1p2881_8041dee0;
extern const f32 float_15_8041dee4;
extern const f64 double_1p2881_802f8078;

u8 Pos[] = {0x01, 0x00, 0x00, 0xFF, 0x00, 0x00};
u8 Nrm[] = {0x00, 0x00, 0x7F};
u8 Col[] = {0xFF, 0xFF, 0xFF, 0xFF};

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

// Constant pool base: MWCC addresses the file's .rodata through vec3_802f8068 in r31 (see the
// .rodata block in the original asm); math_ext.h avoids math.h's weak sqrtf/sqrt doubles and
// double_1p2881_802f8078 is defined after the code so the pool lands in the original's order.
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
        points = wp->points;
        normal = himo_makeVec(-axis.x, -axis.y, -axis.z);
        swing = float_2p5_8041deec * (f32)sin(float_0p1_8041dee8 * ((f32)gp->retraceCount * phase));
        PSVECSubtract(&wp->pos1, &wp->pos0, &diff);

        point = points;
        for (i = 0; i < 4; i++) {
            f32 ang = float_3p1416_8041ded8 * (f32)i;
            f32 t = float_10_8041ded0 * (f32)sin(ang * float_0p25_8041def0) + swing;
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

        points[0] = work.pos0;
        points[3] = work.pos1;
        spline_maketable(4, (s32)points, (s32*)wp->knots, (s32*)wp->tangents);
        vivihimoDisp(camGetCurNo(), wp);
    }
}

// Register allocation is the whole game here. The target keeps the seven loop-invariant floats in
// f21-f31, with the group (a,inv,t,v0) in f23-f26 and v in f20. Declaring local copies of the six
// pool constants in the order below and assigning them just before the loop makes MWCC rotate them
// the same way; under #else the globals are read directly through the HIMO_* macros. The int->float
// magic is a local @-pool entry here, not double_to_int_802f8080: its .rodata offset is fixed by
// himo_sqrtf's 0.5/3.0 literals (weak _half/_three pool entries), which are emitted before this
// function's conversion constant, so the relocation lands at 0x38 not 0x18.
#ifdef MUST_MATCH
#define HIMO_C1 c1
#define HIMO_C10 c10
#define HIMO_CPI cpi
#define HIMO_C082 c082
#define HIMO_C12881 c12881
#else
#define HIMO_C1 float_1_8041ded4
#define HIMO_C10 float_10_8041ded0
#define HIMO_CPI float_3p1416_8041ded8
#define HIMO_C082 float_0p82_8041dedc
#define HIMO_C12881 float_1p2881_8041dee0
#endif
void vivihimoDisp(CameraId cameraId, void* data) {
    HimoWork* work = data;
    CameraEntry* cam;
#ifdef MUST_MATCH
    s32 points = (s32)work->points;
    s32 knots = (s32)work->knots;
    s32 tangents = (s32)work->tangents;
#else
    Vec* points = work->points;
    f32* knots = work->knots;
    f32* tangents = work->tangents;
#endif
    s32 i;
#ifdef MUST_MATCH
    f32 c12881;
    f32 c082;
    f32 cpi;
    f32 c10;
    f32 c1;
    f32 c15;
#endif
    Mtx mtxPrev;
    Mtx mtxCur;
    Mtx mtx;
    Vec prevPos;
    Vec pos;
    Mtx* prev;
    Mtx* cur;
    Mtx* mtxp;
    f32 a;
    f32 inv;
    f32 t;
    f32 v0;
    f32 s;
    f32 v;

    cam = camGetPtr(cameraId);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
    GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 7);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetArray(GX_VA_POS, Pos, 3);
    GXSetArray(GX_VA_NRM, Nrm, 3);
    GXSetArray(GX_VA_CLR0, Col, 4);

    a = (f32)tan(double_1p2881_802f8078);
    t = float_0p066667_8041dec4;
    inv = float_0p5_8041dec0 / a;
    v0 = inv * (a + (f32)tan(float_neg1p2881_8041dec8));

    prev = &mtxPrev;
    cur = &mtxCur;
#ifdef MUST_MATCH
    spline_getvalue((f32*)&prevPos, 4, points, knots, tangents, float_0_8041decc);
#else
    spline_getvalue((f32*)&prevPos, 4, (s32)points, (s32)knots, (s32)tangents, float_0_8041decc);
#endif
    mtx[0][0] = float_10_8041ded0 * (float_1_8041ded4 * work->mtx0[0][0] + float_0_8041decc * work->mtx1[0][0]);
    mtx[0][1] = float_1_8041ded4 * work->mtx0[0][1] + float_0_8041decc * work->mtx1[0][1];
    mtx[0][2] = float_1_8041ded4 * work->mtx0[0][2] + float_0_8041decc * work->mtx1[0][2];
    mtx[0][3] = prevPos.x;
    mtx[1][0] = float_10_8041ded0 * (float_1_8041ded4 * work->mtx0[1][0] + float_0_8041decc * work->mtx1[1][0]);
    mtx[1][1] = float_1_8041ded4 * work->mtx0[1][1] + float_0_8041decc * work->mtx1[1][1];
    mtx[1][2] = float_1_8041ded4 * work->mtx0[1][2] + float_0_8041decc * work->mtx1[1][2];
    mtx[1][3] = prevPos.y;
    mtx[2][0] = float_10_8041ded0 * (float_1_8041ded4 * work->mtx0[2][0] + float_0_8041decc * work->mtx1[2][0]);
    mtx[2][1] = float_1_8041ded4 * work->mtx0[2][1] + float_0_8041decc * work->mtx1[2][1];
    mtx[2][2] = float_1_8041ded4 * work->mtx0[2][2] + float_0_8041decc * work->mtx1[2][2];
    mtx[2][3] = prevPos.z;
    PSMTXConcat(cam->view, mtx, *prev);

#ifdef MUST_MATCH
    c1 = float_1_8041ded4;
    c10 = float_10_8041ded0;
    cpi = float_3p1416_8041ded8;
    c082 = float_0p82_8041dedc;
    c12881 = float_1p2881_8041dee0;
    c15 = float_15_8041dee4;
#endif
    mtxp = &mtx;
    for (i = 0; i < 15; i++) {
#ifdef MUST_MATCH
        spline_getvalue((f32*)&pos, 4, points, knots, tangents, t);
#else
        spline_getvalue((f32*)&pos, 4, (s32)points, (s32)knots, (s32)tangents, t);
#endif
        GXLoadPosMtxImm(*prev, 0);
        s = HIMO_C1 - t;
        (*mtxp)[0][0] = HIMO_C10 * (s * work->mtx0[0][0] + t * work->mtx1[0][0]);
        (*mtxp)[0][1] = s * work->mtx0[0][1] + t * work->mtx1[0][1];
        (*mtxp)[0][2] = s * work->mtx0[0][2] + t * work->mtx1[0][2];
        (*mtxp)[0][3] = pos.x;
        (*mtxp)[1][0] = HIMO_C10 * (s * work->mtx0[1][0] + t * work->mtx1[1][0]);
        (*mtxp)[1][1] = s * work->mtx0[1][1] + t * work->mtx1[1][1];
        (*mtxp)[1][2] = s * work->mtx0[1][2] + t * work->mtx1[1][2];
        (*mtxp)[1][3] = pos.y;
        (*mtxp)[2][0] = HIMO_C10 * (s * work->mtx0[2][0] + t * work->mtx1[2][0]);
        (*mtxp)[2][1] = s * work->mtx0[2][1] + t * work->mtx1[2][1];
        (*mtxp)[2][2] = s * work->mtx0[2][2] + t * work->mtx1[2][2];
        (*mtxp)[2][3] = pos.z;
        PSMTXConcat(cam->view, (*mtxp), *cur);
        GXLoadPosMtxImm(*cur, 3);

        v = inv * (a + (f32)tan(HIMO_CPI * (HIMO_C082 * t) - HIMO_C12881));

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXMatrixIndex1u8(0);
        GXPosition1x8(1);
        GXNormal1x8(0);
        GXColor1x8(0);
        GXTexCoord2f32(MATCH_F32(float_0_8041decc), v0);
        GXMatrixIndex1u8(3);
        GXPosition1x8(1);
        GXNormal1x8(0);
        GXColor1x8(0);
        GXTexCoord2f32(MATCH_F32(float_0_8041decc), v);
        GXMatrixIndex1u8(3);
        GXPosition1x8(0);
        GXNormal1x8(0);
        GXColor1x8(0);
        GXTexCoord2f32(MATCH_F32(float_1_8041ded4), v);
        GXMatrixIndex1u8(0);
        GXPosition1x8(0);
        GXNormal1x8(0);
        GXColor1x8(0);
        GXTexCoord2f32(MATCH_F32(float_1_8041ded4), v0);

        v0 = v;
        prevPos = pos;
        {
            Mtx* tmp = prev;
            prev = cur;
            cur = tmp;
        }
        t = (f32)(i + 2) / float_15_8041dee4;
    }
}

#undef HIMO_C1
#undef HIMO_C10
#undef HIMO_CPI
#undef HIMO_C082
#undef HIMO_C12881

const f64 double_1p2881_802f8078 = 1.288053035736084;
