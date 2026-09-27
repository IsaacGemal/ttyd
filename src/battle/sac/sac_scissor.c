#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

#include "battle/battle.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/offscreendrv.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include "battle/sac/sac_scissor.h"

// The original sac_scissor unit was compiled with -O4,s (optimize for size): the
// comparison idiom (`subi; subic; subfe; and`) and stmw/lmw prologues only come
// out under size optimization. There is no per-Object flag for this unit, so set
// it here (matches sac_deka.c; the orchestrator can move it to configure.py).
#pragma optimize_for_size on

extern BattleWork* _battleWorkPointer;

// Named .sdata2 constants (the original references these symbols, not anonymous literals).
extern f32 float_0_80421734;
extern f32 float_20_80421794;
extern f32 float_120_80421790;
extern f32 float_185_8042178c;
extern f32 float_320_80421788;
extern f32 float_deg2rad_80421798;
extern f32 float_1000_804217b0;
extern f32 float_neg1000_8042179c;
extern u32 dat_80421718;

// Local view of the scissor action-command work area at _battleWorkPointer + 0x1F4C.
// No header declares this struct yet; fields are listed in REPORT.md.
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
    u8 field_0xA0[0x14];          // 0xA0
    f32 field_0xB4;               // 0xB4
    f32 field_0xB8;               // 0xB8
    u8 field_0xBC[0x40];          // 0xBC
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
    u8 field_0x124[0x30];         // 0x124
    f32 field_0x154;              // 0x154
    f32 field_0x158;              // 0x158
    f32 field_0x15C;              // 0x15C
    f32 field_0x160;              // 0x160
    f32 field_0x164;              // 0x164
    f32 field_0x168;              // 0x168
    EffectEntry* field_0x16C;     // 0x16C
    smartEntry* field_0x170;      // 0x170
} ScissorWork;



s32 wait_star_standby(void) {
    ScissorWork* work = GetScissorPtr();

    return (u32)(work->state - 5u) <= 0 ? 2 : 0;
}

