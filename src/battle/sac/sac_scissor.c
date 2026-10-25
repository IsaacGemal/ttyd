#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_starstone.h"
#include "driver/icondrv.h"
#include "driver/offscreendrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include "battle/sac/sac_scissor.h"
#include "battle/battle_camera.h"
#include "battle/battle_database_common.h"
#include "battle/battle_event_cmd.h"
#include "evt/evt_cmd.h"
#include "evt/evt_img.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_snd.h"
#include "party/party_motion.h"

extern BattleWork* _battleWorkPointer;

#pragma pool_data on // the original addresses its .rodata through one base register

// Data objects defined at the end of this file and scripts owned by sac_common.
extern s32 marioAttackEvent_Scissor[];
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_mario_back_home[];
extern s32 sac_start_stardust[];
extern s32 sac_cheer[];
extern s32 sac_cheer_end[];
extern s32 sac_ac_help_on[];
extern s32 sac_ac_help_off[];

extern char str_ofs[4];
extern BattleWeapon weapon_scissor;

// Double constants emitted in this translation unit's .sdata2.

// A 2D point in the scissor polygon buffers (8 bytes, copied by value in tess).
typedef struct ScissorPoint {
    f32 x;
    f32 y;
} ScissorPoint;

// Local view of the scissor action-command work area at _battleWorkPointer + 0x1F4C.
// No header declares this struct yet.
typedef struct ScissorWork {
    u32 flags;                    // 0x00, bit 0: the capture has a visible bounding box
    u8 field_0x04[0x08];          // 0x04
    s32 comboCount;               // 0x0C, allHit rounds so far (capped at 6 when shown)
    s32 pulse;                    // 0x10, 0..0x3B frame counter for the line colour
    s32 triCount;                 // 0x14, triangles in triBuf
    s32 hitUnitIds[16];           // 0x18, unit ids
    s32 hitPower[16];             // 0x58, damage each hit unit took
    u8 audienceVol;               // 0x98, audience sound volume, raised 0x14 per damaging move
    u8 pad_0x99[0x03];            // 0x99
    s32 pointCount;               // 0x9C, points in pointBuf (the StarTrail view starts here)
    s32 field_0xA0;               // 0xA0
    f32 prevStickX;               // 0xA4
    f32 prevStickY;               // 0xA8
    f32 stickX;                   // 0xAC
    f32 stickY;                   // 0xB0
    f32 cursorX;                  // 0xB4, the star cursor; star_disp2D_sub draws the star here
    f32 cursorY;                  // 0xB8
    f32 lastPointX;               // 0xBC, last point written to pointBuf
    f32 lastPointY;               // 0xC0
    u8 field_0xC4[0x10];          // 0xC4
    f32 cursorVelX;               // 0xD4
    f32 cursorVelY;               // 0xD8
    f32 trailX[4];                // 0xDC, the cursor's last four positions (star_control shifts them in every
    f32 trailY[4];                // 0xEC  fourth frame), where star_disp2D_sub draws the trailing stars
    s32 timerPhase;               // 0xFC, scissor_timer_main switch
    s32 timerFrame;               // 0x100
    s32 timeLeft;                 // 0x104, starts at 0x3BF
    f32 timerPos;                 // 0x108, the timer's position (320 .. 120)
    f32 field_0x10C;              // 0x10C
    u8 field_0x110[0x04];         // 0x110
    void* pointBuf;               // 0x114, 0x1000 bytes of ScissorPoint
    f32 (*triBuf)[6];             // 0x118, 0x3000 bytes of triangles (f32 x3, then y3 per triangle)
    s32 state;                    // 0x11C
    s32 stateFrame;               // 0x120
    Vec starPos;                  // 0x124
    Vec starFrom;                 // 0x130, interpolation start of the star's rise
    Vec starTo;                   // 0x13C, interpolation end of the star's rise
    u8 field_0x148[0x0C];         // 0x148
    Vec starScale;                // 0x154
    Vec starRot;                  // 0x160
    EffectEntry* starStone;       // 0x16C
    smartEntry* captureBuf;       // 0x170, texture the scissor area is captured into
} ScissorWork;

// ScissorWork from pointCount (0x9C) on, as star_control and star_disp2D_sub reach it: through a work+0x9C pointer
// in the original.
typedef struct StarTrail {
    u8 field_0x00[0x18];  // 0x00
    f32 cursorX;          // 0x18, ScissorWork.cursorX
    f32 cursorY;          // 0x1C
    u8 field_0x20[0x20];  // 0x20
    f32 trailX[4];        // 0x40, ScissorWork.trailX
    f32 trailY[4];        // 0x50
} StarTrail;

ScissorWork* GetScissorPtr(void) {
    return (ScissorWork*)&_battleWorkPointer->actionCommands.acWork[0];
}

// fabsf as the SDK defines it (fake_tgmath.h); math_ext.h only declares it.
extern inline float fabsf(float x) {
    return __fabsf(x);
}

