#include "eff/eff_damage_star.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_number.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "matching.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

// .rodata constants.
const Vec vec3_802f3448 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802f3458 = 4503601774854144.0;
const f64 double_to_int_mask_802f3460 = 4503599627370496.0;
const char str_802F3468[] = "DamageStar";

// .sdata2 constants used by effDamageStarEntry's inlined sin/cos polynomial.
extern const f32 float_0_8041c990;
extern const f32 float_1_8041c9a8;
extern const f32 float_1p5_8041c9e0;
extern const f32 float_30_8041c9ec;
extern const f32 float_6p2832_8041c9f0;
extern const f32 float_360_8041c9f4;
extern const f32 float_0p00761_8041c9f8;
extern const f32 float_0p16605_8041c9fc;
extern const f32 float_3p1416_8041ca00;
extern const f32 float_1p5708_8041ca04;
extern const f32 float_4p7124_8041ca08;
extern const f32 float_0p03705_8041ca0c;
extern const f32 float_0p4967_8041ca10;
extern const f32 float_0p2_8041ca14;

// .sdata2/sbss2 globals used by effDamageStarDisp's colour build.
GXColor unk_80423038;
u32 damageStar_effHermit_0[312] = {
	0x0000FD80, 0x00000008, 0x00000000, 0x0000000C, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00000000, 0x00000040, 0x00000000,
	0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000008, 0xFDC00005, 0x00050000, 0x00600000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00400000, 0x00000000, 0x00400000, 0x00000000, 0x00400000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0012FDE0, 0x00010001, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00000000, 0x00000040,
	0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000019, 0xFDE20000, 0x00000000,
	0x00400000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00400000, 0x00000000, 0x00400000, 0x00000000, 0x00400000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x001EFDE0, 0x00060006, 0x00000000, 0xFFFCFFFC, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00000000,
	0x00000040, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000028, 0xFE400009,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00400000, 0x00000000, 0x00400000, 0x00000000, 0x00400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x0041FE40, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040,
	0x00000000, 0x00000040, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

u32 damageStar_effHermit_1[445] = {
	0x00000000, 0x00000000, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x00000000, 0x01000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00070000, 0x00000000, 0x00070000, 0x00000000, 0x00070000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x000A0000, 0x00000000, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00040004, 0x00000040,
	0x00010001, 0x00000040, 0x00040004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000012, 0x00000000, 0x00000000,
	0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00530000, 0x00000000, 0x00190001, 0x00010000, 0x00530000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00190000, 0x00000000, 0x00000100, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000033, 0xFFFFFFFF,
	0x00000059, 0x00030003, 0x00000033, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000001E, 0x00000000,
	0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0000FFF0, 0xFFF00000, 0x00400000, 0x00000000, 0x0040FFFF, 0xFFFF0000, 0x00400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00280000, 0x00000000, 0x00000100, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000FF0B, 0xFFF5FFF6, 0x00000040,
	0xFFFFFFFF, 0x00000040, 0xFFFFFFFF, 0x00000040, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000002A,
	0x00000000, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xFF730013, 0x00120000, 0x002C0000, 0x00000000, 0x002C0000, 0x00000000,
	0x002C0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x002E0000, 0x00000000, 0x00000100, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000FF7A, 0x00000000,
	0x00000040, 0xFFFFFFFF, 0x00000040, 0xFFFFFFFF, 0x00000040, 0xFFFFFFFF, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000041, 0x00000000, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0xFF7A0000, 0x00000000, 0x0000FFFD, 0x00000000, 0x0000FFFD,
	0x00000000, 0x0000FFFD, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// .sdata
static u8 scale_data[20] = {
	0x32, 0x37, 0x3C, 0x46, 0x50, 0x64, 0x96, 0xDC, 0xF0, 0xC8,
	0x8C, 0x64, 0x8C, 0xA0, 0x8C, 0x96, 0x9B, 0x96, 0x94, 0x96,
};
static u8 color_rotation_data[36] = {
	0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
	0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
	0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

u32 damageStar_effFace_pPlane168_0[11] = {
	0x0004FC00, 0xFC000000, 0x00000200, 0x0400FC00, 0x00000200, 0x02000400, 0x04000000, 0x02000000,
	0xFC000400, 0x00000000, 0x00000000,
};

u32 damageStar_effFaceDt_pPlane168[1] = { (u32)damageStar_effFace_pPlane168_0 };
u32 damageStar_effAnimRST[8] = {
	7, (u32)damageStar_effHermit_0, 0, 0,
	10, (u32)damageStar_effHermit_1, 1, (u32)damageStar_effFaceDt_pPlane168,
};
u32 damageStar_effAnimRSTDt[2] = { 2, (u32)damageStar_effAnimRST };
u32 damageStar_effAnimDt[3] = { 0, 0x42820000, (u32)damageStar_effAnimRSTDt };

// Inline sine/cosine polynomial approximations (the same two helpers evt_sub.c uses; they
// reproduce the target's polynomial instruction sequence).
static inline f32 damageStarSinf(f32 angle) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;
    f32 t, u;

    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) {
            t = angle * angle;
            v = v * t - b;
            v = v * t + one;
            v = v * angle;
        } else {
            t = angle - 1.5707964f;
            u = 1.5707964f - t;
            t = u * u;
            v = v * t - b;
            v = v * t + one;
            v = v * u;
        }
    } else if (angle < 4.712389f) {
        t = angle - 3.1415927f;
        u = t * t;
        v = v * u - b;
        v = v * u + one;
        v = v * t;
        v = -v;
    } else {
        t = angle - 4.712389f;
        u = 1.5707964f - t;
        t = u * u;
        v = v * t - b;
        v = v * t + one;
        v = v * u;
        v = -v;
    }
    return v;
}