s32 star_stone_appear(void) {
    ScissorWork* work = GetScissorPtr();
    work->state = 1;
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

s32 scissor_init(void) {
    ScissorWork* work = GetScissorPtr();

    memset(work, 0, 0x174);
    work->field_0x104 = 0x3BF;
    work->field_0x114 = BattleAlloc(0x1000);
    work->field_0x118 = BattleAlloc(0x3000);
    work->field_0xB4 = 0.0f;
    work->field_0xB8 = 0.0f;
    work->field_0x0C = 0;
    work->field_0x98 = 0;
    return 2;
}

s32 scissor_offscreen(void) {
    ScissorWork* work = GetScissorPtr();

    dispEntry(CAMERA_OFFSCREEN2, 0.0f, 1, scissor_disp_region, NULL);
    return 2;
}

void scissor_capture(void) {
    ScissorWork* work = GetScissorPtr();

    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(work->field_0x170->address, GX_FALSE);
    GXPixModeSync();
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

// NONMATCHING (97.69%): logic matches; only float/integer register allocation
// differs (original: px=f6, py=f3, zero=f1, count=r9; here py=f7, zero=f8, count=r10).
s32 scissor_damage_sub2(Vec* point) {
    ScissorWork* work = GetScissorPtr();
    f32 cross[3];
    s32 count = work->field_0x14;
    f32 px = point->x;
    f32 py = point->y;
    f32 zero = float_0_80421734;
    s32 i;
    s32 j;

    for (i = 0; i < count; i++) {
        f32* tri = (f32*)((u8*)work->field_0x118 + i * 0x18);
        for (j = 0; j < 3; j++) {
            cross[j] = (py - tri[j + 3]) * (tri[(j + 1) % 3] - tri[j]) -
                       (px - tri[j]) * (tri[((j + 1) % 3) + 3] - tri[j + 3]);
        }
        if ((cross[0] > zero && cross[1] > zero && cross[2] > zero) ||
            (cross[0] < zero && cross[1] < zero && cross[2] < zero)) {
            return 1;
        }
    }
    return 0;
}

// NONMATCHING (94.89%): same instructions, different schedule: case 0's two lfs
// are hoisted before the stfs in the original; cases 5/15 load f1/f2 before r4.
s32 scissor_timer_main(void) {
    ScissorWork* work = GetScissorPtr();

    switch (work->field_0xFC) {
        case 0:
            work->field_0x108 = float_320_80421788;
            work->field_0x10C = float_185_8042178c;
            work->field_0xFC = 5;
            work->field_0x100 = 0;
            // fallthrough
        case 5:
            work->field_0x108 = intplGetValue(4, work->field_0x100, 0xF, float_320_80421788, float_120_80421790);
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

s32 scissor_timer_get(EventEntry* event, BOOL isFirstCall) {
    ScissorWork* work = GetScissorPtr();
    s32* args = event->args;

    if (work->field_0x104 <= 0x3B) {
        work->state = 6;
    }
    evtSetValue(event, args[0], work->field_0x104);
    return 2;
}

// Keep star_disp2D_sub out of line: star_disp2D must call (bl) it.
// NONMATCHING (0.00%): 3 instructions differ, 60 missing, 132 extra, plus register differences.
#pragma dont_inline on
void star_disp2D_sub(s32 index, s32 alpha) {
    ScissorWork* work = GetScissorPtr();
    f32* points = (f32*)((u8*)work + 0x9C);
    Mtx mTrans;
    Mtx mScale;
    Mtx mRotX;
    Mtx mRotY;
    Mtx mRotZ;
    Mtx mTemp;
    GXColor color;
    f32 x;
    f32 y;
    f32 z;
    s32 a = alpha;

    if (work->state >= 6) {
        work->field_0x120++;
        if (work->field_0x120 > 0x1E) {
            work->field_0x120 = 0x1E;
        }
        a = (s32)intplGetValue(0, work->field_0x120, 0x1E, (f32)(u8)alpha, float_0_80421734);
    }
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
    PSMTXRotRad(mRotX, 'x', work->field_0x160 * float_deg2rad_80421798);
    PSMTXRotRad(mRotY, 'y', work->field_0x164 * float_deg2rad_80421798);
    PSMTXRotRad(mRotZ, 'z', work->field_0x168 * float_deg2rad_80421798);
    PSMTXConcat(mRotZ, mRotY, mTemp);
    PSMTXConcat(mRotX, mTemp, mTemp);
    PSMTXConcat(mScale, mTemp, mTemp);
    PSMTXConcat(mTrans, mTemp, mTemp);
    *(u32*)&color = dat_80421718;
    color.a = a;
    iconDispGxCol(mTemp, 0x10, 0x19C, color);
}
#pragma dont_inline reset

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

// NONMATCHING (61.86%): 2 instructions differ, 3 missing, 18 extra, plus register differences.
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

// NONMATCHING (88.36%): 2 instructions differ, 4 extra, plus register differences.
s32 scissor_tess(void) {
    ScissorWork* work = GetScissorPtr();
    f32* points = BattleAlloc(work->field_0x9C << 3);

    memcpy(points, work->field_0x114, work->field_0x9C << 3);
    work->field_0x14 = 0;
    tess(points, work->field_0x9C);
    DCFlushRange(work->field_0x118, 0x3000);
    return 2;
}

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

// NONMATCHING (94.87%): same instructions; the original schedules `lfs f0,
// float_0_80421734` before the prologue's `stw r0, 0x14(r1)`, ours after it.
s32 scissor_intersection3(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    f32 diff = bx - ax;

    if (float_0_80421734 != diff) {
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

// Keep GetScissorPtr out of line: the original issues a call (bl) to it.
#pragma dont_inline on
ScissorWork* GetScissorPtr(void) {
    return (ScissorWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset
