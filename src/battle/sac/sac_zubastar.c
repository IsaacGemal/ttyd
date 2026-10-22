#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_audience.h"
#include "battle/battle_database_common.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "eff/eff_starstone.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "matching.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>
#include <string.h>
#include "battle/battle_camera.h"
#include "battle/sac/sac_common.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "party/party_motion.h"
#include "battle/sac/sac_zubastar.h"

extern BattleWork* _battleWorkPointer;

#pragma pool_data on // the original addresses its .rodata through one base register

#pragma force_active on // the original keeps these .rodata objects; nothing in the code references
                       // them yet, so the linker would otherwise drop them
const char str_btl_wn_sac_zubastar[] = "btl_wn_sac_zubastar";
const char str_msg_zubastar[] = "msg_zubastar";
const char str_msg_sac_8[] = "msg_sac_8";
#pragma force_active reset

extern const char str_SFX_BTL_SAC_ZUBA_STAR3[];

// LOCAL_PROTO(psndSFX_pit): the retail callers disagree on the pitch width and
// signedness; this file passes a full 32-bit signed pitch.
s32 psndSFX_pit(s32 id, s32 pit);

// The original emits this as a local .sdata script (nice_event, local).
static EVT_BEGIN(nice_event)
    CALL(btlevtcmd_ACSuccessEffect, LW(0), -120, 140, 0)
    RETURN
EVT_END()

typedef struct ZubaStarBomb {
    s32 state;       // 0x00
    s32 count;       // 0x04
    Vec* points;     // 0x08
    Vec pos;         // 0x0C
    Vec center;      // 0x18
    Vec from;        // 0x24
    Vec to;          // 0x30
    f32 angle;       // 0x3C
    f32 angleVel;    // 0x40
    f32 velY;        // 0x44
    s32 timer;       // 0x48
    s32 maxTimer;    // 0x4C
} ZubaStarBomb;

typedef struct ZubaStarStar {
    s32 state;     // 0x00
    s32 field_0x04; // 0x04
    s32 timer;     // 0x08
    f32 x0;        // 0x0C
    f32 y0;        // 0x10
    f32 x1;        // 0x14
    f32 y1;        // 0x18
} ZubaStarStar;

struct ZubaStarWork {
    u8 field_0x000[0x04];          // 0x000
    s32 phase;                     // 0x004
    s32 phaseTimer;                // 0x008
    s32 timeLeft;                  // 0x00C
    s32 hitCount;                  // 0x010
    s32 bombCount;                 // 0x014
    s32 sequence;                  // 0x018
    s32 sequenceTimer;             // 0x01C
    Vec cursor;                    // 0x020
    Vec cursorHistory[4];          // 0x02C
    Vec cutStart;                  // 0x05C
    Vec cutEnd;                    // 0x068
    s32 cutFrame;                  // 0x074
    s16 curStar;                   // 0x078
    u8 field_0x07A[0x02];          // 0x07A
    s32 gaugePhase;                // 0x07C
    s32 gaugeTimer;                // 0x080
    f32 gauge;                     // 0x084
    f32 gaugeBoost;                // 0x088
    f32 gaugeDecay;                // 0x08C
    f32 field_0x090;               // 0x090
    Vec gaugePos;                  // 0x094
    u8 field_0x0A0;                // 0x0A0
    u8 iconFrame;                  // 0x0A1
    u8 field_0x0A2[0x02];          // 0x0A2
    s32 gaugeSfx;                  // 0x0A4
    ZubaStarBomb* bombs[0xC8];     // 0x0A8
    ZubaStarStar stars[0x10];      // 0x3C8
    smartEntry* tex;               // 0x588
    s32 starState;                 // 0x58C
    s32 starCounter;               // 0x590
    Vec starCur;                   // 0x594
    Vec starStart;                 // 0x5A0
    Vec starEnd;                   // 0x5AC
    Vec field_0x5B8;               // 0x5B8
    Vec scale1;                    // 0x5C4
    Vec scale2;                    // 0x5D0
    EffectEntry* effect;           // 0x5DC
};

// .text:0x23FC | 0x8023098C | size: 0x2C
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();

    work->starState = 1;
    return 2;
}

// .text:0x23C4 | 0x80230954 | size: 0x38
s32 wait_star_stone_up(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();

    return (work->starState == 4) ? 2 : 0;
}

