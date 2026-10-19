#include "party.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "mario/mario.h"
#include "mario/mario_hit.h"
#include "mario/mario_sbr.h"
#include "math_ext.h"
#include "system.h"
#include "party/party_hit.h"
#include "party/party_yoshi.h"
#include "party/party_move.h"
#include <string.h>

// The original reads its .rodata (Vec templates, sqrtf doubles, block names) through one base register.
#pragma pool_data on

// LOCAL_PROTO for out-of-line MSL math used below.
#define fabsf(x) __fabsf(x)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

extern s32 __float_nan[];
#define NAN (*(f32*)__float_nan)

// Float sqrt used by the original: inline expansion with NaN classification.
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
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

#define ROUND_MILLI(v) ((f32)(s32)(1000.0f * (v) + ((v) >= 0.0f ? 0.5f : -0.5f)) / 1000.0f)

BOOL chkfilter(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;

    marioGetPtr();
    attr = entry->attributes;
    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800004) != 0) {
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

BOOL chkfilterYoshi(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;

    marioGetPtr();
    attr = entry->attributes;
    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) != 0) {
        return TRUE;
    }
    if ((attr & 0x4) != 0) {
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

BOOL chkfilterNokotaro(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;

    marioGetPtr();
    attr = entry->attributes;
    if ((attr & 0x800005) != 0) {
        return FALSE;
    }
    if ((attr & 0x80080) != 0) {
        return TRUE;
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

HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* hitPos, Vec* hitNormal, f32* dist) {
    Vec dirCopy = *dir;
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *pos;
    query.targetDir = dirCopy;
    query.targetDistance = *dist;

    if (yoshiGetStatus() != 0) {
        hit = hitCheckVecFilter(&query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        hit = hitCheckVecFilter(&query, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query, chkfilter);
    }

    if (hit == NULL) {
        return hit;
    }

    *dist = (f32)(s32)(1000.0f * query.targetDistance +
                       (query.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
            1000.0f;
    hitPos->x = (f32)(s32)(1000.0f * query.hitPos.x +
                           (query.hitPos.x >= 0.0f ? 0.5f : -0.5f)) /
                1000.0f;
    hitPos->y = (f32)(s32)(1000.0f * query.hitPos.y +
                           (query.hitPos.y >= 0.0f ? 0.5f : -0.5f)) /
                1000.0f;
    hitPos->z = (f32)(s32)(1000.0f * query.hitPos.z +
                           (query.hitPos.z >= 0.0f ? 0.5f : -0.5f)) /
                1000.0f;
    hitNormal->x = query.hitNormal.x;
    hitNormal->y = query.hitNormal.y;
    hitNormal->z = query.hitNormal.z;
    return hit;
}

f32 partyNrmToAngle(Vec v) {
    f32 len = sqrtf(v.x * v.x + v.z * v.z);

    return angleABf(0.0f, 0.0f, len, -v.y);
}

void partyChkGnd(PartyEntry* party) {
    s32 attr;
    HitEntry* hit;
    f32 a, b, c;

    if (party->flags & PARTY_FLAG_FORCE_FALL) {
        return;
    }
    attr = 0;
    hit = partySearchGround(party, 11.0f, party->jumpPara1);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
        }
    }
    party->groundHit = hit;
    party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
    if (hit == NULL) {
        party->flags = (party->flags & ~PARTY_FLAG_IS_GROUNDED);
        partyChgMot(party, 3);
        party->jumpPara3 = -1.0f;
    } else {
        party->flags = (party->flags | PARTY_FLAG_IS_GROUNDED);
        if (attr & 0x100) {
            party->flags = (party->flags | PARTY_FLAG_IN_SHALLOW_WATER);
        } else {
            party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
        }
    }
    party->position.y = party->groundY;
    if (party->flags & 0x30) {
        return;
    }
    partyChkFrontStep(party, 11.0f, &a, &b, &c);
    if (a - party->position.y >= 5.0f && *(f32*)((u8*)party + 0xE0) <= 30.0f) {
        party->field_120 = a;
        if (party->speed) {
            partyChgMot(party, 6);
        }
    }
}

// The ground probe's filter dispatch and hit read: the early return on a miss is the original's `bne; b` layout.
static inline HitEntry* partyGroundHit(PartyEntry* party, HitCheckQuery* query, f32* hitY, Vec* n) {
    HitEntry* h;

    if (yoshiGetStatus() != 0) {
        h = hitCheckVecFilter(query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        h = hitCheckVecFilter(query, chkfilterNokotaro);
    } else {
        h = hitCheckVecFilter(query, chkfilter);
    }
    if (h == NULL) {
        return h;
    }
    *hitY = ROUND_MILLI(query->hitPos.y);
    n->x = query->hitNormal.x;
    n->y = query->hitNormal.y;
    n->z = query->hitNormal.z;
    return h;
}

static inline f32 partyGroundSlope(PartyEntry* party, Vec n) {
    *(f32*)(party->field_C4 + 0x00) = (f32)(s32)(angleABf(0.0f, 0.0f, 100.0f * n.z, 100.0f * n.y) - 180.0f);
    *(f32*)(party->field_C4 + 0x04) = 0.0f;
    *(f32*)(party->field_C4 + 0x08) = (f32)(s32)(angleABf(0.0f, 0.0f, 100.0f * n.x, 100.0f * n.y) - 180.0f);
    return angleABf(0.0f, 0.0f, sqrtf(n.x * n.x + n.z * n.z), -n.y);
}

HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down) {
    HitCheckQuery query;
    Vec targetPos;
    Vec dir;
    Vec n;
    HitEntry* hit;
    HitEntry* best;
    f32 dz;
    f32 y;
    f32 hitY;
    f32 noCos;
    f32 noSin;
    f32 off;
    f32 targetDist;
    f32 c;
    f32 bestY;
    f32 h;
    f32 offSin;
    f32 offCos;
    f32 px;
    f32 rad;
    f32 s;
    f32 py;
    f32 dx;
    f32 pz;
    s32 i;

    if (party->flags & PARTY_FLAG_FORCE_FALL) {
        return NULL;
    }
    *(Vec*)party->field_C4 = (Vec){0.0f, 0.0f, 0.0f};
    *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = 0.0f;
    off = __fabsf(up);
    off += 0.1f;
    if (marioGetPtr()->currMotionId == MARIO_MOTION_YOSHI) {
        h = 5.0f;
    } else {
        h = 0.75f * partyGetHeight(party);
    }
    party->groundY = party->position.y;
    *(f32*)party->field_E8 = -1.0f;
    if (party->speed) {
        rad = party->dir;
    } else {
        rad = partyToMovedir(party, party->field_10C);
    }
    rad = (3.1415927f * rad) / 180.0f;
    s = (f32)sin((f64)rad);
    offSin = 0.375f * (party->radius * s);
    c = (f32)cos((f64)rad);
    py = party->position.y;
    noSin = -offSin;
    y = py + h;
    px = party->position.x;
    offCos = 0.375f * (party->radius * -c);
    targetDist = h + off;
    pz = party->position.z;
    noCos = -offCos;
    dir = (Vec){0.0f, -1.0f, 0.0f};
    best = NULL;
    bestY = -10000.0f;
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            dx = 0.0f;
            dz = 0.0f;
        } else if (i == 1) {
            dx = offSin;
            dz = offCos;
        } else if (i == 2) {
            dx = noCos;
            dz = offSin;
        } else if (i == 3) {
            dx = offCos;
            dz = noSin;
        } else if (i == 4) {
            dx = noSin;
            dz = noCos;
        }
        targetPos.x = px + dx;
        targetPos.y = y;
        targetPos.z = pz + dz;
        query.targetPos = targetPos;
        query.targetDir = dir;
        query.targetDistance = targetDist;

        hit = partyGroundHit(party, &query, &hitY, &n);
        if (hit != NULL) {
            if (partyGroundSlope(party, n) >= 50.0f) {
                hit = NULL;
            }
        }
        if (hit != NULL) {
            *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = partyGroundSlope(party, n);
            if (hitY < py && __fabsf(hitY - py) >= 2.0f) {
                if (__fabsf(hitY - py) > __fabsf(down)) {
                    *(f32*)party->field_E8 = __fabsf(py - hitY);
                } else {
                    if (hitY > bestY) {
                        bestY = hitY;
                        best = hit;
                    }
                }
            } else if (__fabsf(hitY - py) <= 11.0f) {
                if (hitY > bestY) {
                    bestY = hitY;
                    best = hit;
                }
            }
        }
    }
    if (best != NULL) {
        party->groundY = bestY;
        *(f32*)party->field_E8 = 0.0f;
        return best;
    }
    return NULL;
}

static inline HitEntry* partyFrontStepHit(PartyEntry* party, HitCheckQuery* query, Vec* hitPos, Vec* n) {
    HitEntry* h;

    if (yoshiGetStatus() != 0) {
        h = hitCheckVecFilter(query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        h = hitCheckVecFilter(query, chkfilterNokotaro);
    } else {
        h = hitCheckVecFilter(query, chkfilter);
    }
    if (h == NULL) {
        return h;
    }
    hitPos->x = ROUND_MILLI(query->hitPos.x);
    hitPos->y = ROUND_MILLI(query->hitPos.y);
    hitPos->z = ROUND_MILLI(query->hitPos.z);
    n->x = query->hitNormal.x;
    n->y = query->hitNormal.y;
    n->z = query->hitNormal.z;
    return h;
}

u32 partyChkFrontStep(PartyEntry* party, f32 a, f32* out1, f32* out2, f32* out3) {
    Vec n;
    HitCheckQuery query;
    HitEntry* hit;
    Vec hitPos;
    f32 off;
    f32 angle;
    f32 c;
    f32 h;
    f32 px;
    f32 py;
    f32 s;
    f32 rad;
    f32 pz;

    a += 0.01f;
    *(Vec*)party->field_C4 = (Vec){0.0f, 0.0f, 0.0f};
    *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = 0.0f;
    h = partyGetHeight(party);
    *out1 = party->position.y;
    *out2 = -1.0f;
    if (party->speed) {
        angle = party->dir;
    } else {
        angle = toMovedir(party->field_10C);
    }
    angle = revise360((f32)(s32)(0.5f + angle));
    off = 0.265f * party->radius;
    rad = (3.1415927f * angle) / 180.0f;
    s = (f32)sin((f64)rad);
    c = (f32)cos((f64)rad);
    px = party->position.x;
    py = party->position.y;
    pz = party->position.z;
    {
        f32 dx = off * s;
        f32 dz = off * -c;
        query.targetPos = (Vec){px + dx, py + h, pz + dz};
    }
    query.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    query.targetDistance = h + a;

    hit = partyFrontStepHit(party, &query, &hitPos, &n);
    if (hit != NULL) {
        *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = partyGroundSlope(party, n);
        if (hitPos.y < py && __fabsf(hitPos.y - py) >= 2.0f) {
            *out2 = py - hitPos.y;
            *out3 = angleABf(px, pz, hitPos.x, hitPos.z);
        } else {
            *out1 = hitPos.y;
            *out2 = 0.0f;
            *out3 = angleABf(px, pz, n.x, n.z);
            return (u32)hit;
        }
    }
    return 0;
}

// Two zero Vec templates in the original .rodata that no code references (0x3C and 0x84 from the base).
const Vec vec3_802cd35c = {0.0f, 0.0f, 0.0f};

static inline void partyWallSlide(Vec* pos, Vec n, f32 dx, f32 dz) {
    f32 dot = dx * n.x + dz * n.z;

    pos->x += 0.5f * (dx - dot * n.x);
    pos->z += 0.5f * (dz - dot * n.z);
}

HitEntry* partySearchWallFront(PartyEntry* party, Vec* pos, f32 a, f32 b) {
    Vec p;
    Vec dir;
    Vec hpos;
    Vec bestNorm;
    Vec hnorm;
    Vec delta;
    f32 height[4];
    f32 sin, cos;
    f32 sin2, cos2;
    f32 len;
    f32 dist;
    f32 curDist;
    HitEntry* hit;
    HitEntry* best;
    s32 i;

    delta = (Vec){0.0f, 0.0f, 0.0f};
    sincosf(&sin, &cos, b);
    height[0] = 0.75f * partyGetHeight(party);
    height[1] = 0.5f * partyGetHeight(party);
    if ((party->flags & 0x30) != 0 || yoshiGetStatus() == 2) {
        height[2] = 1.0f;
    } else {
        height[2] = 11.0f;
    }
    len = 0.5f * party->radius;
    dist = len + a;
    p = *pos;
    dir = (Vec){sin, 0.0f, cos};
    best = NULL;
    for (i = 0; i < 3; i++) {
        curDist = len + a;
        p.y = pos->y + height[i];
        hit = marioHitCheckVec(&p, &dir, &hpos, &hnorm, &curDist);
        if (hit != NULL) {
            if (best != NULL) {
                if (curDist < dist) {
                    best = hit;
                    dist = curDist;
                    bestNorm = hnorm;
                }
            } else {
                best = hit;
                dist = curDist;
                bestNorm = hnorm;
            }
        }
    }
    if (best != NULL) {
        partyWallSlide(&delta, bestNorm, sin * a, cos * a);
    }

    sincosf(&sin2, &cos2, 45.0f + b);
    dist = len;
    p = *pos;
    dir = (Vec){sin2, 0.0f, cos2};
    for (i = 0; i < 3; i++) {
        dist = len;
        p.y = pos->y + height[i];
        hit = marioHitCheckVec(&p, &dir, NULL, NULL, &dist);
        if (hit != NULL) {
            break;
        }
    }
    if (hit != NULL) {
        sincosf(&sin2, &cos2, b - 45.0f);
        for (i = 0; i < 3; i++) {
            dist = len;
            p.y = pos->y + height[i];
            dir = (Vec){sin2, 0.0f, cos2};
            hit = marioHitCheckVec(&p, &dir, NULL, NULL, &dist);
        }
    }
    if (best != NULL) {
        if (hit == NULL) {
            pos->x += delta.x;
            pos->z += delta.z;
        }
        return best;
    }
    if (hit == NULL) {
        pos->x += a * sin;
        pos->z += a * cos;
    }
    return NULL;
}

HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir) {
    HitCheckQuery query1;
    HitCheckQuery query2;
    HitEntry* hit;
    HitEntry* hit2;
    HitEntry* h2;
    HitEntry* h1;
    Vec p;
    Vec n;
    Vec d;
    f32 sin, cos;
    f32 f31;
    f32 h;

    sincosf(&sin, &cos, dir);
    f31 = 0.5f * party->radius + dist;

    h = 0.75f * partyGetHeight(party);
    p = party->position;
    p.y += h;
    d = (Vec){sin, 0.0f, cos};
    query1.targetPos = p;
    query1.targetDir = d;
    query1.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        h1 = hitCheckVecFilter(&query1, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        h1 = hitCheckVecFilter(&query1, chkfilterNokotaro);
    } else {
        h1 = hitCheckVecFilter(&query1, chkfilter);
    }

    if (h1 == NULL) {
        hit = h1;
    } else {
        f31 = (f32)(s32)(1000.0f * query1.targetDistance +
                         (query1.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
              1000.0f;
        n.x = query1.hitNormal.x;
        n.y = query1.hitNormal.y;
        n.z = query1.hitNormal.z;
        hit = h1;
    }
    if (hit != NULL) {
        party->field_EC = f31;
        partyWallSlide(pos, n, sin * dist, cos * dist);
        return hit;
    }

    h = 11.0f;
    if (party->currentMemberId == PARTY_MEMBER_BOBBERY && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        h = 1.0f;
    }
    p = party->position;
    f31 = 0.5f * party->radius + dist;
    p.y += h;
    query2.targetPos = p;
    query2.targetDir = d;
    query2.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        h2 = hitCheckVecFilter(&query2, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        h2 = hitCheckVecFilter(&query2, chkfilterNokotaro);
    } else {
        h2 = hitCheckVecFilter(&query2, chkfilter);
    }

    if (h2 == NULL) {
        hit2 = h2;
    } else {
        f31 = (f32)(s32)(1000.0f * query2.targetDistance +
                         (query2.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
              1000.0f;
        n.x = query2.hitNormal.x;
        n.y = query2.hitNormal.y;
        n.z = query2.hitNormal.z;
        hit2 = h2;
    }
    if (hit2 != NULL) {
        if (angleABf(0.0f, 0.0f, sqrtf(n.x * n.x + n.z * n.z), -n.y) >= 45.0f) {
            party->field_EC = f31;
            partyWallSlide(pos, n, sin * dist, cos * dist);
            return hit2;
        }
    }
    party->field_EC = -1.0f;
    pos->x += dist * sin;
    pos->z += dist * cos;
    return NULL;
}

static inline HitEntry* partyHeadHit(Vec* pos, f32* dist, f32 up) {
    HitEntry* hit;
    f32 hx, hy, hz, hnx, hny, hnz, d;
    f32 y, z;
    Vec n = {0.0f, 0.0f, 0.0f}; // unused: its zero template is the original's unreferenced .rodata Vec

    y = pos->y;
    z = pos->z;
    d = *dist;
    hit = hitCheckFilter(NULL, pos->x, up + y, z, 0.0f, 1.0f, 0.0f, &hx, &hy, &hz, &d, &hnx, &hny, &hnz);
    if (hit != NULL) {
        if (*dist >= d) {
            *dist = d;
        }
    }
    return hit;
}

#ifdef MUST_MATCH
// Preserve the y/z snapshots and height operand order through the inlined probe calls.
#pragma push
#pragma opt_propagation off
#endif
HitEntry* partySearchHead(PartyEntry* party, Vec* position, f32* out, f32 angle) {
    HitEntry* hit;
    Vec q;
    f32 sin, cos;
#ifdef MUST_MATCH
    // Match-only: the original keeps the position copy in memory (stored once, reloaded by every probe).
    volatile f32 px, py, pz;
#else
    f32 px, py, pz;
#endif
    f32 h;
    f32 x, y, z;
    f32 dist;
    f32 offSin, offCos;

    h = 0.3f * party->radius;
    sincosf(&sin, &cos, angle);
    dist = *out;
    offCos = -h * cos;
    offSin = h * sin;
    px = position->x;
    py = position->y;
    pz = position->z;
    x = px + offSin;
    z = pz + offCos;
    y = py;
    q = (Vec){x, y, z};
    hit = partyHeadHit(&q, &dist, 10.0f);
    if (hit == NULL) {
        x = px - offSin;
        y = py;
        z = pz - offCos;
        q = (Vec){x, y, z};
        hit = partyHeadHit(&q, &dist, 10.0f);
        if (hit == NULL) {
            x = px + offCos;
            y = py;
            z = pz + offSin;
            q = (Vec){x, y, z};
            hit = partyHeadHit(&q, &dist, 10.0f);
            if (hit == NULL) {
                x = px - offCos;
                y = py;
                z = pz - offSin;
                q = (Vec){x, y, z};
                hit = partyHeadHit(&q, &dist, 10.0f);
            }
        }
    }
    if (hit != NULL) {
        position->x = x;
        position->y = y;
        position->z = z;
        *out = dist;
    }
    if (hit == NULL) {
        position->x = x;
        position->y = y;
        position->z = z;
        *out = 0.0f;
    }
    return hit;
}
#ifdef MUST_MATCH
#pragma pop
#endif


#ifdef MUST_MATCH
// Ceiling height above (x, y, z), rounded to 1/100. Uncalled in retail; generated first in this deferred unit,
// so 10, 0, 1, 0.5, 100 and the conversion double lead the pools.
HitEntry* partyGetRoofY(f32 x, f32 y, f32 z, f32* dist, f32* roofY) {
    HitEntry* hit;
    f32 hx, hy, hz, hnx, hny, hnz;

    hit = hitCheckFilter(NULL, x, y + 10.0f, z, 0.0f, 1.0f, 0.0f, &hx, &hy, &hz, dist, &hnx, &hny, &hnz);
    if (hit != NULL) {
        *roofY = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
    }
    return hit;
}
#endif
