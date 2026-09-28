#include "party.h"
#include "party/party_nokonoko.h"
#include "party/party_move.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "driver/hitdrv.h"
#include "driver/itemdrv.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>
#include <math.h>
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
