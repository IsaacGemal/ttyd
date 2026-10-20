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


#define RUB_SET3C(v) do { \
    rubwork.unk3C[0] = (v); rubwork.unk3C[1] = (v); rubwork.unk3C[2] = (v); rubwork.unk3C[3] = (v); \
    rubwork.unk3C[4] = (v); rubwork.unk3C[5] = (v); rubwork.unk3C[6] = (v); rubwork.unk3C[7] = (v); \
    rubwork.unk3C[8] = (v); rubwork.unk3C[9] = (v); rubwork.unk3C[10] = (v); rubwork.unk3C[11] = (v); \
    rubwork.unk3C[12] = (v); rubwork.unk3C[13] = (v); rubwork.unk3C[14] = (v); rubwork.unk3C[15] = (v); \
    rubwork.unk3C[16] = (v); rubwork.unk3C[17] = (v); rubwork.unk3C[18] = (v); rubwork.unk3C[19] = (v); \
    rubwork.unk3C[20] = (v); \
} while (0)

// Folding the `w->unk0C` head load and the wrap-around loop into this helper makes MWCC give the
// hoisted head load r10 and the idx2/diff temp r9, as the original does; expanded inline at each
// use the two registers swap (99.31%). It computes the same ring index, so behaviour is unchanged.
static inline s32 ringIdx(RubWork* p, s32 off) {
    s32 idx = p->unk0C + off;

    while (idx < 0) {
        idx += 20;
    }
    return idx;
}

static inline void rubStick(MarioWork* p, f32* outAngle, f32* outMag) {
    s32 x;
    s32 invert;
    s32 y2;
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
    if (invert) {
        y2 = 0;
    } else {
        y2 = -((s8)p->wStickDir2);
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
        angle = *(f32*)p->unk19C;
    }
    angle = revise360(angle + p->targetYaw);
    *outAngle = angle;
    *outMag = mag;
}

// Match-only: the target orders the unk04/unk08 updates after the RUB_SET3C stores; with plain stores
// MWCC hoists them into the store block (marioGetRub was 97.57%).
static inline void rubSet3C(s32 v) {
#ifdef MUST_MATCH
    *(volatile s16*)&rubwork.unk3C[0] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[1] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[2] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[3] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[4] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[5] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[6] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[7] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[8] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[9] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[10] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[11] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[12] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[13] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[14] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[15] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[16] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[17] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[18] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[19] = (s16)v;
    *(volatile s16*)&rubwork.unk3C[20] = (s16)v;
#else
    RUB_SET3C(v);
#endif
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

void N_marioSetBottomlessRespawnPosOnBeroEntry(f32 x, f32 y, f32 z) {
    MarioWork* p = marioGetPtr();
    Vec position = (Vec){ 0.0f, 0.0f, 0.0f };

    position.x = x;
    position.y = y + 37.0f;
    position.z = z;
    p->bottomlessResetPosition = position;
}

void N_marioSetBottomlessRespawnPos(f32 x, f32 y, f32 z) {
    MarioWork* p = marioGetPtr();
    Vec position = (Vec){ 0.0f, 0.0f, 0.0f };

    position.x = x;
    position.y = y;
    position.z = z;
    p->bottomlessResetPosition = position;
}

void N_marioReloadMapOnBottomlessOn(void) {
    MarioWork* p = marioGetPtr();
    *(s32*)((u8*)p + 0x2C8) = 1;
}

void N_marioReloadMapOnBottomlessOff(void) {
    MarioWork* p = marioGetPtr();
    *(s32*)((u8*)p + 0x2C8) = 0;
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
        p->dispReqDir = p->unk1A8;
        p->directionView = toMovedirSimple(p->unk1A8);
        *(f32*)((u8*)p + 0x19C) = p->directionView;
        if (++*(s32*)((u8*)p + 0x29C) > frames) {
            p->dispFlags &= ~0x8000;
            return 1;
        }
        return 0;
    } else {
        p->unk1A8 = revise360(270.0f - *(f32*)((u8*)p + 0x294));
        p->dispReqDir = p->unk1A8;
        p->directionView = toMovedirSimple(p->unk1A8);
        *(f32*)((u8*)p + 0x19C) = p->directionView;
        p->dispFlags &= ~0x8000;
        return 1;
    }
}

