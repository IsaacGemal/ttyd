#include "evt/evt_npc.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "system.h"
#include <string.h>
#include "koopa.h"
#include "driver/mobjdrv.h"
#include "eff/n64/eff_magic1_n64.h"
#include "eff/eff_fire.h"
#include "math_ext.h"
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_mario.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_urouro.h"
#include "manager/evtmgr.h"
#include "npc_event.h"
#include "party/party_motion.h"

static USER_FUNC(chain_main);
s32 _2d_dead_rotate_event[16];
s32 wanwan_init_event_sub[64];
s32 enemy_slave_common_dead_event[8];
s32 enemy_common_flag_init_event[12];
s32 npc_urouro_init_event[12];

static void piders_draw_yarn_sub(CameraId cameraId, void* param);

// clang-format off
// Prototypes for the forward references the original function order needs.
static void piders_draw_yarn_sub(CameraId cameraId, void* param);

USER_FUNC(mahoon_get_groupname) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetValue(event, args[0], (s32)npc->tribe->modelName);
    return EVT_RETURN_DONE;
}

USER_FUNC(npc_check_wall_stop) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    if (npc->wallHit != NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_wait_blow_end) {
    s32 id = evtGetValue(event, *event->args);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)id);
    if (npcCheckBlow(npc)) {
        npc->rotation.y += 18.0f;
        return EVT_RETURN_BLOCK;
    }
    npc->rotation.y = 0.0f;
    return EVT_RETURN_DONE;
}

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type


USER_FUNC(pressCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;

    if (kpaGetLevel() == 0) {
        evtSetValue(event, args[0], 0);
        return EVT_RETURN_DONE;
    }

    distance = 5.0f;
    if (hitCheckFilter(0, npc->position.x, npc->position.y + 1.0f, npc->position.z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
                       &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

// 100%: `if (f29)` (not `f29 != 0.0f`) makes MWCC emit the target's `fcmpu cr0, f29, f2` operand order; the
// `else if (dy >= 0.0f) return YIELD;` tail shares the target's single `li r3, 0` return-0 block.
USER_FUNC(_2d_dead_jump) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 f30;
    f32 f25;
    f32 f26;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f31;
    f32 f24;
    NpcEntry* npc;
    Vec diff;
    f32 dy;

    f31 = evtGetFloat(event, args[1]);
    f24 = evtGetFloat(event, args[2]);
    f25 = evtGetFloat(event, args[3]);
    f30 = (f32)evtGetValue(event, args[4]) / 1000.0f;
    f26 = evtGetFloat(event, args[5]);
    f29 = evtGetFloat(event, args[6]);
    f28 = (f32)evtGetValue(event, args[7]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    f27 = f29;

    npc->jumpFlags |= 0x10;
    npc->jumpFlags |= 0x1000;

    if (isFirstCall) {
        npc->flags |= 0x20000;
        npc->flags &= 0xBFFFFFEF;
        npc->moveElapsed = 0;
        npc->airTicks = 0;
        npc->gait = 0;
        npc->jumpTargetPos.x = f31;
        npc->jumpTargetPos.y = f24;
        npc->jumpTargetPos.z = f25;
        npc->jumpStartPos = npc->position;
        PSVECSubtract(&npc->jumpTargetPos, &npc->jumpStartPos, &diff);
#ifdef MUST_MATCH
        // MUST_MATCH: the target keeps a dead `diff.x*diff.x + diff.z*diff.z` compare before the angleABf
        // call. An empty `if` is dropped entirely, but a self-store makes MWCC emit the compare and then
        // optimise the store away, leaving exactly the target's lone fcmpo.
        if (diff.x * diff.x + diff.z * diff.z < 0.0f) {
            npc->moveDir = npc->moveDir;
        }
#endif
        npc->moveDir = angleABf(0.0f, 0.0f, diff.x, diff.z);
        if (f30 > 0.0f) {
            npc->moveSpeed = (f32)(__fabs(f31 - npc->position.x) / f30);
        } else {
            npc->moveSpeed = f26;
        }
        npc->moveTime = 0.0f;
        npc->moveOffset.y = 0.0f;
        npc->jumpVelY = 0.0f;
        npc->jumpYScale = 1.0f;
        dy = npc->jumpTargetPos.y - npc->jumpStartPos.y;
        if (f29) {
            if (dy > 0.0f) {
                f27 = f27 + dy;
            }
            npc->jumpVelY = 0.0f;
            if (f30 == 0.0f) {
                npc->jumpVelY = f29;
            } else {
                f32 f4 = 0.5f * f30;
                f32 y;

                npc->jumpVelY = -(f30 * ((-980.0f * npc->jumpGravity) * f30)) / f30;
                y = npc->jumpVelY * f4 + f4 * ((-980.0f * npc->jumpGravity) * f4);
                if (y != 0.0f) {
                    npc->jumpYScale = f27 / y;
                }
            }
        } else if (dy >= 0.0f) {
            return EVT_RETURN_YIELD;
        }
        return EVT_RETURN_BLOCK;
    } else {
        f32 s;
        f32 c;

        s = (f32)sin(6.2831855f * npc->moveDir / 360.0f);
        npc->moveOffset.x = npc->moveDt * (npc->moveSpeed * s) + npc->moveOffset.x;
        c = (f32)cos(6.2831855f * npc->moveDir / 360.0f);
        npc->moveOffset.z = npc->moveDt * (npc->moveSpeed * -c) + npc->moveOffset.z;
        if (npc->position.y >= f28) {
            return EVT_RETURN_BLOCK;
        }
        npc->moveSpeed = 0.0f;
        npc->jumpVelY = 0.0f;
        npc->jumpYScale = 1.0f;
        npc->jumpGravity = 1.0f;
        return EVT_RETURN_YIELD;
    }
}

EVT_BEGIN(enemy_common_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_damage_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_ENEMY_DIE1"), 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_drop_coin, STRING("me"), LW(3))
    IF_INT_GREATER(LW(3), 0)
        LOOP(LW(3))
            CALL(evt_item_entry, 0, 121, LW(0), LW(1), LW(2), 10, -1, 0)
        END_LOOP
    END_IF
    CALL(evt_npc_get_drop_heart, STRING("me"), LW(3))
    IF_INT_GREATER(LW(3), 0)
        LOOP(LW(3))
            CALL(evt_item_entry, 0, 123, LW(0), LW(1), LW(2), 10, -1, 0)
        END_LOOP
    END_IF
    CALL(evt_npc_get_drop_flower, STRING("me"), LW(3))
    IF_INT_GREATER(LW(3), 0)
        LOOP(LW(3))
            CALL(evt_item_entry, 0, 124, LW(0), LW(1), LW(2), 10, -1, 0)
        END_LOOP
    END_IF
    CALL(evt_npc_get_drop_item, STRING("me"), LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_item_entry, 0, LW(3), LW(0), LW(1), LW(2), 9, -1, 0)
    END_IF
    CALL(evt_npc_getback_item_entry, STRING("me"))
    CALL(evt_npc_get_drop_fixitem, STRING("me"), LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_item_entry, 0, LW(3), LW(0), LW(1), LW(2), 9, -1, 0)
    END_IF
    CALL(evt_npc_delete, STRING("me"))
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(enemy_event_dead_event_sub)
    CALL(evt_npc_set_damage_anim, LW(10))
    WAIT_MS(500)
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_ENEMY_DIE1"), 0)
    CALL(evt_npc_delete, LW(10))
    RETURN
EVT_END()

EVT_BEGIN(enemy_common_blow_event)
    CALL(evt_npc_set_confuse_anim, STRING("me"))
    CALL(_wait_blow_end, STRING("me"))
    WAIT_MS(2000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(enemy_2d_common_dead_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 268435456)
    CALL(pressCheck, LW(15))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("me"), LW(0), LW(1), 200)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_ENEMY_DIE1"), 0)
    CALL(evt_npc_set_damage_anim, STRING("me"))
    CALL(evt_npc_get_kpencount_type, STRING("me"), LW(0))
    SWITCH(LW(0))
        CASE_FLAG(1)
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_calc_score, STRING("me"))
            SPAWN_EVENT(_2d_dead_rotate_event)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(_2d_get_dead_jump_offset, STRING("me"), LW(3))
            ADD(LW(0), LW(3))
            CALL(_2d_dead_jump, STRING("me"), LW(0), LW(1), LW(2), 600, FLOAT(40.0f), FLOAT(0.0f), -300)
        ELSE
            CALL(evt_npc_calc_score, STRING("me"))
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
            CALL(evt_npc_status_onoff, 0, STRING("me"), 2)
            CALL(evt_npc_set_talk_anim, STRING("me"))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 5)
            CALL(evt_npc_set_position, STRING("me"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(30)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            BEGIN_SIBLING_EVENT
                LOOP(30)
                    CALL(evt_npc_add_rotate, STRING("me"), 0, 0, 1)
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            SUBTRACT(LW(0), 40)
            SUBTRACT(LW(1), 20)
            CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, -10, 4, 0)
            LOOP(0)
                BEGIN_SIBLING_EVENT
                    LOOP(60)
                        CALL(evt_npc_add_rotate, STRING("me"), 0, 0, -1)
                        WAIT_FRAMES(1)
                    END_LOOP
                END_SIBLING_EVENT
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                ADD(LW(0), 80)
                SUBTRACT(LW(1), 20)
                CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 1000, 0, -10, 4, 0)
                BEGIN_SIBLING_EVENT
                    LOOP(60)
                        CALL(evt_npc_add_rotate, STRING("me"), 0, 0, 1)
                        WAIT_FRAMES(1)
                    END_LOOP
                END_SIBLING_EVENT
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                SUBTRACT(LW(0), 80)
                SUBTRACT(LW(1), 20)
                CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 1000, 0, -10, 4, 0)
                IF_INT_LESS_OR_EQUAL(LW(1), -200)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_IF
        CASE_FLAG(2)
        CALL(evt_npc_calc_score, STRING("me"))
        SPAWN_EVENT(_2d_dead_rotate_event)
        CALL(evt_npc_status_onoff, 1, STRING("me"), 4194304)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(_2d_get_dead_jump_offset, STRING("me"), LW(3))
        ADD(LW(0), LW(3))
        CALL(_2d_dead_jump, STRING("me"), LW(0), LW(1), LW(2), 600, FLOAT(40.0f), FLOAT(65.0f), -300)
        CASE_FLAG(4)
        CALL(evt_npc_calc_score, STRING("me"))
        SPAWN_EVENT(_2d_dead_rotate_event)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(_2d_get_dead_jump_offset, STRING("me"), LW(3))
        ADD(LW(0), LW(3))
        CALL(_2d_dead_jump, STRING("me"), LW(0), LW(1), LW(2), 600, FLOAT(40.0f), FLOAT(65.0f), -300)
        CASE_FLAG(32)
        SPAWN_EVENT(_2d_dead_rotate_event)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(_2d_get_dead_jump_offset, STRING("me"), LW(3))
        ADD(LW(0), LW(3))
        CALL(_2d_dead_jump, STRING("me"), LW(0), LW(1), LW(2), 600, FLOAT(40.0f), FLOAT(65.0f), -300)
        CASE_FLAG(8)
        CALL(evt_npc_calc_score, STRING("me"))
        SPAWN_EVENT(_2d_dead_rotate_event)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(_2d_get_dead_jump_offset, STRING("me"), LW(3))
        ADD(LW(0), LW(3))
        CALL(_2d_dead_jump, STRING("me"), LW(0), LW(1), LW(2), 600, FLOAT(40.0f), FLOAT(65.0f), -300)
        CASE_FLAG(16)
        CALL(evt_npc_calc_score, STRING("me"))
        SPAWN_EVENT(_2d_dead_rotate_event)
        CALL(evt_npc_status_onoff, 1, STRING("me"), 4194304)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(_2d_get_dead_jump_offset, STRING("me"), LW(3))
        ADD(LW(0), LW(3))
        CALL(_2d_dead_jump, STRING("me"), LW(0), LW(1), LW(2), 600, FLOAT(40.0f), FLOAT(65.0f), -300)
        CASE_DEFAULT
        WAIT_MS(500)
    END_SWITCH
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(_2d_dead_rotate_event)
    SET_VALUE(LW(0), -12)
    LOOP(15)
        CALL(evt_npc_add_rotate, STRING("me"), 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_8033d458)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    LABEL(0)
    WAIT_MS(500)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(npc_urouro_move_event)
    CALL_CHILD(npc_urouro_init_event)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(npc_urouro_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kuriboo_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(kuriboo_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    CALL(evt_npc_homing_target, STRING("me"), STRING("mario"), 0, FLOAT(120.0f), FLOAT(10.0f), FLOAT(60.0f), 4)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kuriboo_lost_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kuriboo_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patakuri_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(patakuri_find_event)
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 0, 75, FLOAT(-50.0f), 0, 3)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 1000, 0, 0, 0, 1)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patakuri_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patakuri_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 1000, 0, 0, 0, 1)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_shell_return_event)
    CALL(evt_npc_get_unitwork, STRING("me"), 1, LW(15))
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
        CALL(evt_npc_set_unitwork, STRING("me"), 1, 0)
        CALL(evt_npc_set_attack_mode, STRING("me"), 0)
        CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
        CALL(evt_npc_set_stay_anim, STRING("me"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1179672)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_move_event)
    CALL_CHILD(nokonoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_find_event)
    CALL_CHILD(nokonoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_NOKO_MOVE1"), 0, 0, 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 1, 1)
    CALL(evt_npc_set_attack_mode, STRING("me"), 11)
    CALL(evt_npc_set_anim, STRING("me"), STRING("NKT_A_1"))
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
    CALL(evt_npc_set_ry, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_NOKO_SHELL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(256.0f))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_add_rotate, STRING("me"), 0, 40, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 800, FLOAT(400.0f), 4)
    DELETE_EVENT(LW(14))
    CALL_CHILD(nokonoko_shell_return_event)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL_CHILD(nokonoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(nokonoko_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL_CHILD(nokonoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(togenoko_shell_return_event)
    CALL(evt_npc_get_unitwork, STRING("me"), 1, LW(15))
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
        CALL(evt_npc_set_unitwork, STRING("me"), 1, 0)
        CALL(evt_npc_set_attack_mode, STRING("me"), 0)
        CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
        CALL(evt_npc_set_stay_anim, STRING("me"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(togenoko_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1179672)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(togenoko_move_event)
    CALL_CHILD(togenoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(togenoko_find_event)
    CALL_CHILD(togenoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_NOKO_MOVE1"), 0, 0, 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 1, 1)
    CALL(evt_npc_set_attack_mode, STRING("me"), 11)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1"))
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
    CALL(evt_npc_set_ry, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_NOKO_SHELL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(256.0f))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_add_rotate, STRING("me"), 0, 40, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 800, FLOAT(400.0f), 4)
    DELETE_EVENT(LW(14))
    CALL_CHILD(togenoko_shell_return_event)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(togenoko_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL_CHILD(togenoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(togenoko_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL_CHILD(togenoko_shell_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patapata_move_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    SET_VALUE(LW(10), 0)
    LOOP(0)
        IF_INT_EQUAL(LW(10), 0)
            CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(-20.0f), FLOAT(20.0f))
        END_IF
        IF_INT_EQUAL(LW(10), 2)
            CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(-20.0f), FLOAT(20.0f))
        END_IF
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(30.0f))
        CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
        CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-7.0f), 0, 2)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(30.0f))
        CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
        CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(7.0f), 0, 2)
        IF_INT_GREATER_OR_EQUAL(LW(10), 4)
            BREAK_LOOP
        END_IF
        ADD(LW(10), 1)
    END_LOOP
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(patapata_find_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_pera_onoff, STRING("me"), 1)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    LOOP(18)
        CALL(evt_npc_add_rotate, STRING("me"), 0, 15, 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("PTP_A_5"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PATA_MOVE2"), LW(0), LW(1), LW(2), 0)
    LOOP(6)
        CALL(evt_npc_add_rotate, STRING("me"), 0, 15, 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 10)
    CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_PATA_MOVE3"), 0, 0, 0, 0)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(200.0f))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 1000, 0, FLOAT(-40.0f), 4, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patapata_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patapata_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(30.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(7.0f), 0, 2)
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(30.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-7.0f), 0, 2)
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(30.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(7.0f), 0, 2)
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(30.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-7.0f), 0, 2)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

// Has no END_SCRIPT of its own: a script running it goes on into met_set_anim_wait_event below.
EVT_BEGIN(met_init_event_common)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 8)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048592)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
};