static s32 main_star(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 scale;

    switch (work->starState) {
    case 0:
        break;
    case 1:
        {
            f32 zero = 0.0f;

            work->starState = 2;
            {
                f32 neg = -1000.0f;

                work->starCounter = 0;
                {
                    f32 one = 1.0f;

                    work->effect = effStarStoneEntry(6, zero, neg, zero, one);
                }
            }
        }
        BtlUnit_GetPos(mario, &work->starCur.x, &work->starCur.y, &work->starCur.z);
        work->starCur.y += mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f;
        work->starStart = work->starCur;
        work->starEnd = work->starCur;
        work->starEnd.y += 50.0f;
        work->starEnd.z += -1.0f;
        work->scale1 = (Vec){ 0.0f, 0.0f, 0.0f };
        work->scale2 = (Vec){ 0.0f, 0.0f, 0.0f };
        // fallthrough
    case 2:
        work->starCounter++;
        if (work->starCounter <= 0x64) {
            work->starCur.x = intplGetValue(0, work->starCounter, 0x64, work->starStart.x, work->starEnd.x);
            work->starCur.y = intplGetValue(0, work->starCounter, 0x64, work->starStart.y, work->starEnd.y);
            work->starCur.z = intplGetValue(0, work->starCounter, 0x64, work->starStart.z, work->starEnd.z);
            scale = intplGetValue(0, work->starCounter, 0x64, 0.0f, 2.0f);
            work->scale1.z = scale;
            work->scale1.y = scale;
            work->scale1.x = scale;
        } else {
            f32 two = 2.0f;

            work->starCur.x = work->starEnd.x;
            work->starCur.y = work->starEnd.y;
            work->starCur.z = work->starEnd.z;
            work->scale1.z = two;
            work->scale1.y = two;
            work->scale1.x = two;
        }
        work->scale2.y = intplGetValue(4, work->starCounter, 0x78, 0.0f, 2160.0f);
        if (work->starCounter >= 0x78) {
            work->starState = 3;
            work->starCounter = 0;
            work->starStart = work->starCur;
            work->starEnd.y = 300.0f;
        }
        break;
    case 3:
        work->starCounter++;
        work->starCur.y = intplGetValue(1, work->starCounter, 0x3C, work->starStart.y, work->starEnd.y);
        if (work->starCounter >= 0x3C) {
            work->starState = 4;
            work->starCounter = 0;
        }
        break;
    case 4:
        break;
    }
    if (work->effect != NULL) {
        effStarStoneData* data = work->effect->userdata;
        f32 s = 1.5f;

        data->x = work->starCur.x;
        data->y = work->starCur.y;
        data->z = work->starCur.z;
        data->tipRotY = work->scale2.x;
        data->rotY = work->scale2.y;
        data->tipRotZ = work->scale2.z;
        data->scale = s * work->scale1.x;
    }
    dispEntry(4, 0.0f, 2, zubaStarDispStar, NULL);
    return 0;
}

// .text:0x2084 | 0x80230614 | size: 0x4
// The original body is a no-op; this exists only as a dispEntry callback.
void zubaStarDispStar(CameraId cameraId, void* param) {
    (void)cameraId;
    (void)param;
}

// .text:0x2078 | 0x80230608 | size: 0xC
ZubaStarWork* GetZubaStarPtr(void) {
    return (ZubaStarWork*)&_battleWorkPointer->actionCommands.field_0x2BC[0];
}

// .text:0x203C | 0x802305CC | size: 0x3C
u32 weaponGetPower_ZubaStar(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2,
                            BattleUnitPart* part) {
    ZubaStarWork* work = GetZubaStarPtr();

    return weapon->hpDamageWork[work->hitCount];
}

// .text:0x1F58 | 0x802304E8 | size: 0xE4
void zubastar_create_takaku(s32 count, Vec* points) {
    ZubaStarWork* work = GetZubaStarPtr();
    ZubaStarBomb* bomb;
    s32 i;

    work->bombs[work->bombCount] = BattleAlloc(0x50);
    bomb = work->bombs[work->bombCount];
    bomb->points = BattleAlloc(count * 0xC);
    bomb->state = 0;
    bomb->count = count;
    bomb->pos = (Vec){ 0.0f, 0.0f, 0.0f };
    bomb->timer = 0;
    bomb->maxTimer = 1;
    for (i = 0; i < count; i++) {
        bomb->points[i] = points[i];
    }
    work->bombCount++;
}

// .text:0x1A90 | 0x80230020 | size: 0x4C8
// Shared copy counters keep the prefix and suffix offsets in the same live ranges.
void zubastar_bunkatu(Vec* a, Vec* b) {
    s32 used[2];
    s32 index[2];
    Vec point[2];
    f32 out1;
    f32 out2;
    ZubaStarWork* work;
    ZubaStarBomb* bomb;
    s32 i;
    s32 j;
    s32 n;
    ZubaStarBomb* bomb1;
    ZubaStarBomb* bomb2;
    s32 k;
    s32 j1;
    s32 m;
    s32 bombCount;
    work = GetZubaStarPtr();
    bombCount = work->bombCount;

    for (i = 0; i < bombCount; i++) {
        bomb = work->bombs[i];

        n = 0;
        for (k = 0; k < 2; k++) {
            used[k] = 0;
        }
        for (j = 0; j < bomb->count; j++) {
            if (n == 2) {
                break;
            }
            if (zubastar_get_kouten(a->x, a->y, b->x, b->y, bomb->points[j].x, bomb->points[j].y,
                                    bomb->points[(j + 1) % bomb->count].x,
                                    bomb->points[(j + 1) % bomb->count].y, &out1, &out2)) {
                for (k = 0; k < n; k++) {
                    if (used[k] == 1 && (s32)point[k].x == (s32)out1 && (s32)point[k].y == (s32)out2) {
                        break;
                    }
                }
                if (k == n) {
                    point[k].x = out1;
                    point[k].y = out2;
                    point[k].z = 0.0f;
                    used[k] = 1;
                    index[k] = j;
                    n++;
                }
            }
        }
        if (n == 2) {
            bomb1 = BattleAlloc(0x50);
            bomb2 = BattleAlloc(0x50);
            for (j = 0; j < bomb->count; j++) {
                if (j == index[0]) {
                    bomb1->count = bomb->count - index[1] + 2 + j;
                    bomb2->count = index[1] - index[0] + 2;
                }
            }
            bomb1->points = BattleAlloc(bomb1->count * 0xC);
            bomb2->points = BattleAlloc(bomb2->count * 0xC);
            for (k = 0, j = 0; j <= index[0]; j++, k++) {
                bomb1->points[k] = bomb->points[j];
            }
            j1 = k + 1;
            bomb1->points[k] = point[0];
            bomb1->points[j1] = point[1];
            k += 2;
            for (j = index[1] + 1; j < bomb->count; j++, k++) {
                bomb1->points[k] = bomb->points[j];
            }
            for (j = 0; j < bomb->count; j++) {
                if (j == index[0]) {
                    bomb2->points[0] = point[0];
                    for (m = 1; m <= bomb2->count - 2; m++) {
                        bomb2->points[m] = bomb->points[j + m];
                    }
                    bomb2->points[bomb2->count - 1] = point[1];
                }
            }
            bomb1->state = 0;
            bomb1->pos = (Vec){ 0.0f, 0.0f, 0.0f };
            bomb1->timer = irand(0x80);
            bomb1->maxTimer = irand(5) + 1;
            bomb2->state = 0;
            bomb2->pos = (Vec){ 0.0f, 0.0f, 0.0f };
            bomb2->timer = irand(0x80);
            bomb2->maxTimer = irand(5) + 1;

            BattleFree(work->bombs[i]->points);
            BattleFree(work->bombs[i]);
            work->bombs[i] = bomb1;
            work->bombs[work->bombCount] = bomb2;
            work->bombCount++;
        }
    }
}

