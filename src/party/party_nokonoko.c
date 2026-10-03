#include "party.h"
#include "party/party_nokonoko.h"
#include "party/party_move.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_hit.h"
#include "mario/mariost.h"
#include "driver/mobjdrv.h"
#include "eff/n64/eff_small_star_n64.h"
#include "eff/eff_kemutest.h"
#include "party/party_bomhei.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "driver/hitdrv.h"
#include "driver/itemdrv.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>
#include "math_ext.h"
#include "system.h"
#include "matching.h"

// nokonoko_use sound effect handle (sbss).
u32 nokoSe;

static const f32 float_37 = 37.0f;
static const f32 float_24 = 24.0f;

static const f32 float_3p1416 = 3.1415927f;
static const f32 float_180 = 180.0f;
static const f32 float_0p1 = 0.1f;
static const f32 float_0 = 0.0f;
static const f32 float_neg1 = -1.0f;

const Vec vec3_802ce138 = {0.0f, 0.0f, 0.0f};
const double double_to_int_802ce148 = 4503601774854144.0;
const char str_802CE150[] = "PNK_K_2";
const char str_802CE158[] = "PNK_A_1";
const double double_180_802ce160 = 180.0;
#ifdef MUST_MATCH
// MUST_MATCH: nokonoko_use reads the pose names (str_802CE150/158) as offsets from this first .rodata object.
#define NK_STR(off, lit) ((char*)vec + (off))
#else
#define NK_STR(off, lit) (lit)
#endif
#define NKF_180 double_180_802ce160

static const f32 float_1p5708 = 1.5707964f;
static const f32 float_4p7124 = 4.712389f;
static const f32 float_90 = 90.0f;
static const f32 float_0p5 = 0.5f;
static const f32 float_10 = 10.0f;
static const f32 float_30 = 30.0f;
static const f32 float_100 = 100.0f;
static const f32 float_11 = 11.0f;
static const f32 float_6 = 6.0f;
static const f32 float_1 = 1.0f;
static const f32 float_0p4 = 0.4f;

#ifdef MUST_MATCH
// MUST_MATCH: inlined add helper; the call boundary steers MWCC's FP register colouring in the probes.
static inline float fadd32(f32 arg0, f32 arg1);
#endif

void lookupSafetyPosSub(Vec* pos, Vec* out, f32 height, f32 angle) {
    Vec sp;
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    Vec* p;
    MarioWork* mario = marioGetPtr();
    f32 rad;
    f32 x;
    f32 y;
    f32 z;
    f32 radius;
    f32 s;
    f32 c;

    radius = mario->wPlayerCollisionBox.x;
    rad = float_3p1416 * angle / float_180;
    s = sin(rad);
    x = radius * s + pos->x;
    c = cos(rad);
    y = pos->y + height;
    z = pos->z + radius * -c;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        sp = vec3_802ce138;
        sp.x = x;
        sp.y = y;
        sp.z = z;
        p = &sp;
    } else {
        p = pos;
    }
    *out = *p;
}

// NONMATCHING (95.03%): probe values and control flow match; z/baseY/distance land like the original. The
// remaining diffs are the sin result s (f27 here, f29 in the original), the float_0/pos->z schedule and the
// tail word copy. Wrapping the x add in an inlined helper (fadd32) steered the baseY/distance colouring.
void lookupSafetyPos3(Vec* pos, Vec* out, f32 height) {
    f32 s, c, distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 x, y, z, radius, baseY;
    f32 t;

    radius = marioGetPtr()->wPlayerCollisionBox.x;
    s = sin(MATCH_F32(float_1p5708));
    c = cos(MATCH_F32(float_1p5708));
    baseY = pos->y;
    s = radius * s;
#ifdef MUST_MATCH
    s = fadd32(s, pos->x);  // MUST_MATCH: inlined helper steers MWCC's FP colouring (see top of file).
#else
    s = s + pos->x;
#endif
    y = pos->y + height;
    t = radius * -c;
    z = pos->z + t;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, s, y, z, float_0, float_neg1, float_0)) {
        out->x = s;
        out->z = z;
        out->y = baseY;
    } else {
        s = sin(MATCH_F32(float_4p7124));
        c = cos(MATCH_F32(float_4p7124));
        baseY = pos->y;
        s = radius * s;
#ifdef MUST_MATCH
        s = fadd32(s, pos->x);  // MUST_MATCH: inlined helper steers MWCC's FP colouring (see top of file).
#else
        s = s + pos->x;
#endif
        y = pos->y + height;
        t = radius * -c;
        z = pos->z + t;
        distance = float_0p1 + height;
        if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, s, y, z, float_0, float_neg1, float_0)) {
            out->x = s;
            out->z = z;
            out->y = baseY;
        } else {
#ifdef MUST_MATCH
            // MUST_MATCH: the natural *out = *pos lets MWCC fold all three words through one r0
            // scratch; volatile word accesses keep the loads/stores as written and recover one instruction
            // of the original's r0/r3 tail. Same three words either way.
            {
                volatile u32* sp = (volatile u32*)pos;
                volatile u32* dp = (volatile u32*)out;
                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
            }
#else
            *out = *pos;
#endif
        }
    }
}