EVT_BEGIN(met_set_anim_wait_event)
    CALL(evt_npc_get_btlsetup_work, STRING("me"), 0, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("MET_S_1"))
        ELSE
            CALL(evt_npc_set_anim, STRING("me"), STRING("TMT_S_1"))
        END_IF
    ELSE
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("SMT_S_1"))
        ELSE
            CALL(evt_npc_set_anim, STRING("me"), STRING("STM_S_1"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(met_set_anim_walk_event)
    CALL(evt_npc_get_btlsetup_work, STRING("me"), 0, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("MET_W_1"))
        ELSE
            CALL(evt_npc_set_anim, STRING("me"), STRING("TMT_W_1"))
        END_IF
    ELSE
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("SMT_W_1"))
        ELSE
            CALL(evt_npc_set_anim, STRING("me"), STRING("STM_W_1"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(met_init_event)
    CALL_CHILD(met_init_event_common)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
    CALL(evt_npc_set_btlsetup_work, STRING("me"), 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(met_init_event_ceil)
    CALL_CHILD(met_init_event_common)
    CALL(evt_npc_set_btlsetup_work, STRING("me"), 0, 1)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(togemet_init_event)
    CALL_CHILD(met_init_event_common)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
    CALL(evt_npc_set_btlsetup_work, STRING("me"), 0, 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    RETURN
EVT_END()

EVT_BEGIN(togemet_init_event_ceil)
    CALL_CHILD(met_init_event_common)
    CALL(evt_npc_set_btlsetup_work, STRING("me"), 0, 1)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    RETURN
EVT_END()

EVT_BEGIN(met_move_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL_CHILD(met_set_anim_wait_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    WAIT_MS(500)
    CALL_CHILD(met_set_anim_walk_event)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(met_find_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_get_btlsetup_work, STRING("me"), 0, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
        CALL(evt_npc_set_attack_mode, STRING("me"), 11)
        CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_MET_ATTACK1"), 0, 0, 0, 0)
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("MET_A_1"))
        ELSE
            CALL(evt_npc_set_anim, STRING("me"), STRING("TMT_A_1"))
        END_IF
        CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), LW(3))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(256.0f))
        BEGIN_SIBLING_EVENT
            SET_VALUE(LW(4), 0)
            SET_VALUE(LW(15), 0)
            LOOP(0)
                WAIT_FRAMES(1)
                ADD(LW(15), 1)
                IF_INT_GREATER_OR_EQUAL(LW(15), 55)
                    BREAK_LOOP
                END_IF
                ADD(LW(4), 40)
                CALL(evt_npc_set_rotate, STRING("me"), 0, LW(4), 0)
            END_LOOP
        END_SIBLING_EVENT
        WAIT_MS(100)
        BEGIN_SIBLING_EVENT_GET_ID(LW(14))
            LOOP(0)
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                ADD(LW(1), 10)
                CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
                WAIT_FRAMES(3)
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 800, FLOAT(400.0f), 4)
        DELETE_EVENT(LW(14))
        CALL(evt_npc_set_attack_mode, STRING("me"), 0)
        CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
        CALL_CHILD(met_set_anim_wait_event)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    ELSE
        CALL(evt_npc_set_attack_mode, STRING("me"), 11)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("SMT_A_1"))
        ELSE
            CALL(evt_npc_set_anim, STRING("me"), STRING("STM_A_1"))
        END_IF
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position, STRING("me"), LW(0), 0, LW(2), 0, 0, 0, 0)
        CALL(evt_npc_set_attack_mode, STRING("me"), 0)
        CALL(evt_npc_set_btlsetup_work, STRING("me"), 0, 0)
        CALL_CHILD(met_set_anim_wait_event)
        WAIT_MS(1000)
    END_IF
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(met_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL_CHILD(met_set_anim_wait_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(met_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL_CHILD(met_set_anim_wait_event)
    WAIT_MS(500)
    CALL_CHILD(met_set_anim_walk_event)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patamet_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(patamet_find_event)
    CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_PATAMET_MOVE2"), 0, 0, 0, 0)
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 0, 75, FLOAT(-50.0f), 0, 3)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 1000, 0, 0, 0, 1)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patamet_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(patamet_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(chorobon_move_event)
    CALL(evt_npc_set_scale, STRING("me"), 1, 1, 1)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    LABEL(0)
    CALL(evt_sub_random, 3, LW(4))
    ADD(LW(4), 2)
    LOOP(LW(4))
        CALL(evt_sub_random, 360, LW(3))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), 50)
        CALL(evt_npc_jump_position, STRING("me"), LW(0), -10000, LW(2), 300, 0, FLOAT(30.0f), 0)
    END_LOOP
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
    CALL(evt_npc_set_ry, STRING("me"), LW(3))
    CALL(evt_sub_random, 500, LW(4))
    ADD(LW(4), 10)
    WAIT_MS(LW(4))
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(chorobon_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    LOOP(3)
        CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), LW(3))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), 50)
        CALL(evt_npc_jump_position, STRING("me"), LW(0), -10000, LW(2), 300, 0, FLOAT(30.0f), 0)
        WAIT_MS(50)
    END_LOOP
    CALL(evt_npc_set_scale, STRING("me"), 1, 1, 1)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(chorobon_lost_event)
    CALL(evt_npc_set_scale, STRING("me"), 1, 1, 1)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(chorobon_return_event)
    CALL(evt_npc_set_scale, STRING("me"), 1, 1, 1)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), 50)
    CALL(evt_npc_jump_position, STRING("me"), LW(0), -10000, LW(2), 300, 0, FLOAT(30.0f), 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(pansy_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(pansy_find_event)
    CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_PANZI_MOVE2"), STRING("SFX_FLD_ENM_PANZI_MOVE2"), 30000, 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_homing_target, STRING("me"), STRING("mario"), 0, FLOAT(100.0f), FLOAT(10.0f), FLOAT(60.0f), 4)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(pansy_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAN_S_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(pansy_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAN_S_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAN_W_1"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(twinkling_pansy_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(twinkling_pansy_find_event)
    CALL(evt_npc_sound_data_set, STRING("me"), STRING("SFX_FLD_ENM_PANZI_MOVE2"), STRING("SFX_FLD_ENM_PANZI_MOVE2"), 30000, 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
    ADD(LW(3), 180)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(300.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 1000, FLOAT(300.0f), 6)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(twinkling_pansy_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAN_S_1_K"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(twinkling_pansy_return_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAN_S_1_K"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAN_W_1_K"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(karon_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(mahoon_get_groupname, LW(10))
    CALL(evt_npc_slave_entry, STRING("me"), 0, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_flag_onoff, 1, LW(0), 536870912)
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("KRN_H_1"))
    CALL(evt_npc_set_width, STRING("slave_0"), 24)
    CALL(evt_npc_set_height, STRING("slave_0"), 8)
    RETURN
EVT_END()

EVT_BEGIN(karon_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(karon_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    BEGIN_SIBLING_EVENT
        LABEL(0)
        CALL(evt_npc_add_rotate, STRING("slave_0"), 0, 0, 30)
        WAIT_FRAMES(1)
        GOTO(0)
    END_SIBLING_EVENT
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_A_1"))
    WAIT_MS(250)
    CALL(evt_npc_get_position, STRING("me"), LW(2), LW(3), LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_KARON_BONE_THROW1"), LW(2), LW(3), LW(4), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_A_2"))
    WAIT_MS(150)
    BEGIN_SIBLING_EVENT
        WAIT_MS(1000)
        CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_S_1"))
    END_SIBLING_EVENT
    CALL(evt_npc_set_rotate, STRING("slave_0"), 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    ADD(LW(1), 25)
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 128)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 12)
    CALL(evt_npc_jump_position, STRING("slave_0"), LW(0), LW(1), LW(2), 1000, 0, FLOAT(75.0f), 0)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(karon_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(karon_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(karon_blow_event)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

EVT_BEGIN(honenoko2_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_Z_2"))
    RETURN
EVT_END()

EVT_BEGIN(honenoko2_move_event)
    DEBUG_PUT_MSG(STRING("[EVENT] HONENOKO MOVE\n"))
    LABEL(0)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_npc_wait_msec, STRING("me"), 1000)
        GOTO(0)
    END_IF
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_D_2"))
    CALL(evt_npc_wait_msec, STRING("me"), 170)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_Z_2"))
    CALL(evt_npc_wait_msec, STRING("me"), 10)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(honenoko2_find_event)
    DEBUG_PUT_MSG(STRING("[EVENT] HONENOKO FIND\n"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_D_3"))
    CALL(evt_npc_wait_msec, STRING("me"), 170)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_D_4"))
    CALL(evt_npc_wait_msec, STRING("me"), 170)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_homing_target, STRING("me"), STRING("mario"), 0, FLOAT(120.0f), FLOAT(10.0f), FLOAT(60.0f), 4)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(honenoko2_lost_event)
    DEBUG_PUT_MSG(STRING("[EVENT] HONENOKO LOST\n"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_wait_msec, STRING("me"), 1000)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_D_2"))
    WAIT_FRAMES(10)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_Z_2"))
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(honenoko2_return_event)
    DEBUG_PUT_MSG(STRING("[EVENT] HONENOKO RETURN\n"))
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_S_1"))
    CALL(evt_npc_wait_msec, STRING("me"), 1000)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KRN_W_1"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(killer_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 64)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 4)
    RETURN
EVT_END()

EVT_BEGIN(killer_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 11)
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), 2000)
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, 200, 4)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 17, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(killer_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_damage_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_ENEMY_DIE1"), 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_drop_coin, STRING("me"), LW(3))
    IF_INT_GREATER(LW(3), 0)
        LOOP(LW(3))
            CALL(evt_item_entry, 0, 121, LW(0), LW(1), LW(2), 8, -1, 0)
        END_LOOP
    END_IF
    CALL(evt_npc_get_drop_heart, STRING("me"), LW(3))
    IF_INT_GREATER(LW(3), 0)
        LOOP(LW(3))
            CALL(evt_item_entry, 0, 123, LW(0), LW(1), LW(2), 8, -1, 0)
        END_LOOP
    END_IF
    CALL(evt_npc_get_drop_flower, STRING("me"), LW(3))
    IF_INT_GREATER(LW(3), 0)
        LOOP(LW(3))
            CALL(evt_item_entry, 0, 124, LW(0), LW(1), LW(2), 8, -1, 0)
        END_LOOP
    END_IF
    CALL(evt_npc_get_drop_item, STRING("me"), LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_item_entry, 0, LW(3), LW(0), LW(1), LW(2), 9, -1, 0)
    END_IF
    CALL(evt_npc_getback_item_entry, STRING("me"))
    CALL(evt_npc_get_drop_fixitem, STRING("me"), LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_item_entry, 0, LW(3), LW(0), LW(1), LW(2), 9, -1, 0)
    END_IF
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(killer_cannon_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_get_dir, STRING("me"), LW(0))
    CALL(evt_npc_set_unitwork, STRING("me"), 0, LW(0))
    RETURN
EVT_END()

EVT_BEGIN(killer_cannon_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_set_anim, STRING("me"), STRING("KTA_S_1"))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    WAIT_MS(3000)
    LABEL(1)
    WAIT_MS(100)
    ADD(LW(10), 1)
    IF_INT_EQUAL(LW(10), 2)
        SET_VALUE(LW(10), 0)
    END_IF
    CALL(evt_npc_set_anim, STRING("me"), STRING("KTA_A_1"))
    WAIT_MS(200)
    CALL(evt_npc_get_position, STRING("me"), LW(2), LW(3), LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_KILLER_CANNON1"), LW(2), LW(3), LW(4), 0)
    WAIT_MS(200)
    CALL(evt_npc_get_dir, STRING("me"), LW(1))
    CALL(evt_npc_get_position, STRING("me"), LW(2), LW(3), LW(4))
    CALL(evt_npc_add_dirdist, LW(2), LW(4), LW(1), 30)
    ADD(LW(3), 30)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(2), LW(3), LW(4), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_get_dir, STRING("me"), LW(1))
    CALL(killer_make_name, LW(10), LW(0))
    CALL(evt_npc_set_ry, LW(0), LW(1))
    CALL(evt_npc_get_position, STRING("me"), LW(2), LW(3), LW(4))
    CALL(evt_npc_add_dirdist, LW(2), LW(4), LW(1), 30)
    ADD(LW(3), 11)
    CALL(killer_make_name, LW(10), LW(0))
    CALL(evt_npc_set_position, LW(0), LW(2), LW(3), LW(4))
    CALL(killer_make_name, LW(10), LW(0))
    CALL(evt_npc_set_anim, LW(0), STRING("KIL_S_1"))
    CALL(killer_make_name, LW(10), LW(0))
    CALL(evt_npc_change_interrupt, LW(0), 1, PTR(killer_move_event))
    CALL(killer_make_name, LW(10), LW(0))
    CALL(evt_npc_restart_regular_event, LW(0))
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(sambo_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048592)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SNB_S_1"))
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(sambo_find_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SNB_S_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_homing_target, STRING("me"), STRING("mario"), 0, FLOAT(100.0f), FLOAT(7.0f), FLOAT(60.0f), 4)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sambo_lost_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SNB_S_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sambo_return_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SNB_S_1"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(100.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sinnosuke_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 8)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049616)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 268435456)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(sinnosuke_move_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049616)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_Z_3"))
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(sinnosuke_find_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_Z_3"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    SET_VALUE(LW(15), 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 270)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_S_1"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 270, 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 360)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    LOOP(0)
        CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_A_1a"))
        WAIT_MS(300)
        CALL(evt_npc_set_attack_mode, STRING("me"), 13)
        CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), LW(3))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(200.0f), 6)
        CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_S_1"))
        CALL(evt_npc_set_attack_mode, STRING("me"), 0)
        WAIT_MS(200)
    END_LOOP
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sinnosuke_lost_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_S_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sinnosuke_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_W_1"))
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(30.0f), 4)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_S_1"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    SET_VALUE(LW(15), 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 270)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIS_Z_3"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 270, 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 360)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sinemon_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 8)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049616)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(sinemon_move_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049616)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_Z_3"))
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(sinemon_find_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_Z_3"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    SET_VALUE(LW(15), 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 270)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_S_1"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 270, 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 360)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    LOOP(0)
        CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_A_1a"))
        WAIT_MS(300)
        CALL(evt_npc_set_attack_mode, STRING("me"), 14)
        CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
        CALL(evt_npc_set_ry, STRING("me"), LW(3))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(200.0f), 6)
        CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_S_1"))
        CALL(evt_npc_set_attack_mode, STRING("me"), 0)
        WAIT_MS(200)
    END_LOOP
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sinemon_lost_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_S_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(sinemon_return_event)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_W_1"))
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(30.0f), 4)
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_S_1"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    SET_VALUE(LW(15), 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 270)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("SIN_Z_3"))
    CALL(evt_npc_set_rotate, STRING("me"), 0, 270, 0)
    LOOP(0)
        WAIT_FRAMES(1)
        ADD(LW(15), 30)
        CALL(evt_npc_set_rotate, STRING("me"), 0, LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(LW(15), 360)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(togedaruma_move_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(togedaruma_find_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_set_attack_mode, STRING("me"), 15)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_add_rotate, STRING("me"), 0, 45, 0)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_homing_target, STRING("me"), STRING("mario"), 0, FLOAT(120.0f), FLOAT(10.0f), FLOAT(60.0f), 4)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(togedaruma_lost_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(togedaruma_return_event)
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(barriern_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_slave_entry, STRING("me"), 0, STRING("c_baria"), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_flag_onoff, 1, LW(0), 536870912)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 131072)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    RETURN
EVT_END()