f32 marioGetMoveRate(f32 angle) {
    f32 diff = angle - marioGetPtr()->targetYaw;
    f32 c = (f32)cos(3.1415927f * diff / 180.0f);
    return 0.19000006f * __fabsf(c) + 1.0f;
}

void marioResetGravity(void) {
    marioGetPtr()->playerGravity = 1.0f;
}

void* marioGetTouchNpcPtr(void) {
    return *(void**)((u8*)marioGetPtr() + 0x1D4);
}

BOOL marioChkPlanePlace(HitEntry* hit) {
    return (hitGetAttr(hit) & 0x38) != 0;
}

void marioSetPeakYpos(void) {
    MarioWork* p = marioGetPtr();
    *(f32*)((u8*)p + 0x12C) = p->position.y;
}

void marioAdjustMoveDir(void) {
    MarioWork* p = marioGetPtr();

    if (p->unk1A8 >= 90.0f && p->unk1A8 <= 270.0f) {
        *(f32*)((u8*)p + 0x19C) = toMovedir(90.0f);
        p->directionView = *(f32*)((u8*)p + 0x19C);
        p->unk1A8 = 180.0f;
        p->dispReqDir = 180.0f;
    } else {
        *(f32*)((u8*)p + 0x19C) = toMovedir(90.0f);
        p->directionView = *(f32*)((u8*)p + 0x19C);
        p->unk1A8 = 0.0f;
        p->dispReqDir = 0.0f;
    }
}