// The four probes, the selection logic and the register mask (r4) match.  What is left is
// scheduler-only: blocks 2-4 load float_0 after pos->z (the original loads float_0 first), and
// the hit0&&hit1 branch reads v1.y instead of the original's pos->y.
// NONMATCHING (98.97%)
s32 lookupSafetyPos(Vec* pos, Vec* out, f32 height) {
    s32 result;
    Vec v1;
    Vec v2;
    Vec v3;
    Vec v4;
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    MarioWork* mario;
    f32 radius;
    f32 angle;
    f32 s;
    f32 c;
    f32 x;
    f32 y;
    f32 z;
    HitEntry* hit0;
    HitEntry* hit1;
    HitEntry* hit2;
    HitEntry* hit3;

    mario = marioGetPtr();
    radius = mario->wPlayerCollisionBox.x;

    angle = revise360(toMovedir(*(f32*)mario->unk1AC) - float_180);
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos->x;
    v1.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v1.y = pos->y;
    v1.z = z;
    distance = float_0p1 + height;
    hit0 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    angle = revise360(toMovedir(*(f32*)mario->unk1AC) - float_90);
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos->x;
    v2.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v2.y = pos->y;
    v2.z = z;
    distance = float_0p1 + height;
    hit1 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    angle = revise360(toMovedir(*(f32*)mario->unk1AC) + float_90);
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos->x;
    v3.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v3.y = pos->y;
    v3.z = z;
    distance = float_0p1 + height;
    hit2 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    angle = revise360(toMovedir(*(f32*)mario->unk1AC));
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos->x;
    v4.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v4.y = pos->y;
    v4.z = z;
    distance = float_0p1 + height;
    hit3 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    result = 0;
    if (hit2) {
        result |= 4;
    }
    if (hit3) {
        result |= 8;
    }
    if (hit0) {
        result |= 1;
    }
    if (hit1) {
        result |= 2;
    }

    if (hit0) {
        if (hit1 && hit2 && hit3) {
            out->x = (v1.x + v3.x) * float_0p5;
            out->z = (v1.z + v3.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else if (hit1) {
            out->x = (v1.x + v2.x) * float_0p5;
            out->z = (v1.z + v2.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else if (hit2) {
            out->x = (v1.x + v3.x) * float_0p5;
            out->z = (v1.z + v3.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else if (hit3) {
            out->x = (v1.x + v4.x) * float_0p5;
            out->z = (v1.z + v4.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else {
            *out = v1;
        }
    } else {
        if (hit3) {
            if (hit1) {
                out->x = (v4.x + v2.x) * float_0p5;
                out->z = (v4.z + v2.z) * float_0p5;
                out->y = v4.y;
                return result;
            } else if (hit2) {
                out->x = (v3.x + v2.x) * float_0p5;
                out->z = (v3.z + v2.z) * float_0p5;
                out->y = v3.y;
                return result;
            } else {
                *out = v4;
            }
        } else {
            if (hit1) {
                *out = v2;
                return result;
            }
            if (hit2) {
                *out = v3;
                return result;
            }
        }
    }
#ifdef MUST_MATCH
    // MUST_MATCH: the natural `*out = *pos;` makes the compiler keep `result` in r5 and reorder the
    // three loads/stores; volatile word accesses pin the original's load/load/store/load/store/store
    // order and its r0/r3/r4 scratch registers.  Same three words either way.
    {
        volatile u32* s = (volatile u32*)pos;
        volatile u32* d = (volatile u32*)out;
        u32 a, b, c;
        a = s[0];
        b = s[1];
        d[0] = a;
        c = s[2];
        d[1] = b;
        d[2] = c;
    }
#else
    *out = *pos;
#endif
    return result;
}

// NONMATCHING (95.83%): early-return four probes; calls, dead pi sin/cos, baseY cache and constants match.
// Only register allocation differs: the original puts height/radius/distance in f29/f30/f31 (sin reuses
// f31), ours uses f30/f31/f29 (f26 for sin), shifting the float_0 load and the final pos copy.
void lookupSafetyPos2(Vec* pos, Vec* out, f32 height) {
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 x;
    f32 radius;
    f32 s;
    f32 c;
    f32 y;
    f32 z;
    f32 baseY;
    f32 t;
    f32 u;

    radius = marioGetPtr()->wPlayerCollisionBox.x;

    s = sin(MATCH_F32(float_1p5708));
    c = cos(MATCH_F32(float_1p5708));
    baseY = pos->y;
    t = radius * s;
#ifdef MUST_MATCH
    x = fadd32(pos->x, t);  // MUST_MATCH: inlined helper steers MWCC's FP colouring (see top of file).
#else
    x = pos->x + t;
#endif
    y = baseY + height;
    u = radius * -c;
    z = pos->z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }

    s = sin(MATCH_F32(float_4p7124));
    c = cos(MATCH_F32(float_4p7124));
    baseY = pos->y;
    t = radius * s;
#ifdef MUST_MATCH
    x = fadd32(pos->x, t);  // MUST_MATCH: inlined helper steers MWCC's FP colouring (see top of file).
#else
    x = pos->x + t;
#endif
    y = baseY + height;
    u = radius * -c;
    z = pos->z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }

    s = sin(MATCH_F32(float_3p1416));
    c = cos(MATCH_F32(float_3p1416));
    x = pos->x;
    z = pos->z;
    baseY = pos->y;
    y = baseY + height;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }

    s = sin(MATCH_F32(float_0));
    c = cos(MATCH_F32(float_0));
    baseY = pos->y;
    t = radius * s;
#ifdef MUST_MATCH
    x = fadd32(pos->x, t);  // MUST_MATCH: inlined helper steers MWCC's FP colouring (see top of file).
#else
    x = pos->x + t;
#endif
    y = baseY + height;
    u = radius * -c;
    z = pos->z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }
#ifdef MUST_MATCH
    // Word-wise copy keeps the original's r0/r3 scratch order.
    *(u32*)&out->x = *(u32*)&pos->x;
    *(u32*)&out->y = *(u32*)&pos->y;
    *(u32*)&out->z = *(u32*)&pos->z;
#else
    *out = *pos;
#endif
}

static inline float fsub32(f32 arg0, f32 arg1)
{
  return arg0 - arg1;
}

#ifdef MUST_MATCH
// Same-byte match for the original's fadds f0, f5, f0 operand order in the first probe.
static inline float fadd32(f32 arg0, f32 arg1)
{
  return arg0 + arg1;
}
#endif

// Four floor/wall probes: straight ahead, then dir-90, dir+90 and the f31 sweeps.
HitEntry* ccwall(PartyEntry* party, s32 flag) {
    u8* p = (u8*)party;
    HitEntry* hit;
    f32 sinV, cosV;
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 sin2, cos2;
    f32 sin3, cos3;
    f32 sin4, cos4;
    f32 y;
    f32 f31;
    f32 ssin, scos;
    f32 spd;
    f32 dot;
    f32 oy;
    f32 x, z;

    y = float_10;

    x = party->position.x;
    z = party->position.z;
    oy = party->position.y + *(f32*)(p + 0xF0) + y;
#ifdef MUST_MATCH
    distance = fadd32(*(f32*)(p + 0xF0), float_30);
#else
    distance = *(f32*)(p + 0xF0) + float_30;
#endif
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, oy,
                         z, float_0, float_neg1, float_0);
    if (((hitGetAttr(hit) >> 9) & 1) == 1) {
        hit = NULL;
    }
    *(HitEntry**)((u8*)party + 0x138) = hit;
    if (hit != NULL) {
        f32 py;
        hitY = (f32)(s32)(float_100 * hitY + float_0p5) / float_100;
        py = (f32)(s32)(float_100 * party->position.y + float_0p5) / float_100;
        if (__fabsf(py - hitY) <= float_11) {
            if (py <= hitY) {
                if (flag == 0) {
                    party->position.y = hitY;
                    if (__fabsf(py - hitY) >= float_1) {
                        *(s16*)(p + 0x17E) |= 0x2;
                    }
                }
            } else {
                if ((*(s16*)(p + 0x17E) & 0x2) != 0 && flag == 1) {
                    party->position.y = hitY;
                }
                if (__fabsf(py - hitY) <= float_6) {
                    party->position.y = hitY;
                }
            }
        }
    }

    *(HitEntry**)((u8*)party + 0x130) = NULL;
    *(HitEntry**)((u8*)party + 0x12C) = NULL;
    y += float_1;

    x = party->position.x;
    z = party->position.z;
    oy = party->position.y + y;
    distance = float_0p5 * *(f32*)(p + 0xF4) + party->speed;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, oy, z, sinV, float_0, cosV);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin2, &cos2, party->dir + float_180);
        party->position.x = hitX + float_0p5 * (sin2 * *(f32*)(p + 0xF4));
        party->position.z = hitZ + float_0p5 * (cos2 * *(f32*)(p + 0xF4));
        spd = party->speed;
        ssin = sinV * spd;
        scos = cosV * spd;
        dot = ssin * hitNX + scos * hitNZ;
        party->position.x += float_0p5 * (ssin - dot * hitNX);
        party->position.z += float_0p5 * (scos - dot * hitNZ);
        if (*(s16*)(p + 0x17C) != 0 || flag == 0) {
            *(HitEntry**)((u8*)party + 0x130) = hit;
        }
        *(HitEntry**)((u8*)party + 0x12C) = hit;
        return hit;
    }

    f31 = float_0p4 * *(f32*)(p + 0xF4);
    z = party->position.z + f31 * (f32)cos((f64)(float_3p1416 * (party->dir - float_90) / float_180));
    x = party->position.x + f31 * (f32)sin((f64)(float_3p1416 * (party->dir - float_90) / float_180));
    oy = party->position.y + y;
    distance = float_0p4 * *(f32*)(p + 0xF4) + party->speed;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, oy, z, sinV, float_0, cosV);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin3, &cos3, party->dir + float_180);
        party->position.x = hitX + float_0p5 * (sin3 * *(f32*)(p + 0xF4));
        party->position.z = hitZ + float_0p5 * (cos3 * *(f32*)(p + 0xF4));
        party->position.z -= f31 * (f32)cos((f64)(float_3p1416 * (party->dir - float_90) / float_180));
        party->position.x -= f31 * (f32)sin((f64)(float_3p1416 * (party->dir - float_90) / float_180));
        spd = party->speed;
        ssin = sinV * spd;
        scos = cosV * spd;
        dot = ssin * hitNX + scos * hitNZ;
        party->position.x += float_0p5 * (ssin - dot * hitNX);
        party->position.z += float_0p5 * (scos - dot * hitNZ);
        if (*(s16*)(p + 0x17C) != 0 || flag == 0) {
            *(HitEntry**)((u8*)party + 0x130) = hit;
        }
        *(HitEntry**)((u8*)party + 0x12C) = hit;
        party->position.x = hitX;
        return hit;
    }

    f31 = float_0p4 * *(f32*)(p + 0xF4);
    z = party->position.z + f31 * (f32)cos((f64)(float_3p1416 * (party->dir + float_90) / float_180));
    x = party->position.x + f31 * (f32)sin((f64)(float_3p1416 * (party->dir + float_90) / float_180));
    oy = party->position.y + y;
