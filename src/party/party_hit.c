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

// The original pools the sqrtf double constants into the rodata blob at vec3_802cd320.
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

// rodata blob; the pooled sqrtf doubles and block-name strings follow these.
const Vec vec3_802cd320 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd32c = {0.0f, -1.0f, 0.0f};
const Vec vec3_802cd338 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd344 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd350 = {0.0f, -1.0f, 0.0f};
const Vec vec3_802cd35c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd368 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd374 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd380 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd38c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd398 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3a4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3b0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3bc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3c8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3d4 = {0.0f, 0.0f, 0.0f};

// .rodata constants: declared here (tentatively) so the inline sqrtf below references the
// named globals instead of folding them into anonymous pool copies, and defined after the
// functions so the emitted .rodata order is magic / 0.5 / 3.0 / 0.0 / block-name strings.
const f64 double_to_int_802cd3e0;
const f64 double_0p5_802cd3e8;
const f64 double_3_802cd3f0;
const f64 double_0_802cd3f8;
const char str_802CD400[0x18];
const char str_802CD418[0x18];
const char str_802CD430[0x18];

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
        guess = double_0p5_802cd3e8 * guess * (double_3_802cd3f0 - guess * guess * x);
        guess = double_0p5_802cd3e8 * guess * (double_3_802cd3f0 - guess * guess * x);
        guess = double_0p5_802cd3e8 * guess * (double_3_802cd3f0 - guess * guess * x);
        return x * guess;
    } else if (x < double_0_802cd3f8) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// .sdata2 constants
extern const f32 float_10_8041b3e8;
extern const f32 float_0_8041b3ec;
extern const f32 float_1_8041b3f0;
extern const f32 float_0p5_8041b3f4;
extern const f32 float_100_8041b3f8;
extern const f32 float_0p3_8041b3fc;
extern const f32 float_0p75_8041b400;
extern const f32 float_neg0p5_8041b404;
extern const f32 float_1000_8041b408;
extern const f32 float_11_8041b40c;
extern const f32 float_45_8041b410;
extern const f32 float_neg1_8041b414;
extern const f32 float_0p01_8041b418;
extern const f32 float_0p265_8041b41c;
extern const f32 float_3p1416_8041b420;
extern const f32 float_180_8041b424;
extern const f32 float_2_8041b428;
extern const f32 float_0p1_8041b42c;
extern const f32 float_5_8041b430;
extern const f32 float_0p375_8041b434;
extern const f32 float_neg10000_8041b438;
extern const f32 float_50_8041b43c;
extern const f32 float_30_8041b440;

