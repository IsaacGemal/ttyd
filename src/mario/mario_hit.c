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

// Vectors the original copies from memory where MWCC would fold or hoist a const one: non-const under MUST_MATCH
// (the section keeps them in .rodata). The probe loops then reload their templates in the loop body (marioChkWallAround
// after each sincosf instead of from three saved GPRs; the landing, ground and head probes per sample), and
// marioSearchGroundRoll / marioSlitChkWallAround read theirs through MWCC's own .rodata base.
#ifdef MUST_MATCH
#define MEM_VEC static __declspec(section ".rodata") Vec
#else
#define MEM_VEC static const Vec
#endif

// vectors defined further down
static const Vec vec3_802c5760;
static const Vec vec3_802c576c;
static const Vec vec3_802c5880;
static const Vec vec3_802c588c;
static const Vec vec3_802c5898;
static const Vec vec3_802c58a4;
static const Vec vec3_802c58b0;
MEM_VEC vec3_802c57f0;
MEM_VEC vec3_802c57fc;
MEM_VEC vec3_802c5808;
static const Vec vec3_802c5814;
static const Vec vec3_802c5820;
static const Vec vec3_802c5874;
static const Vec vec3_802c5868;


//.rodata
MEM_VEC vec3_802c5730 = { 0.0f, 0.0f, 0.0f };
MEM_VEC vec3_802c573c = { 0.0f, 0.0f, 0.0f };

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

// Inline sqrtf from the SDK's math.h, redeclared here because including math.h clashes with
// math_ext.h's out-of-line sqrt. Same expansion the original used (frsqrte + NaN handling).
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

// The probes round hit distances and positions to 0.001 (sign-dependent bias) and slope angles and heights to 0.01.
#define ROUND_CENTI(v) ((f32)(s32)(100.f * (v) + 0.5f) / 100.f)
#define ROUND_MILLI(v) ((f32)(s32)(1000.f * (v) + ((v) >= 0.0f ? 0.5f : -0.5f)) / 1000.f)

#ifdef MUST_MATCH
// Match-only helper (no name evidence; the natural build writes it out): marioHitCheckVec filtered by
// chkFilterAttr2 with user0/user1 set, inlined into both attr branches of marioHitCheck2 as the original has it.
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
    *distance = ROUND_MILLI(query.targetDistance);
    if (c != NULL) {
        *c = query.hitPos;
        c->x = ROUND_MILLI(c->x);
        c->y = ROUND_MILLI(c->y);
        c->z = ROUND_MILLI(c->z);
    }
    if (d != NULL) {
        *d = query.hitNormal;
    }
    return hit;
}
#endif

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
    MapObjectEntry* obj;

    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) == 0 && (attr & 0x4) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_HiddenBadgeBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_Hidden10CountBlock") == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterVecRoll(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    MapObjectEntry* obj;

    if ((attr & 0x80) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) == 0 && (attr & 0x80004) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_HiddenBadgeBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_Hidden10CountBlock") == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterVecVivian(HitCheckQuery* query, HitEntry* entry) {
    s32 attr = entry->attributes;
    MapObjectEntry* obj;

    if ((attr & 0x80804) != 0 && (attr & 0x800000) == 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_HiddenBadgeBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_Hidden10CountBlock") == 0) {
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

    *distance = ROUND_MILLI(query.targetDistance);
    if (c) {
        *c = query.hitPos;
        c->x = ROUND_MILLI(c->x);
        c->y = ROUND_MILLI(c->y);
        c->z = ROUND_MILLI(c->z);
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
HitEntry* marioHitCheck2(Vec* a, Vec* b, Vec* c, Vec* d, f32* e) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    f32 dist;
#ifndef MUST_MATCH
    BOOL filtered;
#endif

    dist = *e;
#ifdef MUST_MATCH
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
#else
    if (wp->currMotionId != MARIO_MOTION_JABARA) {
        filtered = (wp->flags & 0x20000) == 0;
    } else {
        filtered = marioGetJabaraState() == 1;
    }
    if (filtered) {
        HitCheckQuery query;

        query.targetPos = *a;
        query.targetDir = *b;
        query.targetDistance = dist;
        query.user0 = 4;
        query.user1 = 0x800000;
        hit = hitCheckVecFilter(&query, chkFilterAttr2);
        if (hit != NULL) {
            dist = ROUND_MILLI(query.targetDistance);
            if (c != NULL) {
                *c = query.hitPos;
                c->x = ROUND_MILLI(c->x);
                c->y = ROUND_MILLI(c->y);
                c->z = ROUND_MILLI(c->z);
            }
            if (d != NULL) {
                *d = query.hitNormal;
            }
        }
    } else {
        hit = marioHitCheckVec(a, b, c, d, &dist);
    }
#endif
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

#ifdef MUST_MATCH
// Match-only helper (no name evidence; the natural build writes it out in marioChkGnd). Its locals become
// compiler temporaries when inlined. The rolling probe's inline expansion only happens with the raised limits; at
// the default inline_max_size the helper stays a real `bl` and the whole frame shifts.
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
    wp->field_0x164 = wp->prevPosition.y - wp->position.y;
    p = wp->position;
    p.y += 10.0f;
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
    hit = marioHitCheckVec(&p, &dir, &hitPos, &normal, &dist);
    if (hit != NULL) {
        grx = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.z, 100.f * normal.y) - 180.f);
        grz = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.x, 100.f * normal.y) - 180.f);
        angleABf(0.0f, 0.0f, sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
    }
    if (hit != NULL) {
        roll_grx = grx;
        roll_grz = grz;
    }
}

#pragma inline_max_size reset // (1000 already keeps the rest of the unit's scores)
#endif

HitEntry* marioSearchUnder(void) {
    MarioWork* wp = marioGetPtr();
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    HitEntry* hit;
    f32 dist;
    f32 y;

    y = wp->wPlayerCollisionBox.y;
    if (wp->flags & 0x1000000) {
        y *= 0.5f;
    }

    pos = wp->position;
    pos.y = pos.y + y;
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
    dist = 1000.f;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &dist);
    wp->floorY = (hit != NULL) ? hitPos.y : -3000.f;
    return hit;
}

HitEntry* searchUnder2(Vec* out, f32 x, f32 y, f32 z) {
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
    return hit;
}