EVT_BEGIN(barriern_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 131072)
    LABEL(0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_3"))
    WAIT_FRAMES(60)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_FRAMES(60)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 30, 50)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 2000, FLOAT(40.0f), 0, 0, 1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(barriern2_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 131072)
    LABEL(0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_3"))
    WAIT_FRAMES(60)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_FRAMES(60)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 30, 50)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 1000, FLOAT(120.0f), 0, 0, 1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(barriern_dead_event)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(10))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(barriern_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    LABEL(0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_2"))
    WAIT_FRAMES(30)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_3"))
    WAIT_FRAMES(10)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_4"))
    BEGIN_SIBLING_EVENT
        SET_FLOAT_VALUE(LW(15), 0)
        LOOP(60)
            ADD_FLOAT(LW(15), FLOAT(0.03f))
            CALL(evt_npc_set_scale, STRING("slave_0"), 1, LW(15), 1)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("Z_3"))
    CALL(evt_npc_blur_onoff, 1, STRING("slave_0"))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 128)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_BARRI_ATTACK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_BARRI_ATTACK2"), LW(0), LW(1), LW(2), LW(10))
    CALL(evt_npc_set_unitwork, STRING("me"), 0, LW(10))
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(500.0f))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 17)
    CALL(evt_npc_glide_position, STRING("slave_0"), LW(0), LW(1), LW(2), 1250, 10, 0, 0, 1)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(10))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_3"))
    WAIT_FRAMES(30)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_FRAMES(30)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

