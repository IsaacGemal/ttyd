#include "las_04.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/itemdrv.h"
#include "driver/shadowdrv.h"
#include "eff/eff_kemutest.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "math_ext.h"
#include "memory.h"
#include "motion/mot_damage.h"
#include "party/party_motion.h"
#include "party/party_vivian.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_03", "e_bero_1", 5, 5, w_door_close, w_door_open},
    {"e_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_05", "w_bero", 5, 5, e_door_close, e_door_open},
    {NULL},
};

const Vec vec3_las_00011ed4 = {0.0f, 1.0f, 0.0f};
const Vec vec3_las_00011ee0 = {0.0f, 0.0f, 0.0f};

EVT_BEGIN(firebar_coin)
    SWITCH(LW(2))
        CASE_EQUAL(0)
        SET_VALUE(LW(3), 0)
        SET_VALUE(LW(4), 360)
        CASE_LESS(0)
        SET_VALUE(LW(3), 90)
        SET_VALUE(LW(4), 270)
        CASE_GREATER(0)
        SET_VALUE(LW(3), 270)
        SET_VALUE(LW(4), 450)
    END_SWITCH
    SET_VALUE(LW(5), LW(4))
    SUBTRACT(LW(5), LW(3))
    DIVIDE(LW(5), 6)
    SET_VALUE(LW(6), LW(3))
    SET_VALUE(LW(7), LW(5))
    DIVIDE(LW(7), 2)
    ADD(LW(6), LW(7))
    LOOP(5)
        CALL(evt_sub_random, LW(5), LW(7))
        ADD(LW(6), LW(7))
        CALL(coin_entry, LW(0), LW(1), LW(2), LW(6))
        WAIT_MS(200)
    END_LOOP
    RETURN
EVT_END()

// clang-format off
FirebarDesc firebardesc[7] = {
    {0, 5, 0, {-700.0f, 80.0f, -55.0f}, 25.0f, 25.0f, 0.04f, firebar_cb, 0.0f, 0.0f},
    {0, 5, 0, {-500.0f, 80.0f, 55.0f}, 25.0f, 25.0f, -0.04f, firebar_cb, 0.0f, 0.0f},
    {0, 5, 0, {-300.0f, 80.0f, -55.0f}, 25.0f, 25.0f, 0.04f, firebar_cb, 0.0f, 0.0f},
    {0, 4, 0, {0.0f, 80.0f, 0.0f}, 25.0f, 25.0f, 0.03f, firebar_cb, 0.0f, 30.0f},
    {0, 4, 0, {0.0f, 80.0f, 0.0f}, 25.0f, 25.0f, 0.03f, firebar_cb, 3.1415927f, 30.0f},
    {1, 5, 0, {800.0f, -10.0f, -50.0f}, 25.0f, 25.0f, 1.5f, firebar_cb, 225.0f, 0.0f},
    {1, 5, 0, {800.0f, 15.0f, -50.0f}, 25.0f, 25.0f, 1.5f, firebar_cb, 225.0f, 0.0f},
};

EVT_BEGIN(firebar_evt)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[0]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[1]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[2]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[3]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[4]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[5]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[6]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT_GET_ID(LW(0))
        CALL(firebar_entry, PTR(&firebardesc[5]))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(firebar_disp_evt)
    END_INLINE_EVENT
    RETURN
EVT_END()

static HitDamageReturn dmgRetPoint[4] = {
    {"A_oti_01", {0.0f, -1000.0f, 0.0f}},
    {"A_oti_02", {0.0f, -1000.0f, 0.0f}},
    {"A_oti_03", {0.0f, -1000.0f, 0.0f}},
    {NULL, {0.0f, 0.0f, 0.0f}},
};

EVT_BEGIN(las_04_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS2"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hit_damage_return_set, PTR(dmgRetPoint))
    CALL_CHILD(evt_gazigazi_entry)
    SPAWN_EVENT(firebar_evt)
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("hamon"), 1, 0)
    CALL(evt_mobj_brick, STRING("mobj_01"), 0, 150, 0, 134, 11, 0, GSWF(4384))
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -950, 140, -100, 0, 0)
    RETURN
EVT_END()

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

#define FB_NAN (*(f32*)__float_nan)