static inline f32 damageStarCosf(f32 angle) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;
    f32 t, u;

    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) {
            t = angle * angle;
            v = v * t - b;
            v = v * t + one;
        } else {
            t = angle - 1.5707964f;
            u = 1.5707964f - t;
            t = u * u;
            v = v * t - b;
            v = v * t + one;
            v = -v;
        }
    } else if (angle < 4.712389f) {
        t = angle - 3.1415927f;
        t = t * t;
        v = v * t - b;
        v = v * t + one;
        v = -v;
    } else {
        t = angle - 4.712389f;
        u = 1.5707964f - t;
        t = u * u;
        v = v * t - b;
        v = v * t + one;
    }
    return v;
}

// `opt_propagation off` stops MWCC rematerialising the polynomial coefficients into every
// branch; `opt_lifetimes off` keeps the seed loads (0.00761/0.03705) inside the loop instead of
// hoisting them to the preheader (the envdrv.c/envDisp_FF shape). Both are required: with only one
// the coefficient loads move and the diff count grows.
// The named constant locals below (k6/k360/k15/f4) and their declaration and assignment order
// are load-bearing for MWCC's FPR colouring (the eff_starpoint/eff_gonbaba_breath technique):
// without them the whole preheader constant block shifts several registers. The 0.2 scale is
// written as a literal inside the loop rather than a preheader local so it pools last, which is
// where the target keeps float_0p2; with a preheader `k02` local the pool was displaced at slot
// 0x6c (360/.00761/... vs 0.2/360/...). The .sdata2 pool now matches the target entry for entry.
// The instruction sequence, control flow, stack frame, the loop accumulator (f13), rad (f29), the
// 30-count local (f4), the per-branch 1.5708/4.7124 scratch and the preheader load order all match.
// The loop-invariant constants are all live across the loop, so they form a clique and any FPR
// permutation is a legal colouring; only the colour choice differs. Target value->FPR: 30->f6,
// 0.0->f12, 6.28->f11, 360->f10, .166->f2, 1.0->f1, pi->f9, 1.5->f8, .4967->f3, .2->f5, magic->f0,
// 30-count->f4, seed->f13; ours: 30->f6, 0.0->f1, 6.28->f3, 360->f2, .166->f8, 1.0->f12, pi->f11,
// 1.5->f7, .4967->f0, .2->f5, magic->f9, 30-count->f4, seed->f13. Permuting the outer locals' and
// the helpers' internals' declarations/assignments, parameterising or inlining the polynomial,
// const locals, and opt_common_subs/global_optimizer/scheduling/fp_contract/optimization_level/
// optimize_for_size/pool_data variants all leave >=68 differing instructions (or make it worse);
// it is allocator luck.
// NONMATCHING (97.66%): only the loop-invariant constants' FPR colouring differs (allocator luck; a permuter job should finish it).
#pragma opt_propagation off
#pragma opt_lifetimes off
EffectEntry* effDamageStarEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 speed, f32 angle) {
    EffectEntry* entry;
    DamageStarData* data;
    s32 origCount;
    s32 i;
    f32 k6;
    f32 k360;
    f32 k15;
    f32 f4;

    entry = effEntry();
    origCount = count;
    if (count > 20) {
        count = 20;
    }
    entry->type = str_802F3468;
    entry->count = count + 2;
    data = __memAlloc(HEAP_EFFECT, (count + 2) * sizeof(DamageStarData));
    entry->userdata = data;
    memset(data, 0, (count + 2) * sizeof(DamageStarData));
    entry->callback = effDamageStarMain;
    data->anim = effMayaAnimAlloc(damageStar_effAnimDt);
    effPlayMayaAnim(data->anim);
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->rot = speed;
    data->angle = angle;
    data->field_0x24 = 99;
    data->frame = 0;
    data->field_0x3C = origCount;
    data->field_0x40 = gp->retraceCount;

    f4 = 30.0f - (f32)(count + 2);
    k6 = 6.2831855f;
    k15 = 1.5f;
    k360 = 360.0f;
    i = 1;
    data = &data[1];
    for (; i < count + 2; i++, data++) {
        s32 spiral = i * ((i % 2) * 2 - 1) / 2;
        f32 a = f4 * (f32)spiral + angle;
        f32 rad = (k6 * a) / k360;

        data->angle = a;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;

        data->vx = k15 * (speed * -damageStarSinf(rad));
        data->vy = k15 * (speed * damageStarCosf(rad));

        data->vz = 0.0f;
        data->rot = 0.0f;
        data->alpha = 0xFF;
        data->scaleX = 0.2f;
        data->scaleY = 0.2f;
        data->scaleZ = 0.2f;
    }

    return entry;
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset


void effDamageStarMain(EffectEntry* effect) {
    DamageStarData* data;
    Vec position;
    Vec disp;
    u32 count = 0;
    s32 i;

    data = effect->userdata;

    disp = vec3_802f3448;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;

    effCalcMayaAnim(data->anim);

    if (gp->retraceCount > (u32)data->field_0x40) {
        count = gp->retraceCount - data->field_0x40;
    }
    if (count < 1) {
        count = 1;
    }

    while (count--) {
        data = effect->userdata;
        if (data->field_0x24 < 100) {
            data->field_0x24--;
        }
        data->frame++;
        if (data->field_0x24 < 0) {
            effDeleteMayaAnim(data->anim);
            effDelete(effect);
            return;
        }
        {
            s32 frame = data->frame;

            i = 2;
            data = &data[2];
            for (; i < effect->count; i++, data++) {
                if (frame < 0x14) {
                    f32 v = 0.01f * (f32)scale_data[frame];

                    data->scaleX = data->scaleY = data->scaleZ = v;
                } else if (frame < 0x28) {
                    data->rot = intplGetValue(0xB, frame - 0x14, 0x13, 0.0f, 720.0f);
                } else if (frame < 0x32) {
                    data->scaleX = intplGetValue(4, frame - 0x28, 9, 1.0f, 1.5f);
                    data->scaleY = intplGetValue(4, frame - 0x28, 9, 1.0f, 0.5f);
                } else if (frame < 0x3C) {
                    data->scaleX = intplGetValue(1, frame - 0x32, 9, 1.5f, 0.1f);
                    data->scaleY = intplGetValue(1, frame - 0x32, 9, 0.5f, 1.0f);
                    data->position.y = intplGetValue(1, frame - 0x32, 9, data->position.y, 10.0f + data->position.y);
                } else {
                    data->scaleX = data->scaleY = data->scaleZ = 0.0f;
                }

                data->vx *= 0.75f;
                data->vy *= 0.75f;
                data->vz *= 0.75f;
                data->position.x += data->vx;
                data->position.y += data->vy;
                data->position.z += data->vz;
            }
        }
    }

    data = effect->userdata;
    data->field_0x40 = gp->retraceCount;
    dispEntry(CAMERA_3D, dispCalcZ(&position), 8, effDamageStarDisp, effect);
}

// Match-only helpers: MWCC allocates the stack slots of these by-value GXColor arguments in the
// order the (inlined) helper bodies appear, so each colour call goes through a helper. The natural
// form is in the #else branches below.
#ifdef MUST_MATCH
static inline void damageStarSetAmbColor(void) {
    GXSetChanAmbColor(4, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
}

static inline void damageStarSendMatColor(GXColor color) {
    GXSetChanMatColor(4, color);
}

static inline void damageStarSetMatColor(s32 idx, u8 alpha) {
    GXColor color = unk_80423038;
    color.r = color_rotation_data[idx * 3 + 0];
    color.g = color_rotation_data[idx * 3 + 1];
    color.b = color_rotation_data[idx * 3 + 2];
    color.a = alpha;
    damageStarSendMatColor(color);
}

static inline void damageStarSendTevColor(GXColor color) {
    GXSetTevColor(1, color);
}

static inline void damageStarSetTevColor(void) {
    damageStarSendTevColor((GXColor){ 0xFF, 0xFF, 0x00, 0xFF });
}
#endif

// The int->float conversion registers match; the conversion's magic double still references the
// compiler's anonymous @345 copy rather than double_to_int_802f3458, but objdiff resolves data by
// section offset so those two diffs do not score.
// The GXColor stack temp slots match only with the match-only helpers above selected by MUST_MATCH.
void effDamageStarDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    DamageStarData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx298;
    Mtx mtx268;
    Mtx mtx238;
    Mtx mtx208;
    Mtx mtx1D8;
    Mtx mtx1A8;
    Mtx mtx178;
    Mtx mtx148;
    Mtx mtx118;
    Mtx mtxE8;
    Mtx mtxB8;
    Mtx mtx88;
    Mtx mtx58;
    s32 flag;
    f32 angle;
    DamageStarData* elem;
    s32 i;
    s32 frame;
    s32 count;
    s32 type;
    char buf[32];

    cam = camGetPtr(camId);
    data = effect->userdata;
    angle = data->angle;
    frame = data->frame;
    count = data->field_0x3C;
    if (angle > 0.0f) {
        flag = 1;
    } else {
        flag = 0;
    }
    type = data->type;

    PSMTXRotRad(mtx268, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
    PSMTXTrans(mtx208, data->position.x, data->position.y, data->position.z);
    PSMTXScale(mtx238, 0.8f, 0.8f, 0.8f);
    PSMTXConcat(mtx208, mtx268, mtx298);
    PSMTXConcat(mtx298, mtx238, mtx298);

    effGetTexObj(0, &texObj);
    GXLoadTexObj(&texObj, 0);
    effGetTexObj(1, &texObj);
    GXLoadTexObj(&texObj, 1);
    effGetTexObj(2, &texObj);
    GXLoadTexObj(&texObj, 2);
    effGetTexObj(3, &texObj);
    GXLoadTexObj(&texObj, 3);

    GXSetNumTexGens(2);
    GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
    GXSetTexCoordGen2(1, 1, 4, 60, 0, 125);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
#ifdef MUST_MATCH
    damageStarSetAmbColor();
#else
    GXSetChanAmbColor(4, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
#endif
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    elem = &((DamageStarData*)effect->userdata)[2];
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 1, 2, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xA, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 5, 4, 7);

    if (type == 1) {
        effGetTexObj(0x15, &texObj);
        GXLoadTexObj(&texObj, 2);
    }

    for (i = 2; i < effect->count; i++, elem++) {
        s32 idx;

#ifdef MUST_MATCH
        idx = (i + frame) % 12;
        damageStarSetMatColor(idx, elem->alpha);
#else
        color = unk_80423038;
        idx = (i + frame) % 12;
        color.r = color_rotation_data[idx * 3 + 0];
        color.g = color_rotation_data[idx * 3 + 1];
        color.b = color_rotation_data[idx * 3 + 2];
        color.a = elem->alpha;
        GXSetChanMatColor(4, color);
#endif

        PSMTXIdentity(mtx1D8);
        PSMTXScaleApply(mtx1D8, mtx1D8, elem->scaleX, elem->scaleY, elem->scaleZ);
        PSMTXTransApply(mtx1D8, mtx1D8, elem->position.x, elem->position.y, elem->position.z);
        PSMTXRotRad(mtx1A8, 'y', MTXDegToRad(elem->rot));
        PSMTXConcat(mtx1D8, mtx1A8, mtx1D8);
        PSMTXScale(mtx178, 0.8f, 0.8f, 0.8f);
        PSMTXConcat(mtx1D8, mtx178, mtx1D8);
        PSMTXConcat(mtx298, mtx1D8, mtx178);
        PSMTXConcat(camGetPtr(camId)->view, mtx178, mtx178);
        GXLoadPosMtxImm(mtx178, 0);
        GXSetCurrentMtx(0);

        GXSetCullMode(2);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-8.0f, 8.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(8.0f, 8.0f, 0.0f);
        GXTexCoord2f32(2.0f, 0.0f);
        GXPosition3f32(8.0f, -8.0f, 0.0f);
        GXTexCoord2f32(2.0f, 1.0f);
        GXPosition3f32(-8.0f, -8.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        GXSetCullMode(1);
        GXSetTevColorOp(0, 0, 0, 3, 1, 0);
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-8.0f, 8.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(8.0f, 8.0f, 0.0f);
        GXTexCoord2f32(2.0f, 0.0f);
        GXPosition3f32(8.0f, -8.0f, 0.0f);
        GXTexCoord2f32(2.0f, 1.0f);
        GXPosition3f32(-8.0f, -8.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }

    data = effect->userdata;
    PSMTXRotRad(mtxB8, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
    PSMTXTrans(mtx88, data->position.x, data->position.y, data->position.z);
    PSMTXConcat(mtx88, mtxB8, mtx148);
    if (flag == 0) {
        PSMTXScale(mtx58, 12.0f, 12.0f, 12.0f);
        PSMTXConcat(mtx148, mtx58, mtx148);
    } else {
        PSMTXScale(mtx58, -12.0f, 12.0f, 12.0f);
        PSMTXConcat(mtx148, mtx58, mtx148);
    }
    PSMTXTrans(mtx88, 10.0f, -1.5f, 0.0f);
    PSMTXConcat(mtx148, mtx88, mtx148);
    {
        Mtx* animMtx = (Mtx*)((u8*)data->anim + 0x14);

        PSMTXConcat(mtx148, *animMtx, mtx148);
        animMtx = (Mtx*)((u8*)data->anim + 0x48);
        PSMTXConcat(mtx148, *animMtx, mtx148);
    }
    GXSetCurrentMtx(0);
    PSMTXTransApply(mtx148, mtx118, 2.5f, -2.5f, 0.0f);
    PSMTXConcat(cam->view, mtx118, mtx118);
    GXLoadPosMtxImm(mtx118, 0);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 1, 3, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetCullMode(0);
    if (type == 1) {
        effGetTexObj(0x27, &texObj);
        GXLoadTexObj(&texObj, 3);
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(2.0f, 0.0f);
        GXPosition3f32(2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(2.0f, 1.0f);
        GXPosition3f32(-2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    } else {
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(2.0f, 0.0f);
        GXPosition3f32(2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(2.0f, 1.0f);
        GXPosition3f32(-2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }

    PSMTXConcat(cam->view, mtx148, mtx118);
    GXLoadPosMtxImm(mtx118, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 1, 2, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
#ifdef MUST_MATCH
    damageStarSetTevColor();
#else
    GXSetTevColor(1, (GXColor){0xFF, 0xFF, 0x00, 0xFF});
#endif
    if (type == 1) {
        effGetTexObj(0x13, &texObj);
        GXLoadTexObj(&texObj, 2);
        GXSetTevOp(0, 3);
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    } else {
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(2.0f, 2.0f, 0.0f);
        GXTexCoord2f32(2.0f, 0.0f);
        GXPosition3f32(2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(2.0f, 1.0f);
        GXPosition3f32(-2.0f, -2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }

    mtxGetRotationElement(mtx148, mtxE8, 'y', 'x');
    PSMTXInverse(mtxE8, mtxE8);
    PSMTXConcat(mtx148, mtxE8, mtx148);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    sprintf(buf, "%d", count);
    {
        s32 len = strlen(buf);
#ifdef MUST_MATCH
        // Naming 0.5f keeps MWCC's int->float magic double in f4 (matches retail);
        // with the literal it lands in f1 and six fsubs/fmuls registers differ.
        f32 f05 = 0.5f;
        f32 x = 24.0f * (f32)(-(len - 1)) * f05;
#else
        f32 x = 24.0f * (f32)(-(len - 1)) * 0.5f;
#endif

        PSMTXScale(mtx58, 0.04f, 0.04f, 0.04f);
        PSMTXTrans(mtx88, x, 0.0f, 0.0f);
        PSMTXConcat(mtx58, mtx88, mtx88);
        PSMTXConcat(mtx148, mtx88, mtx118);
        effNumberGX(&mtx118, 2, count, 0xFF, 1);
    }
}
