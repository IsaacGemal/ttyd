#include "las_05.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "matching.h"
#include "math_ext.h"
#include "mario/mario_party.h"
#include "motion/mot_hammer.h"
#include "party.h"
#include "party/party_cloud.h"
#include "party/party_nokonoko.h"
#include "party/party_vivian.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

// Position table entry shared by the senkaron "ext" callbacks.
typedef struct KaronPoseEntry {
    s8 type;       // 0x0
    u8 field_1;    // 0x1
    u16 area;      // 0x2
    s32 field_4;   // 0x4
    f32 field_8;   // 0x8
    f32 field_C;   // 0xC
    f32 field_10;  // 0x10
    u32 flags;     // 0x14
} KaronPoseEntry;  // 0x18

// Everything the two karon AI scripts keep between calls.
typedef struct LasWork {
    s32 attackStart;         // 0x0
    s32 escape;              // 0x4
    s32 mesPushing;          // 0x8
    u32 field_C;             // 0xC
    s32 marioPushed;         // 0x10
    s32 field_14;            // 0x14
    KaronPoseEntry* entries; // 0x18
} LasWork;

#pragma force_active on
static USER_FUNC(make_name);
static USER_FUNC(set_party_y_pos);
static USER_FUNC(senkaronGetMarioPushedFlag);
static USER_FUNC(senkaronAttackStart);
static USER_FUNC(senkaronFlagOffMesPushing);
static USER_FUNC(senkaronFlagOnMesPushing);
static USER_FUNC(senkaronToEscape);
static void senkaron_ext_shadow_disp(CameraId cameraId, void* param);
static void senkaron_ext_disp(CameraId cameraId, void* param);
static void senkaron_ext_dispent(void);
static void senkaron_ext_main(void);
static void senkaron_ext_init(void);
static f32 randf(f32 min, f32 max);
static s32 key_evt[5];
extern LasWork* skwp;
extern f32 kurokaron_move_list[12];
extern u8 kurokaron_move_index;
extern u8 kurokaron_move_flag;
extern u8 kurokaron_same_goal;
extern u8 kurokaron_prev_goal[4];
extern u32 kurokaron_vivian_prev_sts;
extern u8 kurokaron_stop_cnt;
extern u16 kurokaron_run_cnt;


#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

extern s32 __float_nan[];
#define NAN (*(f32*)__float_nan)

// senkaron_ext_main's two sound names. MUST_MATCH: retail builds both addresses once, before the main loop,
// straight into callee-saved registers. MWCC's frontend hoists an integer round trip out of the loop as an
// invariant expression; a plain literal is rematerialised at each call instead.
#ifdef MUST_MATCH
#define KARON_SFX(name) ((const char*)(u32)(name))
#else
#define KARON_SFX(name) (name)
#endif

static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// clang-format off

LasWork* skwp;
// file-scope so MWCC lays these out in the original .bss order (flag, count, name)
static char name[20];
static s32 count;
static s32 flag;

extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
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
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_04", "e_bero", 5, 5, w_door_close, w_door_open},
    {"e_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_06", "w_bero", 5, 5, e_door_close, e_door_open},
    {NULL},
};

u8 kurokaron_prev_goal[4] = {
    0x05, 0x00, 0x00, 0x00,
};

// 6 (x, z) destinations the karon walks between.

f32 kurokaron_move_list[12] = {
    500.0f, -130.0f, 0.0f, -200.0f,
    -500.0f, -130.0f, -500.0f, 130.0f,
    0.0f, 200.0f, 500.0f, 130.0f,
};

EVT_BEGIN(kurokaron_move)
    SET_VALUE(LW(3), 0)
    SET_FLOAT_VALUE(GW(0), FLOAT(70.0f))
    SET_VALUE(GW(1), 200)
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(13), 0)
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(15))
        LOOP(0)
            CALL(evt_sub_get_stopwatch, LW(15), LW(14))
            IF_INT_GREATER_OR_EQUAL(LW(14), 10000)
                ADD(LW(13), 1)
                IF_INT_EQUAL(LW(13), 6)
                    BREAK_LOOP
                END_IF
                SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(15))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_FLOAT_VALUE(GW(0), FLOAT(50.0f))
        SET_VALUE(GW(1), 300)
        DEBUG_PUT_MSG(STRING("kurokaron 1 minute late"))
    END_SIBLING_EVENT
    SET_VALUE(LF(0), 0)
    LOOP(0)
        CALL(kurokaron_get_move_pos, LW(0), LW(1))
        CALL(kurokaron_move_position, STRING("黒カロン"), LW(0), LW(1), GW(0))
        CALL(kurokaron_flag_check, 2, LW(2))
        IF_INT_NOT_EQUAL(LW(2), 0)
            IF_INT_EQUAL(LW(3), 0)
                IF_INT_EQUAL(LF(0), 0)
                    WAIT_MS(GW(1))
                    SET_VALUE(LF(0), 1)
                ELSE
                    CALL(evt_sub_random, 300, LW(5))
                    ADD(LW(5), GW(1))
                    WAIT_MS(LW(5))
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(3), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 senkaron_ext_entry_data[78] = {
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x00000000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x3F800000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x40000000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x40400000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x40800000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x40A00000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x40C00000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x40E00000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41000000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41100000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41200000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41300000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41400000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41500000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41600000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41700000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41800000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41880000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41900000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41980000,
    (u32)"c_karon_h", (u32)"KRN_R_1", 0x41A00000, (u32)"c_karon_h",
    (u32)"KRN_R_1", 0x41A80000, (u32)"c_karon_h", (u32)"KRN_R_1",
    0x41B00000, (u32)"c_karon_h", (u32)"KRN_R_1", 0x41B80000,
    (u32)"c_karon_h", (u32)"KRN_D_1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 move_check_list[20] = {
    0x00000019, 0x00000034, 0x0000004F, 0x0000006B,
    0x00000088, 0x000000A6, 0x000000C5, 0x000000E5,
    0x00000107, 0x0000012A, 0x0000014F, 0x00000176,
    0x000001A0, 0x000001CD, 0x000001FE, 0x00000235,
    0x00000274, 0x000002BF, 0x00000326, 0x000003E8,
};
USER_FUNC(kurokaron_flag_check) {
    s32* args = event->args;
    s32 mask = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], kurokaron_move_flag & mask);
    return EVT_RETURN_DONE;
}

