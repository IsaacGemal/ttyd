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
    /* 0x0C */ u8 pad_0C[0x18];
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
    /* 0x1D4 */ u8 pad_1D4[0x18];
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
const f64 double_1_802c1300 = 1.0;
const f64 double_0p5_802c1308 = 0.5;
const f64 double_3_802c1310 = 3.0;
const f64 double_0_802c1318 = 0.0;
const f64 double_to_int_802c1320 = 4503601774854144.0;
const f64 double_0p0001_802c1328 = 0.00009999999747378752;
const f64 double_0p01_802c1330 = 0.009999999776482582;
const f64 double_neg1Eneg07_802c1338 = -0.0000001;
const f64 double_1_802c1340 = 1.0000001;
const char str_802C1348[] = "%scam = %d\n";
const char str_802C1354[] = "%scurve = %d\n";

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
                az / float_10_80419564);

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

// NONMATCHING (91.06%): the inline-sqrtf double pool lands in its own .rodata section instead of
// right after vec3_802c12d0, and the angle/max accumulation uses different FPR allocation.
// Control flow, constants and the branch structure match.
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
            node = (CamRoadNode*)cur->unk_24;
            range = node->unk_44 - node->unk_40;
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
    if (__fabsf(cur->unk_4C - prev->unk_4C) > 1.0) {
        flags |= 0x8;
    }
    dot = cur->unk_34 * prev->unk_34 + cur->unk_3C * prev->unk_3C;
    if (dot > float_1_80419558) {
        dot = float_1_80419558;
    }
    deg = (f32)acos(dot) * float_rad2deg_80419580;
    if (__fabsf(deg) > float_180_80419588) {
        deg = float_360_80419584 - __fabsf(deg);
    }
    maxVal = float_6p2832_80419574 * cur->unk_40 * deg / float_360_80419584 * wp->unk_1B4;
    diff = __fabsf(cur->unk_44 - prev->unk_44);
    if (diff > float_180_80419588) {
        diff = float_360_80419584 - diff;
    }
    cand = float_6p2832_80419574 * cur->unk_40 * diff / float_360_80419584 * wp->unk_1B8;
    if (cand > maxVal) {
        maxVal = cand;
    }
    diff = __fabsf(cur->unk_48 - prev->unk_48);
    if (diff > float_180_80419588) {
        diff = float_360_80419584 - diff;
    }
    cand = float_6p2832_80419574 * cur->unk_40 * diff / float_360_80419584 * wp->unk_1BC;
    if (cand > maxVal) {
        maxVal = cand;
    }
    cand = __fabsf(cur->unk_40 - prev->unk_40) * wp->unk_1C0;
    if (cand > maxVal) {
        maxVal = cand;
    }
    {
        f32 dx = cur->x - prev->x;
        f32 dy = cur->y - prev->y;
        f32 dz = cur->z - prev->z;

        dist = sqrtf(dx * dx + dy * dy + dz * dz);
    }
    if (maxVal >= float_0p01_8041958c || dist >= float_0p3_80419590) {
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
    return 0;
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
