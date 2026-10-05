#include "cam_road.h"
#include "cam_shift.h"
#include "driver/arcdrv.h"
#include "driver/mapdrv.h"
#include "manager/dvdmgr.h"
#include "memory.h"
#include "mario/mariost.h"
#include "system.h"
#include <stdlib.h>
#include <dolphin/os.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>
#include "math_ext.h"

// NOTE: local to cam_road.c; the work struct has no header yet. Offsets listed in REPORT.md.
typedef struct CamRoadState {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ void* unk_24;
    /* 0x28 */ f32 x;
    /* 0x2C */ f32 y;
    /* 0x30 */ f32 z;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 unk_3C;
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ f32 unk_44;
    /* 0x48 */ f32 unk_48;
    /* 0x4C */ f32 unk_4C;
    /* 0x50 */ f32 unk_50;
    /* 0x54 */ u8 pad_54[0x8];
    /* 0x5C */ f32 unk_5C;
} CamRoadState;

typedef struct CamRoadNode {
    /* 0x00 */ u8 pad_00[0x20];
    /* 0x20 */ u32 unk_20;
    /* 0x24 */ u8 pad_24[0x1C];
    /* 0x40 */ s32 unk_40;
    /* 0x44 */ s32 unk_44;
    /* 0x48 */ u8 pad_48[0x34];
    /* 0x7C */ f32 unk_7C;
} CamRoadNode;

typedef struct CamRoadMapEntry {
    /* 0x00 */ char name[0x28];
    /* 0x28 */ s32 unk_28;
    /* 0x2C */ u8 pad_2C[0x8C];
} CamRoadMapEntry;

typedef struct CamRoadMap {
    /* 0x000 */ u8 pad_000[0x44];
    /* 0x044 */ char name[0x84];
    /* 0x0C8 */ s32 count;
    /* 0x0CC */ u8 pad_0CC[0x1C];
    /* 0x0E8 */ u32 unk_E8;
    /* 0x0EC */ CamRoadMapEntry* entries;
    /* 0x0F0 */ u32 unk_F0;
    /* 0x0F4 */ u32 unk_F4;
    /* 0x0F8 */ u32 unk_F8;
    /* 0x0FC */ u32 unk_FC;
    /* 0x100 */ u32 unk_100;
    /* 0x104 */ u32 unk_104;
    /* 0x108 */ u32 unk_108;
} CamRoadMap;

// The 0xB8-byte curve entries of CamRoadMap::entries (CamRoadMapEntry is the same record seen from the name side).
typedef struct CamRoadCurve {
    /* 0x00 */ u8 pad_00[0x20];
    /* 0x20 */ u32 unk_20;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ u32 disabled;
    /* 0x2C */ u8 pad_2C[0x2C];
    /* 0x58 */ f32 unk_58;
    /* 0x5C */ u8 pad_5C[0x8];
    /* 0x64 */ f32 unk_64;
    /* 0x68 */ u8 pad_68[0x8];
    /* 0x70 */ f32 unk_70;
    /* 0x74 */ u8 pad_74[0x8];
    /* 0x7C */ f32 unk_7C;
    /* 0x80 */ u8 pad_80[0xC];
    /* 0x8C */ f32 unk_8C;
    /* 0x90 */ u8 pad_90[0x18];
    /* 0xA8 */ s32 subStart;
    /* 0xAC */ s32 subCount;
    /* 0xB0 */ s32 curveIdx;
    /* 0xB4 */ s32 hasCurve;
} CamRoadCurve;

typedef struct CamRoadSub {
    /* 0x00 */ u8 pad_00[0x58];
    /* 0x58 */ s32 vertStart;
    /* 0x5C */ u8 pad_5C[0x4];
    /* 0x60 */ s32 listStart;
    /* 0x64 */ s32 listCount;
} CamRoadSub;

typedef struct CamRoadList {
    /* 0x0 */ s32 idxStart;
    /* 0x4 */ s32 idxCount;
} CamRoadList;

#define CAMROAD_TIME(st) (*(s64*)((u8*)(st) + 0x50))
#define CAMROAD_BANK(st) (*(f32*)((u8*)(st) + 0x58))

typedef struct CamRoadWork {
    /* 0x000 */ CamRoadMap* map;
    /* 0x004 */ u32 unk_04;
    /* 0x008 */ CamRoadState state0;
    /* 0x068 */ CamRoadState state1;
    /* 0x0C8 */ s32 unk_C8;
    /* 0x0CC */ s32 unk_CC;
    /* 0x0D0 */ f32 unk_D0;
    /* 0x0D4 */ f32 unk_D4;
    /* 0x0D8 */ CamRoadState state2;
    /* 0x138 */ CamRoadState state3;
    /* 0x198 */ s32 unk_198;
    /* 0x19C */ f32 unk_19C;
    /* 0x1A0 */ f32 unk_1A0;
    /* 0x1A4 */ f32 unk_1A4;
    /* 0x1A8 */ f32 unk_1A8;
    /* 0x1AC */ f32 unk_1AC;
    /* 0x1B0 */ f32 unk_1B0;
    /* 0x1B4 */ f32 unk_1B4;
    /* 0x1B8 */ f32 unk_1B8;
    /* 0x1BC */ f32 unk_1BC;
    /* 0x1C0 */ f32 unk_1C0;
    /* 0x1C4 */ f32 unk_1C4;
    /* 0x1C8 */ OSTime unk_1C8;
    /* 0x1D0 */ u32 unk_1D0;
    /* 0x1D4 */ f32 unk_1D4;
    /* 0x1D8 */ f32 unk_1D8;
    /* 0x1DC */ f32 unk_1DC;
    /* 0x1E0 */ f32 unk_1E0;
    /* 0x1E4 */ f32 unk_1E4;
    /* 0x1E8 */ f32 unk_1E8;
    /* 0x1EC */ f32 unk_1EC;
    /* 0x1F0 */ f32 unk_1F0;
    /* 0x1F4 */ f32 unk_1F4;
    /* 0x1F8 */ f32 unk_1F8;
    /* 0x1FC */ f32 unk_1FC;
    /* 0x200 */ f32 unk_200;
    /* 0x204 */ s32 unk_204;
    /* 0x208 */ s32 unk_208;
    /* 0x20C */ u8 pad_20C[0x4];
} CamRoadWork;

//.bss
static CamRoadWork work;

//.sdata
static CamRoadWork* wp = &work;

//.rodata
const Vec vec3_802c12d0 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802c12dc = { 90.0f, 110.0f, 13.0f };
const Vec vec3_802c12e8 = { -110.0f, -70.0f, -190.0f };
const Vec vec3_802c12f4 = { 0.0f, -1.0f, 0.0f };

//.sdata2
extern const f32 float_0p005_80419548;
extern const f32 float_0_8041954c;
extern const f32 float_0p004_80419550;
extern const f32 float_0p008_80419554;
extern const f32 float_1_80419558;
extern const f32 float_2_8041955c;
extern const f32 float_3_80419560;
extern const f32 float_10_80419564;
extern const f32 float_3p1416_80419568;
extern const f32 float_0p5_8041956c;
extern const f32 float_0p5_80419570;
extern const f32 float_6p2832_80419574;
extern const f32 float_3p1416_80419578;
extern const f32 float_neg3p1416_8041957c;
extern const f32 float_rad2deg_80419580;
extern const f32 float_360_80419584;
extern const f32 float_180_80419588;
extern const f32 float_0p01_8041958c;
extern const f32 float_0p3_80419590;
extern const f32 float_90_80419594;
extern const f32 float_20_80419598;
extern const f32 dat_8041959c;
extern const f32 float_neg1E06_804195a0;
extern const f32 float_neg1_804195a4;
extern const f32 dat_804195a8;
extern const f32 float_640_804195ac;
extern const f32 float_480_804195b0;
extern const f32 float_1E06_804195b4;
extern const f32 float_100_804195b8;
extern const f32 float_deg2rad_804195bc;
const char str_804195C0[] = "%s/%s";
const char str_804195C8[] = "1.01";