// .text:0x1990 | 0x8022FF20 | size: 0x100
s32 zubastar_get_kouten(f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32* out1,
                        f32* out2) {
    f32 dx1 = f3 - f1;
    f32 dx2;
    f32 det;
    f32 w1;
    f32 w2;
    f32 slope1;
    f32 slope2;
    f32 t;

    if (dx1 == 0.0f && f7 - f5 == 0.0f) {
        return 0;
    }
    if (dx1 != 0.0f) {
        w1 = dx1;
    } else {
        w1 = 0.00001f;
    }
    dx2 = f7 - f5;
    if (dx2 != 0.0f) {
        w2 = dx2;
    } else {
        w2 = 0.00001f;
    }
    slope1 = (f4 - f2) / w1;
    slope2 = (f8 - f6) / w2;
    det = slope1 - slope2;
    if (det == 0.0f) {
        return 0;
    }
    *out1 = ((f6 - f2) + (slope1 * f1 - slope2 * f5)) / det;
    if (dx1 != 0.0f) {
        *out2 = slope1 * (*out1 - f1) + f2;
    } else {
        *out2 = slope2 * (*out1 - f5) + f6;
    }
    t = (*out1 - f5) / w2;
    if (t >= 0.0f && t <= 1.0f) {
        return 1;
    }
    return 0;
}

