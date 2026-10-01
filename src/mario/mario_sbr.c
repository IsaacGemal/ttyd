#include "mario/mario_sbr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "mario/mario_motion.h"
#include "mario/mario_party.h"
#include "party.h"
#include "driver/seqdrv.h"
#include "driver/hitdrv.h"
#include "demo/DEMOPad.h"
#include "system.h"
#include <string.h>
#include "mot/mot_walk.h"
#include "koopa.h"
#include "party/party_yoshi.h"
#include "motion/mot_walk.h"
#include "motion/mot_slit.h"
#include "math_ext.h"

// locals for symbols with no home header

// rub work state, .bss
typedef struct RubWork {
    u8 unk00[4];    // 0x00
    s32 unk04;      // 0x04
    s32 unk08;      // 0x08
    s32 unk0C;      // 0x0C
    s32 unk10;      // 0x10
    s32 unk14;      // 0x14
    u8 unk18[4];    // 0x18
    s32 unk1C;      // 0x1C
    s32 unk20;      // 0x20
    s32 unk24;      // 0x24
    u8 unk28[0x3C - 0x28];
    s16 unk3C[21];  // 0x3C
    u8 unk66[2];    // 0x66
} RubWork;

static RubWork rubwork;

const Vec vec3_802c5a60 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5a6c = { 0.0f, 0.0f, 0.0f };

// new in retail
void marioSetBottomlessResetPosition(f32 x, f32 y, f32 z) {
    MarioWork* wp = marioGetPtr();
    Vec position;

    position = (Vec){ x, y + 37.0f, z };
    wp->bottomlessResetPosition = position;
}

f32 revise360(f32 angle) {
    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    return angle;
}

void* marioGetTouchNpcPtr(void) {
    return *(void**)((u8*)marioGetPtr() + 0x1D4);
}

void N_marioReloadMapOnBottomlessOff(void) {
    MarioWork* p = marioGetPtr();
    *(s32*)((u8*)p + 0x2C8) = 0;
}

void N_marioReloadMapOnBottomlessOn(void) {
    MarioWork* p = marioGetPtr();
    *(s32*)((u8*)p + 0x2C8) = 1;
}

void marioSetPeakYpos(void) {
    MarioWork* p = marioGetPtr();
    *(f32*)((u8*)p + 0x12C) = p->position.y;
}

BOOL marioChkPlanePlace(HitEntry* hit) {
    return (hitGetAttr(hit) & 0x38) != 0;
}

void marioResetGravity(void) {
    marioGetPtr()->playerGravity = 1.0f;
}

void marioGetRubInit(void) {
    memset(&rubwork, 0, 0x68);
    rubwork.unk20 = -1;
}

s32 N_marioGetMovementSpeed(void) {
    MarioWork* p = marioGetPtr();

    // wStickDir1/2 are declared u8 in mario.h, but the original sign-extends them (`extsb`),
    // so they are really s8. The signed lvalue is needed for mwcc to keep the raw byte in a
    // register and extend it at each use.
    if (*(s8*)&p->wStickDir1 == 0 && *(s8*)&p->wStickDir2 == 0) {
        return 0;
    }
    if (*(s8*)&p->wStickDir1 * *(s8*)&p->wStickDir1 + *(s8*)&p->wStickDir2 * *(s8*)&p->wStickDir2 > 0xBD1) {
        return 2;
    }
    return 1;
}

f32 marioGetMoveRate(f32 angle) {
    f32 diff = angle - marioGetPtr()->targetYaw;
    f32 c = (f32)cos(3.1415927f * diff / 180.0f);
    return 0.19f * __fabsf(c) + 1.0f;
}

f32 marioGetDispDir(void) {
    MarioWork* p = marioGetPtr();
    f32 diff = p->unk1A8 - 90.0f;
    return revise360(diff + p->targetYaw);
}

void N_marioSetBottomlessRespawnPos(f32 x, f32 y, f32 z) {
    MarioWork* p = marioGetPtr();
    Vec position = vec3_802c5a6c;

    position.x = x;
    position.y = y;
    position.z = z;
    p->bottomlessResetPosition = position;
}

void N_marioSetBottomlessRespawnPosOnBeroEntry(f32 x, f32 y, f32 z) {
    MarioWork* p = marioGetPtr();
    Vec position = vec3_802c5a60;

    position.x = x;
    position.y = y + 37.0f;
    position.z = z;
    p->bottomlessResetPosition = position;
}

