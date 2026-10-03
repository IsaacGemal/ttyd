#include "party.h"
#include "party/party_nokonoko.h"
#include "party/party_move.h"
#include "mario/mario.h"
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

// NONMATCHING (93.96%): probe values and control flow match; z lands in f26 like the original,
// but s/x take f27 and baseY/distance use f29/f28 where the original uses f29/f28/f27.
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
    t = radius * s;
    x = pos->x + t;
    y = pos->y + height;
    t = radius * -c;
    z = pos->z + t;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
    } else {
        s = sin(MATCH_F32(float_4p7124));
        c = cos(MATCH_F32(float_4p7124));
        baseY = pos->y;
        t = radius * s;
        x = pos->x + t;
        y = pos->y + height;
        t = radius * -c;
        z = pos->z + t;
        distance = float_0p1 + height;
        if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
            out->x = x;
            out->z = z;
            out->y = baseY;
        } else {
            *out = *pos;
        }
    }
}

// The four probes, the selection logic and the register mask (r4) match; what
// still differs is scheduler-only: blocks 2-4 load float_0 after pos->z (the
// original loads float_0 first), and the final pos copy stores out->x before
// loading pos->y where the original loads pos->y first.
// NONMATCHING (98.38%)
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
    // Word-wise copy keeps `result` in r4 and reuses r4 as the second scratch, as the original does;
    // the plain struct copy makes the compiler pick r5 for result.  Same three words either way.
    *(u32*)&out->x = *(u32*)&pos->x;
    *(u32*)&out->y = *(u32*)&pos->y;
    *(u32*)&out->z = *(u32*)&pos->z;
#else
    *out = *pos;
#endif
    return result;
}

// NONMATCHING (95.80%): early-return four probes; calls, dead pi sin/cos, baseY cache and constants match.
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
    x = pos->x + t;
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
    x = pos->x + t;
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
    x = pos->x + t;
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
