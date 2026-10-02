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

// NONMATCHING (70.07%): the volatile pvx/pvy/pvz locals now spill position->x/y/z to stack like the
// retail code, but the slots land at 0x08-0x10 rather than the original's 0x78-0x80 (the retail
// compiler puts the sin/cos and position scalars above the Vecs, ours puts them below).
HitEntry* partySearchHead(PartyEntry* party, Vec* position, f32* out, f32 angle) {
    HitEntry* hit;
    Vec q;
    Vec p1, p2, p3, p4;
    f32 sin, cos;
    f32 h;
    f32 offSin, offCos;
    f32 py;
#ifdef MUST_MATCH
    // Match-only: forcing the three position scalars through volatile locals reproduces the
    // retail spill of position->x/y/z to stack (the plain Vec copy lands in the wrong slot).
    volatile f32 pvx, pvy, pvz;
#endif
    f32 d1, d2, d3, d4;
    Vec nor1, pos1, nor2, pos2, nor3, pos3, nor4, pos4;
    f32 x, z;
    f32 dist;


    h = 0.3f * *(f32*)((u8*)party + 0xF4);
    sincosf(&sin, &cos, angle);
    offSin = h * sin;
    offCos = -h * cos;
#ifdef MUST_MATCH
    pvx = position->x;
    pvy = position->y;
    pvz = position->z;
    py = pvy;
    dist = *out;
    x = pvx + offSin;
    z = pvz + offCos;
#else
    py = position->y;
    dist = *out;
    x = position->x + offSin;
    z = position->z + offCos;
#endif
    p1 = vec3_802cd3b0;
    p1.x = x;
    p1.y = py;
    p1.z = z;
    q = p1;
    d1 = dist;
    hit = hitCheckFilter(NULL, &pos1.x, &pos1.y, &pos1.z, &d1, &nor1.x, &nor1.y, &nor1.z, q.x, q.y + 10.0f,
                         q.z, 0.0f, 1.0f, 0.0f);
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
        p2.x = x;
        p2.y = py;
        p2.z = z;
        q = p2;
        d2 = dist;
        hit = hitCheckFilter(NULL, &pos2.x, &pos2.y, &pos2.z, &d2, &nor2.x, &nor2.y, &nor2.z, q.x, q.y + 10.0f,
                             q.z, 0.0f, 1.0f, 0.0f);
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
        p3.x = x;
        p3.y = py;
        p3.z = z;
        q = p3;
        d3 = dist;
        hit = hitCheckFilter(NULL, &pos3.x, &pos3.y, &pos3.z, &d3, &nor3.x, &nor3.y, &nor3.z, q.x, q.y + 10.0f,
                             q.z, 0.0f, 1.0f, 0.0f);
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
        p4.x = x;
        p4.y = py;
        p4.z = z;
        q = p4;
        d4 = dist;
        hit = hitCheckFilter(NULL, &pos4.x, &pos4.y, &pos4.z, &d4, &nor4.x, &nor4.y, &nor4.z, q.x, q.y + 10.0f,
                             q.z, 0.0f, 1.0f, 0.0f);
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

// NONMATCHING (96.91%): frame/query offsets/scheduling mostly match; wins were reusing `d` for the second
// query, plain `if (hit != NULL)` both blocks, and `partyNrmToAngle(&n)`. Left: callee-saved GPRs
// (orig party=r30/pos=r31/vec3=r29/hit2=r28), an f4/f6 swap in the push-outs, rodata doubles, if-layout.
HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir) {
    HitCheckQuery query1;
    HitCheckQuery query2;
    HitEntry* hit;
    HitEntry* hit2;
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
        hit = hitCheckVecFilter(&query1, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit = hitCheckVecFilter(&query1, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query1, chkfilter);
    }

    if (hit != NULL) {
        f31 = (f32)(s32)(1000.0f * query1.targetDistance +
                         (query1.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
              1000.0f;
        n.x = query1.hitNormal.x;
        n.y = query1.hitNormal.y;
        n.z = query1.hitNormal.z;
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
    p.y += h;
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4) + dist;
    query2.targetPos = p;
    query2.targetDir = d;
    query2.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        hit2 = hitCheckVecFilter(&query2, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit2 = hitCheckVecFilter(&query2, chkfilterNokotaro);
    } else {
        hit2 = hitCheckVecFilter(&query2, chkfilter);
    }

    if (hit2 != NULL) {
        f31 = (f32)(s32)(1000.0f * query2.targetDistance +
                         (query2.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
              1000.0f;
        n.x = query2.hitNormal.x;
        n.y = query2.hitNormal.y;
        n.z = query2.hitNormal.z;
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

// NONMATCHING (80.52%): register set matches (r26-r31, no extra GPR); the frame is 0x10 smaller
// because the natural direction build leaves out the original's phase-1 temp, so the Vec slots
// sit below the original's.
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
    f32 f31;
#ifdef MUST_MATCH
    // Match-only: `volatile` keeps the running best distance in memory so the compare is
    // reload-and-`fcmpo` like the original instead of living in f27.
    volatile f32 dist;
#else
    f32 dist;
#endif
    f32 curDist;
    f32 sin2, cos2;
    HitEntry* best;
    HitEntry* hit;
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
        f32 nx = nv.x;
        f32 nz = nv.z;
        f32 dz = cos * a;
        f32 dx = sin * a;
        f32 dot = dx * nx + dz * nz;
        delta.x += 0.5f * (dx - dot * nx);
        delta.z += 0.5f * (dz - dot * nz);
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

// NONMATCHING (85.45%): caching 100*n.y in a local makes the FP saves f23-f31 match the original, and a
// f32 hxyz[3] for the rounded hitPos steers the ROUND registers. Still off: the compiler forwards the
// hitNormal copy into query.hitNormal (query 0x24 vs 0x30, frame 0x160 vs 0x150) and the doubles pool locally.
u32 partyChkFrontStep(PartyEntry* party, f32 a, f32* out1, f32* out2, f32* out3) {
    f32 ny100;
    Vec targetPos;
    Vec n;
    HitCheckQuery query;
    HitEntry* hit;
    f32 h;
    f32 f24;
    f32 px, py, pz;
    f32 angle;
    f32 s, c;
    f32 off;
    f32 rad;
    f32 hxyz[3];

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
        hit = hitCheckVecFilter(&query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit = hitCheckVecFilter(&query, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query, chkfilter);
    }
    if (hit == NULL) {
        return 0;
    }
    hxyz[0] = ROUND_MILLI(query.hitPos.x);
    hxyz[1] = ROUND_MILLI(query.hitPos.y);
    hxyz[2] = ROUND_MILLI(query.hitPos.z);
    n = query.hitNormal;
    ny100 = float_100_8041b3f8 * n.y;

    *(f32*)((u8*)party + 0xC4) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * n.z,
                                                     ny100) - float_180_8041b424);
    *(f32*)((u8*)party + 0xC8) = float_0_8041b3ec;
    *(f32*)((u8*)party + 0xCC) = (f32)(s32)(angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * n.x,
                                                     ny100) - float_180_8041b424);
    *(f32*)(party->field_C4 + (0xE0 - 0xC4)) =
        angleABf(float_0_8041b3ec, float_0_8041b3ec, sqrtf(n.x * n.x + n.z * n.z), -n.y);
    if (hxyz[1] < py && __fabsf(hxyz[1] - py) >= float_2_8041b428) {
        *out2 = py - hxyz[1];
        *out3 = angleABf(px, pz, hxyz[0], hxyz[2]);
        return 0;
    }
    *out1 = hxyz[1];
    *out2 = float_0_8041b3ec;
    *out3 = angleABf(px, pz, n.x, n.z);
    return (u32)hit;
}

// The algorithm and stack frame match exactly (0x1b0 frame, all 14 callee-saved floats f18-f31). The
// remaining diffs are FP register colouring (67 arg_mismatch) and a `hit` r27/r3 live-range split; the
// `hitY = hitY;` copy in the `hit == NULL` arm is a dead-path materialisation (compiler-input only).
// NONMATCHING (97.91%): FP colouring + `hit` r27/r3 split
HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down) {
    HitCheckQuery query;
    Vec targetPos;
    Vec dir;
    Vec n;
    Vec tmpA;
    Vec tmpB;
    HitEntry* hit;
    HitEntry* best;
    f32 h;
    f32 off;
    f32 rad;
    f32 s, c;
    f32 noSin, noCos;
    f32 offSin, offCos;
    f32 y;
    f32 px, py, pz;
    f32 targetDist;
    f32 hitY;
    f32 bestY;
    f32 dz;
    f32 dx;
    s32 i;

    if (party->flags & 0x02000000) {
        return NULL;
    }
    off = __fabsf(up) + float_0p1_8041b42c;
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
    offCos = float_0p375_8041b434 * (party->field_F4 * -c);
    targetDist = h + off;
    pz = party->position.z;
    px = party->position.x;
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
            hit = hitCheckVecFilter(&query, chkfilterYoshi);
        } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
            hit = hitCheckVecFilter(&query, chkfilterNokotaro);
        } else {
            hit = hitCheckVecFilter(&query, chkfilter);
        }
        if (hit == NULL) {
            hitY = hitY;
        } else {
            hitY = ROUND_MILLI(query.hitPos.y);
            n.x = query.hitNormal.x;
            n.y = query.hitNormal.y;
            n.z = query.hitNormal.z;
        }
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

// NONMATCHING (83.56%): the inlined sqrtf doubles are pooled in a separate
// .rodata subsection rather than following vec3_802cd320 at 0xc8/0xd0/0xd8.
f32 partyNrmToAngle(Vec* v) {
    f32 len = sqrtf(v->x * v->x + v->z * v->z);

    return angleABf(float_0_8041b3ec, float_0_8041b3ec, len, -v->y);
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