s32 scissor_intersection(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    f32 d1x, d1y, d2x, d2y;
    f32 minX1, minY1, maxX1, maxY1;
    f32 minX2, minY2, maxX2, maxY2;
    s32 type1;
    s32 type2;

    if (ax <= bx) {
        minX1 = ax;
        maxX1 = bx;
    } else {
        minX1 = bx;
        maxX1 = ax;
    }
    if (ay <= by) {
        minY1 = ay;
        maxY1 = by;
    } else {
        minY1 = by;
        maxY1 = ay;
    }
    if (cx <= dx) {
        minX2 = cx;
        maxX2 = dx;
    } else {
        minX2 = dx;
        maxX2 = cx;
    }
    if (cy <= dy) {
        minY2 = cy;
        maxY2 = dy;
    } else {
        minY2 = dy;
        maxY2 = cy;
    }

    d1x = bx - ax;
    type1 = 0;
    if (d1x == 0.0f) type1 = 1;
    d1y = by - ay;
    if (d1y == 0.0f) type1 = 2;
    d2x = dx - cx;
    type2 = 0;
    if (d2x == 0.0f) type2 = 1;
    d2y = dy - cy;
    if (d2y == 0.0f) type2 = 2;

    if (type1 == 0 && type2 == 0) {
        f32 m1, m2, den;

        if (fabsf(d1x) > fabsf(d1y)) {
            m1 = d1y / d1x;
            if (fabsf(d2x) > fabsf(d2y)) {
                m2 = d2y / d2x;
                if (m1 - m2 != 0.0f) {
                    den = m1 - m2;
                } else {
                    den = 0.000001f;
                }
                *ox = ((cy - ay) + (m1 * ax - m2 * cx)) / den;
                *oy = m1 * (*ox - ax) + ay;
            } else {
                m2 = d2x / d2y;
                if (m1 * m2 != 1.0f) {
                    den = 1.0f - m1 * m2;
                } else {
                    den = 0.000001f;
                }
                *ox = (m2 * (-m1 * ax + ay - cy) + cx) / den;
                *oy = m1 * (*ox - ax) + ay;
            }
        } else {
            m1 = d1x / d1y;
            if (fabsf(d2x) > fabsf(d2y)) {
                m2 = d2y / d2x;
                if (m1 * m2 != 1.0f) {
                    den = 1.0f - m1 * m2;
                } else {
                    den = 0.000001f;
                }
                *oy = (m2 * (-m1 * ay + ax - cx) + cy) / den;
                *ox = m1 * (*oy - ay) + ax;
            } else {
                m2 = d2x / d2y;
                if (m1 - m2 != 0.0f) {
                    den = m1 - m2;
                } else {
                    den = 0.000001f;
                }
                *oy = ((cx - ax) + (m1 * ay - m2 * cy)) / den;
                *ox = m1 * (*oy - ay) + ax;
            }
        }
        if (*ox >= minX1 && *ox <= maxX1 && *oy >= minY1 && *oy <= maxY1 &&
            *ox >= minX2 && *ox <= maxX2 && *oy >= minY2 && *oy <= maxY2) {
            return 1;
        }
        return 0;
    } else if (type1 == 0 && type2 == 1) {
        f32 m1 = d1y / d1x;

        *ox = cx;
        *oy = m1 * (*ox - ax) + ay;
        if (*ox >= minX1 && *ox <= maxX1 && *oy >= minY2 && *oy <= maxY2) return 1;
        return 0;
    } else if (type1 == 1 && type2 == 0) {
        f32 m2 = d2y / d2x;

        *ox = ax;
        *oy = m2 * (*ox - cx) + cy;
        if (*ox >= minX2 && *ox <= maxX2 && *oy >= minY1 && *oy <= maxY1) return 1;
        return 0;
    } else if (type1 == 0 && type2 == 2) {
        f32 m1 = d1y / d1x;

        *oy = cy;
        *ox = (m1 * ax + (*oy - ay)) / m1;
        if (*oy >= minY1 && *oy <= maxY1 && *ox >= minX2 && *ox <= maxX2) return 1;
        return 0;
    } else if (type1 == 2 && type2 == 0) {
        f32 m2 = d2y / d2x;

        *oy = ay;
        *ox = (m2 * cx + (*oy - cy)) / m2;
        if (*oy >= minY2 && *oy <= maxY2 && *ox >= minX1 && *ox <= maxX1) return 1;
        return 0;
    } else if (type1 == 1 && type2 == 1) {
        if (ax == cx) {
            *ox = cx;
            *oy = cy;
            if ((ay >= minY2 && ay <= maxY2) || (by >= minY2 && by <= maxY2)) return 1;
        }
        return 0;
    } else if (type1 == 1 && type2 == 2) {
        *ox = ax;
        *oy = cy;
        if (ax >= minX2 && ax <= maxX2 && cy >= minY1 && cy <= maxY1) return 1;
        return 0;
    } else if (type1 == 2 && type2 == 1) {
        *ox = cx;
        *oy = ay;
        if (cx >= minX1 && cx <= maxX1 && ay >= minY2 && ay <= maxY2) return 1;
        return 0;
    } else if (type1 == 2 && type2 == 2) {
        if (ay == cy) {
            *ox = cx;
            *oy = cy;
            if ((ax >= minX2 && ax <= maxX2) || (bx >= minX2 && bx <= maxX2)) return 1;
        }
        return 0;
    }
    return 0;
}