MEM_VEC vec3_802c5778 = { 0.0f, -1.0f, 0.0f };
MEM_VEC vec3_802c5784 = { 0.0f, 0.0f, 0.0f };
MEM_VEC vec3_802c5790 = { 0.0f, -1.0f, 0.0f };
MEM_VEC vec3_802c579c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57a8 = { 0.0f, -1.0f, 0.0f };
MEM_VEC vec3_802c57b4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57c0 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57cc = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c57d8 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c57e4 = { 0.0f, 0.0f, 0.0f };
MEM_VEC vec3_802c57f0 = { 0.0f, 0.0f, 0.0f };
MEM_VEC vec3_802c57fc = { 0.0f, 1.0f, 0.0f };
MEM_VEC vec3_802c5808 = { 0.0f, 0.0f, 0.0f };


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
        wp->hitobjStandOn = marioSearchGround(&gy, &gx, &gz, 37.0f, 0.0f);
        wp->hitobjUnder = marioSearchUnder();
        return;
    }
    wp->hitobjStandOn = NULL;
    if (wp->characterId == 2) {
        kpaClearHitobjRide();
    }
    wp->hitobjUnder = marioSearchUnder();
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
            hit = marioSearchGroundRoll(&gy, &gx, &gz, 11.0f, wp->jumpVelY);
            if (hit != NULL) {
                wp->hitobjStandOn = hit;
                wp->floorY = ROUND_CENTI(gy);
            }
        } else {
            if (wp->characterId == 2) {
                hit = kpaSearchGround(&gy, &gx, &gz, 11.0f);
            } else {
                hit = marioSearchGround(&gy, &gx, &gz, 11.0f, wp->jumpVelY);
            }
            if (hit != NULL) {
                wp->hitobjStandOn = hit;
                wp->floorY = ROUND_CENTI(gy);
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
            if (wp->hitobjStandOn == NULL) {
                marioShipChgFall();
                return;
            }
            wp->flags &= 0xFFFDFFFF;
        }
    } else {
        if (wp->currMotionId != 0x1F && wp->hitobjStandOn != NULL && (hitGetAttr(wp->hitobjStandOn) & 0x200)) {
            if (wp->jumpVelY >= 0.0f) {
                marioSetFallPara();
            }
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x1F);
            return;
        }
        if (wp->hitobjStandOn != NULL && (hitGetAttr(wp->hitobjStandOn) & 0x800)) {
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
                    if (dy <= 11.0f && dy >= 5.0f && wp->slopeAngle <= 30.f) {
                        wp->field_0x1C4 = fy;
                        if (wp->baseSpeed) {
                            if (!(wp->dispFlags & 0x1000000)) {
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
                    if (dy <= 11.0f && dy >= 5.0f && wp->slopeAngle <= 30.f) {
                        wp->field_0x1C4 = fy;
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
            ride = kpaChkHitobjRide();
        } else {
            ride = wp->hitobjStandOn;
        }
        if (ride != NULL) {
            if (wp->flags & 0x1000000) {
#ifdef MUST_MATCH
                marioChkGndRollProbe(wp);
#else
                HitEntry* probe;
                Vec p;
                Vec dir;
                Vec hitPos;
                Vec normal;
                f32 dist;

                dist = 47.0f;
                wp->field_0x164 = wp->prevPosition.y - wp->position.y;
                p = wp->position;
                p.y += 10.0f;
                dir = (Vec){ 0.0f, -1.0f, 0.0f };
                probe = marioHitCheckVec(&p, &dir, &hitPos, &normal, &dist);
                if (probe != NULL) {
                    roll_grx = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.z, 100.f * normal.y) - 180.f);
                    roll_grz = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.x, 100.f * normal.y) - 180.f);
                    angleABf(0.0f, 0.0f, sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
                }
#endif
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
                wp->jumpVelY = -0.4f;
                wp->jumpAccelY = -0.06f;
                wp->jumpJerkY = -0.04f;
                wp->jumpJerkStepY = -0.02f;
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
        hit = marioSearchGround(&outY, &outX, &outZ, 0.1f, wp->jumpVelY);
    } else {
        hit = kpaSearchGround(&outY, &outX, &outZ, 0.1f);
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
        wp->airTimerPeak = 0;

        if (wp->characterId == 2) {
            if (wp->controlStickSensitivity || kpaFireAttackCheck() == 0) {
                if (wp->wStickDir1 * wp->wStickDir1 +
                        wp->wStickDir2 * wp->wStickDir2 >
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

// fabs is __fabsf (no frsp) and the first sample offset is `oz = ox`. The declaration order below is the rasim-best
// one.
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

    wp->slopeTiltX = 0.0f;
    wp->slopeTiltY = 0.0f;
    wp->slopeTiltZ = 0.0f;
    wp->slopeAngle = 0.0f;
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
        angle = revise360(wp->directionView - wp->targetYaw);
    } else {
        angle = toMovedir(wp->dispDir);
    }
    if (wp->flags & 0x100000) {
        rad = 3.1415927f * angle / 180.f;
        s = (f32)sin(rad);
        f24 = 0.425f * (wp->wPlayerCollisionBox.x * s);
        c = (f32)cos(rad);
        f23 = 0.425f * (wp->wPlayerCollisionBox.x * -c);
    } else {
        rad = 3.1415927f * angle / 180.f;
        s = (f32)sin(rad);
        f24 = 0.375f * (wp->wPlayerCollisionBox.x * s);
        c = (f32)cos(rad);
        f23 = 0.375f * (wp->wPlayerCollisionBox.x * -c);
    }
    py = wp->position.y;
    px = wp->position.x;
    pz = wp->position.z;
    result = NULL;
    bestY = -10000.f;
    up = vec3_802c5778;
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
        p = vec3_802c5784;
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
            wp2->slopeTiltX = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.z, 100.f * normal.y) - 180.f);
            wp2->slopeTiltY = 0.0f;
            wp2->slopeTiltZ = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.x, 100.f * normal.y) - 180.f);
            ang = angleABf(0.0f, 0.0f, sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            if (ang >= lim) {
                hit = NULL;
            }
        }
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();
            wp2->slopeTiltX = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.z, 100.f * normal.y) - 180.f);
            wp2->slopeTiltY = 0.0f;
            wp2->slopeTiltZ = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.x, 100.f * normal.y) - 180.f);
            wp->slopeAngle = angleABf(0.0f, 0.0f, sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
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

// The roll variant of marioSearchGround: the sample ring depends on |slopeAngle| (flat: 0..3, steep: 4..6) and the
// accepted slope window is ang > 105 or ang < 75.
// salvaged wave 59 version; its `fabs(...) <= dist` was flipped (the asm is cror eq,gt,eq = >=) and
// is fixed here, the slope window (ang > 105 || ang < 75) re-checked.
// The probe direction and start point are read from their named vectors (non-const under MUST_MATCH, MEM_VEC), so
// MWCC addresses them through its own .rodata base as the original does; the loop index takes the start value
// directly; the probe is the file's marioHitCheckVec (inlined); the slopeTiltZ store comes before the sqrt for the
// third angle. Float declaration order from a rasim search. The rounded cosine sits in its own local before it is
// negated (written inline, the constant and the negated cosine swap f0/f1-f2).
HitEntry* marioSearchGroundRoll(f32* outY, f32* outX, f32* outZ, f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    MarioWork* wp2;
    HitEntry* hit;
    HitEntry* best;
    Vec dir;
    Vec out1;
    Vec out2;
    Vec p;
    f32 dist;
    f32 bestY;
    f32 posX;
    f32 boxY;
    f32 posY;
    f32 posZ;
    f32 len;
    f32 py;
    f32 tdist;
    s32 i;
    s32 end;

    wp->slopeTiltX = 0.0f;
    wp->slopeTiltY = 0.0f;
    wp->slopeTiltZ = 0.0f;
    if (wp->flags & 0x200) {
        *outY = wp->position.y;
        return NULL;
    }
    if (wp->flags & 0x100) {
        *outY = wp->position.y;
        return NULL;
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
    dir = vec3_802c5790;
    if (__fabsf(wp->slopeAngle) < 0.01f) {
        dist = 2.0f;
        i = 0;
        end = 4;
    } else {
        dist = 10.f;
        i = 4;
        end = 7;
    }
    for (; i < end; i++) {
        f32 dx;
        f32 dz;
        f32 rad;
        f32 c;

        switch (i) {
        case 0:
            dx = 0.0f;
            dz = 0.0f;
            break;
        case 1:
            rad = 3.1415927f * (90.f + wp->directionView) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            c = (f32)cos(rad);
            dz = 8.639999f * -c;
            break;
        case 2:
            rad = 3.1415927f * (wp->directionView - 90.f) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            c = (f32)cos(rad);
            dz = 8.639999f * -c;
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
            c = (f32)cos(rad);
            dz = 8.639999f * -c;
            movePos(&dx, &dz, 3.0f, wp->directionView);
            break;
        case 6:
            rad = 3.1415927f * (wp->directionView - 90.f) / 180.f;
            dx = 8.639999f * (f32)sin(rad);
            c = (f32)cos(rad);
            dz = 8.639999f * -c;
            movePos(&dx, &dz, 3.0f, wp->directionView);
            break;
        }
        py = posY + boxY;
        tdist = boxY + len;
        p = vec3_802c579c;
        p.x = posX + dx;
        p.y = py;
        p.z = posZ + dz;
        hit = marioHitCheckVec(&p, &dir, &out1, &out2, &tdist);
        if (hit != NULL) {
            f32 ang1;
            f32 ang2;
            f32 ang3;
            f32 dy;

            wp2 = marioGetPtr();
            ang1 = angleABf(0.0f, 0.0f, 100.f * out2.z,
                            100.f * out2.y);
            wp2->slopeTiltX = ROUND_CENTI(ang1 - 180.f);
            wp2->slopeTiltY = 0.0f;
            ang2 = angleABf(0.0f, 0.0f, 100.f * out2.x,
                            100.f * out2.y);
            wp2->slopeTiltZ = ROUND_CENTI(ang2 - 180.f);
            ang3 = angleABf(0.0f, 0.0f, sqrtf(out2.x * out2.x + out2.z * out2.z), -out2.y);
            if (ang3 <= 105.0f && ang3 >= 75.0f) {
                continue;
            }
            wp->slopeAngle = ang3;
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
        return best;
    }
    return NULL;
}

void marioChkToge(void) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    s32 attr;

    if (vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) {
        return;
    }
    hit = wp->hitobjFront;
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0 &&
            wp->frontWallDist < 0.5f * wp->wPlayerCollisionBox.x) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }

    hit = wp->hitobjWall;
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }

    hit = wp->hitobjHead;
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (wp->currMotionId != 0x20 && (attr & 0x800) != 0) {
            set_damage_root_ypos(wp->position.y);
            marioChgMot(0x20);
        }
    }
}

