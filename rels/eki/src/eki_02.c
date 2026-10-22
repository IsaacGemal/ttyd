#include "eki_02.h"
#include "driver/hitdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "math_ext.h"
#include "party/party_motion.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(s_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    { "s_bero", 1, 3, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "eki_01", "n_bero", 5, 5, &s_close,
      &s_open },
    { NULL },
};

extern const f32 zero_eki_00008ff0;
extern const f32 float_6p2832_eki_00008ff4;
extern const f32 float_360_eki_00008ff8;

static EVT_BEGIN(evt_key)
    SET_VALUE(GSW(0), 314)
    RETURN
EVT_END()

USER_FUNC(hit_move) {
    MarioWork* mario;
    HitEntry* hit;
    Vec vec;
    Vec pos;
    Mtx mtx;
    f32 dist;
    f32 angle;
    f32 diff;
    f32 tmp;

    mario = marioGetPtr();
    hit = mario->hitobjStandOn;
    vec = (Vec){ 0.0f, 0.0f, -1.0f };

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
    }

    if (hit == NULL) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        return EVT_RETURN_BLOCK;
    }

    if ((hit->flags & 0x30) == 0) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        return EVT_RETURN_BLOCK;
    }

    mtxGetRotationElement(hit->worldMtx, mtx, 'x', 'z');
    PSMTXMultVec(mtx, &vec, &vec);
    angle = angleABf(zero_eki_00008ff0, zero_eki_00008ff0, vec.x, vec.z);
    diff = angle - event->userdataf[1];

    if (event->userdata[0] == 0) {
        event->userdata[0] = 1;
        event->userdataf[1] = angle;
        return EVT_RETURN_BLOCK;
    }

    hitObjGetPos(hit->joint->name, &pos);
    dist = distABf(mario->position.x, mario->position.z, pos.x, pos.z);
    // The temporary makes the compiler add diff into the angleABf result register, as the original does.
    tmp = angleABf(pos.x, pos.z, mario->position.x, mario->position.z);
    diff = float_6p2832_eki_00008ff4 * (tmp + diff) / float_360_eki_00008ff8;

    mario->riderDelta.x = (dist * (f32)sin(diff) + pos.x) - mario->position.x;
    mario->riderDelta.z = (pos.z - dist * (f32)cos(diff)) - mario->position.z;
    event->userdataf[1] = angle;
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(eki_02_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI3"))
    CALL(evt_map_playanim, STRING("anm_sikake_1"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_sikake_2"), 1, 0)
    IF_INT_LESS(GSW(0), 314)
        CALL(evt_item_entry, STRING("key"), 25, 525, 220, 29, 16, -1, PTR(evt_key))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(hit_move)
    END_INLINE_EVENT
    CALL(evt_item_entry, STRING("item"), 125, 540, 140, -40, 16, GSWF(5653), 0)
    RETURN
EVT_END()
// clang-format on

const f32 zero_eki_00008ff0 = 0.0f;
const f32 float_6p2832_eki_00008ff4 = 6.2831855f;
const f32 float_360_eki_00008ff8 = 360.0f;

#pragma force_active off