void marioForceMoveMain(void) {
    MarioWork* p = marioGetPtr();

    if (p->flags & MARIO_FLAG_FORCED_MOVEMENT) {
        if (--p->forceMoveTimer <= 0) {
            p->forceMoveTimer = 0;
            p->flags &= ~MARIO_FLAG_FORCED_MOVEMENT;
            p->baseSpeed = 0.0f;
            if (p->controlStickSensitivity > 1.0f) {
                p->baseSpeed = marioGetDashSpd();
            } else if (p->controlStickSensitivity > 0.0f) {
                p->baseSpeed = marioGetWalkSpd();
            }
        }
    }
}

void marioSetForceMove(s32 timer, f32 direction, f32 speed) {
    MarioWork* p = marioGetPtr();

    p->flags |= MARIO_FLAG_FORCED_MOVEMENT;
    *(f32*)((u8*)p + 0x19C) = direction;
    p->directionView = direction;
    p->forceMoveTimer = timer;
    p->baseSpeed = speed;
    if (p->currMotionId == MARIO_MOTION_SHIP ||
        (u16)(p->currMotionId - MARIO_MOTION_SLIT) <= 1 ||
        p->currMotionId == MARIO_MOTION_YOSHI) {
        return;
    }
#ifdef MUST_MATCH
    // Without the (s32) casts the ternary has enum type and mwcc zero-extends it (clrlwi r3,r0,16)
    // before the call; the original passes the constant directly in r3.
    marioChgMot(speed > p->mBaseWalkSpeed ? (s32)MARIO_MOTION_DASH : (s32)MARIO_MOTION_WALK);
#else
    marioChgMot(speed > p->mBaseWalkSpeed ? MARIO_MOTION_DASH : MARIO_MOTION_WALK);
#endif
    *(f32*)((u8*)p + 0x19C) = direction;
    p->directionView = direction;
    p->forceMoveTimer = timer;
    p->baseSpeed = speed;
}

void marioBottomless(void) {
    MarioWork* p = marioGetPtr();
    PartyEntry* party;

    if (p->position.y > -2000.0f) {
        return;
    }
    if (marioBgmodeChk() == 1) {
        marioChgMot(MARIO_MOTION_BOTTOMLESS);
        return;
    }
    if (*(u32*)((u8*)p + 0x2C8) != 0) {
        seqSetSeq(3, gp->currentMapName, gp->beroEnterName);
        return;
    }
    p->position = p->bottomlessResetPosition;
    party = partyGetPtr(marioGetPartyId());
    if (party != NULL) {
        party->position = p->position;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL) {
        party->position = p->position;
    }
    if (marioBgmodeChk() == 1) {
        p->position.z = -450.0f;
    }
    marioChgMot(MARIO_MOTION_FORCE_RESET);
}

f32 toMovedirSimple(f32 angle) {
    MarioWork* p = marioGetPtr();

    angle = (f32)(s32)(100.0f * (0.05f + ((270.0f - angle) + p->targetYaw))) / 100.0f;
    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    return angle;
}