#ifdef MUST_MATCH
    // The original evaluates and stores this probe's distance twice; the first is dead.
    distance = float_0p4 * *(f32*)(p + 0xF4) + party->speed;
    distance = float_0p4 * *(f32*)(p + 0xF4) + party->speed;
#else
    distance = float_0p4 * *(f32*)(p + 0xF4) + party->speed;
#endif
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, oy, z, sinV, float_0, cosV);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin4, &cos4, party->dir + float_180);
        party->position.x = hitX + float_0p5 * (sin4 * *(f32*)(p + 0xF4));
        party->position.z = hitZ + float_0p5 * (cos4 * *(f32*)(p + 0xF4));
        party->position.z -= f31 * (f32)cos((f64)(float_3p1416 * (party->dir + float_90) / float_180));
        party->position.x -= f31 * (f32)sin((f64)(float_3p1416 * (party->dir + float_90) / float_180));
        spd = party->speed;
        ssin = sinV * spd;
        scos = cosV * spd;
        dot = ssin * hitNX + scos * hitNZ;
        party->position.x += float_0p5 * (ssin - dot * hitNX);
        party->position.z += float_0p5 * (scos - dot * hitNZ);
        if (*(s16*)(p + 0x17C) != 0 || flag == 0) {
            *(HitEntry**)((u8*)party + 0x130) = hit;
        }
        *(HitEntry**)((u8*)party + 0x12C) = hit;
        party->position.x = hitX;
        return hit;
    }
    return NULL;
}

void nokotaro_hold_cancel(PartyEntry* party) {
    party->useMotionId = 0x14;
}

#define NKF(off) (*(f32*)((u8*)party + (off)))
#define NKS(off) (*(s32*)((u8*)party + (off)))
#define NKS16(off) (*(s16*)((u8*)party + (off)))
#define NKM(off) (*(f32*)((u8*)mario + (off)))
#define NKI(off) (*(ItemEntry**)((u8*)party + (off)))

