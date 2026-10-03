#include "mario/mario_hit.h"
#include "driver/hitdrv.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "party/party_vivian.h"
#include "system.h"
#include <string.h>
#include "koopa.h"
#include "party.h"
#include "driver/mobjdrv.h"
#include "mario/mario_cam.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "motion/mot_damage.h"
#include "koopa_hit.h"
#include "koopa_motion.h"
#include "motion/mot_jabara.h"
#include "pmario_sound.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include "motion/mot_slit.h"
#include "mario/mariost.h"
#include "matching.h"
#include "mario/mario_balloon.h"
#include "motion/mot_ship.h"
#include "motion/mot_roll.h"

// floats with no home header
extern const f32 float_0_8041a7a4;
extern const f32 float_1000_8041a7a0;
extern const f32 float_1_8041a7bc;
extern const f32 float_11_8041a7ac;
extern const f32 float_13_8041a7b0;
extern const Vec vec3_802c5754;
extern const f32 float_neg3000_8041a870;
extern const Vec vec3_802c5760;
extern const Vec vec3_802c576c;
extern const f32 float_37_8041a86c;
extern const f32 float_1037_8041a888;
extern const Vec vec3_802c5880;
extern const Vec vec3_802c588c;
extern const Vec vec3_802c5898;
extern const Vec vec3_802c58a4;
extern const Vec vec3_802c58b0;
extern const Vec vec3_802c58bc;
extern const f32 float_5_8041a7b8;
extern const f32 float_10_8041a7fc;
extern const f32 float_neg0p4_8041a878;
extern const f32 float_neg0p06_8041a87c;
extern const f32 float_neg0p04_8041a880;
extern const f32 float_neg0p02_8041a884;
extern const f32 float_5000_8041a874;
extern const f32 float_100_8041a7f8;
extern const f32 float_180_8041a7cc;
extern const f32 float_47_8041a84c;
extern const f32 float_30_8041a82c;
extern const f32 float_0p01_8041a854;
extern const f32 float_0p425_8041a864;
extern const f32 float_0p375_8041a868;
extern const f32 float_neg10000_8041a858;
extern const f32 float_75_8041a840;
extern const f32 float_50_8041a844;
extern const f32 float_3p1416_8041a7e0;
extern const f32 float_neg1_8041a7dc;
extern const f32 float_2_8041a7e8;
extern const f32 float_8p64_8041a85c;
extern const f32 float_105_8041a860;
extern const f32 float_90_8041a81c;
extern const f32 float_3_8041a7f0;
extern const f32 float_45_8041a7b4;
extern const f32 float_0p5_8041a7c4;
extern const f32 float_neg0p5_8041a7c8;
extern const f32 float_0p1_8041a7e4;
extern const f32 float_neg10_8041a804;
extern const f32 float_6_8041a88c;
extern const f32 float_18_8041a850;
extern const f32 float_0p25_8041a7d4;
extern const f32 float_0p75_8041a7d8;
extern const f32 float_0p05_8041a848;
extern const Vec vec3_802c57f0;
extern const Vec vec3_802c57fc;
extern const f32 float_9_8041a83c;
extern const Vec vec3_802c5808;
extern const Vec vec3_802c5814;
extern const Vec vec3_802c5820;
extern const f32 float_neg23p7_8041a834;
extern const f32 float_5p8_8041a838;
extern const f32 float_16p8_8041a830;
extern const Vec vec3_802c582c;
extern const f32 float_4_8041a7a8;
extern const f32 float_40_8041a800;
extern const Vec vec3_802c5838;
extern const f32 float_27p75_8041a820;
extern const f32 float_12_8041a824;
extern const f32 float_7_8041a828;
extern const Vec vec3_802c5844;
extern const Vec vec3_802c5850;
extern const Vec vec3_802c585c;
extern const Vec vec3_802c5874;
extern const f32 float_60_8041a808;
extern const f32 float_0p8_8041a80c;
extern const f32 float_20_8041a810;
extern const f32 float_8_8041a814;
extern const f32 float_270_8041a818;
extern const Vec vec3_802c5868;
extern const f32 float_360_8041a7c0;
extern const f32 float_0p125_8041a7d0;
extern const Vec vec3_802c58c8;
extern const Vec vec3_802c58d4;
extern const Vec vec3_802c58e0;
extern const Vec vec3_802c58ec;

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#define ROS(base, off, str) ((const char*)(base) + (off))
#else
#define RO(base, off, sym) ((const char*)&(sym))
#define ROS(base, off, str) (str)
#endif

//.rodata
const Vec vec3_802c5730 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c573c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5748 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c5754 = { 0.0f, -1.0f, 0.0f };

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