s32 scissor_intersection2(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    if (ax == cx && ay == cy) {
        return 0;
    }
    if (ax == dx && ay == dy) {
        return 0;
    }
    if (bx == cx && by == cy) {
        return 0;
    }
    if (bx == dx && by == dy) {
        return 0;
    }
    return scissor_intersection(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}

s32 scissor_intersection3(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    f32 diff = bx - ax;

    if (0.0f != diff) {
        f32 slope = (by - ay) / diff;
        if (ax >= bx) {
            ax += 1000.0f;
            bx -= 1000.0f;
            ay += 1000.0f * slope;
            by -= 1000.0f * slope;
        } else {
            ax -= 1000.0f;
            bx += 1000.0f;
            ay -= 1000.0f * slope;
            by += 1000.0f * slope;
        }
    } else {
        if (ay >= by) {
            ay = 1000.0f;
            by = -1000.0f;
        } else {
            ay = -1000.0f;
            by = 1000.0f;
        }
    }
    return scissor_intersection(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}

s32 scissor_intersection4(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    if (ax == cx && ay == cy) {
        *ox = ax;
        *oy = ay;
        return 1;
    }
    if (ax == dx && ay == dy) {
        *ox = ax;
        *oy = ay;
        return 1;
    }
    if (bx == cx && by == cy) {
        *ox = bx;
        *oy = by;
        return 1;
    }
    if (bx == dx && by == dy) {
        *ox = bx;
        *oy = by;
        return 1;
    }
    return scissor_intersection3(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}

// Matches the original byte-for-byte.
s32 scissor_cross(f32* a, f32* b, f32* c, s32 n) {
    f32 xs[0x200];
    f32 ys[0x200];
    f32 ox;
    f32 oy;
    f32 tx;
    f32 ty;
    f32 one;
    f32* p;
    f32* q;
    s32 count;
    s32 i;
    s32 j;
    s32 m;

    for (i = 0; i < n; i++) {
        s32 k = (i + 1) % n;
        if (scissor_intersection2(a[0], a[1], b[0], b[1],
                                  c[i * 2], c[i * 2 + 1],
                                  c[k * 2], c[k * 2 + 1],
                                  &ox, &oy) != 0) {
            return 0;
        }
    }
    xs[0] = -1000.0f;
    count = 1;
    ys[0] = -1000.0f;
    one = 1.0f;
    for (i = 0; i < n; i++) {
        s32 k = (i + 1) % n;
        p = &c[i * 2];
        q = &c[k * 2];
        if (scissor_intersection4(one + a[0], a[1],
                                  one + b[0], b[1],
                                  p[0], p[1],
                                  q[0], q[1],
                                  &ox, &oy) != 0) {
            s32 flag = 0;
            if (p[0] == ox && p[1] == oy) {
                f32* prev = (i == 0) ? &c[(n - 1) * 2] : &c[(i - 1) * 2];
                if (scissor_intersection3(a[0], a[1], b[0], b[1],
                                          prev[0], prev[1],
                                          q[0], q[1],
                                          &tx, &ty) != 0) {
                    flag = 1;
                }
            }
            if (flag == 1) {
                for (m = 0; m < n; m++) {
                    if (xs[m] == ox && ys[m] == oy) {
                        break;
                    }
                }
                if (m == n) {
                    xs[count] = ox;
                    ys[count] = oy;
                    count++;
                }
            } else {
                xs[count] = ox;
                ys[count] = oy;
                count++;
            }
        }
    }
    xs[count] = 1000.0f;
    ys[count] = 1000.0f;
    if (a[0] != b[0]) {
        for (i = 0; i < count; i++) {
            for (j = i + 1; j < count + 1; j++) {
                if (xs[i] > xs[j]) {
                    f32 t;
                    t = xs[i];
                    xs[i] = xs[j];
                    xs[j] = t;
                    t = ys[i];
                    ys[i] = ys[j];
                    ys[j] = t;
                }
            }
        }
        {
            f32 mid = (a[0] + b[0]) * 0.5f;
            for (i = 0; i < count; i++) {
                if (mid >= xs[i] && mid < xs[i + 1]) {
                    return i & 1;
                }
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            for (j = i + 1; j < count + 1; j++) {
                if (ys[i] > ys[j]) {
                    f32 t;
                    t = xs[i];
                    xs[i] = xs[j];
                    xs[j] = t;
                    t = ys[i];
                    ys[i] = ys[j];
                    ys[j] = t;
                }
            }
        }
        {
            f32 mid = (a[1] + b[1]) * 0.5f;
            for (i = 0; i < count; i++) {
                if (mid >= ys[i] && mid < ys[i + 1]) {
                    return i & 1;
                }
            }
        }
    }
    return 0;
}

s32 scissor_tess(void) {
    ScissorWork* work = GetScissorPtr();
    f32* points = BattleAlloc(work->pointCount << 3);

    memcpy(points, work->pointBuf, work->pointCount << 3);
    work->triCount = 0;
    tess(points, work->pointCount);
    DCFlushRange(work->triBuf, 0x3000);
    return 2;
}

s32 tess(f32* points, s32 count) {
    ScissorWork* work = GetScissorPtr();
    s32 i;
    s32 j;
    s32 k;

    if (count == 3) {
        f32* dst = work->triBuf[work->triCount];
        for (i = 0; i < 3; i++) {
            dst[i] = points[i * 2];
            dst[i + 3] = points[i * 2 + 1];
        }
        work->triCount++;
        BattleFree(points);
    } else {
        ScissorPoint* p = (ScissorPoint*)points;
        for (i = 0; i < count; i++) {
            for (j = 2; j < count - 1; j++) {
                s32 rem = (i + j) % count;
                if (scissor_cross(&p[i].x, &p[rem].x, points, count) == 1) {
                    s32 n1 = j + 1;
                    ScissorPoint* v1 = BattleAlloc(n1 * 8);
                    for (k = 0; k < n1; k++) {
                        s32 r0 = (i + k) % count;
                        v1[k] = p[r0];
                    }
                    {
                        s32 n2 = count - j + 1;
                        ScissorPoint* v2 = BattleAlloc(n2 * 8);
                        v2[0] = p[i];
                        for (k = 0; k < n2 - 1; k++) {
                            s32 r0 = (i + j + k) % count;
                            v2[k + 1] = p[r0];
                        }
                        BattleFree(points);
                        tess((f32*)v1, n1);
                        tess((f32*)v2, n2);
                    }
                    return;
                }
            }
        }
        {
            f32* dst = work->triBuf[work->triCount];
            for (i = 0; i < 3; i++) {
                dst[i] = points[i * 2];
                dst[i + 3] = points[i * 2 + 1];
            }
            work->triCount++;
            BattleFree(points);
        }
    }
}

u32 weaponGetPower_Scissor(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part) {
    ScissorWork* work = GetScissorPtr();
    s32 i;

    if (unit2 == NULL) {
        return 0;
    }
    if (unit2->mAlliance == 1) {
        for (i = 0; i < 0x10; i++) {
            if (work->hitUnitIds[i] == BtlUnit_GetUnitId(unit2)) {
                return work->hitPower[i];
            }
        }
    }
    return 0;
}

// .data:0x0..0xC0
BattleWeapon weapon_scissor = {
    "btl_wn_sac_scissor",                     // name
    412,                                      // icon
    ITEM_NULL,                                // item
    "msg_scissor",                            // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    4,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPower_Scissor,                   // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_5",                              // acHelpMsg
    0x000200C8,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Scissor,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

// .data:0xC0..0x180
BattleWeapon weapon_scissor_finish = {
    "btl_wn_sac_scissor",                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    27,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020088,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};
s32 star_control(void) {
    ScissorWork* work = GetScissorPtr();
    StarTrail* star = (StarTrail*)&work->pointCount;
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;
    s32 i;

    if ((gp->retraceCount & 3) == 0) {
        for (i = 3; i > 0; i--) {
            star->trailX[i] = star->trailX[i - 1];
            star->trailY[i] = star->trailY[i - 1];
        }
        star->trailX[0] = star->cursorX;
        star->trailY[0] = star->cursorY;
    }
    switch (work->state) {
    case 1:
        work->state = 2;
        work->stateFrame = 0;
        work->starStone = effStarStoneEntry(3, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &work->starPos.x, &work->starPos.y, &work->starPos.z);
        work->starPos.y += mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f;
        work->starFrom = work->starPos;
        work->starTo = work->starPos;
        work->starTo.y += 50.0f;
        work->starTo.z += -1.0f;
        work->starScale = (Vec){0.0f, 0.0f, 0.0f};
        work->starRot = (Vec){0.0f, 0.0f, 0.0f};
        /* fallthrough */
    case 2:
        work->stateFrame++;
        if (work->stateFrame <= 0x64) {
            work->starPos.x = intplGetValue(0, work->stateFrame, 0x64, work->starFrom.x, work->starTo.x);
            work->starPos.y = intplGetValue(0, work->stateFrame, 0x64, work->starFrom.y, work->starTo.y);
            work->starPos.z = intplGetValue(0, work->stateFrame, 0x64, work->starFrom.z, work->starTo.z);
            f = intplGetValue(0, work->stateFrame, 0x64, 0.0f, 2.0f);
            work->starScale.z = f;
            work->starScale.y = f;
            work->starScale.x = f;
        } else {
            work->starPos.x = work->starTo.x;
            work->starPos.y = work->starTo.y;
            work->starPos.z = work->starTo.z;
            work->starScale.z = 2.0f;
            work->starScale.y = 2.0f;
            work->starScale.x = 2.0f;
        }
        work->starRot.y = intplGetValue(4, work->stateFrame, 0x78, 0.0f, 2160.0f);
        if (work->stateFrame >= 0x78) {
            work->state = 3;
            work->stateFrame = 0;
            work->starFrom = work->starPos;
            work->starTo.y = 300.0f;
        }
        break;
    case 3:
        work->stateFrame++;
        work->starPos.y = intplGetValue(1, work->stateFrame, 0x3C, work->starFrom.y, work->starTo.y);
        if (work->stateFrame >= 0x3C) {
            work->state = 4;
            work->stateFrame = 0;
            {
                work->starPos = (Vec){0.0f, 300.0f, 0.0f};
                work->starScale = (Vec){1.0f, 1.0f, 1.0f};
                work->starRot = (Vec){0.0f, 0.0f, 0.0f};
                work->starFrom.y = work->starPos.y;
                work->starTo.y = 0.0f;
                star->cursorX = work->starPos.x;
                star->cursorY = work->starPos.y;
                for (i = 0; i < 4; i++) {
                    star->trailX[i] = 0.0f;
                    star->trailY[i] = -1000.0f;
                }
            }
        }
        break;
    case 4:
        work->stateFrame++;
        work->starPos.y = intplGetValue(4, work->stateFrame, 0x3C, work->starFrom.y, work->starTo.y);
        star->cursorX = work->starPos.x;
        star->cursorY = work->starPos.y;
        if (work->stateFrame >= 0x3C) {
            work->state = 5;
            work->stateFrame = 0;
        }
        break;
    case 5:
    case 6:
        break;
    }
    if (work->starStone != NULL) {
        effStarStoneData* data = work->starStone->userdata;

        if (work->state >= 4) {
            f32 zero = 0.0f;

            data->x = zero;
            data->y = -1000.0f;
            data->z = zero;
        } else {
            data->x = work->starPos.x;
            data->y = work->starPos.y;
            data->z = work->starPos.z;
        }
        {
            f32 s = 1.5f;

            data->tipRotY = work->starRot.x;
            data->rotY = work->starRot.y;
            data->tipRotZ = work->starRot.z;
            data->scale = s * work->starScale.x;
        }
    }
    if (work->state >= 4 && work->state <= 6) {
        dispEntry(8, 0.0f, 2, (DispCallback)star_disp2D, NULL);
    }
    return 0;
}

void star_disp2D(void) {
    ScissorWork* work = GetScissorPtr();

    if (work->state >= 2 && work->state <= 6) {
        star_disp2D_sub(0, 0xFF);
        star_disp2D_sub(1, 0xC0);
        star_disp2D_sub(2, 0x80);
        star_disp2D_sub(3, 0x40);
        star_disp2D_sub(4, 0x20);
    }
}

void star_disp2D_sub(s32 index, s32 alpha) {
    ScissorWork* work = GetScissorPtr();
    StarTrail* star = (StarTrail*)&work->pointCount;
    Mtx mTemp;
    Mtx mTrans;
    Mtx mRotX;
    Mtx mRotY;
    Mtx mRotZ;
    Mtx mScale;
    GXColor color;
    f32 x;
    f32 y;
    f32 z;

    if (work->state >= 6) {
        work->stateFrame++;
        if (work->stateFrame > 0x1E) {
            work->stateFrame = 0x1E;
        }
        alpha = (s32)intplGetValue(0, work->stateFrame, 0x1E, (f32)(u32)(u8)alpha, 0.0f);
    }
    color = (GXColor){ 0xFF, 0xFF, 0xFF, alpha };
    if (index == 0) {
        x = star->cursorX;
        y = star->cursorY;
        z = 0.0f;
    } else {
        x = star->trailX[index - 1];
        y = star->trailY[index - 1];
        z = 0.0f;
    }
    PSMTXTrans(mTrans, x, y - 20.0f, z);
    PSMTXScale(mScale, work->starScale.x, work->starScale.y, work->starScale.z);
    PSMTXRotRad(mRotX, 'x', 0.017453292f * work->starRot.x);
    PSMTXRotRad(mRotY, 'y', 0.017453292f * work->starRot.y);
    PSMTXRotRad(mRotZ, 'z', 0.017453292f * work->starRot.z);
    PSMTXConcat(mRotZ, mRotY, mTemp);
    PSMTXConcat(mRotX, mTemp, mTemp);
    PSMTXConcat(mScale, mTemp, mTemp);
    PSMTXConcat(mTrans, mTemp, mTemp);
    iconDispGxCol(mTemp, 0x10, 0x19C, color);
}

s32 scissor_timer_get(EventEntry* event, BOOL isFirstCall) {
    ScissorWork* work = GetScissorPtr();
    s32* args = event->args;

    if (work->timeLeft <= 0x3B) {
        work->state = 6;
    }
    evtSetValue(event, args[0], work->timeLeft);
    return 2;
}

s32 scissor_timer_main(void) {
    ScissorWork* work = GetScissorPtr();

    switch (work->timerPhase) {
        case 0:
        {
            work->timerPos = 320.0f;
            work->field_0x10C = 185.0f;
            work->timerPhase = 5;
            work->timerFrame = 0;
        }
            // fallthrough
        case 5:
            work->timerPos = intplGetValue(4, work->timerFrame, 0xF, 320.0f, 120.0f);
            work->timerFrame++;
            if (work->timerFrame > 0xF) {
                work->timerPhase = 10;
            }
            break;
        case 10:
            work->timeLeft--;
            if (work->timeLeft <= 0x3B) {
                work->timerPhase = 0xF;
                work->timerFrame = 0;
            }
            break;
        case 15:
            work->timerPos = intplGetValue(4, work->timerFrame, 0xF, 120.0f, 320.0f);
            work->timerFrame++;
            if (work->timerFrame > 0xF) {
                work->timerPhase = 0x14;
            }
            work->timeLeft--;
            if (work->timeLeft < 0) {
                work->timeLeft = 0;
            }
            break;
        case 20:
            work->timeLeft--;
            if (work->timeLeft < 0) {
                work->timeLeft = 0;
                return 2;
            }
            break;
    }
    return 0;
}

s32 scissor_control(s32 unused, s32 reset) {
    ScissorWork* work = GetScissorPtr();
    ScissorPoint* points = work->pointBuf;
    s32 i;

    if (reset != 0) {
        work->pointCount = 0;
        work->prevStickY = 0.0f;
        work->prevStickX = 0.0f;
        work->stickY = 0.0f;
        work->stickX = 0.0f;
        work->field_0xA0 = 0;
        work->cursorVelX = 0.0f;
        work->cursorVelY = 0.0f;
        memset(points, 0, 0x1000);
        work->flags &= ~1;
    }
    work->prevStickX = work->stickX;
    work->prevStickY = work->stickY;
    work->stickX = (f32)keyGetStickX(0);
    work->stickY = (f32)keyGetStickY(0);
    {
        f32 mag = (f32)sqrt(work->prevStickX * work->prevStickX + work->prevStickY * work->prevStickY);
        mag = (f32)sqrt(work->stickX * work->stickX + work->stickY * work->stickY);
        if (mag > 42.0f) {
            work->cursorVelX += work->stickX / 50.0f;
            work->cursorVelY += work->stickY / 50.0f;
        }
    }
    work->cursorVelX *= 0.86f;
    work->cursorVelY *= 0.86f;
    work->cursorX += work->cursorVelX;
    work->cursorY += work->cursorVelY;
    if (work->cursorX < -300.0f) {
        work->cursorX = -300.0f;
    }
    if (work->cursorX > 300.0f) {
        work->cursorX = 300.0f;
    }
    if (work->cursorY < -200.0f) {
        work->cursorY = -200.0f;
    }
    if (work->cursorY > 200.0f) {
        work->cursorY = 200.0f;
    }
    if (work->timeLeft <= 0x3B) {
        work->state = 6;
        return 2;
    }
    if (gp->retraceCount % 6 == 0) {
        if (work->pointCount < 0x200) {
            if (work->lastPointX != work->cursorX || work->lastPointY != work->cursorY) {
                if (__fabs(work->cursorX - work->lastPointX) >= 5.0 || __fabs(work->cursorY - work->lastPointY) >= 5.0) {
                    psndSFXOn("SFX_BTL_SAC_KAKONDEPON1");
                }
                points[work->pointCount].x = work->cursorX;
                points[work->pointCount].y = work->cursorY;
                work->lastPointX = work->cursorX;
                work->lastPointY = work->cursorY;
                work->pointCount++;
            }
        }
    }
    if (work->pointCount > 3) {
        for (i = 0; i < work->pointCount - 3; i++) {
            f32 ox;
            f32 oy;

            if (scissor_intersection(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y,
                                     points[work->pointCount - 2].x, points[work->pointCount - 2].y, points[work->pointCount - 1].x, points[work->pointCount - 1].y,
                                     &ox, &oy) != 0) {
                s32 k;

                points[i].x = points[work->pointCount - 1].x = ox;
                points[i].y = points[work->pointCount - 1].y = oy;
                for (k = 0; k < i; k++) {
                    points[k] = points[i];
                }
                if (work->captureBuf != NULL) {
                    smartFree(work->captureBuf);
                    work->captureBuf = NULL;
                }
                work->captureBuf = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, 4, 0, 0), 1);
                dispEntry(8, 0.0f, 0, (DispCallback)scissor_capture, 0);
                return 2;
            }
        }
    }
    dispEntry(8, 0.0f, 0, (DispCallback)scissor_disp_control, 0);
    return 0;
}

// Match note: the declaration order and hoisting unit/i to function scope are what steer MWCC to
// colour damage r21 and unit/i r25. The code is otherwise the natural form.
s32 scissor_damage(EventEntry* event, s32 unused) {
    BattleUnit* unit;
    ScissorWork* work;
    s32 audienceNum;
    BattleWork* battleWork;
    BattleUnit* mario;
    s32 i;
    s32* args;
    s32 unitIndex;
    s32 damage;
    s32 out;
    s32 count;
    s32 allHit;

    args = event->args;
    battleWork = _battleWorkPointer;
    mario = BattleGetMarioPtr(battleWork);
    work = GetScissorPtr();
    count = 0;
    allHit = 1;
    audienceNum = BattleAudience_GetAudienceNum();
    damage = 0;

    for (unitIndex = 0; unitIndex < 0x40; unitIndex++) {
        unit = BattleGetUnitPtr(battleWork, unitIndex);
        if (unit == NULL) {
            continue;
        }
        if (BtlUnit_GetBelong(unit) != 1) {
            continue;
        }
        {
            BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
            if (BtlUnit_CheckStatus(unit, 0x1B) == 1) {
                continue;
            }
            if ((part->attributes & kUnknown_0x4000) || (part->attributes & kUnk_ImmuneToDamageOrStatus) || (part->attributes & kNotTargetable)) {
                continue;
            }
            if (part->defenseAttr->defenseAttrs[0] == 4 || part->defenseAttr->defenseAttrs[0] == 2) {
                continue;
            }
            scissor_damage_sub(unit, &out);
            work->hitUnitIds[count] = unitIndex;
            work->hitPower[count] = out;
            count++;
            if (out < 1) {
                allHit = 0;
            }
        }
    }
    if (count == 0) {
        allHit = 0;
    }
    for (i = 0; i < count; i++) {
        BattleUnit* target;
        BattleUnitPart* part;

        if (work->hitUnitIds[i] == -1) {
            continue;
        }
        if (work->hitPower[i] < 1) {
            continue;
        }
        target = BattleGetUnitPtr(battleWork, work->hitUnitIds[i]);
        part = BtlUnit_GetPartsPtr(target, BtlUnit_GetBodyPartsId(target));
        BattleCheckDamage(mario, target, part, &weapon_scissor, 0);
        damage = 1;
    }
    if (allHit == 1) {
        s32 value;

        work->comboCount++;
        value = work->comboCount + 1;
        if (value >= 7) {
            value = 6;
        }
        evtSetValue(event, args[0], value);
    } else {
        evtSetValue(event, args[0], -1);
    }
    if (damage == 1) {
        if (audienceNum >= 1 && audienceNum <= 0x31) {
            BattleAudienceSoundCallKind(1);
        }
        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
            BattleAudienceSoundCallKind(1);
            BattleAudienceSoundHandBeat();
        }
        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
            BattleAudienceSoundCallKind(2);
        }
        if (audienceNum >= 0x96) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundHandBeat();
        }
        work->audienceVol += 0x14;
        if (work->audienceVol >= 0x64) {
            work->audienceVol = 0x64;
        }
        BattleAudienceSoundSetVol(4, work->audienceVol, 0x1E);
    }
    return 2;
}

