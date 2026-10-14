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
static const Vec vec3_802c5760;
static const Vec vec3_802c576c;
static const Vec vec3_802c5880;
static const Vec vec3_802c588c;
static const Vec vec3_802c5898;
static const Vec vec3_802c58a4;
static const Vec vec3_802c58b0;
static const Vec vec3_802c58bc;
static const Vec vec3_802c57f0;
static const Vec vec3_802c57fc;
static const Vec vec3_802c5808;
static const Vec vec3_802c5814;
static const Vec vec3_802c5820;
static const Vec vec3_802c582c;
static const Vec vec3_802c5838;
static const Vec vec3_802c5844;
static const Vec vec3_802c5850;
static const Vec vec3_802c585c;
static const Vec vec3_802c5874;
static const Vec vec3_802c5868;
static const Vec vec3_802c58c8;
static const Vec vec3_802c58d4;
static const Vec vec3_802c58e0;
static const Vec vec3_802c58ec;

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#define ROS(base, off, str) ((const char*)(base) + (off))
#else
#define RO(base, off, sym) ((const char*)&(sym))
#define ROS(base, off, str) (str)
#endif

// Match-only: a volatile read keeps a Vec template copy inside its loop, as the original does in a few places.
#ifdef MUST_MATCH
#define RELOAD_VEC(x) (*(const volatile Vec*)&(x))
#else
#define RELOAD_VEC(x) (x)
#endif

//.rodata
static const Vec vec3_802c5730 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c573c = { 0.0f, 0.0f, 0.0f };

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

// The ground probes' 0.01 rounding and normal-to-angle steps. The original reloads 100.0 in each of these (one load
// shared within the step, none hoisted out of the loop or kept across a call); a plain read of the const lets MWCC keep
// it in saved FPRs, which costs the frame. MATCH_F32 is a plain read without MUST_MATCH.
static inline f32 gndCenti(f32 v) {
    f32 k = 100.f;

    return (f32)(s32)(k * v + 0.5f) / k;
}