// NONMATCHING (71.38%): stack layout and callee-saved registers match retail. The key was that the hit
// outputs are scalar f32, not Vecs: MWCC groups aggregates above scalars, so scalars interleave (outputs
// 0x08-0x74, position 0x78-0x80, sin/cos 0x84-0x88). Left: MWCC's p/q store and constant-load scheduling.
HitEntry* partySearchHead(PartyEntry* party, Vec* position, f32* out, f32 angle) {
    HitEntry* hit;
    Vec q;
    Vec p1, p2, p3, p4;
    f32 sin, cos;
    f32 h;
    f32 x;
    f32 py;
    f32 z;
    f32 dist;
    f32 offSin, offCos;
#ifdef MUST_MATCH
    // Match-only: forcing the three position scalars through volatile locals reproduces the
    // retail spill of position->x/y/z to stack (the plain Vec copy lands in the wrong slot).
    volatile f32 pvx, pvy, pvz;
#endif
    f32 d1, hnz1, hny1, hnx1, hz1, hy1, hx1;
    f32 d2, hnz2, hny2, hnx2, hz2, hy2, hx2;
    f32 d3, hnz3, hny3, hnx3, hz3, hy3, hx3;
    f32 d4, hnz4, hny4, hnx4, hz4, hy4, hx4;

    h = 0.3f * party->field_F4;
    sincosf(&sin, &cos, angle);
#ifdef MUST_MATCH
    // Match-only ordering: `-cos * h` (not `-h * cos`) and the position store between the two
    // offset products reproduce retail's scheduling; the natural form is under #else.
    offCos = -cos * h;
    pvx = position->x;
    offSin = h * sin;
    pvy = position->y;
    pvz = position->z;
    x = pvx + offSin;
    z = pvz + offCos;
    py = pvy;
    dist = *out;
#else
    offCos = -h * cos;
    offSin = h * sin;
    py = position->y;
    dist = *out;
    x = position->x + offSin;
    z = position->z + offCos;
#endif
    p1 = vec3_802cd3b0;
    p1.y = py;
    p1.z = z;
    p1.x = x;
    q = p1;
    d1 = dist;
    hit = hitCheckFilter(NULL, &hx1, &hy1, &hz1, &d1, &hnx1, &hny1, &hnz1, q.x, q.y + 10.0f, q.z, 0.0f, 1.0f, 0.0f);
    if (hit != NULL) {
        if (dist >= d1) {
            dist = d1;
        }
    }
    if (hit == NULL) {
#ifdef MUST_MATCH
        x = pvx - offSin;
        z = pvz - offCos;
#else
        x = position->x - offSin;
        z = position->z - offCos;
#endif
        p2 = vec3_802cd3bc;
        p2.y = py;
        p2.z = z;
        p2.x = x;
        q = p2;
        d2 = dist;
        hit = hitCheckFilter(NULL, &hx2, &hy2, &hz2, &d2, &hnx2, &hny2, &hnz2, q.x, q.y + 10.0f, q.z, 0.0f, 1.0f,
                             0.0f);
        if (hit != NULL) {
            if (dist >= d2) {
                dist = d2;
            }
        }
    }
    if (hit == NULL) {
#ifdef MUST_MATCH
        x = pvx + offCos;
        z = pvz + offSin;
#else
        x = position->x + offCos;
        z = position->z + offSin;
#endif
        p3 = vec3_802cd3c8;
        p3.y = py;
        p3.z = z;
        p3.x = x;
        q = p3;
        d3 = dist;
        hit = hitCheckFilter(NULL, &hx3, &hy3, &hz3, &d3, &hnx3, &hny3, &hnz3, q.x, q.y + 10.0f, q.z, 0.0f, 1.0f,
                             0.0f);
        if (hit != NULL) {
            if (dist >= d3) {
                dist = d3;
            }
        }
    }
    if (hit == NULL) {
#ifdef MUST_MATCH
        x = pvx - offCos;
        z = pvz - offSin;
#else
        x = position->x - offCos;
        z = position->z - offSin;
#endif
        p4 = vec3_802cd3d4;
        p4.y = py;
        p4.z = z;
        p4.x = x;
        q = p4;
        d4 = dist;
        hit = hitCheckFilter(NULL, &hx4, &hy4, &hz4, &d4, &hnx4, &hny4, &hnz4, q.x, q.y + 10.0f, q.z, 0.0f, 1.0f,
                             0.0f);
        if (hit != NULL) {
            if (dist >= d4) {
                dist = d4;
            }
        }
    }
    if (hit != NULL) {
        position->x = x;
        position->y = py;
        position->z = z;
        *out = dist;
    }
    if (hit == NULL) {
        position->x = x;
        position->y = py;
        position->z = z;
        *out = 0.0f;
    }
    return hit;
}