static inline s32 fb_fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&x & 0x007FFFFF) {
            return FP_NAN;
        } else {
            return FP_INFINITE;
        }
    case 0:
        if (*(s32*)&x & 0x007FFFFF) {
            return FP_SUBNORMAL;
        } else {
            return FP_ZERO;
        }
    default:
        return FP_NORMAL;
    }
}

#define fb_isnan(x) (fb_fpclassifyf(x) == FP_NAN)

static inline f32 fb_sqrtf(f32 x) {
    const f64 _half = 0.5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return FB_NAN;
    } else if (fb_isnan(x)) {
        return FB_NAN;
    }
    return x;
}

// 100%: needs `half = 0.5f` locals (forces variable-first multiplies), the
// single shared return-0 (wrap the body in `if (hit) { ... }`), and dx/dy/dz
// assigned in the order dx, dy, dz for the FPR allocation to match.
s32 _firebar_hitCheck(FirebarEntry* entry, FirebarSegment* seg) {
    MarioWork* mario = marioGetPtr();
    f32 dx;
    f32 dz;
    f32 dy;
    f32 dist2;
    s32 hit = 0;

    if (entry->flags & 6) {
        return 0;
    }
    dx = mario->position.x - seg->position.x;
    dy = mario->position.y - seg->position.y;
    dz = mario->position.z - seg->position.z;
    if (dy > 0.0f) {
        if (dy < 15.0f * entry->scale * 0.8f) {
            hit = 1;
        }
    } else {
        if (__fabsf(dy) < mario->wPlayerCollisionBox.y) {
            hit = 1;
        }
    }
    if (hit) {
        dist2 = dx * dx + dz * dz;
        {
            f32 dist = fb_sqrtf(dist2);
            f32 half = 0.5f;
            f32 a = entry->angleSpeed * half;
            f32 b = mario->wPlayerCollisionBox.x * half;
            f32 limit = b * half + a * entry->scale;

            if (dist <= limit && vivianGetStatus() != 1 && vivianGetStatus() != 2) {
                return 1;
            }
        }
    }
    return 0;
}

// 100%: a local `half = 0.5f` keeps the final multiplies variable-first.
s32 _firebar_hitCheck2(FirebarEntry* entry) {
    FirebarSegment* seg = entry->segments;
    MarioWork* mario = marioGetPtr();
    Vec toMario;
    Vec dir;
    Vec cross;
    Vec up = vec3_las_00011ed4;
    s32 hit;
    f32 dy;

    if (entry->flags & 6) {
        return 0;
    }
    dy = mario->position.y - entry->position.y;
    hit = 0;
    if (dy > 0.0f) {
        if (dy < 15.0f * entry->scale * 0.8f) {
            hit = 1;
        }
    } else {
        if (__fabsf(dy) < mario->wPlayerCollisionBox.y) {
            hit = 1;
        }
    }
    if (!hit) {
        return 0;
    }

    PSVECSubtract(&seg[entry->count - 1].position, &seg->position, &dir);
    PSVECCrossProduct(&dir, &up, &cross);
    {
        f32 mag = PSVECMag(&dir);
        f32 limit = 5.0f * entry->scale;
        f32 dot1;
        f32 dot2;
        f32 box;
        f32 half = 0.5f;

        PSVECNormalize(&dir, &dir);
        PSVECNormalize(&cross, &cross);
        PSVECSubtract(&mario->position, &seg->position, &toMario);
        dot1 = PSVECDotProduct(&dir, &toMario);
        dot2 = PSVECDotProduct(&cross, &toMario);
        box = 10.0f + mario->wPlayerCollisionBox.x;
        if (dot1 > -box * half && dot1 <= box * half + mag &&
            __fabsf(dot2) <= mario->wPlayerCollisionBox.x * half + limit &&
            vivianGetStatus() != 1 && vivianGetStatus() != 2) {
            return 1;
        }
        return 0;
    }
}