// .text:0x85C | 0x8022F128 | size: 0xDF8
// Reconstruction of the SAC mini-game's main event callback. There are three state machines, each a
// switch on work->phase, work->sequence and work->gaugePhase. Case 0x19 of the first
// gathers the star endpoints, sorts them, finds the widest gap on each edge and randomly places a
// split point in it before calling zubastar_bunkatu.
s32 zubastar_main(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    Vec points[4] = {
        { 0.0f, 240.0f, 0.0f },
        { 304.0f, 240.0f, 0.0f },
        { 304.0f, -240.0f, 0.0f },
        { 0.0f, -240.0f, 0.0f },
    };
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 i;
    s32 j;

    (void)event;
    if (isFirstCall) {
        work->tex = smartAlloc(GXGetTexBufferSize(0x190, 0x1E0, 4, 0, 0), 1);
        points[0] = (Vec){ -96.0f, 240.0f, 0.0f };
        points[1] = (Vec){ 304.0f, 240.0f, 0.0f };
        points[2] = (Vec){ 304.0f, -240.0f, 0.0f };
        points[3] = (Vec){ -96.0f, -240.0f, 0.0f };
        zubastar_create_takaku(4, points);
        work->cursor = (Vec){ 0.0f, -1000.0f, 0.0f };
    }

    switch (work->phase) {
    case 0:
        work->phase = 5;
        work->sequence = 5;
        break;
    case 5:
        work->phase = 6;
        work->phaseTimer = 0;
        if (work->gaugePhase == 0) {
            work->gaugePhase = 5;
        }
        if (work->gaugePhase == 0xF) {
            work->gaugePhase = 0x10;
        }
        // fallthrough
    case 6:
        if (work->sequence == 0xA && work->gaugePhase == 0xA) {
            work->phase = 0xA;
        }
        break;
    case 0xA:
        work->phase = 0xB;
        work->timeLeft = 0x12C;
        // fallthrough
    case 0xB:
        if (work->gauge >= 100.0f) {
            EventEntry* entry;

            work->phase = 0x14;
            work->hitCount++;
            entry = evtEntry(nice_event, 0, 0x20);
            {
                s32 n = work->hitCount + 1;

                entry->lwData[0] = n < 6 ? n : 6;
            }
            psndSFXOff(work->gaugeSfx);
        } else {
            work->timeLeft--;
            if (work->timeLeft <= 0) {
                work->phase = 0xF;
                psndSFXOff(work->gaugeSfx);
            } else if (keyGetButtonTrg(0) & 0x100) {
                work->gaugeBoost = 2.8f;
            }
        }
        break;
    case 0xF:
        work->gaugePhase = 0x14;
        psndSFXOff(work->gaugeSfx);
        work->phase = 0x1E;
        break;
    case 0x14:
        work->gaugePhase = 0xF;
        work->gaugeTimer = 0;
        psndSFXOff(work->gaugeSfx);
        if (audienceNum >= 1 && audienceNum <= 0x31) {
            BattleAudienceSoundCallKind(1);
        }
        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
            BattleAudienceSoundCallKind(1);
            BattleAudienceSoundWhistleKind(1);
        }
        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundWhistleKind(2);
        }
        if (audienceNum >= 0x96) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundWhistleKind(3);
        }
        work->phase = 0x15;
        work->sequence = 0xF;
        // fallthrough
    case 0x15:
        if (work->sequence == 0x14) {
            work->phase = 0x19;
            work->phaseTimer = 0;
        }
        break;
    case 0x19: {
        f32 x[18];
        f32 y[18];
        f32 dx[17];
        f32 dy[17];
        s32 n;

        work->phase = 0x1A;
        work->sequence = 0x19;
        if (irand(2) == 0) {
            work->cutStart = (Vec){ 0.0f, 240.0f, 0.0f };
            work->cutEnd = (Vec){ 0.0f, -240.0f, 0.0f };
        } else {
            work->cutStart = (Vec){ 0.0f, -240.0f, 0.0f };
            work->cutEnd = (Vec){ 0.0f, 240.0f, 0.0f };
        }
        y[0] = -96.0f;
        x[0] = -96.0f;
        i = 0;
        n = 1;
        for (; i < 0x11; i++) {
            ZubaStarStar* star = &work->stars[i];

            if (star->state == 5) {
                if (star->y0 == 240.0f) {
                    x[n] = star->x0;
                    y[n] = star->x1;
                } else {
                    x[n] = star->x1;
                    y[n] = star->x0;
                }
                n++;
            }
        }
        x[n] = 344.0f;
        y[n] = 264.0f;
        for (i = 0; i < n; i++) {
            for (j = i + 1; j < n + 1; j++) {
                if (x[i] > x[j]) {
                    f32 t = x[i];
                    x[i] = x[j];
                    x[j] = t;
                }
                if (y[i] > y[j]) {
                    f32 t = y[i];
                    y[i] = y[j];
                    y[j] = t;
                }
            }
        }
        for (i = 0; i < n; i++) {
            dx[i] = x[i + 1] - x[i];
            dy[i] = y[i + 1] - y[i];
        }
        {
            s32 ix = 0;
            s32 iy = 0;
            f32 mx;
            f32 my;

            my = 0.0f;
            mx = my;
            for (i = 0, iy = 0, ix = 0; i < n; i++) {
                if (dx[i] > mx) {
                    mx = dx[i];
                    ix = i;
                }
                if (dy[i] > my) {
                    my = dy[i];
                    iy = i;
                }
            }
            {
                f32 v = x[ix] + dx[ix] * (f32)rand() / 32767.0f;

                if (work->cutStart.y == 240.0f) {
                    work->cutStart.x = v;
                } else {
                    work->cutEnd.x = v;
                }
            }
            {
                f32 v = x[iy] + dx[iy] * (f32)rand() / 32767.0f;

                if (work->cutStart.y == -240.0f) {
                    work->cutStart.x = v;
                } else {
                    work->cutEnd.x = v;
                }
            }
        }
        {
            Vec a;
            Vec b;

            b = work->cutEnd;
            a = work->cutStart;
            zubastar_bunkatu(&a, &b);
        }
        // fallthrough
    }
    case 0x1A:
        if (work->sequence == 0x1E) {
            work->phaseTimer++;
            if (work->phaseTimer >= 3) {
                work->phase = 0x21;
            } else {
                work->phase = 0x19;
            }
        }
        break;
    case 0x1E:
        work->phase = 0x1F;
        work->sequence = 0xF;
        // fallthrough
    case 0x1F:
        if (work->sequence == 0x14) {
            work->phase = 0x23;
        }
        break;
    case 0x21:
        if (work->hitCount >= 5) {
            work->phase = 0x23;
        } else {
            work->phase = 0;
        }
        break;
    case 0x23:
        work->phase = 0x24;
        work->sequence = 0x19;
        work->cutStart = (Vec){ -96.0f, 240.0f, 0.0f };
        work->cutEnd = (Vec){ -96.0f, -240.0f, 0.0f };
        break;
    case 0x24:
        if (work->sequence == 0x1E) {
            work->phase = 0x26;
            work->phaseTimer = 0x3C;
            btl_camera_shake_h(0, 0x3C, 0, 5.0f, 5.0f);
        }
        break;
    case 0x26:
        work->phaseTimer--;
        if (work->phaseTimer <= 0) {
            work->phase = 0x28;
        }
        break;
    case 0x28: {
        s32 j;

        for (j = 0; j < 0x10; j++) {
            work->stars[j].state = 0;
        }
        return 2;
    }
    default:
        break;
    }

    switch (work->sequence) {
    case 0:
        break;
    case 5:
        work->sequence = 6;
        work->sequenceTimer = 0;
        // fallthrough
    case 6:
        work->sequenceTimer++;
        work->cursor.x = 0.0f;
        work->cursor.y = intplGetValue(4, work->sequenceTimer, 0xF, 300.0f, 300.0f);
        work->cursor.z = 0.0f;
        if (work->sequenceTimer >= 0xF) {
            work->sequence = 0xA;
        }
        break;
    case 0xA:
        work->cursor = (Vec){ 0.0f, 300.0f, 0.0f };
        break;
    case 0xF:
        work->sequence = 0x10;
        work->sequenceTimer = 0;
        // fallthrough
    case 0x10:
        work->sequenceTimer++;
        work->cursor.x = 0.0f;
        work->cursor.y = intplGetValue(4, work->sequenceTimer, 0xF, 300.0f, 300.0f);
        work->cursor.z = 0.0f;
        if (work->sequenceTimer >= 0xF) {
            work->sequence = 0x14;
        }
        break;
    case 0x19:
        work->sequence = 0x1A;
        work->cutFrame = 0;
        for (i = 0; i < 0x10; i++) {
            ZubaStarStar* star = &work->stars[i];

            if (star->state == 0) {
                work->curStar = i;
                star->state = 5;
                star->field_0x04 = 0;
                {
                    f32 px = work->cutStart.x;

                    star->x1 = px;
                    star->x0 = px;
                }
                {
                    f32 py = work->cutStart.y;

                    star->y1 = py;
                    star->y0 = py;
                }
                star->timer = 0;
                break;
            }
        }
        psndSFXOn("SFX_BTL_SAC_ZUBA_STAR2");
        // fallthrough
    case 0x1A:
        work->cutFrame++;
        work->cursor.x =
            intplGetValue(0, work->cutFrame, 0xF, work->cutStart.x, work->cutEnd.x);
        work->cursor.y =
            intplGetValue(0, work->cutFrame, 0xF, work->cutStart.y, work->cutEnd.y);
        {
            ZubaStarStar* star = &work->stars[work->curStar];

            star->x1 = work->cursor.x;
            star->y1 = work->cursor.y;
        }
        if (work->cutFrame >= 0xF) {
            work->sequence = 0x1B;
            work->sequenceTimer = 0;
            btl_camera_shake_h(0, 0x1E, 0, 8.0f, 0.0f);
        }
        break;
    case 0x1B:
        work->sequenceTimer++;
        if (work->sequenceTimer >= 5) {
            work->sequence = 0x1C;
        }
        break;
    case 0x1C:
        work->sequence = 0x1E;
        // fallthrough
    case 0x1E:
        work->cursor = (Vec){ 0.0f, -1000.0f, 0.0f };
        break;
    default:
        break;
    }

    work->cursorHistory[3] = work->cursorHistory[2];
    work->cursorHistory[2] = work->cursorHistory[1];
    work->cursorHistory[1] = work->cursorHistory[0];
    work->cursorHistory[0] = work->cursor;

    switch (work->gaugePhase) {
    case 0:
        break;
    case 5:
        work->gaugePhase = 6;
        work->gaugeTimer = 0;
        work->gaugePos = (Vec){ -1000.0f, 0.0f, 0.0f };
        work->field_0x090 = 200.0f;
        work->gauge = 0.0f;
        work->gaugeBoost = 0.0f;
        work->gaugeDecay = -1.0f;
        work->field_0x0A0 = 0xFF;
        work->iconFrame = 0;
        break;
    case 6:
        work->gaugeTimer++;
        work->gaugePos.x = intplGetValue(4, work->gaugeTimer, 0x1E, -1000.0f, 50.0f);
        if (work->gaugeTimer >= 0x1E) {
            work->gaugePhase = 0xA;
        }
        break;
    case 0xA:
        work->gauge += work->gaugeBoost;
        work->gauge += work->gaugeDecay;
        if (work->gauge > 100.0f) {
            work->gauge = 100.0f;
        }
        if (work->gauge < 0.0f) {
            work->gauge = 0.0f;
        }
        work->gaugeBoost -= 0.25f;
        if (work->gaugeBoost < 0.0f) {
            work->gaugeBoost = 0.0f;
        }
        work->iconFrame++;
        if (work->iconFrame >= 8) {
            work->iconFrame = 0;
        }
        if (work->gauge <= 0.0f) {
            work->gauge = 0.0f;
            if (psndSFXChk(work->gaugeSfx) == 0) {
                psndSFXOff(work->gaugeSfx);
            }
        }
        if (work->gauge > 0.0f) {
            if (psndSFXChk(work->gaugeSfx) == -1) {
                work->gaugeSfx = psndSFXOn("SFX_BTL_SAC_ZUBA_STAR1");
            }
        }
        if (psndSFXChk(work->gaugeSfx) == 0) {
            psndSFX_pit(work->gaugeSfx,
                        (s32)(683.0f * (f32)work->hitCount +
                              4096.0f * work->gauge / 100.0f));
        }
        break;
    case 0xF:
        work->gauge = 100.0f;
        work->gaugeBoost = 0.0f;
        work->gaugeDecay = 0.0f;
        work->iconFrame = 0;
        work->gaugeTimer++;
        work->gaugePos.x = intplGetValue(4, work->gaugeTimer, 0x1E, 50.0f, -1000.0f);
        if (work->gaugeTimer >= 0x1E) {
            work->gaugeTimer = 0x1E;
        }
        break;
    case 0x10:
        work->gaugePhase = 6;
        work->gaugeTimer = 0;
        work->gauge = 0.0f;
        work->gaugeBoost = 0.0f;
        work->gaugeDecay = -0.9f - 0.1f * (f32)work->hitCount;
        break;
    case 0x14:
        work->gaugePhase = 0x15;
        work->gaugeTimer = 0;
        break;
    case 0x15:
        work->gaugeTimer++;
        work->gaugePos.x = intplGetValue(4, work->gaugeTimer, 0x1E, 50.0f, -1000.0f);
        if (work->gaugeTimer >= 0x1E) {
            work->gaugeTimer = 0x1E;
        }
        break;
    default:
        break;
    }

    {
        s32 k;

        for (k = 0; k < 0x10; k++) {
            ZubaStarStar* star = &work->stars[k];

            switch (star->state) {
            case 0:
                break;
            case 5:
                star->timer++;
                if (star->timer > 0x3C) {
                    star->timer = 0;
                }
                break;
            default:
                break;
            }
        }
    }
    dispEntry(8, 900.0f, 1, zubastar_disp2D, NULL);
    return 0;
}
#undef ZS_ROV
#undef ZS_RD