// The `t` temp below only exists to steer MWCC's float-register colouring: without it the branch offset
// lands in f1 and the 20.0f literal in f0, the reverse of the original. The natural form is under #else.
USER_FUNC(_2d_get_dead_jump_offset) {
    s32* args = event->args;
    NpcEntry* npc;
    MarioWork* mario;
    s32 arg1;
    s32 name;
    f32 offset;
    f32 t;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    arg1 = args[1];
    mario = marioGetPtr();
    if (mario->position.x == npc->position.x) {
        offset = 0.0f;
    } else if (mario->position.x > npc->position.x) {
        offset = -1.0f;
    } else {
        offset = 1.0f;
    }
    // Splitting the multiply into `t = 20.0f; t = t * offset;` colours offset to f0 and 20.0f to f1, as the
    // original does; the single expression `20.0f * offset` colours them the other way round.
    t = 20.0f;
    t = t * offset;
    evtSetValue(event, arg1, (s32)t);
    return EVT_RETURN_DONE;
}

USER_FUNC(killer_make_name) {
    s32* args = event->args;
    static char name[32];
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    s32 value = evtGetValue(event, args[0]);
    s32 len;

    strcpy(name, npc->description);
    len = strlen(name);
    name[len] = value + 0x31;
    name[len + 1] = '\0';
    evtSetValue(event, args[1], (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(piders_get_height) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetFloat(event, args[0], npc->territoryLoiter.z);
    return EVT_RETURN_DONE;
}

static void piders_draw_yarn_sub(CameraId cameraId, void* param) {
    NpcEntry* npc = param;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec base2;
    Vec pos2;
    Vec base = { 0.0f, 0.0f, 0.0f };
    Vec pos = { 0.0f, 0.0f, 0.0f };

    base.x = npc->territoryBase.x;
    base.y = npc->territoryBase.y + 30.0f;
    base.z = npc->territoryBase.z;
    pos.x = npc->position.x;
    pos.y = npc->position.y + 30.0f;
    pos.z = npc->position.z;

    base2 = base;
    pos2 = pos;

    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXLoadPosMtxImm(cam->view, 0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(base2.x - 1.0f, base2.y, base2.z - 5.0f);
    GXPosition3f32(base2.x + 1.0f, base2.y, base2.z - 5.0f);
    GXPosition3f32(pos2.x + 1.0f, pos2.y, pos2.z - 5.0f);
    GXPosition3f32(pos2.x - 1.0f, pos2.y, pos2.z - 5.0f);
    GXEnd();
}

EVT_BEGIN(barriern_lost_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_3"))
    WAIT_FRAMES(60)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_FRAMES(60)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(barriern_return_event)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 30, 50)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 2000, FLOAT(40.0f), 0, 0, 1)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(barriern_blow_event)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(10))
    CALL(evt_snd_sfxoff, LW(10))
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

EVT_BEGIN(piders_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PAI_S_1"))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(piders_draw_yawn)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
            SET_VALUE(LW(3), LW(1))
            SUBTRACT(LW(1), 10)
            CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 150, 0, 0, 4, 0)
            CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(3), LW(2), 150, 0, 0, 4, 0)
            WAIT_FRAMES(100)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
        LOOP(20)
            CALL(evt_npc_add_rotate, STRING("me"), 0, 2, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(0)
            LOOP(40)
                CALL(evt_npc_add_rotate, STRING("me"), 0, -2, 0)
                WAIT_FRAMES(1)
            END_LOOP
            LOOP(40)
                CALL(evt_npc_add_rotate, STRING("me"), 0, 2, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_LOOP
    END_SIBLING_EVENT
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(piders_find_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(piders_draw_yawn)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(evt_npc_add_rotate, STRING("me"), 0, 6, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(22)
            CALL(evt_npc_add_rotate, STRING("me"), 0, -5, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(20)
            CALL(evt_npc_add_rotate, STRING("me"), 0, 4, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(16)
            CALL(evt_npc_add_rotate, STRING("me"), 0, -3, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(14)
            CALL(evt_npc_add_rotate, STRING("me"), 0, 2, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(10)
            CALL(evt_npc_add_rotate, STRING("me"), 0, -1, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_set_rotate, STRING("me"), 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAIDA_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(piders_get_height, LW(15))
    SET_VALUE(LW(14), LW(15))
    DIVIDE(LW(14), 3)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    SUBTRACT(LW(1), LW(15))
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, 0, 4, 0)
    WAIT_FRAMES(60)
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAIDA_MOVE2"), LW(0), LW(1), LW(2), 0)
    ADD(LW(1), LW(14))
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 700, 0, 20, 4, 0)
    ADD(LW(1), LW(14))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAIDA_MOVE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 700, 0, 20, 4, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAIDA_MOVE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(3), LW(2), 600, 0, 0, 4, 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(piders_blow_event)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(piders_draw_yawn)
        END_LOOP
    END_SIBLING_EVENT
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

EVT_BEGIN(pakkun_attack_event)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAKKUN_ATTACK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_A_1A"))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_A_1B"))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 18)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 30)
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_A_1C"))
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    WAIT_MS(200)
    RETURN
EVT_END()

EVT_BEGIN(pakkun_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_slave_entry, STRING("me"), 0, STRING("c_pakflwr"), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_tribe, STRING("slave_0"), STRING("パックンフラワー"))
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    RETURN
EVT_END()

EVT_BEGIN(pakkun_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_S_1"))
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(pakkun_find_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAKKUN_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_E_2"))
    WAIT_FRAMES(10)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_sub_random, 10, LW(3))
    CALL(evt_mario_get_mov_spd, LW(4))
    MULTIPLY_FLOAT(LW(4), FLOAT(0.5f))
    ADD_FLOAT(LW(3), LW(4))
    ADD_FLOAT(LW(3), 30)
    CALL(evt_npc_facedirection_add, STRING("mario"), LW(0), LW(3))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 200, 0, 4)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAKKUN_MOVE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_E_1"))
    WAIT_FRAMES(10)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    WAIT_FRAMES(20)
    CALL_CHILD(pakkun_attack_event)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(pakkun_return_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_PAKKUN_MOVE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_E_1"))
    WAIT_FRAMES(10)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    WAIT_FRAMES(20)
    CALL_CHILD(pakkun_attack_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_E_2"))
    WAIT_FRAMES(10)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 200, 0, 4)
    CALL(evt_npc_set_anim, STRING("me"), STRING("PKF_E_1"))
    WAIT_FRAMES(10)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    WAIT_FRAMES(25)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(dokugassun_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 268435456)
    RETURN
EVT_END()

EVT_BEGIN(dokugassun_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(6.0f), 0, 2)
    CALL(evt_npc_sound_data_set, STRING("me"), 0, 0, 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(-6.0f), 0, 2)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(6.0f), 0, 2)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(6.0f), 0, 2)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(dokugassun_find_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(200)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(200.0f))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 10, 500)
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 1000, 1, 0, 4, 1)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(dokugassun_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(dokugassun_return_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("me"), STRING("W_1"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(-6.0f), 0, 2)
    CALL(evt_npc_sound_data_set, STRING("me"), 0, 0, 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(6.0f), 0, 2)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(-6.0f), 0, 2)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 10, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 1, FLOAT(6.0f), 0, 2)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(basabasa_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049624)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    RETURN