USER_FUNC(kurokaron_get_move_pos) {
    s32* args = event->args;

    evtSetFloat(event, args[0], kurokaron_move_list[kurokaron_move_index * 2]);
    evtSetFloat(event, args[1], kurokaron_move_list[kurokaron_move_index * 2 + 1]);
    return EVT_RETURN_DONE;
}

s32 _kurokaron_run_away(f32 x1, f32 z1, f32 x2, f32 z2) {
    s32 candidates[6];
    u8 index;
    s32 count;
    s32 other;
    f32 base = angleABf(x1, z1, x2, z2);
    f32 curAngle;
    f32 otherAngle;
    s32 result;
    s32 i;

    index = kurokaron_move_index;
    if (!(kurokaron_move_flag & 1)) {
        if (index == 0) {
            other = 5;
        } else {
            other = index - 1;
        }
    } else {
        other = (index + 1) % 6;
    }

    count = 0;
    for (i = 0; i < 6; i++) {
        f32 angle = __fabsf(compAngle(angleABf(x1, z1, kurokaron_move_list[i * 2], kurokaron_move_list[i * 2 + 1]), base));

        if (i == index) {
            curAngle = angle;
        }
        if (i == other) {
            otherAngle = angle;
        }
        if (angle > 90.0f) {
            candidates[count] = i;
            count++;
        }
    }

    if (count > 0) {
        result = candidates[rand() % count];
        if (curAngle < 90.0f) {
            if (!(kurokaron_move_flag & 1)) {
                kurokaron_move_flag |= 1;
            } else {
                kurokaron_move_flag &= ~1;
            }
        }
    } else if (curAngle < otherAngle) {
        if (!(kurokaron_move_flag & 1)) {
            if (kurokaron_move_index == 0) {
                result = 5;
            } else {
                result = kurokaron_move_index - 1;
            }
            kurokaron_move_flag |= 1;
        } else {
            kurokaron_move_flag &= ~1;
            result = (kurokaron_move_index + 1) % 6;
        }
    }
    return result;
}
// clang-format on
s32 kurokaron_move2(NpcEntry* npc, MarioWork* mario) {
    if (!(kurokaron_move_flag & 2)) {
        flag = 0;
        return 0;
    }
    kurokaron_move_flag &= ~4;

    if (npc->position.x < -560.0f || npc->position.x > 560.0f ||
        npc->position.z < -260.0f || npc->position.z > 260.0f) {
        f32 angle;

        if (rand() % 100 == 0) {
            if (kurokaron_move_flag & 8) {
                kurokaron_move_flag &= ~8;
            } else {
                kurokaron_move_flag |= 8;
            }
        }
        angle = angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z);

        if (kurokaron_move_flag & 8) {
            npc->moveDir = reviseAngle(angle - 90.0f);
        } else {
            npc->moveDir = reviseAngle(90.0f + angle);
        }
        kurokaron_move_flag |= 4;
        flag = 0;
        return 0;
    }

    if (distABf(npc->position.x, npc->position.z, mario->position.x, mario->position.z) < 100.0f) {
        if (count != 0) {
            count--;
        } else {
            if (npc->position.x < -490.0f || npc->position.x > 490.0f ||
                npc->position.z < -190.0f || npc->position.z > 190.0f) {
                f32 angle = angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z);

                if (kurokaron_move_flag & 8) {
                    npc->moveDir = angle - 90.0f;
                } else {
                    npc->moveDir = 90.0f + angle;
                }
                count = 10;
            } else {
                npc->moveDir = angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z);
                count = 20;
            }
            flag = 0;
        }
        return 0;
    }

    if (flag == 0) {
        kurokaron_move_index = _kurokaron_run_away(npc->position.x, npc->position.z, mario->position.x, mario->position.z);
        flag = 1;
        return 1;
    }
    return 0;
}

void _kurokaron_next_move_index(void) {
    if (kurokaron_move_flag & 1) {
        if (kurokaron_move_index == 0) {
            kurokaron_move_index = 5;
        } else {
            kurokaron_move_index--;
        }
    } else {
        kurokaron_move_index = kurokaron_move_index + 1;
        kurokaron_move_index = kurokaron_move_index % 6;
    }
}