static inline f32 gndAngle(f32 x, f32 y) {
    f32 k = 100.f;

    return angleABf(0.0f, 0.0f, k * x, k * y);
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

// Filter+round half of the above, with the query built by the caller. marioChkFront needs
// its query.targetPos written before the dir build (the original schedules the pos->x/y/z
// loads first), which marioHitCheckVec cannot express because it copies *a itself.
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
    *distance = (f32)(s32)(1000.0f * query->targetDistance + (query->targetDistance >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
    if (c) {
        *c = query->hitPos;
        c->x = (f32)(s32)(1000.0f * c->x + (c->x >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
        c->y = (f32)(s32)(1000.0f * c->y + (c->y >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
        c->z = (f32)(s32)(1000.0f * c->z + (c->z >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
    }
    if (d) {
        *d = query->hitNormal;
    }
    return hit;
}


#define GND_PTR(wp, type, off) (*(type*)((u8*)(wp) + (off)))
#define GND_CENTI(v) ((f32)(s32)(100.f * (v) + 0.5f) / 100.f)
#define LND_CENTI(v) ((f32)(s32)(100.f * (v) + 0.5f) / 100.f)
#define ROUND_MILLI(v)                                                                        \
    ((f32)(s32)(1000.f * (v) +                                                   \
                ((v) >= 0.0f ? 0.5f : -0.5f)) /      \
     1000.f)
#define ROUND_CENTI(v)                                                                        \
    ((f32)(s32)(100.f * (v) + 0.5f) / 100.f)

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
    *distance = (f32)(s32)(1000.f * query.targetDistance +
                           (query.targetDistance >= 0.0f ? 0.5f
                                                                     : -0.5f)) /
                1000.f;
    if (c != NULL) {
        *c = query.hitPos;
        c->x = (f32)(s32)(1000.f * c->x +
                          (c->x >= 0.0f ? 0.5f : -0.5f)) /
               1000.f;
        c->y = (f32)(s32)(1000.f * c->y +
                          (c->y >= 0.0f ? 0.5f : -0.5f)) /
               1000.f;
        c->z = (f32)(s32)(1000.f * c->z +
                          (c->z >= 0.0f ? 0.5f : -0.5f)) /
               1000.f;
    }
    if (d != NULL) {
        *d = query.hitNormal;
    }
    return hit;
}





void marioHitCheckWidth(void) {
    MarioWork* wp = marioGetPtr();

    if (!(wp->flags & 0x100000)) {
        f32 target;

        if ((wp->flags & 0x1000000) == 0) {
            target = wp->wPlayerCollisionBox.x;
        } else {
            target = 18.0f;
        }
        if ((wp->wPlayerCollisionBox.z += 1.0f) > target) {
            wp->wPlayerCollisionBox.z = target;
        }
    } else {
        if (wp->wPlayerCollisionBox.z > 6.0f) {
            if ((wp->wPlayerCollisionBox.z -= 1.0f) < 6.0f) {
                wp->wPlayerCollisionBox.z = 6.0f;
            }
        }
        if (wp->wPlayerCollisionBox.z < 6.0f) {
            if ((wp->wPlayerCollisionBox.z += 1.0f) > 6.0f) {
                wp->wPlayerCollisionBox.z = 6.0f;
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

    *distance = (f32)(s32)(1000.f * query.targetDistance +
                           (query.targetDistance >= 0.0f ? 0.5f
                                                                     : -0.5f)) /
                1000.f;
    if (c) {
        *c = query.hitPos;
        c->x = (f32)(s32)(1000.f * c->x +
                          (c->x >= 0.0f ? 0.5f : -0.5f)) /
               1000.f;
        c->y = (f32)(s32)(1000.f * c->y +
                          (c->y >= 0.0f ? 0.5f : -0.5f)) /
               1000.f;
        c->z = (f32)(s32)(1000.f * c->z +
                          (c->z >= 0.0f ? 0.5f : -0.5f)) /
               1000.f;
    }
    if (d) {
        *d = query.hitNormal;
    }
    return hit;
}

HitEntry* marioHitCheck(f32* outX, f32* outY, f32* outZ, f32* distance,
                        f32* outNX, f32* outNY, f32* outNZ,
                        f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz) {
    Vec hitPos;
    Vec hitNormal;
    Vec pos;
    Vec dir;
    HitEntry* hit;
    f32 dist;

    pos = vec3_802c5730;
    dir = vec3_802c573c;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    dir.x = nx;
    dir.y = ny;
    dir.z = nz;
    dist = *distance;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
    *distance = dist;
    *outX = hitPos.x;
    *outY = hitPos.y;
    *outZ = hitPos.z;
    *outNX = hitNormal.x;
    *outNY = hitNormal.y;
    *outNZ = hitNormal.z;
    return hit;
}

#pragma dont_inline on // called out of line by marioSearchHead and marioSearchHead_jabara
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
#pragma dont_inline reset

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

// marioChkGnd's angle/centi steps: plain reads of the 100.0 scale (not MATCH_F32) let the
// inlined copies share the live scale across the gndAngle/gndCenti boundary, as the original does.
// gndAngle/gndCenti with a plain read of float_100, so MWCC can share one load across the rounding and angle steps.
static inline f32 gndAnglePlain(f32 x, f32 y) {
    f32 k = 100.f;

    return angleABf(0.0f, 0.0f, k * x, k * y);
}

static inline f32 gndCentiPlain(f32 v) {
    f32 k = 100.f;

    return (f32)(s32)(k * v + 0.5f) / k;
}

// The rolling probe's inline expansion only happens with the raised limits; at the default
// inline_max_size the helper stays a real `bl` and the whole frame shifts.
#pragma inline_max_total_size(60000) // let the rolling probe's inlined callees expand
#pragma inline_max_size(1000) // per-callee inline budget for the probe's callees
static inline void marioChkGndRollProbe(MarioWork* wp) {
    HitEntry* hit;
    Vec p;
    Vec dir;
    Vec hitPos;
    Vec normal;
    f32 dist;
    f32 grz;
    f32 grx;

    dist = 47.0f;
    GND_PTR(wp, f32, 0x164) = GND_PTR(wp, f32, 0x114) - wp->position.y;
    p = wp->position;
    p.y += 10.0f;
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
    hit = marioHitCheckVec(&p, &dir, &hitPos, &normal, &dist);
    if (hit != NULL) {
        grx = gndCentiPlain(gndAnglePlain(normal.z, normal.y) - 180.f);
        grz = gndCentiPlain(gndAnglePlain(normal.x, normal.y) - 180.f);
        angleABf(0.0f, 0.0f, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
    }
    if (hit != NULL) {
        roll_grx = grx;
        roll_grz = grz;
    }
}

#pragma inline_max_total_size reset // restore the file's default inline budget after marioChkGnd
#pragma inline_max_size reset // (1000 already keeps the rest of the unit's scores)

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
        y *= 0.5f;
    }

    pos = wp->position;
    pos.y = pos.y + y;
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
    dist = 1000.f;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
    *(f32*)wp->field_0x1C0 = (hit != NULL) ? hitPos.y : -3000.f;
    return hit;
}

s32 searchUnder2(Vec* out, f32 x, f32 y, f32 z) {
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;

    pos = (Vec){ x, y, z };
    pos.y += 37.0f;
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
    dist = 1037.0f;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
    out->x = (hit != NULL) ? hitPos.y : -1.0f;
    return (s32)hit;
}

static const Vec vec3_802c5778 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5784 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5790 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c579c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57a8 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c57b4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57c0 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57cc = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c57d8 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57e4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57f0 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57fc = { 0.0f, 1.0f, 0.0f };
static const Vec vec3_802c5808 = { 0.0f, 0.0f, 0.0f };


void marioChkGnd(void) {
    MarioWork* wp = marioGetPtr();
    MarioWork* m;
    HitEntry* hit;
    HitEntry* ride;
    f32 gx;
    f32 gy;
    f32 gz;
    f32 h;
    f32 fy;
    f32 fd;
    f32 fa;
    f32 dy;
    s32 ok;

    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        GND_PTR(wp, HitEntry*, 0x1E4) = (HitEntry*)marioSearchGround(&gy, &gx, &gz, 37.0f, 0.0f);
        GND_PTR(wp, HitEntry*, 0x1F8) = marioSearchUnder();
        return;
    }
    GND_PTR(wp, HitEntry*, 0x1E4) = NULL;
    if (wp->characterId == 2) {
        kpaClearHitobjRide();
    }
    GND_PTR(wp, HitEntry*, 0x1F8) = marioSearchUnder();
    if (wp->flags & 0x30000) {
        return;
    }
    m = marioGetPtr();
    if (m->flags & 0x100) {
        ok = 0;
    } else if (m->position.y >= 5000.f) {
        ok = 0;
    } else {
        ok = 1;
    }
    if (ok == 0) {
        return;
    }
    if (!(wp->flags & 0x200)) {
        if (wp->flags & 0x1000000) {
            hit = (HitEntry*)marioSearchGroundRoll(&gy, &gx, &gz, 11.0f, GND_PTR(wp, f32, 0x7C));
            if (hit != NULL) {
                GND_PTR(wp, HitEntry*, 0x1E4) = hit;
                GND_PTR(wp, f32, 0x1C0) = (f32)(s32)(100.f * gy + 0.5f) / 100.f;
            }
        } else {
            if (wp->characterId == 2) {
                hit = (HitEntry*)kpaSearchGround(&gy, &gx, &gz, 11.0f);
            } else {
                hit = (HitEntry*)marioSearchGround(&gy, &gx, &gz, 11.0f, GND_PTR(wp, f32, 0x7C));
            }
            if (hit != NULL) {
                GND_PTR(wp, HitEntry*, 0x1E4) = hit;
                GND_PTR(wp, f32, 0x1C0) = (f32)(s32)(100.f * gy + 0.5f) / 100.f;
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
            if (GND_PTR(wp, f32, 0x7C) >= 0.0f) {
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
                hit = marioChkFrontStep(&fy, &fd, &fa);
                dy = fy - wp->position.y;
                if (hit != NULL) {
                    if (dy <= 11.0f && dy >= 5.0f && GND_PTR(wp, f32, 0x214) <= 30.f) {
                        GND_PTR(wp, f32, 0x1C4) = fy;
                        if (wp->baseSpeed) {
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
                hit = marioChkFrontStep(&fy, &fd, &fa);
                dy = fy - wp->position.y;
                if (hit != NULL) {
                    if (dy <= 11.0f && dy >= 5.0f && GND_PTR(wp, f32, 0x214) <= 30.f) {
                        GND_PTR(wp, f32, 0x1C4) = fy;
                        if (wp->baseSpeed) {
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
                marioChkGndRollProbe(wp);
            }
        } else {
            if (!(wp->flags & 0x1000000)) {
                if (!(wp->flags & 0x20000)) {
                    if (gx < 0.0f) {
                        marioChgMot(0xA);
                        marioSetFallPara();
                    }
                    if (gx > 0.0f) {
                        marioChgMot(0xB);
                        marioSetFallPara();
                    }
                }
            } else if (!(wp->flags & 0x20000)) {
                wp->flags &= 0xFFFEFFFF;
                wp->flags |= 0x20000;
                GND_PTR(wp, f32, 0x7C) = -0.4f;
                GND_PTR(wp, f32, 0x80) = -0.06f;
                GND_PTR(wp, f32, 0x84) = -0.04f;
                GND_PTR(wp, f32, 0x88) = -0.02f;
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
        hit = (HitEntry*)marioSearchGround(&outY, &outX, &outZ, 0.1f, *(f32*)((u8*)wp + 0x7C));
    } else {
        hit = (HitEntry*)kpaSearchGround(&outY, &outX, &outZ, 0.1f);
    }

    if (hit == NULL) {
        return;
    }

    dy = wp->position.y - outY;
    if (dy < 0.0f && dy >= -10.f) {
        wp->hitobjStandOn = hit;
        wp->position.y = outY;
        wp->flags &= ~0xC0;
        wp->airTimer = 0;
        *(s16*)((u8*)wp + 0x52) = 0;

        if (wp->characterId == 2) {
            if (wp->controlStickSensitivity || kpaFireAttackCheck() == 0) {
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

// fabs is __fabsf (no frsp), the first sample offset is `oz = ox`, and the angle rounding goes through
// gndAngle/gndCenti. The declaration order below is the rasim-best one.
HitEntry* marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    Vec up;
    Vec hitPos;
    Vec normal;
    Vec p;
    HitEntry* hit;
    HitEntry* result;
    s32 i;
    f32 bestY;
    f32 f24;
    f32 f23;
    f32 rad;
    f32 ang;
    f32 px;
    f32 fb;
    f32 py;
    f32 ox;
    f32 s;
    f32 pz;
    f32 c;
    f32 lim;
    f32 angle;
    f32 oz;
    f32 dist;

    *(f32*)((u8*)wp + 0x218) = 0.0f;
    *(f32*)((u8*)wp + 0x21C) = 0.0f;
    *(f32*)((u8*)wp + 0x220) = 0.0f;
    *(f32*)((u8*)wp + 0x214) = 0.0f;
    if (wp->flags & 0x200) {
        *outY = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *outY = wp->position.y;
        return NULL;
    }
    a = __fabsf(a);
    a += 0.01f;
    fb = 5.0f + __fabsf(b);
    *outY = wp->position.y;
    *outX = -1.0f;
    if (wp->baseSpeed) {
        angle = revise360(*(f32*)((u8*)wp + 0x1A0) - *(f32*)((u8*)wp + 0x198));
    } else {
        angle = toMovedir(*(f32*)((u8*)wp + 0x1A8));
    }
    if (wp->flags & 0x100000) {
        rad = 3.1415927f * angle / 180.f;
        s = (f32)sin(rad);
        f24 = 0.425f * (*(f32*)((u8*)wp + 0x1B4) * s);
        c = (f32)cos(rad);
        f23 = 0.425f * (*(f32*)((u8*)wp + 0x1B4) * -c);
    } else {
        rad = 3.1415927f * angle / 180.f;
        s = (f32)sin(rad);
        f24 = 0.375f * (*(f32*)((u8*)wp + 0x1B4) * s);
        c = (f32)cos(rad);
        f23 = 0.375f * (*(f32*)((u8*)wp + 0x1B4) * -c);
    }
    py = wp->position.y;
    px = wp->position.x;
    pz = wp->position.z;
    result = NULL;
    bestY = -10000.f;
    up = RELOAD_VEC(vec3_802c5778);
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            ox = 0.0f;
            oz = ox;
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
        p = RELOAD_VEC(vec3_802c5784);
        p.x = px + ox;
        p.y = py + fb;
        p.z = pz + oz;
        dist = fb + a;
        hit = marioHitCheckVec(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2;
            if (wp->dispFlags & 0x1000000) {
                lim = 75.0f;
            } else if (wp->currMotionId == 0x10) {
                lim = 75.0f;
            } else {
                lim = 50.f;
            }
            wp2 = marioGetPtr();
            GND_PTR(wp2, f32, 0x218) = gndCentiPlain(gndAnglePlain(normal.z, normal.y) - 180.f);
            *(f32*)((u8*)wp2 + 0x21C) = 0.0f;
            GND_PTR(wp2, f32, 0x220) = gndCentiPlain(gndAnglePlain(normal.x, normal.y) - 180.f);
            ang = angleABf(0.0f, 0.0f, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            if (ang >= lim) {
                hit = NULL;
            }
        }
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();
            GND_PTR(wp2, f32, 0x218) = gndCentiPlain(gndAnglePlain(normal.z, normal.y) - 180.f);
            *(f32*)((u8*)wp2 + 0x21C) = 0.0f;
            GND_PTR(wp2, f32, 0x220) = gndCentiPlain(gndAnglePlain(normal.x, normal.y) - 180.f);
            *(f32*)((u8*)wp + 0x214) = angleABf(0.0f, 0.0f, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            if (hitPos.y < py && __fabsf(hitPos.y - py) >= 2.0f) {
                if (__fabsf(hitPos.y - py) > __fabsf(b)) {
                    *outX = __fabsf(py - hitPos.y);
                    *outZ = angleABf(px, pz, hitPos.x, hitPos.z);
                } else if (hitPos.y > bestY) {
                    *outY = hitPos.y;
                    bestY = hitPos.y;
                    *outX = 0.0f;
                    *outZ = angleABf(px, pz, hitPos.x, hitPos.z);
                    marioUpdateCamCollision(hitPos);
                    result = hit;
                }
            } else if (__fabsf(hitPos.y - py) <= 11.0f && hitPos.y > bestY) {
                *outY = hitPos.y;
                bestY = hitPos.y;
                *outX = 0.0f;
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
// salvaged wave 59 version; its `fabs(...) <= dist` was flipped (the asm is cror eq,gt,eq = >=) and
// is fixed here, the slope window (ang > 105 || ang < 75) re-checked.
// NONMATCHING (90.16%): only FPR/GPR colouring and scheduling differ (posY/posX/posZ land in f25-f29 where the
// original has f25-f27, and the out pointers sit one register lower); the frame size now matches and
// calls/immediates agree.
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

    *(f32*)((u8*)wp + 0x218) = 0.0f;
    *(f32*)((u8*)wp + 0x21C) = 0.0f;
    *(f32*)((u8*)wp + 0x220) = 0.0f;
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
    a += 0.01f;
    len = a;
    best = NULL;
    *outY = wp->position.y;
    *outX = -1.0f;
    bestY = -10000.f;
    posY = wp->position.y;
    posX = wp->position.x;
    posZ = wp->position.z;
    dir = *(const Vec*)(base + 0x60);
    if (__fabsf(*(f32*)((u8*)wp + 0x214)) < 0.01f) {
        dist = 2.0f;
        start = 0;
        end = 4;
    } else {
        dist = 10.f;
        start = 4;
        end = 7;
    }
    for (i = start; i < end; i++) {
        f32 dx;
        f32 dz;
        f32 rad;

        switch (i) {
        case 0:
            dx = 0.0f;
            dz = 0.0f;
            break;
        case 1:
            rad = 3.1415927f * (90.f + wp->directionView) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            dz = 8.639999f * -(f32)cos(rad);
            break;
        case 2:
            rad = 3.1415927f * (wp->directionView - 90.f) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            dz = 8.639999f * -(f32)cos(rad);
            break;
        case 3:
            dx = 0.0f;
            dz = 0.0f;
            movePos(&dx, &dz, 3.0f, wp->directionView);
            break;
        case 4:
            dx = 0.0f;
            dz = 0.0f;
            break;
        case 5:
            rad = 3.1415927f * (90.f + wp->directionView) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            dz = 8.639999f * -(f32)cos(rad);
            movePos(&dx, &dz, 3.0f, wp->directionView);
            break;
        case 6:
            rad = 3.1415927f * (wp->directionView - 90.f) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            dz = 8.639999f * -(f32)cos(rad);
            movePos(&dx, &dz, 3.0f, wp->directionView);
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
        out1.x = (f32)(s32)(1000.f * out1.x +
                            (out1.x >= 0.0f ? 0.5f : -0.5f)) /
                 1000.f;
        out1.y = (f32)(s32)(1000.f * out1.y +
                            (out1.y >= 0.0f ? 0.5f : -0.5f)) /
                 1000.f;
        out1.z = (f32)(s32)(1000.f * out1.z +
                            (out1.z >= 0.0f ? 0.5f : -0.5f)) /
                 1000.f;
        if (&out2 != NULL) {
            out2 = query.hitNormal;
        }
    check_hit2:
        if (hit != NULL) {
#else
        if (hit != NULL) {
            out1 = query.hitPos;
            out1.x = (f32)(s32)(1000.f * out1.x +
                                (out1.x >= 0.0f ? 0.5f : -0.5f)) /
                     1000.f;
            out1.y = (f32)(s32)(1000.f * out1.y +
                                (out1.y >= 0.0f ? 0.5f : -0.5f)) /
                     1000.f;
            out1.z = (f32)(s32)(1000.f * out1.z +
                                (out1.z >= 0.0f ? 0.5f : -0.5f)) /
                     1000.f;
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
            ang1 = angleABf(0.0f, 0.0f, 100.f * out2.z,
                            100.f * out2.y);
            *(f32*)((u8*)wp2 + 0x218) = (f32)(s32)(100.f * (ang1 - 180.f) + 0.5f) /
                                        100.f;
            *(f32*)((u8*)wp2 + 0x21C) = 0.0f;
            ang2 = angleABf(0.0f, 0.0f, 100.f * out2.x,
                            100.f * out2.y);
            len2 = sqrtf(out2.x * out2.x + out2.z * out2.z);
            *(f32*)((u8*)wp2 + 0x220) = (f32)(s32)(100.f * (ang2 - 180.f) + 0.5f) /
                                        100.f;
            ang3 = angleABf(0.0f, 0.0f, len2, -out2.y);
            if (ang3 <= 105.0f && ang3 >= 75.0f) {
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
                    *outX = 0.0f;
                    *outZ = angleABf(posX, posZ, out1.x, out1.z);
                    best = hit;
                }
                continue;
            }
            dy = __fabsf(out1.y - posY);
            if (dy <= 11.0f && out1.y > bestY) {
                *outY = out1.y;
                bestY = out1.y;
                *outX = 0.0f;
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
            *(f32*)&wp->field_0x1F0[0x20] < 0.5f * wp->wPlayerCollisionBox.x) {
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


// Older landing check (the Koopa-aware ground probe): same five-sample ring as marioChkLandon2 but the ray points
// down from 0.75 * height and the accepted hit y is returned rounded to 0.01. The hit's y and normal are read into
// locals straight after the probe; the explicit ny100 local keeps 100 * ny in a saved FPR across both angleABf calls.
// The original shares one 100.0 load between the first gndCenti's rounding and the second angleABf's 100 * nx
// argument; MATCH_F32's volatile read cannot share it, so ours emits one extra lfs, and the rest is FPR colouring in
// the sample ring. Compares: fabs(...) < 0.1 and the rounding tests (cror eq,gt -> >=) checked against the asm.
// NONMATCHING (98.27%): same 15 calls and the same 0x200 frame; one extra lfs plus FPR colouring.
void* marioLandonChk_old(f32* pos, f32 spd) {
    MarioWork* wp = marioGetPtr();
    Vec dir;
    Vec p;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    f32 h;
    f32 d0;
    f32 pz;
    f32 yy;
    f32 fa;
    f32 px;
    f32 rad;
    f32 ang;
    f32 angle;
    f32 f20;
    f32 f19;
    f32 py;
    f32 ny100;
    f32 ny;
    f32 nz;
    f32 ox;
    f32 oz;
    f32 hy;
    f32 dist;
    f32 nx;
    f32 gy;
    f32 gx;
    f32 gz;
    s32 i;
    void* r;

    *(f32*)((u8*)wp + 0x218) = 0.0f;
    *(f32*)((u8*)wp + 0x21C) = 0.0f;
    *(f32*)((u8*)wp + 0x220) = 0.0f;
    *(f32*)((u8*)wp + 0x214) = 0.0f;
    if (wp->flags & 0x200) {
        *pos = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *pos = wp->position.y;
        return NULL;
    }
    fa = 0.01f + __fabsf(spd);
    h = 0.75f * wp->wPlayerCollisionBox.y;
    if (wp->baseSpeed) {
        angle = revise360(*(f32*)((u8*)wp + 0x1A0) - *(f32*)((u8*)wp + 0x198));
    } else {
        angle = toMovedir(*(f32*)((u8*)wp + 0x1A8));
    }
    rad = 3.1415927f * angle / 180.f;
    f20 = 0.25f * (*(f32*)((u8*)wp + 0x1B4) * (f32)sin(rad));
    f19 = 0.25f * (*(f32*)((u8*)wp + 0x1B4) * -(f32)cos(rad));
    px = wp->position.x;
    py = wp->position.y;
    pz = wp->position.z;
    if (wp->characterId == 2) {
        r = kpaSearchGround(&gy, &gx, &gz, 11.0f);
        *pos = gy;
        return r;
    }
    d0 = h + fa;
    yy = py + h;
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            ox = 0.0f;
            oz = ox;
        } else if (i == 1) {
            ox = f20;
            oz = f19;
        } else if (i == 2) {
            ox = -f19;
            oz = f19;
        } else if (i == 3) {
            ox = f19;
            oz = -f20;
        } else if (i == 4) {
            ox = -f20;
            oz = -f19;
        }
        p = RELOAD_VEC(vec3_802c5730);
        dir = RELOAD_VEC(vec3_802c573c);
        p.x = px + ox;
        p.y = yy;
        p.z = pz + oz;
        dir.x = 0.0f;
        dir.y = -1.0f;
        dir.z = 0.0f;
        dist = d0;
        hit = marioHitCheckVec(&p, &dir, &hitPos, &normal, &dist);
        hy = hitPos.y;
        nx = normal.x;
        ny = normal.y;
        nz = normal.z;
        if (hit != NULL) {
            MarioWork* wp2;

            wp2 = marioGetPtr();
            ny100 = 100.f * ny;
            *(f32*)&wp2->field_0x1F0[0x28] = gndCenti(angleABf(0.0f, 0.0f, 100.f * nz, ny100) - 180.f);
            *(f32*)((u8*)wp2 + 0x21C) = 0.0f;
            *(f32*)&wp2->field_0x1F0[0x30] = gndCenti(angleABf(0.0f, 0.0f, 100.f * nx, ny100) - 180.f);
            ang = angleABf(0.0f, 0.0f, (f32)sqrtf(nx * nx + nz * nz), -ny);
            *(f32*)((u8*)wp + 0x214) = ang;
            if (__fabsf(LND_CENTI((yy - hy) - dist)) < 0.1f) {
                *pos = LND_CENTI(hy);
                return hit;
            }
        }
    }
    return NULL;
}

// The hit sample ring of marioChkLandon, with the ground y returned through *out.
// NONMATCHING (94.88%): same 13 calls and the same 0x1a0 frame (angle rounding through gndAngle/gndCenti); remaining:
// stack slots and FPR colouring. fabs() is __fabsf (f32 add, no frsp) as in the original; the compare `fabs(...) < 0.1` is plain.
void* marioChkLandon2(f32* out, f32 a, f32 b) {
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

    *(f32*)((u8*)wp + 0x218) = 0.0f;
    *(f32*)((u8*)wp + 0x21C) = 0.0f;
    *(f32*)((u8*)wp + 0x220) = 0.0f;
    *(f32*)((u8*)wp + 0x214) = 0.0f;
    if (wp->flags & 0x200) {
        *out = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *out = wp->position.y;
        return NULL;
    }
    fa = 0.1f + __fabsf(a);
    h = 0.5f * wp->wPlayerCollisionBox.y;
    if (wp->baseSpeed) {
        angle = *(f32*)((u8*)wp + 0x1A0);
    } else {
        angle = toMovedir(*(f32*)((u8*)wp + 0x1A8));
    }
    rad = 3.1415927f * angle / 180.f;
    f27 = 0.25f * (*(f32*)((u8*)wp + 0x1B4) * (f32)sin(rad));
    up = RELOAD_VEC(vec3_802c57a8);
    d0 = h + fa;
    yy = wp->position.y + b + h;
    f26 = 0.25f * (*(f32*)((u8*)wp + 0x1B4) * -(f32)cos(rad));
    px = wp->position.x;
    pz = wp->position.z;
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            ox = 0.0f;
            oz = ox;
        } else if (i == 1) {
            ox = f27;
            oz = f26;
        } else if (i == 2) {
            ox = -f26;
            oz = f26;
        } else if (i == 3) {
            ox = f26;
            oz = -f27;
        } else if (i == 4) {
            ox = -f27;
            oz = -f26;
        }
        p = RELOAD_VEC(vec3_802c57b4);
        p.x = px + ox;
        p.y = yy;
        p.z = pz + oz;
        dist = d0;
        hit = marioHitCheckVec(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();

            *(f32*)((u8*)wp2 + 0x218) = gndCenti(gndAngle(normal.z, normal.y) - 180.f);
            *(f32*)((u8*)wp2 + 0x21C) = 0.0f;
            *(f32*)((u8*)wp2 + 0x220) = gndCenti(gndAngle(normal.x, normal.y) - 180.f);
            ang = angleABf(0.0f, 0.0f, (f32)sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            *(f32*)((u8*)wp + 0x214) = ang;
            if (__fabsf(LND_CENTI((yy - hitPos.y) - dist)) < 0.1f) {
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
            f8 += 5.0f;
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead_jabara(&v, &f8, &out, 0.0f);
            if (*(void**)((u8*)wp + 0x1F4) == NULL) {
                *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, (wp->flags & 0x10000) ? *(f32*)((u8*)wp + 0x7C) : 10.f);
            }
        }
    } else if (wp->flags & 0x1000000) {
        v = wp->position;
        *(void**)((u8*)wp + 0x1F4) = marioSearchHead_roll(&v, &f8, 40.f);
    } else {
        v = wp->position;
        f8 = wp->wPlayerCollisionBox.y;
        f31 = (wp->flags & 0x10000) ? *(f32*)((u8*)wp + 0x7C) : 10.f;
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
            wp->baseSpeed = 0.0f;
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
        v.y = (f32)(s32)(10.f * (v.y - f31) + 0.5f) / 10.f - 1.0f;
        if (!(wp->flags & 0x20000) && v.y < *(f32*)((u8*)wp + 0x120)) {
            v.y = *(f32*)((u8*)wp + 0x120);
            if (!(wp->flags & 0x1000000) && wp->currMotionId != 0x19 &&
                wp->currMotionId != 0x18 && wp->currMotionId != 0x1C &&
                wp->characterId != 2 && marioChkSlitEnd() == 0) {
                if (wp->baseSpeed == 0.0f) {
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
            if (f8 >= 0.0f && f8 <= 5.0f) {
                wp->flags |= 0x08000000;
            } else {
                wp->flags &= ~0x08000000;
            }
        } else {
            if (f8 >= 0.0f && f8 + wp->wPlayerCollisionBox.y <= 47.0f) {
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
    f32 ofs = 5.0f;

    *out = 0;
    *outF = -1.0f;
    v = wp->position;
    f8 = wp->wPlayerCollisionBox.y;
    if (wp->currMotionId == 8) {
        return v.y;
    }
    if (jabaraNoHitChk() == 0) {
        f8 += ofs;
        *(void**)((u8*)wp + 0x1F4) = marioSearchHead_jabara(&v, &f8, out, 0.0f);
        if (*(void**)((u8*)wp + 0x1F4) == NULL) {
            f8 = wp->wPlayerCollisionBox.y + ofs;
            *(void**)((u8*)wp + 0x1F4) = marioSearchHead(&v, &f8, 0.0f);
        }
    }
    wp->position.x = v.x;
    if (*(void**)((u8*)wp + 0x1F4) != NULL) {
        wp->flags |= 0x08000000;
        v.y = v.y - wp->wPlayerCollisionBox.y - 0.1f;
        if (wp->currMotionId == 3) {
            marioChgMot(0);
        }
    } else {
        if (f8 > 0.0f) {
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
// The slope angles round through gndCenti.
// NONMATCHING (96.28%): only FPR colouring remains (the loop-body ox/oz take f31/f30 and rotate
// the whole long-lived set). Reordering declarations and extending `a`'s live range were tried.
void* marioSearchHead(Vec* pos, f32* outY, f32 a) {
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

    if (wp->baseSpeed) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->unk1A8);
    }
    rad = 3.1415927f * angle / 180.f;
    s = (f32)sin(rad);
    f25 = 0.5f * ((wp->wPlayerCollisionBox.x - 0.5f) * s);
    c = (f32)cos(rad);
    f24 = 0.5f * ((wp->wPlayerCollisionBox.x - 0.5f) * -c);
    f27 = -f25;
    f28 = -f24;
    y0 = *outY;
    f26 = 0.5f * y0;
    f22 = y0 + a;
    *outY = -1.0f;
    for (i = 0; i < 5; i++) {
        f32 ox;
        f32 oz;

        if (i == 0) {
            ox = 0.0f;
            oz = 0.0f;
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
        t = RELOAD_VEC(vec3_802c57f0);
        t.x = pos->x + ox;
        t.y = pos->y + f26;
        t.z = pos->z + oz;
        p = t;
        up = RELOAD_VEC(vec3_802c57fc);
        hit = marioHitCheck2(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2;
            f32 ny100;
            f32 ang;

            nz = normal.z;
            ny = normal.y;
            nx = normal.x;
            wp2 = marioGetPtr();
            ny100 = 100.f * ny;

            *(f32*)&wp2->field_0x1F0[0x28] =
                gndCenti(angleABf(0.0f, 0.0f, 100.f * nz, ny100) - 180.f);
            *(f32*)&wp2->field_0x1F0[0x2C] = 0.0f;
            *(f32*)&wp2->field_0x1F0[0x30] =
                gndCenti(angleABf(0.0f, 0.0f, 100.f * nx, ny100) - 180.f);
            ang = angleABf(0.0f, 0.0f, (f32)sqrtf(nx * nx + nz * nz), -ny);
            if (!(wp->flags & 0x1000000)) {
                f32 lim = (wp->dispFlags & 0x1000000) ? 75.0f : 50.f;

                if (ang <= 180.f - lim && ang >= lim) {
                    hit = NULL;
                }
            }
        }
        if (wp->flags & 0x1000000) {
            if (hit != NULL) {
                f32 y = (f32)(s32)(100.f * hitPos.y + f26) / 100.f;

                y = y - 0.05f * y0;
                hitPos.y = y;
                if (pos->y + y0 >= y) {
                    pos->y = y;
                    *outY = 0.0f;
                    return hit;
                } else {
                    *outY = (y - pos->y) - y0;
                    return NULL;
                }
            }
        } else {
            if (hit != NULL) {
                f32 y = (f32)(s32)(100.f * hitPos.y + f26) / 100.f;

                hitPos.y = y;
                if (pos->y + y0 >= y) {
                    pos->y = y;
                    *outY = 0.0f;
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

    *outY = -1.0f;
    y = wp->wPlayerCollisionBox.y;
    dist = y + a;
    for (i = 0; i < 5; i++) {
        f32 rad;

        switch (i) {
        case 0:
            ox = 0.0f;
            oz = 0.0f;
            break;
        case 1:
            rad = 3.1415927f * (90.f + wp->directionView) / 180.f;
            ss = (f32)sin(rad);
            ox = 9.0f * ss;
            cc = (f32)cos(rad);
            oz = 9.0f * -cc;
            movePos(&ox, &oz, 3.0f, wp->directionView);
            break;
        case 2:
            rad = 3.1415927f * (wp->directionView - 90.f) / 180.f;
            ss = (f32)sin(rad);
            ox = 9.0f * ss;
            cc = (f32)cos(rad);
            oz = 9.0f * -cc;
            movePos(&ox, &oz, 3.0f, wp->directionView);
            break;
        case 3:
            rad = 3.1415927f * (90.f + wp->directionView) / 180.f;
            ss = (f32)sin(rad);
            ox = 9.0f * ss;
            cc = (f32)cos(rad);
            oz = 9.0f * -cc;
            movePos(&ox, &oz, 3.0f, 180.f + wp->directionView);
            break;
        case 4:
            rad = 3.1415927f * (wp->directionView - 90.f) / 180.f;
            ss = (f32)sin(rad);
            ox = 9.0f * ss;
            cc = (f32)cos(rad);
            oz = 9.0f * -cc;
            movePos(&ox, &oz, 3.0f, 180.f + wp->directionView);
            break;
        }
#ifdef MUST_MATCH
        p = RELOAD_VEC(vec3_802c5808);
        dir = (Vec){ 0.0f, 1.0f, 0.0f };
#else
        p = vec3_802c5808;
        dir = vec3_802c5814;
#endif
        p.x = pos->x + ox;
        p.y = 1.0f + pos->y;
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
        out1.x = (f32)(s32)(1000.f * out1.x +
                            (out1.x >= 0.0f ? 0.5f : -0.5f)) /
                  1000.f;
        out1.y = (f32)(s32)(1000.f * out1.y +
                            (out1.y >= 0.0f ? 0.5f : -0.5f)) /
                  1000.f;
        out1.z = (f32)(s32)(1000.f * out1.z +
                            (out1.z >= 0.0f ? 0.5f : -0.5f)) /
                  1000.f;
        if (&out2 != NULL) {
            out2 = query.hitNormal;
        }
    check_hit2:
        if (hit != NULL) {
#else
        if (hit != NULL) {
            out1 = query.hitPos;
            out1.x = (f32)(s32)(1000.f * out1.x +
                                (out1.x >= 0.0f ? 0.5f : -0.5f)) /
                      1000.f;
            out1.y = (f32)(s32)(1000.f * out1.y +
                                (out1.y >= 0.0f ? 0.5f : -0.5f)) /
                      1000.f;
            out1.z = (f32)(s32)(1000.f * out1.z +
                                (out1.z >= 0.0f ? 0.5f : -0.5f)) /
                      1000.f;
            out2 = query.hitNormal;
        }
        if (hit != NULL) {
#endif
            if (pos->y + y >= out1.y) {
                pos->y = out1.y - y - 1.0f;
                *outY = 0.0f;
                return hit;
            } else {
                f32 d = (out1.y - pos->y) - y;

                if (*outY < 0.0f) {
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
    radius = -23.7f;
    *outY = -1.0f;
    y = a2 + a;
#ifdef MUST_MATCH
    up = (Vec){ 0.0f, 1.0f, 0.0f };
#else
    up = vec3_802c5820;
#endif

    for (i = 0; i < 8; i++) {
        outF = y;
        p.x = pos->x + radius * (f32)sin(3.1415927f * wp->targetYaw / 180.f);
        p.z = pos->z - radius * (f32)cos(3.1415927f * wp->targetYaw / 180.f);
        p.y = 0.5f + pos->y;
        hit = marioHitCheck2(&p, &up, &out1, &out2, &outF);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x40000) {
                if (pos->y + a2 >= out1.y) {
                    pos->y = out1.y;
                    *outY = 0.0f;
                    *out = (p.z - pos->z < 0.0f) ? -1 : 1;
                    return hit;
                } else {
                    *outY = (out1.y - pos->y) - a2;
                }
            } else if (i == 3) {
                if (pos->y + a2 >= out1.y) {
                    pos->y = out1.y;
                    *outY = 0.0f;
                    *out = 0;
                    return hit;
                } else {
                    *outY = (out1.y - pos->y) - a2;
                }
            }
        }
        radius += 5.8f;
    }
    return NULL;
}

static const Vec vec3_802c582c = { 0.0f, 1.0f, 0.0f };
static const Vec vec3_802c5838 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5844 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5850 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c585c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5868 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5874 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5880 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c588c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5898 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58a4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58b0 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58bc = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c58c8 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58d4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58e0 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58ec = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58f8 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5904 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5910 = { 0.0f, 0.0f, 0.0f };

// prototypes for in-file functions

void marioCheckWall(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    Vec v;
    f32 f8;
    s32 r;

    *(f32*)&wp->field_0x1F0[0x20] = 1000.f;
    if (a && wp->airTimer == 0) {
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
        *(f32*)&wp->field_0x1C0[0x8] = 0.0f;
#else
        *(f32*)((u8*)wp + 0x1C8) = 0.0f;
#endif
    }

    if (!a) {
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
            if (a) {
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
    halfx = 0.5f * wp->wPlayerCollisionBox.x;
    if (wp->flags & 0x100000) {
        halfx = 0.0f;
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
                movePos(&v.x, &v.z, 0.5f * wp->wPlayerCollisionBox.x, angle);
                angle += 180.f;
                hit = searchFrontWall(&v, &f8b, wp->wPlayerCollisionBox.x, angle, wp->wPlayerCollisionBox.z);
                if (hit != NULL && wp->wPlayerCollisionBox.x >= f8b) {
                    *(void**)((u8*)wp + 0x1E0) = hit;
                    f8 = 0.5f * f8b;
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
    marioChkWallAround(&v, 0, 0.0f, a, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    wp->position.x = v.x;
    wp->position.z = v.z;
}

// The hit-test block builds the query, dispatches the filter and rounds the result in one
// place, so the original inlines marioHitCheckVecRound there.
void marioCheckWall_roll(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;
    f32 f31;
    HitEntry* hit;
    HitCheckQuery query;
    s32 r;

    *(f32*)&wp->field_0x1F0[0x20] = 1000.f;
    if (a && wp->airTimer == 0) {
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
        *(f32*)&wp->field_0x1C0[0x8] = 0.0f;
    }

    if (a && wp->airTimer == 0) {
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
        f31 = 30.f;
        query.targetPos = v2;
        query.targetDir = dir;
        query.targetDistance = f31;
        headHit = marioHitCheckVecRound(&query, &out1, &out2, &f31);
        if (headHit != NULL && f31 > 16.8f) {
            headHit = NULL;
        }
        *(void**)((u8*)wp + 0x1F4) = headHit;
        if (headHit != NULL) {
            wp->position.y = out1.y - wp->wPlayerCollisionBox.y;
        }
    }

    if (!a) {
        marioSearchFrontWall_staying(b);
        return;
    }

    if (*(void**)((u8*)wp + 0x1F4) != NULL) {
        wp->baseSpeed = 0.0f;
        a = 0.0f;
        *(f32*)&wp->field_0x7C[0] = 0.0f;
        *(f32*)&wp->field_0x7C[4] = 0.0f;
        *(f32*)&wp->field_0x7C[8] = 0.0f;
        *(f32*)&wp->field_0x7C[0xC] = 0.0f;
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
            if (a) {
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

// The angle fields are rounded to 0.01 with an unconditional +0.5 (different from
// ROUND_MILLI's 1000 and sign-dependent bias).



// The caller builds `query` (via marioHitCheckVecRound) so query.targetPos = *pos is written first.
// The remaining diffs are the schedule of that query/dir build (the original issues the vec3.y and
// pos.z loads earlier, which also changes the registers those webs get) and one hitPos rounding
// temporary (the y component) that takes f5 where the original has f4.
// NONMATCHING (98.57%): same instruction count, calls and immediates; scheduling and allocation only.
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
    *out = -1.0f;
    return NULL;
}

// NONMATCHING (91.67%): the three passes build a `HitCheckQuery` local and call `marioHitCheckVecRound` (the caller-built shape `marioChkFront` uses), which reproduces the 3 query slots and the distance/hitPos/normal rounding; calls and control flow match. Both directions are built through `makeDirVec` (a by-value inline that starts from the named rodata vec) so MWCC materialises the original's built-then-copied `Vec` slots, and the pass-1 best-hit update is two branches (the original duplicates the copy for `best == NULL` and for a closer hit). The frame matches; what remains is stack slot placement (declaration order does not move it) and register colouring.
HitEntry* searchFrontWall(Vec* pos, f32* out, f32 a, f32 b, f32 c) {
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
        if (__fabsf(*(f32*)&wp->field_0x1F0[0x28]) >= 45.0f ||
            __fabsf(*(f32*)&wp->field_0x1F0[0x30]) >= 45.0f) {
            pos->x += a * sin;
            pos->z += a * cos;
            return NULL;
        }
    }
    count = 2;
    if (wp->characterId == 2) {
        if (*(u32*)wp->field_0x14 & 1) {
            count = 3;
            half[0] = 0.75f * wp->wPlayerCollisionBox.y;
            half[1] = 0.5f * wp->wPlayerCollisionBox.y;
            half[2] = 11.0f;
        } else {
            half[0] = 0.75f * wp->wPlayerCollisionBox.y;
            half[1] = 11.0f;
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
        half[0] = 0.75f * marioHeight;
        half[1] = 11.0f;
    } else if ((wp->flags & 0x1000000) == 0) {
        half[0] = 27.75f;
        half[1] = 11.0f;
        if (wp->airTimer != 0) {
            half[2] = 0.1f;
            count = 3;
        }
    } else {
        half[0] = 12.0f;
        half[1] = 11.0f;
        if (wp->airTimer != 0) {
            half[2] = 7.0f;
            count = 3;
        }
    }
    dist = 0.5f * c;
    p = *pos;
#ifdef MUST_MATCH
    dtmp = makeDirVec(&vec3_802c5844, sin, cos);
    dir = dtmp;
#else
    dir = vec3_802c5844;
    dir.x = sin;
    dir.z = cos;
#endif
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
            if (best != NULL) {
                if (curDist < bestDist) {
                    bestDist = curDist;
                    best = hit;
                    bestNorm = norm;
                }
            } else {
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

        deltaX = 0.5f * (dx - dot * nx);
        deltaZ = 0.5f * (dz - dot * nz);
    }
    sincosf(&sin2, &cos2, 45.0f + b);
#ifdef MUST_MATCH
    dtmp2 = makeDirVec(&vec3_802c5850, sin2, cos2);
    dir2 = dtmp2;
#else
    dir2 = vec3_802c5850;
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
        sincosf(&sin2, &cos2, b - 45.0f);
        dir3 = vec3_802c585c;
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
            *out = 0.0f;
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
    dist = (wp->flags & 0x100000) ? 2.0f : 8.0f;
    tmp = pos;
    tmp.y += 60.f;
    dir = vec3_802c5868;
    dist100 = 100.f;
    hit = marioHitCheckVec(&tmp, &dir, &hitPos, &hitNormal, &dist100);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, dist, 270.f);
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, dist, 90.f);
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }
out:
    return hit;
}

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
    tmp.y += 60.f;
    dir = vec3_802c5874;
    dist = 100.f;
    hit = marioHitCheckVec(&tmp, &dir, &hitPos, &hitNormal, &dist);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, 8.0f, 270.f);
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, 8.0f, 90.f);
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        *out = 0.8f;
    }
    return hit;
}

// Matches. The original builds targetPos by first copying a named zero vector
// (vec3_802c5880 / vec3_802c5898) and then overwriting the components, and adds the
// float_5 clearance after the copy; both shapes are needed for the exact stack layout.
// 0x1E0 is unnamed MarioWork padding.
HitEntry* N_dou10_yoko_yari3(void* unused) {
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
    movePos(&vx, &vz, 10.f, wp->directionView);
    v = *(const Vec*)(RO(base, 0x150, vec3_802c5880));
    v.x = vx;
    v.y = vy;
    v.z = vz;
    query.targetPos = v;
    query.targetPos.y += 5.0f;
    sincosf(&sn, &cs, 0.0f);
    dir = *(const Vec*)(RO(base, 0x15C, vec3_802c588c));
    dir.x = sn;
    dir.z = cs;
    query.targetDir = dir;
    query.targetDistance = 40.f;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        *(HitEntry**)((u8*)wp + 0x1E0) = hit;
        return hit;
    }
    vx = wp->position.x;
    vy = wp->position.y;
    vz = wp->position.z;
    movePos(&vx, &vz, -10.f, wp->directionView);
    v2 = *(const Vec*)(RO(base, 0x168, vec3_802c5898));
    v2.x = vx;
    v2.y = vy;
    v2.z = vz;
    query.targetPos = v2;
    query.targetPos.y += 5.0f;
    sincosf(&sn, &cs, 0.0f);
    dir2 = *(const Vec*)(RO(base, 0x174, vec3_802c58a4));
    dir2.x = sn;
    dir2.z = cs;
    query.targetDir = dir2;
    query.targetDistance = 40.f;
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
// schedule) brought it to 86.93%. A searched FPR declaration order (`py, px, off, s, rad, pz, h, angle, c`) reached 87.14%
// (float_100 is a plain read here; the volatile reloads are in marioSearchHead).
// Still differs: the callee-saved GPR set (ours keeps hit in r30 and reuses r30 for
// the base, so the hit test is `cmplwi r30` instead of the original's `mr r29,r3`); and the
// ROUND_CENTI float_100 loads are hoisted into f31/f30 (callee-saved) instead of the
// original's volatile f7/f5 reloads, which costs `h` its f31.
// NONMATCHING (96.17%): the GPR/FPR colouring above, and the frame is 0x100 vs 0x110.
HitEntry* marioChkFrontStep(f32* outY, f32* outDist, f32* outAngle) {
#ifdef MUST_MATCH
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

    *(f32*)&wp->field_0x1F0[0x28] = 0.0f;
    *(f32*)&wp->field_0x1F0[0x2C] = 0.0f;
    *(f32*)&wp->field_0x1F0[0x30] = 0.0f;
    *(f32*)&wp->field_0x1F0[0x24] = 0.0f;
    if (wp->flags & 0x1000000) {
        h = 18.5f;
    } else {
        h = 0.5f * wp->wPlayerCollisionBox.y;
    }
    py = wp->position.y;
    *outY = wp->position.y;
    *outDist = -1.0f;
    if (wp->baseSpeed) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->unk1A8);
    }
    angle = revise360((f32)(s32)(0.5f + angle));
    if (wp->flags & 0x1000000) {
        off = 3.0f;
    } else {
        off = 0.265f * wp->wPlayerCollisionBox.x;
    }
    rad = (3.1415927f * angle) / 180.f;
    s = (f32)sin((f64)rad);
    c = (f32)cos((f64)rad);
    px = wp->position.x;
    py = wp->position.y;
#ifdef MUST_MATCH
    v = vec3_802c58b0;
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
    query.targetDir = vec3_802c58bc;
#else
    query.targetDir = vec3_802c58bc;
#endif
    query.targetDistance = 0.1f + (11.0f + h);
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
            gndAngle(n.z, n.y) -
            180.f);
        *(f32*)&wp2->field_0x1F0[0x2C] = 0.0f;
        *(f32*)&wp2->field_0x1F0[0x30] = ROUND_CENTI(
            gndAngle(n.x, n.y) -
            180.f);
        *(f32*)&wp->field_0x1F0[0x24] =
            angleABf(0.0f, 0.0f, sqrtf(n.x * n.x + n.z * n.z), -n.y);
        if (hitPos.y < py && __fabsf(hitPos.y - py) >= 2.0f) {
            *outDist = py - hitPos.y;
            *outAngle = angleABf(px, pz, hitPos.x, hitPos.z);
            return NULL;
        }
        *outY = hitPos.y;
        *outDist = 0.0f;
        *outAngle = angleABf(px, pz, hitPos.x, hitPos.z);
        return hit;
    }
    return NULL;
}

// NONMATCHING (92.23%): each sweep builds `pHigh`/`pLow` copies of `position` (this fixes the original's yTop/yBot GPR-bit handling and was worth +7%) and calls `marioHitCheckVecRound` with caller-built queries (NULL hitPos/normal, so only targetDistance rounds). Both sweeps build `dir` through `makeDirVec` (a by-value inline starting from the named rodata vec) so MWCC emits the original's built-then-copied `Vec` slots (first-sweep copy at 0x6c, second at 0x18); that closed the frame gap (now 0x260, matching). Remaining: register colouring, 3 extra saved GPRs (r16-r31 vs r19-r31, costs exactly the 4 extra insns of the i2f pool workaround). Named args: f1 (`a`) is unused, f24=f2 `b`, f25=f3 `c`, f26=f4 `d`.
s32 marioChkWallAround(Vec* position, s32 flag, f32 a, f32 b, f32 c, f32 d) {
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
    step = 360.f / (f32)n;
    dist = 0.5f * c;
    disp = vec3_802c58c8;
    yTop = position->y + 11.0f;
    yBot = position->y + (d - 1.0f);
    p = *position;
    pHigh = p;
    pLow = p;
    pHigh.y = yTop;
    pLow.y = yBot;
    for (i = 0; i < n; i++) {
        sincosf(&sin, &cos, angle);
#ifdef MUST_MATCH
        dtmpW2 = makeDirVec(&vec3_802c58d4, sin, cos);
        dir = dtmpW2;
#else
        dir = vec3_802c58d4;
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

            sincosf(&sin, &cos, 180.f + angle);
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
    limit = 0.125f * c;
    if (flag == 0) {
        MarioWork* wp2 = marioGetPtr();

        if (wp2->currMotionId == 0xA || wp2->currMotionId == 0xB) {
            limit = 0.25f * c;
        }
    }
    if (limit < 1.0f) {
        limit = 1.0f;
    }
    prev = disp;
    mag = PSVECMag(&disp);
    if (mag > limit) {
        PSVECScale(&disp, &disp, limit / mag);
    }
    PSVECAdd(position, &disp, position);
    angle = b;
    disp = vec3_802c58e0;
    yTop = position->y + 11.0f;
    yBot = position->y + (d - 1.0f);
    p = *position;
    pHigh = p;
    pLow = p;
    pHigh.y = yTop;
    pLow.y = yBot;
    for (i = 0; i < n; i++) {
        sincosf(&sin, &cos, angle);
#ifdef MUST_MATCH
        dtmpW = makeDirVec(&vec3_802c58ec, sin, cos);
        dir = dtmpW;
#else
        dir = vec3_802c58ec;
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

            sincosf(&sin, &cos, 180.f + angle);
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
    if (PSVECDotProduct(&prev, &disp) >= 0.0f) {
        return 1;
    }
    PSVECScale(&disp, &disp, 0.5f);
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
    f32 ofs = 5.0f;
    s32 i;

    pos = wp->position;
    pos.y += 11.0f;
    angle = 0.0f;
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
            query.targetDistance = 13.0f;
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
            angle += 45.0f;
        }
    }

    pos.y = wp->position.y + ofs;
    angle = 0.0f;
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
                query.targetDistance = 13.0f;
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
            angle += 45.0f;
        }
    }

    pos.y = wp->position.y + (wp->wPlayerCollisionBox.y - 1.0f);
    angle = 0.0f;
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
                query.targetDistance = 13.0f;
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
            angle += 45.0f;
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

    *(f32*)&wp->field_0x1F0[0x20] = 1000.f;
    if (a && wp->airTimer == 0) {
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
        *(f32*)&wp->field_0x1C0[0x8] = 0.0f;
#else
        *(f32*)&wp->field_0x1C0[0x8] = 0.0f;
#endif
    }

    if (a && wp->airTimer == 0) {
        *(f32*)((u8*)wp + 0x19C) = *(f32*)((u8*)wp + 0x1CC);
    }

    if (!a) {
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
            if (a) {
                wp->hitobjPush = hit;
                *(void**)((u8*)wp + 0x1F0) = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
            v = wp->position;
            return;
        } else {
            if (!a) {
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
    marioChkWallAround(&wp->position, 0, a, b, wp->wPlayerCollisionBox.z, 4.0f);
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
static const Vec vec3_802c5760 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c576c = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5814 = { 0.0f, 1.0f, 0.0f };
static const Vec vec3_802c5820 = { 0.0f, 1.0f, 0.0f };
#endif

// 4-byte zero word that ends the unit's .data: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8030E5DC_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

//.sdata2 (defined after the code so the uses stay loads of the named symbols)