// 100%: `half = 0.5f` plus explicit `a`/`b`/`c`/`e` temporaries give the
// original variable-first multiplies and the f0+d2 add order.
void _firebar_rot(FirebarEntry* entry) {
    FirebarSegment* seg = entry->segments;
    MarioWork* mario = marioGetPtr();
    f32 f29;
    f32 f30;
    f32 f31;
    f32 half;
    s32 i;

    sin(6.2831855f * entry->pulsePhase / 360.0f);
    for (i = 0; i < entry->count; i++, seg++) {
        animPoseMain(seg->poseId);
        if (!(entry->flags & 1)) {
            f29 = entry->scale * ((f32)i * entry->angleSpeed) + entry->baseRadius;
            f30 = (f32)sin(entry->angle);
            {
                f32 c = (f32)cos(entry->angle);
                f32 dx = f29 * f30;
                f32 dz = -f29 * c;

                seg->position.x = entry->position.x + dx;
                seg->position.y = entry->position.y;
                seg->position.z = entry->position.z + dz;
            }
        }
    }

    entry->angle += entry->speed * (1.0f + (f32)entry->counter / 10.0f);
    {
        s32 hit = _firebar_hitCheck2(entry);
        f32 d2;
        f32 a;
        f32 b;
        f30 = distABf(entry->position.x, entry->position.z, mario->position.x, mario->position.z);
        d2 = distABf(entry->position.x, entry->position.z, entry->segments[entry->count - 1].position.x,
                     entry->segments[entry->count - 1].position.z);
        half = 0.5f;
        {
            f32 a = entry->angleSpeed * half;
            f32 b = mario->wPlayerCollisionBox.x * half;
            f32 c = a * entry->scale;
            f32 e = b * half + c * half;
            f29 = e + d2;
        }
        a = angleABf(entry->position.x, entry->position.z, mario->position.x, mario->position.z);
        b = angleABf(entry->position.x, entry->position.z, entry->segments[entry->count - 1].position.x,
                     entry->segments[entry->count - 1].position.z);
        f31 = compAngle(a, b);
        if (hit > 0) {
            entry->counter = 0;
            if (entry->callback != NULL) {
                entry->callback(entry, 2);
            }
        } else if (f30 > entry->baseRadius && f30 < f29 && !(entry->flags & 6) && hit == 0 &&
                   vivianGetStatus() != 1 && vivianGetStatus() != 2) {
            if (entry->speed > 0.0f) {
                if (entry->prevAngleDiff < 0.0f && f31 > 0.0f && entry->position.y < mario->position.y) {
                    entry->counter++;
                    if (entry->callback != NULL) {
                        entry->callback(entry, 1);
                    }
                }
            } else if (entry->speed < 0.0f) {
                if (entry->prevAngleDiff > 0.0f && f31 < 0.0f && entry->position.y < mario->position.y) {
                    entry->counter++;
                    if (entry->callback != NULL) {
                        entry->callback(entry, 1);
                    }
                }
            }
        }
        entry->prevAngleDiff = f31;
    }
}

// 100%: declare/assign dz before dy so the conversion and multiplies land in
// the original FPRs.
void _firebar_trans(FirebarEntry* entry) {
    FirebarSegment* seg = entry->segments;
    MarioWork* mario = marioGetPtr();
    f32 f31;
    s32 i;

    sin(6.2831855f * entry->pulsePhase / 360.0f);
    switch (entry->state) {
    case 0:
        entry->scale += 0.05f;
        if (entry->scale > 1.0f) {
            entry->scale = 1.0f;
            entry->state++;
        }
        break;
    case 1:
        if (__fabsf(entry->position.x - entry->targetX) < 20.0f * __fabsf(entry->speed)) {
            entry->state++;
        }
        break;
    case 2:
        entry->scale -= 0.05f;
        if (entry->scale < 0.0f) {
            entry->position.x = entry->desc->position.x;
            entry->state = 0;
        }
        break;
    }

    if (entry->state == 1) {
        f31 = entry->scale;
    } else {
        f31 = 1.0f;
    }
    entry->position.x += entry->speed;
    for (i = 0; i < entry->count; i++, seg++) {
        animPoseMain(seg->poseId);
        if (!(entry->flags & 1)) {
            f32 dz = f31 * ((f32)i * entry->angleSpeed);
            f32 dy = 0.5f * (20.0f * (1.0f - entry->scale));

            seg->position.x = entry->position.x;
            seg->position.y = entry->position.y + dy;
            seg->position.z = entry->position.z + dz;
        }
    }

    if (entry->state == 1) {
        s32 hit = _firebar_hitCheck2(entry);
        f32 a = angleABf(entry->position.x, entry->position.z, mario->position.x, mario->position.z);
        f32 b = angleABf(entry->position.x, entry->position.z, entry->segments[entry->count - 1].position.x,
                         entry->segments[entry->count - 1].position.z);

        f31 = compAngle(a, b);
        if (hit > 0) {
            entry->counter = 0;
            if (entry->callback != NULL) {
                entry->callback(entry, 2);
            }
        } else if (!(entry->flags & 6) && hit == 0 && vivianGetStatus() != 1 && vivianGetStatus() != 2 &&
                   entry->prevAngleDiff < 0.0f && f31 > 0.0f && entry->position.y < mario->position.y) {
            entry->counter++;
            if (entry->callback != NULL) {
                entry->callback(entry, 1);
            }
        }
        entry->prevAngleDiff = f31;
    }
}