// NONMATCHING (99.82%): the `h2` intermediate for the second filter and flipping the two
// `!= NULL` tests to `== NULL` reproduced retail's phi and branch layout (and with them the GPR
// rotation). Only the f4/f6 nor.z-vs-dot FP colouring and the pooled i2f magic (0x128 vs 0xc0) remain.
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
    f32 sx, cz;
    f32 sin, cos;
    f32 f31;
    f32 h;
    f32 dot;

    sincosf(&sin, &cos, dir);
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4) + dist;

    h = 0.75f * partyGetHeight(party);
    p = party->position;
    p.y += h;
    d = vec3_802cd398;
    d.x = sin;
    d.z = cos;
    query1.targetPos = p;
    query1.targetDir = d;
    query1.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        h1 = hitCheckVecFilter(&query1, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
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
        Vec nor = n;
        *(f32*)((u8*)party + 0xEC) = f31;
        sx = sin * dist;
        cz = cos * dist;
        dot = sx * nor.x + cz * nor.z;
        pos->x += 0.5f * (sx - dot * nor.x);
        pos->z += 0.5f * (cz - dot * nor.z);
        return hit;
    }

    h = 11.0f;
    if (party->currentMemberId == PARTY_MEMBER_BOBBERY && (party->flags & 0x100)) {
        h = 1.0f;
    }
    p = party->position;
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4) + dist;
    p.y += h;
    query2.targetPos = p;
    query2.targetDir = d;
    query2.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        h2 = hitCheckVecFilter(&query2, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
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
        if (partyNrmToAngle(&n) >= 45.0f) {
            Vec nor = n;
            *(f32*)((u8*)party + 0xEC) = f31;
            sx = sin * dist;
            cz = cos * dist;
            dot = sx * nor.x + cz * nor.z;
            pos->x += 0.5f * (sx - dot * nor.x);
            pos->z += 0.5f * (cz - dot * nor.z);
            return hit2;
        }
    }
    *(f32*)((u8*)party + 0xEC) = -1.0f;
    pos->x += dist * sin;
    pos->z += dist * cos;
    return NULL;
}

// NONMATCHING (80.69%): register set matches (r26-r31); the frame is 0x10 smaller because the natural
// direction build leaves out the original's phase-1 temp, so the Vec slots sit below the original's.
// Using the best-normal copy (`nv.x/nv.z`) directly in the push-out dot/fma gained the last 0.12%.
HitEntry* partySearchWallFront(PartyEntry* party, Vec* pos, f32 a, f32 b) {
    Vec dir;
    Vec p;
    Vec hpos;
    Vec bestNorm;
    Vec base;
    Vec hnorm;
    Vec delta;
#ifdef MUST_MATCH
    // Match-only: the original builds the direction in a temporary Vec and then copies it into
    // `dir`; the natural form under #else assigns the fields in place.
    Vec dtmp2;
#endif
    f32 sin, cos;
    f32 sin2, cos2;
    f32 f31;
#ifdef MUST_MATCH
    // Match-only: `volatile` keeps the running best distance in memory so the compare is
    // reload-and-`fcmpo` like the original instead of living in f27.
    volatile f32 dist;
#else
    f32 dist;
#endif
    HitEntry* hit;
    HitEntry* best;
    f32 curDist;
    s32 i;


    delta = vec3_802cd368;
    sincosf(&sin, &cos, b);
    base.x = 0.75f * partyGetHeight(party);
    base.y = 0.5f * partyGetHeight(party);
    if ((party->flags & 0x30) != 0 || yoshiGetStatus() == 2) {
        base.z = 1.0f;
    } else {
        base.z = 11.0f;
    }
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4);
    dist = f31 + a;
    dir = vec3_802cd374;
    p = *pos;
    dir.x = sin;
    dir.z = cos;

    best = NULL;
    for (i = 0; i < 3; i++) {
        p.y = pos->y + ((f32*)&base)[i];
        curDist = f31 + a;
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
        Vec nv = bestNorm;
        f32 dx = sin * a;
        f32 dz = cos * a;
        f32 dot = dx * nv.x + dz * nv.z;
        delta.x += 0.5f * (dx - dot * nv.x);
        delta.z += 0.5f * (dz - dot * nv.z);
    }

    sincosf(&sin2, &cos2, 45.0f + b);
#ifdef MUST_MATCH
    dtmp2 = vec3_802cd380;
    dtmp2.x = sin2;
    dtmp2.z = cos2;
    dir = dtmp2;
#else
    dir = vec3_802cd380;
    dir.x = sin2;
    dir.z = cos2;