HitEntry* marioChkLandon(f32* pos, f32 spd) {
    MarioWork* wp = marioGetPtr();
    f32 outY;
    f32 outX;

    if (wp->characterId != 2) {
        if ((wp->flags & 0x1000000) == 0) {
            return marioSearchGround(pos, &outY, &outX, spd, spd);
        } else {
            return marioSearchGroundRoll(pos, &outY, &outX, spd, spd);
        }
    } else {
        return marioLandonChk_old(pos, spd);
    }
}


// Older landing check (the Koopa-aware ground probe): same five-sample ring as marioChkLandon2 but the ray points
// down from 0.75 * height and the accepted hit y is returned rounded to 0.01. Each sample probes through the file's
// marioHitCheck (inlined, as in the original: it reads the hit's y and normal back into locals, and its 0.0/-1.0
// direction arguments are hoisted). The explicit ny100 local keeps 100 * ny in a saved FPR across both angleABf
// calls. The original shares one 100.0 load between the first ROUND_CENTI and the second angleABf's 100 * nx
// argument. d0/yy are assigned in the loop body after the offset chain (MWCC hoists them after the -f19/-f20
// negations, as the original's preheader has them); float declaration order from a rasim search.
// Compares: fabs(...) < 0.1 and the rounding tests (cror eq,gt -> >=) checked against the asm.
HitEntry* marioLandonChk_old(f32* pos, f32 spd) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    f32 nz;
    f32 ny100;
    f32 py;
    f32 h;
    f32 px;
    f32 f20;
    f32 angle;
    f32 f19;
    f32 pz;
    f32 fa;
    f32 hy;
    f32 hx;
    f32 hz;
    f32 nx;
    f32 ny;
    f32 ox;
    f32 ang;
    f32 rad;
    f32 sn;
    f32 c;
    f32 oz;
    f32 dist;
    f32 d0;
    f32 yy;
    f32 gy;
    f32 gx;
    f32 gz;
    s32 i;
    void* r;

    wp->slopeTiltX = 0.0f;
    wp->slopeTiltY = 0.0f;
    wp->slopeTiltZ = 0.0f;
    wp->slopeAngle = 0.0f;
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
        angle = revise360(wp->directionView - wp->targetYaw);
    } else {
        angle = toMovedir(wp->dispDir);
    }
    rad = 3.1415927f * angle / 180.f;
    sn = (f32)sin(rad);
    f20 = 0.25f * (wp->wPlayerCollisionBox.x * sn);
    c = (f32)cos(rad);
    f19 = 0.25f * (wp->wPlayerCollisionBox.x * -c);
    px = wp->position.x;
    py = wp->position.y;
    pz = wp->position.z;
    if (wp->characterId == 2) {
        r = kpaSearchGround(&gy, &gx, &gz, 11.0f);
        *pos = gy;
        return r;
    }
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
        d0 = h + fa;
        yy = py + h;
        dist = d0;
        hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz, px + ox, yy, pz + oz, 0.0f, -1.0f, 0.0f);
        if (hit != NULL) {
            MarioWork* wp2;

            wp2 = marioGetPtr();
            ny100 = 100.f * ny;
            wp2->slopeTiltX = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * nz, ny100) - 180.f);
            wp2->slopeTiltY = 0.0f;
            wp2->slopeTiltZ = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * nx, ny100) - 180.f);
            ang = angleABf(0.0f, 0.0f, sqrtf(nx * nx + nz * nz), -ny);
            wp->slopeAngle = ang;
            if (__fabsf(ROUND_CENTI((yy - hy) - dist)) < 0.1f) {
                *pos = ROUND_CENTI(hy);
                return hit;
            }
        }
    }
    return NULL;
}