void camRoadCurveOff(const char* name) {
    CamRoadMapEntry* entry = wp->map->entries;
    s32 i;

    if (wp->unk_1D0 == 0x65) {
        for (i = 0; i < wp->map->count; i++, entry++) {
            if (strcmp(entry->name, name) == 0) {
                entry->unk_28 = 1;
                break;
            }
        }
    }
}

void camRoadCurveOn(const char* name) {
    CamRoadMapEntry* entry = wp->map->entries;
    s32 i;

    if (wp->unk_1D0 == 0x65) {
        for (i = 0; i < wp->map->count; i++, entry++) {
            if (strcmp(entry->name, name) == 0) {
                entry->unk_28 = 0;
                break;
            }
        }
    }
}

void camRoadReset(void) {
    s32 f204 = wp->unk_204;
    void* map = wp->map;
    s32 f1d0 = wp->unk_1D0;

    memset(wp, 0, sizeof(CamRoadWork));
    wp->unk_C8 = 0;
    wp->state2.unk_00 = 0;
    wp->state2.unk_00 = 0;
    wp->unk_19C = float_0p005_80419548;
    wp->unk_1A0 = float_0_8041954c;
    wp->unk_1A4 = float_0p004_80419550;
    wp->unk_1A8 = float_0p008_80419554;
    wp->unk_1AC = float_1_80419558;
    wp->unk_1B0 = float_1_80419558;
    wp->unk_1B4 = float_2_8041955c;
    wp->unk_1B8 = float_1_80419558;
    wp->unk_1BC = float_1_80419558;
    wp->unk_1C0 = float_3_80419560;
    wp->unk_1C4 = float_1_80419558;
    wp->unk_204 = -1;
    wp->unk_208 = 0;
    wp->unk_204 = f204;
    wp->map = map;
    wp->unk_1D0 = f1d0;
}

f32 camRoadGetCurrentShiftXRate(void) {
    if (wp->state1.unk_00 != 0) {
        return wp->state1.unk_5C;
    }
    return 0.0f;
}

CamRoadWork* camRoadGetWorkPtr(void) {
    return wp;
}

// The clamp's literal (see #ifdef below) restores the original's f0 constant / f2 accumulator / f1 result;
// the named `float_1_80419558` instead puts the constant in f2. The extra `w` alias in the final else keeps
// the original's `wp` in r7 across the state-copy loops; without it MWCC reloads wp into r4 and the copies
// shift registers.
void camRoadMain(f32* dst, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank) {
    u32 result;

    wp->unk_1EC = x;
    wp->unk_1F0 = y;
    wp->unk_1F4 = z;
    wp->unk_1F8 = ax;
    wp->unk_1FC = ay;
    wp->unk_200 = az;
    calcCamRoad(&wp->state1, &wp->state2, x / float_10_80419564, y / float_10_80419564,
                z / float_10_80419564, ax / float_10_80419564, ay / float_10_80419564,
                az / float_10_80419564, bank);

    if (wp->unk_C8 != 0) {
        result = compareCamRoad(&wp->state2, &wp->state1, &wp->unk_D4);
        if (result != 0) {
            wp->unk_198 = result;
            wp->unk_C8 = 1;
            wp->unk_1C8 = gp->renderFieldTime;
            wp->unk_D0 = float_0_8041954c;
            wp->state3 = wp->state0;
            wp->state2 = wp->state1;
            if (wp->state2.unk_08 != 0 || wp->state3.unk_08 != 0 ||
                (wp->unk_1D0 == 0x65 &&
                 ((wp->state2.unk_24 != NULL && ((CamRoadNode*)wp->state2.unk_24)->unk_20 != 0) ||
                  (wp->state3.unk_24 != NULL && ((CamRoadNode*)wp->state3.unk_24)->unk_20 != 0))) ||
                (wp->state2.unk_24 != NULL && ((CamRoadNode*)wp->state2.unk_24)->unk_7C != float_0_8041954c) ||
                (wp->state3.unk_24 != NULL && ((CamRoadNode*)wp->state3.unk_24)->unk_7C != float_0_8041954c)) {
                wp->unk_CC = 1;
            } else {
                wp->unk_CC = 0;
            }
        } else {
            wp->state2 = wp->state1;
        }
    }
    if (wp->unk_C8 == 0) {
        result = compareCamRoad(&wp->state1, &wp->state2, &wp->unk_D4);
        if (result != 0) {
            wp->unk_198 = result;
            wp->unk_C8 = 1;
            wp->unk_1C8 = gp->renderFieldTime;
            wp->unk_D0 = float_0_8041954c;
            wp->state3 = wp->state2;
            wp->state2 = wp->state1;
            if (wp->state2.unk_08 != 0 || wp->state3.unk_08 != 0 ||
                (wp->unk_1D0 == 0x65 &&
                 ((wp->state2.unk_24 != NULL && ((CamRoadNode*)wp->state2.unk_24)->unk_20 != 0) ||
                  (wp->state3.unk_24 != NULL && ((CamRoadNode*)wp->state3.unk_24)->unk_20 != 0))) ||
                (wp->state2.unk_24 != NULL && ((CamRoadNode*)wp->state2.unk_24)->unk_7C != float_0_8041954c) ||
                (wp->state3.unk_24 != NULL && ((CamRoadNode*)wp->state3.unk_24)->unk_7C != float_0_8041954c)) {
                wp->unk_CC = 1;
            } else {
                wp->unk_CC = 0;
            }
        }
    }

    if (wp->unk_C8 != 0) {
        wp->unk_D0 += getV60FPS(wp->unk_D4, wp->unk_1C8, gp->renderFieldTime);
#ifdef MUST_MATCH
        // Match-only: the literal makes MWCC keep 1.0 in f0 and the accumulator in f2/f1 like the original;
        // the natural named constant below instead puts the constant in f2.
        if (wp->unk_D0 > 1.0f) {
            wp->unk_D0 = 1.0f;
        }
#else
        if (wp->unk_D0 > float_1_80419558) {
            wp->unk_D0 = float_1_80419558;
        }
#endif
        wp->unk_1C8 = gp->renderFieldTime;
        calcHokanCamRoad(wp->unk_CC, &wp->state3, &wp->state2, wp->unk_D0, &wp->state1, &wp->state0);
        if (wp->unk_CC != 0) {
            wp->state0.unk_08 = 1;
        } else {
            wp->state0.unk_08 = 0;
        }
        if (float_1_80419558 == wp->unk_D0) {
            wp->state0 = wp->state2;
            wp->unk_C8 = 0;
        }
    } else {
#ifdef MUST_MATCH
        // Match-only: the local alias keeps wp in r7 across the copy loops (see the comment above the function).
        CamRoadWork* w = wp;
        w->state2 = w->state1;
        wp->state0 = w->state2;
#else
        wp->state2 = wp->state1;
        wp->state0 = wp->state2;
#endif
    }

    calcCurrentCamState(dst);
    dst[6] *= float_10_80419564;
    dst[7] *= float_10_80419564;
    dst[8] *= float_10_80419564;
    dst[3] *= float_10_80419564;
    dst[4] *= float_10_80419564;
    dst[5] *= float_10_80419564;
}