EVT_END()

EVT_BEGIN(basabasa_move_event)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(4), LW(1))
        CALL(evt_npc_set_walk_anim, STRING("me"))
        CALL(evt_set_dir_to_home, STRING("me"))
        CALL(evt_npc_get_dir, STRING("me"), LW(3))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
        CALL(evt_npc_get_home_position, STRING("me"), LW(4), LW(1), LW(5))
        CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-50.0f), 0, 2)
    END_IF
    LABEL(0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("BSA_S_2"))
    WAIT_FRAMES(60)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(basabasa_find_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_get_home_position, STRING("me"), LW(4), LW(5), LW(6))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(5), LW(3), 0, 75, FLOAT(-140.0f), 0, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(basabasa_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(basabasa_return_event)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_npc_get_home_position, STRING("me"), LW(4), LW(1), LW(5))
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-50.0f), 0, 2)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(basabasa2_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(basabasa2_find_event)
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 80, 500)
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 0, 75, FLOAT(-80.0f), 0, 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(basabasa2_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(basabasa2_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 80, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(teresa_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, 255)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-10.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(10.0f), 0, 3)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(teresa_find_event)
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 0, 75, FLOAT(-40.0f), 0, 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(teresa_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(teresa_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    SET_VALUE(LW(0), 255)
    LOOP(16)
        CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, LW(0))
        SUBTRACT(LW(0), 16)
        WAIT_MS(30)
    END_LOOP
    CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, 0)
    WAIT_MS(500)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("me"), LW(0), LW(1), LW(2))
    SET_VALUE(LW(0), 0)
    LOOP(16)
        ADD(LW(0), 16)
        IF_INT_GREATER(LW(0), 255)
            SET_VALUE(LW(0), 255)
        END_IF
        CALL(evt_npc_set_color, STRING("me"), 255, 255, 255, LW(0))
        WAIT_MS(30)
    END_LOOP
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(bubble_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_sound_data_set, STRING("me"), 0, 0, 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(5.0f), 0, 3)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(bubble_find_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 0, 90, FLOAT(0.0f), 0, 2)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(bubble_lost_event)
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(bubble_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_sound_data_reset, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_sound_data_set, STRING("me"), 0, 0, 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hbom_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(20.0f), 4)
    WAIT_MS(1000)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(hbom_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 300, 0, FLOAT(20.0f), 0)
    CALL(evt_npc_homing_target, STRING("me"), STRING("mario"), 0, FLOAT(60.0f), FLOAT(10.0f), FLOAT(60.0f), 4)
    WAIT_MS(500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hbom_lost_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hbom_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(30.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_slave_entry, STRING("me"), 0, STRING("c_zako_m"), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_flag_onoff, 1, LW(0), 536870912)
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("B_1A"))
    CALL(evt_npc_set_width, STRING("slave_0"), 10)
    CALL(evt_npc_set_height, STRING("slave_0"), 10)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    CALL(npc_check_wall_stop, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(500)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(1000)
    END_IF
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1B"))
    WAIT_MS(500)
    BEGIN_SIBLING_EVENT
        WAIT_MS(700)
        CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    END_SIBLING_EVENT
    CALL(evt_npc_set_rotate, STRING("slave_0"), 0, 0, 0)
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    ADD(LW(1), 25)
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 128)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("B_1A"))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 12)
    CALL(evt_npc_jump_position, STRING("slave_0"), LW(0), LW(1), LW(2), 1000, 0, FLOAT(75.0f), 0)
    CALL(evt_npc_get_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 16, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("B_2"))
    WAIT_MS(300)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_blow_event)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

EVT_BEGIN(hannya_move_event)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(hannya_find_event)
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_HANNYA_MOVE1"), LW(1), LW(2), LW(3), 0)
    CALL(evt_npc_get_home_position, STRING("me"), LW(4), LW(5), LW(6))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_npc_set_attack_mode, STRING("me"), 11)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_3"))
    CALL(evt_npc_move_position, STRING("me"), LW(1), LW(3), 0, FLOAT(180.0f), 6)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hannya_lost_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hannya_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 2)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 3000, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(mahoon_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(mahoon_get_groupname, LW(10))
    CALL(evt_npc_slave_entry, STRING("me"), 0, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("MAH_S_2A"))
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_slave_entry, STRING("me"), 1, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, STRING("slave_1"), STRING("MAH_S_2A"))
    CALL(evt_npc_set_position, STRING("slave_1"), 0, -1000, 0)
    CALL(evt_npc_slave_entry, STRING("me"), 2, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, STRING("slave_2"), STRING("MAH_S_2A"))
    CALL(evt_npc_set_position, STRING("slave_2"), 0, -1000, 0)
    CALL(evt_npc_slave_entry, STRING("me"), 3, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, STRING("slave_3"), STRING("MAH_S_2A"))
    CALL(evt_npc_set_position, STRING("slave_3"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_1"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_2"), 131072)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_3"), 131072)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_1"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_2"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_3"), 32)
    RETURN
EVT_END()

EVT_BEGIN(mahoon_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_get_dir, STRING("me"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_0"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_1"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_2"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_3"), LW(3))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_1"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_2"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_3"), LW(0), LW(1), LW(2))
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(5.0f), 0, 3)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(mahoon_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_1"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_2"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_3"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(mahoon_find_event)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_get_dir, STRING("me"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_0"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_1"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_2"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_3"), LW(3))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_1"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_2"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_3"), LW(0), LW(1), LW(2))
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_run_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_get_home_position, STRING("me"), LW(4), LW(5), LW(6))
    CALL(evt_npc_add_dirdist, LW(1), LW(3), LW(0), FLOAT(100.0f))
    CALL(evt_fbat_trans_floor_position, LW(1), LW(2), LW(3), 20, 500)
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_glide_position, STRING("me"), LW(1), LW(2), LW(3), 0, 135, FLOAT(0.0f), 0, 0)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(mahoon_lost_event)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_get_dir, STRING("me"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_0"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_1"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_2"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_3"), LW(3))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_1"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_2"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_3"), LW(0), LW(1), LW(2))
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(mahoon_return_event)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(evt_npc_get_dir, STRING("me"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_0"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_1"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_2"), LW(3))
            CALL(evt_npc_set_ry, STRING("slave_3"), LW(3))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 5)
            CALL(evt_npc_set_position, STRING("slave_1"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_2"), LW(0), LW(1), LW(2))
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(-25.0f))
            ADD(LW(1), 30)
            CALL(evt_npc_set_position, STRING("slave_3"), LW(0), LW(1), LW(2))
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 20, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(mahoon_get_groupname, LW(10))
    CALL(evt_npc_slave_entry, STRING("me"), 0, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_flag_onoff, 1, LW(0), 536870912)
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_flag_onoff, 0, LW(0), 131072)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("KMK_Z_1"))
    CALL(evt_npc_set_width, STRING("slave_0"), 10)
    CALL(evt_npc_set_height, STRING("slave_0"), 10)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    RETURN
EVT_END()

EVT_BEGIN(kamec_move_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_S_2"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    LABEL(0)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 50, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(kamec_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_A_3A"))
    WAIT_MS(700)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_A_3B"))
    WAIT_MS(150)
    BEGIN_SIBLING_EVENT
        WAIT_MS(1000)
        CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_S_2"))
    END_SIBLING_EVENT
    CALL(evt_npc_set_rotate, STRING("slave_0"), 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(20.0f))
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    ADD(LW(4), 20)
    CALL(kamec_fire_magic, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(10))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 12)
    CALL(evt_npc_glide_position, STRING("slave_0"), LW(0), LW(1), LW(2), 1000, 0, FLOAT(0.0f), 0, 3)
    CALL(kamec_fire_magic_delete, LW(10))
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_set_walk_anim, STRING("me"))
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(-5.0f), 0, 3)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(25.0f))
    CALL(evt_fbat_trans_floor_position, LW(0), LW(1), LW(2), 50, 500)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 500, 0, FLOAT(5.0f), 0, 3)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec_blow_event)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