f32 partyToMovedir(PartyEntry* party, f32 dir) {
    f32 a;
    MarioWork* player = party->playerPtr;

    if (party->flags & 0x1000) {
#ifdef MUST_MATCH
        // Fusing the field load into the first comparison makes mwcc emit the 280 constant load
        // before the field load, which the original does (plain `a = ...; if (280.0f == a)` emits them swapped).
        if (280.0f == (a = party->field_10C) || 460.0f == a) {
#else
        a = party->field_10C;
        if (280.0f == a || 460.0f == a) {
#endif
            dir = 0.0f;
        } else if (a == 260.0f || a == 440.0f) {
            dir = 180.0f;
        } else {
            while (a < 0.0f) {
                a += 360.0f;
            }
            while (a >= 360.0f) {
                a -= 360.0f;
            }
            dir = (a >= 180.0f) ? 0.0f : 180.0f;
        }
    }
    dir = (f32)(s32)(100.0f * (0.05f + ((270.0f - dir) + player->targetYaw))) / 100.0f;
    while (dir < 0.0f) {
        dir += 360.0f;
    }
    while (dir >= 360.0f) {
        dir -= 360.0f;
    }
    return dir;
}

f32 toMovedir(f32 angle) {
    MarioWork* p = marioGetPtr();
    f32 dir;

    if (p->flags & MARIO_FLAG_PAPER_MODE) {
#ifdef MUST_MATCH
        // See partyToMovedir: fusing the load keeps the 280 constant load ahead of the field load.
        if (280.0f == (dir = p->unk1A8) || 460.0f == dir) {
#else
        dir = p->unk1A8;
        if (280.0f == dir || 460.0f == dir) {
#endif
            angle = 0.0f;
        } else if (260.0f == dir || 440.0f == dir) {
            angle = 180.0f;
        } else {
            while (dir < 0.0f) {
                dir += 360.0f;
            }
            while (dir >= 360.0f) {
                dir -= 360.0f;
            }
            angle = (dir >= 180.0f) ? 0.0f : 180.0f;
        }
    }
    angle = (f32)(s32)(100.0f * (0.05f + ((270.0f - angle) + p->targetYaw))) / 100.0f;
    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    return angle;
}

f32 toMovedir2(f32 base, f32 adjust) {
    MarioWork* p = marioGetPtr();
    f32 dir;
    f32 delta;

    if (p->flags & MARIO_FLAG_PAPER_MODE) {
#ifdef MUST_MATCH
        // See partyToMovedir: fusing the load keeps the 280 constant load ahead of the field load.
        if (280.0f == (dir = p->unk1A8) || 460.0f == dir) {
#else
        dir = p->unk1A8;
        if (280.0f == dir || 460.0f == dir) {
#endif
            base = 0.0f;
        } else if (260.0f == dir || 440.0f == dir) {
            base = 180.0f;
        } else {
            while (dir < 0.0f) {
                dir += 360.0f;
            }
            while (dir >= 360.0f) {
                dir -= 360.0f;
            }
            base = (dir >= 180.0f) ? 0.0f : 180.0f;
        }
    }
    base = (f32)(s32)(100.0f * (0.05f + ((270.0f - base) + p->targetYaw))) / 100.0f;
    while (base < 0.0f) {
        base += 360.0f;
    }
    while (base >= 360.0f) {
        base -= 360.0f;
    }
    delta = base - p->targetYaw;
    while (delta < 0.0f) {
        delta += 360.0f;
    }
    while (delta >= 360.0f) {
        delta -= 360.0f;
    }
    base += (delta >= 180.0f) ? -adjust : adjust;
    return revise360(base);
}

void marioAdjustMoveDir(void) {
    MarioWork* p = marioGetPtr();

    if (p->unk1A8 >= 90.0f && p->unk1A8 <= 270.0f) {
        *(f32*)((u8*)p + 0x19C) = toMovedir(90.0f);
        p->directionView = *(f32*)((u8*)p + 0x19C);
        p->unk1A8 = 180.0f;
        *(f32*)((u8*)p + 0x1AC) = 180.0f;
    } else {
        *(f32*)((u8*)p + 0x19C) = toMovedir(90.0f);
        p->directionView = *(f32*)((u8*)p + 0x19C);
        p->unk1A8 = 0.0f;
        *(f32*)((u8*)p + 0x1AC) = 0.0f;
    }
}

s32 marioSetDirEventMain(void) {
    MarioWork* p = marioGetPtr();
    s32 frames;
    f32 dir;
    f32 t;

    if (!(p->dispFlags & 0x8000)) {
        return 1;
    }
    frames = sysMsec2Frame(*(s32*)((u8*)p + 0x298));
    if (frames > 0) {
        dir = revise360(270.0f - *(f32*)((u8*)p + 0x294));
        t = (dir - *(f32*)((u8*)p + 0x290)) * ((f32)(*(s32*)((u8*)p + 0x29C)) / (f32)frames);
        p->unk1A8 = revise360(*(f32*)((u8*)p + 0x290) + t);
        *(f32*)((u8*)p + 0x1AC) = p->unk1A8;
        p->directionView = toMovedirSimple(p->unk1A8);
        *(f32*)((u8*)p + 0x19C) = p->directionView;
        if (++*(s32*)((u8*)p + 0x29C) > frames) {
            p->dispFlags &= ~0x8000;
            return 1;
        }
        return 0;
    } else {
        p->unk1A8 = revise360(270.0f - *(f32*)((u8*)p + 0x294));
        *(f32*)((u8*)p + 0x1AC) = p->unk1A8;
        p->directionView = toMovedirSimple(p->unk1A8);
        *(f32*)((u8*)p + 0x19C) = p->directionView;
        p->dispFlags &= ~0x8000;
        return 1;
    }
}

s32 checkOneRevolution(s32 dir) {
RubWork* w = &rubwork;
s32 idx;
s32 cur, prev, saved;
s32 i;
s32 result = 0;
#ifdef MUST_MATCH
s32* head; // retail reads the ring head through a pointer taken before the loop
#endif

    if (w->unk08 < 2) {
        return -1;
    }
    cur = w->unk3C[w->unk0C];
    saved = w->unk14;
#ifdef MUST_MATCH
    head = &w->unk0C;
#endif
    for (i = 1; i <= 20; i++) {
#ifdef MUST_MATCH
        idx = *head - i;
#else
        idx = w->unk0C - i;
#endif
        while (idx < 0) {
            idx += 20;
        }
        prev = w->unk3C[idx];
        if (cur != prev) {
            break;
        }
    }
    if (cur == prev) {
        return -1;
    }
    if (prev == -1) {
        return -1;
    }
    if (dir != 0) {
        if (cur > prev) {
            if (saved < prev || cur <= saved) {
                result = 1;
            }
        } else {
            if (cur <= saved && saved < prev) {
                result = 1;
            }
        }
    } else {
        if (cur < prev) {
            if (saved > prev || saved <= cur) {
                result = 1;
            }
        } else {
            if (cur >= saved && saved > prev) {
                result = 1;
            }
        }
    }
    return result;
}

// NONMATCHING (96.91%): identical instruction sequence but the allocator picks different
// registers for w, cur, prev, countUp, countDown and idx1/idx2 (e.g. original cur=r30/prev=r29/
// countUp=r31, ours cur=r31/prev=r30/countUp=r6). Declaration reordering did not help.
s32 chkRevolveDir(s32* left, s32* right) {
RubWork* w = &rubwork;
s32 i;
s32 j;
s32 cur, prev, diff;
s32 idx1, idx2;
s32 countUp = 0;
s32 countDown = 0;
s32 adj;

    j = 0;
    for (i = 0; i < 10; i++) {
        idx1 = w->unk0C + j;
        while (idx1 < 0) {
            idx1 += 20;
        }
        cur = w->unk3C[idx1];
        idx2 = j - 1 + w->unk0C;
        while (idx2 < 0) {
            idx2 += 20;
        }
        prev = w->unk3C[idx2];
        j--;
        if (i == 0 && cur == prev) {
            continue;
        }
        diff = cur - prev;
        if (__fabsf((f32)diff) > 180.0f) {
            adj = -360;
            if (diff > 0) {
                adj = 360;
            }
            cur += adj;
        }
        diff = cur - prev;
        if (diff > -90 && diff < 0) {
            countDown++;
        }
        if (diff > 0 && diff < 90) {
            countUp++;
        }
    }
    if (countDown == 0 && countUp >= 4) {
        w->unk20 = 0;
        *left = countDown;
        *right = countUp;
        return 0;
    }
    if (countUp == 0 && countDown >= 4) {
        w->unk20 = 1;
        *left = countDown;
        *right = countUp;
        return 1;
    }
    *left = countDown;
    *right = countUp;
    w->unk20 = -1;
    return -1;
}

void marioGetStick(f32* outAngle, f32* outMag) {
    MarioWork* p = marioGetPtr();
    s32 x;
    s32 invert;
    s32 y2;
#ifdef MUST_MATCH
    s32 t;
#endif
    f32 mag;
    f32 angle;

    if (p->flags & MARIO_FLAG_FORCED_MOVEMENT) {
        *outAngle = p->directionView;
        *outMag = p->baseSpeed;
        return;
    }
    x = *(s8*)&p->wStickDir1;
    invert = 0;
    if ((p->flags & 0x02000000) || kpaGetStageViewType() == 0) {
        invert = 1;
    }
#ifdef MUST_MATCH
    // The original picks the y value with a separate temp and copies it into r28 at the join
    // (`li r0,0` / `neg r0,r0` / `mr r28,r0`). The plain if/else below coalesces the temp into
    // r28; putting the two arms in a switch keeps them separate basic blocks, which reproduces
    // the temp. `default` must precede `case 0` for mwcc to emit the target's `cmpwi/beq/b` order.
    switch (invert) {
    default:
        t = 0;
        break;
    case 0:
        t = -((s8)p->wStickDir2);
        break;
    }
    y2 = t;
#else
    if (invert) {
        y2 = 0;
    } else {
        y2 = -((s8)p->wStickDir2);
    }
#endif
    if (yoshiGetStatus() == 2) {
        y2 = 0;
    }
    y2 = -y2;
    mag = distABf(0.0f, 0.0f, (f32)x, (f32)y2);
    if (mag >= 70.0f) {
        mag = 70.0f;
    }
    angle = angleABf(0.0f, 0.0f, (f32)x, (f32)y2);
    if (!mag) {
        angle = *(f32*)((u8*)p + 0x19C);
    }
    angle = revise360(angle + p->targetYaw);
    *outAngle = angle;
    *outMag = mag;
}

void marioUpdateKeyData(void) {
    MarioWork* p = marioGetPtr();
    u32 mask;
    u16 dirs;
    u32 old;

    p->wStickDir1 = gp->stickX[0];
    p->wStickDir2 = -gp->stickY[0];
    *(u8*)((u8*)p + 0x250) = gp->substickX[0];
    *(u8*)((u8*)p + 0x251) = -gp->substickY[0];
    *(u8*)((u8*)p + 0x252) = gp->triggerLeft[0];
    *(u8*)((u8*)p + 0x253) = gp->triggerRight[0];
    *(u8*)((u8*)p + 0x254) = 0;
    *(u8*)((u8*)p + 0x255) = 0;
    if (*(u32*)((u8*)p + 0x284) != 0) {
        mask = *(u32*)((u8*)p + 0x284);
    } else {
        mask = -1;
    }
    *(u16*)((u8*)p + 0x246) = gp->button[0] & mask;
    *(u16*)((u8*)p + 0x248) = gp->buttonNew[0] & mask;
    *(u16*)((u8*)p + 0x24A) = gp->buttonRepeat[0] & mask;
    *(u16*)((u8*)p + 0x24C) = gp->buttonUp[0] & mask;
    if (!(p->flags & 0x0E)) {
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_STICK) {
            p->wStickDir1 = 0;
            p->wStickDir2 = 0;
        }
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_SUB_STICK) {
            *(u8*)((u8*)p + 0x250) = 0;
            *(u8*)((u8*)p + 0x251) = 0;
        }
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_TRIGGERS) {
            *(u8*)((u8*)p + 0x252) = 0;
            *(u8*)((u8*)p + 0x253) = 0;
        }
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_UNUSED_INPUT) {
            *(u8*)((u8*)p + 0x254) = 0;
            *(u8*)((u8*)p + 0x255) = 0;
        }
    }
    if ((p->flags & 0x0E) || !marioChkKey()) {
        p->wStickDir1 = 0;
        p->wStickDir2 = 0;
        *(u8*)((u8*)p + 0x250) = 0;
        *(u8*)((u8*)p + 0x251) = 0;
        *(u8*)((u8*)p + 0x252) = 0;
        *(u8*)((u8*)p + 0x253) = 0;
        *(u8*)((u8*)p + 0x254) = 0;
        *(u8*)((u8*)p + 0x255) = 0;
        if (*(u32*)((u8*)p + 0x284) == 0) {
            *(u16*)((u8*)p + 0x246) = 0;
            *(u16*)((u8*)p + 0x248) = 0;
            *(u16*)((u8*)p + 0x24A) = 0;
            *(u16*)((u8*)p + 0x24C) = 0;
        }
        if (marioGetMotSlitCancel3() == 0 && (p->dispFlags & 0x01000000)) {
            *(u16*)((u8*)p + 0x246) = 0x20;
        }
    }
    dirs = DemoPad[0].dirs;
    old = *(u32*)((u8*)p + 0x258);
    *(u32*)((u8*)p + 0x25C) = dirs & (dirs ^ old);
    *(u32*)((u8*)p + 0x258) = dirs;
}