USER_FUNC(kurokaron_move_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);
    Vec vec;
    MarioWork* mario;
    f32 distTarget;
    f32 distMario;
    f32 speed;
    f32 speed2;

    npc->jumpFlags |= 0x10;
    if (isFirstCall != 0) {
        npc->moveElapsed = 0;
        npc->gait = 0;
        npc->jumpTargetPos.x = x;
        npc->jumpTargetPos.y = npc->position.y;
        npc->jumpTargetPos.z = y;
        npc->jumpStartPos = npc->position;
        PSVECSubtract(&npc->jumpTargetPos, &npc->jumpStartPos, &vec);
#ifdef MUST_MATCH
        // The original evaluates a now-dead sqrtf here; MWCC keeps its argument check.
        distTarget = sqrtf(vec.x * vec.x + vec.z * vec.z);
#endif
        npc->moveDir = angleABf(0.0f, 0.0f, vec.x, vec.z);
        npc->moveSpeed = z;
        npc->moveOffset.y = 0.0f;
        if (npc->moveSpeed <= 0.1f && 0.0f == npc->moveTime) {
            npc->moveSpeed = 1.0f;
        }
        npc->footstepAccum = 0.0f;
        npc->footstepToggle = 1;
    }

    mario = marioGetPtr();
    PSVECSubtract(&npc->jumpTargetPos, &npc->position, &vec);
    distTarget = sqrtf(vec.x * vec.x + vec.z * vec.z);
    speed = 20.0f * npc->moveSpeed / 1000.0f;
    PSVECSubtract(&mario->position, &npc->position, &vec);
    distMario = sqrtf(vec.x * vec.x + vec.z * vec.z);

    if (distMario < 80.0f && (s32)kurokaron_vivian_prev_sts == 3 && vivianGetStatus() == 0) {
        kurokaron_stop_cnt = 60;
    }
    kurokaron_vivian_prev_sts = vivianGetStatus();
    if (kurokaron_stop_cnt != 0) {
        npc->jumpFlags &= ~0x10;
        kurokaron_stop_cnt--;
        return EVT_RETURN_BLOCK;
    }

    if (!(kurokaron_move_flag & 2) && vivianGetStatus() != 1 && vivianGetStatus() != 2 && vivianGetStatus() != 3 &&
        distMario < 80.0f) {
        kurokaron_move_index = _kurokaron_run_away(npc->position.x, npc->position.z, mario->position.x, mario->position.z);
        kurokaron_run_cnt = 350;
        kurokaron_move_flag |= 2;
        return EVT_RETURN_DONE;
    }

    if (kurokaron_move2(npc, mario) != 0) {
        return EVT_RETURN_DONE;
    }
    if (kurokaron_run_cnt != 0) {
        kurokaron_run_cnt--;
    }
    if (npc->wallHit != NULL) {
        f32 angle = angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z);

        if (kurokaron_move_flag & 4) {
            if (kurokaron_move_flag & 8) {
                kurokaron_move_flag &= ~8;
                npc->moveDir = reviseAngle(90.0f + angle);
            } else {
                kurokaron_move_flag |= 8;
                npc->moveDir = reviseAngle(angle - 90.0f);
            }
        }
        kurokaron_run_cnt = 350;
        kurokaron_move_flag |= 2;
    }

    if (distTarget <= speed && distMario > 100.0f) {
        npc->moveSpeed = 0.0f;
        kurokaron_move_flag &= ~2;
        if (kurokaron_prev_goal[0] == kurokaron_move_index) {
            kurokaron_same_goal++;
            if (kurokaron_same_goal > 3) {
                kurokaron_move_flag |= 2;
            }
        } else {
            kurokaron_same_goal = 0;
        }
        kurokaron_prev_goal[0] = kurokaron_move_index;
        if (kurokaron_move_flag & 1) {
            if (kurokaron_move_index == 0) {
                kurokaron_move_index = 5;
            } else {
                kurokaron_move_index--;
            }
        } else {
            kurokaron_move_index++;
            kurokaron_move_index %= 6;
        }
        return EVT_RETURN_DONE;
    }

    if ((npc->jumpFlags & 2) != 0 && npc->moveSpeed > 0.1f) {
        npcTuningRy(npc, npc->moveDir);
    }
    if ((kurokaron_move_flag & 2) != 0 && kurokaron_run_cnt != 0) {
        speed2 = 1.8f * npc->moveSpeed;
    } else {
        speed2 = npc->moveSpeed;
    }

    {
        f32 s = (f32)sin(6.2831855f * npc->moveDir / 360.0f);
        npc->moveOffset.x += npc->moveDt * (speed2 * s);
    }
    {
        f32 c = (f32)cos(6.2831855f * npc->moveDir / 360.0f);
        npc->moveOffset.z += npc->moveDt * (speed2 * -c);
    }

    if (npc->footstepAccum > (f32)npc->footstepStride / npc->moveSpeed) {
        if (npc->footstepToggle != 0) {
            if (npc->footstepSfxA != NULL) {
                psndSFXOn_3D(npc->footstepSfxA, &npc->position);
            }
        } else {
            if (npc->footstepSfxB != NULL) {
                psndSFXOn_3D(npc->footstepSfxB, &npc->position);
            }
        }
        npc->footstepAccum = 0.0f;
        // The original truncates the toggle to s8 before storing (MWCC emits extsb).
        npc->footstepToggle = (s8)(1 - npc->footstepToggle);
    }
    if (npc->footstepSfxB != NULL || npc->footstepToggle != 0) {
        npc->footstepAccum += 1000.0f * npc->moveDt;
    }
    return EVT_RETURN_BLOCK;
}

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    f32 d = max - min;
    return r * d + min;
}

static void senkaron_ext_init(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();
    KaronPoseEntry* entries;

    skwp = _mapAlloc(0x18);
    memset(skwp, 0, 0x18);
    skwp->entries = _mapAlloc(0x5DC0);
    entries = skwp->entries;
    i = extGetPoseNum() - 1;
    while (i >= 0) {
        pose->flags = 3;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = randf(-570.0f, 570.0f);
        (*pose->matrix)[1][3] = 250.0f;
        (*pose->matrix)[2][3] = randf(-270.0f, 270.0f);
        pose->heading = 90.0f;
        entries[i].type = 0;
        entries[i].field_4 = (s32)randf(1.0f, 200.0f);
        entries[i].field_C = -3.0f;
        i--;
        pose++;
    }
}

void move_check(ExtPose* pose, s32 index) {
    MarioWork* mario;
    KaronPoseEntry* entries;
    KaronPoseEntry* entry;
    KaronPoseEntry* other;
    ExtPose* nextPose;
    s32 count;
    s32 j;

    extGetPosePtr();
    mario = marioGetPtr();
    entries = skwp->entries;
    count = extGetPoseNum() - (index + 1);
    entry = &entries[index];

    if (entry->type != 1) {
        return;
    }
    nextPose = pose + 1;

    for (j = index + 1; count != 0; j++, nextPose++, count--) {
        other = &entries[j];

        if (other->type == 1 && other->area == entry->area) {
            f32 px = __fabsf((*pose->matrix)[0][3] - mario->position.x);
            f32 pz = __fabsf((*pose->matrix)[2][3] - mario->position.z);
            f32 nx = __fabsf((*nextPose->matrix)[0][3] - mario->position.x);
            f32 nz = __fabsf((*nextPose->matrix)[2][3] - mario->position.z);

            if (px * px + pz * pz > nx * nx + nz * nz) {
                f32 ax = __fabsf((*pose->matrix)[0][3] - (*nextPose->matrix)[0][3]);
                f32 az = __fabsf((*pose->matrix)[2][3] - (*nextPose->matrix)[2][3]);

                if (__fabsf(ax - az) < 20.0f) {
                    entry->flags |= 2;
                }
            } else {
                other->flags |= 2;
            }
        }
    }
}

void calc_area(s32 index, f32 x, f32 z) {
    KaronPoseEntry* entries = skwp->entries;
    f32 dx = x - -570.0f;
    f32 dz = z - -270.0f;
    u16 a;
    u16 b;
    dx /= 40.0f;
    dz /= 40.0f;
    a = dz;
    b = dx;
    entries[index].area = (u16)(s32)(29.5f * (f32)a + (f32)b);
}