EVT_BEGIN(kamec2_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049624)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(mahoon_get_groupname, LW(10))
    CALL(evt_npc_slave_entry, STRING("me"), 0, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_flag_onoff, 1, LW(0), 536870912)
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_flag_onoff, 0, LW(0), 131072)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("KMK_Z_1"))
    CALL(evt_npc_set_width, STRING("slave_0"), 10)
    CALL(evt_npc_set_height, STRING("slave_0"), 10)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    RETURN
EVT_END()

EVT_BEGIN(kamec2_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_S_1"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(kamec2_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_A_2A"))
    WAIT_MS(700)
    CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_A_2B"))
    WAIT_MS(150)
    BEGIN_SIBLING_EVENT
        WAIT_MS(1000)
        CALL(evt_npc_set_anim, STRING("me"), STRING("KMK_S_1"))
    END_SIBLING_EVENT
    CALL(evt_npc_set_rotate, STRING("slave_0"), 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(20.0f))
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    ADD(LW(4), 20)
    CALL(kamec_fire_magic, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(10))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 12)
    CALL(evt_npc_glide_position, STRING("slave_0"), LW(0), LW(1), LW(2), 1000, 0, FLOAT(0.0f), 0, 3)
    CALL(kamec_fire_magic_delete, LW(10))
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec2_lost_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 1, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec2_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(kamec2_blow_event)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

EVT_BEGIN(hbross_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(mahoon_get_groupname, LW(10))
    CALL(evt_npc_slave_entry, STRING("me"), 0, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_flag_onoff, 1, LW(0), 536870912)
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("BRO_A_1B"))
    CALL(evt_npc_set_width, STRING("slave_0"), 10)
    CALL(evt_npc_set_height, STRING("slave_0"), 10)
    RETURN
EVT_END()

EVT_BEGIN(hbross_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL(evt_npc_set_anim, STRING("me"), STRING("BRO_S_1"))
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(hbross_find_event)
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    BEGIN_SIBLING_EVENT
        LABEL(0)
        CALL(evt_npc_add_rotate, STRING("slave_0"), 0, 0, 30)
        WAIT_FRAMES(1)
        GOTO(0)
    END_SIBLING_EVENT
    CALL(evt_npc_set_anim, STRING("me"), STRING("BRO_A_1A"))
    WAIT_MS(400)
    BEGIN_SIBLING_EVENT
        WAIT_MS(500)
        CALL(evt_npc_set_anim, STRING("me"), STRING("BRO_S_1"))
    END_SIBLING_EVENT
    CALL(evt_npc_set_rotate, STRING("slave_0"), 0, 0, 0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    ADD(LW(1), 25)
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(20.0f))
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 128)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 12)
    CALL(evt_npc_jump_position, STRING("slave_0"), LW(0), LW(1), LW(2), 1000, 0, FLOAT(75.0f), 0)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hbross_lost_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hbross_return_event)
    CALL(evt_npc_set_stay_anim, STRING("me"))
    WAIT_MS(500)
    CALL(evt_set_dir_to_home, STRING("me"))
    CALL(evt_npc_get_dir, STRING("me"), LW(3))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(40.0f), 4)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(hbross_blow_event)
    CALL(evt_npc_set_attack_mode, STRING("slave_0"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL_CHILD(enemy_common_blow_event)
    RETURN
EVT_END()

USER_FUNC(piders_draw_yawn) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    dispEntry(CAMERA_3D, 0.0f, 1, piders_draw_yarn_sub, npc);
    return EVT_RETURN_DONE;
}

USER_FUNC(kamec_fire_magic) {
    s32* args = event->args;
    s32 a0 = evtGetValue(event, args[0]);
    s32 a1 = evtGetValue(event, args[1]);
    s32 a2 = evtGetValue(event, args[2]);
    s32 a3 = evtGetValue(event, args[3]);
    s32 a4 = evtGetValue(event, args[4]);
    s32 a5 = evtGetValue(event, args[5]);
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    EffectEntry* eff;

    eff = effMagic1N64Entry(0, 0x3C, (f32)a0, (f32)a1, (f32)a2, (f32)a3, (f32)a4, (f32)a5);
    effSetName(eff, npc->description);
    evtSetValue(event, args[6], (s32)eff);
    return EVT_RETURN_DONE;
}

USER_FUNC(kamec_fire_magic_delete) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    EffectEntry* eff = effNameToPtr(npc->description);
    if (eff != NULL) {
        ((effMagic1Data*)eff->userdata)->timer = 1;
    }
    return EVT_RETURN_DONE;
}

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

USER_FUNC(chain_main) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    NpcEntry* ref;
    NpcEntry* cur;
    f32 len;
    f32 sag;
    f32 x;
    f32 y;
    f32 z;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 dist;
    f32 t;
    s32 i;

    ref = npc->slaves[3];
    dx = npc->position.x - ref->position.x;
    dy = npc->position.y - ref->position.y;
    dz = npc->position.z - ref->position.z;
    len = (f32)sqrt(dx * dx + dy * dy + dz * dz) / 4.0f;
    if (len < 10.0f) {
        len = 10.0f;
    }
    sag = 300.0f / (len * len);

    {
    Vec pos;

    cur = npc->slaves[0];
    y = cur->position.y;
    x = cur->position.x;
    z = cur->position.z;
    y = y - sag;
    dx = npc->position.x - x;
    dy = npc->position.y - y;
    dz = npc->position.z - z;
    dist = (f32)sqrt(dx * dx + dy * dy + dz * dz);
    t = 1.0f - len / dist;
    y = dy * t + y;
    x = dx * t + x;
    z = dz * t + z;
    if (y < npc->territoryBase.y) {
        y = npc->territoryBase.y;
    }
    pos = (Vec){ 0.0f, 0.0f, 0.0f };
    pos.x = x;
    pos.y = y;
    pos.z = z;
    cur->position = pos;
    }

    for (i = 0; i < 2; i++) {
        Vec pos;

        ref = npc->slaves[i];
        cur = npc->slaves[i + 1];
        y = cur->position.y;
        y = y - sag;
        x = cur->position.x;
        z = cur->position.z;
        dx = ref->position.x - x;
        dy = ref->position.y - y;
        dz = ref->position.z - z;
        dist = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        t = 1.0f - len / dist;
        y = dy * t + y;
        x = dx * t + x;
        z = dz * t + z;
        if (y < npc->territoryBase.y) {
            y = npc->territoryBase.y;
        }
        pos = (Vec){ 0.0f, 0.0f, 0.0f };
        pos.x = x;
        pos.y = y;
        pos.z = z;
        cur->position = pos;
    }

    for (i = 3; i >= 1; i--) {
        Vec pos;

        ref = npc->slaves[i];
        cur = npc->slaves[i - 1];
        y = cur->position.y;
        y = y - sag;
        x = cur->position.x;
        z = cur->position.z;
        dx = ref->position.x - x;
        dy = ref->position.y - y;
        dz = ref->position.z - z;
        dist = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        t = 1.0f - len / dist;
        y = dy * t + y;
        x = dx * t + x;
        z = dz * t + z;
        if (y < npc->territoryBase.y) {
            y = npc->territoryBase.y;
        }
        pos = (Vec){ 0.0f, 0.0f, 0.0f };
        pos.x = x;
        pos.y = y;
        pos.z = z;
        cur->position = pos;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(wanwan_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(mahoon_get_groupname, LW(10))
    CALL(evt_npc_slave_entry, STRING("me"), 0, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, LW(0), STRING("WAN_S_1B"))
    CALL_CHILD(wanwan_init_event_sub)
    CALL(evt_npc_slave_entry, STRING("me"), 1, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, LW(0), STRING("WAN_S_1B"))
    CALL_CHILD(wanwan_init_event_sub)
    CALL(evt_npc_slave_entry, STRING("me"), 2, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, LW(0), STRING("WAN_S_1B"))
    CALL_CHILD(wanwan_init_event_sub)
    CALL(evt_npc_slave_entry, STRING("me"), 3, LW(10), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_anim, LW(0), STRING("WAN_Z_2"))
    CALL_CHILD(wanwan_init_event_sub)
    RETURN
EVT_END()

EVT_BEGIN(wanwan_init_event_sub)
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_npc_flag_onoff, 0, LW(0), 131072)
    CALL(evt_npc_flag_onoff, 0, LW(0), 16)
    CALL(evt_npc_flag_onoff, 0, LW(0), 512)
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_flag_onoff, 0, LW(0), 2048)
    CALL(evt_npc_flag_onoff, 1, LW(0), 16777216)
    CALL(evt_npc_flag_onoff, 1, LW(0), 2097152)
    CALL(evt_npc_flag_onoff, 1, LW(0), 64)
    CALL(evt_npc_flag_onoff, 1, LW(0), 262144)
    CALL(evt_npc_get_home_position, STRING("me"), LW(1), LW(2), LW(3))
    CALL(evt_npc_set_position, LW(0), LW(1), LW(2), LW(3))
    RETURN
EVT_END()

EVT_BEGIN(wanwan_move_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_set_attack_mode, STRING("me"), 0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("WAN_S_1A"))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
            IF_INT_GREATER_OR_EQUAL(LW(0), LW(10))
                CALL(evt_npc_set_ry, STRING("me"), 270)
            ELSE
                CALL(evt_npc_set_ry, STRING("me"), 90)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
        WAIT_MS(1500)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 2097152)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(chain_main)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("WAN_A_1"))
            WAIT_MS(1000)
            CALL(evt_npc_set_anim, STRING("me"), STRING("WAN_S_1A"))
            WAIT_MS(500)
        END_LOOP
    END_SIBLING_EVENT
    LABEL(0)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_sub_random, 360, LW(3))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), 50)
    CALL(evt_sub_random, 15, LW(4))
    ADD(LW(4), 15)
    SET_VALUE(LW(5), LW(4))
    SET_VALUE(LW(6), LW(4))
    MULTIPLY(LW(6), 17)
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_WAN_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), LW(6), 80, LW(5), 1)
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_WAN_LANDING1"), LW(0), LW(1), LW(2), 0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(wanwan_find_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_set_anim, STRING("me"), STRING("WAN_A_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_npc_set_attack_mode, STRING("me"), 12)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(chain_main)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_WAN_MOVE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_WAN_ATTACK2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_get_home_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(3))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), 100)
    CALL(evt_npc_glide_position, STRING("me"), LW(0), LW(1), LW(2), 0, 600, FLOAT(0.0f), 0, 6)
    WAIT_MS(1500)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