void scissor_damage_sub(BattleUnit* unit, s32* out) {
    Vec pos;
    Vec point;
    Vec screen;
    f32 ratio;
    f32 halfW;
    f32 halfH;
    f32 width;
    f32 height;
    f32 sign;
    f32 nine;
    s32 i;
    s32 j;
    s32 hits = 0;

    if (unit->mAttributeFlags & kCeiling) {
        sign = -1.0f;
    } else {
        sign = 1.0f;
    }
    width = unit->cutWidth * unit->mSizeMultiplier;
    height = unit->cutHeight * unit->mSizeMultiplier;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    nine = 9.0f;
    pos.x += unit->cutBaseOffset.x * unit->mSizeMultiplier;
    pos.y += sign * (unit->cutBaseOffset.y * unit->mSizeMultiplier);
    halfH = 0.5f * -height;
    halfW = 0.5f * width;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            point.x = pos.x - halfW + ((f32)i * width) / nine;
            point.y = sign * (halfH + ((f32)j * height) / nine) + pos.y;
            point.z = pos.z;
            btlGetScreenPoint(&point, &point);
            screen = point;
            if (scissor_damage_sub2(&screen)) {
                hits++;
            }
        }
    }
    ratio = (f32)hits / 100.0f;
    *out = 0;
    if (ratio >= 0.3f) {
        *out = 1;
    }
    if (ratio >= 0.6f) {
        *out = 2;
    }
    if (ratio >= 0.9f) {
        *out = 3;
    }
}