extern s32 __float_nan[];
#define NAN (*(f32*)__float_nan)


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

    if (x > float_0_8041a7a4) {
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

// locals for symbols with no home header

// floats with no home header
#ifdef MUST_MATCH
// Match-only: building the direction in an inline that starts from the named
// vector and returns by value makes MWCC materialise the original's extra
// built-then-copied Vec slot (as in searchUnder2's makeVec5760).
static inline Vec makeDirVec(const Vec* src, f32 x, f32 z) {
    Vec v = *src;
    v.x = x;
    v.z = z;
    return v;
}
#endif

// Same as marioHitCheckVec but writes through the third output pointer without a
// NULL test (every caller passes the address of a local); the original inlines
// marioHitCheckVec this way. Used by dou10_yoko_yari / dou10_yoko_yari2 /
// marioChkFront. Under MUST_MATCH the float_0/float_1000 reads are cast through a
// pointer so MWCC reloads them once per component (plain form otherwise).
static inline HitEntry* marioHitCheckVecOut(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance) {
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *a;
    query.targetDir = *b;
    query.targetDistance = *distance;

    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(&query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(&query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(&query, chkfilterVec);
    }

    if (hit == NULL) {
        return NULL;
    }

#ifdef MUST_MATCH
    // The cast defeats MWCC's CSE of the 0.0f load so the target's per-component reload is
    // reproduced; the plain form is identical and used for non-matching builds.
    *distance = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                           (query.targetDistance >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                *(const f32*)&float_1000_8041a7a0;
#else
    *distance = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                           (query.targetDistance >= float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                *(const f32*)&float_1000_8041a7a0;
#endif
    if (c) {
        *c = query.hitPos;
        c->x = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * c->x +
                          (c->x >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               *(const f32*)&float_1000_8041a7a0;
        c->y = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * c->y +
                          (c->y >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               *(const f32*)&float_1000_8041a7a0;
        c->z = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * c->z +
                          (c->z >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               *(const f32*)&float_1000_8041a7a0;
    }
    if (d) {
        *d = query.hitNormal;
    }
    return hit;
}

// Filter+round half of the above, with the query built by the caller. marioChkFront needs
// its query.targetPos written before the dir build (the original schedules the pos->x/y/z
// loads first), which marioHitCheckVecOut cannot express because it copies *a itself.
static inline HitEntry* marioHitCheckVecRound(HitCheckQuery* query, Vec* c, Vec* d, f32* distance) {
    HitEntry* hit;

    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(query, chkfilterVec);
    }
    if (hit == NULL) {
        return NULL;
    }
#ifdef MUST_MATCH
    *distance = (f32)(s32)(float_1000_8041a7a0 * query->targetDistance +
                           (query->targetDistance >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                *(const f32*)&float_1000_8041a7a0;
#else
    *distance = (f32)(s32)(float_1000_8041a7a0 * query->targetDistance +
                           (query->targetDistance >= float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                float_1000_8041a7a0;
#endif
    if (c) {
        *c = query->hitPos;
        c->x = (f32)(s32)(c->x * *(const f32*)&float_1000_8041a7a0 +
                          (c->x >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               *(const f32*)&float_1000_8041a7a0;
        c->y = (f32)(s32)(c->y * *(const f32*)&float_1000_8041a7a0 +
                          (c->y >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               *(const f32*)&float_1000_8041a7a0;
        c->z = (f32)(s32)(c->z * *(const f32*)&float_1000_8041a7a0 +
                          (c->z >= *(const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               *(const f32*)&float_1000_8041a7a0;
    }
    if (d) {
        *d = query->hitNormal;
    }
    return hit;
}

#ifdef MUST_MATCH
static HitEntry* marioHitCheckVecNoCTest(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
#ifdef MUST_MATCH
static HitEntry* marioHitCheckVecDistOut(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
#endif
#endif

#ifdef MUST_MATCH
// Match-only Vec builders: returning by value makes MWCC place pos at 0x14 and dir at 0x8
// like the original; the natural field-assignment form is under #else.
static Vec mhc_makePos(f32 x, f32 y, f32 z) {
    Vec v = vec3_802c5730;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
static Vec mhc_makeDir(f32 x, f32 y, f32 z) {
    Vec v = vec3_802c573c;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
#endif

// Matches 100%. marioHitCheck uses the match-only marioHitCheckVecDistOut above (it keeps the
// incoming *distance in f31 across the filter call, like the original); the three `*c` roundings
// read float_1000 through a pointer cast so MWCC colours the component/1000 FPRs like the target.
HitEntry* marioHitCheck(f32* outX, f32* outY, f32* outZ, f32* distance,
                        f32* outNX, f32* outNY, f32* outNZ,
                        f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz) {
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
#ifdef MUST_MATCH
    // The inliner's c null test is folded in the original; pass c through a local and use the
    // match-only no-c-test copy so the emitted tests line up. Natural call under #else.
    Vec* php;
#endif

#ifdef MUST_MATCH
    pos = mhc_makePos(x, y, z);
    dir = mhc_makeDir(nx, ny, nz);
#else
    pos = vec3_802c5730;
    dir = vec3_802c573c;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    dir.x = nx;
    dir.y = ny;
    dir.z = nz;
#endif
#ifdef MUST_MATCH
    php = &hitPos;
    hit = marioHitCheckVecDistOut(&pos, &dir, php, &hitNormal, distance);
#else
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, distance);
#endif
#ifdef MUST_MATCH
    // The original sinks the out copies into the shared epilogue (the null path
    // branches straight there), so the guard is elided; the guarded form is under #else.
    *outX = hitPos.x;
    *outY = hitPos.y;
    *outZ = hitPos.z;
    *outNX = hitNormal.x;
    *outNY = hitNormal.y;
    *outNZ = hitNormal.z;
#else
    if (hit != NULL) {
        *outX = hitPos.x;
        *outY = hitPos.y;
        *outZ = hitPos.z;
        *outNX = hitNormal.x;
        *outNY = hitNormal.y;
        *outNZ = hitNormal.z;
    }
#endif
    return hit;
}

// Like marioHitCheckVec but filtered by chkFilterAttr2 with user0/user1 set;
// the original inlines this into marioHitCheck2 (both attr branches).
static inline HitEntry* marioHitCheckVecAttr(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance) {
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *a;
    query.targetDir = *b;
    query.targetDistance = *distance;
    query.user0 = 4;
    query.user1 = 0x800000;
    hit = hitCheckVecFilter(&query, chkFilterAttr2);
    if (hit == NULL) {
        return NULL;
    }
    *distance = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                           (query.targetDistance >= float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                float_1000_8041a7a0;
    if (c != NULL) {
        *c = query.hitPos;
        c->x = (f32)(s32)(float_1000_8041a7a0 * c->x +
                          (c->x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->y = (f32)(s32)(float_1000_8041a7a0 * c->y +
                          (c->y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->z = (f32)(s32)(float_1000_8041a7a0 * c->z +
                          (c->z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
    }
    if (d != NULL) {
        *d = query.hitNormal;
    }
    return hit;
}


#ifdef MUST_MATCH
// Match-only variant used by marioHitCheck: guards the rounding with `if (hit != NULL)` and
// stores `*distance` from the shared tail, so MWCC keeps the incoming *distance in f31 across
// the filter call. marioSearchUnder/searchUnder2 use marioHitCheckVecNoCTest below (their
// distance is dead, and there the early-return shape is what the original inlines).
static HitEntry* marioHitCheckVecDistOut(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance) {
    HitCheckQuery query;
    HitEntry* hit;
    f32 td;

    td = *distance;
    query.targetPos = *a;
    query.targetDir = *b;
    query.targetDistance = td;

    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(&query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(&query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(&query, chkfilterVec);
    }

    if (hit == NULL) {
        hit = NULL;
    } else {
        td = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                        (query.targetDistance >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
             float_1000_8041a7a0;
        *c = query.hitPos;
        c->x = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * c->x +
                          (c->x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->y = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * c->y +
                          (c->y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->z = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * c->z +
                          (c->z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        if (d != NULL) {
            *d = query.hitNormal;
        }
    }
    *distance = td;
    return hit;
}
#endif

#ifdef MUST_MATCH
// Match-only variant of marioHitCheckVec that assumes `c` is non-null, so MWCC emits no
// null test for it (the original compiler folded the inlined `c != NULL`). The natural
// build never sees this and calls marioHitCheckVec directly.
static HitEntry* marioHitCheckVecNoCTest(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance) {
    HitCheckQuery query;
    HitEntry* hit;
    f32 td;

    td = *distance;
    query.targetPos = *a;
    query.targetDir = *b;
    query.targetDistance = td;

    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(&query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(&query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(&query, chkfilterVec);
    }

    if (hit == NULL) {
        return NULL;
    }

    td = (f32)(s32)(float_1000_8041a7a0 * td +
                    (td >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
         float_1000_8041a7a0;
    *c = query.hitPos;
    c->x = (f32)(s32)(float_1000_8041a7a0 * c->x +
                      (c->x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
           float_1000_8041a7a0;
    c->y = (f32)(s32)(float_1000_8041a7a0 * c->y +
                      (c->y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
           float_1000_8041a7a0;
    c->z = (f32)(s32)(float_1000_8041a7a0 * c->z +
                      (c->z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
           float_1000_8041a7a0;
    if (d != NULL) {
        *d = query.hitNormal;
    }
    *distance = td;
    return hit;
}
#endif

static inline Vec makeVec5760(f32 x, f32 y, f32 z) {
    Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

void marioHitCheckWidth(void) {
    MarioWork* wp = marioGetPtr();

    if (!(wp->flags & 0x100000)) {
        f32 target;
        f32 z;

        if ((wp->flags & 0x1000000) == 0) {
            target = wp->wPlayerCollisionBox.x;
        } else {
            target = float_18_8041a850;
        }
        z = wp->wPlayerCollisionBox.z + float_1_8041a7bc;
        wp->wPlayerCollisionBox.z = z;
        if (z > target) {
            wp->wPlayerCollisionBox.z = target;
        }
    } else {
        if (wp->wPlayerCollisionBox.z > float_6_8041a88c) {
            f32 z = wp->wPlayerCollisionBox.z - float_1_8041a7bc;

            wp->wPlayerCollisionBox.z = z;
            if (z < float_6_8041a88c) {
                wp->wPlayerCollisionBox.z = float_6_8041a88c;
            }
        }
        if (wp->wPlayerCollisionBox.z < float_6_8041a88c) {
            f32 z = wp->wPlayerCollisionBox.z + float_1_8041a7bc;

            wp->wPlayerCollisionBox.z = z;
            if (z > float_6_8041a88c) {
                wp->wPlayerCollisionBox.z = float_6_8041a88c;
            }
        }
    }
}

BOOL chkFilterAttr2(HitCheckQuery* query, HitEntry* entry) {
    return (entry->attributes & query->user0) == 0 || (entry->attributes & query->user1) != 0;
}

BOOL chkfilterVec(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    const u8* base = (const u8*)&vec3_802c5730;
    u8* obj;

    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) == 0 && (attr & 0x4) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = (u8*)mobjHitObjPtrToPtr(entry);
    if (strcmp((const char*)(obj + 0x15), "MOBJ_HiddenHatenaBlock") == 0 ||
        strcmp((const char*)(obj + 0x15), "MOBJ_HiddenBadgeBlock") == 0 ||
        strcmp((const char*)(obj + 0x15), "MOBJ_Hidden10CountBlock") == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterVecRoll(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    const u8* base = (const u8*)&vec3_802c5730;
    u8* obj;

    if ((attr & 0x80) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) == 0 && (attr & 0x80004) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = (u8*)mobjHitObjPtrToPtr(entry);
    if (strcmp((const char*)(obj + 0x15), "MOBJ_HiddenHatenaBlock") == 0 ||
        strcmp((const char*)(obj + 0x15), "MOBJ_HiddenBadgeBlock") == 0 ||
        strcmp((const char*)(obj + 0x15), "MOBJ_Hidden10CountBlock") == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}



// strings/rodata with no home header (addressed from the rodata base symbol)

BOOL chkfilterVecVivian(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    const u8* base = (const u8*)&vec3_802c5730;
    u8* obj;

    if ((attr & 0x80804) != 0 && (attr & 0x800000) == 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = (u8*)mobjHitObjPtrToPtr(entry);
    if (strcmp((const char*)(obj + 0x15), "MOBJ_HiddenHatenaBlock") == 0 ||
        strcmp((const char*)(obj + 0x15), "MOBJ_HiddenBadgeBlock") == 0 ||
        strcmp((const char*)(obj + 0x15), "MOBJ_Hidden10CountBlock") == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

HitEntry* marioHitCheckVec(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance) {
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *a;
    query.targetDir = *b;
    query.targetDistance = *distance;

    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(&query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(&query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(&query, chkfilterVec);
    }

    if (hit == NULL) {
        return NULL;
    }

    *distance = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                           (query.targetDistance >= float_0_8041a7a4 ? float_0p5_8041a7c4
                                                                     : float_neg0p5_8041a7c8)) /
                float_1000_8041a7a0;
    if (c != NULL) {
        *c = query.hitPos;
        c->x = (f32)(s32)(float_1000_8041a7a0 * c->x +
                          (c->x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->y = (f32)(s32)(float_1000_8041a7a0 * c->y +
                          (c->y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
        c->z = (f32)(s32)(float_1000_8041a7a0 * c->z +
                          (c->z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
               float_1000_8041a7a0;
    }
    if (d != NULL) {
        *d = query.hitNormal;
    }
    return hit;
}

void* marioHitCheck2(Vec* a, Vec* b, Vec* c, Vec* d, f32* e) {
    MarioWork* wp = marioGetPtr();
    void* hit;
    f32 dist;

    dist = *e;
    if (wp->currMotionId != MARIO_MOTION_JABARA) {
        if ((wp->flags & 0x20000) == 0) {
            hit = marioHitCheckVecAttr(a, b, c, d, &dist);
        } else {
            hit = marioHitCheckVec(a, b, c, d, &dist);
        }
    } else {
        if (marioGetJabaraState() == 1) {
            hit = marioHitCheckVecAttr(a, b, c, d, &dist);
        } else {
            hit = marioHitCheckVec(a, b, c, d, &dist);
        }
    }
    *e = dist;
    return hit;
}
void clear_hitobj_ride(void) {
    MarioWork* wp = marioGetPtr();

    wp->hitobjStandOn = NULL;
    if (wp->characterId == 2) {
        kpaClearHitobjRide();
    }
}



BOOL marioChkBero(HitEntry* hit) {
    s32 attr = hitGetAttr(hit);
    MarioWork* wp = marioGetPtr();

    if ((attr & 0x80000) != 0 && (wp->flags & 0x1000000) == 0) {
        return FALSE;
    }
    if ((attr & 0x80) == 0) {
        return (attr & 0x800000) != 0;
    }
    return wp->currMotionId == 0x19;
}

HitEntry* marioSearchUnder(void) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;
    f32 y;
#ifdef MUST_MATCH
    // Passing the out-pointers through locals makes the inliner's null tests on c/d
    // use registers (and lands them at the offsets the original uses); the natural
    // call is under #else.
    Vec* php;
#endif

    y = wp->wPlayerCollisionBox.y;
    if (wp->flags & 0x1000000) {
        y = y * float_0p5_8041a7c4;
    }

    pos = wp->position;
    pos.y = pos.y + y;
    dir = vec3_802c5754;
    dist = float_1000_8041a7a0;
#ifdef MUST_MATCH
    php = &hitPos;
    hit = marioHitCheckVecNoCTest(&pos, &dir, php, &hitNormal, &dist);
#else
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
#endif
    *(f32*)wp->field_0x1C0 = (hit != NULL) ? hitPos.y : float_neg3000_8041a870;
    return hit;
}

// Matches 100%. pos is built from the zero vector via makeVec5760, then copied to pos and pos.y
// gets the +37; `dir` is a compound literal so the prologue's field loads schedule like the
// target (reading the named vec3_802c576c hoisted its first word early).
s32 searchUnder2(Vec* out, f32 x, f32 y, f32 z) {
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;
#ifdef MUST_MATCH
    // The inliner's c null test is folded in the original; pass c through a local and use the
    // match-only no-c-test copy so the emitted tests line up. Natural call under #else.
    Vec* php;
    Vec tmp;
#endif

#ifdef MUST_MATCH
    tmp = makeVec5760(x, y, z);
    pos = tmp;
    pos.y = pos.y + float_37_8041a86c;
#else
    pos = makeVec5760(x, y + float_37_8041a86c, z);
#endif
#ifdef MUST_MATCH
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
#else
    dir = vec3_802c576c;
#endif
    dist = float_1037_8041a888;
#ifdef MUST_MATCH
    php = &hitPos;
    hit = marioHitCheckVecNoCTest(&pos, &dir, php, &hitNormal, &dist);
#else
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
#endif
    out->x = (hit != NULL) ? hitPos.y : float_neg1_8041a7dc;
    return (s32)hit;
}
const Vec vec3_802c5778 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c5784 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5790 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c579c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c57a8 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c57b4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c57c0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c57cc = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c57d8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c57e4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c57f0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c57fc = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802c5808 = { 0.0f, 0.0f, 0.0f };

static inline HitEntry* marioHitCheckVecOut(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
#define GND_PTR(wp, type, off) (*(type*)((u8*)(wp) + (off)))

// NONMATCHING (94.74%): same 56 calls and immediates as the original. Remaining: the frame is 0x1c0 against 0x1b0
// with f27-f29 also saved: the three inlined marioHitCheckVecOut copies keep more FPR temps than the original's two (f30/f31).
void marioChkGnd(void) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    MarioWork* m;
    HitEntry* hit;
    HitEntry* ride;
    Vec p;
    Vec dir;
    Vec hitPos;
    Vec normal;
    f32 dist;
    f32 gy;
    f32 gx;
    f32 gz;
    f32 h;
    f32 fy;
    f32 fd;
    f32 fa;
    f32 dy;
    f32 grx;
    f32 grz;
    s32 ok;

    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        GND_PTR(wp, HitEntry*, 0x1E4) = (HitEntry*)marioSearchGround(&gy, &gx, &gz, float_37_8041a86c, float_0_8041a7a4);
        m = marioGetPtr();
        h = GND_PTR(m, f32, 0x1B8);
        if (m->flags & 0x1000000) {
            h *= float_0p5_8041a7c4;
        }
        p = m->position;
        p.y += h;
        dir = *(const Vec*)(base + 0x24);
        dist = float_1000_8041a7a0;
        hit = marioHitCheckVecOut(&p, &dir, &hitPos, &normal, &dist);
        GND_PTR(m, f32, 0x1C0) = hit != NULL ? hitPos.y : float_neg3000_8041a870;
        GND_PTR(wp, HitEntry*, 0x1F8) = hit;
        return;
    }
    GND_PTR(wp, HitEntry*, 0x1E4) = NULL;
    if (wp->characterId == 2) {
        kpaClearHitobjRide();
    }
    m = marioGetPtr();
    h = GND_PTR(m, f32, 0x1B8);
    if (m->flags & 0x1000000) {
        h *= float_0p5_8041a7c4;
    }
    p = m->position;
    p.y += h;
    dir = *(const Vec*)(base + 0x24);
    dist = float_1000_8041a7a0;
    hit = marioHitCheckVecOut(&p, &dir, &hitPos, &normal, &dist);
    GND_PTR(m, f32, 0x1C0) = hit != NULL ? hitPos.y : float_neg3000_8041a870;
    GND_PTR(wp, HitEntry*, 0x1F8) = hit;
    if (wp->flags & 0x30000) {
        return;
    }
    m = marioGetPtr();
    if (m->flags & 0x100) {
        ok = 0;
    } else if (m->position.y >= float_5000_8041a874) {
        ok = 0;
    } else {
        ok = 1;
    }
    if (ok == 0) {
        return;
    }
    if (!(wp->flags & 0x200)) {
        if (wp->flags & 0x1000000) {
            hit = (HitEntry*)marioSearchGroundRoll(&gy, &gx, &gz, float_11_8041a7ac, GND_PTR(wp, f32, 0x7C));
            if (hit != NULL) {
                GND_PTR(wp, HitEntry*, 0x1E4) = hit;
                GND_PTR(wp, f32, 0x1C0) = (f32)(s32)(float_100_8041a7f8 * gy + float_0p5_8041a7c4) / float_100_8041a7f8;
            }
        } else {
            if (wp->characterId == 2) {
                hit = (HitEntry*)kpaSearchGround(&gy, &gx, &gz, float_11_8041a7ac);
            } else {
                hit = (HitEntry*)marioSearchGround(&gy, &gx, &gz, float_11_8041a7ac, GND_PTR(wp, f32, 0x7C));
            }
            if (hit != NULL) {
                GND_PTR(wp, HitEntry*, 0x1E4) = hit;
                GND_PTR(wp, f32, 0x1C0) = (f32)(s32)(float_100_8041a7f8 * gy + float_0p5_8041a7c4) / float_100_8041a7f8;
            }
        }
    } else {
        gy = wp->position.y;
        if (wp->characterId == 2) {
            if (wp->airTimer == 0) {
                marioChgMot(0xA);
                marioSetFallPara();
                return;
            }
        } else if (wp->airTimer == 0) {
            marioChgMot(0xA);
            marioSetFallPara();
            return;
        }
    }
    if (wp->currMotionId == 0x19) {
        if (marioChkShipMoveMode() != 0) {
            if (GND_PTR(wp, HitEntry*, 0x1E4) == NULL) {
                marioShipChgFall();
                return;
            }
            wp->flags &= 0xFFFDFFFF;
        }
    } else {
        if (wp->currMotionId != 0x1F && GND_PTR(wp, HitEntry*, 0x1E4) != NULL && (hitGetAttr(GND_PTR(wp, HitEntry*, 0x1E4)) & 0x200)) {
            if (GND_PTR(wp, f32, 0x7C) >= float_0_8041a7a4) {
                marioSetFallPara();
            }
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x1F);
            return;
        }
        if (GND_PTR(wp, HitEntry*, 0x1E4) != NULL && (hitGetAttr(GND_PTR(wp, HitEntry*, 0x1E4)) & 0x800)) {
            if (vivianGetStatus() != 2 && vivianGetStatus() != 1 && vivianGetStatus() != 3) {
                set_damage_root_ypos(wp->position.y);
                marioChgMot(0x20);
            }
            return;
        }
        if (wp->airTimer == 0) {
            if (!(wp->flags & 0x1000000)) {
                if (marioChkFrontStep(&fy, &fd, &fa) != NULL) {
                    dy = fy - wp->position.y;
                    if (dy <= float_11_8041a7ac && dy >= float_5_8041a7b8 && GND_PTR(wp, f32, 0x214) <= float_30_8041a82c) {
                        GND_PTR(wp, f32, 0x1C4) = fy;
                        if (wp->baseSpeed != float_0_8041a7a4) {
                            if (!(GND_PTR(wp, u32, 4) & 0x1000000)) {
                                marioChgMot(0xC);
                                return;
                            }
                            marioChgMotSub(0xC, 0);
                            return;
                        }
                    }
                }
            } else {
                if (marioChkFrontStep(&fy, &fd, &fa) != NULL) {
                    dy = fy - wp->position.y;
                    if (dy <= float_11_8041a7ac && dy >= float_5_8041a7b8 && GND_PTR(wp, f32, 0x214) <= float_30_8041a82c) {
                        GND_PTR(wp, f32, 0x1C4) = fy;
                        if (wp->baseSpeed != float_0_8041a7a4) {
                            roll_upstairs_on();
                            return;
                        }
                    }
                }
            }
        }
        wp->position.y = gy;
        if (wp->characterId == 2) {
            ride = (HitEntry*)kpaChkHitobjRide();
        } else {
            ride = GND_PTR(wp, HitEntry*, 0x1E4);
        }
        if (ride != NULL) {
            if (wp->flags & 0x1000000) {
                GND_PTR(wp, f32, 0x164) = GND_PTR(wp, f32, 0x114) - wp->position.y;
                p = wp->position;
                p.y += float_10_8041a7fc;
                dir = *(const Vec*)(base + 0x18);
                dist = float_47_8041a84c;
                hit = marioHitCheckVecOut(&p, &dir, &hitPos, &normal, &dist);
                if (hit != NULL) {
                    grx = (f32)(s32)(float_100_8041a7f8 * (angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.z, float_100_8041a7f8 * normal.y) - float_180_8041a7cc) + float_0p5_8041a7c4) / float_100_8041a7f8;
                    grz = (f32)(s32)(float_100_8041a7f8 * (angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.x, float_100_8041a7f8 * normal.y) - float_180_8041a7cc) + float_0p5_8041a7c4) / float_100_8041a7f8;
                    angleABf(float_0_8041a7a4, float_0_8041a7a4, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
                    roll_grx = grx;
                    roll_grz = grz;
                }
            }
        } else {
            if (!(wp->flags & 0x1000000)) {
                if (!(wp->flags & 0x20000)) {
                    if (gx < float_0_8041a7a4) {
                        marioChgMot(0xA);
                        marioSetFallPara();
                    }
                    if (gx > float_0_8041a7a4) {
                        marioChgMot(0xB);
                        marioSetFallPara();
                    }
                }
            } else if (!(wp->flags & 0x20000)) {
                wp->flags &= 0xFFFEFFFF;
                wp->flags |= 0x20000;
                GND_PTR(wp, f32, 0x7C) = float_neg0p4_8041a878;
                GND_PTR(wp, f32, 0x80) = float_neg0p06_8041a87c;
                GND_PTR(wp, f32, 0x84) = float_neg0p04_8041a880;
                GND_PTR(wp, f32, 0x88) = float_neg0p02_8041a884;
            }
        }
    }
}

void marioChkGnd2(void) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    f32 outX;
    f32 outY;
    f32 outZ;
    f32 dy;

    if (wp->flags & 0x10000) {
        return;
    }

    if (wp->characterId != 2) {
        hit = (HitEntry*)marioSearchGround(&outY, &outX, &outZ, float_0p1_8041a7e4, *(f32*)((u8*)wp + 0x7C));
    } else {
        hit = (HitEntry*)kpaSearchGround(&outY, &outX, &outZ, float_0p1_8041a7e4);
    }

    if (hit == NULL) {
        return;
    }

    dy = wp->position.y - outY;
    if (dy < float_0_8041a7a4 && dy >= float_neg10_8041a804) {
        wp->hitobjStandOn = hit;
        wp->position.y = outY;
        wp->flags &= ~0xC0;
        wp->airTimer = 0;
        *(s16*)((u8*)wp + 0x52) = 0;

        if (wp->characterId == 2) {
            if (wp->controlStickSensitivity != float_0_8041a7a4 || kpaFireAttackCheck() == 0) {
                if (*(s8*)((u8*)wp + 0x24E) * *(s8*)((u8*)wp + 0x24E) +
                        *(s8*)((u8*)wp + 0x24F) * *(s8*)((u8*)wp + 0x24F) >
                    0xBD1) {
                    marioChgMot(2);
                } else {
                    marioChgMot(1);
                }
            } else {
                marioChgMot(0);
            }
            quake_kpaLandOn();
        } else {
            marioChgMot(0);
        }

        if (wp->characterId != 2) {
            if (marioBgmodeChk() == 0) {
                camFollowYOn();
            }
        } else {
            kpaCamFollow();
        }
    }
}

static inline HitEntry* marioHitCheckVecOut(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
#define GND_CENTI(v) ((f32)(s32)(float_100_8041a7f8 * (v) + float_0p5_8041a7c4) / float_100_8041a7f8)

// NONMATCHING (91.21%): same 25 calls and immediates as the original; the frame is 0x210 against 0x1e0 (three more
// 12-byte vec temps than the original keeps).
HitEntry* marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    HitEntry* result;
    f32 dist;
    f32 fa;
    f32 fb;
    f32 ox;
    f32 oz;
    f32 f23;
    f32 f24;
    f32 bestY;
    f32 py;
    f32 px;
    f32 pz;
    f32 lim;
    f32 rad;
    f32 angle;
    f32 ang;
    s32 i;

    *(f32*)((u8*)wp + 0x218) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x21C) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x220) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x214) = float_0_8041a7a4;
    if (wp->flags & 0x200) {
        *outY = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *outY = wp->position.y;
        return NULL;
    }
    fa = __fabs(a);
    fb = __fabs(b);
    *outY = wp->position.y;
    fa = fa + float_0p01_8041a854;
    *outX = float_neg1_8041a7dc;
    fb = float_5_8041a7b8 + fb;
    if (wp->baseSpeed != float_0_8041a7a4) {
        angle = revise360(*(f32*)((u8*)wp + 0x1A0) - *(f32*)((u8*)wp + 0x198));
    } else {
        angle = toMovedir(*(f32*)((u8*)wp + 0x1A8));
    }
    if (wp->flags & 0x100000) {
        rad = float_3p1416_8041a7e0 * angle / float_180_8041a7cc;
        f24 = float_0p425_8041a864 * (*(f32*)((u8*)wp + 0x1B4) * (f32)sin(rad));
        f23 = float_0p425_8041a864 * (*(f32*)((u8*)wp + 0x1B4) * -(f32)cos(rad));
    } else {
        rad = float_3p1416_8041a7e0 * angle / float_180_8041a7cc;
        f24 = float_0p375_8041a868 * (*(f32*)((u8*)wp + 0x1B4) * (f32)sin(rad));
        f23 = float_0p375_8041a868 * (*(f32*)((u8*)wp + 0x1B4) * -(f32)cos(rad));
    }
    py = wp->position.y;
    px = wp->position.x;
    pz = wp->position.z;
    result = NULL;
    bestY = float_neg10000_8041a858;
    up = *(const Vec*)(base + 0x48);
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
        } else if (i == 1) {
            ox = f24;
            oz = f23;
        } else if (i == 2) {
            ox = -f23;
            oz = f24;
        } else if (i == 3) {
            ox = f23;
            oz = -f24;
        } else if (i == 4) {
            ox = -f24;
            oz = -f23;
        }
        p = *(const Vec*)(base + 0x54);
        p.x = px + ox;
        p.y = py + fb;
        p.z = pz + oz;
        dist = fb + fa;
        hit = marioHitCheckVecOut(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2;
            if (wp->dispFlags & 0x1000000) {
                lim = float_75_8041a840;
            } else if (wp->currMotionId == 0x10) {
                lim = float_75_8041a840;
            } else {
                lim = float_50_8041a844;
            }
            wp2 = marioGetPtr();
            *(f32*)((u8*)wp2 + 0x218) = GND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.z, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            *(f32*)((u8*)wp2 + 0x21C) = float_0_8041a7a4;
            *(f32*)((u8*)wp2 + 0x220) = GND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.x, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            ang = angleABf(float_0_8041a7a4, float_0_8041a7a4, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            if (ang >= lim) {
                hit = NULL;
            }
        }
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();
            *(f32*)((u8*)wp2 + 0x218) = GND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.z, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            *(f32*)((u8*)wp2 + 0x21C) = float_0_8041a7a4;
            *(f32*)((u8*)wp2 + 0x220) = GND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.x, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            *(f32*)((u8*)wp + 0x214) = angleABf(float_0_8041a7a4, float_0_8041a7a4, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            if (hitPos.y < py && __fabs(hitPos.y - py) >= float_2_8041a7e8) {
                if (__fabs(hitPos.y - py) > __fabs(b)) {
                    *outX = __fabs(py - hitPos.y);
                    *outZ = angleABf(px, pz, hitPos.x, hitPos.z);
                } else if (hitPos.y > bestY) {
                    *outY = hitPos.y;
                    bestY = hitPos.y;
                    *outX = float_0_8041a7a4;
                    *outZ = angleABf(px, pz, hitPos.x, hitPos.z);
                    marioUpdateCamCollision(hitPos);
                    result = hit;
                }
            } else if (__fabs(hitPos.y - py) <= float_11_8041a7ac && hitPos.y > bestY) {
                *outY = hitPos.y;
                bestY = hitPos.y;
                *outX = float_0_8041a7a4;
                *outZ = angleABf(px, pz, hitPos.x, hitPos.z);
                marioUpdateCamCollision(hitPos);
                result = hit;
            }
        }
    }
    if (result != NULL) {
        return result;
    }
    return NULL;
}


// The roll variant of marioSearchGround: the sample ring depends on |wp+0x214| (flat: 0..3, steep: 4..6) and the
// accepted slope window is ang > 105 or ang < 75.
// NONMATCHING (90.16%): salvaged wave 59 version; its `fabs(...) <= dist` was flipped (the asm is cror eq,gt,eq = >=) and
// is fixed here, the slope window (ang > 105 || ang < 75) re-checked. Only FPR/GPR colouring and scheduling differ (posY/posX/posZ land in f25-f29
// where the original has f25-f27, and the out pointers sit one register lower); the frame size now matches
// and calls/immediates agree.
s32 marioSearchGroundRoll(f32* outY, f32* outX, f32* outZ, f32 a, f32 b) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    MarioWork* wp2;
    HitCheckQuery query;
    HitEntry* hit;
    HitEntry* best;
    Vec out1;
    Vec out2;
    Vec dir;
    Vec p;
    f32 posY;
    f32 posX;
    f32 posZ;
    f32 boxY;
    f32 len;
    f32 dist;
    f32 py;
    f32 tdist;
    f32 bestY;
    s32 i;
    s32 start;
    s32 end;

    *(f32*)((u8*)wp + 0x218) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x21C) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x220) = float_0_8041a7a4;
    if (wp->flags & 0x200) {
        *outY = wp->position.y;
        return 0;
    }
    if (wp->flags & 0x100) {
        *outY = wp->position.y;
        return 0;
    }
    boxY = wp->wPlayerCollisionBox.y;
    a = __fabsf(a);
    a = a + float_0p01_8041a854;
    len = a;
    best = NULL;
    *outY = wp->position.y;
    *outX = float_neg1_8041a7dc;
    bestY = float_neg10000_8041a858;
    posY = wp->position.y;
    posX = wp->position.x;
    posZ = wp->position.z;
    dir = *(const Vec*)(base + 0x60);
    if (__fabsf(*(f32*)((u8*)wp + 0x214)) < float_0p01_8041a854) {
        dist = *(const volatile f32*)&float_2_8041a7e8;
        start = 0;
        end = 4;
    } else {
        dist = *(const volatile f32*)&float_10_8041a7fc;
        start = 4;
        end = 7;
    }
    for (i = start; i < end; i++) {
        f32 dx;
        f32 dz;
        f32 rad;

        switch (i) {
        case 0:
            dx = float_0_8041a7a4;
            dz = float_0_8041a7a4;
            break;
        case 1:
            rad = float_3p1416_8041a7e0 * (float_90_8041a81c + wp->directionView) / float_180_8041a7cc;
            dx = float_8p64_8041a85c * (f32)sin(rad);
            dz = float_8p64_8041a85c * -(f32)cos(rad);
            break;
        case 2:
            rad = float_3p1416_8041a7e0 * (wp->directionView - float_90_8041a81c) / float_180_8041a7cc;
            dx = float_8p64_8041a85c * (f32)sin(rad);
            dz = float_8p64_8041a85c * -(f32)cos(rad);
            break;
        case 3:
            dx = float_0_8041a7a4;
            dz = float_0_8041a7a4;
            movePos(&dx, &dz, float_3_8041a7f0, wp->directionView);
            break;
        case 4:
            dx = float_0_8041a7a4;
            dz = float_0_8041a7a4;
            break;
        case 5:
            rad = float_3p1416_8041a7e0 * (float_90_8041a81c + wp->directionView) / float_180_8041a7cc;
            dx = float_8p64_8041a85c * (f32)sin(rad);
            dz = float_8p64_8041a85c * -(f32)cos(rad);
            movePos(&dx, &dz, float_3_8041a7f0, wp->directionView);
            break;
        case 6:
            rad = float_3p1416_8041a7e0 * (wp->directionView - float_90_8041a81c) / float_180_8041a7cc;
            dx = float_8p64_8041a85c * (f32)sin(rad);
            dz = float_8p64_8041a85c * -(f32)cos(rad);
            movePos(&dx, &dz, float_3_8041a7f0, wp->directionView);
            break;
        }
        py = posY + boxY;
        tdist = boxY + len;
        p = *(const volatile Vec*)(base + 0x6c);
        p.x = posX + dx;
        p.y = py;
        p.z = posZ + dz;
        query.targetPos = p;
        query.targetDir = dir;
        query.targetDistance = tdist;

        if (marioGetPtr()->currMotionId == 0x1C) {
            hit = hitCheckVecFilter(&query, chkfilterVecVivian);
        } else if (marioGetPtr()->flags & 0x1000000) {
            hit = hitCheckVecFilter(&query, chkfilterVecRoll);
        } else {
            hit = hitCheckVecFilter(&query, chkfilterVec);
        }
#ifdef MUST_MATCH
        if (hit == NULL) {
            hit = NULL;
            goto check_hit2;
        }
        out1 = query.hitPos;
        out1.x = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * out1.x +
                            (out1.x >= *(volatile const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                 *(const f32*)&float_1000_8041a7a0;
        out1.y = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * out1.y +
                            (out1.y >= *(volatile const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                 *(const f32*)&float_1000_8041a7a0;
        out1.z = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * out1.z +
                            (out1.z >= *(volatile const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                 *(const f32*)&float_1000_8041a7a0;
        if (&out2 != NULL) {
            out2 = query.hitNormal;
        }
    check_hit2:
        if (hit != NULL) {
#else
        if (hit != NULL) {
            out1 = query.hitPos;
            out1.x = (f32)(s32)(float_1000_8041a7a0 * out1.x +
                                (out1.x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                     float_1000_8041a7a0;
            out1.y = (f32)(s32)(float_1000_8041a7a0 * out1.y +
                                (out1.y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                     float_1000_8041a7a0;
            out1.z = (f32)(s32)(float_1000_8041a7a0 * out1.z +
                                (out1.z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                     float_1000_8041a7a0;
            out2 = query.hitNormal;
        }
        if (hit != NULL) {
#endif
            f32 ang1;
            f32 ang2;
            f32 ang3;
            f32 len2;
            f32 dy;

            wp2 = marioGetPtr();
            ang1 = angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * out2.z,
                            float_100_8041a7f8 * out2.y);
            *(f32*)((u8*)wp2 + 0x218) = (f32)(s32)(float_100_8041a7f8 * (ang1 - float_180_8041a7cc) + float_0p5_8041a7c4) /
                                        float_100_8041a7f8;
            *(f32*)((u8*)wp2 + 0x21C) = float_0_8041a7a4;
            ang2 = angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * out2.x,
                            float_100_8041a7f8 * out2.y);
            len2 = sqrtf(out2.x * out2.x + out2.z * out2.z);
            *(f32*)((u8*)wp2 + 0x220) = (f32)(s32)(float_100_8041a7f8 * (ang2 - float_180_8041a7cc) + float_0p5_8041a7c4) /
                                        float_100_8041a7f8;
            ang3 = angleABf(float_0_8041a7a4, float_0_8041a7a4, len2, -out2.y);
            if (ang3 <= float_105_8041a860 && ang3 >= float_75_8041a840) {
                continue;
            }
            *(f32*)((u8*)wp + 0x214) = ang3;
            if (out1.y < posY && __fabsf(out1.y - posY) >= dist) {
                if (__fabsf(out1.y - posY) > __fabsf(b)) {
                    *outX = __fabsf(posY - out1.y);
                    *outZ = angleABf(posX, posZ, out1.x, out1.z);
                } else if (out1.y > bestY) {
                    *outY = out1.y;
                    bestY = out1.y;
                    *outX = float_0_8041a7a4;
                    *outZ = angleABf(posX, posZ, out1.x, out1.z);
                    best = hit;
                }
                continue;
            }
            dy = __fabsf(out1.y - posY);
            if (dy <= float_11_8041a7ac && out1.y > bestY) {
                *outY = out1.y;
                bestY = out1.y;
                *outX = float_0_8041a7a4;
                *outZ = angleABf(posX, posZ, out1.x, out1.z);
                best = hit;
            }
        }
    }
    if (best != NULL) {
        return (s32)best;
    }
    return 0;
}


void marioChkToge(void) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    s32 attr;

    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        return;
    }
    hit = (HitEntry*)*(void**)((u8*)wp + 0x1E0);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0 &&
            *(f32*)&wp->field_0x1F0[0x20] < float_0p5_8041a7c4 * wp->wPlayerCollisionBox.x) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }

    hit = (HitEntry*)*(void**)((u8*)wp + 0x1F0);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }

    hit = (HitEntry*)*(void**)((u8*)wp + 0x1F4);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }
}


void* marioChkLandon(f32* pos, f32 spd) {
    MarioWork* wp = marioGetPtr();
    f32 outY;
    f32 outX;

    if (wp->characterId != 2) {
        if ((wp->flags & 0x1000000) == 0) {
            return (void*)marioSearchGround(pos, &outY, &outX, spd, spd);
        } else {
            return (void*)marioSearchGroundRoll(pos, &outY, &outX, spd, spd);
        }
    } else {
        return marioLandonChk_old(pos, spd);
    }
}

static inline HitEntry* marioHitCheckVecOut(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
#define LND_CENTI(v) ((f32)(s32)(float_100_8041a7f8 * (v) + float_0p5_8041a7c4) / float_100_8041a7f8)


// Older landing check (the Koopa-aware ground probe): same five-sample ring as marioChkLandon2 but the ray points
// down from 0.75 * height and the accepted hit y is returned rounded to 0.01.
// NONMATCHING (81.67%): same 15 calls; remaining: frame size/FPR colouring and the inlined sqrt's classification
// switch (cmpwi 1/3). Compares: fabs(...) < 0.1 and the rounding tests (cror eq,gt -> >=) checked against the asm.
void* marioLandonChk_old(f32* pos, f32 spd) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec dir;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    f32 dist;
    f32 fa;
    f32 h;
    f32 rad;
    f32 angle;
    f32 f20;
    f32 f19;
    f32 yy;
    f32 d0;
    f32 px;
    f32 pz;
    f32 ox;
    f32 oz;
    f32 ang;
    f32 gy;
    f32 gx;
    f32 gz;
    s32 i;
    void* r;

    *(f32*)((u8*)wp + 0x218) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x21C) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x220) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x214) = float_0_8041a7a4;
    if (wp->flags & 0x200) {
        *pos = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *pos = wp->position.y;
        return NULL;
    }
    fa = float_0p01_8041a854 + __fabsf(spd);
    h = float_0p75_8041a7d8 * wp->wPlayerCollisionBox.y;
    if (wp->baseSpeed != float_0_8041a7a4) {
        angle = revise360(*(f32*)((u8*)wp + 0x1A0) - *(f32*)((u8*)wp + 0x198));
    } else {
        angle = toMovedir(*(f32*)((u8*)wp + 0x1A8));
    }
    rad = float_3p1416_8041a7e0 * angle / float_180_8041a7cc;
    f20 = float_0p25_8041a7d4 * (*(f32*)((u8*)wp + 0x1B4) * (f32)sin(rad));
    px = wp->position.x;
    pz = wp->position.z;
    f19 = float_0p25_8041a7d4 * (*(f32*)((u8*)wp + 0x1B4) * -(f32)cos(rad));
    if (wp->characterId == 2) {
        r = kpaSearchGround(&gy, &gx, &gz, float_11_8041a7ac);
        *pos = gy;
        return r;
    }
    d0 = h + fa;
    yy = wp->position.y + h;
    for (i = 0; i < 5; i++) {
        switch (i) {
        case 0:
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
            break;
        case 1:
            ox = f20;
            oz = f19;
            break;
        case 2:
            ox = -f19;
            oz = f19;
            break;
        case 3:
            ox = f19;
            oz = -f20;
            break;
        case 4:
            ox = -f20;
            oz = -f19;
            break;
        }
        p = *(const Vec*)(base + 0);
        dir = *(const Vec*)(base + 0xC);
        p.x = px + ox;
        p.y = yy;
        p.z = pz + oz;
        dir.x = float_0_8041a7a4;
        dir.y = float_neg1_8041a7dc;
        dir.z = float_0_8041a7a4;
        dist = d0;
        hit = marioHitCheckVecOut(&p, &dir, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();

            *(f32*)((u8*)wp2 + 0x218) = LND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.z, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            *(f32*)((u8*)wp2 + 0x21C) = float_0_8041a7a4;
            *(f32*)((u8*)wp2 + 0x220) = LND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.x, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            ang = angleABf(float_0_8041a7a4, float_0_8041a7a4, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            *(f32*)((u8*)wp + 0x214) = ang;
            if (__fabsf(LND_CENTI((yy - hitPos.y) - dist)) < float_0p1_8041a7e4) {
                *pos = LND_CENTI(hitPos.y);
                return hit;
            }
        }
    }
    return NULL;
}

// The hit sample ring of marioChkLandon, with the ground y returned through *out.
// NONMATCHING (85.03%): same 13 calls; the inlined sqrt's classification switch (cmpwi 1/3) is missing from ours and the
// frame is 0x1c0 vs 0x1a0. fabs() is __fabsf (f32 add, no frsp) as in the original; the compare `fabs(...) < 0.1` is plain.
void* marioChkLandon2(f32* out, f32 a, f32 b) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    f32 dist;
    f32 fa;
    f32 h;
    f32 rad;
    f32 angle;
    f32 f27;
    f32 f26;
    f32 yy;
    f32 d0;
    f32 px;
    f32 pz;
    f32 ox;
    f32 oz;
    f32 ang;
    s32 i;

    *(f32*)((u8*)wp + 0x218) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x21C) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x220) = float_0_8041a7a4;
    *(f32*)((u8*)wp + 0x214) = float_0_8041a7a4;
    if (wp->flags & 0x200) {
        *out = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *out = wp->position.y;
        return NULL;
    }
    fa = float_0p1_8041a7e4 + __fabsf(a);
    h = float_0p5_8041a7c4 * wp->wPlayerCollisionBox.y;
    if (wp->baseSpeed != float_0_8041a7a4) {
        angle = *(f32*)((u8*)wp + 0x1A0);
    } else {
        angle = toMovedir(*(f32*)((u8*)wp + 0x1A8));
    }
    rad = float_3p1416_8041a7e0 * angle / float_180_8041a7cc;
    f27 = float_0p25_8041a7d4 * (*(f32*)((u8*)wp + 0x1B4) * (f32)sin(rad));
    up = *(const Vec*)(base + 0x78);
    d0 = h + fa;
    yy = wp->position.y + b + h;
    f26 = float_0p25_8041a7d4 * (*(f32*)((u8*)wp + 0x1B4) * -(f32)cos(rad));
    px = wp->position.x;
    pz = wp->position.z;
    for (i = 0; i < 5; i++) {
        switch (i) {
        case 0:
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
            break;
        case 1:
            ox = f27;
            oz = f26;
            break;
        case 2:
            ox = -f26;
            oz = f26;
            break;
        case 3:
            ox = f26;
            oz = -f27;
            break;
        case 4:
            ox = -f27;
            oz = -f26;
            break;
        }
        p = *(const Vec*)(base + 0x84);
        p.x = px + ox;
        p.y = yy;
        p.z = pz + oz;
        dist = d0;
        hit = marioHitCheckVecOut(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();

            *(f32*)((u8*)wp2 + 0x218) = LND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.z, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            *(f32*)((u8*)wp2 + 0x21C) = float_0_8041a7a4;
            *(f32*)((u8*)wp2 + 0x220) = LND_CENTI(angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * normal.x, float_100_8041a7f8 * normal.y) - float_180_8041a7cc);
            ang = angleABf(float_0_8041a7a4, float_0_8041a7a4, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            *(f32*)((u8*)wp + 0x214) = ang;
            if (__fabsf(LND_CENTI((yy - hitPos.y) - dist)) < float_0p1_8041a7e4) {
                *out = hitPos.y;
                return hit;
            }
        }
    }
    return NULL;
}


f32 marioChkOverhead(void) {
    const u8* base = (const u8*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec v;
    s32 out;
    f32 f8;
    f32 f31;
    HitEntry* hit;

    if (wp->currMotionId == 8) {
        return wp->position.y;
    }
    if (wp->currMotionId == 0x14) {
        v = wp->position;
        f8 = wp->wPlayerCollisionBox.y;
        if (jabaraNoHitChk() == 0) {
            f8 = f8 + float_5_8041a7b8;
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead_jabara(&v, &f8, &out, float_0_8041a7a4);
            if (*(void**)((u8*)wp + 0x1F4) == NULL) {
                *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, (wp->flags & 0x10000) ? *(f32*)((u8*)wp + 0x7C) : float_10_8041a7fc);
            }
        }
    } else if (wp->flags & 0x1000000) {
        v = wp->position;
        *(void**)((u8*)wp + 0x1F4) = marioSearchHead_roll(&v, &f8, float_40_8041a800);
    } else {
        v = wp->position;
        f8 = wp->wPlayerCollisionBox.y;
        f31 = (wp->flags & 0x10000) ? *(f32*)((u8*)wp + 0x7C) : float_10_8041a7fc;
        if (wp->characterId == 2) {
            *(void**)((u8*)wp + 0x1F4) = kpaSearchHead(&v, &f8, f31);
        } else {
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, f31);
        }
    }
    wp->position.x = v.x;
    hit = *(HitEntry**)((u8*)wp + 0x1F4);
    if (hit != NULL) {
        MarioWork* wp2;
        s32 ok;

#ifdef MUST_MATCH
        // Original folds the redundant hit test and the attr test into short-circuit
        // value branches (`bne then; li r0,0; b join`), so write them as inverted ifs.
        if (!(hit != NULL)) {
            ok = 0;
        } else if (!(hitGetAttr(hit) & 0x80000000)) {
            ok = 0;
        } else {
            u8* obj = (u8*)mobjHitObjPtrToPtr(hit);

            if (strcmp((const char*)(obj + 0x15), "MOBJ_PowerUpBlock") == 0 ||
                strcmp((const char*)(obj + 0x15), "MOBJ_SaveBlock") == 0 ||
                strcmp((const char*)(obj + 0x15), "MOBJ_RecoveryBlock") == 0 ||
                strcmp((const char*)(obj + 0x15), "MOBJ_CoinTakenRecoveryBlock") == 0) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
#else
        if (hitGetAttr(hit) & 0x80000000) {
            u8* obj = (u8*)mobjHitObjPtrToPtr(hit);

            if (strcmp((const char*)(obj + 0x15), "MOBJ_PowerUpBlock") == 0 ||
                strcmp((const char*)(obj + 0x15), "MOBJ_SaveBlock") == 0 ||
                strcmp((const char*)(obj + 0x15), "MOBJ_RecoveryBlock") == 0 ||
                strcmp((const char*)(obj + 0x15), "MOBJ_CoinTakenRecoveryBlock") == 0) {
                ok = 1;
            } else {
                ok = 0;
            }
        } else {
            ok = 0;
        }
#endif
        if (ok) {
            wp->baseSpeed = float_0_8041a7a4;
            wp->flags |= 0x80;
        }
        wp->flags |= 0x08000000;
        if (wp->characterId == 2) {
            marioItemGetDisable();
            if (kpaGetBodyStatus() == 0) {
                marioClearJumpPara();
            }
        }
        wp2 = marioGetPtr();
        f31 = wp2->wPlayerCollisionBox.y;
        if (wp2->currMotionId == 0x1A) {
            void* p = partyGetPtr(marioGetPartyId());

            if (p != NULL) {
                f31 = partyGetHeight(p);
            }
        }
        v.y = (f32)(s32)(float_10_8041a7fc * (v.y - f31) + float_0p5_8041a7c4) / float_10_8041a7fc - float_1_8041a7bc;
        if (!(wp->flags & 0x20000) && v.y < *(f32*)((u8*)wp + 0x120)) {
            v.y = *(f32*)((u8*)wp + 0x120);
            if (!(wp->flags & 0x1000000) && wp->currMotionId != 0x19 &&
                wp->currMotionId != 0x18 && wp->currMotionId != 0x1C &&
                wp->characterId != 2 && marioChkSlitEnd() == 0) {
                if (float_0_8041a7a4 == wp->baseSpeed) {
                    if (wp->currMotionId != 0) {
                        marioChgStayMotion();
                    }
                } else {
                    marioChgMot(wp->baseSpeed > wp->mBaseWalkSpeed ? 2 : 1);
                }
            }
        }
        if (wp->currMotionId == 3 || wp->currMotionId == 6) {
            psndSFXOn_3D((const char*)0x156, &wp->position);
            marioClearJumpPara();
            wp->flags &= ~0x000F0000;
            if (wp->characterId != 2) {
                marioChgMot(0xA);
                marioSetFallPara();
            }
            wp->position.y = v.y;
        }
    } else {
        if (!(wp->flags & 0x1000000)) {
            if (f8 >= float_0_8041a7a4 && f8 <= float_5_8041a7b8) {
                wp->flags |= 0x08000000;
            } else {
                wp->flags &= ~0x08000000;
            }
        } else {
            if (f8 >= float_0_8041a7a4 && f8 + wp->wPlayerCollisionBox.y <= float_47_8041a84c) {
                wp->flags |= 0x08000000;
            } else {
                wp->flags &= ~0x08000000;
            }
        }
    }
    return v.y;
}



f32 marioChkOverheadJabara(s32* out, f32* outF) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;

    *out = 0;
    *outF = float_neg1_8041a7dc;
    v = wp->position;
    f8 = wp->wPlayerCollisionBox.y;
    if (wp->currMotionId == 8) {
        return v.y;
    }
    if (jabaraNoHitChk() == 0) {
        f8 = f8 + float_5_8041a7b8;
        *(void**)((u8*)wp + 0x1F4) = marioSearchHead_jabara(&v, &f8, out, float_0_8041a7a4);
        if (*(void**)((u8*)wp + 0x1F4) == NULL) {
            f8 = wp->wPlayerCollisionBox.y + float_5_8041a7b8;
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, float_0_8041a7a4);
        }
    }
    wp->position.x = v.x;
    if (*(void**)((u8*)wp + 0x1F4) != NULL) {
        wp->flags |= 0x08000000;
        v.y = v.y - wp->wPlayerCollisionBox.y - float_0p1_8041a7e4;
        if (wp->currMotionId == 3) {
            marioChgMot(0);
        }
    } else {
        if (f8 > float_0_8041a7a4) {
            *outF = f8;
            wp->flags |= 0x08000000;
        } else {
            wp->flags &= ~0x08000000;
        }
    }
    return v.y;
}


// Inline sqrtf from the SDK's math.h, redeclared here because including math.h clashes with
// math_ext.h's out-of-line sqrt. Same expansion the original used (frsqrte + NaN handling).

// The p/up named-vector copies are inside the loop like marioSearchHead_roll (the `t` temp is
// what makes MWCC emit the original's built-then-copied p at 0x10->0x40); f25 is built from
// sin before cos is called.
// Only FPR colouring remains -- the original keeps a=f22/rad-y0=f23, temps f24-f28,
// nz/ny/nx=f29-f31, ox/oz=f21/f20; ours shares a's f19 with ny100, so the temps land in
// f26-f31 and the normals in f20-f22.
// NONMATCHING (93.09%): only FPR colouring remains (the loop-body ox/oz take f31/f30 and rotate
// the whole long-lived set). Reordering declarations and extending `a`'s live range were tried.
void* marioSearchHead(Vec* pos, f32* outY, f32 a) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up;
    Vec hitPos;
    Vec normal;
    Vec t;
    HitEntry* hit;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 y0;
    f32 f25;
    f32 f24;
    f32 f27;
    f32 f28;
    f32 f26;
    f32 f22;
    f32 rad;
    f32 angle;
    f32 s;
    f32 c;
    f32 dist;
    s32 i;

    if (wp->baseSpeed != float_0_8041a7a4) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->unk1A8);
    }
    rad = float_3p1416_8041a7e0 * angle / float_180_8041a7cc;
    s = (f32)sin(rad);
    f25 = float_0p5_8041a7c4 * ((wp->wPlayerCollisionBox.x - float_0p5_8041a7c4) * s);
    c = (f32)cos(rad);
    f24 = float_0p5_8041a7c4 * ((wp->wPlayerCollisionBox.x - float_0p5_8041a7c4) * -c);
    f27 = -f25;
    f28 = -f24;
    y0 = *outY;
    f26 = float_0p5_8041a7c4 * y0;
    f22 = y0 + a;
    *outY = float_neg1_8041a7dc;
    for (i = 0; i < 5; i++) {
        f32 ox;
        f32 oz;

        if (i == 0) {
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
        } else if (i == 1) {
            ox = f25;
            oz = f24;
        } else if (i == 2) {
            ox = f28;
            oz = f25;
        } else if (i == 3) {
            ox = f24;
            oz = f27;
        } else if (i == 4) {
            ox = f27;
            oz = f28;
        }
        dist = f22;
        t = *(const volatile Vec*)(RO(base, 0xC0, vec3_802c57f0));
        t.x = pos->x + ox;
        t.y = pos->y + f26;
        t.z = pos->z + oz;
        p = t;
        up = *(const volatile Vec*)(RO(base, 0xCC, vec3_802c57fc));
        hit = marioHitCheck2(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2;
            f32 ny100;
            f32 ang;

            nz = normal.z;
            ny = normal.y;
            nx = normal.x;
            wp2 = marioGetPtr();
            ny100 = float_100_8041a7f8 * ny;

            *(f32*)&wp2->field_0x1F0[0x28] =
                (f32)(s32)(float_100_8041a7f8 * (angleABf(float_0_8041a7a4, float_0_8041a7a4,
                                                            float_100_8041a7f8 * nz,
                                                            ny100) -
                                                  float_180_8041a7cc) +
                          float_0p5_8041a7c4) /
                (*(const volatile f32*)&float_100_8041a7f8);
            *(f32*)&wp2->field_0x1F0[0x2C] = float_0_8041a7a4;
            *(f32*)&wp2->field_0x1F0[0x30] =
                (f32)(s32)((*(const volatile f32*)&float_100_8041a7f8) * (angleABf(float_0_8041a7a4, float_0_8041a7a4,
                                                            float_100_8041a7f8 * nx,
                                                            ny100) -
                                                  float_180_8041a7cc) +
                          float_0p5_8041a7c4) /
                float_100_8041a7f8;
            ang = angleABf(float_0_8041a7a4, float_0_8041a7a4, (f32)sqrtf(nx * nx + nz * nz), -ny);
            if (!(wp->flags & 0x1000000)) {
                f32 lim = (wp->dispFlags & 0x1000000) ? float_75_8041a840 : float_50_8041a844;

                if (ang <= float_180_8041a7cc - lim && ang >= lim) {
                    hit = NULL;
                }
            }
        }
        if (wp->flags & 0x1000000) {
            if (hit != NULL) {
                f32 y = (f32)(s32)(float_100_8041a7f8 * hitPos.y + f26) / float_100_8041a7f8;

                y = y - float_0p05_8041a848 * y0;
                hitPos.y = y;
                if (pos->y + y0 >= y) {
                    pos->y = y;
                    *outY = float_0_8041a7a4;
                    return hit;
                } else {
                    *outY = (y - pos->y) - y0;
                    return NULL;
                }
            }
        } else {
            if (hit != NULL) {
                f32 y = (f32)(s32)(float_100_8041a7f8 * hitPos.y + f26) / float_100_8041a7f8;

                hitPos.y = y;
                if (pos->y + y0 >= y) {
                    pos->y = y;
                    *outY = float_0_8041a7a4;
                    return hit;
                } else {
                    *outY = (y - pos->y) - y0;
                    return NULL;
                }
            }
        }
    }
    return NULL;
}


// The sin/cos colouring and the hit-check join now match. The ROUND_MILLI register colours are
// reproduced with volatile reads of the float constants (marked MUST_MATCH): the original reloads
// float_0 per component and float_1000 for the y component, which MW only does if the loads are
// not CSE-able. The dead out2 copy and the `hit=0` join are also under MUST_MATCH.
// Matches 100%. The p read is forced volatile (cast below) so the compiler re-loads it each
// iteration like the original; `dir` is built as a compound literal so its first word is not
// scheduled one slot early (the named-vector read was).
void* marioSearchHead_roll(Vec* pos, f32* outY, f32 a) {
    MarioWork* wp = marioGetPtr();
    f32 ox;
    f32 oz;
    f32 ss;
    f32 cc;
    Vec out1;
    Vec out2;
    Vec p;
    Vec dir;
    HitCheckQuery query;
    HitEntry* hit;
    f32 y;
    f32 dist;
    s32 i;

    *outY = float_neg1_8041a7dc;
    y = wp->wPlayerCollisionBox.y;
    dist = y + a;
    for (i = 0; i < 5; i++) {
        f32 rad;

        switch (i) {
        case 0:
            ox = float_0_8041a7a4;
            oz = float_0_8041a7a4;
            break;
        case 1:
            rad = float_3p1416_8041a7e0 * (float_90_8041a81c + wp->directionView) / float_180_8041a7cc;
            ss = (f32)sin(rad);
            ox = float_9_8041a83c * ss;
            cc = (f32)cos(rad);
            oz = float_9_8041a83c * -cc;
            movePos(&ox, &oz, float_3_8041a7f0, wp->directionView);
            break;
        case 2:
            rad = float_3p1416_8041a7e0 * (wp->directionView - float_90_8041a81c) / float_180_8041a7cc;
            ss = (f32)sin(rad);
            ox = float_9_8041a83c * ss;
            cc = (f32)cos(rad);
            oz = float_9_8041a83c * -cc;
            movePos(&ox, &oz, float_3_8041a7f0, wp->directionView);
            break;
        case 3:
            rad = float_3p1416_8041a7e0 * (float_90_8041a81c + wp->directionView) / float_180_8041a7cc;
            ss = (f32)sin(rad);
            ox = float_9_8041a83c * ss;
            cc = (f32)cos(rad);
            oz = float_9_8041a83c * -cc;
            movePos(&ox, &oz, float_3_8041a7f0, float_180_8041a7cc + wp->directionView);
            break;
        case 4:
            rad = float_3p1416_8041a7e0 * (wp->directionView - float_90_8041a81c) / float_180_8041a7cc;
            ss = (f32)sin(rad);
            ox = float_9_8041a83c * ss;
            cc = (f32)cos(rad);
            oz = float_9_8041a83c * -cc;
            movePos(&ox, &oz, float_3_8041a7f0, float_180_8041a7cc + wp->directionView);
            break;
        }
#ifdef MUST_MATCH
        p = *(const volatile Vec*)&vec3_802c5808;
        dir = (Vec){ 0.0f, 1.0f, 0.0f };
#else
        p = vec3_802c5808;
        dir = vec3_802c5814;
#endif
        p.x = pos->x + ox;
        p.y = float_1_8041a7bc + pos->y;
        p.z = pos->z + oz;
        query.targetPos = p;
        query.targetDir = dir;
        query.targetDistance = dist;

        if (marioGetPtr()->currMotionId == 0x1C) {
            hit = hitCheckVecFilter(&query, chkfilterVecVivian);
        } else if (marioGetPtr()->flags & 0x1000000) {
            hit = hitCheckVecFilter(&query, chkfilterVecRoll);
        } else {
            hit = hitCheckVecFilter(&query, chkfilterVec);
        }
#ifdef MUST_MATCH
        // The original re-materialises hit=0 at the join of the two null checks and keeps a dead
        // out2 copy guarded by a null test on the local's address; reproduce both.
        if (hit == NULL) {
            hit = NULL;
            goto check_hit2;
        }
        out1 = query.hitPos;
        out1.x = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * out1.x +
                            (out1.x >= *(volatile const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                  *(const f32*)&float_1000_8041a7a0;
        out1.y = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * out1.y +
                            (out1.y >= *(volatile const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                  *(const f32*)&float_1000_8041a7a0;
        out1.z = (f32)(s32)(*(const f32*)&float_1000_8041a7a0 * out1.z +
                            (out1.z >= *(volatile const f32*)&float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                  *(const f32*)&float_1000_8041a7a0;
        if (&out2 != NULL) {
            out2 = query.hitNormal;
        }
    check_hit2:
        if (hit != NULL) {
#else
        if (hit != NULL) {
            out1 = query.hitPos;
            out1.x = (f32)(s32)(float_1000_8041a7a0 * out1.x +
                                (out1.x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                      float_1000_8041a7a0;
            out1.y = (f32)(s32)(float_1000_8041a7a0 * out1.y +
                                (out1.y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                      float_1000_8041a7a0;
            out1.z = (f32)(s32)(float_1000_8041a7a0 * out1.z +
                                (out1.z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                      float_1000_8041a7a0;
            out2 = query.hitNormal;
        }
        if (hit != NULL) {
#endif
            if (pos->y + y >= out1.y) {
                pos->y = out1.y - y - float_1_8041a7bc;
                *outY = float_0_8041a7a4;
                return hit;
            } else {
                f32 d = (out1.y - pos->y) - y;

                if (*outY < float_0_8041a7a4) {
                    *outY = d;
                } else if (*outY > d) {
                    *outY = d;
                }
            }
        }
    }
    return NULL;
}


// 100%. `up` must be built as a compound literal (not the named vec3_802c5820 read) so MWCC
// schedules its loads after y = a2 + a; the named read starts the copy one slot early and
// costs the prologue interleave (96.55%). The literal is the MUST_MATCH spelling.
void* marioSearchHead_jabara(Vec* pos, f32* outY, s32* out, f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up;
    Vec out1;
    Vec out2;
    f32 outF;
    f32 a2;
    f32 y;
    f32 radius;
    HitEntry* hit;
    s32 i;

    *out = 0;
    a2 = *outY;
    radius = float_neg23p7_8041a834;
    *outY = float_neg1_8041a7dc;
    y = a2 + a;
#ifdef MUST_MATCH
    up = (Vec){ 0.0f, 1.0f, 0.0f };
#else
    up = vec3_802c5820;
#endif

    for (i = 0; i < 8; i++) {
        outF = y;
        p.x = pos->x + radius * (f32)sin(float_3p1416_8041a7e0 * wp->targetYaw / float_180_8041a7cc);
        p.z = pos->z - radius * (f32)cos(float_3p1416_8041a7e0 * wp->targetYaw / float_180_8041a7cc);
        p.y = float_0p5_8041a7c4 + pos->y;
        hit = marioHitCheck2(&p, &up, &out1, &out2, &outF);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x40000) {
                if (pos->y + a2 >= out1.y) {
                    pos->y = out1.y;
                    *outY = float_0_8041a7a4;
                    *out = (p.z - pos->z < float_0_8041a7a4) ? -1 : 1;
                    return hit;
                } else {
                    *outY = (out1.y - pos->y) - a2;
                }
            } else if (i == 3) {
                if (pos->y + a2 >= out1.y) {
                    pos->y = out1.y;
                    *outY = float_0_8041a7a4;
                    *out = 0;
                    return hit;
                } else {
                    *outY = (out1.y - pos->y) - a2;
                }
            }
        }
        radius = radius + float_5p8_8041a838;
    }
    return NULL;
}
const Vec vec3_802c582c = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802c5838 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5844 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5850 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c585c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5868 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c5874 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c5880 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c588c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5898 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58a4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58b0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58bc = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c58c8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58d4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58e0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58ec = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c58f8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5904 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5910 = { 0.0f, 0.0f, 0.0f };

// prototypes for in-file functions

void marioCheckWall(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    Vec v;
    f32 f8;
    s32 r;

    *(f32*)&wp->field_0x1F0[0x20] = float_1000_8041a7a0;
    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        if (*(f32*)((u8*)wp + 0x1C8) < a) {
            *(f32*)((u8*)wp + 0x1C8) = a;
            *(f32*)((u8*)wp + 0x1CC) = b;
            *(s32*)((u8*)wp + 0x1D0) = 0xE;
        } else {
            s32 t = *(s32*)((u8*)wp + 0x1D0) - 1;

            *(s32*)((u8*)wp + 0x1D0) = t;
            if (t <= 0) {
                *(s32*)((u8*)wp + 0x1D0) = 0;
                *(f32*)((u8*)wp + 0x1C8) = a;
                *(f32*)((u8*)wp + 0x1CC) = b;
            }
        }
        *(f32*)((u8*)wp + 0x19C) = *(f32*)((u8*)wp + 0x1CC);
    } else {
#ifdef MUST_MATCH
        // volatile forces a fresh float_0 load; without it MWCC CSEs the constant into
        // f1, which also flips the float_0/float_1000 allocation at the top.
        *(f32*)&wp->field_0x1C0[0x8] = *(volatile const f32*)&float_0_8041a7a4;
#else
        *(f32*)((u8*)wp + 0x1C8) = float_0_8041a7a4;
#endif
    }

    if (a == float_0_8041a7a4) {
        marioSearchFrontWall_staying(b);
        dou10_yoko_yari((f32*)&wp->field_0x1F0[0x20]);
        return;
    }

    v = wp->position;
    *(s32*)((u8*)wp + 0x20C) = 0;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        if (*(void**)((u8*)wp + 0x20C) != NULL) {
            *(void**)((u8*)wp + 0x1E0) = *(void**)((u8*)wp + 0x20C);
        } else {
            *(void**)((u8*)wp + 0x1E0) = hit;
        }
        if (marioChkBero(hit)) {
            if (a != float_0_8041a7a4) {
                wp->hitobjPush = hit;
                *(void**)((u8*)wp + 0x1F0) = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
        } else {
            s32 t;

            *(f32*)&wp->field_0x1F0[0x20] = f8;
            *(void**)((u8*)wp + 0x1F0) = hit;
            t = wp->field_0x3E[1] + 1;
            wp->field_0x3E[1] = t;
            if ((s8)t >= 0x14) {
                wp->field_0x3E[1] = 0x14;
                if ((hitGetAttr(hit) & 0x80) == 0) {
                    r = 1;
                } else {
                    MarioWork* wp2 = marioGetPtr();

                    r = wp2->currMotionId == 0x19;
                }
                if (r != 0) {
                    wp->hitobjPush = hit;
                }
            }
            wp->position = v;
        }
    } else {
        wp->position = v;
    }
    marioChkWallAround(&wp->position, 0, a, b, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    dou10_yoko_yari((f32*)&wp->field_0x1F0[0x20]);
}

void marioSearchFrontWall_staying(f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;
    f32 f8b;
    f32 halfx;
    f32 angle;
    HitEntry* hit;

    *(s8*)((u8*)wp + 0x3F) = 0;
    halfx = float_0p5_8041a7c4 * wp->wPlayerCollisionBox.x;
    if (wp->flags & 0x100000) {
        halfx = float_0_8041a7a4;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, halfx, wp->directionView - wp->targetYaw, wp->wPlayerCollisionBox.z);
    if (wp->currMotionId != 0x13) {
        *(void**)((u8*)wp + 0x1E0) = hit;
    } else {
        *(void**)((u8*)wp + 0x1E0) = NULL;
    }

    if (hit == NULL) {
        angle = toMovedir(wp->unk1A8);
        v = wp->position;
        hit = searchFrontWall(&v, &f8, halfx, angle, wp->wPlayerCollisionBox.z);
        if (wp->currMotionId != 0x13) {
            *(void**)((u8*)wp + 0x1E0) = hit;
        } else {
            *(void**)((u8*)wp + 0x1E0) = NULL;
        }
    } else {
        if (strcmp(gp->currentMapName, "dou_10") == 0) {
            if (vivianGetStatus() != 2 && vivianGetStatus() != 1 && vivianGetStatus() != 3) {
                angle = toMovedir(wp->unk1A8);
                v = wp->position;
                movePos(&v.x, &v.z, float_0p5_8041a7c4 * wp->wPlayerCollisionBox.x, angle);
                angle = angle + float_180_8041a7cc;
                hit = searchFrontWall(&v, &f8b, wp->wPlayerCollisionBox.x, angle, wp->wPlayerCollisionBox.z);
                if (hit != NULL && wp->wPlayerCollisionBox.x >= f8b) {
                    *(void**)((u8*)wp + 0x1E0) = hit;
                    f8 = float_0p5_8041a7c4 * f8b;
                }
            }
        }
    }

    if (*(void**)((u8*)wp + 0x1E0) != NULL) {
        *(f32*)((u8*)wp + 0x210) = f8;
    }

    if (*(u16*)((u8*)wp + 0x248) & 0x100) {
        *(void**)((u8*)wp + 0x1DC) = NULL;
        v = wp->position;
        hit = searchFrontWall(&v, &f8, halfx, wp->directionView, wp->wPlayerCollisionBox.z);
        if (hit != NULL) {
            *(void**)((u8*)wp + 0x1DC) = *(void**)((u8*)wp + 0x1E0);
        }
    }

    v = wp->position;
    marioChkWallAround(&v, 0, float_0_8041a7a4, a, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    wp->position.x = v.x;
    wp->position.z = v.z;
}


// The targetDistance round colours float_1000 f3 / the fctiwz double f2 / the early
// out1.x load f1 where the original uses f2 / f1 / f4 (a global allocation-order
// difference). The extra MATCH_F32 read of float_1000 on the out1.x divisor recovers
// the original's per-round reload and 0.62% over the plain form. Conversions reference
// a local @865 double rather than the shared double_to_int_802c5920 (name only, no score
// cost; same project-wide limitation noted in window/win_*.c).
// NONMATCHING (99.30%): only FPR colouring and the local @865 double differ from the original.
void marioCheckWall_roll(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;
    f32 f31;
    HitEntry* hit;
    HitCheckQuery query;
    s32 r;

    *(f32*)&wp->field_0x1F0[0x20] = float_1000_8041a7a0;
    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        if (*(f32*)&wp->field_0x1C0[0x8] < a) {
            *(f32*)&wp->field_0x1C0[0x8] = a;
            *(f32*)&wp->field_0x1C0[0xC] = b;
            *(s32*)&wp->field_0x1C0[0x10] = 0xE;
        } else {
            s32 t = *(s32*)&wp->field_0x1C0[0x10] - 1;

            *(s32*)&wp->field_0x1C0[0x10] = t;
            if (t <= 0) {
                *(s32*)&wp->field_0x1C0[0x10] = 0;
                *(f32*)&wp->field_0x1C0[0x8] = a;
                *(f32*)&wp->field_0x1C0[0xC] = b;
            }
        }
    } else {
        *(f32*)&wp->field_0x1C0[0x8] = MATCH_F32(float_0_8041a7a4);
    }

    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        *(f32*)&wp->unk19C[0] = *(f32*)&wp->field_0x1C0[0xC];
    }

    if (wp->flags & 0x10000) {
        Vec v2;
        Vec dir;
        Vec out1;
        Vec out2;
        HitEntry* headHit;

        v2 = wp->position;
        dir = vec3_802c582c;
        f31 = float_30_8041a82c;
        query.targetPos = v2;
        query.targetDir = dir;
        query.targetDistance = f31;
        if (marioGetPtr()->currMotionId == 0x1C) {
            headHit = hitCheckVecFilter(&query, chkfilterVecVivian);
        } else if (marioGetPtr()->flags & 0x1000000) {
            headHit = hitCheckVecFilter(&query, chkfilterVecRoll);
        } else {
            headHit = hitCheckVecFilter(&query, chkfilterVec);
        }
        if (headHit == NULL) {
            headHit = NULL;
        } else {
            f31 = (f32)(s32)(float_1000_8041a7a0 * query.targetDistance +
                             (query.targetDistance >= MATCH_F32(float_0_8041a7a4) ? float_0p5_8041a7c4
                                                                       : float_neg0p5_8041a7c8)) /
                  float_1000_8041a7a0;
            {
                Vec* pOut = &out1;
                out1 = query.hitPos;
                // MATCH: the volatile read of the divisor forces MWCC to reload
                // float_1000 for this round as the original does (plain form: 98.68).
                pOut->x = (f32)(s32)(float_1000_8041a7a0 * pOut->x +
                                     (pOut->x >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                           MATCH_F32(float_1000_8041a7a0);
                pOut->y = (f32)(s32)(float_1000_8041a7a0 * pOut->y +
                                     (pOut->y >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                           float_1000_8041a7a0;
                pOut->z = (f32)(s32)(float_1000_8041a7a0 * pOut->z +
                                     (pOut->z >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /
                           float_1000_8041a7a0;
            }
#ifdef MUST_MATCH
            if (&out2 != NULL) {
                out2 = query.hitNormal;
            }
#else
            out2 = query.hitNormal;
#endif
        }
        if (headHit != NULL && f31 > float_16p8_8041a830) {
            headHit = NULL;
        }
        *(void**)((u8*)wp + 0x1F4) = headHit;
        if (headHit != NULL) {
            wp->position.y = out1.y - wp->wPlayerCollisionBox.y;
        }
    }

    if (a == float_0_8041a7a4) {
        marioSearchFrontWall_staying(b);
        return;
    }

    if (*(void**)((u8*)wp + 0x1F4) != NULL) {
        wp->baseSpeed = float_0_8041a7a4;
        a = float_0_8041a7a4;
        *(f32*)&wp->field_0x7C[0] = float_0_8041a7a4;
        *(f32*)&wp->field_0x7C[4] = float_0_8041a7a4;
        *(f32*)&wp->field_0x7C[8] = float_0_8041a7a4;
        *(f32*)&wp->field_0x7C[0xC] = float_0_8041a7a4;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        *(void**)((u8*)wp + 0x1E0) = hit;
        r = hitGetAttr(hit);
        {
            MarioWork* wp2 = marioGetPtr();

            if ((r & 0x80000) && !(wp2->flags & 0x1000000)) {
                r = 0;
            } else if ((r & 0x80) == 0) {
                r = (r & 0x800000) >> 23;
            } else {
                r = (wp2->currMotionId == 0x19);
            }
        }
        if (r != 0) {
            if (a != float_0_8041a7a4) {
                wp->hitobjPush = hit;
                *(void**)((u8*)wp + 0x1F0) = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
        } else {
            s32 t;

            *(f32*)&wp->field_0x1F0[0x20] = f8;
            *(void**)((u8*)wp + 0x1F0) = hit;
            t = wp->field_0x3E[1] + 1;
            wp->field_0x3E[1] = t;
            if ((s8)t >= 0x14) {
                wp->field_0x3E[1] = 0x14;
                if ((hitGetAttr(hit) & 0x80) == 0) {
                    r = 1;
                } else {
                    MarioWork* wp3 = marioGetPtr();

                    r = (wp3->currMotionId == 0x19);
                }
                if (r != 0) {
                    wp->hitobjPush = hit;
                }
            }
            wp->position = v;
        }
    } else {
        wp->position = v;
    }

    v = wp->position;
    marioChkWallAround(&v, 0, a, b, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    wp->position.x = v.x;
    wp->position.z = v.z;
}


#define ROUND_MILLI(v)                                                                        \
    ((f32)(s32)(float_1000_8041a7a0 * (v) +                                                   \
                ((v) >= float_0_8041a7a4 ? float_0p5_8041a7c4 : float_neg0p5_8041a7c8)) /      \
     float_1000_8041a7a0)

// The angle fields are rounded to 0.01 with an unconditional +0.5 (different from
// ROUND_MILLI's 1000 and sign-dependent bias).
#define ROUND_CENTI(v)                                                                        \
    ((f32)(s32)(float_100_8041a7f8 * (v) + float_0p5_8041a7c4) / float_100_8041a7f8)



// NONMATCHING (98.27%): the caller builds `query` (via marioHitCheckVecRound) so
// query.targetPos = *pos is written first, reproducing the original's load/store order. The
// remaining diffs are GPR naming in that build and the hitPos rounding's f3/f4 colouring.
void* marioChkFront(f32* out, Vec* pos, f32 angle, f32 dist) {
    HitCheckQuery query;
    Vec hitPos;
    Vec hitNormal;
    Vec dir;
    HitEntry* hit;
    f32 distance;
    f32 s;
    f32 c;

    sincosf(&s, &c, angle);
    query.targetPos = *pos;
    dir = vec3_802c5838;
    dir.x = s;
    dir.z = c;
    query.targetDir = dir;
    query.targetDistance = dist;
    distance = dist;
    hit = marioHitCheckVecRound(&query, &hitPos, &hitNormal, &distance);
    if (hit != NULL && distance <= dist) {
        *out = distABf(pos->x, pos->z, hitPos.x, hitPos.z);
        return hit;
    }
    *out = float_neg1_8041a7dc;
    return NULL;
}

// NONMATCHING (86.00%): the three passes build a `HitCheckQuery` local and call `marioHitCheckVecRound` (the caller-built shape `marioChkFront` uses), which reproduces the 3 query slots and the distance/hitPos/normal rounding; calls and control flow match. The pass-2 direction is built through `makeDirVec` (a by-value inline that starts from the named rodata vec) so MWCC materialises the original's built-then-copied `Vec` slot. What remains is register colouring, a 0x10 frame gap (0x240 vs 0x250 - the original keeps a second dead `Vec` copy in pass 1 that the inliner drops).
HitEntry* searchFrontWall(Vec* pos, f32* out, f32 a, f32 b, f32 c) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    HitCheckQuery query;
    HitCheckQuery query2;
    HitCheckQuery query3;
    Vec dir;
    Vec dir2;
    Vec dir3;
    Vec bestNorm;
    Vec p;
    Vec hitPos;
    Vec norm;
#ifdef MUST_MATCH
    Vec dtmp;
    Vec dtmp2;
#endif
    HitEntry* hit;
    HitEntry* best;
    HitEntry* hit2;
    f32 sin;
    f32 cos;
    f32 sin2;
    f32 cos2;
    f32 half[3];
    f32 dist;
    f32 curDist;
    f32 bestDist;
    f32 deltaX;
    f32 deltaZ;
    s32 count;
    s32 i;

    if (vivianGetStatus() == 2) {
        return NULL;
    }
    sincosf(&sin, &cos, b);
    if (marioChkSts(0x10) != 0 && strcmp(gp->currentMapName, "eki_04") == 0) {
        if (__fabsf(*(f32*)&wp->field_0x1F0[0x28]) >= float_45_8041a7b4 ||
            __fabsf(*(f32*)&wp->field_0x1F0[0x30]) >= float_45_8041a7b4) {
            pos->x += a * sin;
            pos->z += a * cos;
            return NULL;
        }
    }
    count = 2;
    if (wp->characterId == 2) {
        if (*(u32*)wp->field_0x14 & 1) {
            count = 3;
            half[0] = float_0p75_8041a7d8 * wp->wPlayerCollisionBox.y;
            half[1] = float_0p5_8041a7c4 * wp->wPlayerCollisionBox.y;
            half[2] = float_11_8041a7ac;
        } else {
            half[0] = float_0p75_8041a7d8 * wp->wPlayerCollisionBox.y;
            half[1] = float_11_8041a7ac;
        }
    } else if (wp->currMotionId == 0x1A) {
        MarioWork* wp2 = marioGetPtr();
        f32 marioHeight = wp2->wPlayerCollisionBox.y;

        if (wp2->currMotionId == 0x1A) {
            PartyEntry* party = partyGetPtr(marioGetPartyId());

            if (party != NULL) {
                marioHeight = partyGetHeight(party);
            }
        }
        half[0] = float_0p75_8041a7d8 * marioHeight;
        half[1] = float_11_8041a7ac;
    } else if ((wp->flags & 0x1000000) == 0) {
        half[0] = float_27p75_8041a820;
        half[1] = float_11_8041a7ac;
        if (wp->airTimer != 0) {
            half[2] = float_0p1_8041a7e4;
            count = 3;
        }
    } else {
        half[0] = float_12_8041a824;
        half[1] = float_11_8041a7ac;
        if (wp->airTimer != 0) {
            half[2] = float_7_8041a828;
            count = 3;
        }
    }
    dist = float_0p5_8041a7c4 * c;
    p = *pos;
    dir = *(const Vec*)RO(base, 0x114, vec3_802c5844);
    dir.x = sin;
    dir.z = cos;
    curDist = dist + a;
    bestDist = curDist;
    best = NULL;
    for (i = 0; i < count; i++) {
        p.y = pos->y + half[i];
        query.targetPos = p;
        query.targetDir = dir;
        query.targetDistance = dist + a;
        curDist = dist + a;
        hit = marioHitCheckVecRound(&query, &hitPos, &norm, &curDist);
        if (hit != NULL) {
            if (chkBalloon(hit) != 0) {
                *(void**)((u8*)wp + 0x20C) = hit;
            }
            if (best == NULL || curDist < bestDist) {
                bestDist = curDist;
                best = hit;
                bestNorm = norm;
            }
        }
    }
    if (best != NULL) {
        f32 nx = bestNorm.x;
        f32 nz = bestNorm.z;
        f32 dx = sin * a;
        f32 dz = cos * a;
        f32 dot = dx * nx + dz * nz;

        deltaX = float_0p5_8041a7c4 * (dx - dot * nx);
        deltaZ = float_0p5_8041a7c4 * (dz - dot * nz);
    }
    sincosf(&sin2, &cos2, float_45_8041a7b4 + b);
#ifdef MUST_MATCH
    dtmp2 = makeDirVec((const Vec*)RO(base, 0x120, vec3_802c5850), sin2, cos2);
    dir2 = dtmp2;
#else
    dir2 = *(const Vec*)RO(base, 0x120, vec3_802c5850);
    dir2.x = sin2;
    dir2.z = cos2;
#endif
    hit2 = NULL;
    for (i = 0; i < count; i++) {
        p.y = pos->y + half[i];
        query2.targetPos = p;
        query2.targetDir = dir2;
        query2.targetDistance = dist;
        curDist = dist;
        hit = marioHitCheckVecRound(&query2, NULL, NULL, &curDist);
        hit2 = hit;
        if (hit2 != NULL) {
            break;
        }
    }
    if (hit2 != NULL) {
        sincosf(&sin2, &cos2, b - float_45_8041a7b4);
        dir3 = *(const Vec*)RO(base, 0x12C, vec3_802c585c);
        dir3.x = sin2;
        dir3.z = cos2;
        hit2 = NULL;
        for (i = 0; i < count; i++) {
            p.y = pos->y + half[i];
            query3.targetPos = p;
            query3.targetDir = dir3;
            query3.targetDistance = dist;
            curDist = dist;
            hit = marioHitCheckVecRound(&query3, NULL, NULL, &curDist);
            hit2 = hit;
        }
    }
    if (best != NULL) {
        if (hit2 == NULL) {
            *out = curDist;
            pos->x += deltaX;
            pos->z += deltaZ;
        } else {
            *out = float_0_8041a7a4;
        }
        return best;
    }
    if (hit2 == NULL) {
        pos->x += a * sin;
        pos->z += a * cos;
    }
    return NULL;
}

// MATCHING: `goto out` gives the single shared `mr r3,r28` exit the original has; the
// rounding is the same MUST_MATCH reload form as dou10_yoko_yari2.
HitEntry* dou10_yoko_yari(f32* out) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec tmp;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 f8;
    f32 dist100;
    f32 dist;

    if (wp->currMotionId == 0x16) {
        return NULL;
    }
    toMovedir(wp->unk1A8);
    hit = *(HitEntry**)((u8*)wp + 0x1E0);
    if (hit != NULL) {
        return hit;
    }
    pos = wp->position;
    if (strcmp(gp->currentMapName, "dou_10") != 0) {
        goto out;
    }
    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        goto out;
    }
    if (hit != NULL) {
        goto out;
    }
    dist = (wp->flags & 0x100000) ? float_2_8041a7e8 : float_8_8041a814;
    tmp = pos;
    tmp.y = tmp.y + float_60_8041a808;
    dir = vec3_802c5868;
    dist100 = float_100_8041a7f8;
    hit = marioHitCheckVecOut(&tmp, &dir, &hitPos, &hitNormal, &dist100);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, dist, float_270_8041a818);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, dist, float_90_8041a81c);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }
out:
    return hit;
}

// MATCHING: marioHitCheckVecOut inlines marioHitCheckVec with the `c != NULL` test folded
// away, and the MUST_MATCH casts (on the 0.0f tests and both float_1000 uses) make MWCC
// reload those constants once per component exactly as the original does.
HitEntry* dou10_yoko_yari2(f32* out) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec tmp;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;
    f32 f8;

    toMovedir(wp->unk1A8);
    pos = wp->position;
    if (strcmp(gp->currentMapName, "dou_10") != 0) {
        return NULL;
    }
    tmp = pos;
    tmp.y = tmp.y + float_60_8041a808;
    dir = vec3_802c5874;
    dist = float_100_8041a7f8;
    hit = marioHitCheckVecOut(&tmp, &dir, &hitPos, &hitNormal, &dist);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, float_8_8041a814, float_270_8041a818);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, float_8_8041a814, float_90_8041a81c);
    hit = searchFrontWall(&pos, &f8, float_20_8041a810, float_0_8041a7a4, float_20_8041a810);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = float_0p8_8041a80c;
    }
    return hit;
}


// Matches. The original builds targetPos by first copying a named zero vector
// (vec3_802c5880 / vec3_802c5898) and then overwriting the components, and adds the
// float_5 clearance after the copy; both shapes are needed for the exact stack layout.
// 0x1E0 is unnamed MarioWork padding.
HitEntry* N_dou10_yoko_yari3(void) {
    const char* base = (const char*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    f32 sn;
    f32 cs;
    f32 vx;
    f32 vy;
    f32 vz;
    Vec v;
    Vec dir;
    Vec v2;
    Vec dir2;
    HitCheckQuery query;
    HitEntry* hit;

    if (strcmp(gp->currentMapName, ROS(base, 0x210, "dou_10")) != 0) {
        return NULL;
    }

    vx = wp->position.x;
    vy = wp->position.y;
    vz = wp->position.z;
    movePos(&vx, &vz, float_10_8041a7fc, wp->directionView);
    v = *(const Vec*)(RO(base, 0x150, vec3_802c5880));
    v.x = vx;
    v.y = vy;
    v.z = vz;
    query.targetPos = v;
    query.targetPos.y = query.targetPos.y + float_5_8041a7b8;
    sincosf(&sn, &cs, float_0_8041a7a4);
    dir = *(const Vec*)(RO(base, 0x15C, vec3_802c588c));
    dir.x = sn;
    dir.z = cs;
    query.targetDir = dir;
    query.targetDistance = float_40_8041a800;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        return hit;
    }
    vx = wp->position.x;
    vy = wp->position.y;
    vz = wp->position.z;
    movePos(&vx, &vz, float_neg10_8041a804, wp->directionView);
    v2 = *(const Vec*)(RO(base, 0x168, vec3_802c5898));
    v2.x = vx;
    v2.y = vy;
    v2.z = vz;
    query.targetPos = v2;
    query.targetPos.y = query.targetPos.y + float_5_8041a7b8;
    sincosf(&sn, &cs, float_0_8041a7a4);
    dir2 = *(const Vec*)(RO(base, 0x174, vec3_802c58a4));
    dir2.x = sn;
    dir2.z = cs;
    query.targetDir = dir2;
    query.targetDistance = float_40_8041a800;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
    }
    return hit;
}

// The original loads `base` (vec3_802c5730) early and reads targetPos at +0x180 and
// targetDir at +0x18c; `n`/`hitPos` are declared volatile to force the same stack
// spills the original has (kept only under MUST_MATCH).
// The 0.01-rounded angle fields use 100.0 with an unconditional +0.5 (ROUND_CENTI); using
// ROUND_MILLI (1000, sign-dependent) there was the main miss (73.93%). A top-declared
// `wp2 = marioGetPtr()` for the field writes (the inline-declaration form makes MWCC loop
// forever) plus non-volatile hitPos/n locals (the volatile forced a different copy/round
// schedule) brought it to 86.93%. Using the literal `float_100_8041a7f8` in the two `angleABf` arguments
// and a searched FPR declaration order (`py, px, off, s, rad, pz, h, angle, c`) reached 87.14%.
// Still differs: the int->float magic double and the five sqrtf coefficient doubles are not
// folded into the rodata base pool (routing sqrtf through base hangs MWCC here); the
// callee-saved GPR set differs as a consequence (ours keeps hit in r30 and reuses r30 for
// the base, so the hit test is `cmplwi r30` instead of the original's `mr r29,r3`); and the
// ROUND_CENTI float_100 loads are hoisted into f31/f30 (callee-saved) instead of the
// original's volatile f7/f5 reloads, which costs `h` its f31.
// NONMATCHING (87.28%): only the GPR/FPR colouring above remains.
HitEntry* marioChkFrontStep(f32* outY, f32* outDist, f32* outAngle) {
#ifdef MUST_MATCH
    const u8* base = (const u8*)&vec3_802c5730;
#endif
    MarioWork* wp = marioGetPtr();
    MarioWork* wp2;
    HitCheckQuery query;
    HitEntry* hit;
    Vec hitPos;
    Vec n;
    Vec v;
    f32 py;
    f32 px;
    f32 off;
    f32 s;
    f32 rad;
    f32 pz;
    f32 h;
    f32 angle;
    f32 c;

    *(f32*)&wp->field_0x1F0[0x28] = float_0_8041a7a4;
    *(f32*)&wp->field_0x1F0[0x2C] = float_0_8041a7a4;
    *(f32*)&wp->field_0x1F0[0x30] = float_0_8041a7a4;
    *(f32*)&wp->field_0x1F0[0x24] = float_0_8041a7a4;
    if (wp->flags & 0x1000000) {
        h = float_18p5_8041a7ec;
    } else {
        h = float_0p5_8041a7c4 * wp->wPlayerCollisionBox.y;
    }
    py = wp->position.y;
    *outY = wp->position.y;
    *outDist = float_neg1_8041a7dc;
    if (wp->baseSpeed != float_0_8041a7a4) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->unk1A8);
    }
    angle = revise360((f32)(s32)(float_0p5_8041a7c4 + angle));
    if (wp->flags & 0x1000000) {
        off = float_3_8041a7f0;
    } else {
        off = float_0p265_8041a7f4 * wp->wPlayerCollisionBox.x;
    }
    rad = (float_3p1416_8041a7e0 * angle) / float_180_8041a7cc;
    s = (f32)sin((f64)rad);
    c = (f32)cos((f64)rad);
    px = wp->position.x;
    py = wp->position.y;
#ifdef MUST_MATCH
    v = *(const Vec*)(base + 0x180);
#else
    v = vec3_802c58b0;
#endif
    {
        f32 t = off * s;
        v.x = px + t;
        v.y = py + h;
        pz = wp->position.z;
        t = off * -c;
        v.z = pz + t;
    }
    query.targetPos = v;
#ifdef MUST_MATCH
    query.targetDir = *(const Vec*)(base + 0x18c);
#else
    query.targetDir = vec3_802c58bc;
#endif
    query.targetDistance = float_0p1_8041a7e4 + (float_11_8041a7ac + h);
    if (marioGetPtr()->currMotionId == 0x1C) {
        hit = hitCheckVecFilter(&query, chkfilterVecVivian);
    } else if (marioGetPtr()->flags & 0x1000000) {
        hit = hitCheckVecFilter(&query, chkfilterVecRoll);
    } else {
        hit = hitCheckVecFilter(&query, chkfilterVec);
    }
    if (hit != NULL) {
        hitPos = query.hitPos;
        hitPos.x = ROUND_MILLI(hitPos.x);
        hitPos.y = ROUND_MILLI(hitPos.y);
        hitPos.z = ROUND_MILLI(hitPos.z);
#ifdef MUST_MATCH
        if (&n != NULL) {
            n = query.hitNormal;
        }
#else
        n = query.hitNormal;
#endif
    }
    if (hit != NULL) {
        wp2 = marioGetPtr();
        *(f32*)&wp2->field_0x1F0[0x28] = ROUND_CENTI(
            angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * n.z, float_100_8041a7f8 * n.y) -
            float_180_8041a7cc);
        *(f32*)&wp2->field_0x1F0[0x2C] = float_0_8041a7a4;
        *(f32*)&wp2->field_0x1F0[0x30] = ROUND_CENTI(
            angleABf(float_0_8041a7a4, float_0_8041a7a4, float_100_8041a7f8 * n.x, float_100_8041a7f8 * n.y) -
            float_180_8041a7cc);
        *(f32*)&wp->field_0x1F0[0x24] =
            angleABf(float_0_8041a7a4, float_0_8041a7a4, sqrtf(n.x * n.x + n.z * n.z), -n.y);
        if (hitPos.y < py && __fabsf(hitPos.y - py) >= float_2_8041a7e8) {
            *outDist = py - hitPos.y;
            *outAngle = angleABf(px, pz, hitPos.x, hitPos.z);
            return NULL;
        }
        *outY = hitPos.y;
        *outDist = float_0_8041a7a4;
        *outAngle = angleABf(px, pz, hitPos.x, hitPos.z);
        return hit;
    }
    return NULL;
}




// NONMATCHING (91.98%): each sweep builds `pHigh`/`pLow` copies of `position` (this fixes the original's yTop/yBot GPR-bit handling and was worth +7%) and calls `marioHitCheckVecRound` with caller-built queries (NULL hitPos/normal, so only targetDistance rounds). Both sweeps build `dir` through `makeDirVec` (a by-value inline starting from the named rodata vec) so MWCC emits the original's built-then-copied `Vec` slots (first-sweep copy at 0x6c, second at 0x18); that closed the frame gap (now 0x260, matching). Remaining: register colouring, 3 extra saved GPRs (r16-r31 vs r19-r31, costs exactly the 4 extra insns of the i2f pool workaround). Named args: f1 (`a`) is unused, f24=f2 `b`, f25=f3 `c`, f26=f4 `d`.
s32 marioChkWallAround(Vec* position, s32 flag, f32 a, f32 b, f32 c, f32 d) {
    const char* base = (const char*)&vec3_802c5730;
    HitEntry* hit;
    HitEntry* hit2;
    HitCheckQuery query1;
    HitCheckQuery query2;
    HitCheckQuery query3;
    HitCheckQuery query4;
    Vec dir;
    Vec p;
    Vec pHigh;
    Vec pLow;
    Vec disp;
    Vec prev;
#ifdef MUST_MATCH
    Vec dtmpW;
    Vec dtmpW2;
#endif
    f32 sin;
    f32 cos;
    f32 step;
    f32 angle;
    f32 dist;
    f32 d1;
    f32 d2;
    f32 yTop;
    f32 yBot;
    f32 limit;
    f32 mag;
    s32 n;
    s32 i;
    s32 found;

    n = 8;
    found = 0;
    if (kpaGetStageViewType() == 0) {
        n = 2;
    }
    angle = b;
    step = float_360_8041a7c0 / (f32)n;
    dist = float_0p5_8041a7c4 * c;
    disp = *(const Vec*)RO(base, 0x198, vec3_802c58c8);
    yTop = position->y + float_11_8041a7ac;
    yBot = position->y + (d - float_1_8041a7bc);
    p = *position;
    pHigh = p;
    pLow = p;
    pHigh.y = yTop;
    pLow.y = yBot;
    for (i = 0; i < n; i++) {
        sincosf(&sin, &cos, angle);
#ifdef MUST_MATCH
        dtmpW2 = makeDirVec((const Vec*)RO(base, 0x1A4, vec3_802c58d4), sin, cos);
        dir = dtmpW2;
#else
        dir = *(const Vec*)RO(base, 0x1A4, vec3_802c58d4);
        dir.x = sin;
        dir.z = cos;
#endif
        query1.targetPos = pHigh;
        query1.targetDir = dir;
        query1.targetDistance = dist;
        d1 = dist;
        hit = marioHitCheckVecRound(&query1, NULL, NULL, &d1);
        query2.targetPos = pLow;
        query2.targetDir = dir;
        query2.targetDistance = dist;
        d2 = dist;
        hit2 = marioHitCheckVecRound(&query2, NULL, NULL, &d2);
        if (marioChkBero(hit)) {
            hit = NULL;
        }
        if (marioChkBero(hit2)) {
            hit2 = NULL;
        }
        if (hit != NULL && hit2 != NULL) {
            if (d1 > d2) {
                d1 = d2;
                hit = hit2;
            }
        } else if (hit2 != NULL) {
            hit = hit2;
            d1 = d2;
        }
        if (hit != NULL) {
            f32 t;

            sincosf(&sin, &cos, float_180_8041a7cc + angle);
            t = __fabsf(dist - d1);
            disp.x += sin * t;
            disp.z += cos * t;
            found = 1;
        }
        angle += step;
    }
    if (found == 0) {
        return found;
    }
    limit = float_0p125_8041a7d0 * c;
    if (flag == 0) {
        MarioWork* wp2 = marioGetPtr();

        if (wp2->currMotionId == 0xA || wp2->currMotionId == 0xB) {
            limit = float_0p25_8041a7d4 * c;
        }
    }
    if (limit < float_1_8041a7bc) {
        limit = float_1_8041a7bc;
    }
    prev = disp;
    mag = PSVECMag(&disp);
    if (mag > limit) {
        PSVECScale(&disp, &disp, limit / mag);
    }
    PSVECAdd(position, &disp, position);
    angle = b;
    disp = *(const Vec*)RO(base, 0x1B0, vec3_802c58e0);
    yTop = position->y + float_11_8041a7ac;
    yBot = position->y + (d - float_1_8041a7bc);
    p = *position;
    pHigh = p;
    pLow = p;
    pHigh.y = yTop;
    pLow.y = yBot;
    for (i = 0; i < n; i++) {
        sincosf(&sin, &cos, angle);
#ifdef MUST_MATCH
        dtmpW = makeDirVec((const Vec*)RO(base, 0x1BC, vec3_802c58ec), sin, cos);
        dir = dtmpW;
#else
        dir = *(const Vec*)RO(base, 0x1BC, vec3_802c58ec);
        dir.x = sin;
        dir.z = cos;
#endif
        query3.targetPos = pHigh;
        query3.targetDir = dir;
        query3.targetDistance = dist;
        d1 = dist;
        hit = marioHitCheckVecRound(&query3, NULL, NULL, &d1);
        query4.targetPos = pLow;
        query4.targetDir = dir;
        query4.targetDistance = dist;
        d2 = dist;
        hit2 = marioHitCheckVecRound(&query4, NULL, NULL, &d2);
        if (marioChkBero(hit)) {
            hit = NULL;
        }
        if (marioChkBero(hit2)) {
            hit2 = NULL;
        }
        if (hit != NULL && hit2 != NULL) {
            if (d1 > d2) {
                d1 = d2;
                hit = hit2;
            }
        } else if (hit2 != NULL) {
            hit = hit2;
            d1 = d2;
        }
        if (hit != NULL) {
            f32 t;

            sincosf(&sin, &cos, float_180_8041a7cc + angle);
            t = __fabsf(dist - d1);
            disp.x += sin * t;
            disp.z += cos * t;
            found = 1;
        }
        angle += step;
    }
    if (found == 0) {
        return 1;
    }
    if (PSVECDotProduct(&prev, &disp) >= float_0_8041a7a4) {
        return 1;
    }
    PSVECScale(&disp, &disp, float_0p5_8041a7c4);
    PSVECAdd(position, &disp, position);
    return 1;
}

// The original copies each dir vector as three raw words in x, z, y order, then
// overwrites x with sn and z with cs. A plain `dir = *(volatile Vec*)` emits the
// copy in x, y, z order, so the MUST_MATCH branch does the copy as three u32
// assignments in the original's order; the `volatile` source keeps the loads
// inside the loop, since the const table would otherwise be hoisted as
// loop-invariant. The MUST_MATCH `hit = hit ? NULL : hit` is the original's
// redundant "if (hit == NULL) hit = NULL" test.
s32 marioSlitChkWallAround(void) {
    const u8* base = (const u8*)&vec3_802c5730;
    MarioWork* wp = marioGetPtr();
    s32 flags[8];
    HitEntry* hit;
    Vec pos;
    f32 sn;
    f32 cs;
    f32 angle;
    s32 i;

    pos = wp->position;
    pos.y = pos.y + float_11_8041a7ac;
    angle = float_0_8041a7a4;
    {
        HitCheckQuery query;

        for (i = 0; i < 8; i++) {
            Vec dir;
            flags[i] = 0;
            sincosf(&sn, &cs, angle);
#ifdef MUST_MATCH
            *(u32*)&dir.x = *(volatile u32*)(base + 0x1C8);
            *(u32*)&dir.z = *(volatile u32*)(base + 0x1D0);
            *(u32*)&dir.y = *(volatile u32*)(base + 0x1CC);
#else
            dir = *(const Vec*)(base + 0x1C8);
#endif
            dir.x = sn;
            dir.z = cs;
            query.targetPos = pos;
            query.targetDir = dir;
            query.targetDistance = float_13_8041a7b0;
            if (marioGetPtr()->currMotionId == 0x1C) {
                hit = hitCheckVecFilter(&query, chkfilterVecVivian);
            } else if (marioGetPtr()->flags & 0x1000000) {
                hit = hitCheckVecFilter(&query, chkfilterVecRoll);
            } else {
                hit = hitCheckVecFilter(&query, chkfilterVec);
            }
#ifdef MUST_MATCH
            hit = (hit == NULL) ? NULL : hit;
#endif
            if (hit != NULL) {
                if (marioChkBero(hit) == 0) {
                    flags[i] = 1;
                }
            }
            angle = angle + float_45_8041a7b4;
        }
    }

    pos.y = wp->position.y + float_5_8041a7b8;
    angle = float_0_8041a7a4;
    {
        HitCheckQuery query;

        for (i = 0; i < 8; i++) {
            Vec dir;
            if (flags[i] != 1) {
                sincosf(&sn, &cs, angle);
#ifdef MUST_MATCH
                *(u32*)&dir.x = *(volatile u32*)(base + 0x1D4);
                *(u32*)&dir.z = *(volatile u32*)(base + 0x1DC);
                *(u32*)&dir.y = *(volatile u32*)(base + 0x1D8);
#else
                dir = *(const Vec*)(base + 0x1D4);
#endif
                dir.x = sn;
                dir.z = cs;
                query.targetPos = pos;
                query.targetDir = dir;
                query.targetDistance = float_13_8041a7b0;
                if (marioGetPtr()->currMotionId == 0x1C) {
                    hit = hitCheckVecFilter(&query, chkfilterVecVivian);
                } else if (marioGetPtr()->flags & 0x1000000) {
                    hit = hitCheckVecFilter(&query, chkfilterVecRoll);
                } else {
                    hit = hitCheckVecFilter(&query, chkfilterVec);
                }
#ifdef MUST_MATCH
                hit = (hit == NULL) ? NULL : hit;
#endif
                if (hit != NULL) {
                    if (marioChkBero(hit) == 0) {
                        flags[i] = 1;
                    }
                }
            }
            angle = angle + float_45_8041a7b4;
        }
    }

    pos.y = wp->position.y + (wp->wPlayerCollisionBox.y - float_1_8041a7bc);
    angle = float_0_8041a7a4;
    {
        HitCheckQuery query;

        for (i = 0; i < 8; i++) {
            Vec dir;
            if (flags[i] != 1) {
                sincosf(&sn, &cs, angle);
#ifdef MUST_MATCH
                *(u32*)&dir.x = *(volatile u32*)(base + 0x1E0);
                *(u32*)&dir.z = *(volatile u32*)(base + 0x1E8);
                *(u32*)&dir.y = *(volatile u32*)(base + 0x1E4);
#else
                dir = *(const Vec*)(base + 0x1E0);
#endif
                dir.x = sn;
                dir.z = cs;
                query.targetPos = pos;
                query.targetDir = dir;
                query.targetDistance = float_13_8041a7b0;
                if (marioGetPtr()->currMotionId == 0x1C) {
                    hit = hitCheckVecFilter(&query, chkfilterVecVivian);
                } else if (marioGetPtr()->flags & 0x1000000) {
                    hit = hitCheckVecFilter(&query, chkfilterVecRoll);
                } else {
                    hit = hitCheckVecFilter(&query, chkfilterVec);
                }
#ifdef MUST_MATCH
                hit = (hit == NULL) ? NULL : hit;
#endif
                if (hit != NULL) {
                    if (marioChkBero(hit) == 0) {
                        flags[i] = 1;
                    }
                }
            }
            angle = angle + float_45_8041a7b4;
        }
    }

    for (i = 0; i < 8; i++) {
        if (flags[i] == 0 && flags[(i + 1) & 7] == 0 && flags[(i + 2) & 7] == 0) {
            return 0;
        }
    }
    return 1;
}

void marioCheckWallShip(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    Vec v;
    Vec v2;
    Vec v3;
    f32 f8;
    f32 d;
    f32 mag;
    s32 r;

    *(f32*)&wp->field_0x1F0[0x20] = float_1000_8041a7a0;
    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        if (*(f32*)((u8*)wp + 0x1C8) < a) {
            *(f32*)((u8*)wp + 0x1C8) = a;
            *(f32*)((u8*)wp + 0x1CC) = b;
            *(s32*)((u8*)wp + 0x1D0) = 0xE;
        } else {
            s32 t = *(s32*)((u8*)wp + 0x1D0) - 1;

            *(s32*)((u8*)wp + 0x1D0) = t;
            if (t <= 0) {
                *(s32*)((u8*)wp + 0x1D0) = 0;
                *(f32*)((u8*)wp + 0x1C8) = a;
                *(f32*)((u8*)wp + 0x1CC) = b;
            }
        }
    } else {
#ifdef MUST_MATCH
        // volatile forces a fresh float_0 load; without it MWCC CSEs the constant into
        // f1, which also flips the float_0/float_1000 allocation at the top.
        *(f32*)&wp->field_0x1C0[0x8] = *(volatile const f32*)&float_0_8041a7a4;
#else
        *(f32*)&wp->field_0x1C0[0x8] = float_0_8041a7a4;
#endif
    }

    if (a != float_0_8041a7a4 && wp->airTimer == 0) {
        *(f32*)((u8*)wp + 0x19C) = *(f32*)((u8*)wp + 0x1CC);
    }

    if (a == float_0_8041a7a4) {
        marioSearchFrontWall_staying(b);
        return;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        MarioWork* wp2;

        *(void**)((u8*)wp + 0x1E0) = hit;
        r = hitGetAttr(hit);
        wp2 = marioGetPtr();
        if ((r & 0x80000) && !(wp2->flags & 0x1000000)) {
            r = 0;
        } else if ((r & 0x80) == 0) {
            r = (r & 0x800000) >> 23;
        } else {
            r = (wp2->currMotionId == 0x19);
        }
        if (r != 0) {
            if (a != float_0_8041a7a4) {
                wp->hitobjPush = hit;
                *(void**)((u8*)wp + 0x1F0) = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
            v = wp->position;
            return;
        } else {
            if (a == float_0_8041a7a4) {
                wp->field_0x3E[1] = 0;
            } else {
                s32 t;

                *(f32*)((u8*)wp + 0x210) = f8;
                *(void**)((u8*)wp + 0x1F0) = hit;
                t = wp->field_0x3E[1] + 1;
                wp->field_0x3E[1] = t;
                if ((s8)t >= 0x14) {
                    wp->field_0x3E[1] = 0x14;
                    if ((hitGetAttr(hit) & 0x80) == 0) {
                        r = 1;
                    } else {
                        MarioWork* wp3 = marioGetPtr();

                        r = (wp3->currMotionId == 0x19);
                    }
                    if (r != 0) {
                        wp->hitobjPush = hit;
                    }
                }
            }
            wp->position = v;
        }
    }
    v2 = v;
    wp->position = v;
    marioChkWallAround(&wp->position, 0, a, b, wp->wPlayerCollisionBox.z, float_4_8041a7a8);
    d = wp->mBaseDashSpeed;
    PSVECSubtract(&wp->position, &v2, &v3);
    mag = PSVECMag(&v3);
    if (mag > d) {
        PSVECScale(&v3, &v3, d / mag);
        PSVECAdd(&v2, &v3, &wp->position);
    }
}

#ifndef MUST_MATCH
// The match build gets these three from compound-literal templates; the natural build names them.
const Vec vec3_802c5760 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c576c = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802c5814 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802c5820 = { 0.0f, 1.0f, 0.0f };
#endif

// 4-byte zero word that ends the unit's .data: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8030E5DC_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

//.sdata2 (defined after the code so the uses stay loads of the named symbols)
const f32 float_1000_8041a7a0 = 1000.0f;
const f32 float_0_8041a7a4 = 0.0f;
const f32 float_4_8041a7a8 = 4.0f;
const f32 float_11_8041a7ac = 11.0f;
const f32 float_13_8041a7b0 = 13.0f;
const f32 float_45_8041a7b4 = 45.0f;
const f32 float_5_8041a7b8 = 5.0f;
const f32 float_1_8041a7bc = 1.0f;
const f32 float_360_8041a7c0 = 360.0f;
const f32 float_0p5_8041a7c4 = 0.5f;
const f32 float_neg0p5_8041a7c8 = -0.5f;
const f32 float_180_8041a7cc = 180.0f;
const f32 float_0p125_8041a7d0 = 0.125f;
const f32 float_0p25_8041a7d4 = 0.25f;
const f32 float_0p75_8041a7d8 = 0.75f;
const f32 float_neg1_8041a7dc = -1.0f;
const f32 float_3p1416_8041a7e0 = 3.14159274f;
const f32 float_0p1_8041a7e4 = 0.100000001f;
const f32 float_2_8041a7e8 = 2.0f;
const f32 float_18p5_8041a7ec = 18.5f;
const f32 float_3_8041a7f0 = 3.0f;
const f32 float_0p265_8041a7f4 = 0.264999986f;
const f32 float_100_8041a7f8 = 100.0f;
const f32 float_10_8041a7fc = 10.0f;
const f32 float_40_8041a800 = 40.0f;
const f32 float_neg10_8041a804 = -10.0f;
const f32 float_60_8041a808 = 60.0f;
const f32 float_0p8_8041a80c = 0.800000012f;
const f32 float_20_8041a810 = 20.0f;
const f32 float_8_8041a814 = 8.0f;
const f32 float_270_8041a818 = 270.0f;
const f32 float_90_8041a81c = 90.0f;
const f32 float_27p75_8041a820 = 27.75f;
const f32 float_12_8041a824 = 12.0f;
const f32 float_7_8041a828 = 7.0f;
const f32 float_30_8041a82c = 30.0f;
const f32 float_16p8_8041a830 = 16.7999992f;
const f32 float_neg23p7_8041a834 = -23.7000008f;
const f32 float_5p8_8041a838 = 5.80000019f;
const f32 float_9_8041a83c = 9.0f;
const f32 float_75_8041a840 = 75.0f;
const f32 float_50_8041a844 = 50.0f;
const f32 float_0p05_8041a848 = 0.0500000007f;
const f32 float_47_8041a84c = 47.0f;
const f32 float_18_8041a850 = 18.0f;
const f32 float_0p01_8041a854 = 0.00999999978f;
const f32 float_neg10000_8041a858 = -10000.0f;
const f32 float_8p64_8041a85c = 8.63999939f;
const f32 float_105_8041a860 = 105.0f;
const f32 float_0p425_8041a864 = 0.425000012f;
const f32 float_0p375_8041a868 = 0.375f;
const f32 float_37_8041a86c = 37.0f;
const f32 float_neg3000_8041a870 = -3000.0f;
const f32 float_5000_8041a874 = 5000.0f;
const f32 float_neg0p4_8041a878 = -0.400000006f;
const f32 float_neg0p06_8041a87c = -0.0599999987f;
const f32 float_neg0p04_8041a880 = -0.0399999991f;
const f32 float_neg0p02_8041a884 = -0.0199999996f;
const f32 float_1037_8041a888 = 1037.0f;
const f32 float_6_8041a88c = 6.0f;