#endif
    for (i = 0; i < 3; i++) {
        p.y = pos->y + ((f32*)&base)[i];
        curDist = f31;
        hit = marioHitCheckVec(&p, &dir, NULL, NULL, &curDist);
        if (hit != NULL) {
            break;
        }
    }
    if (hit != NULL) {
        sincosf(&sin2, &cos2, b - 45.0f);
        dir = vec3_802cd38c;
        dir.x = sin2;
        dir.y = 0.0f;
        dir.z = cos2;
        for (i = 0; i < 3; i++) {
            p.y = pos->y + ((f32*)&base)[i];
            curDist = f31;
            hit = marioHitCheckVec(&p, &dir, NULL, NULL, &curDist);
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

// Rounds v to three decimal places.
#define ROUND_MILLI(v) \
    ((f32)(s32)(float_1000_8041b408 * (v) + ((v) >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) / \
     float_1000_8041b408)

// Caching 100*n.y, f32 hxyz[3], and giving each hitCheckVecFilter result its own variable (h2) that is copied
// into `hit` at the end of each arm reproduces the retail phi (raw r3 tested, copy only inside the arm).
// Keeping a second live `n2` copy of the hit normal (the original does the same) fixes the query offset
// (0x30) and the frame (0x150); using only `n` collapsed that slot and shifted the whole frame down 0x10.
// Hoisting query.hitPos.y and query.hitPos.z into locals around the first ROUND_MILLI (`hpy`/`hpz`,
// placed after the first rounding) took the function 87.17% -> 88.82% (87 -> 83 diffs).
// Remaining: FP register colouring (f24/f25/f26 rotated), the pooled i2f magic double (ours sits at
// rodata+0x128, the original's named object at +0xC0), and some statement scheduling.
// Both `return 0` exits are written as `goto fail` to one label so MWCC tail-merges them into the single
// shared `li r3, 0x0` block the original has (two separate `return 0;` statements make it inline one of them).
// NONMATCHING (89.49%): FP colouring + magic-double offset
u32 partyChkFrontStep(PartyEntry* party, f32 a, f32* out1, f32* out2, f32* out3) {
    f32 ny100;
    Vec n;
    Vec targetPos;
    Vec n2;
    HitCheckQuery query;
    HitEntry* hit;
    HitEntry* h2;
    f32 px, py, pz;
    f32 angle;
    f32 s, c;
    f32 rad;
    f32 off;
    f32 hxyz[3];
    f32 h;
    f32 f24;
    f32 hpy, hpz;

    f24 = a + float_0p01_8041b418;
    *(Vec*)party->field_C4 = vec3_802cd338;
    *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = float_0_8041b3ec;
    h = partyGetHeight(party);
    *out1 = party->position.y;
    *out2 = float_neg1_8041b414;
    if (party->speed != float_0_8041b3ec) {
        angle = party->dir;
    } else {
        angle = toMovedir(party->field_10C);
    }
    rad = (float_3p1416_8041b420 * revise360((f32)(s32)(float_0p5_8041b3f4 + angle))) / float_180_8041b424;
    off = float_0p265_8041b41c * *(f32*)((u8*)party + 0xF4);
    s = (f32)sin((f64)rad);
    c = (f32)cos((f64)rad);
    px = party->position.x;
    py = party->position.y;
    pz = party->position.z;
    targetPos = vec3_802cd344;
    {
        f32 dx = off * s;
        f32 dz = off * -c;
        targetPos.x = px + dx;
        targetPos.y = py + h;
        targetPos.z = pz + dz;
    }
    query.targetPos = targetPos;
    query.targetDir = vec3_802cd350;
    query.targetDistance = h + f24;

    if (yoshiGetStatus() != 0) {
        h2 = hitCheckVecFilter(&query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        h2 = hitCheckVecFilter(&query, chkfilterNokotaro);
    } else {
        h2 = hitCheckVecFilter(&query, chkfilter);
    }
    if (h2 == NULL) {
        hit = h2;
    } else {
        hxyz[0] = ROUND_MILLI(query.hitPos.x);
        hpy = query.hitPos.y;
        hpz = query.hitPos.z;
        hxyz[1] = ROUND_MILLI(hpy);
        hxyz[2] = ROUND_MILLI(hpz);
        n = query.hitNormal;
        n2 = n;
        ny100 = float_100_8041b3f8 * n2.y;
        hit = h2;
    }
    if (hit == NULL) {
        goto fail;
    }

    *(f32*)(party->field_C4 + 0x00) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * n2.z,
                                                     ny100) - float_180_8041b424);
    *(f32*)((u8*)party + 0xC8) = float_0_8041b3ec;
    *(f32*)(party->field_C4 + 0x08) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * n2.x,
                                                     ny100) - float_180_8041b424);
    *(f32*)(party->field_C4 + (0xE0 - 0xC4)) =
        angleABf(float_0_8041b3ec, float_0_8041b3ec, sqrtf(n2.x * n2.x + n2.z * n2.z), -n2.y);
    if (hxyz[1] < py && __fabsf(hxyz[1] - py) >= float_2_8041b428) {
        *out2 = py - hxyz[1];
        *out3 = angleABf(px, pz, hxyz[0], hxyz[2]);
        goto fail;
    }
    *out1 = hxyz[1];
    *out2 = float_0_8041b3ec;
    *out3 = angleABf(px, pz, n.x, n.z);
    return (u32)hit;
fail:
    return 0;
}