f32 marioGetDispDir(void) {
    MarioWork* p = marioGetPtr();
    f32 diff = p->unk1A8 - 90.0f;
    return revise360(diff + p->targetYaw);
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

f32 partyToMovedir(PartyEntry* party, f32 dir) {
    f32 a;
    MarioWork* player = party->playerPtr;

    if (party->flags & PARTY_FLAG_PAPER_MODE) {
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

void marioUpdateKeyData(void) {
    MarioWork* p = marioGetPtr();
    u32 mask;
    u16 dirs;
    u32 old;

    p->wStickDir1 = gp->stickX[0];
    p->wStickDir2 = -gp->stickY[0];
    p->substickX = gp->substickX[0];
    p->substickY = -gp->substickY[0];
    p->triggerLeft = gp->triggerLeft[0];
    p->triggerRight = gp->triggerRight[0];
    p->unusedInput[0] = 0;
    p->unusedInput[1] = 0;
    if (*(u32*)((u8*)p + 0x284) != 0) {
        mask = *(u32*)((u8*)p + 0x284);
    } else {
        mask = -1;
    }
    p->buttonHeld = gp->button[0] & mask;
    p->buttonNew = gp->buttonNew[0] & mask;
    p->buttonRepeat = gp->buttonRepeat[0] & mask;
    p->buttonUp = gp->buttonUp[0] & mask;
    if (!(p->flags & 0x0E)) {
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_STICK) {
            p->wStickDir1 = 0;
            p->wStickDir2 = 0;
        }
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_SUB_STICK) {
            p->substickX = 0;
            p->substickY = 0;
        }
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_TRIGGERS) {
            p->triggerLeft = 0;
            p->triggerRight = 0;
        }
        if (p->trigFlags & MARIO_TRIG_FLAG_DISABLE_UNUSED_INPUT) {
            p->unusedInput[0] = 0;
            p->unusedInput[1] = 0;
        }
    }
    if ((p->flags & 0x0E) || !marioChkKey()) {
        p->wStickDir1 = 0;
        p->wStickDir2 = 0;
        p->substickX = 0;
        p->substickY = 0;
        p->triggerLeft = 0;
        p->triggerRight = 0;
        p->unusedInput[0] = 0;
        p->unusedInput[1] = 0;
        if (*(u32*)((u8*)p + 0x284) == 0) {
            p->buttonHeld = 0;
            p->buttonNew = 0;
            p->buttonRepeat = 0;
            p->buttonUp = 0;
        }
        if (marioGetMotSlitCancel3() == 0 && (p->dispFlags & 0x01000000)) {
            p->buttonHeld = 0x20;
        }
    }
    dirs = DemoPad[0].dirs;
    old = p->padDirs;
    p->padDirsPressed = dirs & (dirs ^ old);
    p->padDirs = dirs;
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

void marioGetRubInit(void) {
    memset(&rubwork, 0, 0x68);
    rubwork.unk20 = -1;
}

s32 chkRevolveDir(s32* left, s32* right) {
s32 adj;
s32 diff;
s32 i;
s32 j;
s32 countDown;
s32 idx2;
s32 cur;
s32 countUp;
s32 idx1;
s32 prev;
RubWork* w;

    w = &rubwork;
    j = 0;
    countDown = 0;
    countUp = 0;
    for (i = 0; i < 10; i++) {
        idx1 = ringIdx(w, j);
        cur = w->unk3C[idx1];
        idx2 = ringIdx(w, j - 1);
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

// 100%. The unk04/unk08 advances must stay after the 21-store rubSet3C block; MWCC's scheduler
// otherwise hoists the load into the block. rubSet3C's MUST_MATCH form stores through volatile and the
// advances are volatile read-modify-writes, which pins the order; the #else form is the natural code.
s32 marioGetRub(u32 mask, s32* out1, s32* out2, f32* out3) {
    MarioWork* p;
    MarioWork* wp;
    f32 angle;
    f32 mag;
    s32 chk;
    s32 result = 0;
    s32 r28;
    s32 idx;
    s32 local1;
    s32 local2;
    RubWork* w;

    p = marioGetPtr();
    w = &rubwork;
    if (mask != 0 && (p->buttonHeld & mask) == 0) {
        memset(&rubwork, 0, 0x68);
        rubwork.unk20 = -1;
        *out1 = -1;
        *out2 = 0;
        *out3 = 0.0f;
        return -1;
    }
    wp = marioGetPtr();
    rubStick(wp, &angle, &mag);
    r28 = (s32)angle;
    if (mag < 50.0f) {
        if (++w->unk24 > 10) {
#ifdef MUST_MATCH
            rubSet3C(r28);
            *(volatile s32*)&w->unk08 = 0;
#else
            rubSet3C(r28);
            w->unk08 = 0;
#endif
        }
        return -1;
    }
    w->unk24 = 0;
    w->unk3C[w->unk0C] = r28;
    idx = rubwork.unk0C;
    idx = idx + 1;
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
            rubSet3C(r28);
        }
        w->unk1C = 0;
        w->unk08++;
        if (checkOneRevolution(0) == 1) {
#ifdef MUST_MATCH
            // volatile so MWCC keeps the unk04 read-modify-write after the rubSet3C stores.
            result = 1;
            rubSet3C(w->unk14);
            *(volatile s32*)&w->unk04 = *(volatile s32*)&w->unk04 + 1;
#else
            result = 1;
            rubSet3C(w->unk14);
            w->unk04++;
#endif
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
            rubSet3C(r28);
        }
        w->unk1C = 0;
        w->unk08++;
        if (checkOneRevolution(1) == 1) {
#ifdef MUST_MATCH
            // volatile so MWCC keeps the unk04 read-modify-write after the rubSet3C stores.
            result = 1;
            rubSet3C(w->unk14);
            *(volatile s32*)&w->unk04 = *(volatile s32*)&w->unk04 + 1;
#else
            result = 1;
            rubSet3C(w->unk14);
            w->unk04++;
#endif
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

// Not in this version of the game.
#ifndef MUST_MATCH
void marioSetBottomlessResetPosition(f32 x, f32 y, f32 z) {
    MarioWork* wp = marioGetPtr();
    Vec position;

    position = (Vec){ x, y + 37.0f, z };
    wp->bottomlessResetPosition = position;
}
#endif