// Match note: taking `const Vec*` and reading point->x/point->y directly (no px/py
// locals) is what lets MWCC hoist the two loads into f6/f3 and colour the float
// temporaries as the original does; caching them in locals rotated py/ny/min.
// Leaving the loop bound as work->triCount (no `count` local) is what steers the
// bound to r9 and the &tri[j] temp to r10.
s32 scissor_damage_sub2(const Vec* point) {
    ScissorWork* work = GetScissorPtr();
    f32 cross[3];
    f32 zero;
    f32 sub;
    f32 ny;
    f32 min;
    f32 cy;
    f32 cx;
    f32 d;
    f32 e;
    f32 nx;
    s32 i;
    s32 j;

    zero = 0.0f;
    for (i = 0; i < work->triCount; i++) {
        f32* tri = work->triBuf[i];
        for (j = 0; j < 3; j++) {
            s32 k = (j + 1) % 3;

            cy = tri[j + 3];
            cx = tri[j];
            sub = point->y - cy;
            min = point->x - cx;
            nx = tri[k];
            ny = tri[k + 3];
            d = nx - cx;
            e = ny - cy;
            cross[j] = min * e - sub * d;
        }
        if ((cross[0] > zero && cross[1] > zero && cross[2] > zero) ||
            (cross[0] < zero && cross[1] < zero && cross[2] < zero)) {
            return 1;
        }
    }
    return 0;
}

