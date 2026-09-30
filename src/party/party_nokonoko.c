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

// NONMATCHING (72%): the register allocation of the height argument (original
// f30, ours f31) and the z multiply/add still differ.
void lookupSafetyPos3(Vec* pos, Vec* out, f32 height) {
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 x;
    f32 radius;
    f32 s;
    f32 c;
    f32 y;
    f32 z;

    radius = marioGetPtr()->wPlayerCollisionBox.x;
    distance = float_0p1 + height;
    s = sin(MATCH_F32(float_1p5708));
    c = cos(MATCH_F32(float_1p5708));
    x = radius * s;
    x = pos->x + x;
    y = pos->y + height;
    z = radius * -c;
    z = pos->z + z;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = pos->y;
    } else {
        s = sin(MATCH_F32(float_4p7124));
        c = cos(MATCH_F32(float_4p7124));
        x = radius * s;
        x = pos->x + x;
        y = pos->y + height;
        z = radius * -c;
        z = pos->z + z;
        if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
            out->x = x;
            out->z = z;
            out->y = pos->y;
        } else {
            *out = *pos;
        }
    }
}

// The four probes and the selection logic match structurally; what differs is
// only register allocation: block 1's rad lands in f28 instead of f31, the mask
// lands in r5 instead of r4 (so the copy branches use a different scratch), and
// blocks 2-4 schedule the float_0 load after pos->z.
// NONMATCHING (96.81%)
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
    f32 rad;
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
    rad = float_3p1416 * angle / float_180;
    s = sin(rad);
    x = radius * s + pos->x;
    v1.x = x;
    c = cos(rad);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v1.y = pos->y;
    v1.z = z;
    distance = float_0p1 + height;
    hit0 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    angle = revise360(toMovedir(*(f32*)mario->unk1AC) - float_90);
    rad = float_3p1416 * angle / float_180;
    s = sin(rad);
    x = radius * s + pos->x;
    v2.x = x;
    c = cos(rad);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v2.y = pos->y;
    v2.z = z;
    distance = float_0p1 + height;
    hit1 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    angle = revise360(toMovedir(*(f32*)mario->unk1AC) + float_90);
    rad = float_3p1416 * angle / float_180;
    s = sin(rad);
    x = radius * s + pos->x;
    v3.x = x;
    c = cos(rad);
    y = pos->y + height;
    z = pos->z + radius * -c;
    v3.y = pos->y;
    v3.z = z;
    distance = float_0p1 + height;
    hit2 = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1,
                          float_0);

    angle = revise360(toMovedir(*(f32*)mario->unk1AC));
    rad = float_3p1416 * angle / float_180;
    s = sin(rad);
    x = radius * s + pos->x;
    v4.x = x;
    c = cos(rad);
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
            out->y = pos->y;
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
            return result;
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
        }
        if (hit1) {
            *out = v2;
            return result;
        }
        if (hit2) {
            *out = v3;
            return result;
        }
        *out = *pos;
    }
    return result;
}

// NONMATCHING (76%): the pi branch in the original makes dead sin/cos calls
// and register allocation differs.
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

    radius = marioGetPtr()->wPlayerCollisionBox.x;
    distance = float_0p1 + height;
    s = sin(MATCH_F32(float_1p5708));
    c = cos(MATCH_F32(float_1p5708));
    x = radius * s;
    x = pos->x + x;
    y = pos->y + height;
    z = radius * -c;
    z = pos->z + z;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
        out->x = x;
        out->z = z;
        out->y = pos->y;
    } else {
        s = sin(MATCH_F32(float_4p7124));
        c = cos(MATCH_F32(float_4p7124));
        x = radius * s;
        x = pos->x + x;
        y = pos->y + height;
        z = radius * -c;
        z = pos->z + z;
        if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
            out->x = x;
            out->z = z;
            out->y = pos->y;
        } else {
            x = pos->x;
            y = pos->y + height;
            z = pos->z;
            if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
                out->x = x;
                out->z = z;
                out->y = pos->y;
            } else {
                s = sin(float_0);
                c = cos(float_0);
                x = radius * s;
                x = pos->x + x;
                y = pos->y + height;
                z = radius * -c;
                z = pos->z + z;
                if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, float_0, float_neg1, float_0)) {
                    out->x = x;
                    out->z = z;
                    out->y = pos->y;
                } else {
                    *out = *pos;
                }
            }
        }
    }
}