USER_FUNC(camGetPos) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    evtSetFloat(event, args[0], cam->cameraPos.x);
    return EVT_RETURN_DONE;
}

USER_FUNC(cliffCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;
    f32 xoff;
    HitEntry* hitA;
    HitEntry* hitB;

    if (npcTransRytoFaceDir(npc) == 90.0f) {
        xoff = 1.0f;
    } else {
        xoff = -1.0f;
    }
    distance = 5.0f;
    hitA = hitCheckFilter(0, npc->position.x, npc->position.y + 1.0f, npc->position.z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
                          &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    distance = 5.0f;
    hitB = hitCheckFilter(0, npc->position.x, npc->position.y + 1.0f, npc->position.z, xoff, -1.0f, 0.0f, &hitX, &hitY,
                          &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    if (hitA != NULL && hitB != NULL) {
        evtSetValue(event, args[0], 0);
    } else if (hitA != NULL && hitB == NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 2);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(upperCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;
    HitEntry* hitA;
    HitEntry* hitB;

    distance = 5.0f;
    hitA = hitCheckFilter(0, npc->position.x - 15.0f, npc->position.y + 30.0f, npc->position.z, 0.0f, 2.0f, 0.0f, &hitX, &hitY,
                          &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    hitB = hitCheckFilter(0, npc->position.x + 15.0f, npc->position.y + 30.0f, npc->position.z, 0.0f, 2.0f, 0.0f, &hitX, &hitY,
                          &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    if (hitA != NULL || hitB != NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(getSpdWalk) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetFloat(event, args[0], npc->territoryLoiter.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(getSpdRun) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetFloat(event, args[0], npc->territoryHoming.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(limitSpd) {
    s32* args = event->args;
    f32 speed = evtGetFloat(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    npc->maxFallSpeed = speed;
    return EVT_RETURN_DONE;
}

// TODO: type

// TODO: type

// TODO: type

// TODO: type

USER_FUNC(zakoEntryDokan) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    char mobName[32];
    char names[3][16];
    const char* hit;
    s32 i;

    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x50") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x50");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x51");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x52");
        strcpy(mobName, "MOBJ_mobj9");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x51") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x53");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x54");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x55");
        strcpy(mobName, "MOBJ_mobj14");
    }

    hit = hitGetName(marioGetPtr()->hitobjStandOn);
    if (hit != NULL && strcmp(hit, mobName) == 0) {
        return EVT_RETURN_DONE;
    }
    hit = hitGetName(marioGetPtr()->hitobjJumpFrom);
    if (hit != NULL && strcmp(hit, mobName) == 0) {
        return EVT_RETURN_DONE;
    }
    hit = hitGetName(marioGetPtr()->hitobjFront);
    if (hit != NULL && strcmp(hit, mobName) == 0) {
        return EVT_RETURN_DONE;
    }

    for (i = 0; i < 3; i++) {
        NpcEntry* target = npcNameToPtr(names[i]);

        if (target->position.y <= -75.0f) {
            Vec pos;

            target->flags |= 0x20410;
            target->flags &= ~0x20;
            target->jumpFlags |= 0x1002;
            target->jumpFlags &= 0xEFAFFFFB;

            pos = (Vec){ 0.0f, 0.0f, 0.0f };
            pos.x = npc->position.x;
            pos.y = npc->position.y - 35.0f;
            pos.z = npc->position.z;
            target->position = pos;
            target->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
            target->unitWork[0] = (void*)1;

            if (target->moveEvent != NULL) {
                if (target->regularEvtId != 0) {
                    evtDeleteID(target->regularEvtId);
                }
                {
                    EventEntry* e = evtEntry(target->moveEvent, 0, 0x20);
                    e->wNpcEventType = 1;
                    e->ownerNPC = target;
                    target->regularEvtId = e->eventId;
                }
            }
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(dokanCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    char buf[32];
    MapObjectEntry* mobj;

    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x50") == 0) {
        strcpy(buf, "mobj9");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x51") == 0) {
        strcpy(buf, "mobj14");
    }
    mobj = mobjNameToPtrNoAssert(buf);
    if (mobj != NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(zakoEntryFall) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    char names[3][16];
    s32 i;

    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x92\x6e\x93\x5f\x82\x50") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x50");
        strcpy(names[1], "\x82\xc8\x82\xb5");
        strcpy(names[2], "\x82\xc8\x82\xb5");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x92\x6e\x93\x5f\x82\x51\x81\x51\x82\x50") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x50");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x51");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x52");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x92\x6e\x93\x5f\x82\x51\x81\x51\x82\x51") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x53");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x54");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x55");
    }

    for (i = 0; i < 3; i++) {
        NpcEntry* target = npcNameToPtr_NoAssert(names[i]);

        if (target != NULL && target->position.y <= -75.0f) {
            Vec pos;

            target->flags |= 0x20410;
            target->flags &= ~0x20;
            target->jumpFlags |= 0x1002;
            target->jumpFlags &= 0xEF8FFFFB;

            pos = (Vec){ 0.0f, 0.0f, 0.0f };
            pos.x = npc->position.x;
            pos.y = npc->position.y;
            pos.z = npc->position.z;
            target->position = pos;
            target->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
            target->unitWork[0] = (void*)2;

            if (target->moveEvent != NULL) {
                if (target->regularEvtId != 0) {
                    evtDeleteID(target->regularEvtId);
                }
                {
                    EventEntry* e = evtEntry(target->moveEvent, 0, 0x20);
                    e->wNpcEventType = 1;
                    e->ownerNPC = target;
                    target->regularEvtId = e->eventId;
                }
            }
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(kuriboo2D_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 134217728)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    CALL(limitSpd, FLOAT(400.0f))
    RETURN
EVT_END()

EVT_BEGIN(kuriboo2D_move_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    LABEL(0)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 4096)
    CALL(evt_npc_get_position, STRING("me"), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(camGetPos, LW(1))
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), 250)
        GOTO(1)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(1)
    CALL(evt_npc_set_anim, STRING("me"), STRING("W_1"))
    LABEL(2)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 1000)
    CALL(getSpdWalk, LW(3))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, LW(3), 4)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_FRAMES(1)
    GOTO(2)
    RETURN
EVT_END()

EVT_BEGIN(kuriboo2D_find_event)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    IF_INT_GREATER_OR_EQUAL(LW(1), -100)
        CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    END_IF
    CALL(evt_npc_set_anim, STRING("me"), STRING("R_1"))
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_reverse_ry, STRING("me"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 1000)
    CALL(getSpdRun, LW(3))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 4000, LW(3), 2)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(zako2D_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 134217728)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    CALL(limitSpd, FLOAT(400.0f))
    RETURN
EVT_END()

EVT_BEGIN(zako2D_move_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    LABEL(0)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 4096)
    CALL(evt_npc_get_position, STRING("me"), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(camGetPos, LW(1))
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), 250)
        GOTO(1)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(1)
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
        CALL(evt_npc_set_anim, STRING("me"), STRING("Z_1"))
        CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
        CALL(evt_npc_set_ry, STRING("me"), LW(0))
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 16)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
        LOOP(35)
            CALL(evt_npc_status_onoff, 1, STRING("me"), 4096)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            ADD(LW(1), 1)
            CALL(evt_npc_set_position, STRING("me"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 16)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
        CALL(evt_npc_set_anim, STRING("me"), STRING("W_1"))
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 50)
        CALL(evt_npc_jump_position, STRING("me"), LW(0), LW(1), LW(2), 500, FLOAT(100.0f), FLOAT(30.0f), 1)
    END_IF
    CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
        CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 131072)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position, STRING("me"), LW(0), -100, LW(2), 0, FLOAT(100.0f), FLOAT(0.0f), 1)
    END_IF
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4096)
    CALL(evt_npc_set_anim, STRING("me"), STRING("W_1"))
    LABEL(10)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 1000)
            CALL(getSpdWalk, LW(3))
            CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, LW(3), 4)
            CALL(evt_npc_reverse_ry, STRING("me"))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    LABEL(11)
    CALL(cliffCheck, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        DELETE_EVENT(LW(15))
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_FRAMES(1)
        GOTO(10)
    END_IF
    WAIT_FRAMES(1)
    GOTO(11)
    RETURN
EVT_END()

EVT_BEGIN(zako2D_find_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    IF_INT_GREATER_OR_EQUAL(LW(1), -100)
        CALL(evt_eff_fukidashi, 1, 0, STRING("me"), 0, 0, 0, 0, 0, 0, 0, 30)
    END_IF
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1"))
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 1000)
    CALL(getSpdRun, LW(3))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 3000, LW(3), 2)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    LABEL(99)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(dokan2D_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 160)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    RETURN
EVT_END()