// The algorithm and stack frame match exactly (0x1b0 frame, all 14 callee-saved floats f18-f31). Giving the
// first hitCheckVecFilter result its own variable (h2) and copying it into `hit` after the test removes the
// r27/r3 live-range split. Loading px before the offCos/targetDist statements fixes the px/pz load order.
// Splitting the f32 declarations and reordering them (hitY early, bestY/targetDist/offSin moved) cut the
// diffs to 44. The rest is FP register colouring: original has py=f21, targetDist=f26, offCos=f23,
// noCos=f28, off=f22 while ours has py=f28 (f30 with some declaration orders), targetDist=f21,
// offCos=f26, noCos=f23, off=f21.
// NONMATCHING (98.80%): FP register colouring
HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down) {
    HitCheckQuery query;
    Vec targetPos;
    Vec dir;
    Vec n;
    Vec tmpA;
    Vec tmpB;
    HitEntry* hit;
    HitEntry* best;
    f32 dz;
    f32 off;
    f32 hitY;
    f32 rad;
    f32 s;
    f32 c;
    f32 py;
    f32 noCos;
    f32 noSin;
    f32 offCos;
    f32 y;
    f32 pz;
    f32 bestY;
    f32 targetDist;
    f32 offSin;
    HitEntry* h2;
    f32 h;
    f32 dx;
    f32 px;
    s32 i;

    if (party->flags & 0x02000000) {
        return NULL;
    }
#ifdef MUST_MATCH
    // Match-only: the two-statement form makes MWCC issue the abs into off's own register
    // (`fabs f21,f1; fadds f21,f21,f0`) like the original, instead of a throwaway `fabs f0,f1`.
    off = __fabsf(up);
    off = off + float_0p1_8041b42c;
#else
    off = __fabsf(up) + float_0p1_8041b42c;
#endif
    *(Vec*)party->field_C4 = vec3_802cd320;
    *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = float_0_8041b3ec;
    if (marioGetPtr()->currMotionId == MARIO_MOTION_YOSHI) {
        h = float_5_8041b430;
    } else {
        h = float_0p75_8041b400 * partyGetHeight(party);
    }
    party->field_E4 = party->position.y;
    *(f32*)party->field_E8 = float_neg1_8041b414;
    if (party->speed != float_0_8041b3ec) {
        rad = party->dir;
    } else {
        rad = partyToMovedir(party, party->field_10C);
    }
    rad = (float_3p1416_8041b420 * rad) / float_180_8041b424;
    s = (f32)sin((f64)rad);
    offSin = float_0p375_8041b434 * (party->field_F4 * s);
    c = (f32)cos((f64)rad);
    py = party->position.y;
    noSin = -offSin;
    y = py + h;
    px = party->position.x;
    offCos = float_0p375_8041b434 * (party->field_F4 * -c);
    targetDist = h + off;
    pz = party->position.z;
    noCos = -offCos;
    dir = vec3_802cd32c;
    best = NULL;
    bestY = float_neg10000_8041b438;
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

        if (yoshiGetStatus() != 0) {
            h2 = hitCheckVecFilter(&query, chkfilterYoshi);
        } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
            h2 = hitCheckVecFilter(&query, chkfilterNokotaro);
        } else {
            h2 = hitCheckVecFilter(&query, chkfilter);
        }
        if (h2 != NULL) {
            hitY = ROUND_MILLI(query.hitPos.y);
            n.x = query.hitNormal.x;
            n.y = query.hitNormal.y;
            n.z = query.hitNormal.z;
        }
        hit = h2;
        if (hit != NULL) {
            tmpA = n;
            *(f32*)(party->field_C4 + 0x00) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec,
                                                             float_100_8041b3f8 * tmpA.z,
                                                             float_100_8041b3f8 * tmpA.y) - float_180_8041b424);
            *(f32*)(party->field_C4 + 0x04) = float_0_8041b3ec;
            *(f32*)(party->field_C4 + 0x08) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec,
                                                             float_100_8041b3f8 * tmpA.x,
                                                             float_100_8041b3f8 * tmpA.y) - float_180_8041b424);
            if (partyNrmToAngle(&tmpA) >= float_50_8041b43c) {
                hit = NULL;
            }
        }
        if (hit != NULL) {
            tmpB = n;
            *(f32*)(party->field_C4 + 0x00) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec,
                                                             float_100_8041b3f8 * tmpB.z,
                                                             float_100_8041b3f8 * tmpB.y) - float_180_8041b424);
            *(f32*)(party->field_C4 + 0x04) = float_0_8041b3ec;
            *(f32*)(party->field_C4 + 0x08) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec,
                                                             float_100_8041b3f8 * tmpB.x,
                                                             float_100_8041b3f8 * tmpB.y) - float_180_8041b424);
            *(f32*)(party->field_C4 + (0xE0 - 0xC4)) = partyNrmToAngle(&tmpB);
            if (hitY < py && __fabsf(hitY - py) >= float_2_8041b428) {
                if (__fabsf(hitY - py) > __fabsf(down)) {
                    *(f32*)party->field_E8 = __fabsf(py - hitY);
                } else {
                    if (hitY > bestY) {
                        bestY = hitY;
                        best = hit;
                    }
                }
            } else if (__fabsf(hitY - py) <= float_11_8041b40c) {
                if (hitY > bestY) {
                    bestY = hitY;
                    best = hit;
                }
            }
        }
    }
    if (best != NULL) {
        party->field_E4 = bestY;
        *(f32*)party->field_E8 = float_0_8041b3ec;
        return best;
    }
    return NULL;
}