s32 scissor_ofscreen_ok(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    ScissorWork* work = GetScissorPtr();

    if (work->flags & 1) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

void scissor_capture(void) {
    ScissorWork* work = GetScissorPtr();

    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(work->captureBuf->address, GX_FALSE);
    GXPixModeSync();
}

#ifdef MUST_MATCH
// Inline locals are allocated after call argument temporaries: this puts the
// copied HSV input at r1+0x8 and the RGB argument at r1+0xC, matching retail.
static inline void scissorSetHSVColor(GXColor* hsv) {
    GXColor tmp = *hsv;

    GXSetChanMatColor(GX_COLOR0A0, HSV2RGB(&tmp));
}
#endif

void scissor_disp_control(CameraId cameraId, void* param) {
    ScissorWork* work = GetScissorPtr();
    f32* points = work->pointBuf;
    GXColor hsv;
    s32 i;

    work->pulse++;
    if (work->pulse >= 0x3C) {
        work->pulse -= 0x3C;
    }
    if (work->pulse <= 0x1D) {
        intplGetValue(0, work->pulse, 0x1D, 0.0f, 255.0f);
    } else {
        intplGetValue(0, work->pulse - 0x1E, 0x3B, 255.0f, 0.0f);
    }
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXLoadPosMtxImm(camGetPtr(cameraId)->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetLineWidth(0x10, GX_TO_ZERO);
    for (i = 0; i < work->pointCount - 1; i++) {
        hsv = (GXColor){ (u8)(((gp->retraceCount + i * 2) % 0x7F) << 1), 0xFF, 0xFF, 0xFF };
#ifdef MUST_MATCH
        scissorSetHSVColor(&hsv);
#else
        GXSetChanMatColor(GX_COLOR0A0, HSV2RGB(&hsv));
#endif
        GXBegin(GX_LINES, GX_VTXFMT0, 2);
        GXPosition3f32(points[i * 2], points[i * 2 + 1], 0.0f);
        GXPosition3f32(points[i * 2 + 2], points[i * 2 + 3], 0.0f);
    }
}

s32 scissor_offscreen(void) {
    ScissorWork* work = GetScissorPtr();

    dispEntry(CAMERA_OFFSCREEN2, 0.0f, 1, scissor_disp_region, NULL);
    return 2;
}

void scissor_disp_region(CameraId cameraId, void* param) {
    ScissorWork* work = GetScissorPtr();
    GXTexObj texObj;
    Mtx mtx;
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;
    f32* tri;
    s32 i;

    sysWaitDrawSync();
    GXClearBoundingBox();
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXInitTexObj(&texObj, work->captureBuf->address, 0x260, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    C_MTXLightOrtho(mtx, -240.0f, 240.0f, -304.0f, 304.0f, 0.5f, 0.5f, 0.5f, 0.5f);
    PSMTXConcat(mtx, camGetPtr(CAMERA_2D)->view, mtx);
    GXLoadTexMtxImm(mtx, 0x1E, GX_MTX3x4);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_POS, 0x1E, GX_FALSE, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_1);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, GX_PNMTX0);
    tri = *work->triBuf;
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, (u16)(work->triCount * 3));
    for (i = 0; i < work->triCount; i++) {
        GXPosition3f32(tri[0], tri[3], 0.0f);
        GXPosition3f32(tri[1], tri[4], 0.0f);
        GXPosition3f32(tri[2], tri[5], 0.0f);
        tri += 6;
    }
    sysWaitDrawSync();
    if (work->captureBuf != NULL) {
        smartFree(work->captureBuf);
        work->captureBuf = NULL;
    }
    GXReadBoundingBox(&left, &top, &right, &bottom);
    if (left == 0x3FF && right == 0 && top == 0x3FF && bottom == 0) {
        work->flags &= ~1;
    } else {
        work->flags |= 1;
    }
    offscreenAddBoundingBox(offscreenNameToId(str_ofs), left, top, right, bottom);
}