EVT_BEGIN(dokan2D_regl_event)
    LABEL(0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(camGetPos, LW(1))
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), 250)
        GOTO(1)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(1)
    CALL(dokanCheck, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(zakoEntryDokan)
    WAIT_FRAMES(120)
    GOTO(1)
    LABEL(99)
    CALL(evt_npc_delete, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(fall2D_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 160)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    RETURN
EVT_END()

EVT_BEGIN(fall2D_regl_event)
    LABEL(0)
    CALL(evt_npc_get_position, STRING("me"), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(camGetPos, LW(1))
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), 250)
        GOTO(1)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(1)
    CALL(zakoEntryFall)
    WAIT_FRAMES(120)
    GOTO(1)
    RETURN
EVT_END()

EVT_BEGIN(zakoM2D_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 134217728)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    CALL(limitSpd, FLOAT(400.0f))
    CALL(evt_npc_slave_entry, STRING("me"), 0, STRING("c_zako_km"), LW(0), PTR(enemy_slave_common_dead_event))
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(set_bottle_flag_init)
    RETURN
EVT_END()

EVT_BEGIN(zakoM2D_move_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    LABEL(2)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 4096)
    CALL(evt_npc_status_check, STRING("me"), 4, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(3)
    END_IF
    WAIT_FRAMES(1)
    GOTO(2)
    LABEL(3)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 100, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    LABEL(0)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 4096)
    CALL(evt_npc_get_position, STRING("me"), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(camGetPos, LW(1))
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), 250)
        GOTO(1)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(1)
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    WAIT_FRAMES(1)
    GOTO(1)
    RETURN
EVT_END()

EVT_BEGIN(zakoM2D_find_event)
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("B_1"))
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 10)
    ADD(LW(1), 20)
    CALL(evt_npc_set_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 32)
    CALL(evt_npc_flag_onoff, 0, STRING("slave_0"), 128)
    IF_INT_GREATER_OR_EQUAL(LW(10), 200)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_facedirection_add, STRING("me"), LW(0), 100)
        CALL(evt_npc_jump_position, STRING("slave_0"), LW(0), LW(1), LW(2), 0, FLOAT(300.0f), FLOAT(300.0f), 0)
    ELSE
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_facedirection_add, STRING("me"), LW(0), LW(10))
        MULTIPLY_FLOAT(LW(10), FLOAT(1.5f))
        CALL(evt_npc_jump_position, STRING("slave_0"), LW(0), LW(1), LW(2), 0, LW(10), FLOAT(300.0f), 0)
    END_IF
    CALL(evt_npc_get_position, STRING("slave_0"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_CONDITION_FIRE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ENM_3RD2_CRASH1"), LW(0), LW(1), LW(2), 0)
    SUBTRACT(LW(1), 5)
    ADD(LW(2), 200)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 16, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    LOOP(12)
        CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(unk_800ece0c, LW(0), LW(1), LW(2), 60)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("slave_0"), STRING("B_2"))
    WAIT_FRAMES(10)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    WAIT_FRAMES(60)
    CALL(evt_npc_set_position, STRING("slave_0"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 32)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_803497e8)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 100, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
    SUBTRACT_FLOAT(LW(10), LW(0))
    IF_INT_LESS(LW(10), 0)
        MULTIPLY_FLOAT(LW(10), FLOAT(-1.0f))
    END_IF
    IF_INT_LESS(LW(10), 30)
        SET_FLOAT_VALUE(LW(10), FLOAT(30.0f))
    END_IF
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1"))
    WAIT_FRAMES(64)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_2"))
    WAIT_FRAMES(24)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    END_SIBLING_EVENT
    SPAWN_EVENT(zakoM2D_find_event)
    WAIT_FRAMES(100)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(gesso2D_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 134217728)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 268435456)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2097152)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 4)
    RETURN
EVT_END()

EVT_BEGIN(gesso2D_move_event)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    LABEL(0)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 4096)
    CALL(evt_npc_get_position, STRING("me"), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(camGetPos, LW(1))
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), 250)
        GOTO(1)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    LABEL(1)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(13), FLOAT(0.0f))
    LOOP(0)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        WAIT_FRAMES(1)
        SUBTRACT(LW(0), LW(10))
        IF_INT_GREATER_OR_EQUAL(LW(0), 300)
            GOTO(5)
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(0), -300)
            GOTO(5)
        END_IF
        IF_INT_LESS(LW(1), LW(11))
            BREAK_LOOP
        END_IF
        IF_INT_LESS(LW(1), 90)
            BREAK_LOOP
        END_IF
        LABEL(5)
        CALL(gesso_ground_check, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            SET_FLOAT_VALUE(LW(14), LW(1))
            SUBTRACT_FLOAT(LW(14), LW(13))
            ADD_FLOAT(LW(13), FLOAT(0.1f))
            IF_INT_GREATER_OR_EQUAL(LW(13), FLOAT(1.0f))
                SET_FLOAT_VALUE(LW(13), FLOAT(1.0f))
            END_IF
            CALL(evt_npc_set_position, STRING("me"), LW(0), LW(14), LW(2))
        END_IF
    END_LOOP
    CALL(evt_get_target_dir, STRING("me"), STRING("mario"), LW(0))
    CALL(evt_npc_set_ry, STRING("me"), LW(0))
    CALL(evt_npc_get_position, STRING("me"), LW(10), LW(11), LW(12))
    CALL(evt_npc_facedirection_add, STRING("me"), LW(10), 100)
    ADD(LW(11), 50)
    IF_INT_GREATER(LW(11), 350)
        SET_VALUE(LW(11), 350)
    END_IF
    LOOP(60)
        CALL(gesso_ground_check, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            SET_FLOAT_VALUE(LW(14), LW(1))
            SUBTRACT_FLOAT(LW(14), LW(13))
            ADD_FLOAT(LW(13), FLOAT(0.1f))
            IF_INT_GREATER_OR_EQUAL(LW(13), FLOAT(1.0f))
                SET_FLOAT_VALUE(LW(13), FLOAT(1.0f))
            END_IF
            CALL(evt_npc_set_position, STRING("me"), LW(0), LW(14), LW(2))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_1"))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_GESSO_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_set_unitwork, STRING("me"), 0, 0)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(evt_npc_glide_position, STRING("me"), LW(10), LW(11), LW(12), 2000, FLOAT(50.0f), 10, 4, 0)
        CALL(evt_npc_set_unitwork, STRING("me"), 0, 1)
    END_SIBLING_EVENT
    LOOP(0)
        CALL(evt_npc_get_unitwork, STRING("me"), 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        CALL(upperCheck, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            DELETE_EVENT(LW(15))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    GOTO(1)
    RETURN
EVT_END()

EVT_BEGIN(enemy_event_dead_event)
    SET_VALUE(LW(10), STRING("me"))
    CALL_CHILD(enemy_event_dead_event_sub)
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(enemy_slave_common_dead_event)
    CALL(evt_npc_delete, STRING("me"))
    CALL(evt_npc_return_interrupt, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 269616152)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(testenemynpc_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    RETURN
EVT_END()

EVT_BEGIN(npc_urouro_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1536)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(kuriboo_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    RETURN
EVT_END()

EVT_BEGIN(patakuri_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(patapata_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1114136)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(patamet_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(chorobon_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1179672)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(pansy_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    RETURN
EVT_END()

EVT_BEGIN(twinkling_pansy_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    RETURN
EVT_END()

EVT_BEGIN(karon_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(sambo_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 8)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1049616)
    RETURN
EVT_END()

EVT_BEGIN(togedaruma_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    RETURN
EVT_END()

EVT_BEGIN(piders_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    RETURN
EVT_END()

EVT_BEGIN(basabasa_dead_event)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(basabasa2_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(basabasa2_dead_event)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(teresa_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(teresa_dead_event)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(bubble_init_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1048600)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(hbom_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    RETURN
EVT_END()

EVT_BEGIN(zakowiz_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(hannya_init_event)
    CALL_CHILD(enemy_common_flag_init_event)
    CALL(evt_npc_status_onoff, 0, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(kamec_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(kamec2_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(hbross_dead_event)
    CALL(evt_npc_flag_onoff, 1, STRING("slave_0"), 128)
    CALL_CHILD(enemy_common_dead_event)
    RETURN
EVT_END()

EVT_BEGIN(zakoM2D_dead_event)
    CALL(evt_npc_status_onoff, 0, STRING("slave_0"), 4)
    CALL_CHILD(enemy_2d_common_dead_event)
    RETURN
EVT_END()

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type

// TODO: type
// clang-format on

USER_FUNC(unk_800ece0c) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    s32* args = event->args;
    f32 f31 = evtGetFloat(event, args[0]);
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    s32 v = evtGetValue(event, args[3]);
    EffectEntry* eff;

    if (isFirstCall) {
        eff = effFireEntry(5, 1000, f31, f30, f29, 1.0f);
        npc->unitWork[0] = eff;
        npc->unitWork[1] = (void*)v;
    }
    eff = npc->unitWork[0];
    npc->unitWork[1] = (void*)((s32)npc->unitWork[1] - 1);
    if ((s32)npc->unitWork[1] <= 0x1E) {
        f32 t = intplGetValue(1, 0x1E - (s32)npc->unitWork[1], 0x1E, 1.0f, 0.0f);
        ((effFireData*)eff->userdata)->scaleX = t;
        ((effFireData*)eff->userdata)->scaleY = t;
        ((effFireData*)eff->userdata)->alpha =
            (u8)(s32)intplGetValue(0, 0x1E - (s32)npc->unitWork[1], 0x1E, 255.0f, 0.0f);
    }
    if ((s32)npc->unitWork[1] <= 0) {
        effDelete(eff);
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(set_bottle_flag_init) {
    NpcEntry* npc = evtNpcNameToPtr(event, "slave_0");
    npc->flags |= 0x28020020;
    npc->hazardImmuneFlags |= 0x1F;
    return EVT_RETURN_DONE;
}

// From driver/mobjdrv.c (no prototype in header)

// From driver/npcdrv.c (not yet decompiled / no prototype in header)
// From mario/... (not yet decompiled)
// From driver/effdrv.c (not yet decompiled)

USER_FUNC(gesso_ground_check) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");

    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;

    distance = 5.0f;
    if (hitCheckFilter(0, npc->position.x, npc->position.y + 1.0f, npc->position.z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
                       &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}