// The original spills `entry` to 0x8(r1) and reloads it at region boundaries. Taking
// `entry`'s address through the MUST_MATCH self-assignment below forces MWCC to keep it in
// that slot ((void)&entry; is optimized away, and an `if (&entry == NULL)` test adds three
// instructions the original does not have). Two further details are needed to match:
// `rand()` must be called inline (a named `s32 r` temporary changes the effect loop's fadds
// operand order and scratch registers), and the release loop must reuse the first loop's
// `i`/`seg` variables (`s32 k; FirebarSegment* sp;` makes the counter allocate to r25
// instead of the original's r27).
USER_FUNC(firebar_entry) {
    s32* args = event->args;
    FirebarDesc* desc = (FirebarDesc*)evtGetValue(event, args[0]);
    FirebarEntry* entry;
    FirebarSegment* seg;
    s32 i;

    if (isFirstCall) {
        entry = _mapAlloc(sizeof(FirebarEntry));
        memset(entry, 0, sizeof(FirebarEntry));

        entry->type = desc->type;
        entry->position = desc->position;
        entry->count = desc->count;
        entry->callback = desc->callback;
        entry->field_0x20 = desc->field_0x14;
        entry->angleSpeed = desc->angleSpeed;
        entry->speed = desc->speed;
        entry->scale = 1.0f;
        entry->prevAngleDiff = 0.0f;
        entry->angle = 0.0f;
        entry->counter = 0;
        entry->desc = desc;
        entry->targetX = desc->field_0x20;
        entry->baseRadius = desc->baseRadius;
        entry->sfxId = 0;
        entry->state = 0;
        entry->pulsePhase = 0.0f;

        switch (entry->type) {
        case 0:
            entry->angle = desc->field_0x20;
            break;
        case 1:
            if (desc->position.x < desc->field_0x20) {
                entry->speed = __fabsf(desc->speed);
            } else {
                entry->speed = -1.0f * __fabsf(desc->speed);
            }
            entry->scale = 0.0f;
            break;
        }

#ifdef MUST_MATCH
        // Self-assignment through a taken address forces MWCC to keep `entry` in its
        // stack slot (0x8(r1)) like the original, without emitting a branch.
        entry = *(FirebarEntry**)&entry;
#endif
        entry->segments = _mapAlloc(entry->count << 4);
        memset(entry->segments, 0, entry->count << 4);
        {
            seg = entry->segments;
            for (i = 0; i < entry->count; i++, seg++) {
                seg->poseId = animPoseEntry("c_fireber", 0);
                animPosePeraOn(seg->poseId);
                // The original reloads the zero vector's words from .rodata on
                // every iteration instead of hoisting the const copy out of the loop.
#ifdef MUST_MATCH
                {
                    const volatile u32* src = (const volatile u32*)&vec3_las_00011ee0;
                    volatile u32* dst = (volatile u32*)&seg->position;
                    u32 w1;
                    u32 w0;
                    w0 = src[0];
                    w1 = src[1];
                    dst[0] = w0;
                    dst[1] = w1;
                    dst[2] = src[2];
                }
#else
                seg->position = vec3_las_00011ee0;
#endif
                animPoseSetAnim(seg->poseId, "FRB_S_1", 1);
            }
        }
        event->userdata[0] = (s32)entry;
    }
    entry = (FirebarEntry*)event->userdata[0];

    if (entry->flags & 8) {
        if (psndSFXChk(entry->sfxId) == 0) {
            psndSFXOff(entry->sfxId);
            entry->sfxId = -1;
        }
        {
            FirebarSegment* bp = entry->segments;
            s32 m;
            for (m = 0; m < 3; m++) {
                s32 index = (entry->count + 1) * m / 3;
                f32 x;
                f32 y;

                x = bp[index].position.x + (f32)(rand() % 16 - 8);
                y = bp[index].position.y + 10.0f;
                seg = &bp[index];

                effKemuTestEntry(0, x, y, seg->position.z + (f32)(rand() % 5 + 3), 1.0f);
            }
        }
        {
            seg = entry->segments;
            for (i = 0; i < entry->count; i++, seg++) {
                animPoseRelease(seg->poseId);
            }
        }
        _mapFree(entry->segments);
        _mapFree(entry);
        return EVT_RETURN_DONE;
    }

    {
        MarioWork* mario = marioGetPtr();

        if (mario->currMotionId == 0x20 || mario->currMotionId == 0x1F || mario->unk4C > 0) {
            entry->flags |= 4;
        } else {
            entry->flags &= ~4;
        }
        if (__fabsf(mario->position.x - entry->position.x) < 120.0f) {
            if (psndSFXChk(entry->sfxId) != 0) {
                entry->sfxId = psndSFXOn_3D((const char*)0x1CD, &entry->position);
            }
        } else {
            if (psndSFXChk(entry->sfxId) == 0) {
                psndSFXOff(entry->sfxId);
                entry->sfxId = -1;
            }
        }
    }

    entry->pulsePhase += 6.0f;
    if (entry->pulsePhase > 360.0f) {
        entry->pulsePhase -= 360.0f;
    }
    switch (entry->type) {
    case 0:
        _firebar_rot(entry);
        break;
    case 1:
        _firebar_trans(entry);
        break;
    }
    if (entry->callback != NULL) {
        entry->callback(entry, 0);
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(firebar_disp_evt) {
    if (evtCheckID(event->lwData[0])) {
        FirebarEntry* entry = (FirebarEntry*)evtGetPtrID(event->lwData[0])->userdata[0];
        f32 b;
        f32 a;
        f32 t;
        FirebarSegment* seg;
        s32 i;

        f32 s;

        s = (f32)sin(6.2831855f * entry->pulsePhase / 360.0f);
        t = 0.625f + 0.375f * s;
        b = 0.4f * t;
        a = 0.2f * t;
        seg = entry->segments;
        for (i = 0; i < entry->count; i++, seg++) {
            shadowSetVolumeDecay(shadowEntryMode(2, seg->position.x, seg->position.y, seg->position.z, 17.0f * entry->scale),
                                 t, a, b);
        }
        dispEntry(4, dispCalcZ(&entry->position), 1, firebar_disp, entry);
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

void firebar_disp(CameraId camera, void* param) {
    FirebarEntry* entry = param;
    FirebarSegment* seg = entry->segments;
    s32 i;

    for (i = 0; i < entry->count; i++, seg++) {
        Mtx mtx;

        PSMTXTrans(mtx, seg->position.x, seg->position.y, seg->position.z);
        animPoseDrawMtx(seg->poseId, mtx, 1, 0.0f, 2.0f * entry->scale);
        animPoseDrawMtx(seg->poseId, mtx, 2, 0.0f, 2.0f * entry->scale);
        animPoseDrawMtx(seg->poseId, mtx, 3, 0.0f, 2.0f * entry->scale);
    }
}

USER_FUNC(coin_entry) {
    s32* args = event->args;
    s32 v0 = evtGetValue(event, args[0]);
    s32 v1 = evtGetValue(event, args[1]);
    s32 v2 = evtGetValue(event, args[2]);
    s32 v3 = evtGetValue(event, args[3]);
    ItemEntry* item = itemEntry(NULL, ITEM_COIN, 3, -1, 0, (f32)v0, (f32)v1, (f32)v2);

    itemStatusOn(item, 0x10);
    item->boundRate = 0.8f;
    item->gravity = 1.0f;
    item->boundLimit = 5;
    item->jumpPower = 550.0f;
    item->moveSpeed = 70.0f;
    item->moveDir = v3;
    return EVT_RETURN_DONE;
}

void firebar_cb(FirebarEntry* entry, s32 kind) {
    MarioWork* mario = marioGetPtr();

    switch (kind) {
    case 1:
        if (entry->counter >= 10 && entry->type == 0) {
            EventEntry* evt;
            entry->flags |= 8;
            evt = evtEntry(firebar_coin, 0, 0);
            evt->lwData[0] = (s32)entry->position.x;
            evt->lwData[1] = (s32)entry->position.y;
            evt->lwData[2] = (s32)entry->position.z;
        }
        break;
    case 2:
        set_damage_root_ypos(mario->position.y);
        marioChgMot(0x20);
        break;
    }
}
// clang-format on
