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

extern const f32 float_0_80421734;
extern const f32 float_neg240_80421738;
extern const f32 float_240_8042173c;
extern const f32 float_neg304_80421740;
extern const f32 float_304_80421744;
extern const f32 float_0p5_80421748;
extern const f32 float_255_8042174c;
extern const f32 float_neg1_80421750;
extern f32 float_1_80421754;
extern const f32 float_9_80421758;
extern const f32 float_100_8042175c;
extern const f32 float_0p3_80421760;
extern const f32 float_0p6_80421764;
extern const f32 float_0p9_80421768;
extern const f32 float_42_8042176c;
extern const f32 float_50_80421770;
extern const f32 float_0p86_80421774;
extern const f32 float_neg300_80421778;
extern const f32 float_300_8042177c;
extern const f32 float_neg200_80421780;
extern const f32 float_200_80421784;
extern const f32 float_320_80421788;
extern const f32 float_185_8042178c;
extern const f32 float_120_80421790;
extern const f32 float_20_80421794;
extern const f32 float_deg2rad_80421798;
extern const f32 float_neg1000_8042179c;
extern const f32 float_37_804217a0;
extern const f32 float_2_804217a4;
extern const f32 float_2160_804217a8;
extern const f32 float_1p5_804217ac;
extern const f32 float_1000_804217b0;
extern const f32 float_1Eneg06_804217b4;
#ifdef MUST_MATCH
#define RELOAD_F32(x) (*(const volatile f32*)&(x))
#else
#define RELOAD_F32(x) (x)
#endif


extern BattleWork* _battleWorkPointer;

// Data objects defined at the end of this file and scripts owned by sac_common.
extern u32 marioAttackEvent_Scissor[];
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_mario_back_home[];
extern s32 sac_start_stardust[];
extern s32 sac_cheer[];
extern s32 sac_cheer_end[];
extern s32 sac_ac_help_on[];
extern s32 sac_ac_help_off[];

// Named .sdata2 constants (the original references these symbols, not anonymous literals).
extern const Vec vec3_802ff59c;
extern const Vec vec3_802ff5a8;
extern const Vec vec3_802ff5b4;
extern const Vec vec3_802ff5c0;
extern const Vec vec3_802ff5cc;
extern u32 dat_80421718;
extern u32 dat_8042171c;
extern char str_80421728[4];
extern u32 weapon_scissor[];

// Double constants emitted in this translation unit's .sdata2.

// A 2D point in the scissor polygon buffers (8 bytes, copied by value in tess).
typedef struct ScissorPoint {
    f32 x;
    f32 y;
} ScissorPoint;

// Local view of the scissor action-command work area at _battleWorkPointer + 0x1F4C.
// No header declares this struct yet.
typedef struct ScissorWork {
    u32 flags;                    // 0x00
    u8 field_0x04[0x08];          // 0x04
    s32 field_0x0C;               // 0x0C
    s32 field_0x10;               // 0x10
    s32 field_0x14;               // 0x14, vertex count
    s32 field_0x18[16];           // 0x18, unit ids
    s32 field_0x58[16];           // 0x58, accumulated power
    u8 field_0x98;                // 0x98
    u8 field_0x99[0x03];          // 0x99
    s32 field_0x9C;               // 0x9C, point count
    s32 field_0xA0;               // 0xA0
    f32 field_0xA4;               // 0xA4
    f32 field_0xA8;               // 0xA8
    f32 field_0xAC;               // 0xAC
    f32 field_0xB0;               // 0xB0
    f32 field_0xB4;               // 0xB4
    f32 field_0xB8;               // 0xB8
    f32 field_0xBC;               // 0xBC
    f32 field_0xC0;               // 0xC0
    u8 field_0xC4[0x10];          // 0xC4
    f32 field_0xD4;               // 0xD4
    f32 field_0xD8;               // 0xD8
    u8 field_0xDC[0x20];          // 0xDC
    s32 field_0xFC;               // 0xFC
    s32 field_0x100;              // 0x100
    s32 field_0x104;              // 0x104
    f32 field_0x108;              // 0x108
    f32 field_0x10C;              // 0x10C
    u8 field_0x110[0x04];         // 0x110
    void* field_0x114;            // 0x114
    void* field_0x118;            // 0x118
    s32 state;                    // 0x11C
    s32 field_0x120;              // 0x120
    f32 field_0x124;              // 0x124
    f32 field_0x128;              // 0x128
    f32 field_0x12C;              // 0x12C
    f32 field_0x130;              // 0x130
    f32 field_0x134;              // 0x134
    f32 field_0x138;              // 0x138
    f32 field_0x13C;              // 0x13C
    f32 field_0x140;              // 0x140
    f32 field_0x144;              // 0x144
    u8 field_0x148[0x0C];         // 0x148
    f32 field_0x154;              // 0x154
    f32 field_0x158;              // 0x158
    f32 field_0x15C;              // 0x15C
    f32 field_0x160;              // 0x160
    f32 field_0x164;              // 0x164
    f32 field_0x168;              // 0x168
    EffectEntry* field_0x16C;     // 0x16C
    smartEntry* field_0x170;      // 0x170
} ScissorWork;

// Star trail area reached through a work+0x9C pointer in the original.
// No header declares it.
typedef struct StarTrail {
    u8 field_0x00[0x18];  // 0x00
    f32 velX;             // 0x18
    f32 velY;             // 0x1C
    u8 field_0x20[0x20];  // 0x20
    f32 trailX[4];        // 0x40
    f32 trailY[4];        // 0x50
} StarTrail;

// Star-stone effect userdata written at the end of star_control.
// No header declares it.
typedef struct ScissorEffectData {
    u8 field_0x00[0x8];  // 0x00
    f32 posX;            // 0x08
    f32 posY;            // 0x0C
    f32 posZ;            // 0x10
    f32 scale;           // 0x14
    f32 rotX;            // 0x18
    f32 rotY;            // 0x1C
    f32 rotZ;            // 0x20
} ScissorEffectData;