// Koops' overworld use state machine.  Reconstructed from the original asm; the
// many switches mirror the jump tables in party_nokonoko.s.  Control flow, jump
// tables, case bodies, callees and constants all match.  What is left is the
// compiler's own int->float magic double loading through a fresh `lis` instead
// of `lfd 0x10(r31)` (the same objdiff-only base/offset difference documented
// for party_cloud.c), and the effKemuTestEntry height product's two `lfs` in
// swapped order.
// NONMATCHING (99.73%): register colouring and stack-slot placement only.
void nokonoko_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    const Vec* vec = &vec3_802ce138;
    HitEntry* hit;
    u32 res;
    Vec out;
    Vec out2;
    Vec v;
    Vec scr;
    f32 sx, sy, sz;
    f32 a, b;
    f32 f0, f1, f2, f3, f4, f5, f6, f31;

    if (!(party->flags & 0x80000000)) {
        if (marioKeyOffChk()) {
            scr = party->position;

#ifdef MUST_MATCH
            // MUST_MATCH: the original passes the copy's address (by-value Vec is passed as a
            // pointer in the ABI), which lets `scr` keep a fixed slot at 0x114 like the original.
            ((void (*)(Vec*, f32*, f32*, f32*))marioGetScreenPos)(&scr, &sx, &sy, &sz);
#else
            marioGetScreenPos(scr, &sx, &sy, &sz);
#endif
            if (!marioChkInScreen((s32)sx, (s32)sy)) {
                a = mario->position.x;
                b = mario->position.z;
                movePos(&a, &b, 160.0f,
                        angleABf(mario->position.x, mario->position.z, party->position.x, party->position.z));
                party->position.x = a;
                party->position.z = b;
                nokonoko_finish(party);
                goto L_800CD8BC;
            }
        }
    }

    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        NKI(0x174) = NULL;
        NKS(0x178) = 0;
        NKS(0x17c) = 0;
        NKS(0x180) = 0;
        if (party->flags & 0x80000000) {
            if (party->unk16C > 0) {
                NKS16(0x17e) |= 0x1;
            }
        } else {
            NKS16(0x17e) &= ~0x1;
        }
        party->useMotionId = 0;
    }

    switch (party->useMotionId) {
    case 0:
        if (*(u16*)&mario->currMotionId == 0x20 || *(u16*)&mario->currMotionId == 0x1f) {
            party->useMotionId = 0x46;
            goto L_800CC620;
        }
        partyClearJumpPara(party);
        party->flags |= 0x04000000;
        party->flags2 |= 0x10;
        partyChgPose(party, NK_STR(0x18, "PNK_K_2"));
        NKF(0xF0) = 18.5f;
        party->useMotionId = 1;
        NKS(0x24) = 100;
        NKI(0x174) = NULL;
        NKS16(0x17c) = 0;
        marioAdjustMoveDir();
        mario->baseSpeed = 0.0f;
        *(Vec*)((u8*)party + 0x94) = mario->position;
        {
            u8* base = (u8*)party;
            movePos((f32*)(base + 0x94), (f32*)(base + 0x9c), 2.0f,
                    toMovedirSimple(*(f32*)((u8*)mario + 0x1AC)));
        }
        NKS(0x24) = 4;
        NKF(0x114) = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        party->speed = distABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c)) / (f32)NKS(0x24);
        NKF(0x10C) = revise360(45.0f + NKF(0x10C));
        party->useMotionId = 1;
        marioChgSmallJumpMotion();
        mario->trigFlags |= 0x20000;
        mario->baseSpeed = 0.0f;
        goto L_800CC620;

    case 1:
        if (*(u16*)&mario->currMotionId == 0x20 || *(u16*)&mario->currMotionId == 0x1f) {
            party->useMotionId = 0x46;
            goto L_800CC620;
        }
        NKF(0x10C) = revise360(45.0f + NKF(0x10C));
        NKF(0x58) += (NKF(0x94) - NKF(0x58)) / (f32)NKS(0x24);
        NKF(0x5c) += (NKF(0x98) - NKF(0x5c)) / (f32)NKS(0x24);
        NKF(0x60) += (NKF(0x9c) - NKF(0x60)) / (f32)NKS(0x24);
        if (--NKS(0x24) > 0) {
            goto L_800CC620;
        }
        *(Vec*)((u8*)party + 0x58) = *(Vec*)((u8*)party + 0x94);
        NKF(0x104) = 0.0f;
        NKF(0x110) = *(f32*)(mario->unk1AC);
        NKF(0x10C) = *(f32*)(mario->unk1AC);
        party->useMotionId = 2;
        partyChgPose(party, NK_STR(0x20, "PNK_A_1"));
        /* fallthrough */
    case 2:
        if (*(u16*)&mario->currMotionId == 0x20 || *(u16*)&mario->currMotionId == 0x1f) {
            party->useMotionId = 0x46;
            goto L_800CC620;
        }
        if (mario->hitobjStandOn == NULL) {
            goto L_800CC620;
        }
        marioChgMot(3);
        mario->flags |= 0x80;
        mario->baseSpeed = 0.0f;
        party->useMotionId = 10;
        NKS(0x24) = 15;
        NKF(0x104) = 0.0f;
        NKF(0x128) = 0.0f;
        party->dir = toMovedir(*(f32*)(mario->unk1AC));
        psndSFXOn_3D((const char*)0x92d, &party->position);
        goto L_800CC620;

    case 10:
        psndSFXOn_3D((const char*)0x92a, &party->position);
        f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
        f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
        effSmallStarN64Entry(3, 3, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                             f4, -1.0f, -f31);
        party->flags |= 0x04000000;
        party->flags |= 0x01000000;
        party->useMotionId = 11;
        /* fallthrough */
    case 11:
        NKF(0x104) += 2.0f;
        if (NKF(0x104) > 10.0f) {
            NKF(0x104) = 10.0f;
        }
        /* fallthrough */
    case 12:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (gp->retraceCount & 1) {
            EffectEntry* eff = effKemuTestEntry(0, party->position.x, NKF(0xF0) * 0.25f + party->position.y,
                                                party->position.z, 0.3f);
            effKemuTestDrawCam(eff, party->camId);
        }
        NKF(0x128) += NKF(0x104);
        if (NKI(0x174) == NULL) {
            NKI(0x174) = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->field_F4 + 12.0f);
            if (NKI(0x174) != NULL) {
                itemPickUp(NKI(0x174));
                party->useMotionId = 0x32;
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                     f4, -1.0f, -f31);
                goto L_800CC620;
            }
        }
        if (ccwall(party, 0)) {
            party->useMotionId = 0x32;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
            f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
            effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                 f4, -1.0f, -f31);
            goto L_800CC620;
        }
        if (!(NKF(0x128) >= 120.0f)) {
            goto L_800CC620;
        }
        party->useMotionId = 0xC;
        NKF(0x104) += 0.15f * (-1.0f - NKF(0x104));
        if (!(NKF(0x104) <= 0.0f)) {
            goto L_800CC620;
        }
        NKF(0x104) = 0.0f;
        if (!(party->flags & 0x80000000)) {
            if (*(u16*)(mario->field_0x245 + 1) & 0x400) {
                party->useMotionId = 0xD;
                NKS(0x24) = 0;
            } else {
                party->useMotionId = 0x32;
            }
            goto L_800CC620;
        }
        if (NKS16(0x17e) & 1) {
            NKS(0x24) = 5;
            party->useMotionId = 0x15;
            NKS16(0x17c) = 1;
            if (nokoSe == 0xFFFFFFFFu) {
                nokoSe = psndSFXOn_3D((const char*)0x92e, &party->position);
            }
        } else {
            party->useMotionId = 0x32;
        }
        goto L_800CC620;

    case 13:
        if (++NKS(0x24) >= 5) {
            NKS(0x24) = 5;
            party->useMotionId = 0x14;
            NKS16(0x17c) = 1;
            if (nokoSe == 0xFFFFFFFFu) {
                nokoSe = psndSFXOn_3D((const char*)0x92e, &party->position);
            }
        }
        /* fallthrough */
    case 20:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (gp->retraceCount % 30 == 0) {
            EffectEntry* eff = effKemuTestEntry(5, party->position.x, party->position.y, party->position.z, 1.0f);
            effKemuTestDrawCam(eff, party->camId);
        }
        if (ccwall(party, 0)) {
            party->useMotionId = 0x32;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
            f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
            effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                 f4, -1.0f, -f31);
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            goto L_800CC620;
        }
        if (NKI(0x174) == NULL) {
            NKI(0x174) = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->field_F4 + 12.0f);
            if (NKI(0x174) != NULL) {
                itemPickUp(NKI(0x174));
                party->useMotionId = 0x32;
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                     f4, -1.0f, -f31);
                marioGetPtr();
                if (nokoSe != 0xFFFFFFFFu) {
                    psndSFXOff(nokoSe);
                    nokoSe = 0xFFFFFFFFu;
                }
                goto L_800CC620;
            }
        }
        if (!(party->flags & 0x80000000)) {
            u16 mot = *(u16*)&mario->currMotionId;
            if (mot == 0x19) { goto L_800CC4E8; }
            if (mot == 0x14) { goto L_800CC4E8; }
            if (mot == 0x18) { goto L_800CC4E8; }
            if (mot == 0x16) { goto L_800CC4E8; }
            if ((u16)(mot - 0x1f) <= 1) { goto L_800CC4E8; }
            if (mario->flags & 0x20) { goto L_800CC4E8; }
            if (marioChkKey() != 0) {
                goto L_800CC514;
            }
        L_800CC4E8:
            nokonoko_finish(party);
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            goto L_800CC620;
        L_800CC514:
            if (*(u16*)(mario->field_0x245 + 1) & 0x400) {
                goto L_800CC620;
            }
            party->useMotionId = (NKS16(0x17c) != 0) ? 0x50 : 0x32;
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            goto L_800CC620;
        }
        if (NKS16(0x17e) & 1) {
            party->useMotionId = 0x50;
        } else {
            party->useMotionId = 0x32;
        }
        marioGetPtr();
        if (nokoSe != 0xFFFFFFFFu) {
            psndSFXOff(nokoSe);
            nokoSe = 0xFFFFFFFFu;
        }
        goto L_800CC620;

    case 21:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (gp->retraceCount % 30 == 0) {
            EffectEntry* eff = effKemuTestEntry(5, party->position.x, party->position.y, party->position.z, 1.0f);
            effKemuTestDrawCam(eff, party->camId);
        }
        goto L_800CC620;

    default:
        goto L_800CC620;
    }

L_800CC620:
    switch (party->useMotionId) {
    case 0x32:
        partyClearJumpPara(party);
        party->useMotionId = 0x33;
        NKF(0x104) = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        psndSFXOn_3D((const char*)0x92b, &party->position);
        goto L_800CCD8C;

    case 0x33:
        if (NKS16(0x17c) != 0) {
            f3 = 20.0f;
        } else {
            f3 = 10.0f;
        }
        NKF(0x104) += 0.3f * (f3 - NKF(0x104));
        if (NKF(0x104) >= f3) {
            NKF(0x104) = f3;
            if (NKF(0x128) < 10.0f) {
                party->useMotionId = 0x34;
            }
        }
        /* fallthrough */
    case 0x34:
        if (ccwall(party, 1)) {
            f32 ang, spd;
            partyGetMoveDirSpd(party, &ang, &spd);
            marioChkWallAround(&party->position, 1, spd, ang, party->field_F4, NKF(0xF0));
            party->useMotionId = 0x46;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
            f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
            effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                 f4, -1.0f, -f31);
            goto L_800CD8BC;
        }
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        NKF(0x104) = 0.1f * (-1.0f - NKF(0x104)) + NKF(0x104);
        NKF(0x128) -= NKF(0x104);
        if (!(NKF(0x128) <= 0.0f)) {
            goto L_800CCD8C;
        }
        NKF(0x128) = 0.0f;
        party->useMotionId = 0x35;
        NKF(0x104) = 0.1f * (-1.0f - NKF(0x104)) + NKF(0x104);
        NKF(0x178) = 50.0f;
        goto L_800CCD8C;

    case 0x35:
        if (ccwall(party, 1)) {
            party->useMotionId = 0x46;
            goto L_800CD8BC;
        }
        NKF(0x178) -= 2.2f;
        if (NKF(0x178) <= 0.0f) {
            NKF(0x178) = 0.0f;
        }
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        f3 = -2.0f - NKF(0x104);
        f2 = (NKS16(0x17c) != 0) ? 0.2f : 0.1f;
        NKF(0x104) = f3 * f2 + *(volatile f32*)((u8*)party + 0x104);
        NKF(0x128) -= NKF(0x104);
        if (!(NKF(0x104) <= 0.0f)) {
            goto L_800CCD8C;
        }
        NKF(0x104) = 0.0f;
        {
            Vec pos = *(Vec*)((u8*)party + 0x94);
            res = lookupSafetyPos(&pos, &out, NKF(0xF0));
        }
        if (party->field_138 != NULL && (res == 1 || res == 2 || res == 4 || res == 8)) {
            party->useMotionId = 0x46;
            goto L_800CCD8C;
        }
        switch (res) {
        case 3:
            { Vec pos1 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos1, &out, NKF(0xF0), 135.0f); }
            break;
        case 5:
            { Vec pos2 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos2, &out, NKF(0xF0), 315.0f); }
            break;
        case 10:
            { Vec pos3 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos3, &out, NKF(0xF0), 225.0f); }
            break;
        case 12:
            { Vec pos4 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos4, &out, NKF(0xF0), 315.0f); }
            break;
        case 7: case 14: case 15:
            party->useMotionId = 0x46;
            break;
        case 1:
            { Vec pos5 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos5, &out, NKF(0xF0), 270.0f); }
            break;
        case 8:
            { Vec pos6 = *(Vec*)((u8*)party + 0x94); lookupSafetyPos3(&pos6, &out, NKF(0xF0)); }
            break;
        case 0: case 2: case 4: case 6: case 9: case 11: case 13:
        default:
            { Vec pos7 = *(Vec*)((u8*)party + 0x94); lookupSafetyPos2(&pos7, &out, NKF(0xF0)); }
            break;
        }
        party->useMotionId = 0x36;
        *(Vec*)((u8*)party + 0x94) = out;
        NKF(0x128) = distABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        party->dir = angleABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        goto L_800CCD8C;

    case 0x36:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        NKF(0x104) += 0.5f;
        if (NKF(0x104) >= 4.0f) {
            NKF(0x104) = 4.0f;
        }
        NKF(0x128) -= NKF(0x104);
        if (NKF(0x128) <= 10.0f) {
            party->useMotionId = 0x46;
            NKF(0x104) = 0.0f;
            goto L_800CD8BC;
        }
        goto L_800CCD8C;

    case 0x37:
        if (ccwall(party, 1)) {
            party->useMotionId = 0x46;
            goto L_800CD8BC;
        }
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        NKF(0x178) -= 2.2f;
        if (NKF(0x178) <= 0.0f) {
            NKF(0x178) = 0.0f;
        }
        f3 = -2.0f - NKF(0x104);
        f2 = (NKS16(0x17c) != 0) ? 0.2f : 0.1f;
        NKF(0x104) = f3 * f2 + *(volatile f32*)((u8*)party + 0x104);
        if (NKF(0x104) <= 0.0f) {
            party->useMotionId = 0x46;
            NKF(0x104) = 0.0f;
            goto L_800CD8BC;
        }
        goto L_800CCD8C;

    case 0x46:
        f3 = party->position.x - mario->position.x;
        f4 = party->position.y - mario->position.y;
        f2 = party->position.z - mario->position.z;
        {
            f32 d4 = f3 * f3;
            f32 d3 = f4 * f4;
            f32 d2 = f2 * f2;
            f1 = (f32)sqrt(d4 + d3 + d2);
        }
        if (f1 < 5.0f) {
            f0 = (mario->directionView >= NKF_180) ? -90.0f : 90.0f;
            movePos(&party->position.x, &party->position.z,
                    0.5f * mario->wPlayerCollisionBox.x, mario->directionView + f0);
            ccwall(party, 1);
        }
        NKF(0x104) = 0.0f;
        party->flags2 &= ~0x10;
        party->flags2 |= 0x20;
        party->useMotionId = 0x47;
        goto L_800CCD8C;

    case 0x47:
        if (NKI(0x174) != NULL) {
            itemNokoForceGet(NKI(0x174));
            NKI(0x174) = NULL;
        }
        nokonoko_finish(party);
        goto L_800CD8BC;

    default:
        goto L_800CCD8C;
    }