// .text:0x800 | 0x8022ED90 | size: 0x398
// The state machine matches. Every named float constant is a literal (0.0f, 104.0f, 600.0f, 200.0f,
// 70.0f, 32767.0f, 35.0f, 2.0f, -1.25f) and the two initial vectors are the compound literals
// (Vec){0,0,0} and (Vec){0,-1000,200} (the original's vec3_802ff7ac/b8 hold exactly those), so MWCC has
// the known values and recovers the schedule. Two structural details matter:
// a single-case `switch` instead of `if` for the early-out guard, which makes MWCC emit `beq body;
// b main_loop` like the original, and hoisting the SFX string into a loop-lived local to fix the
// callee-saved colouring.
s32 zubastar_bomb(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;

    if (isFirstCall) {
        dispEntry(8, 0.0f, 0, zubastar_capture, NULL);
        work->phase = 0;
        return 0;
    }
    switch (work->phase) {
    case 0: {
        s32 count = 0;

        for (i = 0; i < work->bombCount; i++) {
            if (work->bombs[i]->state != 0xA) {
                break;
            }
            count++;
        }
        if (count == work->bombCount) {
            return 2;
        }
        break;
    }
    }
    // The SFX string address is held in a local across the main loop (a callee-saved register), with
    // the main-loop counter declared after it; otherwise MWCC colours the callee-saved set one
    // register higher.
    {
        const char* sfx = "SFX_BTL_SAC_ZUBA_STAR3";
        s32 i;
        for (i = 0; i < work->bombCount; i++) {
            ZubaStarBomb* bomb = work->bombs[i];

            switch (bomb->state) {
            case 0: {
                s32 maxTimer = irand(0x1E) + 0x1E;
                f32 sx = 0.0f;
                f32 sy = 0.0f;
                f32 sz = 0.0f;
                s32 j;

                bomb->maxTimer = maxTimer;
                bomb->from = (Vec){ 0.0f, 0.0f, 0.0f };
                bomb->to = (Vec){ 0.0f, -1000.0f, 200.0f };
                for (j = 0; j < bomb->count; j++) {
                    sx += bomb->points[j].x;
                    sy += bomb->points[j].y;
                    sz += bomb->points[j].z;
                }
                bomb->center.x = sx / (f32)bomb->count;
                bomb->center.y = sy / (f32)bomb->count;
                bomb->center.z = sz / (f32)bomb->count;
                bomb->to.x +=
                    (bomb->center.x - 104.0f) * 600.0f / 200.0f;
                bomb->angle = 0.0f;
                bomb->angleVel = 70.0f * (f32)rand() / 32767.0f - 35.0f;
                // (f32)maxTimer is cast at each use: the original converts it three times, so a temp
                // here would fold them into one and change the schedule.
                bomb->velY = (2.0f * (bomb->to.y - bomb->from.y) -
                                    ((f32)maxTimer * (-1.25f * (f32)maxTimer))) /
                                   (2.0f * (f32)maxTimer);
                bomb->timer = 0;
                bomb->state = 5;
                psndSFXOn(sfx);
                break;
            }
            case 5:
                bomb->pos.x =
                    intplGetValue(0, bomb->timer, bomb->maxTimer, bomb->from.x, bomb->to.x);
                bomb->pos.z =
                    intplGetValue(0, bomb->timer, bomb->maxTimer, bomb->from.z, bomb->to.z);
                bomb->pos.y += bomb->velY;
                bomb->angle += bomb->angleVel;
                bomb->velY += -1.25f;
                bomb->timer++;
                if (bomb->timer > bomb->maxTimer) {
                    bomb->state = 0xA;
                }
                break;
            case 0xA:
                break;
            }
        }
    }
    dispEntry(8, 0.0f, 1, zubastar_disp_bomb, NULL);
    return 0;
}