#ifdef MUST_MATCH
#define SC_P (&points[i])
#else
#define SC_P p
#endif
#ifdef MUST_MATCH
#define SCVEC(off, sym) (*(const Vec*)(base + (off)))
#else
#define SCVEC(off, sym) (sym)
#endif

// Keep GetScissorPtr out of line: the original issues a call (bl) to it.
#pragma dont_inline on
ScissorWork* GetScissorPtr(void) {
    return (ScissorWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

#ifdef MUST_MATCH
// MUST_MATCH: calling fabs through a helper makes MWCC evaluate the second argument
// first, so the original's |dy|-before-|dx| schedule comes out.
static inline f32 scissor_abs(f32 x) {
    return __fabsf(x);
}
#else
#define scissor_abs(x) __fabs(x)
#endif

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

        if (scissor_abs(d1x) > scissor_abs(d1y)) {
            m1 = d1y / d1x;
            if (scissor_abs(d2x) > scissor_abs(d2y)) {
                m2 = d2y / d2x;
                if (m1 - m2 != 0.0f) {
                    den = m1 - m2;
                } else {
                    den = float_1Eneg06_804217b4;
                }
                *ox = ((cy - ay) + (m1 * ax - m2 * cx)) / den;
                *oy = m1 * (*ox - ax) + ay;
            } else {
#ifdef MUST_MATCH
                // MUST_MATCH: the named temp makes MWCC load 1.0 into f3 and reuse it
                // for both the compare and the subtraction, as the original does.
                f32 one = float_1_80421754;
                m2 = d2x / d2y;
                if (m1 * m2 != one) {
                    den = one - m1 * m2;
                } else {
#else
                m2 = d2x / d2y;
                if (m1 * m2 != float_1_80421754) {
                    den = float_1_80421754 - m1 * m2;
                } else {
#endif
                    den = float_1Eneg06_804217b4;
                }
                *ox = (m2 * (-m1 * ax + ay - cy) + cx) / den;
                *oy = m1 * (*ox - ax) + ay;
            }
        } else {
            m1 = d1x / d1y;
            if (scissor_abs(d2x) > scissor_abs(d2y)) {
#ifdef MUST_MATCH
                // MUST_MATCH: named temp for the same reason as the branch above.
                f32 one = float_1_80421754;
                m2 = d2y / d2x;
                if (m1 * m2 != one) {
                    den = one - m1 * m2;
                } else {
#else
                m2 = d2y / d2x;
                if (m1 * m2 != float_1_80421754) {
                    den = float_1_80421754 - m1 * m2;
                } else {
#endif
                    den = float_1Eneg06_804217b4;
                }
                *oy = (m2 * (-m1 * ay + ax - cx) + cy) / den;
                *ox = m1 * (*oy - ay) + ax;
            } else {
                m2 = d2x / d2y;
                if (m1 - m2 != 0.0f) {
                    den = m1 - m2;
                } else {
                    den = float_1Eneg06_804217b4;
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

// Keep scissor_intersection2 out of line: scissor_cross calls (bl) it in the
// original rather than inlining its body.
#pragma dont_inline on
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
#pragma dont_inline reset

#pragma dont_inline on
s32 scissor_intersection3(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    f32 diff = bx - ax;

    if (0.0f != diff) {
        f32 slope = (by - ay) / diff;
        if (ax >= bx) {
            ax += float_1000_804217b0;
            bx -= float_1000_804217b0;
            ay += float_1000_804217b0 * slope;
            by -= float_1000_804217b0 * slope;
        } else {
            ax -= float_1000_804217b0;
            bx += float_1000_804217b0;
            ay -= float_1000_804217b0 * slope;
            by += float_1000_804217b0 * slope;
        }
    } else {
        if (ay >= by) {
            ay = float_1000_804217b0;
            by = float_neg1000_8042179c;
        } else {
            ay = float_neg1000_8042179c;
            by = float_1000_804217b0;
        }
    }
    return scissor_intersection(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}
#pragma dont_inline reset

// Keep scissor_intersection3 out of line: the original issues a call (bl) to it
// from scissor_intersection4 rather than inlining the body.
#pragma dont_inline on
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
#pragma dont_inline reset

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
    xs[0] = float_neg1000_8042179c;
    count = 1;
    ys[0] = float_neg1000_8042179c;
    one = float_1_80421754;
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
    xs[count] = float_1000_804217b0;
    ys[count] = float_1000_804217b0;
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
            f32 mid = (a[0] + b[0]) * float_0p5_80421748;
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
            f32 mid = (a[1] + b[1]) * float_0p5_80421748;
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
    f32* points = BattleAlloc(work->field_0x9C << 3);

    memcpy(points, work->field_0x114, work->field_0x9C << 3);
    work->field_0x14 = 0;
    tess(points, work->field_0x9C);
    DCFlushRange(work->field_0x118, 0x3000);
    return 2;
}

s32 tess(f32* points, s32 count) {
    ScissorWork* work = GetScissorPtr();
    s32 i;
    s32 j;
    s32 k;

    if (count == 3) {
        f32* dst = (f32*)((u8*)work->field_0x118 + work->field_0x14 * 0x18);
        for (i = 0; i < 3; i++) {
            dst[i] = points[i * 2];
            dst[i + 3] = points[i * 2 + 1];
        }
        work->field_0x14++;
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
            f32* dst = (f32*)((u8*)work->field_0x118 + work->field_0x14 * 0x18);
            for (i = 0; i < 3; i++) {
                dst[i] = points[i * 2];
                dst[i + 3] = points[i * 2 + 1];
            }
            work->field_0x14++;
            BattleFree(points);
        }
    }
}

s32 weaponGetPower_Scissor(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part) {
    ScissorWork* work = GetScissorPtr();
    s32 i;

    if (unit2 == NULL) {
        return 0;
    }
    if (unit2->mAlliance == 1) {
        for (i = 0; i < 0x10; i++) {
            if (work->field_0x18[i] == BtlUnit_GetUnitId(unit2)) {
                return work->field_0x58[i];
            }
        }
    }
    return 0;
}

// The five .rodata vectors and the int->float magic double are addressed in the original from one
// common base, str_802FF570 (held in r30); without the base MWCC emits a separate lis/addi per
// vector and the FP allocation through case 1 shifts. MUST_MATCH reads them through the base;
// the natural named-symbol form is under #else. MWCC still pools the int->float magic double as
// an anonymous @NNN slot (project-wide limitation) instead of the named 4503601774854144.0
// at base+0xC8.
// With the base pointer the vector accesses line up and the remaining differences are two
// scheduling/colouring sites: the int->float magic double is loaded through an anonymous @NNN
// slot instead of the original's `lfd f1, 0xc8(r30)` (MWCC emits the i2f constant itself, never
// define it), which costs one `lis`/`lha` register; and the `work->state = 2` store in case 1 is
// scheduled one instruction before the effStarStoneEntry constant load. A hand-written i2f
// (union + base+0xC8) recovered the magic load but emitted `fsub`+`frsp` instead of `fsubs` and
// scored worse, so it is not kept.
// NONMATCHING (98.93%): the i2f double load and one case-1 store schedule differ (see above).
s32 star_control(void) {
    const u8* base = (const u8*)&str_802FF570;
    ScissorWork* work = GetScissorPtr();
    StarTrail* star = (StarTrail*)((u8*)work + 0x9C);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;
    s32 i;

    (void)base;

    if ((gp->retraceCount & 3) == 0) {
        for (i = 3; i > 0; i--) {
            star->trailX[i] = star->trailX[i - 1];
            star->trailY[i] = star->trailY[i - 1];
        }
        star->trailX[0] = star->velX;
        star->trailY[0] = star->velY;
    }
    switch (work->state) {
    case 1:
        work->state = 2;
        work->field_0x120 = 0;
        work->field_0x16C = effStarStoneEntry(3, float_0_80421734, float_neg1000_8042179c, float_0_80421734, float_1_80421754);
        BtlUnit_GetPos(mario, &work->field_0x124, &work->field_0x128, &work->field_0x12C);
        work->field_0x128 += mario->mSizeMultiplier * (f32)mario->mHeight + float_37_804217a0;
        *(Vec*)&work->field_0x130 = *(Vec*)&work->field_0x124;
        *(Vec*)&work->field_0x13C = *(Vec*)&work->field_0x124;
        work->field_0x140 += float_50_80421770;
        work->field_0x144 += float_neg1_80421750;
        *(Vec*)&work->field_0x154 = SCVEC(0x2C, vec3_802ff59c);
        *(Vec*)&work->field_0x160 = SCVEC(0x38, vec3_802ff5a8);
        /* fallthrough */
    case 2:
        work->field_0x120++;
        if (work->field_0x120 <= 0x64) {
            work->field_0x124 = intplGetValue(0, work->field_0x120, 0x64, work->field_0x130, work->field_0x13C);
            work->field_0x128 = intplGetValue(0, work->field_0x120, 0x64, work->field_0x134, work->field_0x140);
            work->field_0x12C = intplGetValue(0, work->field_0x120, 0x64, work->field_0x138, work->field_0x144);
            f = intplGetValue(0, work->field_0x120, 0x64, float_0_80421734, float_2_804217a4);
            work->field_0x15C = f;
            work->field_0x158 = f;
            work->field_0x154 = f;
        } else {
            work->field_0x124 = work->field_0x13C;
            work->field_0x128 = work->field_0x140;
            work->field_0x12C = work->field_0x144;
            work->field_0x15C = float_2_804217a4;
            work->field_0x158 = float_2_804217a4;
            work->field_0x154 = float_2_804217a4;
        }
        work->field_0x164 = intplGetValue(4, work->field_0x120, 0x78, float_0_80421734, float_2160_804217a8);
        if (work->field_0x120 >= 0x78) {
            work->state = 3;
            work->field_0x120 = 0;
            *(Vec*)&work->field_0x130 = *(Vec*)&work->field_0x124;
            work->field_0x140 = float_300_8042177c;
        }
        break;
    case 3:
        work->field_0x120++;
        work->field_0x128 = intplGetValue(1, work->field_0x120, 0x3C, work->field_0x134, work->field_0x140);
        if (work->field_0x120 >= 0x3C) {
            work->state = 4;
            work->field_0x120 = 0;
            {
                *(Vec*)&work->field_0x124 = SCVEC(0x44, vec3_802ff5b4);
                *(Vec*)&work->field_0x154 = SCVEC(0x50, vec3_802ff5c0);
                *(Vec*)&work->field_0x160 = SCVEC(0x5C, vec3_802ff5cc);
                work->field_0x134 = work->field_0x128;
                work->field_0x140 = float_0_80421734;
                star->velX = work->field_0x124;
                star->velY = work->field_0x128;
                for (i = 0; i < 4; i++) {
                    star->trailX[i] = float_0_80421734;
                    star->trailY[i] = float_neg1000_8042179c;
                }
            }
        }
        break;
    case 4:
        work->field_0x120++;
        work->field_0x128 = intplGetValue(4, work->field_0x120, 0x3C, work->field_0x134, work->field_0x140);
        star->velX = work->field_0x124;
        star->velY = work->field_0x128;
        if (work->field_0x120 >= 0x3C) {
            work->state = 5;
            work->field_0x120 = 0;
        }
        break;
    case 5:
    case 6:
        break;
    }
    if (work->field_0x16C != NULL) {
        ScissorEffectData* data = work->field_0x16C->userdata;

        if (work->state >= 4) {
            f32 zero = float_0_80421734;

            data->posX = zero;
            data->posY = float_neg1000_8042179c;
            data->posZ = zero;
        } else {
            data->posX = work->field_0x124;
            data->posY = work->field_0x128;
            data->posZ = work->field_0x12C;
        }
        {
            f32 s = float_1p5_804217ac;

            data->rotX = work->field_0x160;
            data->rotY = work->field_0x164;
            data->rotZ = work->field_0x168;
            data->scale = s * work->field_0x154;
        }
    }
    if (work->state >= 4 && work->state <= 6) {
        dispEntry(8, float_0_80421734, 2, (DispCallback)star_disp2D, NULL);
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

#pragma dont_inline on
void star_disp2D_sub(s32 index, s32 alpha) {
    ScissorWork* work = GetScissorPtr();
    f32* points = (f32*)((u8*)work + 0x9C);
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
#ifdef MUST_MATCH
        s32 step;
        f32 start;
#endif
        work->field_0x120++;
        if (work->field_0x120 > 0x1E) {
            work->field_0x120 = 0x1E;
        }
#ifdef MUST_MATCH
        alpha = (s32)intplGetValue(0, (step = work->field_0x120, step), 0x1E, (start = (f32)(u32)(u8)alpha, start), float_0_80421734);
#else
        alpha = (s32)intplGetValue(0, work->field_0x120, 0x1E, (f32)(u32)(u8)alpha, float_0_80421734);
#endif
    }
    *(u32*)&color = dat_80421718;
    color.a = alpha;
    if (index == 0) {
        x = points[6];
        y = points[7];
        z = float_0_80421734;
    } else {
        x = *(f32*)((u8*)points + index * 4 + 0x3C);
        y = *(f32*)((u8*)points + index * 4 + 0x4C);
        z = float_0_80421734;
    }
    PSMTXTrans(mTrans, x, y - float_20_80421794, z);
    PSMTXScale(mScale, work->field_0x154, work->field_0x158, work->field_0x15C);
    PSMTXRotRad(mRotX, 'x', float_deg2rad_80421798 * work->field_0x160);
    PSMTXRotRad(mRotY, 'y', float_deg2rad_80421798 * work->field_0x164);
    PSMTXRotRad(mRotZ, 'z', float_deg2rad_80421798 * work->field_0x168);
    PSMTXConcat(mRotZ, mRotY, mTemp);
    PSMTXConcat(mRotX, mTemp, mTemp);
    PSMTXConcat(mScale, mTemp, mTemp);
    PSMTXConcat(mTrans, mTemp, mTemp);
    iconDispGxCol(mTemp, 0x10, 0x19C, color);
}
#pragma dont_inline reset

s32 scissor_timer_get(EventEntry* event, BOOL isFirstCall) {
    ScissorWork* work = GetScissorPtr();
    s32* args = event->args;

    if (work->field_0x104 <= 0x3B) {
        work->state = 6;
    }
    evtSetValue(event, args[0], work->field_0x104);
    return 2;
}

s32 scissor_timer_main(void) {
    ScissorWork* work = GetScissorPtr();

    switch (work->field_0xFC) {
        case 0:
        {
            work->field_0x108 = float_320_80421788;
            work->field_0x10C = float_185_8042178c;
            work->field_0xFC = 5;
            work->field_0x100 = 0;
        }
            // fallthrough
        case 5:
            work->field_0x108 = intplGetValue(4, work->field_0x100, 0xF, RELOAD_F32(float_320_80421788), float_120_80421790);
            work->field_0x100++;
            if (work->field_0x100 > 0xF) {
                work->field_0xFC = 10;
            }
            break;
        case 10:
            work->field_0x104--;
            if (work->field_0x104 <= 0x3B) {
                work->field_0xFC = 0xF;
                work->field_0x100 = 0;
            }
            break;
        case 15:
            work->field_0x108 = intplGetValue(4, work->field_0x100, 0xF, float_120_80421790, float_320_80421788);
            work->field_0x100++;
            if (work->field_0x100 > 0xF) {
                work->field_0xFC = 0x14;
            }
            work->field_0x104--;
            if (work->field_0x104 < 0) {
                work->field_0x104 = 0;
            }
            break;
        case 20:
            work->field_0x104--;
            if (work->field_0x104 < 0) {
                work->field_0x104 = 0;
                return 2;
            }
            break;
    }
    return 0;
}

// MUST_MATCH: re-deriving &points[i] at each use (instead of the cached `p`) gives retail's register use
// (permuter). The natural build uses `p`.
s32 scissor_control(s32 unused, s32 reset) {
    ScissorWork* work = GetScissorPtr();
    ScissorPoint* points = (ScissorPoint*)work->field_0x114;
    ScissorPoint* p;
    s32 i;
    s32 offset;

    if (reset != 0) {
        work->field_0x9C = 0;
        work->field_0xA8 = float_0_80421734;
        work->field_0xA4 = float_0_80421734;
        work->field_0xB0 = float_0_80421734;
        work->field_0xAC = float_0_80421734;
        work->field_0xA0 = 0;
        work->field_0xD4 = float_0_80421734;
        work->field_0xD8 = float_0_80421734;
        memset(points, 0, 0x1000);
        work->flags &= ~1;
    }
    work->field_0xA4 = work->field_0xAC;
    work->field_0xA8 = work->field_0xB0;
    work->field_0xAC = (f32)keyGetStickX(0);
    work->field_0xB0 = (f32)keyGetStickY(0);
    {
        f32 mag = (f32)sqrt(work->field_0xA4 * work->field_0xA4 + work->field_0xA8 * work->field_0xA8);
        mag = (f32)sqrt(work->field_0xAC * work->field_0xAC + work->field_0xB0 * work->field_0xB0);
        if (mag > float_42_8042176c) {
            work->field_0xD4 += work->field_0xAC / float_50_80421770;
            work->field_0xD8 += work->field_0xB0 / float_50_80421770;
        }
    }
    work->field_0xD4 *= float_0p86_80421774;
    work->field_0xD8 *= float_0p86_80421774;
    work->field_0xB4 += work->field_0xD4;
    work->field_0xB8 += work->field_0xD8;
    if (work->field_0xB4 < -300.0f) {
        work->field_0xB4 = -300.0f;
    }
    if (work->field_0xB4 > 300.0f) {
        work->field_0xB4 = 300.0f;
    }
    if (work->field_0xB8 < -200.0f) {
        work->field_0xB8 = -200.0f;
    }
    if (work->field_0xB8 > 200.0f) {
        work->field_0xB8 = 200.0f;
    }
    if (work->field_0x104 <= 0x3B) {
        work->state = 6;
        return 2;
    }
    if (gp->retraceCount % 6 == 0) {
        if (work->field_0x9C < 0x200) {
            if (work->field_0xBC != work->field_0xB4 || work->field_0xC0 != work->field_0xB8) {
                if (__fabs(work->field_0xB4 - work->field_0xBC) >= 5.0 || __fabs(work->field_0xB8 - work->field_0xC0) >= 5.0) {
                    psndSFXOn("SFX_BTL_SAC_KAKONDEPON1");
                }
                points[work->field_0x9C].x = work->field_0xB4;
                points[work->field_0x9C].y = work->field_0xB8;
                work->field_0xBC = work->field_0xB4;
                work->field_0xC0 = work->field_0xB8;
                work->field_0x9C++;
            }
        }
    }
    if (work->field_0x9C > 3) {
        for (i = 0, offset = 0; i < work->field_0x9C - 3; i++, offset += 8) {
            f32 ox;
            f32 oy;
            p = &points[i];

            if (scissor_intersection(SC_P->x, SC_P->y, SC_P[1].x, SC_P[1].y,
                                     points[work->field_0x9C - 2].x, points[work->field_0x9C - 2].y, points[work->field_0x9C - 1].x, points[work->field_0x9C - 1].y,
                                     &ox, &oy) != 0) {
                s32 k;

                SC_P->x = points[work->field_0x9C - 1].x = ox;
                SC_P->y = points[work->field_0x9C - 1].y = oy;
                for (k = 0; k < i; k++) {
                    points[k] = *SC_P;
                }
                if (work->field_0x170 != NULL) {
                    smartFree(work->field_0x170);
                    work->field_0x170 = NULL;
                }
                work->field_0x170 = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, 4, 0, 0), 1);
                dispEntry(8, float_0_80421734, 0, (DispCallback)scissor_capture, 0);
                return 2;
            }
        }
    }
    dispEntry(8, float_0_80421734, 0, (DispCallback)scissor_disp_control, 0);
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
            if ((s8)part->defenseAttr->defenseAttrs[0] == 4 || (s8)part->defenseAttr->defenseAttrs[0] == 2) {
                continue;
            }
            scissor_damage_sub(unit, &out);
            work->field_0x18[count] = unitIndex;
            work->field_0x58[count] = out;
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

        if (work->field_0x18[i] == -1) {
            continue;
        }
        if (work->field_0x58[i] < 1) {
            continue;
        }
        target = BattleGetUnitPtr(battleWork, work->field_0x18[i]);
        part = BtlUnit_GetPartsPtr(target, BtlUnit_GetBodyPartsId(target));
        BattleCheckDamage(mario, target, part, (BattleWeapon*)weapon_scissor, 0);
        damage = 1;
    }
    if (allHit == 1) {
        s32 value;

        work->field_0x0C++;
        value = work->field_0x0C + 1;
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
        work->field_0x98 += 0x14;
        if (work->field_0x98 >= 0x64) {
            work->field_0x98 = 0x64;
        }
        BattleAudienceSoundSetVol(4, work->field_0x98, 0x1E);
    }
    return 2;
}

// Remaining diffs are the unnamed expression webs (`@NNN`) wanting f0 but coloured f1; the named
// locals now match (halfH=f30, width=f29, height=f28, halfW=f31, nine=f24, rowBase=f25, sign=f27,
// mult=f4). `mult` reproduces the original's reload of `mSizeMultiplier` into f4 after
// BtlUnit_GetPos. Declaration order was hill-climbed (`rasim.py search 32 42 --fpr`) and
// `neighbors` finds no single edge to fix; NO-SOURCE-LEVER: permuter case.
// NONMATCHING (97.23%): unnamed expression webs want f0 but are coloured f1.
void scissor_damage_sub(BattleUnit* unit, s32* out) {
    Vec pos;
    Vec point;
    Vec screen;
    f32 halfH;
    f32 width;
    f32 sign;
    f32 ratio;
    f32 height;
    f32 halfW;
    f32 nine;
    f32 rowBase;
    s32 i;
    s32 j;
    s32 hits = 0;

    sign = (unit->mAttributeFlags & kCeiling) ? float_neg1_80421750 : float_1_80421754;
    width = unit->cutWidth * unit->mSizeMultiplier;
    height = unit->cutHeight * unit->mSizeMultiplier;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    halfH = float_0p5_80421748 * -height;
    halfW = float_0p5_80421748 * width;
    nine = float_9_80421758;
    {
        f32 mult = RELOAD_F32(unit->mSizeMultiplier);
        pos.x += unit->cutBaseOffset.x * mult;
        pos.y += sign * (unit->cutBaseOffset.y * mult);
    }
    for (i = 0; i < 10; i++) {
        rowBase = ((f32)i * width) / nine;
        for (j = 0; j < 10; j++) {
            point.x = pos.x - halfW + rowBase;
            point.z = pos.z;
            point.y = sign * (halfH + ((f32)j * height) / nine) + pos.y;
            btlGetScreenPoint(&point, &point);
            screen = point;
            if (scissor_damage_sub2(&screen)) {
                hits++;
            }
        }
    }
    ratio = (f32)hits / float_100_8042175c;
    *out = 0;
    if (ratio >= float_0p3_80421760) {
        *out = 1;
    }
    if (ratio >= float_0p6_80421764) {
        *out = 2;
    }
    if (ratio >= float_0p9_80421768) {
        *out = 3;
    }
}

// Keep scissor_damage_sub2 out of line: scissor_damage_sub calls (bl) it in the original.
#pragma dont_inline on
// Match note: taking `const Vec*` and reading point->x/point->y directly (no px/py
// locals) is what lets MWCC hoist the two loads into f6/f3 and colour the float
// temporaries as the original does; caching them in locals rotated py/ny/min.
// Leaving the loop bound as work->field_0x14 (no `count` local) is what steers the
// bound to r9 and the &tri[j] temp to r10.  The 0.0f literal pools as a local symbol
// instead of the original's float_0_80421734, which is not scored as a mismatch.
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
    for (i = 0; i < work->field_0x14; i++) {
        f32* tri = (f32*)((u8*)work->field_0x118 + i * 0x18);
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
#pragma dont_inline reset

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
    GXCopyTex(work->field_0x170->address, GX_FALSE);
    GXPixModeSync();
}

#ifdef MUST_MATCH
#define SCISSOR_HSV2RGB(h) ((GXColor (*)(GXColor*))HSV2RGB)(h)
#else
static inline GXColor SCISSOR_HSV2RGB(GXColor* h) {
    GXColor c;

    *(u32*)&c = HSV2RGB(h);
    return c;
}
#endif
// The original keeps the built HSV colour at r1+0x8 and GXSetChanMatColor's outgoing
// argument at r1+0xC; ours has the argument at r1+0x8 and the colour at r1+0xC (same
// instructions, same frame). The original materialises HSV2RGB's result directly in the
// argument slot; calling it through a GXColor-returning pointer removes the extra copy
// the u32 return type would add. The hsvTmp copy reproduces the original's
// build-in-a-temp then copy-into-the-local shape.
// NONMATCHING (99.97%): the two stack slots are swapped (see above).
void scissor_disp_control(CameraId cameraId, void* param) {
    ScissorWork* work = GetScissorPtr();
    f32* points = work->field_0x114;
    GXColor hsv;
#ifdef MUST_MATCH
    GXColor hsvTmp;
#endif
    s32 i;

    work->field_0x10++;
    if (work->field_0x10 >= 0x3C) {
        work->field_0x10 -= 0x3C;
    }
    if (work->field_0x10 <= 0x1D) {
        intplGetValue(0, work->field_0x10, 0x1D, RELOAD_F32(float_0_80421734), float_255_8042174c);
    } else {
        intplGetValue(0, work->field_0x10 - 0x1E, 0x3B, float_255_8042174c, float_0_80421734);
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
    for (i = 0; i < work->field_0x9C - 1; i++) {
        hsv = *(GXColor*)&dat_8042171c;
        hsv.r = (u8)(((gp->retraceCount + i * 2) % 0x7F) << 1);
#ifdef MUST_MATCH
        hsvTmp = hsv;
        GXSetChanMatColor(GX_COLOR0A0, SCISSOR_HSV2RGB(&hsvTmp));
#else
        GXSetChanMatColor(GX_COLOR0A0, SCISSOR_HSV2RGB(&hsv));
#endif
        GXBegin(GX_LINES, GX_VTXFMT0, 2);
        GXPosition3f32(points[i * 2], points[i * 2 + 1], 0.0f);
        GXPosition3f32(points[i * 2 + 2], points[i * 2 + 3], 0.0f);
    }
}

s32 scissor_offscreen(void) {
    ScissorWork* work = GetScissorPtr();

    dispEntry(CAMERA_OFFSCREEN2, float_0_80421734, 1, scissor_disp_region, NULL);
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
    GXInitTexObj(&texObj, work->field_0x170->address, 0x260, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_80421734, float_0_80421734, float_0_80421734, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    C_MTXLightOrtho(mtx, float_neg240_80421738, float_240_8042173c, float_neg304_80421740, float_304_80421744, float_0p5_80421748, float_0p5_80421748, float_0p5_80421748, float_0p5_80421748);
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
    tri = (f32*)work->field_0x118;
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, (u16)(work->field_0x14 * 3));
    for (i = 0; i < work->field_0x14; i++) {
        GXPosition3f32(tri[0], tri[3], 0.0f);
        GXPosition3f32(tri[1], tri[4], 0.0f);
        GXPosition3f32(tri[2], tri[5], 0.0f);
        tri += 6;
    }
    sysWaitDrawSync();
    if (work->field_0x170 != NULL) {
        smartFree(work->field_0x170);
        work->field_0x170 = NULL;
    }
    GXReadBoundingBox(&left, &top, &right, &bottom);
    if (left == 0x3FF && right == 0 && top == 0x3FF && bottom == 0) {
        work->flags &= ~1;
    } else {
        work->flags |= 1;
    }
    offscreenAddBoundingBox(offscreenNameToId(str_80421728), left, top, right, bottom);
}

s32 scissor_init(void) {
    ScissorWork* work = GetScissorPtr();

    memset(work, 0, 0x174);
    work->field_0x104 = 0x3BF;
    work->field_0x114 = BattleAlloc(0x1000);
    work->field_0x118 = BattleAlloc(0x3000);
    work->field_0xB4 = float_0_80421734;
    work->field_0xB8 = float_0_80421734;
    work->field_0x0C = 0;
    work->field_0x98 = 0;
    return 2;
}

s32 scissor_end(void) {
    ScissorWork* work = GetScissorPtr();

    effDelete(*(EffectEntry**)((u8*)work->field_0x16C->userdata + 0x34));
    effDelete(work->field_0x16C);
    if (work->field_0x170 != NULL) {
        smartFree(work->field_0x170);
        work->field_0x170 = NULL;
    }
    BattleFree(work->field_0x114);
    BattleFree(work->field_0x118);
    return 2;
}

s32 star_stone_appear(void) {
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

// .rodata:0x0..0xC8 (own strings and the vectors scissor_disp_region reads)
__declspec(section ".rodata") char str_802FF570[20] = "btl_wn_sac_scissor";
const char str_802FF584[12] = "msg_scissor";
const char str_802FF590[12] = "msg_sac_5";
#pragma force_active on
const Vec vec3_802ff59c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802ff5a8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802ff5b4 = { 0.0f, 300.0f, 0.0f };
const Vec vec3_802ff5c0 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802ff5cc = { 0.0f, 0.0f, 0.0f };
#pragma force_active reset
const char str_802FF5D8[20] = "SFX_BTL_SAC_START1";
const char str_802FF5EC[20] = "SFX_BTL_SAC_START2";
const char str_802FF600[20] = "SFX_BTL_SAC_START3";
const char str_802FF614[12] = "OFF_d_bom";
const char str_802FF620[24] = "SFX_BTL_SAC_KAKONDEPON2";

// .sdata2:0x0..0x1C (colour words and short strings)
__declspec(section ".sdata2") u32 dat_80421718 = 0xFFFFFF00;
__declspec(section ".sdata2") u32 dat_8042171c = 0x00FFFFFF;
__declspec(section ".sdata2") char str_80421720[8] = "M_I_Y";
__declspec(section ".sdata2") char str_80421728[4] = "ofs";
__declspec(section ".sdata2") char str_8042172C[4] = "img";
__declspec(section ".sdata2") char str_80421730[4] = "A_1";

// .sdata2:0x1C..0xA0 (the unit's named float constants)
#pragma explicit_zero_data on
const f32 float_0_80421734 = 0.0f;
#pragma explicit_zero_data reset
const f32 float_neg240_80421738 = -240.f;
const f32 float_240_8042173c = 240.f;
const f32 float_neg304_80421740 = -304.0f;
const f32 float_304_80421744 = 304.0f;
const f32 float_0p5_80421748 = 0.5f;
const f32 float_255_8042174c = 255.0f;
const f32 float_neg1_80421750 = -1.0f;
__declspec(section ".sdata2") f32 float_1_80421754 = 1.0f;
const f32 float_9_80421758 = 9.0f;
const f32 float_100_8042175c = 100.f;
const f32 float_0p3_80421760 = 0.3f;
const f32 float_0p6_80421764 = 0.6f;
const f32 float_0p9_80421768 = 0.9f;
const f32 float_42_8042176c = 42.0f;
const f32 float_50_80421770 = 50.f;
const f32 float_0p86_80421774 = 0.86f;
const f32 float_neg300_80421778 = -300.f;
const f32 float_300_8042177c = 300.f;
const f32 float_neg200_80421780 = -200.f;
const f32 float_200_80421784 = 200.f;
const f32 float_320_80421788 = 320.f;
const f32 float_185_8042178c = 185.0f;
const f32 float_120_80421790 = 120.f;
const f32 float_20_80421794 = 20.f;
const f32 float_deg2rad_80421798 = 0.017453292f;
const f32 float_neg1000_8042179c = -1000.f;
const f32 float_37_804217a0 = 37.0f;
const f32 float_2_804217a4 = 2.0f;
const f32 float_2160_804217a8 = 2160.f;
const f32 float_1p5_804217ac = 1.5f;
const f32 float_1000_804217b0 = 1000.f;
const f32 float_1Eneg06_804217b4 = 0.000001f;

// .data:0x0..0xC0
u32 weapon_scissor[48] = {
    (u32)(void*)str_802FF570, 0x019C0000, 0x00000000, (u32)(void*)str_802FF584,
    0x64000400, 0x3F800000, 0x01056432, (u32)(void*)&weaponGetPower_Scissor,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
    0x02101260, 0x20000000, 0x00000302, (u32)(void*)str_802FF590,
    0x000200C8, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)(void*)marioAttackEvent_Scissor, 0x00000064, 0x00000000, 0x00000000,
};

// .data:0xC0..0x180
u32 weapon_scissor_finish[48] = {
    (u32)(void*)str_802FF570, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01056432, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
    0x02101260, 0x20000000, 0x001B0302, 0x00000000,
    0x00020088, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// .data:0x180..0x630
u32 marioAttackEvent_Scissor[300] = {
    0x0001005B, (u32)(void*)&scissor_init, 0x0000006E, 0x0001005B,
    (u32)(void*)&star_control, 0x00000070, 0x0001005E, (u32)(void*)&sac_start_camera_evt,
    0x0001000A, 0x000001F4, 0x0006005B, (u32)(void*)&btlevtcmd_snd_se,
    0xFFFFFFFE, (u32)(void*)str_802FF5D8, 0xF1194D80, 0x00000000,
    0xF1194D80, 0x0001005E, (u32)(void*)&sac_mario_item_on_evt, 0x0001000A,
    0x000001F4, 0x0006005B, (u32)(void*)&btlevtcmd_snd_se, 0xFFFFFFFE,
    (u32)(void*)str_802FF5EC, 0xF1194D80, 0x00000000, 0xFE363C8B,
    0x0001005B, (u32)(void*)&star_stone_appear, 0x0000006E, 0x0001005E,
    (u32)(void*)&sac_start_stardust, 0x00000070, 0x00010009, 0x00000064,
    0x0006005B, (u32)(void*)&btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)str_802FF600,
    0xF1194D80, 0x00000000, 0xF1194D80, 0x0002005B,
    (u32)(void*)&evt_snd_sfxoff, 0xFE363C8B, 0x0003005B, (u32)(void*)&evt_btl_camera_set_mode,
    0x00000000, 0x00000000, 0x00010009, 0x00000028,
    0x0001005E, (u32)(void*)&sac_mario_item_on_end_evt, 0x0001005E, (u32)(void*)&sac_mario_back_home,
    0x0004005B, (u32)(void*)&btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001,
    (u32)(void*)str_80421720, 0x0000006E, 0x00010009, 0x0000001E,
    0x0003005B, (u32)(void*)&evt_btl_camera_set_mode, 0x00000000, 0x00000003,
    0x000A005B, (u32)(void*)&evt_btl_camera_set_moveto, 0x00000000, 0x0000004B,
    0x00000069, 0x00000258, 0x0000004B, 0x00000037,
    0x00000000, 0x0000001E, 0x00000000, 0x00000070,
    0x0001005B, (u32)(void*)&wait_star_standby, 0x0000006E, 0x0001005B,
    (u32)(void*)&scissor_timer_main, 0x00000070, 0x0000006E, 0x0002005B,
    (u32)(void*)&sac_cheer, 0x00000001, 0x00000070, 0x0001005B,
    (u32)(void*)&sac_ac_help_on, 0x00010005, 0x00000000, 0x00020032,
    0xFE363C8A, (u32)(void*)str_802FF614, 0x0002005B, (u32)(void*)&evt_paper_entry,
    0xFE363C8A, 0x0002005B, (u32)(void*)&evt_offscreen_entry, (u32)(void*)str_80421728,
    0x0002005B, (u32)(void*)&evt_img_entry, (u32)(void*)str_8042172C, 0x0003005B,
    (u32)(void*)&evt_img_set_paper, (u32)(void*)str_8042172C, 0xFE363C8A, 0x0003005B,
    (u32)(void*)&evt_img_onoff, (u32)(void*)str_8042172C, 0x00000000, 0x0001005B,
    (u32)(void*)&scissor_control, 0x0002005B, (u32)(void*)&scissor_timer_get, 0xFE363C80,
    0x0002001C, 0xFE363C80, 0x0000003B, 0x0003005B,
    (u32)(void*)&evt_img_onoff, (u32)(void*)str_8042172C, 0x00000000, 0x0003005B,
    (u32)(void*)&evt_img_set_shadow, (u32)(void*)str_8042172C, 0x00000000, 0x0002005B,
    (u32)(void*)&evt_img_release, (u32)(void*)str_8042172C, 0x0002005B, (u32)(void*)&evt_offscreen_delete,
    (u32)(void*)str_80421728, 0x0002005B, (u32)(void*)&evt_paper_delete, 0xFE363C8A,
    0x00010004, 0x0000005A, 0x00000021, 0x00010009,
    0x00000001, 0x0001005B, (u32)(void*)&scissor_tess, 0x00010009,
    0x00000001, 0x0001005B, (u32)(void*)&scissor_offscreen, 0x0009005B,
    (u32)(void*)&evt_img_alloc_capture, (u32)(void*)str_8042172C, 0x00000000, (u32)(void*)str_80421728,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00010009, 0x00000001, 0x0002005B,
    (u32)(void*)&scissor_ofscreen_ok, 0xFE363C80, 0x00020019, 0xFE363C80,
    0x00000000, 0x0003005B, (u32)(void*)&evt_snd_sfxon, (u32)(void*)str_802FF620,
    0x00000000, 0x0003005B, (u32)(void*)&evt_img_onoff, (u32)(void*)str_8042172C,
    0x00000001, 0x0003005B, (u32)(void*)&evt_img_set_shadow, (u32)(void*)str_8042172C,
    0x00000002, 0x0002005B, (u32)(void*)&scissor_damage, 0xFE363C80,
    0x0002001D, 0xFE363C80, 0x00000002, 0x0005005B,
    (u32)(void*)&btlevtcmd_ACSuccessEffect, 0xFE363C80, 0x0000004B, 0x00000064,
    0xFFFFFF9C, 0x00000021, 0x0003005B, (u32)(void*)&evt_img_set_paper_anim,
    (u32)(void*)str_8042172C, (u32)(void*)str_80421730, 0x0002005B, (u32)(void*)&evt_img_clear_virtual_point,
    (u32)(void*)str_8042172C, 0x00010009, 0x0000003C, 0x00000021,
    0x0003005B, (u32)(void*)&evt_img_onoff, (u32)(void*)str_8042172C, 0x00000000,
    0x0003005B, (u32)(void*)&evt_img_set_shadow, (u32)(void*)str_8042172C, 0x00000000,
    0x0002005B, (u32)(void*)&evt_img_release, (u32)(void*)str_8042172C, 0x0002005B,
    (u32)(void*)&evt_offscreen_delete, (u32)(void*)str_80421728, 0x0002005B, (u32)(void*)&evt_paper_delete,
    0xFE363C8A, 0x0002005B, (u32)(void*)&scissor_timer_get, 0xFE363C80,
    0x0002001C, 0xFE363C80, 0x0000003B, 0x00010004,
    0x0000005A, 0x00000021, 0x00000006, 0x00010003,
    0x0000005A, 0x0001005B, (u32)(void*)&sac_ac_help_off, 0x0001005B,
    (u32)(void*)&sac_cheer_end, 0x0003005B, (u32)(void*)&btlevtcmd_GetSelectEnemy, 0xFE363C83,
    0xFE363C84, 0x0001005B, (u32)(void*)&btlevtcmd_WaitGuardMove, 0x0002005B,
    (u32)(void*)&btlevtcmd_CommandPayWeaponCost, 0xFFFFFFFE, 0x0003005B, (u32)(void*)&btlevtcmd_CommandGetWeaponAddress,
    0xFFFFFFFE, 0xFE363C80, 0x0002005B, (u32)(void*)&btlevtcmd_WeaponAftereffect,
    0xFE363C80, 0x00010003, 0x0000005F, 0x00020018,
    0xFE363C83, 0xFFFFFFFF, 0x00010004, 0x00000061,
    0x00000021, 0x0003005B, (u32)(void*)&btlevtcmd_GetTotalDamage, 0xFE363C83,
    0xFE363C80, 0x0002001D, 0xFE363C80, 0x00000001,
    0x0007005B, (u32)(void*)&btlevtcmd_CheckDamage, 0xFFFFFFFE, 0xFE363C83,
    0xFE363C84, (u32)(void*)weapon_scissor_finish, 0x00000100, 0xFE363C85,
    0x00000021, 0x0003005B, (u32)(void*)&btlevtcmd_GetSelectNextEnemy, 0xFE363C83,
    0xFE363C84, 0x00010004, 0x0000005F, 0x00010003,
    0x00000061, 0x0003005B, (u32)(void*)&evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0001000A, 0x000003E8, 0x0001005B,
    (u32)(void*)&scissor_end, 0x00010003, 0x00000063, 0x0002005B,
    (u32)(void*)&btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001,
};

// .data:0x684 (trailing zero word before the next split's data)
#pragma force_active on
__declspec(section ".data") s32 gap_05_803B24BC_data[1] = { 0x00000000 };
#pragma force_active reset