// Four floor/wall probes; control flow and constants match, remaining diffs are
// float register allocation in the probe origins and the normal projection.
// NONMATCHING (92.6%): see REPORT.md for details.
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
    f32 nx, nz;
    f32 dot;
    f32 oy;
    f32 x, z;

    y = 10.0f;

    oy = party->position.y + *(f32*)(p + 0xF0) + y;
    distance = *(f32*)(p + 0xF0) + 30.0f;
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, party->position.x, oy,
                         party->position.z, 0.0f, -1.0f, 0.0f);
    if (((hitGetAttr(hit) >> 9) & 1) == 1) {
        hit = NULL;
    }
    *(HitEntry**)((u8*)party + 0x138) = hit;
    if (hit != NULL) {
        f32 py;
        hitY = (f32)(s32)(100.0f * hitY + 0.5f) / 100.0f;
        py = (f32)(s32)(100.0f * party->position.y + 0.5f) / 100.0f;
        if (__fabsf(py - hitY) <= 11.0f) {
            if (py <= hitY) {
                if (flag == 0) {
                    party->position.y = hitY;
                    if (__fabsf(py - hitY) >= 1.0f) {
                        *(s16*)(p + 0x17E) |= 0x2;
                    }
                }
            } else {
                if ((*(s16*)(p + 0x17E) & 0x2) != 0 && flag == 1) {
                    party->position.y = hitY;
                }
                if (__fabsf(py - hitY) <= 6.0f) {
                    party->position.y = hitY;
                }
            }
        }
    }

    *(HitEntry**)((u8*)party + 0x130) = NULL;
    *(HitEntry**)((u8*)party + 0x12C) = NULL;
    y += 1.0f;

    distance = 0.5f * *(f32*)(p + 0xF4) + party->speed;
    x = party->position.x;
    z = party->position.z;
    oy = party->position.y + y;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, oy, z, sinV, 0.0f, cosV);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin2, &cos2, party->dir + 180.0f);
        party->position.x = hitX + 0.5f * (*(f32*)(p + 0xF4) * sin2);
        party->position.z = hitZ + 0.5f * (*(f32*)(p + 0xF4) * cos2);
        nx = hitNX;
        nz = hitNZ;
        dot = party->speed * sinV * nx + party->speed * cosV * nz;
        party->position.x += 0.5f * (party->speed * sinV - dot * nx);
        party->position.z += 0.5f * (party->speed * cosV - dot * nz);
        if (*(s16*)(p + 0x17C) != 0 || flag == 0) {
            *(HitEntry**)((u8*)party + 0x130) = hit;
        }
        *(HitEntry**)((u8*)party + 0x12C) = hit;
        return hit;
    }

    f31 = 0.4f * *(f32*)(p + 0xF4);
    z = party->position.z + f31 * (f32)cos((f64)(3.1415927f * (party->dir - 90.0f) / 180.0f));
    x = party->position.x + f31 * (f32)sin((f64)(3.1415927f * (party->dir - 90.0f) / 180.0f));
    distance = 0.5f * *(f32*)(p + 0xF4) + party->speed;
    oy = party->position.y + y;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, oy, z, sinV, 0.0f, cosV);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin3, &cos3, party->dir + 180.0f);
        party->position.x = hitX + 0.5f * (*(f32*)(p + 0xF4) * sin3);
        party->position.z = hitZ + 0.5f * (*(f32*)(p + 0xF4) * cos3);
        party->position.z -= f31 * (f32)cos((f64)(3.1415927f * (party->dir - 90.0f) / 180.0f));
        party->position.x -= f31 * (f32)sin((f64)(3.1415927f * (party->dir - 90.0f) / 180.0f));
        nx = hitNX;
        nz = hitNZ;
        dot = party->speed * sinV * nx + party->speed * cosV * nz;
        party->position.x += 0.5f * (party->speed * sinV - dot * nx);
        party->position.z += 0.5f * (party->speed * cosV - dot * nz);
        if (*(s16*)(p + 0x17C) != 0 || flag == 0) {
            *(HitEntry**)((u8*)party + 0x130) = hit;
        }
        *(HitEntry**)((u8*)party + 0x12C) = hit;
        party->position.x = hitX;
        return hit;
    }

    f31 = 0.4f * *(f32*)(p + 0xF4);
    z = party->position.z + f31 * (f32)cos((f64)(3.1415927f * (party->dir + 90.0f) / 180.0f));
    x = party->position.x + f31 * (f32)sin((f64)(3.1415927f * (party->dir + 90.0f) / 180.0f));
    distance = 0.5f * *(f32*)(p + 0xF4) + party->speed;
    y = party->position.y + y;
    distance = 0.5f * *(f32*)(p + 0xF4) + party->speed;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, sinV, 0.0f, cosV);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin4, &cos4, party->dir + 180.0f);
        party->position.x = hitX + 0.5f * (*(f32*)(p + 0xF4) * sin4);
        party->position.z = hitZ + 0.5f * (*(f32*)(p + 0xF4) * cos4);
        party->position.z -= f31 * (f32)cos((f64)(3.1415927f * (party->dir + 90.0f) / 180.0f));
        party->position.x -= f31 * (f32)sin((f64)(3.1415927f * (party->dir + 90.0f) / 180.0f));
        nx = hitNX;
        nz = hitNZ;
        dot = party->speed * sinV * nx + party->speed * cosV * nz;
        party->position.x += 0.5f * (party->speed * sinV - dot * nx);
        party->position.z += 0.5f * (party->speed * cosV - dot * nz);
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