s32 scissor_init(void) {
    ScissorWork* work = GetScissorPtr();

    memset(work, 0, 0x174);
    work->timeLeft = 0x3BF;
    work->pointBuf = BattleAlloc(0x1000);
    work->triBuf = BattleAlloc(0x3000);
    work->cursorX = 0.0f;
    work->cursorY = 0.0f;
    work->comboCount = 0;
    work->audienceVol = 0;
    return 2;
}

s32 scissor_end(void) {
    ScissorWork* work = GetScissorPtr();
    effStarStoneData* data = work->starStone->userdata;

    effDelete(data->dustEffect);
    effDelete(work->starStone);
    if (work->captureBuf != NULL) {
        smartFree(work->captureBuf);
        work->captureBuf = NULL;
    }
    BattleFree(work->pointBuf);
    BattleFree(work->triBuf);
    return 2;
}

static s32 star_stone_appear(void) {
    ScissorWork* work = GetScissorPtr();
    work->state = 1;
    return 2;
}

s32 wait_star_standby(void) {
    ScissorWork* work = GetScissorPtr();

    return (work->state == 5) ? 2 : 0;
}

// ---------------------------------------------------------------------------
// Ported data for this unit. All objects are defined after the code, in the
// original section and address order, so MWCC emits the named objects ahead of
// its own literal pool.
// ---------------------------------------------------------------------------