void partyChkGnd(PartyEntry* party) {
    s32 attr;
    HitEntry* hit;
    f32 a, b, c;

    if (party->flags & 0x02000000) {
        return;
    }
    attr = 0;
    hit = partySearchGround(party, float_11_8041b40c, party->jumpPara1);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
        }
    }
    party->field_138 = hit;
    party->flags = (party->flags & ~0x80);
    if (hit == NULL) {
        party->flags = (party->flags & ~0x2);
        partyChgMot(party, 3);
        party->jumpPara3 = float_neg1_8041b414;
    } else {
        party->flags = (party->flags | 0x2);
        if (attr & 0x100) {
            party->flags = (party->flags | 0x80);
        } else {
            party->flags = (party->flags & ~0x80);
        }
    }
    party->position.y = party->field_E4;
    if (party->flags & 0x30) {
        return;
    }
    partyChkFrontStep(party, float_11_8041b40c, &a, &b, &c);
    if (a - party->position.y >= float_5_8041b430 && *(f32*)((u8*)party + 0xE0) <= float_30_8041b440) {
        *(f32*)party->field_120 = a;
        if (party->speed != float_0_8041b3ec) {
            partyChgMot(party, 6);
        }
    }
}

#pragma opt_common_subs off // defers the rodata base (the two double constants) into the sqrtf body like the original
// NONMATCHING (84.11%): the inline-sqrtf body and the two fmuls/fadds shape match once the
// multiply contraction is suppressed, but this source contracts x*x into an fmadds. Every
// two-temp form tried that keeps the two fmuls moves the sum to f4 (f3 is reserved for the
// angleABf argument) and the pooled 0.5 double to f3; the target also schedules the lis/addi
// of the rodata base after the two position loads and the LR store after the muls.
f32 partyNrmToAngle(Vec* v) {
    f32 len = sqrtf(v->x * v->x + v->z * v->z);

    return angleABf(float_0_8041b3ec, float_0_8041b3ec, len, -v->y);
}
#pragma opt_common_subs reset

HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* hitPos, Vec* hitNormal, f32* dist) {
    Vec dirCopy = *dir;
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *pos;
    query.targetDir = dirCopy;
    query.targetDistance = *dist;

    if (yoshiGetStatus() != 0) {
        hit = hitCheckVecFilter(&query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit = hitCheckVecFilter(&query, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query, chkfilter);
    }

    if (hit == NULL) {
        return hit;
    }

    *dist = (f32)(s32)(float_1000_8041b408 * query.targetDistance +
                       (query.targetDistance >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
            float_1000_8041b408;
    hitPos->x = (f32)(s32)(float_1000_8041b408 * query.hitPos.x +
                           (query.hitPos.x >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
                float_1000_8041b408;
    hitPos->y = (f32)(s32)(float_1000_8041b408 * query.hitPos.y +
                           (query.hitPos.y >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
                float_1000_8041b408;
    hitPos->z = (f32)(s32)(float_1000_8041b408 * query.hitPos.z +
                           (query.hitPos.z >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
                float_1000_8041b408;
    hitNormal->x = query.hitNormal.x;
    hitNormal->y = query.hitNormal.y;
    hitNormal->z = query.hitNormal.z;
    return hit;
}

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#else
#define RO(base, off, sym) ((const char*)&(sym))
#endif


BOOL chkfilterNokotaro(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;
    const u8* base = (const u8*)&vec3_802cd320;

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
    if (strcmp(obj->animName, RO(base, 0xE0, str_802CD400)) == 0 ||
        strcmp(obj->animName, RO(base, 0xF8, str_802CD418)) == 0 ||
        strcmp(obj->animName, RO(base, 0x110, str_802CD430)) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterYoshi(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;
    const u8* base = (const u8*)&vec3_802cd320;

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
    if (strcmp(obj->animName, RO(base, 0xE0, str_802CD400)) == 0 ||
        strcmp(obj->animName, RO(base, 0xF8, str_802CD418)) == 0 ||
        strcmp(obj->animName, RO(base, 0x110, str_802CD430)) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilter(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;
    const u8* base = (const u8*)&vec3_802cd320;

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
    if (strcmp(obj->animName, RO(base, 0xE0, str_802CD400)) == 0 ||
        strcmp(obj->animName, RO(base, 0xF8, str_802CD418)) == 0 ||
        strcmp(obj->animName, RO(base, 0x110, str_802CD430)) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

// Definitions for the .rodata constants declared above (see the comment there).
const f64 double_to_int_802cd3e0 = 4503601774854144.0;
const f64 double_0p5_802cd3e8 = 0.5;
const f64 double_3_802cd3f0 = 3.0;
const f64 double_0_802cd3f8 = 0.0;
const char str_802CD400[0x18] = "MOBJ_HiddenHatenaBlock";
const char str_802CD418[0x18] = "MOBJ_HiddenBadgeBlock";
const char str_802CD430[0x18] = "MOBJ_Hidden10CountBlock";