#define RUB_SET3C(v) do { \
    rubwork.unk3C[0] = (v); rubwork.unk3C[1] = (v); rubwork.unk3C[2] = (v); rubwork.unk3C[3] = (v); \
    rubwork.unk3C[4] = (v); rubwork.unk3C[5] = (v); rubwork.unk3C[6] = (v); rubwork.unk3C[7] = (v); \
    rubwork.unk3C[8] = (v); rubwork.unk3C[9] = (v); rubwork.unk3C[10] = (v); rubwork.unk3C[11] = (v); \
    rubwork.unk3C[12] = (v); rubwork.unk3C[13] = (v); rubwork.unk3C[14] = (v); rubwork.unk3C[15] = (v); \
    rubwork.unk3C[16] = (v); rubwork.unk3C[17] = (v); rubwork.unk3C[18] = (v); rubwork.unk3C[19] = (v); \
    rubwork.unk3C[20] = (v); \
} while (0)

// NONMATCHING (94.49%): stick part now matches except the forced-move path keeps angle/mag in
// f1/f31 instead of f0/f2, and the RUB_SET3C expansion + later `unk3C`/`unk14` handling differ in
// base pointer/register choice.
s32 marioGetRub(u32 mask, s32* out1, s32* out2, f32* out3) {
    MarioWork* p;
    MarioWork* wp;
    s32 x;
    s32 invert;
    s32 y2;
    f32 mag;
    f32 angle;
    s32 chk;
    s32 result = 0;
    s32 r28;
    s32 v14;
    s32 idx;
    s32 local1;
    s32 local2;
    RubWork* w;

    p = marioGetPtr();
    w = &rubwork;
    if (mask != 0 && (*(u16*)((u8*)p + 0x246) & mask) == 0) {
        memset(&rubwork, 0, 0x68);
        rubwork.unk20 = -1;
        *out1 = -1;
        *out2 = 0;
        *out3 = 0.0f;
        return -1;
    }
    wp = marioGetPtr();
    if (wp->flags & MARIO_FLAG_FORCED_MOVEMENT) {
        angle = wp->directionView;
        mag = wp->baseSpeed;
    } else {
        x = *(s8*)&wp->wStickDir1;
        invert = 0;
        if ((wp->flags & 0x02000000) || kpaGetStageViewType() == 0) {
            invert = 1;
        }
        if (invert) {
            y2 = 0;
        } else {
            y2 = -((s8)wp->wStickDir2);
        }
        if (yoshiGetStatus() == 2) {
            y2 = 0;
        }
        y2 = -y2;
        mag = distABf(0.0f, 0.0f, (f32)x, (f32)y2);
        if (mag >= 70.0f) {
            mag = 70.0f;
        }
        angle = angleABf(0.0f, 0.0f, (f32)x, (f32)y2);
        if (!mag) {
            angle = *(f32*)((u8*)wp + 0x19C);
        }
        angle = revise360(angle + wp->targetYaw);
    }
    r28 = (s32)angle;
    if (mag < 50.0f) {
        if (++w->unk24 > 10) {
            RUB_SET3C(r28);
            w->unk08 = 0;
        }
        return -1;
    }
    w->unk24 = 0;
    w->unk3C[w->unk0C] = r28;
    idx = w->unk0C + 1;
    while (idx < 0) {
        idx += 20;
    }
    w->unk3C[idx] = -1;
    chk = chkRevolveDir(&local1, &local2);
    if (chk == 0) {
        if (w->unk10 == 2) {
            w->unk08 = 0;
            w->unk04 = 0;
        }
        if (w->unk08 == 0) {
            w->unk14 = r28;
            RUB_SET3C(r28);
        }
        w->unk1C = 0;
        w->unk08++;
        if (checkOneRevolution(0) == 1) {
            result = 1;
            v14 = w->unk14;
            RUB_SET3C(v14);
            w->unk04++;
        }
        w->unk10 = 1;
    }
    if (chk == 0) {
        *out1 = w->unk10 - 1;
        *out2 = w->unk04;
        w->unk0C++;
        if (w->unk0C >= 20) {
            w->unk0C -= 20;
        }
        return result;
    }
    if (chk == 1) {
        if (w->unk10 == 1) {
            w->unk08 = 0;
            w->unk04 = 0;
        }
        if (w->unk08 == 0) {
            w->unk14 = r28;
            RUB_SET3C(r28);
        }
        w->unk1C = 0;
        w->unk08++;
        if (checkOneRevolution(1) == 1) {
            result = 1;
            v14 = w->unk14;
            RUB_SET3C(v14);
            w->unk04++;
        }
        w->unk10 = 2;
    }
    if (chk == 1) {
        *out1 = w->unk10 - 1;
        *out2 = w->unk04;
        w->unk0C++;
        if (w->unk0C >= 20) {
            w->unk0C -= 20;
        }
        return result;
    }
    w->unk0C++;
    if (w->unk0C >= 20) {
        w->unk0C -= 20;
    }
    return -1;
}