// .text:0x7D0 | 0x8022ED60 | size: 0x30
s32 zubastar_init(EventEntry* event, BOOL isFirstCall) {
    memset(GetZubaStarPtr(), 0, 0x5E0);
    return 2;
}

// .text:0x72C | 0x8022ECBC | size: 0xA4
s32 zubastar_end(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;

    effDelete(((effStarStoneData*)work->effect->userdata)->dustEffect);
    effDelete(work->effect);
    for (i = 0; i < 0xC8; i++) {
        if (work->bombs[i] != NULL) {
            if (work->bombs[i]->points != NULL) {
                BattleFree(work->bombs[i]->points);
                work->bombs[i]->points = NULL;
            }
            BattleFree(work->bombs[i]);
            work->bombs[i] = NULL;
        }
    }
    smartFree(work->tex);
    return 2;
}

// .text:0x6C4 | 0x8022EC54 | size: 0x68
void zubastar_capture(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();

    GXSetTexCopySrc(0xD0, 0, 0x190, 0x1E0);
    GXSetTexCopyDst(0x190, 0x1E0, 4, 0);
    GXCopyTex(work->tex->address, 0);
    GXPixModeSync();
}

// .text:0x368 | 0x8022E8F8 | size: 0x35C
// Constant-register mapping, Mtx slots and both loops match; loading p->x/p->y into locals fixes the inner
// vertex reloads, and indexing bomb->points[j] directly (no local Vec*) gives the original's r5 inner
// counter. The inner loop computes both full texcoords before the position call (that is what gives the
// target's f0/f3 colouring and the interleaved stores).
#pragma opt_propagation off // needed: without it MWCC hoists the t numerator and colours the s/t chain f1/f0 (98.88%)
void zubastar_disp_bomb(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();
    CameraEntry* cam = camGetPtr(8);
    GXTexObj texObj;
    Mtx m1;
    Mtx m2;
    Mtx m4;
    Mtx m3;
    Mtx mtx;
    ZubaStarBomb* bomb;
    s32 j;
    s32 i;
    f32 c480;
    f32 c240;
    f32 c400;
    f32 c96;
    f32 cdeg;
    static const f32 degree = 0.017453292f;

    GXSetCullMode(GX_CULL_NONE);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ONE);
    GXInitTexObj(&texObj, work->tex->address, 0x190, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f,
                    0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    c96 = 96.0f;
    c400 = 400.0f;
    c240 = 240.0f;
    c480 = 480.0f;
    // Keep the stored coefficient: folding this read adds a second pool entry.
    cdeg = MATCH_F32(degree);
    for (i = 0; i < work->bombCount; i++) {
        bomb = work->bombs[i];

        PSMTXTrans(m1, bomb->center.x, bomb->center.y, bomb->center.z);
        PSMTXTrans(m2, -bomb->center.x, -bomb->center.y, -bomb->center.z);
        PSMTXTrans(m3, bomb->pos.x, bomb->pos.y, bomb->pos.z);
        PSMTXRotRad(m4, 'z', cdeg * bomb->angle);
        PSMTXConcat(m4, m2, mtx);
        PSMTXConcat(m1, mtx, mtx);
        PSMTXConcat(m3, mtx, mtx);
        PSMTXConcat(cam->view, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, bomb->count);
        for (j = 0; j < bomb->count; j++) {
            f32 y = bomb->points[j].y;
            f32 x = bomb->points[j].x;
            f32 s = (c96 + x) / c400;
            f32 t = (c240 - y) / c480;

            GXPosition3f32(x, y, 0.0f);
            GXTexCoord2f32(s, t);
        }
    }
}
#pragma opt_propagation reset