L_800CCD8C:
    switch (party->useMotionId) {
    case 0x50:
        party->useMotionId = 0x51;
        NKF(0x104) = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        NKF(0x128) += 75.0f;
        psndSFXOn_3D((const char*)0x92b, &party->position);
        /* fallthrough */
    case 0x51:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        NKF(0x104) += 3.0f;
        if (NKF(0x104) >= 18.0f) {
            NKF(0x104) = 18.0f;
        }
        if (NKI(0x174) == NULL) {
            NKI(0x174) = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->field_F4 + 12.0f);
            if (NKI(0x174) != NULL) {
                itemPickUp(NKI(0x174));
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                     f4, -1.0f, -f31);
            }
        }
        hit = ccwall(party, 1);
        if (hit != NULL) {
            if (!(hitGetAttr(hit) & 0x80000000)) {
                party->useMotionId = 0x64;
                goto L_800CD8BC;
            }
            mobjHitObjPtrToPtr(hit);
            party->useMotionId = 0x6e;
            goto L_800CD8BC;
        }
        NKF(0x128) -= NKF(0x104);
        if (NKF(0x128) <= 20.0f) {
            party->useMotionId = 0x52;
        }
        goto L_800CD8A8;

    case 0x52:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        NKF(0x104) += 0.35f * (-1.0f - NKF(0x104));
        if (NKI(0x174) == NULL) {
            NKI(0x174) = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->field_F4 + 12.0f);
            if (NKI(0x174) != NULL) {
                itemPickUp(NKI(0x174));
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + NKF(0xF0), party->position.z,
                                     f4, -1.0f, -f31);
            }
        }
        if (ccwall(party, 1)) {
            party->useMotionId = 0x64;
            goto L_800CD8A8;
        }
        NKF(0x128) -= NKF(0x104);
        if (NKF(0x128) <= 0.0f) {
            NKF(0x128) = distABf(NKF(0x94), NKF(0x9c), party->position.x, party->position.z);
            party->useMotionId = 0x54;
            NKF(0x178) = 50.0f;
            party->dir = angleABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        }
        goto L_800CD8A8;

    case 0x54:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (ccwall(party, 1)) {
            party->useMotionId = 0x64;
            goto L_800CD8A8;
        }
        NKF(0x128) -= NKF(0x104);
        if (NKF(0x128) <= 30.0f) {
            party->useMotionId = 0x56;
            NKF(0x104) = 0.0f;
            NKS(0x24) = sysMsec2Frame(500);
        }
        goto L_800CD8A8;

    case 0x56:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (--NKS(0x24) <= 0) {
            party->useMotionId = 0x64;
        }
        if (ccwall(party, 1)) {
            party->useMotionId = 0x64;
        }
        goto L_800CD8A8;

    case 0x64:
        ccwall(party, 1);
        f3 = party->position.x - mario->position.x;
        f4 = party->position.y - mario->position.y;
        f2 = party->position.z - mario->position.z;
        {
            f32 d4 = f3 * f3;
            f32 d3 = f4 * f4;
            f32 d2 = f2 * f2;
            f1 = (f32)sqrt(d4 + d3 + d2);
        }
        if (f1 < 5.0f) {
            f0 = (mario->directionView >= NKF_180) ? 90.0f : -90.0f;
            movePos(&party->position.x, &party->position.z,
                    0.5f * mario->wPlayerCollisionBox.x, mario->directionView + f0);
            ccwall(party, 1);
        }
        NKF(0x104) = 0.0f;
        party->flags2 &= ~0x10;
        if (NKI(0x174) != NULL) {
            itemNokoForceGet(NKI(0x174));
            NKI(0x174) = NULL;
        }
        if (party->field_138 == NULL) {
            party->useMotionId = 0x58;
            goto L_800CD8A8;
        }
        if (NKI(0x174) != NULL) {
            itemNokoForceGet(NKI(0x174));
            NKI(0x174) = NULL;
        }
        nokonoko_finish(party);
        goto L_800CD8BC;

    case 0x58:
        NKF(0x104) = 0.0f;
        {
            Vec pos = *(Vec*)((u8*)party + 0x94);
            res = lookupSafetyPos(&pos, &out2, NKF(0xF0));
        }
        if (party->field_138 != NULL && (res == 1 || res == 2 || res == 4 || res == 8)) {
            party->useMotionId = 0x46;
            goto L_800CD8A8;
        }
        switch (res) {
        case 3:
            { Vec pos8 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos8, &out2, NKF(0xF0), 135.0f); }
            break;
        case 5:
            { Vec pos9 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos9, &out2, NKF(0xF0), 315.0f); }
            break;
        case 10:
            { Vec pos10 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos10, &out2, NKF(0xF0), 225.0f); }
            break;
        case 12:
            { Vec pos11 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos11, &out2, NKF(0xF0), 315.0f); }
            break;
        case 7: case 14: case 15:
            party->useMotionId = 0x46;
            break;
        case 1:
            { Vec pos12 = *(Vec*)((u8*)party + 0x94); lookupSafetyPosSub(&pos12, &out2, NKF(0xF0), 270.0f); }
            break;
        case 8:
            { Vec pos13 = *(Vec*)((u8*)party + 0x94); lookupSafetyPos3(&pos13, &out2, NKF(0xF0)); }
            break;
        case 0: case 2: case 4: case 6: case 9: case 11: case 13:
        default:
            { Vec pos14 = *(Vec*)((u8*)party + 0x94); lookupSafetyPos2(&pos14, &out2, NKF(0xF0)); }
            break;
        }
        party->useMotionId = 0x5A;
        *(Vec*)((u8*)party + 0x94) = out2;
        NKF(0x128) = distABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        party->dir = angleABf(party->position.x, party->position.z, NKF(0x94), NKF(0x9c));
        NKF(0x178) = 50.0f;
        goto L_800CD8A8;

    case 0x5A:
        f1 = revise360(party->dir - NKF(0xF8));
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        NKF(0x104) += 0.5f;
        if (NKF(0x104) >= 4.0f) {
            NKF(0x104) = 4.0f;
        }
        NKF(0x128) -= NKF(0x104);
        if (NKF(0x128) <= 10.0f) {
            party->useMotionId = 0x46;
            NKF(0x104) = 0.0f;
        }
        goto L_800CD8A8;

    case 0x6E:
        NKF(0x104) = 0.0f;
        NKF(0x114) = 15.0f;
        NKF(0x118) = 0.0f;
        NKS(0x24) = sysMsec2Frame(1000);
        party->useMotionId = 0x6f;
        /* fallthrough */
    case 0x6F:
        NKF(0x5c) += NKF(0x114);
        {
            Vec vscr = party->position;
            f32 vsx, vsy, vsz;
            marioGetScreenPos(vscr, &vsx, &vsy, &vsz);
            if (vsy <= -300.0f) {
                NKS(0x24) = sysMsec2Frame(800);
                party->useMotionId = 0x70;
            }
        }
        goto L_800CD8A8;

    case 0x70:
        if (--NKS(0x24) > 0) {
            goto L_800CD8A8;
        }
        if (!(party->flags & 0x80000000)) {
            partyGetReturnPos(&v);
        } else {
            *(Vec*)&v = *(Vec*)((u8*)mario + 0x8c);
            f1 = *(f32*)(mario->unk1AC);
            {
                s32 flag = 0;
                if (f1 >= 90.0f && f1 <= 270.0f) {
                    flag = 1;
                }
                movePos(&v.x, &v.z, 30.0f, toMovedir2(flag ? 0.0f : 180.0f, -30.0f));
            }
        }
        party->position.x = v.x;
        party->position.y = v.y + 1000.0f;
        party->position.z = v.z;
        NKF(0x114) = 0.0f;
        NKF(0x118) = -1.0f;
        NKF(0x104) = 0.0f;
        party->flags |= 0x20;
        party->flags |= 0x02000000;
        party->flags &= ~0x04000000;
        party->flags &= ~0x01000000;
        party->flags2 &= ~0x20;
        party->flags2 &= ~0x10;
        NKF(0xF0) = 37.0f;
        party->useMotionId = 0x71;
        goto L_800CD8A8;

    case 0x71:
        {
            Vec wscr = party->position;
            f32 wsx, wsy, wsz;
            marioGetScreenPos(wscr, &wsx, &wsy, &wsz);
            if (wsy > 240.0f) {
                party->flags &= ~0x02000000;
            }
            (void)wsx;
            (void)wsz;
        }
        partyMove(party);
        if (NKF(0x114) <= -6.0f) {
            NKF(0x118) = 0.0f;
            NKF(0x114) = -6.0f;
        }
        if (party->position.y < -2000.0f) {
            *(Vec*)((u8*)party + 0x58) = *(Vec*)((u8*)mario + 0x8c);
            party->position.y += 200.0f;
        }
        if (party->field_138 != NULL) {
            NKF(0x110) = *(f32*)(mario->unk1AC);
            NKF(0x10C) = NKF(0x110);
            if (((hitGetAttr((HitEntry*)party->field_138) >> 9) & 1) == 0) {
                party->useMotionId = 0x72;
                NKS(0x24) = 30;
            }
        }
        goto L_800CD8A8;

    case 0x72:
        if (--NKS(0x24) > 0) {
            goto L_800CD8A8;
        }
        nokonoko_finish(party);
        goto L_800CD8BC;

    default:
        goto L_800CD8A8;
    }