s32 party_hit_check(f32 x, f32 z, f32* out) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 result = 0;
    s32 isNokonoko = 0;
    f32 dx, dz;

    if (party == NULL) {
        return 0;
    }

    dx = __fabsf(x - party->position.x);
    dz = __fabsf(z - party->position.z);

    switch (party->currentMemberId) {
    case PARTY_MEMBER_KOOPS:
        {
            s32 status = nokonokoGetStatus(party);

            switch (status) {
            case 2:
            case 3:
            case 4:
            case 5:
                isNokonoko = 1;
                break;
            }
        }
        if (isNokonoko && dx < party->radius && dz < party->radius) {
            result = 1;
            if (x < party->position.x) {
                *out = 1.0f;
            } else {
                *out = -1.0f;
            }
        }
        break;
    case PARTY_MEMBER_BOBBERY:
        if (partyGetPtr(PARTY_SLOT_PARTY)->useMotionId == 0x46 && dx < 80.0f && dz < 80.0f) {
            result = 1;
            if (x < party->position.x) {
                *out = 1.0f;
            } else {
                *out = -1.0f;
            }
        }
        break;
    case PARTY_MEMBER_FLURRIE:
        if ((party->flags & PARTY_FLAG_IS_BEING_USED) != 0 && dx < cloudGetBreathDist() && dz < 30.0f) {
            if (cloudGetBreathDir() < 180.0f) {
                if (x > party->position.x) {
                    result = 1;
                    *out = -1.0f;
                }
            } else {
                if (x < party->position.x) {
                    result = 1;
                    *out = 1.0f;
                }
            }
        }
        break;
    }
    return result;
}