// .text:0x0 | 0x8022E590 | size: 0x368
// The original holds work+0x18 in r30 and reaches sequence, cursor and cursorHistory
// through it (the compiler CSEs the base). MWCC here does not, so under MUST_MATCH a local
// u8* b18 = (u8*)work + 0x18 with Z18/Z20/Z2C(i) forces the same base; the natural struct-field
// accesses are under #else. All of disp2D's named float constants are now literals (200.0f, 60.0f, ...),
// which gives MWCC their known values and recovers the original's scheduling; the original TU defined
// them, so its loads use their names and ours use anonymous pooled entries (arg_mismatch only). Same for
// the `pos`/vertex zeros.
void zubastar_disp2D(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();
#ifdef MUST_MATCH
    u8* b18 = (u8*)work + 0x18;
#define Z18 (*(s32*)b18)
#define Z20 ((Vec*)(b18 + 8))
#define Z2C(i) (((Vec*)(b18 + 0x14)) + (i))
#else
#define Z18 (work->sequence)
#define Z20 (&work->cursor)
#define Z2C(i) (&work->cursorHistory[i])
#endif
    ZubaStarStar* star;
    s32 i;
    s32 alpha;
    Vec pos;

    if (work->gaugePhase >= 5) {
        s32 icon;

        icon = (work->iconFrame <= 3) ? 0x64 : 0x65;
        pos = (Vec){ 0.0f, 0.0f, 0.0f };
        pos.x = work->gaugePos.x - 200.0f;
        pos.y = 60.0f + work->gaugePos.y;
        iconDispGx(1.0f, &pos, 0x10, icon);
        BattleAcDrawGauge((s32)work->gaugePos.x, (s32)work->gaugePos.y, 0.01f * work->gauge, 0xB1, 4, 0x1A, 0x34, 0x4E,
                          0);
    }
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetCurrentMtx(0);
    GXLoadPosMtxImm(camGetPtr(cameraId)->view, 0);
    GXSetLineWidth(0x18, 0);
    for (i = 0; i < 0x10; i++) {
        star = &work->stars[i];
        if (star->timer <= 0x1E) {
            alpha = (s32)intplGetValue(0, star->timer, 0x1E, 128.0f,
                                       255.0f);
        } else {
            alpha = (s32)intplGetValue(0, star->timer - 0x1E, 0x1E, 255.0f,
                                       128.0f);
        }
        if (star->state == 5) {
            {
                GXSetChanMatColor(GX_COLOR0A0, (GXColor){ alpha, alpha, alpha, 0xFF });
            }
            GXBegin(0xA8, 0, 2);
            GXPosition3f32(star->x0, star->y0, 0.0f);
            GXPosition3f32(star->x1, star->y1, 0.0f);
        }
    }
    iconDispGx(1.0f, Z20, 0x10, 0x19D);
    if (Z18 >= 0x19 && Z18 < 0x1E) {
        f32 yoffset = 20.0f;

        for (i = 0; i < 4; i++) {
            Mtx mtx;

            PSMTXTrans(mtx, Z2C(i)->x, Z2C(i)->y - yoffset, 0.0f);
            iconDispGxCol(mtx, 0x10, 0x19D, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF - (i + 1) * 0x32 });
        }
    }
}
#undef Z18
#undef Z20
#undef Z2C