// .sdata2:0x8..0x1C (short strings, after the colour literals' templates)
__declspec(section ".sdata2") char str_80421720[8] = "M_I_Y";
__declspec(section ".sdata2") char str_ofs[4] = "ofs";
__declspec(section ".sdata2") char str_img[4] = "img";
__declspec(section ".sdata2") char str_80421730[4] = "A_1";

// .data:0x180..0x630
EVT_BEGIN(marioAttackEvent_Scissor)
    CALL(scissor_init)
    BEGIN_SIBLING_EVENT
        CALL(star_control)
    END_SIBLING_EVENT
    CALL_CHILD(sac_start_camera_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL_CHILD(sac_mario_item_on_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START2"), EVTDAT_POINTER_MAX, 0, LW(11))
    CALL(star_stone_appear)
    BEGIN_SIBLING_EVENT
        CALL_CHILD(sac_start_stardust)
    END_SIBLING_EVENT
    WAIT_FRAMES(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(40)
    CALL_CHILD(sac_mario_item_on_end_evt)
    CALL_CHILD(sac_mario_back_home)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR(str_80421720))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 75, 105, 600, 75, 55, 0, 30, 0)
    END_SIBLING_EVENT
    CALL(wait_star_standby)
    BEGIN_SIBLING_EVENT
        CALL(scissor_timer_main)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(sac_cheer, 1)
    END_SIBLING_EVENT
    CALL(sac_ac_help_on)
    LOOP(0)
        SET_VALUE(LW(10), STRING("OFF_d_bom"))
        CALL(evt_paper_entry, LW(10))
        CALL(evt_offscreen_entry, PTR(str_ofs))
        CALL(evt_img_entry, PTR(str_img))
        CALL(evt_img_set_paper, PTR(str_img), LW(10))
        CALL(evt_img_onoff, PTR(str_img), 0)
        CALL(scissor_control)
        CALL(scissor_timer_get, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 59)
            CALL(evt_img_onoff, PTR(str_img), 0)
            CALL(evt_img_set_shadow, PTR(str_img), 0)
            CALL(evt_img_release, PTR(str_img))
            CALL(evt_offscreen_delete, PTR(str_ofs))
            CALL(evt_paper_delete, LW(10))
            GOTO(90)
        END_IF
        WAIT_FRAMES(1)
        CALL(scissor_tess)
        WAIT_FRAMES(1)
        CALL(scissor_offscreen)
        CALL(evt_img_alloc_capture, PTR(str_img), 0, PTR(str_ofs), 1, 0, 0, 0, 0)
        WAIT_FRAMES(1)
        CALL(scissor_ofscreen_ok, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_snd_sfxon, STRING("SFX_BTL_SAC_KAKONDEPON2"), 0)
            CALL(evt_img_onoff, PTR(str_img), 1)
            CALL(evt_img_set_shadow, PTR(str_img), 2)
            CALL(scissor_damage, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 2)
                CALL(btlevtcmd_ACSuccessEffect, LW(0), 75, 100, -100)
            END_IF
            CALL(evt_img_set_paper_anim, PTR(str_img), PTR(str_80421730))
            CALL(evt_img_clear_virtual_point, PTR(str_img))
            WAIT_FRAMES(60)
        END_IF
        CALL(evt_img_onoff, PTR(str_img), 0)
        CALL(evt_img_set_shadow, PTR(str_img), 0)
        CALL(evt_img_release, PTR(str_img))
        CALL(evt_offscreen_delete, PTR(str_ofs))
        CALL(evt_paper_delete, LW(10))
        CALL(scissor_timer_get, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 59)
            GOTO(90)
        END_IF
    END_LOOP
    LABEL(90)
    CALL(sac_ac_help_off)
    CALL(sac_cheer_end)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    LABEL(95)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(97)
    END_IF
    CALL(btlevtcmd_GetTotalDamage, LW(3), LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_scissor_finish), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(95)
    LABEL(97)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    CALL(scissor_end)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()