static void senkaron_ext_main(void) {
    ExtPose* pose;
    MarioWork* mario;
    KaronPoseEntry* entries;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 half = 0.5f;
    s32 i;
    s32 j;
    s32 start;
    s32 dir;
    s32 end;
    s32 hit;
    KaronPoseEntry* p;
    ExtPose* ep;

    pose = extGetPosePtr();
    mario = marioGetPtr();
    entries = skwp->entries;
    f29 = mario->wPlayerCollisionBox.x * half;
    skwp->marioPushed = 0;
    dir = 0;
    if (mario->currMotionId == MARIO_MOTION_HAMMER) {
        skwp->field_14 = skwp->field_14 + 1;
        skwp->field_14 = skwp->field_14 % 0xF;
    } else {
        skwp->field_14 = 0;
    }

    if (mario->currMotionId == MARIO_MOTION_HAMMER) {
        if (skwp->field_14 == 0xA) {
            if (mario->dispReqDir == 0.0f) {
                dir = 1;
            } else if (mario->dispReqDir == 180.0f) {
                dir = 2;
            } else if (mario->unk1A8 < 90.0f || 270.0f < mario->unk1A8) {
                dir = 1;
            } else {
                dir = 2;
            }
        }
    }

    j = skwp->field_C % 20;
    if (j != 0) {
        start = move_check_list[j - 1] + 1;
    } else {
        start = 0;
    }
    end = move_check_list[j];

    pose += start;
    for (j = start; j < end; j++, pose++) {
        entries[j].flags &= ~2;
        calc_area(j, (*pose->matrix)[0][3], (*pose->matrix)[2][3]);
    }

    pose = extGetPosePtr() + start;
    for (j = start; j < end; j++, pose++) {
        move_check(pose, j);
    }

    skwp->field_C++;
    ep = extGetPosePtr();

    for (i = 0; i < 1000; i++, ep++) {
        p = &entries[i];

        switch (p->type) {
        case 0:
            ep->texId = (gp->retraceCount + i * i) % 0x18;
            if (p->field_4 > 0) {
                if (skwp->escape != 0) {
                    ep->flags &= ~EXT_POSE_VISIBLE;
                    p->type = 0x63;
                } else if (skwp->attackStart != 0) {
                    p->field_4--;
                }
            } else {
                (*ep->matrix)[1][3] += p->field_C;
                p->field_C -= 0.3f;
                if ((*ep->matrix)[1][3] < 0.0f) {
                    (*ep->matrix)[1][3] = 0.0f;
                    p->type = 1;
                    p->field_4 = 0;
                    p->flags &= 0xFFFE;
                    if (i % 8 == 0) {
                        Vec v = {0.0f, 0.0f, 0.0f};

                        v.x = (*ep->matrix)[0][3];
                        v.y = (*ep->matrix)[1][3];
                        v.z = (*ep->matrix)[2][3];
                        psndSFXOn_3D(KARON_SFX("SFX_STG8_KARON_LANDING1"), &v);
                    }
                }
            }
            break;
        case 1:
            ep->texId = (gp->retraceCount + i * i) % 0x18;
            if (skwp->escape != 0) {
                if ((*ep->matrix)[0][3] == 0.0f && (*ep->matrix)[2][3] == 0.0f) {
                    p->field_8 = 1.0f;
                    p->field_10 = 1.0f;
                } else {
                    f32 len = (f32)sqrt((*ep->matrix)[0][3] * (*ep->matrix)[0][3] +
                                        (*ep->matrix)[2][3] * (*ep->matrix)[2][3]);

                    p->field_8 = 4.0f * (*ep->matrix)[0][3] / len;
                    p->field_10 = 4.0f * (*ep->matrix)[2][3] / len;
                }
                if (p->field_8 < 0.0f) {
                    ep->heading = 90.0f;
                } else {
                    ep->heading = 270.0f;
                }
                p->type = 0x62;
                p->field_4 = 300;
                break;
            }
            hit = 0;
            f27 = (*ep->matrix)[0][3] - mario->position.x;
            f26 = __fabsf(f27);
            f25 = __fabsf((*ep->matrix)[2][3] - mario->position.z);
            f28 = 0.0f;

            if ((*ep->matrix)[0][3] >= mario->position.x) {
                if (p->flags & 1) {
                    ep->heading = 270.0f;
                } else {
                    ep->heading = 90.0f;
                }
            } else {
                if (p->flags & 1) {
                    ep->heading = 90.0f;
                } else {
                    ep->heading = 270.0f;
                }
            }

            if (skwp->mesPushing != 0) {
                break;
            }

            if (dir != 0 && f25 < 80.0f) {
                if (dir == 2) {
                    if ((*ep->matrix)[0][3] > mario->position.x && f26 < 1.5f * (6.0f * f29)) {
                        hit = 1;
                        f28 = 1.0f;
                    }
                } else {
                    if ((*ep->matrix)[0][3] < mario->position.x && f26 < 1.5f * (6.0f * f29)) {
                        hit = 1;
                        f28 = -1.0f;
                    }
                }
            }

            {
                f32 ka = marioGetAngleKaitenHammer();

                if (ka >= 0.0f && f26 < 80.0f && f25 < 80.0f) {
                    if (ka >= 0.0f && ka < 90.0f) {
                        if ((*ep->matrix)[2][3] > mario->position.z &&
                            (*ep->matrix)[0][3] > mario->position.x) {
                            hit = 1;
                            f28 = 1.0f;
                        }
                    } else if (ka >= 90.0f && ka < 180.0f) {
                        if ((*ep->matrix)[2][3] > mario->position.z &&
                            (*ep->matrix)[0][3] < mario->position.x) {
                            hit = 1;
                            f28 = -1.0f;
                        }
                    } else if (ka >= 180.0f && ka < 270.0f) {
                        if ((*ep->matrix)[2][3] < mario->position.z &&
                            (*ep->matrix)[0][3] < mario->position.x) {
                            hit = 1;
                            f28 = -1.0f;
                        }
                    } else {
                        if ((*ep->matrix)[2][3] < mario->position.z &&
                            (*ep->matrix)[0][3] > mario->position.x) {
                            hit = 1;
                            f28 = -1.0f;
                        }
                    }
                }
            }

            {
                f32 out = 0.0f;

                if (party_hit_check((*ep->matrix)[0][3], (*ep->matrix)[2][3], &out) != 0) {
                    hit = 2;
                    f28 = out;
                }
            }

            if (hit != 0) {
                f32 len = sqrtf(f26 * f26 + f25 * f25);

                p->field_8 = f28 + 3.0f * f27 / len;
                p->field_C = randf(5.0f, 10.0f);
                p->field_10 = randf(-5.0f, 10.0f);
                p->type = 0x61;
                if (hit == 1) {
                    Vec v = {0.0f, 0.0f, 0.0f};

                    v.x = (*ep->matrix)[0][3];
                    v.y = (*ep->matrix)[1][3];
                    v.z = (*ep->matrix)[2][3];
                    psndSFXOn_3D(KARON_SFX("SFX_STG8_KARON_FLY1"), &v);
                }
                break;
            }

            if (f25 < 40.0f && vivianGetStatus() != 1 && vivianGetStatus() != 2 && vivianGetStatus() != 3) {
                if ((*ep->matrix)[0][3] > 0.0f) {
                    if ((*ep->matrix)[0][3] > mario->position.x + f29 - 10.0f &&
                        (*ep->matrix)[0][3] < mario->position.x + 3.0f * f29) {
                        skwp->marioPushed |= 1;
                    }
                } else {
                    if ((*ep->matrix)[0][3] > mario->position.x - 3.0f * f29 &&
                        (*ep->matrix)[0][3] < mario->position.x - f29 + 10.0f) {
                        skwp->marioPushed |= 2;
                    }
                }
            }

            if (p->field_4 == 0) {
                f32 dx = (*ep->matrix)[0][3] - mario->position.x;
                f32 dz = (*ep->matrix)[2][3] - mario->position.z;
                f32 d = 10.0f * (f32)sqrt(dx * dx + dz * dz);

                hit = rand() % 50;
                p->field_8 = randf(-0.5f, 0.5f);
                p->field_10 = randf(-0.5f, 0.5f);
                p->field_8 -= dx / d;
                p->field_10 -= dz / d;
                if (hit == 1) {
                    p->flags |= 1;
                } else {
                    p->flags &= 0xFFFE;
                }
                p->field_4 = 60;
            }
            p->field_4--;
            if (p->flags & 2) {
                (*ep->matrix)[0][3] -= p->field_8;
                (*ep->matrix)[2][3] -= p->field_10;
            } else {
                (*ep->matrix)[0][3] += p->field_8;
                (*ep->matrix)[2][3] += p->field_10;
            }
            if ((*ep->matrix)[0][3] > 570.0f) {
                (*ep->matrix)[0][3] = 570.0f;
            }
            if ((*ep->matrix)[0][3] < -570.0f) {
                (*ep->matrix)[0][3] = -570.0f;
            }
            if ((*ep->matrix)[2][3] > 270.0f) {
                (*ep->matrix)[2][3] = 270.0f;
            }
            if ((*ep->matrix)[2][3] < -270.0f) {
                (*ep->matrix)[2][3] = -270.0f;
            }
            break;
        case 0x61:
            ep->flags |= EXT_POSE_NO_SHADOW;
            if ((*ep->matrix)[1][3] > -100.0f) {
                (*ep->matrix)[0][3] += p->field_8;
                (*ep->matrix)[1][3] += p->field_C;
                (*ep->matrix)[2][3] += p->field_10;
                if ((*ep->matrix)[2][3] <= -270.0f) {
                    (*ep->matrix)[2][3] = -270.0f;
                }
                p->field_C -= 0.3f;
                ep->texId = 0x18;
            } else {
                marioGetPtr();
                (*ep->matrix)[0][3] = randf(-570.0f, 570.0f);
                (*ep->matrix)[1][3] = 250.0f;
                (*ep->matrix)[2][3] = randf(-270.0f, 270.0f);
                ep->heading = randf(0.0f, 360.0f);
                ep->flags &= ~EXT_POSE_NO_SHADOW;
                p->type = 0;
                p->field_4 = rand() % 1000 + 1;
                p->field_C = 0.0f;
            }
            break;
        case 0x62:
            if (p->field_4 > 0) {
                (*ep->matrix)[0][3] += p->field_8;
                (*ep->matrix)[2][3] += p->field_10;
                p->field_4--;
                if (p->field_4 % 10 == 0) {
                    if (((p->field_4 / 5) & 1) == 0) {
                        Vec v = {0.0f, 0.0f, 0.0f};

                        v.x = (*ep->matrix)[0][3];
                        v.y = (*ep->matrix)[1][3];
                        v.z = (*ep->matrix)[2][3];
                        psndSFXOn_3D((const char*)0xAFB, &v);
                    } else {
                        Vec v = {0.0f, 0.0f, 0.0f};

                        v.x = (*ep->matrix)[0][3];
                        v.y = (*ep->matrix)[1][3];
                        v.z = (*ep->matrix)[2][3];
                        psndSFXOn_3D((const char*)0xAFC, &v);
                    }
                }
            } else {
                ep->flags &= ~EXT_POSE_VISIBLE;
                p->type = 0x63;
            }
            ep->texId = (gp->retraceCount + i * i) % 0x18;
            break;
        case 0x63:
            break;
        }
    }

    if (((skwp->marioPushed & 1) && mario->position.x > 0.0f) ||
        ((skwp->marioPushed & 2) && mario->position.x < 0.0f)) {
        f32 speed;
        f32 dx;
        f32 dz;

        dz = -mario->position.z;
        dx = -mario->position.x;
        {
            f32 len = sqrtf(dx * dx + dz * dz);

            switch (mario->currMotionId) {
            case MARIO_MOTION_HAMMER:
            case MARIO_MOTION_HAMMER_2:
                speed = 0.0f;
                break;
            case MARIO_MOTION_WALK:
                speed = randf(3.0f, 3.5f);
                break;
            case MARIO_MOTION_DASH:
                speed = randf(3.8f, 4.3f);
                break;
            case MARIO_MOTION_JUMP:
                speed = randf(3.3f, 3.8f);
                break;
            default:
                speed = randf(1.7f, 2.2f);
                break;
            }
            mario->position.x += speed * (dx / len);
            mario->position.z += speed * (dz / len);
        }
    }
}