L_800CD8A8:
    if (party->useMotionId != 0x64) {
        partyMoveNoHosei(party);
    }
L_800CD8BC:
    return;
}

#undef NKF
#undef NKS
#undef NKS16
#undef NKM

void nokonoko_finish(PartyEntry* party) {
    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    if (*(ItemEntry**)((u8*)party + 0x174) != NULL) {
        itemNokoForceGet(*(ItemEntry**)((u8*)party + 0x174));
        *(ItemEntry**)((u8*)party + 0x174) = NULL;
    }
    marioGetPtr();
    if (nokoSe != 0xFFFFFFFFu) {
        psndSFXOff(nokoSe);
        nokoSe = 0xFFFFFFFFu;
    }
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags2 &= ~0x20;
    party->flags2 &= ~0x10;
    *(f32*)((u8*)party + 0xF0) = float_37;
    *(s32*)((u8*)party + 0x178) = 0;
    *(s32*)((u8*)party + 0x17C) = 0;
    *(s32*)((u8*)party + 0x180) = 0;
    if (party->field_34 == 9) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
        partyChgRunMode(party, 2);
        party->field_3B = 0;
    } else {
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        partyChgRunMode(party, 0);
    }
}

void nokonoko_use_post(PartyEntry* party) {
    if (*(ItemEntry**)((u8*)party + 0x174) != NULL) {
        itemNokoForceGet(*(ItemEntry**)((u8*)party + 0x174));
        *(ItemEntry**)((u8*)party + 0x174) = NULL;
    }
    marioGetPtr();
    if (nokoSe != 0xFFFFFFFFu) {
        psndSFXOff(nokoSe);
        nokoSe = 0xFFFFFFFFu;
    }
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags2 &= ~0x20;
    party->flags2 &= ~0x10;
    *(f32*)((u8*)party + 0xF0) = float_37;
    *(s32*)((u8*)party + 0x178) = 0;
    *(s32*)((u8*)party + 0x17C) = 0;
    *(s32*)((u8*)party + 0x180) = 0;
}