void calcHokanCamRoad(s32 flag, CamRoadState* a, CamRoadState* b, f32 t, CamRoadState* c, CamRoadState* out) {
    f32 f31;
    f32 f30;
    f32 f6;
    f32 t0;
    f32 t1;
    f32 angle;
    f32 angleB;
    f32 fa;

    f31 = (f32)cos(float_0_8041954c);
    f30 = (f32)cos(float_0_8041954c);
    // temp for the cos argument: the original pools its constants through this temp's
    // register assignment (inlining it back swaps f1/f2 in the f6 fmadds)
    fa = float_3p1416_80419568 * t;
    f6 = (f32)cos(fa * float_1_80419558 + float_0_8041954c);
    t0 = float_2_8041955c / (float_1_80419558 + f31);
    t1 = float_0p5_8041956c * (float_1_80419558 - f30) + f6;
    t1 = t0 * t1;
    f31 = float_0p5_80419570 * (float_2_8041955c - (float_1_80419558 + t1));
    if (f31 > float_1_80419558) {
        f31 = float_1_80419558;
    }
    if (flag != 0) {
        out->x = a->x + f31 * (b->x - a->x);
        out->y = a->y + f31 * (b->y - a->y);
        out->z = a->z + f31 * (b->z - a->z);
    } else {
        out->x = b->x;
        out->y = b->y;
        out->z = b->z;
    }
#ifdef MUST_MATCH
    // the original converts the first atan2 result in a scratch then copies it to angle
    t0 = (f32)atan2(a->unk_34, a->unk_3C);
    angle = t0;
#else
    angle = (f32)atan2(a->unk_34, a->unk_3C);
#endif
    angleB = (f32)atan2(b->unk_34, b->unk_3C);
    while (angle - angleB > 3.1415927f) {
        angle -= 6.2831855f;
    }
    while (angle - angleB < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle = angle + f31 * (angleB - angle);
    out->unk_34 = (f32)sin(angle);
    out->unk_3C = (f32)cos(angle);
    out->unk_38 = a->unk_38 + f31 * (b->unk_38 - a->unk_38);
    out->unk_40 = a->unk_40 + f31 * (b->unk_40 - a->unk_40);
    out->unk_44 = a->unk_44 + f31 * (b->unk_44 - a->unk_44);
    out->unk_48 = a->unk_48 + f31 * (b->unk_48 - a->unk_48);
    out->unk_5C = a->unk_5C + f31 * (b->unk_5C - a->unk_5C);
}

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// NONMATCHING (98.92%): the `maxVal` product lands in f5 instead of f3, the `node`/`range`/`abs(d)`
// GPRs are assigned differently, and the `dist`/sqrtf tail is f3 instead of f6. Every opcode and
// immediate matches; only these compiler-temp register choices differ.
u32 compareCamRoad(CamRoadState* cur, CamRoadState* prev, f32* out) {
    u32 flags = 0;
    CamRoadNode* node;
    s32 d;
    s32 range;
    f32 dot;
    f32 deg;
    f32 maxVal;
    f32 diff;
    f32 dist;
    f32 cand;

    if (prev->unk_00 == 0) {
        return 0;
    }
    if (cur->unk_24 == prev->unk_24) {
        if (!(cur->unk_04 == 1 && prev->unk_04 == 0) && !(cur->unk_04 == 0 && prev->unk_08 == 1)) {
            d = (s32)cur->unk_4C - (s32)prev->unk_4C;
            if (abs(d) <= 1) {
                return 0;
            }
#ifdef MUST_MATCH
            // ordering the cast-expression before the `node` store matches the original's r5/node,
            // r4/range, r3/abs assignment; the natural form is under #else.
            range = ((CamRoadNode*)cur->unk_24)->unk_44 - ((CamRoadNode*)cur->unk_24)->unk_40;
            node = (CamRoadNode*)cur->unk_24;
#else
            node = (CamRoadNode*)cur->unk_24;
            range = node->unk_44 - node->unk_40;
#endif
            if (abs(d) >= range - 1) {
                return 0;
            }
            if (abs(d) >= range - 2) {
                flags |= 0x20;
            }
        }
    }
    if (cur->unk_24 != prev->unk_24) {
        flags |= 0x1;
    }
    if (cur->unk_04 == 1 && prev->unk_04 == 0) {
        flags |= 0x2;
    }
    if (cur->unk_04 == 0 && prev->unk_08 == 1) {
        flags |= 0x4;
    }
#ifdef MUST_MATCH
    // the compiler materialises the negated `<=` here; the natural `> 1.0` emits a plain branch.
    if (!(__fabsf(cur->unk_4C - prev->unk_4C) <= 1.0)) {
#else
    if (__fabsf(cur->unk_4C - prev->unk_4C) > 1.0) {
#endif
        flags |= 0x8;
    }
    dot = cur->unk_34 * prev->unk_34 + cur->unk_3C * prev->unk_3C;
#ifdef MUST_MATCH
    // a literal 1.0f keeps this constant out of the shared `float_1_80419558` slot,
    // which is what lets the dot product land in f1/f0; the named constant does not.
    if (dot > 1.0f) {
        dot = 1.0f;
    }
#else
    if (dot > float_1_80419558) {
        dot = float_1_80419558;
    }
#endif
    deg = (f32)acos(dot) * float_rad2deg_80419580;
    deg = __fabsf(deg);
    if (deg > float_180_80419588) {
        deg = float_360_80419584 - deg;
    }
    maxVal = float_6p2832_80419574 * cur->unk_40;
    maxVal = maxVal * deg / float_360_80419584;
    maxVal = maxVal * wp->unk_1B4;
    diff = __fabsf(cur->unk_44 - prev->unk_44);
    if (diff > float_180_80419588) {
        diff = float_360_80419584 - diff;
    }
    cand = float_6p2832_80419574 * cur->unk_40;
    cand = cand * diff / float_360_80419584;
    cand = cand * wp->unk_1B8;
    if (cand > maxVal) {
        maxVal = cand;
    }
    {
        f32 d2 = __fabsf(cur->unk_48 - prev->unk_48);
        if (d2 > float_180_80419588) {
            d2 = float_360_80419584 - d2;
        }
        cand = float_6p2832_80419574 * cur->unk_40;
        cand = cand * d2 / float_360_80419584;
        cand = cand * wp->unk_1BC;
    }
    if (cand > maxVal) {
        maxVal = cand;
    }
    cand = __fabsf(cur->unk_40 - prev->unk_40);
    cand = cand * wp->unk_1C0;
    if (cand > maxVal) {
        maxVal = cand;
    }
    {
        f32 dx = cur->x - prev->x;
        f32 dy = cur->y - prev->y;
        f32 dz = cur->z - prev->z;
#ifdef MUST_MATCH
        // separate square temps stop the compiler fusing the sum into fmadds; the original keeps the
        // x == 0 short-circuit path and the accumulator's register (f6) through the sqrtf, which the
        // natural single expression below does not.
        f32 dxx = dx * dx;
        f32 dyy = dy * dy;
        f32 dzz = dz * dz;
        f32 d2;

        d2 = dxx;
        d2 = d2 + dyy;
        d2 = d2 + dzz;
        if (d2) {
            d2 = sqrtf(d2);
        } else {
            d2 = 0.0f;
        }
        dist = d2;
#else
        dist = sqrtf(dx * dx + dy * dy + dz * dz);
#endif
    }
    if (maxVal < float_0p01_8041958c && dist < float_0p3_80419590) {
        return 0;
    }
    cand = dist * wp->unk_1C4;
    if (cand > maxVal) {
        maxVal = cand;
    }
    if (maxVal > float_90_80419594) {
        maxVal = float_90_80419594;
    }
    if (maxVal < float_20_80419598) {
        maxVal = float_20_80419598;
    }
    *out = float_1_80419558 / maxVal;
    if (flags == 0) {
        flags |= 0x10;
    }
    return flags;
}


// Finds the camera road under the player (ray tests down the road triangles, the curve collision) and eases the
// look-at point along it. The seventh float arg (f7) is the bank.
// collisionCurve/collisionTri_simple take their Vec arguments by value (the caller makes the stack copies).
// t1/t2 start as struct copies of pos (the original copies words, then overwrites y).
// NONMATCHING (81.88%): 19 calls and every immediate identical; the original keeps x..az in f16..f21 (one FPR higher
// than here) and spills `b` to the stack, and the doubles load through the unported rodata base.
void calcCamRoad(CamRoadState* a, CamRoadState* b, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank) {
    s32 m;
    s32* idxp;
    s32 k;
    s32 found;
    CamRoadCurve* entry = (CamRoadCurve*)wp->map->entries;
    CamRoadCurve* hit = NULL;
    s32 subCount;
    CamRoadSub* sub;
    Vec* verts;
    s32 n;
    CamRoadList* list;
    s32 j;
    s32* idx;
    s32 i;
    CamRoadCurve* cur;
    s32 flags;
    s32 found2;
    f32 tmin = dat_8041959c;
    Vec pos;
    Vec ang;
    Vec q;
    CollisionCurveHit h1;
    CollisionCurveHit h2;
    CollisionCurveHit h3;
    f32 f30;
    f32 f29;
    f32 nzv;
    f32 px;
    f32 py;
    f32 pz;
    f32 f24;
    f32 rad;
    f32 f31;
    f32 s1B0;
    f32 s1AC;
    f32 s1A8;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    ang.x = ax;
    ang.y = ay;
    ang.z = az;

    for (i = 0; i < wp->map->count; i++, entry++) {
        subCount = entry->subCount;
        sub = (CamRoadSub*)wp->map->unk_F0 + entry->subStart;
        if (wp->unk_1D0 != 0x65 || entry->disabled == 0) {
            if (subCount == 0) {
                f32 top = entry->unk_8C;

                if (!(top > ay) && top > tmin) {
                    tmin = top;
                    hit = entry;
                }
            } else {
                for (k = 0; k < subCount; k++, sub++) {
                    CamRoadMap* map = wp->map;

                    verts = (Vec*)map->unk_FC + sub->vertStart;
                    n = sub->listCount;
                    list = (CamRoadList*)map->unk_F4 + sub->listStart;
                    for (j = 0; j < n; j++, list++) {
                        idx = (s32*)wp->map->unk_100 + list->idxStart;
                        idxp = idx + 1;
                        m = list->idxCount - 1;
                        for (found = 1; found < m; found++, idxp++) {
                            if (collisionTri_simple(ang, verts[idx[0]], verts[idxp[0]], verts[idxp[1]], &tmin)) {
                                hit = entry;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (hit != NULL) {
        h1.unk_08 = float_neg1E06_804195a0;
        h1.unk_18 = float_0_8041954c;
        h1.unk_1C = float_0_8041954c;
        h1.unk_20 = float_0_8041954c;
        if (!collisionCurve(0, (CollisionCurveData*)hit, pos, &h1)) {
            hit = NULL;
        }
    }

    cur = hit;
    if (hit == NULL) {
        if (b->unk_00 != 0) {
            cur = (CamRoadCurve*)b->unk_24;
        } else {
            cur = NULL;
        }
        if (cur == NULL) {
            wp->unk_1D4 = x;
            wp->unk_1D8 = y;
            wp->unk_1DC = z;
            wp->unk_1E0 = ax;
            wp->unk_1E4 = ay;
            wp->unk_1E8 = az;
            return;
        }
        px = x;
        if (wp->unk_1D0 == 0x65 && cur != NULL && cur->unk_20 != 0) {
            py = cur->unk_24;
        } else {
            py = y;
        }
        pz = z;
        flags = 0;
        f29 = -b->unk_3C;
        f31 = -b->unk_34;
        s1B0 = b->unk_44;
        f30 = -b->unk_38;
        f24 = b->unk_40;
        s1AC = b->unk_48;
        s1A8 = b->unk_4C;
    } else {
        f32 nx;
        f32 nz;
        f32 len;

        if (h1.unk_00 < float_0_8041954c) {
            h1.unk_00 = float_0_8041954c;
        }
        if (h1.unk_00 > float_1_80419558) {
            h1.unk_00 = float_1_80419558;
        }
        s1B0 = cur->unk_64;
        f24 = cur->unk_58;
        s1AC = cur->unk_70;
        px = h1.unk_0C;
        if (wp->unk_1D0 == 0x65 && cur->unk_20 != 0) {
            py = cur->unk_24;
        } else {
            py = y;
        }
        nz = -h1.unk_20;
        nx = h1.unk_18;
        len = nz * nz + h1.unk_18 * h1.unk_18;
        pz = h1.unk_14;
        flags = h1.unk_28;
        s1A8 = (f32)h1.unk_24;
        f30 = h1.unk_1C;
        if (len != 0.0) {
            len = sqrtf(len);
            nz /= len;
            f30 = float_0_8041954c;
            nx /= len;
        }
        f31 = nz * float_neg1_804195a4;
        f30 = f30 * float_neg1_804195a4;
        f29 = nx * float_neg1_804195a4;
    }

    if (cur->hasCurve != 0) {
        q.x = px;
        q.y = dat_804195a8;
        q.z = pz;
        h1.unk_08 = float_neg1E06_804195a0;
        if (collisionCurve(1, (CollisionCurveData*)((CamRoadSub*)wp->map->unk_104 + cur->curveIdx), q, &h1)) {
            px = h1.unk_0C;
            pz = h1.unk_14;
            flags |= h1.unk_28;
        }
    }

    if (cur != NULL) {
        rad = float_3p1416_80419568 * ((*(f32*)wp->map->unk_E8 * float_0p5_8041956c) / float_180_80419588);

        if (bank != float_0_8041954c && hit != NULL && b->unk_00 != 0) {
            f32 c1;
            f32 s1;
            f32 k1;
            f32 k2;
            f32 bk;
            f32 m1;
            f32 m2;
            f32 lim1;
            f32 lim2;
            f32 vel1;
            f32 vel2;
            f32 hx;
            f32 hz;
            f32 pp;
            f32 qq;
            f32 sgnq;
            f32 sgnp;
            f32 fx;
            f32 fz;
            Vec dA;
            Vec dB;
            Vec u;
            Vec w;
            Vec w1;
            Vec w2;
            Vec t1;
            Vec t2;
            f32 gx;
            f32 gz;

            c1 = (f32)cos(rad);
            s1 = (f32)sin(rad);
            nzv = -f29;
            t1 = pos;
            bk = CAMROAD_BANK(b);
            k1 = f24 * s1;
            k1 = k1 / c1;
            k1 = float_640_804195ac * k1;
            k1 = k1 / float_480_804195b0;
            h2.unk_08 = float_neg1E06_804195a0;
            h2.unk_18 = float_0_8041954c;
            t1.y = float_1E06_804195b4;
            h2.unk_1C = float_0_8041954c;
            h2.unk_20 = float_0_8041954c;
            k2 = float_0p01_8041958c * (bk * (float_1_80419558 * k1));
            t1.x = t1.x + k2 * nzv;
            t1.z = t1.z + k2 * f31;
            found = 0;
            if (collisionCurve(0, (CollisionCurveData*)cur, t1, &h2)) {
                found = 1;
                t1.x = h2.unk_0C;
                t1.y = py;
                t1.z = h2.unk_14;
            }
            if (cur->hasCurve != 0) {
                h2.unk_08 = float_neg1E06_804195a0;
                if (collisionCurve(1, (CollisionCurveData*)((CamRoadSub*)wp->map->unk_104 + cur->curveIdx), t1, &h2)) {
                    found = 1;
                    t1.x = h2.unk_0C;
                    t1.y = py;
                    t1.z = h2.unk_14;
                }
            }
            if (found != 0) {
                gx = t1.x;
                gz = t1.z;
            } else {
                gx = px;
                gz = pz;
            }

            t2 = pos;
            found2 = 0;
            flags = 0;
            k1 = (float_640_804195ac * ((f24 * (f32)sin(rad)) / (f32)cos(rad))) / float_480_804195b0;
            h3.unk_08 = float_neg1E06_804195a0;
            h3.unk_18 = float_0_8041954c;
            h3.unk_1C = float_0_8041954c;
            h3.unk_20 = float_0_8041954c;
            t2.y = float_1E06_804195b4;
            k2 = float_0p01_8041958c * (bank * (float_1_80419558 * k1));
            t2.x = t2.x + k2 * nzv;
            t2.z = t2.z + k2 * f31;
            if (collisionCurve(0, (CollisionCurveData*)cur, t2, &h3)) {
                found2 = 1;
                flags = h3.unk_28;
                t2.x = h3.unk_0C;
                t2.y = py;
                t2.z = h3.unk_14;
            }
            if (cur->hasCurve != 0) {
                h3.unk_08 = float_neg1E06_804195a0;
                if (collisionCurve(1, (CollisionCurveData*)((CamRoadSub*)wp->map->unk_104 + cur->curveIdx), t2, &h3)) {
                    found2 = 1;
                    t2.x = h3.unk_0C;
                    flags |= h3.unk_28;
                    t2.z = h3.unk_14;
                }
            }
            if (found2 != 0) {
                hx = t2.x;
                hz = t2.z;
                if (!(h3.unk_28 & 1) && (b->unk_08 & 1)) {
                    wp->unk_1AC = float_0_8041954c;
                }
                if (!(h3.unk_28 & 2) && (b->unk_08 & 2)) {
                    wp->unk_1B0 = float_0_8041954c;
                }
            } else {
                hx = px;
                hz = pz;
            }

            dA.x = gx - b->x;
            dA.y = float_0_8041954c;
            dA.z = gz - b->z;
            dB.x = hx - b->x;
            dB.y = float_0_8041954c;
            dB.z = hz - b->z;
            m1 = PSVECMag(&dA);
            PSVECMag(&dB);
            m1 = m1 + getV60FPS(wp->unk_19C, CAMROAD_TIME(b), gp->renderFieldTime);
            m2 = getV60FPS(wp->unk_1A0, CAMROAD_TIME(b), gp->renderFieldTime);
            lim1 = m1 * wp->unk_1AC;
            lim2 = m1 * wp->unk_1B0;
            vel1 = m2 * wp->unk_1AC;
            vel2 = m2 * wp->unk_1B0;
            u.x = nzv;
            u.y = float_0_8041954c;
            u.z = f31;
            w.x = -f31;
            w.y = float_0_8041954c;
            w.z = nzv;
            w1.y = float_0_8041954c;
            w2.y = float_0_8041954c;
            if (__fabsf(f31) > __fabsf(nzv)) {
                pp = (dB.x - nzv * (dB.z / f31)) / (-f31 - nzv * (nzv / f31));
                qq = (dB.z - pp * nzv) / f31;
            } else {
                pp = (dB.z - f31 * (dB.x / nzv)) / (nzv - f31 * (-f31 / nzv));
                qq = (dB.x - pp * -f31) / nzv;
            }
            w1.x = qq * u.x;
            w1.z = qq * u.z;
            w2.x = pp * w.x;
            w2.z = pp * w.z;
            if (qq < float_0_8041954c) {
                sgnq = float_neg1_804195a4;
            } else {
                sgnq = float_1_80419558;
            }
            if (pp < float_0_8041954c) {
                sgnp = float_neg1_804195a4;
            } else {
                sgnp = float_1_80419558;
            }
            fx = PSVECMag(&w1);
            fz = PSVECMag(&w2);
            if (fx > lim1 && fx > vel1) {
                PSVECScale(&u, &w1, lim1 * sgnq);
            }
            if (fz > lim2 && fz > vel2) {
                PSVECScale(&w, &w2, lim2 * sgnp);
            }
            px = b->x + (w1.x + w2.x);
            pz = b->z + (w1.z + w2.z);
        }
    }

    wp->unk_1AC = wp->unk_1AC + wp->unk_1A4;
    if (wp->unk_1AC > float_1_80419558) {
        wp->unk_1AC = float_1_80419558;
    }
    wp->unk_1B0 = wp->unk_1B0 + wp->unk_1A8;
    if (wp->unk_1B0 > float_1_80419558) {
        wp->unk_1B0 = float_1_80419558;
    }

    a->unk_00 = 1;
    a->unk_04 = hit != NULL;
    a->unk_08 = flags;
    a->unk_0C = x;
    a->unk_10 = y;
    a->unk_14 = z;
    a->unk_18 = ax;
    a->unk_1C = ay;
    a->unk_20 = az;
    a->unk_24 = cur;
    a->x = px;
    a->y = py;
    a->z = pz;
    a->unk_34 = -f31;
    a->unk_38 = -f30;
    a->unk_3C = -f29;
    a->unk_40 = f24;
    a->unk_44 = s1B0;
    a->unk_48 = s1AC;
    a->unk_4C = s1A8;
    CAMROAD_TIME(a) = gp->renderFieldTime;
    CAMROAD_BANK(a) = bank;
    a->unk_5C = cur->unk_7C;
}


// The `if (x != 0.0) x = sqrtf(x);` guards are intentional: removing them drops the pool's
// `fcmpu 0.0,x; beq` skip that the target has. The callee-saved float locals must be declared in
// DESCENDING order (f31..f14): MWCC hands the declaration order back to the FPR allocator, and
// reversing it recovers ~100 FPR operand matches (91.7 -> 92.9).
// The GPR allocation permutes the parameters/locals: target p/out->r16/r17, i18-i24->r18-r24, pool
// base->r31; ours shifts the i's down (i23->r17, i24->r16), p/out->r24/r25 and pool base->r23. The
// scratch FPRs and spill slots (0x18/0x1c vs 0x24/0x28) still differ, and this compiler folds the
// dead `if (f28 < -1e-7)` compare. Declaration order, statement order and the mode-block order were
// searched extensively (f0-before-f1 and the mode0/mode1 reorders recovered 95.90 -> 96.52).
// NONMATCHING (96.84%): register/encoding only; see notes above.
BOOL collisionCurve(s32 mode, CollisionCurveData* data, Vec p, CollisionCurveHit* out) {
    f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f10, f9;
    f32 f31, f30, f29, f28, f27, f26, f25, f24, f23, f22, f21, f20, f19, f18, f17, f16, f15, f14;
    f32 sf18, sf1c, sf20, sf24, sf28, sf2c;
    f64 d0;
    Vec* base;
    s32 i22, i21, i20, i19, i18, i23, i24, half, i0, i15;
    Vec *r25, *r30, *r29, *r6, *r28, *r27, *r26, *r7;

    sf20 = dat_804195a8;
    i24 = 0;
    d0 = 0.0;

    if (mode == 0) {
        i20 = data->unk_84;
        f26 = data->unk_4C;
        i19 = 0;
        half = data->unk_A4 / 2;
        base = (Vec*)wp->map->unk_F8;
        r6 = &base[data->unk_A0];
        f27 = data->unk_48;
        r7 = &base[data->unk_A0 + half];
        i21 = data->unk_44;
        i22 = data->unk_40;
    } else {
        i20 = 0;
        f27 = 0.0f;
        base = (Vec*)wp->map->unk_F8;
        f26 = 0.0f;
        r6 = &base[data->unk_60];
        i21 = 1;
        i22 = 0;
        i19 = 1;
        half = data->unk_64 / 2;
        r7 = &base[data->unk_60 + half];
    }

    sf28 = r7[i22].y;
    sf24 = r7[i21].y;
    if (__fabsf(sf24 - sf28) < 0.00009999999747378752) {
        sf28 = sf24;
    }
    if (i19 != 0) {
        i20 = 0;
    }

    r30 = r6;
    r29 = r7;
    r28 = &r7[i22];
    r27 = &r6[i22];
    r26 = &r7[i21];
    r25 = &r6[i21];
    i15 = half - 1;
    i23 = 0;
    goto L_D160;

L_C4BC:
    f23 = r30[0].z;
    i18 = 0;
    f3 = p.z;
    f0 = f3 - f23;
    f1 = r29[1].x;
    f5 = r29[0].z;
    f25 = r30[0].x;
    f7 = -f1;
    f4 = p.x;
    f0 = f5 * f0;
    f1 = f4 - f25;
    f6 = r29[0].x;
    f2 = r29[1].z;
    f24 = r30[0].y;
    f1 = f6 * f1 + f0;
    f8 = -f2;
    f22 = r30[1].x;
    f21 = r30[1].z;
    if (f1 < d0) goto L_D154;
    f0 = f3 - f21;
    f1 = f4 - f22;
    f0 = f8 * f0;
    f1 = f7 * f1 + f0;
    if (f1 < d0) goto L_D154;

    f2 = -f5;
    f3 = f6;
    f4 = f8;
    f0 = f2 - f8;
    f5 = -f7;
    f0 = __fabsf(f0);
    if (!(f0 < 0.009999999776482582)) goto L_C57C;
    f0 = f6 - f5;
    f0 = __fabsf(f0);
    if (!(f0 < 0.009999999776482582)) goto L_C57C;
    f1 = f2 + f8;
    f2 = float_0p5_8041956c;
    f0 = f6 + f5;
    f4 = f2 * f1;
    f5 = f2 * f0;
    f2 = f4;
    f3 = f5;
L_C57C:
    if (f2 == f4 && f3 == f5) {
        goto L_CD80;
    }

L_C58C:
    f29 = float_1_80419558;
    sf18 = f29;
    if (0.0 == f4) goto L_C5C0;
    f1 = f25 - f22;
    f0 = f2 / f4;
    f1 = f1 / f4;
    f1 = f5 * f1 + f21;
    f0 = f3 - f5 * f0;
    f1 = f1 - f23;
    f0 = f1 / f0;
    goto L_C5EC;
L_C5C0:
    if (0.0 == f5) goto L_C5E8;
    f1 = f23 - f21;
    f0 = f3 / f5;
    f1 = f1 / f5;
    f1 = f4 * f1 + f22;
    f0 = f2 - f4 * f0;
    f1 = f1 - f25;
    f0 = f1 / f0;
    goto L_C5EC;
L_C5E8:
    f0 = 0.0f;
L_C5EC:
    f20 = f0 * f3 + f23;
    f0 = f0 * f2 + f25;
    f15 = f23 - f20;
    sf1c = f0;
    f0 = (f32)f0;
    f16 = f25 - f0;
    f0 = f15 * f15;
    f19 = f16 * f16 + f0;
    if (f19 != 0.0) f19 = sqrtf(f19);
L_C6F8:
    f0 = p.z;
    f2 = p.x;
    f30 = f0 - f20;
    f0 = sf1c;
    f31 = f2 - f0;
    f0 = f30 * f30;
    f17 = f31 * f31 + f0;
    if (f17 != 0.0) f17 = sqrtf(f17);
L_C800:
    f1 = f21 - f20;
    f0 = sf1c;
    f0 = f22 - f0;
    f2 = f1 * f1;
    f18 = f0 * f0 + f2;
    if (f18 != 0.0) f18 = sqrtf(f18);
L_C900:
    f3 = f15 * f30;
    f2 = float_neg1_804195a4;
    f1 = f1 * f30;
    f4 = f19 * f17;
    f5 = f16 * f31 + f3;
    f3 = f0 * f31 + f1;
    f0 = f18 * f17;
    f1 = f5 / f4;
    f14 = f3 / f0;
    if (f1 < f2) {
        f1 = f2;
    } else if (f1 > float_1_80419558) {
        f1 = float_1_80419558;
    }
    if (f14 < f2) {
        f14 = f2;
    } else if (f14 > float_1_80419558) {
        f14 = float_1_80419558;
    }
    f0 = (f32)acos(f1);
    f1 = f14;
    sf2c = f0;
    f3 = (f32)acos(f14);
    f0 = sf2c;
    f4 = f17 - f19;
    f1 = 0.0f;
    f2 = f17 - f18;
    f3 = f0 + f3;
    f2 = f2 - f4;
    f5 = f0 / f3;
    f0 = f5 * f2 + f4;
    if (f0 < f1) {
        f29 = float_neg1_804195a4;
        f0 = -f0;
    }
L_C9B0:
    if (f0 >= sf20) goto L_D154;
    f3 = f21 - f23;
    f4 = r29[0].y;
    f2 = f22 - f25;
    f1 = r29[1].y;
    sf20 = f0;
    f1 = f1 - f4;
    f3 = -f3;
    f2 = f15 * f2;
    f1 = f5 * f1 + f4;
    f3 = f16 * f3 + f2;
    if (i20 == 0) goto L_CC7C;

    if (f1 < sf28) {
        if (f3 > 0.0) {
            f1 = 0.0f;
            if (f29 < f1) {
                if (f0 > f27) {
                    f0 = f27;
                    i18 |= 2;
                }
            } else {
                if (f0 > f26) {
                    f0 = f26;
                    i18 |= 2;
                }
                f0 = -f0;
            }
        } else {
            f1 = 0.0f;
            if (f29 < f1) {
                if (f0 > f26) {
                    f0 = f26;
                    i18 |= 2;
                }
                f0 = -f0;
            } else {
                if (f0 > f27) {
                    f0 = f27;
                    i18 |= 2;
                }
            }
        }
    } else {
        goto L_CAD0;
    }
L_CA74:
    f1 = sf28;
    i0 = i18 | 1;
    f3 = 0.0f;
    i24 = 1;
    out->unk_00 = f1;
    f2 = r28[0].z;
    f1 = r27[0].x;
    out->unk_0C = f0 * f2 + f1;
    out->unk_10 = f3;
    f2 = r28[0].x;
    f1 = r27[0].z;
    out->unk_14 = f1 - f0 * f2;
    f0 = r28[0].x;
    out->unk_18 = f0;
    out->unk_1C = f3;
    f0 = r28[0].z;
    out->unk_20 = f0;
    out->unk_08 = f24;
    out->unk_24 = i22;
    out->unk_28 = i0;
    goto L_D154;

L_CAD0:
    if (f1 > sf24) {
    if (f3 > 0.0) {
        f1 = 0.0f;
        if (f29 < f1) {
            if (f0 > f27) {
                f0 = f27;
                i18 |= 2;
            }
        } else {
            if (f0 > f26) {
                f0 = f26;
                i18 |= 2;
            }
            f0 = -f0;
        }
    } else {
        f1 = 0.0f;
        if (f29 < f1) {
            if (f0 > f26) {
                f0 = f26;
                i18 |= 2;
            }
            f0 = -f0;
        } else {
            if (f0 > f27) {
                f0 = f27;
                i18 |= 2;
            }
        }
    }
L_CB54:
    f1 = sf24;
    i0 = i18 | 1;
    f3 = 0.0f;
    i24 = 1;
    out->unk_00 = f1;
    f2 = r26[0].z;
    f1 = r25[0].x;
    out->unk_0C = f0 * f2 + f1;
    out->unk_10 = f3;
    f2 = r26[0].x;
    f1 = r25[0].z;
    out->unk_14 = f1 - f0 * f2;
    f0 = r26[0].x;
    out->unk_18 = f0;
    out->unk_1C = f3;
    f0 = r26[0].z;
    out->unk_20 = f0;
    out->unk_08 = f24;
    out->unk_24 = i21;
    out->unk_28 = i0;
    goto L_D154;
    }

L_CBB0:
    if (f3 > 0.0) {
    f2 = 0.0f;
    if (f29 < 0.0f) {
    if (f0 > f27) {
    f31 = f31 / f17;
    i18 |= 2;
    f30 = f30 / f17;
    f0 = f0 - f27;
    f17 = f17 + f0;
    f31 = f31 * f17;
    f30 = f30 * f17;
    }
    } else {
    if (f0 > f26) {
    f31 = f31 / f17;
    i18 |= 2;
    f30 = f30 / f17;
    f0 = f0 - f26;
    f17 = f17 - f0;
    f31 = f31 * f17;
    f30 = f30 * f17;
    }
    goto L_CD0C;
    }
    goto L_CD0C;
    } else {
    f2 = 0.0f;
    if (f29 < 0.0f) {
    if (f0 > f26) {
    f31 = f31 / f17;
    i18 |= 2;
    f30 = f30 / f17;
    f0 = f0 - f26;
    f17 = f17 + f0;
    f31 = f31 * f17;
    f30 = f30 * f17;
    }
    } else {
    if (f0 > f27) {
    f31 = f31 / f17;
    i18 |= 2;
    f30 = f30 / f17;
    f0 = f0 - f27;
    f17 = f17 - f0;
    f31 = f31 * f17;
    f30 = f30 * f17;
    }
    }
    f0 = float_neg1_804195a4;
    sf18 = f0;
    }
    goto L_CD0C;

L_CC7C:
    if (i19 == 0) goto L_CCF8;
    if (f3 > 0.0) {
    f2 = 0.0f;
    if (f29 < 0.0f) goto L_CD0C;
    if (f0 > 0.0f) {
    f31 = f31 / f17;
    i18 = 3;
    f30 = f30 / f17;
    f17 = f17 - f0;
    f31 = f31 * f17;
    f30 = f30 * f17;
    }
    goto L_CD0C;
    }
    f2 = 0.0f;
    if (f29 < 0.0f) {
    if (f0 > 0.0f) {
    f31 = f31 / f17;
    i18 = 3;
    f30 = f30 / f17;
    f17 = f17 + f0;
    f31 = f31 * f17;
    f30 = f30 * f17;
    }
    }
L_CCEC:
    f0 = float_neg1_804195a4;
    sf18 = f0;
    goto L_CD0C;
L_CCF8:
    if (f3 > 0.0) goto L_CD0C;
    f0 = float_neg1_804195a4;
    sf18 = f0;

L_CD0C:
    f0 = sf1c;
    f2 = f20 + f30;
    f3 = 0.0f;
    f4 = f0 + f31;
    f0 = sf18;
    out->unk_0C = f4;
    out->unk_10 = f3;
    out->unk_14 = f2;
    if (f0 < f3) {
        f2 = -f30;
        f0 = f31 / f17;
        f2 = f2 / f17;
        out->unk_18 = f2;
        out->unk_1C = f3;
        out->unk_20 = f0;
    } else {
        f2 = f30 / f17;
        f0 = -f31;
        out->unk_18 = f2;
        out->unk_1C = f3;
        f0 = f0 / f17;
        out->unk_20 = f0;
    }
L_CD68:
    out->unk_00 = f1;
    i24 = 1;
    out->unk_08 = f24;
    out->unk_24 = i23;
    out->unk_28 = i18;
    goto L_D154;

L_CD80:
    f0 = r30[0].z;
    f1 = r30[1].z;
    f10 = r30[0].x;
    f6 = f1 - f0;
    f2 = r30[1].x;
    f1 = p.x;
    f5 = f2 - f10;
    f2 = p.z;
    f7 = -f6;
    f3 = __fabsf(f5);
    f4 = __fabsf(f7);
    if (f4 > f3) {
    f4 = f10 - f1;
    f3 = f5 / f7;
    f4 = f4 / f7;
    f4 = f5 * f4 + f2;
    f3 = f6 - f5 * f3;
    f4 = f4 - f0;
    f4 = f4 / f3;
    goto L_CE04;
    }
L_CDD4:
    if (0.0 == f5) goto L_CE00;
    f4 = f0 - f2;
    f3 = f6 / f5;
    f4 = f4 / f5;
    f4 = f7 * f4 + f1;
    f3 = f5 - f7 * f3;
    f4 = f4 - f10;
    f4 = f4 / f3;
    goto L_CE04;
L_CE00:
    f4 = 0.0f;
L_CE04:
    f9 = __fabsf(f5);
    f8 = __fabsf(f6);
    f3 = f4 * f5 + f10;
    f4 = f4 * f6 + f0;
    if (f9 > f8) {
        f6 = f3 - f10;
        f28 = f6 / f5;
        if (f28 < -0.0000001) {
            goto L_CE48;
        }
        // dead like the compare above; it pools the double the original has at +0x70
        if (f28 > 1.0000001) {
            goto L_CE48;
        }
    } else {
        if (0.0 == f6) goto L_CE48;
        f0 = f4 - f0;
        f28 = f0 / f6;
    }
L_CE48:
    f6 = f2 - f4;
    f10 = r29[0].y;
    f8 = r29[1].y;
    f2 = f1 - f3;
    f0 = f6 * f6;
    f8 = f8 - f10;
    f1 = f2 * f2 + f0;
    f0 = f28 * f8 + f10;
    if (f1 > 0.0) f1 = sqrtf(f1);
L_CF48:
    f8 = sf20;
    if (f1 >= f8) goto L_D154;
    sf20 = f1;
    if (i20 == 0) goto L_D0FC;
    if (f0 < sf28) {
    f3 = f6 * f5;
    f0 = 0.0f;
    f2 = f2 * f7 + f3;
    if (f2 < 0.0f) {
    if (f1 > f27) {
    f1 = f27;
    i18 |= 2;
    }
    } else {
    if (f1 > f26) {
    f1 = f26;
    i18 |= 2;
    }
    f1 = -f1;
    }
    f0 = sf28;
    i0 = i18 | 1;
    f3 = 0.0f;
    i24 = 1;
    out->unk_00 = f0;
    f2 = r28[0].z;
    f0 = r27[0].x;
    out->unk_0C = f1 * f2 + f0;
    out->unk_10 = f3;
    f2 = r28[0].x;
    f0 = r27[0].z;
    out->unk_14 = f0 - f1 * f2;
    f0 = r28[0].x;
    out->unk_18 = f0;
    out->unk_1C = f3;
    f0 = r28[0].z;
    out->unk_20 = f0;
    out->unk_08 = f24;
    out->unk_24 = i22;
    out->unk_28 = i0;
    goto L_D154;
    } else {
    if (f0 > sf24) {
    f3 = f6 * f5;
    f0 = 0.0f;
    f2 = f2 * f7 + f3;
    if (f2 < 0.0f) {
    if (f1 > f27) {
    f1 = f27;
    i18 |= 2;
    }
    } else {
    if (f1 > f26) {
    f1 = f26;
    i18 |= 2;
    }
    f1 = -f1;
    }
    f0 = sf24;
    i0 = i18 | 1;
    f3 = 0.0f;
    i24 = 1;
    out->unk_00 = f0;
    f2 = r26[0].z;
    f0 = r25[0].x;
    out->unk_0C = f1 * f2 + f0;
    out->unk_10 = f3;
    f2 = r26[0].x;
    f0 = r25[0].z;
    out->unk_14 = f0 - f1 * f2;
    f0 = r26[0].x;
    out->unk_18 = f0;
    out->unk_1C = f3;
    f0 = r26[0].z;
    out->unk_20 = f0;
    out->unk_08 = f24;
    out->unk_24 = i21;
    out->unk_28 = i0;
    goto L_D154;
    }
    }
L_D0AC:
    f8 = f6 * f5;
    f5 = 0.0f;
    f7 = f2 * f7 + f8;
    if (f7 < 0.0f) {
    if (f1 > f27) {
    f2 = f2 / f1;
    i18 |= 2;
    f1 = f6 / f1;
    f2 = f27 * f2;
    f6 = f27 * f1;
    }
    goto L_D0FC;
    } else {
    if (f1 > f26) {
    f2 = f2 / f1;
    i18 |= 2;
    f1 = f6 / f1;
    f2 = f26 * f2;
    f6 = f26 * f1;
    }
    }
L_D0FC:
    f2 = f3 + f2;
    out->unk_00 = f0;
    f1 = 0.0f;
    f0 = f4 + f6;
    i24 = 1;
    out->unk_0C = f2;
    out->unk_10 = f1;
    out->unk_14 = f0;
    f2 = r29[0].x;
    f0 = r29[1].x;
    f0 = f0 - f2;
    f0 = f28 * f0 + f2;
    out->unk_18 = f0;
    out->unk_1C = f1;
    f1 = r29[0].z;
    f0 = r29[1].z;
    f0 = f0 - f1;
    f0 = f28 * f0 + f1;
    out->unk_20 = f0;
    out->unk_08 = f24;
    out->unk_24 = i23;
    out->unk_28 = i18;

L_D154:
    r30++;
    r29++;
    i23++;
L_D160:
    if (i23 < i15) goto L_C4BC;
    return i24;
}

// Match-only helpers: the original materialises sub/cross/dot results as Vec locals and
// copies them into by-value argument slots, which only happens when these are inline functions
// taking Vec by value.
static inline Vec collisionTriSub(Vec a, Vec b) {
    Vec r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

static inline Vec collisionTriCross(Vec a, Vec b) {
    Vec r;
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

static inline f32 collisionTriDot(Vec a, Vec b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec collisionTriScale(Vec v, f32 s) {
    Vec r;
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
    return r;
}

static inline Vec collisionTriAdd(Vec a, Vec b) {
    Vec r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

// The `dir` vector must be the anonymous literal `{ 0.0f, -1.0f, 0.0f }`, not a copy of the named
// `vec3_802c12f4`: copied from the named global MWCC hoists all three loads to the prologue (98.0%);
// the literal pools to the same .rodata Vec and restores the target's interleaved load schedule.
// Remaining diff: the first two vertex loads are swapped -- the target issues `lwz v1.x` before the `stmw`
// (this build issues `lwz v0.x` after it). The `dot(...) > 0.0` edge guards must be `0.0` (double, reusing the
// guard block's already-resident `0.0` double), not `0.0f`; the float form keeps a second 0.0f live and
// shifts the `collisionTriDot(n, dir)` guard accumulators one FP register higher (f2..f5 vs f0,f2,f3,f4).
// This is the same known-hard load-hoist class as `marioOfsRotReset` (the scheduler moves a load across the
// prologue store in the opposite direction; `scheduling off` collapses the whole function). Everything else --
// instructions, slots, constants and all FP/GP register assignments -- matches.
// NONMATCHING (99.65%): first vertex-load pair swapped (target hoists `lwz v1.x` above `stmw`).
BOOL collisionTri_simple(Vec p, Vec v0, Vec v1, Vec v2, f32* out) {
    Vec dir = vec3_802c12f4;
    Vec bc = collisionTriSub(v2, v0);
    Vec ab = collisionTriSub(v0, v1);
    Vec ca = collisionTriSub(v1, v2);
    Vec n = collisionTriCross(bc, ab);
    f32 d;
    f32 s;
    f32 y;

    if (n.x == 0.0f && n.y == 0.0f && n.z == 0.0f) {
        return FALSE;
    }
    d = collisionTriDot(n, collisionTriSub(p, v0));
    if (d > 0.0f) {
        if (collisionTriDot(n, dir) >= 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v0), collisionTriCross(bc, dir)) > 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v1), collisionTriCross(ab, dir)) > 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v2), collisionTriCross(ca, dir)) > 0.0) {
            return FALSE;
        }
    } else {
        if (collisionTriDot(n, dir) <= 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v0), collisionTriCross(bc, dir)) < 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v1), collisionTriCross(ab, dir)) < 0.0) {
            return FALSE;
        }
        if (collisionTriDot(collisionTriSub(p, v2), collisionTriCross(ca, dir)) < 0.0) {
            return FALSE;
        }
    }
    s = -d / collisionTriDot(n, dir);
    {
        Vec hit = collisionTriAdd(p, collisionTriScale(dir, s));
        if (hit.y < *out) {
            return FALSE;
        }
        *out = hit.y;
        return TRUE;
    }
}

void calcCurrentCamState(f32* dst) {
    Vec* src = (Vec*)wp->map->unk_E8;
    f32 f31;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 f23;
    f32 f25b;
    f32 angle;
    Mtx mtx;
    Vec temp;
    Vec out;
    Vec axis;

    dst[0] = src->x;
    dst[1] = src->y;
    dst[2] = src->z;
    if (wp->state0.unk_00 == 0) {
        dst[3] = 0.0f;
        dst[4] = 100.0f;
        dst[5] = 100.0f;
        dst[6] = 0.0f;
        dst[7] = 0.0f;
        dst[8] = 0.0f;
        return;
    }
    sx = wp->state0.x;
    sy = wp->state0.y;
    sz = wp->state0.z;
    f27 = wp->state0.unk_34;
    f26 = wp->state0.unk_3C;
    f25 = wp->state0.unk_40;
    f31 = wp->state0.unk_48;
    angle = wp->state0.unk_44 * 3.141592f / 180.0f;
    f23 = f27 * f25 * (f32)cos(angle);
    f24 = f25 * (f32)sin(angle);
    f25b = f26 * f25 * (f32)cos(angle);
    dst[3] = sx + f23;
    dst[4] = sy + f24;
    dst[5] = sz + f25b;
    axis.x = -f26;
    axis.y = 0.0f;
    axis.z = f27;
    PSMTXRotAxisRad(mtx, &axis, f31 * 0.017453292f);
    temp.x = -f23;
    temp.y = -f24;
    temp.z = -f25b;
    PSMTXMultVec(mtx, &temp, &out);
    dst[6] = dst[3] + out.x;
    dst[7] = dst[4] + out.y;
    dst[8] = dst[5] + out.z;
}

BOOL camRoadSetup(const char* filename) {
    u32 length;
    char path[0x80];
    DVDEntry* entry;
    s32 dbg;

    memset(wp, 0, sizeof(CamRoadWork));
    wp->unk_C8 = 0;
    wp->state2.unk_00 = 0;
    wp->state2.unk_00 = 0;
    wp->unk_19C = float_0p005_80419548;
    wp->unk_1A0 = float_0_8041954c;
    wp->unk_1A4 = float_0p004_80419550;
    wp->unk_1A8 = float_0p008_80419554;
    wp->unk_1AC = float_1_80419558;
    wp->unk_1B0 = float_1_80419558;
    wp->unk_1B4 = float_2_8041955c;
    wp->unk_1B8 = float_1_80419558;
    wp->unk_1BC = float_1_80419558;
    wp->unk_1C0 = float_3_80419560;
    wp->unk_1C4 = float_1_80419558;
    wp->unk_204 = -1;
    wp->unk_208 = 0;
    {
        // The original pools these two debug-print strings but nothing reads them; a function-local static
        // lands them after the doubles the earlier functions pool.
        static const char s1[] = "%scam = %d\n";
        static const char s2[16] = "%scurve = %d\n";
        dbg = (s32)s1 + (s32)s2;
    }
    camShiftInit();
    arcOpen(filename, (void**)&wp->map, &length);
    if (wp->map == NULL) {
        sprintf(path, str_804195C0, getMapDataDvdRoot(), filename);
        entry = DVDMgrOpen(path, 2, 0);
        if (entry != NULL) {
            length = DVDMgrGetLength(entry);
            wp->map = _mapAlloc(OSRoundUp32B(length));
            DVDMgrRead(entry, wp->map, OSRoundUp32B(length), 0);
            DVDMgrClose(entry);
        } else {
            return FALSE;
        }
    }
    if (wp->map->unk_E8 < (u32)wp->map) {
        wp->map->unk_E8 += (u32)wp->map;
        wp->map->entries = (CamRoadMapEntry*)((u32)wp->map->entries + (u32)wp->map);
        wp->map->unk_F0 += (u32)wp->map;
        wp->map->unk_F4 += (u32)wp->map;
        wp->map->unk_F8 += (u32)wp->map;
        wp->map->unk_FC += (u32)wp->map;
        wp->map->unk_100 += (u32)wp->map;
        wp->map->unk_104 += (u32)wp->map;
        wp->map->unk_108 += (u32)wp->map;
    }
    if (strcmp(wp->map->name, str_804195C8) == 0) {
        wp->unk_1D0 = 0x65;
    } else {
        wp->unk_1D0 = 0x64;
    }
    return TRUE;
}

// 4-byte zero word that ends the unit's .sdata: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8040F2EC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