s32 marioAttackEvent_ZubaStar[177];

BattleWeapon weapon_zubastar = {
    /* name */ str_btl_wn_sac_zubastar,
    /* icon */ 0x19B,
    /* item */ 0,
    /* description */ str_msg_zubastar,
    /* baseAccuracy */ 100,
    /* baseFpCost */ 0,
    /* baseSpCost */ 6,
    /* superguardType */ 0,
    /* field_0x14 */ 1.0f,
    /* mStylishCommandMultiplier */ 1,
    /* field_0x19 */ 5,
    /* mBingoSlotChance */ 100,
    /* field_0x1B */ 50,
    /* hpDamageFn */ weaponGetPower_ZubaStar,
    /* hpDamageWork */ { 0, 3, 6, 9, 12, 15, 0, 0 },
    /* fpDamageFn */ 0,
    /* fpDamageWork */ { 0 },
    /* targetClassFlags */ 0x02101260,
    /* targetPropertyFlags */ 0x20000000,
    /* element */ 0,
    /* field_0x6D */ 0,
    /* actionLevel */ 3,
    /* field_0x6F */ 2,
    /* acHelpMsg */ str_msg_sac_8,
    /* special */ (BattleWeaponSpecialFlags)0x000200C8,
    /* counterResistanceFlags */ 0x7FF,
    /* targetWeightingFlags */ 0,
    /* sleepChance */ 0,
    /* sleepTime */ 0,
    /* stopChance */ 0,
    /* stopTime */ 0,
    /* dizzyChance */ 0,
    /* dizzyTime */ 0,
    /* poisonChance */ 0,
    /* poisonTime */ 0,
    /* poisonStrength */ 0,
    /* confuseChance */ 0,
    /* confuseTime */ 0,
    /* electricChance */ 0,
    /* electricTime */ 0,
    /* dodgyChance */ 0,
    /* dodgyTime */ 0,
    /* burnChance */ 0,
    /* burnTime */ 0,
    /* freezeChance */ 0,
    /* freezeTime */ 0,
    /* sizeChangeChance */ 0,
    /* sizeChangeTime */ 0,
    /* sizeChangeStrength */ 0,
    /* atkChangeChance */ 0,
    /* atkChangeTime */ 0,
    /* atkChangeStrength */ 0,
    /* defChangeChance */ 0,
    /* defChangeTime */ 0,
    /* defChangeStrength */ 0,
    /* allergicChance */ 0,
    /* allergicTime */ 0,
    /* ohkoChance */ 0,
    /* chargeStrength */ 0,
    /* fastChance */ 0,
    /* fastTime */ 0,
    /* slowChance */ 0,
    /* slowTime */ 0,
    /* frightChance */ 0,
    /* galeForceChance */ 0,
    /* paybackTime */ 0,
    /* holdFastTime */ 0,
    /* invisibleChance */ 0,
    /* invisibleTime */ 0,
    /* hpRegenTime */ 0,
    /* hpRegenStrength */ 0,
    /* fpRegenTime */ 0,
    /* fpRegenStrength */ 0,
    /* mAttackEvtScript */ marioAttackEvent_ZubaStar,
    /* field_B4 */ { 0, 0, 0, 100 },
};

EVT_BEGIN(marioAttackEvent_ZubaStar)
    CALL(zubastar_init)
    BEGIN_SIBLING_EVENT
        CALL(main_star)
    END_SIBLING_EVENT
    CALL_CHILD(sac_start_camera_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING((void*)"SFX_BTL_SAC_START1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL_CHILD(sac_mario_item_on_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING((void*)"SFX_BTL_SAC_START2"), EVTDAT_POINTER_MAX, 0, LW(11))
    CALL(star_stone_appear)
    BEGIN_SIBLING_EVENT
        CALL_CHILD(sac_start_stardust)
    END_SIBLING_EVENT
    WAIT_FRAMES(100)
    CALL(btlevtcmd_snd_se, -2, STRING((void*)"SFX_BTL_SAC_START3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(40)
    CALL_CHILD(sac_mario_item_on_end_evt)
    CALL_CHILD(sac_mario_back_home)
    CALL(wait_star_stone_up)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_Y"))
    WAIT_MS(500)
    BEGIN_SIBLING_EVENT
        CALL(sac_handbeat)
    END_SIBLING_EVENT
    CALL(sac_ac_help_on)
    CALL(zubastar_main)
    CALL(sac_ac_help_off)
    CALL(sac_handbeat_end)
    BEGIN_SIBLING_EVENT
        CALL(evt_fade_entry, 17, 1, 255, 255, 255)
        WAIT_MS(100)
        CALL(evt_fade_entry, 16, 250, 255, 255, 255)
    END_SIBLING_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 0)
        LOOP(6)
            CALL(evt_btl_camera_shake_w, 0, 10, 0, 10, 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(95)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(97)
    END_IF
    CALL(btlevtcmd_CheckPartsAttribute, LW(3), LW(4), 16384, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_zubastar), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(95)
    LABEL(97)
    CALL(zubastar_bomb)
    CALL(zubastar_end)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()