void unk_800cd9a0(void) {
    PartyEntry* party;

    marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (nokonokoGetStatus(party)) {
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
            party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
            party->flags &= ~PARTY_FLAG_FORCE_FALL;
            party->flags2 &= ~0x20;
            party->flags2 &= ~0x10;
            *(f32*)((u8*)party + 0xF0) = float_37;
            *(s32*)((u8*)party + 0x178) = 0;
            *(s32*)((u8*)party + 0x17C) = 0;
            *(s32*)((u8*)party + 0x180) = 0;
        }
        if (party->field_34 != 8 && party->field_34 != 0xD) {
            partyChgRunMode(party, 2);
            party->field_3B = 0;
        }
    }
}

void nokonoko_move(PartyEntry* party) {
    marioGetPtr();
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyWalkInit(party, 1);
    }
    unk_800c92c4(party);
    partyRecordFootmark(party);
    switch (party->field_3B) {
    case 0:
        party_move_mode_stay(party);
        break;
    case 1:
        party_move_mode_walk(party);
        break;
    case 2:
        party_move_mode_behind_mario(party);
        break;
    case 3:
        party_move_mode_beside_mario(party);
        break;
    case 4:
        break;
    default:
        break;
    }
}

void nokonoko_init(PartyEntry* party) {
    party->field_33 = 2;
    party->field_32 = 0;
    *(f32*)((u8*)party + 0xF0) = float_37;
    *(f32*)((u8*)party + 0xF4) = float_24;
    nokoSe = 0xFFFFFFFFu;
}

void nokonoko_bye(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyGoodbyeInit(party);
    }
    if (partyGoodbyeMain(party)) {
        partyChgRunMode(party, 0xE);
    }
}

s32 nokonoko_holdItem(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    return *(ItemEntry**)((u8*)party + 0x174) != NULL;
}

s32 nokonokoGetStatus(PartyEntry* party) {
    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    {
        s32 result = 7;
        if (party->useMotionId <= 2) {
            result = 1;
        } else if (party->useMotionId >= 0xA && party->useMotionId <= 0xD) {
            result = 2;
        } else if (party->useMotionId >= 0x14 && party->useMotionId <= 0x15) {
            result = 3;
        } else if (party->useMotionId >= 0x32 && party->useMotionId <= 0x47) {
            result = 4;
        } else if (party->useMotionId >= 0x50 && party->useMotionId <= 0x64) {
            result = 5;
        } else if (party->useMotionId >= 0x6E && party->useMotionId <= 0x72) {
            result = 6;
        }
        return result;
    }
}