// The hit sample ring of marioChkLandon, with the ground y returned through *out.
// The cos offset is computed right after the sin one (before the up copy), and d0 and yy are assigned in the loop
// body (MWCC hoists them after the offset negations, d0 first, so h dies into yy's register as in the original); Vec
// and float declaration orders from the stack slots and a rasim search. fabs() is __fabsf (f32 add, no frsp) as in
// the original; the compare `fabs(...) < 0.1` is plain.
HitEntry* marioChkLandon2(f32* out, f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    Vec up;
    Vec hitPos;
    Vec normal;
    Vec p;
    HitEntry* hit;
    f32 d0;
    f32 yy;
    f32 py;
    f32 rad;
    f32 f27;
    f32 h;
    f32 sn;
    f32 fa;
    f32 ang;
    f32 f26;
    f32 px;
    f32 pz;
    f32 dist;
    f32 ox;
    f32 oz;
    f32 angle;
    f32 c;
    s32 i;

    wp->slopeTiltX = 0.0f;
    wp->slopeTiltY = 0.0f;
    wp->slopeTiltZ = 0.0f;
    wp->slopeAngle = 0.0f;
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
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->dispDir);
    }
    rad = 3.1415927f * angle / 180.f;
    sn = (f32)sin(rad);
    f27 = 0.25f * (wp->wPlayerCollisionBox.x * sn);
    c = (f32)cos(rad);
    f26 = 0.25f * (wp->wPlayerCollisionBox.x * -c);
    up = vec3_802c57a8;
    py = wp->position.y + b;
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
        d0 = h + fa;
        yy = py + h;
        p = vec3_802c57b4;
        p.x = px + ox;
        p.y = yy;
        p.z = pz + oz;
        dist = d0;
        hit = marioHitCheckVec(&p, &up, &hitPos, &normal, &dist);
        if (hit != NULL) {
            MarioWork* wp2 = marioGetPtr();

            wp2->slopeTiltX = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.z, 100.f * normal.y) - 180.f);
            wp2->slopeTiltY = 0.0f;
            wp2->slopeTiltZ = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * normal.x, 100.f * normal.y) - 180.f);
            ang = angleABf(0.0f, 0.0f, sqrtf(normal.x * normal.x + normal.z * normal.z), -normal.y);
            wp->slopeAngle = ang;
            if (__fabsf(ROUND_CENTI((yy - hitPos.y) - dist)) < 0.1f) {
                *out = hitPos.y;
                return hit;
            }
        }
    }
    return NULL;
}

#ifdef MUST_MATCH
// The rolling form of the landing probe (a fixed 18.0 ray start, as marioHitCheckWidth's rolling width). Uncalled in
// retail; generated between marioChkOverhead and marioChkLandon2, so 18.0 pools at that point.
HitEntry* marioChkLandonRoll(f32* out, f32 spd) {
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec dir;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    f32 dist;

    p.x = wp->position.x;
    p.y = wp->position.y + 18.0f;
    p.z = wp->position.z;
    dir.x = 0.0f;
    dir.y = -1.0f;
    dir.z = 0.0f;
    dist = 18.0f + (0.1f + __fabsf(spd));
    hit = marioHitCheckVec(&p, &dir, &hitPos, &normal, &dist);
    if (hit != NULL) {
        *out = hitPos.y;
    } else {
        *out = wp->position.y;
    }
    return hit;
}
#endif