static void senkaron_ext_dispent(void) {
    dispEntry(CAMERA_3D, 0.0f, 1, senkaron_ext_disp, NULL);
    dispEntry(CAMERA_3D, 0.0f, 2, senkaron_ext_shadow_disp, NULL);
}

static void senkaron_ext_disp(CameraId cameraId, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

static void senkaron_ext_shadow_disp(CameraId cameraId, void* param) {
    static Mtx mt = {
        {10.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 10.0f, 0.0f, 0.1f},
        {0.0f, 0.0f, 10.0f, 0.0f},
    };

    extLoadShadowMtx(mt);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

static EVT_BEGIN(senkaron_event)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_mario_key_onoff, 0)
        BEGIN_INLINE_EVENT
            LOOP(10)
                CALL(evt_npc_get_position, STRING("黒カロン"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KARON_APPEAR1"), LW(0), LW(1), LW(2), 0)
                WAIT_MS(200)
            END_LOOP
        END_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("黒カロン"), STRING("KRN_D_3"))
        WAIT_MS(300)
        CALL(evt_cam3d_evt_set, 550, 80, 310, 550, 30, -90, 300, 0)
        WAIT_MS(300)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg8_las_02"), 0, STRING("黒カロン"))
        CALL(evt_ext_entry, 1000, PTR(senkaron_ext_entry_data), PTR(senkaron_ext_init), PTR(senkaron_ext_main), PTR(senkaron_ext_dispent))
        CALL(senkaronFlagOffMesPushing)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_mario_adjust_dir)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING("fukidashi"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        WAIT_MS(1000)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_set_pose, STRING("M_N_5B"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_I_Y"))
        END_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("黒カロン"), STRING("KRN_D_4"))
        SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(11), FLOAT(10.0f))
        SET_FLOAT_VALUE(LW(12), FLOAT(-1.0f))
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_get_position, STRING("黒カロン"), LW(13), LW(10), LW(14))
            ADD_FLOAT(LW(10), LW(11))
            ADD_FLOAT(LW(11), LW(12))
            IF_INT_LESS_OR_EQUAL(LW(10), 0)
                SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
                CALL(evt_npc_set_position, STRING("黒カロン"), LW(13), LW(10), LW(14))
                BREAK_LOOP
            END_IF
            CALL(evt_npc_set_position, STRING("黒カロン"), LW(13), LW(10), LW(14))
        END_LOOP
        CALL(evt_npc_set_anim, STRING("黒カロン"), STRING("KRN_R_1"))
        CALL(evt_cam3d_evt_off, 300, 0)
        CALL(senkaronAttackStart)
        CALL(senkaronFlagOnMesPushing)
        BEGIN_INLINE_EVENT
            WAIT_MS(600)
            CALL(evt_snd_sfxon, STRING("SFX_STG8_KARON_DOWN1"), 0)
            CALL(evt_snd_sfxon, STRING("SFX_STG8_KARON_MOVE1"), GW(13))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 3500)
        END_INLINE_EVENT
        SET_VALUE(GF(0), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_npc_flag_onoff, 1, STRING("黒カロン"), 32)
            CALL(kurokaron_get_move_pos, LW(0), LW(1))
            CALL(evt_npc_move_position, STRING("黒カロン"), LW(0), LW(1), 0, FLOAT(80.0f), 0)
            WAIT_FRAMES(1)
            CALL(evt_npc_flag_onoff, 0, STRING("黒カロン"), 32)
            SET_VALUE(GF(0), 1)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_mario_set_dir, 270, 300, 0)
        END_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_print_party, STRING("stg8_las_03"))
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_print, 0, STRING("stg8_las_04"), 0, STRING("黒カロン"))
        SET_FLOAT_VALUE(LW(3), FLOAT(-1.5f))
        SET_FLOAT_VALUE(LW(4), FLOAT(0.001f))
        SET_VALUE(LW(5), 270)
        CALL(evt_mario_set_pose, STRING("M_I_O"))
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(0), 370)
                BREAK_LOOP
            END_IF
            ADD_FLOAT(LW(0), LW(3))
            ADD_FLOAT(LW(3), LW(4))
            IF_INT_GREATER_OR_EQUAL(LW(3), 0)
                SET_FLOAT_VALUE(LW(3), FLOAT(-0.1f))
            END_IF
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            ADD(LW(5), 10)
            IF_INT_GREATER_OR_EQUAL(LW(5), 360)
                SUBTRACT(LW(5), 360)
            END_IF
            CALL(evt_mario_set_dispdir, LW(5))
        END_LOOP
        CALL(senkaronFlagOffMesPushing)
        CALL(evt_npc_flag_onoff, 1, STRING("黒カロン"), 32)
        CALL(evt_npc_change_interrupt, STRING("黒カロン"), 1, PTR(kurokaron_move))
        CALL(evt_npc_restart_regular_event, STRING("黒カロン"))
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CALL(evt_mario_key_onoff, 1)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_off)
        SET_VALUE(GF(0), 0)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(7), 0)
            LOOP(0)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    BREAK_LOOP
                END_IF
                CALL(senkaronGetMarioPushedFlag, LW(6))
                IF_INT_NOT_EQUAL(LW(6), 0)
                    SET_VALUE(LW(7), 18)
                END_IF
                CALL(evt_mario_party_use_check, LW(6))
                IF_INT_NOT_EQUAL(LW(6), 0)
                    SET_VALUE(LW(7), 0)
                END_IF
                IF_INT_GREATER(LW(7), 0)
                    CALL(evt_mario_get_motion, LW(8))
                    IF_INT_EQUAL(LW(8), 0)
                        CALL(evt_mario_get_pose, LW(9))
                        IF_STR_NOT_EQUAL(LW(9), STRING("M_I_O"))
                            CALL(evt_mario_set_pose, STRING("M_I_O"))
                        END_IF
                        CALL(evt_mario_get_dispdir, LW(5))
                        ADD(LW(5), 10)
                        IF_INT_GREATER_OR_EQUAL(LW(5), 360)
                            SUBTRACT(LW(5), 360)
                        END_IF
                        CALL(evt_mario_set_dispdir, LW(5))
                        CALL(evt_party_set_dispdir, 0, LW(5))
                        SUBTRACT(LW(7), 1)
                        IF_INT_EQUAL(LW(7), 0)
                            CALL(evt_mario_set_pose, STRING("M_S_1"))
                        END_IF
                    ELSE
                        SET_VALUE(LW(7), 0)
                    END_IF
                ELSE
                    CALL(evt_mario_get_motion, LW(8))
                    IF_INT_EQUAL(LW(8), 0)
                        CALL(evt_mario_get_dispdir, LW(5))
                        SET_VALUE(LF(1), 0)
                        IF_INT_GREATER_OR_EQUAL(LW(5), 90)
                            IF_INT_LESS(LW(5), 270)
                                SET_VALUE(LF(1), 1)
                            END_IF
                        END_IF
                        IF_INT_EQUAL(LF(1), 0)
                            CALL(evt_mario_set_dispdir, 0)
                        ELSE
                            CALL(evt_mario_set_dispdir, 180)
                        END_IF
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            SET_VALUE(LF(0), 0)
            SET_VALUE(LW(15), 0)
            LOOP(0)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    CALL(evt_cam_letter_box_disable, 0)
                    BREAK_LOOP
                END_IF
                SET_VALUE(LW(5), 0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_sub_area_check, 0, 0, 0, 5, LW(0), LW(2), LW(3))
                ADD(LW(5), LW(3))
                CALL(evt_key_get_dir, 0, LW(0))
                CALL(evt_key_get_button, 0, LW(1))
                ADD(LW(0), LW(1))
                IF_INT_EQUAL(LW(0), 0)
                    ADD(LW(5), 1)
                END_IF
                SET_VALUE(LW(2), 0)
                CALL(evt_mario_party_use_check, LW(2))
                SWITCH(LW(2))
                    CASE_EQUAL(0)
                    ADD(LW(5), 1)
                    CASE_EQUAL(4)
                    ADD(LW(5), 1)
                END_SWITCH
                CALL(evt_mario_get_motion, LW(4))
                SWITCH(LW(4))
                    CASE_EQUAL(0)
                    ADD(LW(5), 1)
                    CASE_EQUAL(22)
                    ADD(LW(5), 1)
                    CASE_EQUAL(26)
                    ADD(LW(5), 1)
                END_SWITCH
                IF_INT_EQUAL(LW(5), 4)
                    IF_INT_EQUAL(LF(0), 0)
                        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(14))
                        SET_VALUE(LF(0), 1)
                    ELSE
                        CALL(evt_sub_get_stopwatch, LW(14), LW(0))
                        SWITCH(LW(15))
                            CASE_EQUAL(0)
                            IF_INT_GREATER_OR_EQUAL(LW(0), 1000)
                                CALL(evt_cam_letter_box_onoff, 1, 0)
                                CALL(evt_cam_letter_box_disable, 1)
                                CALL(evt_mario_cont_onoff, 0)
                                CALL(evt_party_cont_onoff, 0, 0)
                                CALL(evt_mario_cam_off)
                                ADD(LW(15), 1)
                            END_IF
                            CASE_EQUAL(1)
                            IF_INT_GREATER_OR_EQUAL(LW(0), 2000)
                                CALL(evt_msg_print_party, STRING("stg8_las_05"))
                                SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(14))
                                ADD(LW(15), 1)
                            END_IF
                            CASE_EQUAL(2)
                            IF_INT_GREATER_OR_EQUAL(LW(0), 3000)
                                CALL(evt_msg_print_party, STRING("stg8_las_06"))
                                CALL(evt_seq_set_seq, 5, 0, 0)
                            END_IF
                        END_SWITCH
                        IF_INT_GREATER(LW(15), 0)
                            CALL(get_mario_y_pos, LW(0))
                            CALL(get_party_y_pos, LW(1))
                            ADD_FLOAT(LW(0), FLOAT(-0.16602f))
                            ADD_FLOAT(LW(1), FLOAT(-0.16602f))
                            CALL(set_mario_y_pos, LW(0))
                            CALL(set_party_y_pos, LW(1))
                        END_IF
                    END_IF
                ELSE
                    CALL(mario_land_on)
                    CALL(party_land_on)
                    CALL(chk_mario_ctrloff, LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL(evt_mario_cont_onoff, 1)
                        CALL(evt_mario_cam_on)
                        CALL(evt_party_cont_onoff, 0, 1)
                        CALL(evt_cam_letter_box_disable, 0)
                    END_IF
                    SET_VALUE(LF(0), 0)
                    SET_VALUE(LW(15), 0)
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(party_hit, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                BREAK_LOOP
            END_IF
            SET_VALUE(LF(0), 0)
            CALL(evt_party_get_status, 6, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(0), 2)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LW(0), 3)
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LF(0), 0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_npc_get_position, STRING("黒カロン"), LW(3), LW(4), LW(5))
                CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(6))
                IF_INT_LESS_OR_EQUAL(LW(6), 30)
                    BREAK_LOOP
                END_IF
            END_IF
        END_LOOP
        SET_VALUE(GF(0), 1)
        WAIT_FRAMES(1)
        CALL(mario_land_on)
        CALL(party_land_on)
        CALL(evt_mario_adjust_dir)
        CALL(evt_mario_set_motion, 0)
        CALL(chk_mario_ctrloff, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_mario_cont_onoff, 1)
            CALL(evt_mario_cam_on)
            CALL(evt_party_cont_onoff, 0, 1)
        END_IF
        CALL(evt_npc_get_reglid, STRING("黒カロン"), LW(0))
        DELETE_EVENT(LW(0))
        CALL(evt_npc_change_interrupt, STRING("黒カロン"), 1, 0)
        CALL(evt_npc_set_stay_anim, STRING("黒カロン"))
        CALL(senkaronFlagOnMesPushing)
        CALL(evt_snd_sfxoff, GW(13))
        WAIT_FRAMES(1)
        CALL(evt_npc_set_battle_info, STRING("黒カロン"), 7)
        CALL(evt_npc_battle_start, STRING("黒カロン"))
        CALL(evt_npc_wait_battle_end)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
        CALL(senkaronToEscape)
        SET_VALUE(GSW(0), 383)
        SET_VALUE(LW(0), STRING("w_bero"))
        CALL_CHILD(bero_case_switch_on)
        CALL(evt_npc_get_position, STRING("黒カロン"), LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(evt_sub_get_dir, LW(3), LW(5), LW(0), LW(2), LW(6))
        CALL(evt_item_entry, STRING("key"), 46, LW(0), LW(1), LW(2), 4, -1, PTR(key_evt))
        CALL(evt_item_set_move_dir_speed, STRING("key"), LW(6), FLOAT(45.0f))
        CALL(evt_item_set_jump_power, STRING("key"), FLOAT(500.0f))
        CALL(evt_item_set_gravity, STRING("key"), FLOAT(1.0f))
        CALL(evt_item_set_bound_rate, STRING("key"), FLOAT(1.0f))
        CALL(evt_item_set_bound_limit, STRING("key"), 0)
        CALL(evt_item_status_on, STRING("key"), 4)
        CALL(evt_item_status_on, STRING("key"), 16)
        CALL(evt_npc_set_position, STRING("黒カロン"), 0, -1000, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kurokaron_init)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_npc_set_position, STRING("me"), 570, 0, 0)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
        CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_Z_2"))
        CALL(evt_npc_set_balloontype, STRING("me"), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kurokaron_talk)
    SPAWN_EVENT(senkaron_event)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[69] = {
    (u32)"黒カロン", 0x01000000, 0x00000000, (u32)&kurokaron_init,
    0x00000000, (u32)&kurokaron_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"カロン",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(key_evt)
    SET_VALUE(GSW(0), 384)
    RETURN
EVT_END()

// TODO: type
static u32 key_tbl[2] = {
    0x0000002E, 0xFFFFFFFF,
};

static EVT_BEGIN(key_check_evt)
    CALL(evt_pouch_check_item, 46, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(unlock_evt)
    SET_VALUE(GSW(0), 385)
    SET_VALUE(LW(0), STRING("e_bero_1"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()
static USER_FUNC(senkaronToEscape) {
    skwp->escape = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(senkaronFlagOnMesPushing) {
    skwp->mesPushing = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(senkaronFlagOffMesPushing) {
    skwp->mesPushing = 0;
    return EVT_RETURN_DONE;
}

static USER_FUNC(senkaronAttackStart) {
    skwp->attackStart = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(senkaronGetMarioPushedFlag) {
    evtSetValue(event, event->args[0], skwp->marioPushed);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_mario_y_pos) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, args[0], mario->position.y);
    return EVT_RETURN_DONE;
}

USER_FUNC(set_mario_y_pos) {
    f32 y = evtGetFloat(event, event->args[0]);

    marioGetPtr()->position.y = y;
    return EVT_RETURN_DONE;
}

USER_FUNC(get_party_y_pos) {
    s32* args = event->args;
    PartyEntry* party = partyGetPtr(partyCtrlNo);

    if (party != NULL) {
        evtSetFloat(event, args[0], party->position.y);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(set_party_y_pos) {
    f32 y = evtGetFloat(event, event->args[0]);
    PartyEntry* party = partyGetPtr(partyCtrlNo);

    if (party != NULL) {
        party->position.y = y;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(mario_land_on) {
    MarioWork* mario = marioGetPtr();

    if (mario->position.y < 0.0f) {
        mario->position.y = 0.0f;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(party_land_on) {
    PartyEntry* party = partyGetPtr(partyCtrlNo);

    if (party != NULL) {
        if (party->position.y < 0.0f) {
            party->position.y = 0.0f;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(chk_mario_ctrloff) {
    s32* args = event->args;

    evtSetValue(event, args[0], marioCtrlOffChk());
    return EVT_RETURN_DONE;
}

USER_FUNC(party_hit) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    NpcEntry* npc = npcNameToPtr("黒カロン");
    s32* args = event->args;
    f32 dir;
    f32 nx = npc->position.x;
    f32 nz = npc->position.z;

    if (party_hit_check(nx, nz, &dir) != 0 &&
        (party->currentMemberId == 2 || party->currentMemberId == 3)) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(shan_init)
    SET_VALUE(LW(0), 1)
    LOOP(3)
        SWITCH(LW(0))
            CASE_EQUAL(1)
            SET_VALUE(LW(2), 8)
            CASE_DEFAULT
            SET_VALUE(LW(2), 4)
        END_SWITCH
        SET_VALUE(LW(1), 1)
        LOOP(LW(2))
            CALL(make_name, LW(0), LW(1), LW(3))
            CALL(evt_mapobj_flag_onoff, 1, 1, LW(3), 1)
            CALL(evt_mapobj_get_position, LW(3), LW(4), LW(5), LW(6))
            SUBTRACT_FLOAT(LW(5), FLOAT(7.0f))
            CALL(evt_eff, 0, STRING("fire"), 3, LW(4), LW(5), LW(6), FLOAT(0.8f), 1000, 0, 0, 0, 0, 0, 0)
            ADD(LW(1), 1)
        END_LOOP
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()
static USER_FUNC(make_name) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 b = evtGetValue(event, args[1]);

    sprintf(name, "S_shan_%02d_rou_%02d", a, b);
    evtSetValue(event, args[2], (s32)name);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(las_05_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_EQUAL(GSW(0), 382)
        CALL(evt_npc_setup, PTR(npcEnt))
    END_IF
    IF_INT_EQUAL(GSW(0), 383)
        CALL(evt_item_entry, STRING("key"), 46, 0, 0, 0, 0, -1, PTR(key_evt))
    END_IF
    IF_INT_LESS(GSW(0), 385)
        CALL(evt_mobj_lock, STRING("lock"), 46, 590, 20, 0, -90, PTR(key_check_evt), PTR(unlock_evt), 0)
        SET_VALUE(LW(0), STRING("e_bero_1"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL_CHILD(shan_init)
    RETURN
EVT_END()

u16 kurokaron_run_cnt;
u8 kurokaron_stop_cnt;
u32 kurokaron_vivian_prev_sts;
u8 kurokaron_same_goal;
u8 kurokaron_move_flag;
u8 kurokaron_move_index;