f32 marioChkOverhead(void) {
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
            wp->hitobjHead = marioSearchHead_jabara(&v, &f8, &out, 0.0f);
            if (wp->hitobjHead == NULL) {
                wp->hitobjHead = marioSearchHead(&v, &f8, (wp->flags & 0x10000) ? wp->jumpVelY : 10.f);
            }
        }
    } else if (wp->flags & 0x1000000) {
        v = wp->position;
        wp->hitobjHead = marioSearchHead_roll(&v, &f8, 40.f);
    } else {
        v = wp->position;
        f8 = wp->wPlayerCollisionBox.y;
        f31 = (wp->flags & 0x10000) ? wp->jumpVelY : 10.f;
        if (wp->characterId == 2) {
            wp->hitobjHead = kpaSearchHead(&v, &f8, f31);
        } else {
            wp->hitobjHead = marioSearchHead(&v, &f8, f31);
        }
    }
    wp->position.x = v.x;
    hit = wp->hitobjHead;
    if (hit != NULL) {
        MarioWork* wp2;
        s32 ok;

        if (hit == NULL) {
            ok = 0;
        } else if (!(hitGetAttr(hit) & 0x80000000)) {
            ok = 0;
        } else {
            MapObjectEntry* obj = mobjHitObjPtrToPtr(hit);

            if (strcmp(obj->animName, "MOBJ_PowerUpBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_SaveBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_RecoveryBlock") == 0 ||
                strcmp(obj->animName, "MOBJ_CoinTakenRecoveryBlock") == 0) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
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
        if (!(wp->flags & 0x20000) && v.y < wp->field_0x11C.y) {
            v.y = wp->field_0x11C.y;
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
        wp->hitobjHead = marioSearchHead_jabara(&v, &f8, out, 0.0f);
        if (wp->hitobjHead == NULL) {
            f8 = wp->wPlayerCollisionBox.y + ofs;
            wp->hitobjHead = marioSearchHead(&v, &f8, 0.0f);
        }
    }
    wp->position.x = v.x;
    if (wp->hitobjHead != NULL) {
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

// The p/up named-vector copies are inside the loop like marioSearchHead_roll (the `t` temp is
// what makes MWCC emit the original's built-then-copied p at 0x10->0x40); f25 is built from
// sin before cos is called.
// The slope angles round through ROUND_CENTI.
// Float declaration order from a rasim search; the rounded hit y is written back into hitPos.y and compared from
// there (the original keeps the store in both branches).
HitEntry* marioSearchHead(Vec* pos, f32* outY, f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec up;
    Vec hitPos;
    Vec normal;
    HitEntry* hit;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 f28;
    f32 s;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 y0;
    f32 rad;
    f32 f22;
    f32 c;
    f32 angle;
    f32 dist;
    s32 i;

    if (wp->baseSpeed) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->dispDir);
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
#ifdef MUST_MATCH
        {
            Vec t;

            t = vec3_802c57f0;
            t.x = pos->x + ox;
            t.y = pos->y + f26;
            t.z = pos->z + oz;
            p = t;
        }
#else
        p = vec3_802c57f0;
        p.x = pos->x + ox;
        p.y = pos->y + f26;
        p.z = pos->z + oz;
#endif
        up = vec3_802c57fc;
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

            wp2->slopeTiltX =
                ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * nz, ny100) - 180.f);
            wp2->slopeTiltY = 0.0f;
            wp2->slopeTiltZ =
                ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * nx, ny100) - 180.f);
            ang = angleABf(0.0f, 0.0f, sqrtf(nx * nx + nz * nz), -ny);
            if (!(wp->flags & 0x1000000)) {
                f32 lim = (wp->dispFlags & 0x1000000) ? 75.0f : 50.f;

                if (ang <= 180.f - lim && ang >= lim) {
                    hit = NULL;
                }
            }
        }
        if (wp->flags & 0x1000000) {
            if (hit != NULL) {
                hitPos.y = (f32)(s32)(100.f * hitPos.y + f26) / 100.f;
                hitPos.y -= 0.05f * y0;
                if (pos->y + y0 >= hitPos.y) {
                    pos->y = hitPos.y;
                    *outY = 0.0f;
                    return hit;
                } else {
                    *outY = (hitPos.y - pos->y) - y0;
                    return NULL;
                }
            }
        } else {
            if (hit != NULL) {
                hitPos.y = (f32)(s32)(100.f * hitPos.y + f26) / 100.f;
                if (pos->y + y0 >= hitPos.y) {
                    pos->y = hitPos.y;
                    *outY = 0.0f;
                    return hit;
                } else {
                    *outY = (hitPos.y - pos->y) - y0;
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

// The probe is the file's marioHitCheckVec (inlined; its distance result is unused). `dir` is built as a
// compound literal so its first word is not scheduled one slot early (the named-vector read was).
HitEntry* marioSearchHead_roll(Vec* pos, f32* outY, f32 a) {
    MarioWork* wp = marioGetPtr();
    f32 ox;
    f32 oz;
    f32 ss;
    f32 cc;
    Vec out1;
    Vec out2;
    Vec p;
    Vec dir;
    HitEntry* hit;
    f32 y;
    f32 dist;
    f32 hitDist;
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
        p = vec3_802c5808;
        dir = (Vec){ 0.0f, 1.0f, 0.0f };
        p.x = pos->x + ox;
        p.y = 1.0f + pos->y;
        p.z = pos->z + oz;
        hitDist = dist;
        hit = marioHitCheckVec(&p, &dir, &out1, &out2, &hitDist);
        if (hit != NULL) {
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
// costs the prologue interleave (96.55%). The literal is the spelling that matches.
HitEntry* marioSearchHead_jabara(Vec* pos, f32* outY, s32* out, f32 a) {
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
    up = (Vec){ 0.0f, 1.0f, 0.0f };

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

void marioCheckWall(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    HitEntry* hit;
    Vec v;
    f32 f8;
    s32 r;

    wp->frontWallDist = 1000.f;
    if (a && wp->airTimer == 0) {
        if (wp->wallDist < a) {
            wp->wallDist = a;
            wp->wallYaw = b;
            wp->wallHoldTimer = 0xE;
        } else {
            if (--wp->wallHoldTimer <= 0) {
                wp->wallHoldTimer = 0;
                wp->wallDist = a;
                wp->wallYaw = b;
            }
        }
        wp->moveDir = wp->wallYaw;
    } else {
        wp->wallDist = 0.0f;
    }

    if (!a) {
        marioSearchFrontWall_staying(b);
        dou10_yoko_yari(&wp->frontWallDist);
        return;
    }

    v = wp->position;
    wp->hitobjBalloon = 0;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        if (wp->hitobjBalloon != NULL) {
            wp->hitobjFront = wp->hitobjBalloon;
        } else {
            wp->hitobjFront = hit;
        }
        if (marioChkBero(hit)) {
            if (a) {
                wp->hitobjPush = hit;
                wp->hitobjWall = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
        } else {
            s32 t;

            wp->frontWallDist = f8;
            wp->hitobjWall = hit;
            t = wp->field_0x3F + 1;
            wp->field_0x3F = t;
            if ((s8)t >= 0x14) {
                wp->field_0x3F = 0x14;
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
    dou10_yoko_yari(&wp->frontWallDist);
}

void marioSearchFrontWall_staying(f32 a) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;
    f32 f8b;
    f32 halfx;
    f32 angle;
    HitEntry* hit;

    wp->field_0x3F = 0;
    halfx = 0.5f * wp->wPlayerCollisionBox.x;
    if (wp->flags & 0x100000) {
        halfx = 0.0f;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, halfx, wp->directionView - wp->targetYaw, wp->wPlayerCollisionBox.z);
    if (wp->currMotionId != 0x13) {
        wp->hitobjFront = hit;
    } else {
        wp->hitobjFront = NULL;
    }

    if (hit == NULL) {
        angle = toMovedir(wp->dispDir);
        v = wp->position;
        hit = searchFrontWall(&v, &f8, halfx, angle, wp->wPlayerCollisionBox.z);
        if (wp->currMotionId != 0x13) {
            wp->hitobjFront = hit;
        } else {
            wp->hitobjFront = NULL;
        }
    } else {
        if (strcmp(gp->currentMapName, "dou_10") == 0) {
            if (vivianGetStatus() != 2 && vivianGetStatus() != 1 && vivianGetStatus() != 3) {
                angle = toMovedir(wp->dispDir);
                v = wp->position;
                movePos(&v.x, &v.z, 0.5f * wp->wPlayerCollisionBox.x, angle);
                angle += 180.f;
                hit = searchFrontWall(&v, &f8b, wp->wPlayerCollisionBox.x, angle, wp->wPlayerCollisionBox.z);
                if (hit != NULL && wp->wPlayerCollisionBox.x >= f8b) {
                    wp->hitobjFront = hit;
                    f8 = 0.5f * f8b;
                }
            }
        }
    }

    if (wp->hitobjFront != NULL) {
        wp->frontWallDist = f8;
    }

    if (wp->buttonNew & 0x100) {
        wp->actionHitObj = NULL;
        v = wp->position;
        hit = searchFrontWall(&v, &f8, halfx, wp->directionView, wp->wPlayerCollisionBox.z);
        if (hit != NULL) {
            wp->actionHitObj = wp->hitobjFront;
        }
    }

    v = wp->position;
    marioChkWallAround(&v, 0, 0.0f, a, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
    wp->position.x = v.x;
    wp->position.z = v.z;
}

void marioCheckWall_roll(f32 a, f32 b) {
    MarioWork* wp = marioGetPtr();
    Vec v;
    f32 f8;
    f32 f31;
    HitEntry* hit;
    s32 r;

    wp->frontWallDist = 1000.f;
    if (a && wp->airTimer == 0) {
        if (wp->wallDist < a) {
            wp->wallDist = a;
            wp->wallYaw = b;
            wp->wallHoldTimer = 0xE;
        } else {
            if (--wp->wallHoldTimer <= 0) {
                wp->wallHoldTimer = 0;
                wp->wallDist = a;
                wp->wallYaw = b;
            }
        }
    } else {
        wp->wallDist = 0.0f;
    }

    if (a && wp->airTimer == 0) {
        wp->moveDir = wp->wallYaw;
    }

    if (wp->flags & 0x10000) {
        Vec v2;
        Vec dir;
        Vec out1;
        Vec out2;
        HitEntry* headHit;

        v2 = wp->position;
        dir = (Vec){ 0.0f, 1.0f, 0.0f };
        f31 = 30.f;
        headHit = marioHitCheckVec(&v2, &dir, &out1, &out2, &f31);
        if (headHit != NULL && f31 > 16.8f) {
            headHit = NULL;
        }
        wp->hitobjHead = headHit;
        if (headHit != NULL) {
            wp->position.y = out1.y - wp->wPlayerCollisionBox.y;
        }
    }

    if (!a) {
        marioSearchFrontWall_staying(b);
        return;
    }

    if (wp->hitobjHead != NULL) {
        wp->baseSpeed = 0.0f;
        a = 0.0f;
        wp->jumpVelY = 0.0f;
        wp->jumpAccelY = 0.0f;
        wp->jumpJerkY = 0.0f;
        wp->jumpJerkStepY = 0.0f;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        wp->hitobjFront = hit;
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
                wp->hitobjWall = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
        } else {
            s32 t;

            wp->frontWallDist = f8;
            wp->hitobjWall = hit;
            t = wp->field_0x3F + 1;
            wp->field_0x3F = t;
            if ((s8)t >= 0x14) {
                wp->field_0x3F = 0x14;
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

// `pos` is passed by value (the callers in mot_jump build it in a local first), so MWCC knows it can't alias the
// inlined probe's query and loads all of it before the query stores.
HitEntry* marioChkFront(f32* out, Vec pos, f32 angle, f32 dist) {
    Vec hitPos;
    Vec hitNormal;
    Vec dir;
    HitEntry* hit;
    f32 distance;
    f32 s;
    f32 c;

    sincosf(&s, &c, angle);
    dir = (Vec){ 0.0f, 0.0f, 0.0f };
    dir.x = s;
    dir.z = c;
    distance = dist;
    hit = marioHitCheckVec(&pos, &dir, &hitPos, &hitNormal, &distance);
    if (hit != NULL && distance <= dist) {
        *out = distABf(pos.x, pos.z, hitPos.x, hitPos.z);
        return hit;
    }
    *out = -1.0f;
    return NULL;
}

// The three passes probe through the file's marioHitCheckVec (inlined). Directions are compound literals; the
// later passes re-assign `dir` (pass 3 inside its loop) and pass 2 re-copies `*pos`, so MWCC splits them into the
// original's stack objects. `reach` holds the pass-1 ray length (the original's f30 under `dist`'s f31), and hit2 is
// cleared before each sincosf, which keeps it in r26.
HitEntry* searchFrontWall(Vec* pos, f32* out, f32 a, f32 b, f32 c) {
    MarioWork* wp = marioGetPtr();
    Vec p;
    Vec dir;
    Vec hitPos;
    Vec bestNorm;
    Vec norm;
    f32 half[4];
    HitEntry* hit;
    HitEntry* best;
    HitEntry* hit2;
    f32 sin;
    f32 cos;
    f32 sin2;
    f32 cos2;
    f32 dist;
    f32 reach;
    f32 deltaX;
    f32 dx;
    f32 deltaZ;
    f32 dz;
    f32 bestDist;
    f32 nz;
    f32 dot;
    f32 curDist;
    f32 marioHeight;
    f32 nx;
    s32 count;
    s32 i;

    if (vivianGetStatus() == 2) {
        return NULL;
    }
    sincosf(&sin, &cos, b);
    if (marioChkSts(0x10) != 0 && strcmp(gp->currentMapName, "eki_04") == 0) {
        if (__fabsf(wp->slopeTiltX) >= 45.0f ||
            __fabsf(wp->slopeTiltZ) >= 45.0f) {
            pos->x += a * sin;
            pos->z += a * cos;
            return NULL;
        }
    }
    count = 2;
    if (wp->characterId == 2) {
        if (wp->field_0x14 & 1) {
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

        marioHeight = wp2->wPlayerCollisionBox.y;

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
    dir = (Vec){sin, 0.0f, cos};
    reach = dist + a;
    bestDist = reach;
    best = NULL;
    for (i = 0; i < count; i++) {
        p.y = pos->y + half[i];
        curDist = reach;
        hit = marioHitCheckVec(&p, &dir, &hitPos, &norm, &curDist);
        if (hit != NULL) {
            if (chkBalloon(hit) != 0) {
                wp->hitobjBalloon = hit;
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
        nx = bestNorm.x;
        nz = bestNorm.z;
        dz = cos * a;
        dx = sin * a;
        dot = dx * nx + dz * nz;

        deltaX = 0.5f * (dx - dot * nx);
        deltaZ = 0.5f * (dz - dot * nz);
    }
    hit2 = NULL;
    sincosf(&sin2, &cos2, 45.0f + b);
    curDist = dist;
    p = *pos;
    dir = (Vec){sin2, 0.0f, cos2};
    for (i = 0; i < count; i++) {
        p.y = pos->y + half[i];
        curDist = dist;
        hit2 = marioHitCheckVec(&p, &dir, NULL, NULL, &curDist);
        if (hit2 != NULL) {
            break;
        }
    }
    if (hit2 != NULL) {
        hit2 = NULL;
        sincosf(&sin2, &cos2, b - 45.0f);
        for (i = 0; i < count; i++) {
            p.y = pos->y + half[i];
            curDist = dist;
            dir = (Vec){sin2, 0.0f, cos2};
            hit2 = marioHitCheckVec(&p, &dir, NULL, NULL, &curDist);
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

static const Vec vec3_802c5868 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5874 = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5880 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c588c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c5898 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58a4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c58b0 = { 0.0f, 0.0f, 0.0f };

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
    toMovedir(wp->dispDir);
    hit = wp->hitobjFront;
    if (hit != NULL) {
        return hit;
    }
    pos = wp->position;
    if (strcmp(gp->currentMapName, "dou_10") == 0 &&
        !(vivianGetStatus() == 2 || vivianGetStatus() == 1 || vivianGetStatus() == 3) && hit == NULL) {
        dist = (wp->flags & 0x100000) ? 2.0f : 8.0f;
        tmp = pos;
        tmp.y += 60.f;
        dir = vec3_802c5868;
        dist100 = 100.f;
        hit = marioHitCheckVec(&tmp, &dir, &hitPos, &hitNormal, &dist100);
        if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
            wp->hitobjFront = hit;
            *out = 0.8f;
        }

        pos = wp->position;
        hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
        if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
            wp->hitobjFront = hit;
            *out = 0.8f;
        }

        pos = wp->position;
        movePos(&pos.x, &pos.z, dist, 270.f);
        hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
        if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
            wp->hitobjFront = hit;
            *out = 0.8f;
        }

        pos = wp->position;
        movePos(&pos.x, &pos.z, dist, 90.f);
        hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
        if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
            wp->hitobjFront = hit;
            *out = 0.8f;
        }
    }
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

    toMovedir(wp->dispDir);
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
        wp->hitobjFront = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        wp->hitobjFront = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, 8.0f, 270.f);
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        wp->hitobjFront = hit;
        *out = 0.8f;
    }

    pos = wp->position;
    movePos(&pos.x, &pos.z, 8.0f, 90.f);
    hit = searchFrontWall(&pos, &f8, 20.f, 0.0f, 20.f);
    if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
        wp->hitobjFront = hit;
        *out = 0.8f;
    }
    return hit;
}

// Matches. The original builds targetPos by first copying a named zero vector
// (vec3_802c5880 / vec3_802c5898) and then overwriting the components, and adds the
// float_5 clearance after the copy; both shapes are needed for the exact stack layout.
// 0x1E0 is unnamed MarioWork padding.
HitEntry* N_dou10_yoko_yari3(void* unused) {
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

    if (strcmp(gp->currentMapName, "dou_10") != 0) {
        return NULL;
    }

    vx = wp->position.x;
    vy = wp->position.y;
    vz = wp->position.z;
    movePos(&vx, &vz, 10.f, wp->directionView);
    v = vec3_802c5880;
    v.x = vx;
    v.y = vy;
    v.z = vz;
    query.targetPos = v;
    query.targetPos.y += 5.0f;
    sincosf(&sn, &cs, 0.0f);
    dir = vec3_802c588c;
    dir.x = sn;
    dir.z = cs;
    query.targetDir = dir;
    query.targetDistance = 40.f;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        wp->hitobjFront = hit;
        return hit;
    }
    vx = wp->position.x;
    vy = wp->position.y;
    vz = wp->position.z;
    movePos(&vx, &vz, -10.f, wp->directionView);
    v2 = vec3_802c5898;
    v2.x = vx;
    v2.y = vy;
    v2.z = vz;
    query.targetPos = v2;
    query.targetPos.y += 5.0f;
    sincosf(&sn, &cs, 0.0f);
    dir2 = vec3_802c58a4;
    dir2.x = sn;
    dir2.z = cs;
    query.targetDir = dir2;
    query.targetDistance = 40.f;
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        wp->hitobjFront = hit;
    }
    return hit;
}

// The 0.01-rounded angle fields use 100.0 with an unconditional +0.5 (ROUND_CENTI). The probe is the file's own
// marioHitCheckVec (inlined: its distance rounding is dead and its hitPos/normal tests fold, which is the original's
// shape and frame); float declaration order from a rasim search; the step-down case falls through to the shared
// `return NULL`. `hit` is a `void*`: the conversion from the probe's HitEntry* keeps the inlined result in r3 until
// after the NULL test (as a HitEntry* it is coalesced into r29).
HitEntry* marioChkFrontStep(f32* outY, f32* outDist, f32* outAngle) {
    MarioWork* wp = marioGetPtr();
    MarioWork* wp2;
    Vec dir;
    f32 dist;
    void* hit;
    Vec hitPos;
    Vec n;
    Vec v;
    f32 s;
    f32 h;
    f32 angle;
    f32 px;
    f32 rad;
    f32 py;
    f32 pz;
    f32 off;
    f32 c;

    wp->slopeTiltX = 0.0f;
    wp->slopeTiltY = 0.0f;
    wp->slopeTiltZ = 0.0f;
    wp->slopeAngle = 0.0f;
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
        angle = toMovedir(wp->dispDir);
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
    v = vec3_802c58b0;
    {
        f32 t = off * s;
        v.x = px + t;
        v.y = py + h;
        pz = wp->position.z;
        t = off * -c;
        v.z = pz + t;
    }
    dir = (Vec){ 0.0f, -1.0f, 0.0f };
    dist = 0.1f + (11.0f + h);
    hit = marioHitCheckVec(&v, &dir, &hitPos, &n, &dist);
    if (hit != NULL) {
        wp2 = marioGetPtr();
        wp2->slopeTiltX = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * n.z, 100.f * n.y) - 180.f);
        wp2->slopeTiltY = 0.0f;
        wp2->slopeTiltZ = ROUND_CENTI(angleABf(0.0f, 0.0f, 100.f * n.x, 100.f * n.y) - 180.f);
        wp->slopeAngle =
            angleABf(0.0f, 0.0f, sqrtf(n.x * n.x + n.z * n.z), -n.y);
        if (hitPos.y < py && __fabsf(hitPos.y - py) >= 2.0f) {
            *outDist = py - hitPos.y;
            *outAngle = angleABf(px, pz, hitPos.x, hitPos.z);
        } else {
            *outY = hitPos.y;
            *outDist = 0.0f;
            *outAngle = angleABf(px, pz, hitPos.x, hitPos.z);
            return hit;
        }
    }
    return NULL;
}

#ifdef MUST_MATCH
// The front-step probe from before the rolling form: the ray starts at 3/4 of the height, with no roll case and no
// angle rounding. Uncalled in retail; generated between marioChkWallAround and marioChkFrontStep in this deferred
// unit, so 0.75, -1, pi, 0.1 and 2 pool at that point.
HitEntry* marioChkFrontStep_old(f32* outY, f32* outDist, f32* outAngle) {
    MarioWork* wp = marioGetPtr();
    HitCheckQuery query;
    HitEntry* hit;
    f32 h;
    f32 angle;
    f32 rad;
    f32 px;
    f32 py;
    f32 pz;

    h = 0.75f * wp->wPlayerCollisionBox.y;
    py = wp->position.y;
    *outY = py;
    *outDist = -1.0f;
    if (wp->baseSpeed) {
        angle = wp->directionView;
    } else {
        angle = toMovedir(wp->dispDir);
    }
    rad = 3.1415927f * angle / 180.f;
    px = wp->position.x;
    pz = wp->position.z;
    query.targetPos.x = px + 0.5f * wp->wPlayerCollisionBox.x * (f32)sin(rad);
    query.targetPos.y = py + h;
    query.targetPos.z = pz - 0.5f * wp->wPlayerCollisionBox.x * (f32)cos(rad);
    query.targetDir.x = 0.0f;
    query.targetDir.y = -1.0f;
    query.targetDir.z = 0.0f;
    query.targetDistance = 0.1f + (11.0f + h);
    hit = hitCheckVecFilter(&query, chkfilterVec);
    if (hit != NULL) {
        if (query.hitPos.y < py && __fabsf(query.hitPos.y - py) >= 2.0f) {
            *outDist = py - query.hitPos.y;
            *outAngle = angleABf(px, pz, query.hitPos.x, query.hitPos.z);
            return NULL;
        }
        *outY = query.hitPos.y;
        *outDist = 0.0f;
        *outAngle = angleABf(px, pz, query.hitPos.x, query.hitPos.z);
    }
    return hit;
}
#endif

// Each sweep copies `*position` into pHigh/pLow, raises their y, and probes through the file's marioHitCheckVec
// (NULL hitPos/normal, so only the distance rounds). Directions are compound literals; the second sweep's angle is
// its own variable. d1 and d2 are both set before the first probe (`d2 = d1 = dist`), and the push distance is
// written out at each use. The second sweep's push spells out `0.5f * c` instead of reading dist; with dist at every
// site the entry block schedules the multiply and the y stores in a different order. Float declaration order from a
// rasim search.
s32 marioChkWallAround(Vec* position, s32 flag, f32 a, f32 b, f32 c, f32 d) {
    HitEntry* hit;
    HitEntry* hit2;
    Vec pHigh;
    Vec pLow;
    Vec dir;
    Vec disp;
    Vec prev;
    f32 sin;
    f32 cos;
    f32 bsin;
    f32 bcos;
    f32 dist;
    f32 angle;
    f32 angle2;
    f32 d1;
    f32 d2;
    f32 step;
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
    pHigh = *position;
    pLow = *position;
    disp = (Vec){0.0f, 0.0f, 0.0f};
    pHigh.y += 11.0f;
    pLow.y += d - 1.0f;
    for (i = 0; i < n; i++) {
        sincosf(&sin, &cos, angle);
        dir = (Vec){sin, 0.0f, cos};
        d2 = d1 = dist;
        hit = marioHitCheckVec(&pHigh, &dir, NULL, NULL, &d1);
        hit2 = marioHitCheckVec(&pLow, &dir, NULL, NULL, &d2);
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
            sincosf(&bsin, &bcos, 180.f + angle);
            disp.x += bsin * __fabsf(dist - d1);
            disp.z += bcos * __fabsf(dist - d1);
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
    angle2 = b;
    pHigh = *position;
    pLow = *position;
    disp = (Vec){0.0f, 0.0f, 0.0f};
    pHigh.y += 11.0f;
    pLow.y += d - 1.0f;
    for (i = 0; i < n; i++) {
        sincosf(&sin, &cos, angle2);
        dir = (Vec){sin, 0.0f, cos};
        d2 = d1 = dist;
        hit = marioHitCheckVec(&pHigh, &dir, NULL, NULL, &d1);
        hit2 = marioHitCheckVec(&pLow, &dir, NULL, NULL, &d2);
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
            sincosf(&bsin, &bcos, 180.f + angle2);
            disp.x += bsin * __fabsf(0.5f * c - d1);
            disp.z += bcos * __fabsf(0.5f * c - d1);
            found = 1;
        }
        angle2 += step;
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

MEM_VEC vec3_802c58f8 = { 0.0f, 0.0f, 0.0f };
MEM_VEC vec3_802c5904 = { 0.0f, 0.0f, 0.0f };
MEM_VEC vec3_802c5910 = { 0.0f, 0.0f, 0.0f };

// Each dir vector is copied from its named template (MEM_VEC: non-const under MUST_MATCH, so the copy stays inside
// the loop and is not hoisted as loop-invariant), then x/z are overwritten with sn/cs.
s32 marioSlitChkWallAround(void) {
    MarioWork* wp = marioGetPtr();
    s32 flags[8];
    HitEntry* hit;
    Vec pos;
    f32 sn;
    f32 cs;
    f32 angle;
    s32 i;
    f32 dist;

    pos = wp->position;
    pos.y += 11.0f;
    angle = 0.0f;
    for (i = 0; i < 8; i++) {
        Vec dir;
        flags[i] = 0;
        sincosf(&sn, &cs, angle);
        dir = vec3_802c58f8;
        dir.x = sn;
        dir.z = cs;
        dist = 13.0f;
        hit = marioHitCheckVec(&pos, &dir, NULL, NULL, &dist);
        if (hit != NULL) {
            if (marioChkBero(hit) == 0) {
                flags[i] = 1;
            }
        }
        angle += 45.0f;
    }

#ifdef MUST_MATCH
    {
        f32 ofs = 5.0f; // a named constant: with the literal the add's two loads swap

        pos.y = wp->position.y + ofs;
    }
#else
    pos.y = wp->position.y + 5.0f;
#endif
    angle = 0.0f;
    for (i = 0; i < 8; i++) {
        Vec dir;
        if (flags[i] != 1) {
            sincosf(&sn, &cs, angle);
            dir = vec3_802c5904;
            dir.x = sn;
            dir.z = cs;
            dist = 13.0f;
            hit = marioHitCheckVec(&pos, &dir, NULL, NULL, &dist);
            if (hit != NULL) {
                if (marioChkBero(hit) == 0) {
                    flags[i] = 1;
                }
            }
        }
        angle += 45.0f;
    }

    pos.y = wp->position.y + (wp->wPlayerCollisionBox.y - 1.0f);
    angle = 0.0f;
    for (i = 0; i < 8; i++) {
        Vec dir;
        if (flags[i] != 1) {
            sincosf(&sn, &cs, angle);
            dir = vec3_802c5910;
            dir.x = sn;
            dir.z = cs;
            dist = 13.0f;
            hit = marioHitCheckVec(&pos, &dir, NULL, NULL, &dist);
            if (hit != NULL) {
                if (marioChkBero(hit) == 0) {
                    flags[i] = 1;
                }
            }
        }
        angle += 45.0f;
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

    wp->frontWallDist = 1000.f;
    if (a && wp->airTimer == 0) {
        if (wp->wallDist < a) {
            wp->wallDist = a;
            wp->wallYaw = b;
            wp->wallHoldTimer = 0xE;
        } else {
            if (--wp->wallHoldTimer <= 0) {
                wp->wallHoldTimer = 0;
                wp->wallDist = a;
                wp->wallYaw = b;
            }
        }
    } else {
        wp->wallDist = 0.0f;
    }

    if (a && wp->airTimer == 0) {
        wp->moveDir = wp->wallYaw;
    }

    if (!a) {
        marioSearchFrontWall_staying(b);
        return;
    }

    v = wp->position;
    hit = searchFrontWall(&v, &f8, a, b, wp->wPlayerCollisionBox.z);
    if (hit != NULL) {
        MarioWork* wp2;

        wp->hitobjFront = hit;
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
                wp->hitobjWall = hit;
            }
            movePos(&wp->position.x, &wp->position.z, a, b);
            v = wp->position;
            return;
        } else {
            if (!a) {
                wp->field_0x3F = 0;
            } else {
                s32 t;

                wp->frontWallDist = f8;
                wp->hitobjWall = hit;
                t = wp->field_0x3F + 1;
                wp->field_0x3F = t;
                if ((s8)t >= 0x14) {
                    wp->field_0x3F = 0x14;
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
// The match build gets these four from compound-literal templates; the natural build names them.
static const Vec vec3_802c5760 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802c576c = { 0.0f, -1.0f, 0.0f };
static const Vec vec3_802c5814 = { 0.0f, 1.0f, 0.0f };
static const Vec vec3_802c5820 = { 0.0f, 1.0f, 0.0f };
#endif

//.sdata2 (defined after the code so the uses stay loads of the named symbols)
