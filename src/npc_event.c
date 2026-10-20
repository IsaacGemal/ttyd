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

s32 enemy_common_dead_event[156] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_damage_anim, (s32)"me",
    0x0001000A, 0x000001F4, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000F005B, (s32)evt_eff, 0x00000000, (s32)"kemuri_test",
    0x00000004, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xF24A7DB3, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_snd_sfxon, (s32)"SFX_ENEMY_DIE1", 0x00000000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0003005B, (s32)evt_npc_get_drop_coin,
    (s32)"me", 0xFE363C83, 0x0002001B, 0xFE363C83,
    0x00000000, 0x00010005, 0xFE363C83, 0x0009005B,
    (s32)evt_item_entry, 0x00000000, 0x00000079, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000000A, 0xFFFFFFFF,
    0x00000000, 0x00000006, 0x00000021, 0x0003005B,
    (s32)evt_npc_get_drop_heart, (s32)"me", 0xFE363C83, 0x0002001B,
    0xFE363C83, 0x00000000, 0x00010005, 0xFE363C83,
    0x0009005B, (s32)evt_item_entry, 0x00000000, 0x0000007B,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000000A,
    0xFFFFFFFF, 0x00000000, 0x00000006, 0x00000021,
    0x0003005B, (s32)evt_npc_get_drop_flower, (s32)"me", 0xFE363C83,
    0x0002001B, 0xFE363C83, 0x00000000, 0x00010005,
    0xFE363C83, 0x0009005B, (s32)evt_item_entry, 0x00000000,
    0x0000007C, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000000A, 0xFFFFFFFF, 0x00000000, 0x00000006,
    0x00000021, 0x0003005B, (s32)evt_npc_get_drop_item, (s32)"me",
    0xFE363C83, 0x00020019, 0xFE363C83, 0x00000000,
    0x0009005B, (s32)evt_item_entry, 0x00000000, 0xFE363C83,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000009,
    0xFFFFFFFF, 0x00000000, 0x00000021, 0x0002005B,
    (s32)evt_npc_getback_item_entry, (s32)"me", 0x0003005B, (s32)evt_npc_get_drop_fixitem,
    (s32)"me", 0xFE363C83, 0x00020019, 0xFE363C83,
    0x00000000, 0x0009005B, (s32)evt_item_entry, 0x00000000,
    0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000009, 0xFFFFFFFF, 0x00000000, 0x00000021,
    0x0002005B, (s32)evt_npc_delete, (s32)"me", 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 enemy_event_dead_event_sub[36] = {
    0x0002005B, (s32)evt_npc_set_damage_anim, 0xFE363C8A, 0x0001000A,
    0x000001F4, 0x0005005B, (s32)evt_npc_get_position, 0xFE363C8A,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000F005B,
    (s32)evt_eff, 0x00000000, (s32)"kemuri_test", 0x00000004,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xF24A7DB3,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0003005B,
    (s32)evt_snd_sfxon, (s32)"SFX_ENEMY_DIE1", 0x00000000, 0x0002005B,
    (s32)evt_npc_delete, 0xFE363C8A, 0x00000002, 0x00000001,
};
s32 enemy_common_blow_event[13] = {
    0x0002005B, (s32)evt_npc_set_confuse_anim, (s32)"me", 0x0002005B,
    (s32)_wait_blow_end, (s32)"me", 0x0001000A, 0x000007D0,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 enemy_2d_common_dead_event[415] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x10000000, 0x0002005B, (s32)pressCheck,
    0xFE363C8F, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0x000000C8, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000F005B,
    (s32)evt_eff, 0x00000000, (s32)"kemuri_test", 0x00000004,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xF24A7DB3,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0003005B,
    (s32)evt_snd_sfxon, (s32)"SFX_ENEMY_DIE1", 0x00000000, 0x0002005B,
    (s32)evt_npc_set_damage_anim, (s32)"me", 0x0003005B, (s32)evt_npc_get_kpencount_type,
    (s32)"me", 0xFE363C80, 0x00010022, 0xFE363C80,
    0x0001002D, 0x00000001, 0x00020018, 0xFE363C8F,
    0x00000000, 0x0002005B, (s32)evt_npc_calc_score, (s32)"me",
    0x0001005C, (s32)&_2d_dead_rotate_event, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)_2d_get_dead_jump_offset, (s32)"me", 0xFE363C83,
    0x00020035, 0xFE363C80, 0xFE363C83, 0x0009005B,
    (s32)_2d_dead_jump, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000258, 0xF24B1A80, 0xF24A7A80,
    0xFFFFFED4, 0x00000020, 0x0002005B, (s32)evt_npc_calc_score,
    (s32)"me", 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"me", 0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0004005B,
    (s32)evt_npc_status_onoff, 0x00000000, (s32)"me", 0x00000002,
    0x0002005B, (s32)evt_npc_set_talk_anim, (s32)"me", 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00020036, 0xFE363C81, 0x00000005,
    0x0005005B, (s32)evt_npc_set_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00010009, 0x0000001E,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000006E, 0x00010005,
    0x0000001E, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0x00000000, 0x00000001, 0x00010009,
    0x00000001, 0x00000006, 0x00000070, 0x00020036,
    0xFE363C80, 0x00000028, 0x00020036, 0xFE363C81,
    0x00000014, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xFFFFFFF6, 0x00000004, 0x00000000,
    0x00010005, 0x00000000, 0x0000006E, 0x00010005,
    0x0000003C, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00010009,
    0x00000001, 0x00000006, 0x00000070, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00020035, 0xFE363C80, 0x00000050,
    0x00020036, 0xFE363C81, 0x00000014, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000003E8, 0x00000000, 0xFFFFFFF6,
    0x00000004, 0x00000000, 0x0000006E, 0x00010005,
    0x0000003C, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0x00000000, 0x00000001, 0x00010009,
    0x00000001, 0x00000006, 0x00000070, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00020036, 0xFE363C80, 0x00000050,
    0x00020036, 0xFE363C81, 0x00000014, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000003E8, 0x00000000, 0xFFFFFFF6,
    0x00000004, 0x00000000, 0x0002001C, 0xFE363C81,
    0xFFFFFF38, 0x00000007, 0x00000021, 0x00000006,
    0x00000021, 0x0001002D, 0x00000002, 0x0002005B,
    (s32)evt_npc_calc_score, (s32)"me", 0x0001005C, (s32)&_2d_dead_rotate_event,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000001, (s32)"me",
    0x00400000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)_2d_get_dead_jump_offset, (s32)"me", 0xFE363C83, 0x00020035,
    0xFE363C80, 0xFE363C83, 0x0009005B, (s32)_2d_dead_jump,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000258, 0xF24B1A80, 0xF24B7E80, 0xFFFFFED4,
    0x0001002D, 0x00000004, 0x0002005B, (s32)evt_npc_calc_score,
    (s32)"me", 0x0001005C, (s32)&_2d_dead_rotate_event, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0003005B, (s32)_2d_get_dead_jump_offset, (s32)"me",
    0xFE363C83, 0x00020035, 0xFE363C80, 0xFE363C83,
    0x0009005B, (s32)_2d_dead_jump, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000258, 0xF24B1A80,
    0xF24B7E80, 0xFFFFFED4, 0x0001002D, 0x00000020,
    0x0001005C, (s32)&_2d_dead_rotate_event, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)_2d_get_dead_jump_offset, (s32)"me", 0xFE363C83,
    0x00020035, 0xFE363C80, 0xFE363C83, 0x0009005B,
    (s32)_2d_dead_jump, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000258, 0xF24B1A80, 0xF24B7E80,
    0xFFFFFED4, 0x0001002D, 0x00000008, 0x0002005B,
    (s32)evt_npc_calc_score, (s32)"me", 0x0001005C, (s32)&_2d_dead_rotate_event,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0003005B, (s32)_2d_get_dead_jump_offset,
    (s32)"me", 0xFE363C83, 0x00020035, 0xFE363C80,
    0xFE363C83, 0x0009005B, (s32)_2d_dead_jump, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000258,
    0xF24B1A80, 0xF24B7E80, 0xFFFFFED4, 0x0001002D,
    0x00000010, 0x0002005B, (s32)evt_npc_calc_score, (s32)"me",
    0x0001005C, (s32)&_2d_dead_rotate_event, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00400000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0003005B, (s32)_2d_get_dead_jump_offset, (s32)"me",
    0xFE363C83, 0x00020035, 0xFE363C80, 0xFE363C83,
    0x0009005B, (s32)_2d_dead_jump, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000258, 0xF24B1A80,
    0xF24B7E80, 0xFFFFFED4, 0x0000002A, 0x0001000A,
    0x000001F4, 0x00000031, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 _2d_dead_rotate_event[16] = {
    0x00020032, 0xFE363C80, 0xFFFFFFF4, 0x00010005,
    0x0000000F, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0x00000000, 0xFE363C80, 0x00010009,
    0x00000001, 0x00000006, 0x00000002, 0x00000001,
};
s32 unk_evt_8033d458[13] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x00010003, 0x00000000, 0x0001000A,
    0x000001F4, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 npc_urouro_move_event[21] = {
    0x0001005E, (s32)&npc_urouro_init_event, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0007005B, (s32)urouro_init_func, (s32)"me", 0xFE363C80,
    0xFE363C82, 0xF24C0A80, 0xF24AF280, 0x00000000,
    0x0002005B, (s32)urouro_main_func, (s32)"me", 0x00000002,
    0x00000001,
};
s32 npc_urouro_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 kuriboo_move_event[39] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x00010003, 0x00000000,
    0x0001000A, 0x000001F4, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80,
    0xF24E3A80, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24C0A80, 0x0007005B,
    (s32)evt_npc_move_position, (s32)"me", 0xFE363C80, 0xFE363C82,
    0x000007D0, 0xF24B1A80, 0x00000004, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 kuriboo_find_event[54] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24ACA80, 0x00000000, 0x0008005B, (s32)evt_npc_homing_target,
    (s32)"me", (s32)"mario", 0x00000000, 0xF24C5A80,
    0xF24AA280, 0xF24B6A80, 0x00000004, 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 kuriboo_lost_event[31] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 kuriboo_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 patakuri_move_event[148] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_sub_random, 0x00000168,
    0xFE363C83, 0x00010003, 0x00000000, 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000032, 0xF24A5280,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000032, 0xF24AA280, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000032, 0xF24A5280,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000032, 0xF24AA280, 0x00000000, 0x00000003,
    0x00010004, 0x00000000, 0x00000002, 0x00000001,
};
s32 patakuri_find_event[90] = {
    0x0002005B, (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C81, 0xFE363C83, 0xFE363C80,
    0xF24C0A80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000000, 0x0000004B,
    0xF249B280, 0x00000000, 0x00000003, 0x0002005B,
    (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C81, 0xFE363C82, 0xFE363C83,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C81, 0xFE363C82, 0xFE363C83,
    0x000003E8, 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 patakuri_lost_event[31] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 patakuri_return_event[170] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me",
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x000003E8, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B, (s32)evt_set_dir_to_home,
    (s32)"me", 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000000,
    0xF24A5280, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000000, 0xF24AA280, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000000,
    0xF24A5280, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000000, 0xF24AA280, 0x00000000,
    0x00000003, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 nokonoko_shell_return_event[50] = {
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000001,
    0xFE363C8F, 0x00020018, 0xFE363C8F, 0x00000001,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000001, 0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me",
    0x00000001, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0009005B, (s32)evt_npc_jump_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000012C,
    0x00000000, 0xF24ACA80, 0x00000000, 0x00000021,
    0x00000002, 0x00000001,
};
s32 nokonoko_init_event[28] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00120018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0x00000000, 0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me",
    0x00000001, 0x00000000, 0x00000002, 0x00000001,
};
s32 nokonoko_move_event[58] = {
    0x0001005E, (s32)&nokonoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x00010003,
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim,
    (s32)"me", 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000007D0,
    0xF24B1A80, 0x00000004, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 nokonoko_find_event[172] = {
    0x0001005E, (s32)&nokonoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000000, 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24ACA80, 0x00000000, 0x0007005B, (s32)evt_npc_sound_data_set,
    (s32)"me", (s32)"SFX_FLD_ENM_NOKO_MOVE1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (s32)evt_npc_set_unitwork,
    (s32)"me", 0x00000001, 0x00000001, 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"me", 0x0000000B, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"NKT_A_1", 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C83,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_FLD_ENM_NOKO_SHELL1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24E7A80, 0x0000006E,
    0x00010005, 0x00000000, 0x0004005B, (s32)evt_npc_get_unitwork,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00020018,
    0xFE363C8F, 0x00000001, 0x00000007, 0x00000021,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0x00000028, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00000070, 0x0001000A, 0x00000064,
    0x0001006F, 0xFE363C8E, 0x00010005, 0x00000000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00020035, 0xFE363C81,
    0x0000000A, 0x000F005B, (s32)evt_eff, (s32)"",
    (s32)"kemuri_test", 0x00000000, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xF24A7BB3, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00010009, 0x00000003, 0x00000006,
    0x00000070, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000320, 0xF250BA80,
    0x00000004, 0x0001005F, 0xFE363C8E, 0x0001005E,
    (s32)&nokonoko_shell_return_event, 0x0001000A, 0x000001F4, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 nokonoko_lost_event[46] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0001005E,
    (s32)&nokonoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 nokonoko_return_event[55] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0001005E,
    (s32)&nokonoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24B1A80, 0x00000004, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 togenoko_shell_return_event[50] = {
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000001,
    0xFE363C8F, 0x00020018, 0xFE363C8F, 0x00000001,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000001, 0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me",
    0x00000001, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0009005B, (s32)evt_npc_jump_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000012C,
    0x00000000, 0xF24ACA80, 0x00000000, 0x00000021,
    0x00000002, 0x00000001,
};
s32 togenoko_init_event[28] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00120018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0x00000000, 0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me",
    0x00000001, 0x00000000, 0x00000002, 0x00000001,
};
s32 togenoko_move_event[58] = {
    0x0001005E, (s32)&togenoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x00010003,
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim,
    (s32)"me", 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000007D0,
    0xF24B1A80, 0x00000004, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 togenoko_find_event[172] = {
    0x0001005E, (s32)&togenoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000000, 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24ACA80, 0x00000000, 0x0007005B, (s32)evt_npc_sound_data_set,
    (s32)"me", (s32)"SFX_FLD_ENM_NOKO_MOVE1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (s32)evt_npc_set_unitwork,
    (s32)"me", 0x00000001, 0x00000001, 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"me", 0x0000000B, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"A_1", 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C83,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_FLD_ENM_NOKO_SHELL1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24E7A80, 0x0000006E,
    0x00010005, 0x00000000, 0x0004005B, (s32)evt_npc_get_unitwork,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00020018,
    0xFE363C8F, 0x00000001, 0x00000007, 0x00000021,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0x00000028, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00000070, 0x0001000A, 0x00000064,
    0x0001006F, 0xFE363C8E, 0x00010005, 0x00000000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00020035, 0xFE363C81,
    0x0000000A, 0x000F005B, (s32)evt_eff, (s32)"",
    (s32)"kemuri_test", 0x00000000, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xF24A7BB3, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00010009, 0x00000003, 0x00000006,
    0x00000070, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000320, 0xF250BA80,
    0x00000004, 0x0001005F, 0xFE363C8E, 0x0001005E,
    (s32)&togenoko_shell_return_event, 0x0001000A, 0x000001F4, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 togenoko_lost_event[46] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0001005E,
    (s32)&togenoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 togenoko_return_event[55] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0001005E,
    (s32)&togenoko_shell_return_event, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24B1A80, 0x00000004, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 patapata_move_event[123] = {
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x0002005B, (s32)evt_npc_sound_data_reset,
    (s32)"me", 0x00010003, 0x00000000, 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x00020032,
    0xFE363C8A, 0x00000000, 0x00010005, 0x00000000,
    0x00020018, 0xFE363C8A, 0x00000000, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24A2A80, 0xF24ACA80,
    0x00000021, 0x00020018, 0xFE363C8A, 0x00000002,
    0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24A2A80,
    0xF24ACA80, 0x00000021, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24AF280, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A5E80, 0x00000000, 0x00000002,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24AF280,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A9680,
    0x00000000, 0x00000002, 0x0002001D, 0xFE363C8A,
    0x00000004, 0x00000007, 0x00000021, 0x00020035,
    0xFE363C8A, 0x00000001, 0x00000006, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 patapata_find_event[131] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0003005B, (s32)evt_npc_pera_onoff, (s32)"me", 0x00000001,
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00010005, 0x00000012,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0x0000000F, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"PTP_A_5", 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PATA_MOVE2", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00010005, 0x00000006,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0x0000000F, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x0005005B, (s32)evt_npc_set_rotate, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"me", 0x0000000A, 0x0007005B,
    (s32)evt_npc_sound_data_set, (s32)"me", (s32)"SFX_FLD_ENM_PATA_MOVE3", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C80,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C81, 0xFE363C83, 0xFE363C80, 0xF24D9A80,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x000003E8, 0x00000000, 0xF249DA80,
    0x00000004, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 patapata_lost_event[44] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry,
    (s32)"me", 0x0001000A, 0x000003E8, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 patapata_return_event[135] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim,
    (s32)"me", 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24AF280,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A9680,
    0x00000000, 0x00000002, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24AF280,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A5E80,
    0x00000000, 0x00000002, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24AF280,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A9680,
    0x00000000, 0x00000002, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24AF280,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A5E80,
    0x00000000, 0x00000002, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
u32 met_init_event_common[21] = {
    0x0004005B, (u32)evt_npc_flag_onoff, 0x00000001, (u32)"me",
    0x00000008, 0x0004005B, (u32)evt_npc_flag_onoff, 0x00000001,
    (u32)"me", 0x00100010, 0x0004005B, (u32)evt_npc_status_onoff,
    0x00000001, (u32)"me", 0x00000002, 0x0005005B,
    (u32)evt_npc_set_rotate, (u32)"me", 0x00000000, 0x00000000,
    0x00000000,
};
s32 met_set_anim_wait_event[48] = {
    0x0004005B, (s32)evt_npc_get_btlsetup_work, (s32)"me", 0x00000000,
    0xFE363C8F, 0x00020018, 0xFE363C8F, 0x00000000,
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0xFE363C8F, 0x00020018, 0xFE363C8F, 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"MET_S_1",
    0x00000020, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"TMT_S_1", 0x00000021, 0x00000020, 0x0004005B,
    (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000, 0xFE363C8F,
    0x00020018, 0xFE363C8F, 0x00000000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"SMT_S_1", 0x00000020,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"STM_S_1",
    0x00000021, 0x00000021, 0x00000002, 0x00000001,
};
s32 met_set_anim_walk_event[48] = {
    0x0004005B, (s32)evt_npc_get_btlsetup_work, (s32)"me", 0x00000000,
    0xFE363C8F, 0x00020018, 0xFE363C8F, 0x00000000,
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0xFE363C8F, 0x00020018, 0xFE363C8F, 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"MET_W_1",
    0x00000020, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"TMT_W_1", 0x00000021, 0x00000020, 0x0004005B,
    (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000, 0xFE363C8F,
    0x00020018, 0xFE363C8F, 0x00000000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"SMT_W_1", 0x00000020,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"STM_W_1",
    0x00000021, 0x00000021, 0x00000002, 0x00000001,
};
s32 met_init_event[19] = {
    0x0001005E, (s32)&met_init_event_common, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_set_btlsetup_work, (s32)"me", 0x00000000, 0x00000000,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000000, 0x00000002, 0x00000001,
};
s32 met_init_event_ceil[14] = {
    0x0001005E, (s32)&met_init_event_common, 0x0004005B, (s32)evt_npc_set_btlsetup_work,
    (s32)"me", 0x00000000, 0x00000001, 0x0004005B,
    (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000, 0x00000000,
    0x00000002, 0x00000001,
};
s32 togemet_init_event[19] = {
    0x0001005E, (s32)&met_init_event_common, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_set_btlsetup_work, (s32)"me", 0x00000000, 0x00000000,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000001, 0x00000002, 0x00000001,
};
s32 togemet_init_event_ceil[14] = {
    0x0001005E, (s32)&met_init_event_common, 0x0004005B, (s32)evt_npc_set_btlsetup_work,
    (s32)"me", 0x00000000, 0x00000001, 0x0004005B,
    (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000, 0x00000001,
    0x00000002, 0x00000001,
};
s32 met_move_event[54] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0001005E, (s32)&met_set_anim_wait_event, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x00010003,
    0x00000000, 0x0001000A, 0x000001F4, 0x0001005E,
    (s32)&met_set_anim_walk_event, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000007D0,
    0xF24B1A80, 0x00000004, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 met_find_event[269] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0002005B, (s32)evt_npc_sound_data_reset,
    (s32)"me", 0x0004005B, (s32)evt_npc_get_btlsetup_work, (s32)"me",
    0x00000000, 0xFE363C8F, 0x00020018, 0xFE363C8F,
    0x00000000, 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24ACA80, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x0000000B, 0x0007005B, (s32)evt_npc_sound_data_set,
    (s32)"me", (s32)"SFX_FLD_ENM_MET_ATTACK1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (s32)evt_npc_get_unitwork,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"MET_A_1", 0x00000020, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"TMT_A_1", 0x00000021,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24E7A80, 0x0000006E, 0x00020032, 0xFE363C84,
    0x00000000, 0x00020032, 0xFE363C8F, 0x00000000,
    0x00010005, 0x00000000, 0x00010009, 0x00000001,
    0x00020035, 0xFE363C8F, 0x00000001, 0x0002001D,
    0xFE363C8F, 0x00000037, 0x00000007, 0x00000021,
    0x00020035, 0xFE363C84, 0x00000028, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0xFE363C84,
    0x00000000, 0x00000006, 0x00000070, 0x0001000A,
    0x00000064, 0x0001006F, 0xFE363C8E, 0x00010005,
    0x00000000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C81, 0x0000000A, 0x000F005B, (s32)evt_eff,
    (s32)"", (s32)"kemuri_test", 0x00000000, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0xF24A7BB3, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00010009, 0x00000003,
    0x00000006, 0x00000070, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000320,
    0xF250BA80, 0x00000004, 0x0001005F, 0xFE363C8E,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0001005E, (s32)&met_set_anim_wait_event,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0009005B, (s32)evt_npc_jump_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000012C, 0x00000000, 0xF24ACA80, 0x00000000,
    0x00000020, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x0000000B, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00020000, 0x0004005B, (s32)evt_npc_get_unitwork,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SMT_A_1", 0x00000020, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"STM_A_1", 0x00000021,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0009005B, (s32)evt_npc_jump_position,
    (s32)"me", 0xFE363C80, 0x00000000, 0xFE363C82,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0004005B, (s32)evt_npc_set_btlsetup_work, (s32)"me", 0x00000000,
    0x00000000, 0x0001005E, (s32)&met_set_anim_wait_event, 0x0001000A,
    0x000003E8, 0x00000021, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 met_lost_event[43] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000, 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0001005E, (s32)&met_set_anim_wait_event, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 met_return_event[51] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x0001005E, (s32)&met_set_anim_wait_event, 0x0001000A,
    0x000001F4, 0x0001005E, (s32)&met_set_anim_walk_event, 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24B1A80, 0x00000004, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 patamet_move_event[151] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_sub_random, 0x00000168,
    0xFE363C83, 0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me",
    0x00010003, 0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x0001000A, 0x000001F4, 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24A5280, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24AA280, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24A5280, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24AA280, 0x00000000, 0x00000003, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 patamet_find_event[108] = {
    0x0007005B, (s32)evt_npc_sound_data_set, (s32)"me", (s32)"SFX_FLD_ENM_PATAMET_MOVE2",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0002005B, (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C81, 0xFE363C83, 0xFE363C80,
    0xF24C0A80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000000, 0x0000004B,
    0xF249B280, 0x00000000, 0x00000003, 0x0002005B,
    (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry,
    (s32)"me", 0x0001000A, 0x000001F4, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x000003E8, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 patamet_lost_event[34] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 patamet_return_event[143] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A5280, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24AA280,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A5280, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24AA280,
    0x00000000, 0x00000003, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 chorobon_move_event[69] = {
    0x0005005B, (s32)evt_npc_set_scale, (s32)"me", 0x00000001,
    0x00000001, 0x00000001, 0x0002005B, (s32)evt_npc_set_walk_anim,
    (s32)"me", 0x00010003, 0x00000000, 0x0003005B,
    (s32)evt_sub_random, 0x00000003, 0xFE363C84, 0x00020035,
    0xFE363C84, 0x00000002, 0x00010005, 0xFE363C84,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0x00000032,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFFFFD8F0, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24AF280, 0x00000000, 0x00000006, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C83,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C83,
    0x0003005B, (s32)evt_sub_random, 0x000001F4, 0xFE363C84,
    0x00020035, 0xFE363C84, 0x0000000A, 0x0001000A,
    0xFE363C84, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 chorobon_find_event[63] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x00010005, 0x00000003, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C83,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0x00000032,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFFFFD8F0, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24AF280, 0x00000000, 0x0001000A, 0x00000032,
    0x00000006, 0x0005005B, (s32)evt_npc_set_scale, (s32)"me",
    0x00000001, 0x00000001, 0x00000001, 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 chorobon_lost_event[14] = {
    0x0005005B, (s32)evt_npc_set_scale, (s32)"me", 0x00000001,
    0x00000001, 0x00000001, 0x0002005B, (s32)evt_npc_set_walk_anim,
    (s32)"me", 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 chorobon_return_event[43] = {
    0x0005005B, (s32)evt_npc_set_scale, (s32)"me", 0x00000001,
    0x00000001, 0x00000001, 0x0002005B, (s32)evt_npc_set_walk_anim,
    (s32)"me", 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0x00000032,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFFFFD8F0, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24AF280, 0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 pansy_move_event[42] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x0002005B, (s32)evt_npc_sound_data_reset,
    (s32)"me", 0x00010003, 0x00000000, 0x0001000A,
    0x000001F4, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000007D0,
    0xF24B1A80, 0x00000004, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 pansy_find_event[46] = {
    0x0007005B, (s32)evt_npc_sound_data_set, (s32)"me", (s32)"SFX_FLD_ENM_PANZI_MOVE2",
    (s32)"SFX_FLD_ENM_PANZI_MOVE2", 0x00007530, 0x00000000, 0x00000000,
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0008005B, (s32)evt_npc_homing_target,
    (s32)"me", (s32)"mario", 0x00000000, 0xF24C0A80,
    0xF24AA280, 0xF24B6A80, 0x00000004, 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 pansy_lost_event[35] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PAN_S_1", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 pansy_return_event[45] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PAN_S_1", 0x0001000A,
    0x000001F4, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"PAN_W_1", 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 twinkling_pansy_move_event[42] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x0002005B, (s32)evt_npc_sound_data_reset,
    (s32)"me", 0x00010003, 0x00000000, 0x0001000A,
    0x000001F4, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000007D0,
    0xF24B1A80, 0x00000004, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 twinkling_pansy_find_event[54] = {
    0x0007005B, (s32)evt_npc_sound_data_set, (s32)"me", (s32)"SFX_FLD_ENM_PANZI_MOVE2",
    (s32)"SFX_FLD_ENM_PANZI_MOVE2", 0x00007530, 0x00000000, 0x00000000,
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C83, 0x00020035, 0xFE363C83,
    0x000000B4, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24F2A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x000003E8, 0xF24F2A80,
    0x00000006, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 twinkling_pansy_lost_event[35] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PAN_S_1_K", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 twinkling_pansy_return_event[45] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PAN_S_1_K", 0x0001000A,
    0x000001F4, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"PAN_W_1_K", 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 karon_init_event[36] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0002005B, (s32)mahoon_get_groupname,
    0xFE363C8A, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000000, 0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
    0x20000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x00000020, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"slave_0", (s32)"KRN_H_1", 0x0003005B, (s32)evt_npc_set_width,
    (s32)"slave_0", 0x00000018, 0x0003005B, (s32)evt_npc_set_height,
    (s32)"slave_0", 0x00000008, 0x00000002, 0x00000001,
};
s32 karon_move_event[57] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000080, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1", 0x0003005B,
    (s32)evt_sub_random, 0x00000168, 0xFE363C83, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x00010003, 0x00000000, 0x0001000A, 0x000001F4,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x000007D0, 0xF24B1A80,
    0x00000004, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 karon_find_event[150] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0000006E,
    0x00010003, 0x00000000, 0x0005005B, (s32)evt_npc_add_rotate,
    (s32)"slave_0", 0x00000000, 0x00000000, 0x0000001E,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00000070, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KRN_A_1", 0x0001000A, 0x000000FA, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C82, 0xFE363C83,
    0xFE363C84, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_KARON_BONE_THROW1",
    0xFE363C82, 0xFE363C83, 0xFE363C84, 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"KRN_A_2",
    0x0001000A, 0x00000096, 0x0000006E, 0x0001000A,
    0x000003E8, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KRN_S_1", 0x00000070, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"slave_0", 0x00000000, 0x00000000, 0x00000000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00020035, 0xFE363C81,
    0x00000019, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0", 0x00000080,
    0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"slave_0", 0x00000020, 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x0000000C, 0x0009005B,
    (s32)evt_npc_jump_position, (s32)"slave_0", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000003E8, 0x00000000, 0xF24BA680,
    0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0",
    0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"slave_0", 0x00000020, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_0", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00200000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 karon_lost_event[18] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 karon_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 karon_blow_event[19] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0001005E,
    (s32)&enemy_common_blow_event, 0x00000002, 0x00000001,
};
s32 honenoko2_init_event[18] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0004005B,
    (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000, 0x00000001,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"KRN_Z_2",
    0x00000002, 0x00000001,
};
s32 honenoko2_move_event[49] = {
    0x00010071, (s32)"[EVENT] HONENOKO MOVE\n", 0x00010003, 0x00000000,
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0003005B, (s32)evt_npc_wait_msec, (s32)"me", 0x000003E8,
    0x00010004, 0x00000000, 0x00000021, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000001, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KRN_D_2", 0x0003005B, (s32)evt_npc_wait_msec, (s32)"me",
    0x000000AA, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KRN_Z_2", 0x0003005B, (s32)evt_npc_wait_msec, (s32)"me",
    0x0000000A, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 honenoko2_find_event[58] = {
    0x00010071, (s32)"[EVENT] HONENOKO FIND\n", 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"KRN_D_3", 0x0003005B,
    (s32)evt_npc_wait_msec, (s32)"me", 0x000000AA, 0x0004005B,
    (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"KRN_D_4",
    0x0003005B, (s32)evt_npc_wait_msec, (s32)"me", 0x000000AA,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000400, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0008005B, (s32)evt_npc_homing_target,
    (s32)"me", (s32)"mario", 0x00000000, 0xF24C5A80,
    0xF24AA280, 0xF24B6A80, 0x00000004, 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 honenoko2_lost_event[36] = {
    0x00010071, (s32)"[EVENT] HONENOKO LOST\n", 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0003005B,
    (s32)evt_npc_wait_msec, (s32)"me", 0x000003E8, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000001, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KRN_D_2", 0x00010009, 0x0000000A, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"KRN_Z_2", 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 honenoko2_return_event[46] = {
    0x00010071, (s32)"[EVENT] HONENOKO RETURN\n", 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"KRN_S_1", 0x0003005B, (s32)evt_npc_wait_msec,
    (s32)"me", 0x000003E8, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"KRN_W_1", 0x0002005B, (s32)evt_set_dir_to_home,
    (s32)"me", 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000000, 0xF24B1A80,
    0x00000004, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 killer_init_event[19] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x00000040,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000004, 0x00000002, 0x00000001,
};
s32 killer_move_event[79] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_status_onoff, 0x00000000, (s32)"me", 0x00000004,
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x0000000B, 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0x000007D0, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0x000000C8, 0x00000004, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020035, 0xFE363C81, 0x0000000F, 0x000F005B,
    (s32)evt_eff, (s32)"", (s32)"kemuri_test", 0x00000011,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xF24A7E80,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"me", 0x00000000, 0xFFFFFC18,
    0x00000000, 0x00000002, 0x00000001,
};
s32 killer_dead_event[156] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_set_damage_anim, (s32)"me",
    0x0001000A, 0x000001F4, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000F005B, (s32)evt_eff, 0x00000000, (s32)"kemuri_test",
    0x00000004, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xF24A7DB3, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_snd_sfxon, (s32)"SFX_ENEMY_DIE1", 0x00000000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0003005B, (s32)evt_npc_get_drop_coin,
    (s32)"me", 0xFE363C83, 0x0002001B, 0xFE363C83,
    0x00000000, 0x00010005, 0xFE363C83, 0x0009005B,
    (s32)evt_item_entry, 0x00000000, 0x00000079, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000008, 0xFFFFFFFF,
    0x00000000, 0x00000006, 0x00000021, 0x0003005B,
    (s32)evt_npc_get_drop_heart, (s32)"me", 0xFE363C83, 0x0002001B,
    0xFE363C83, 0x00000000, 0x00010005, 0xFE363C83,
    0x0009005B, (s32)evt_item_entry, 0x00000000, 0x0000007B,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000008,
    0xFFFFFFFF, 0x00000000, 0x00000006, 0x00000021,
    0x0003005B, (s32)evt_npc_get_drop_flower, (s32)"me", 0xFE363C83,
    0x0002001B, 0xFE363C83, 0x00000000, 0x00010005,
    0xFE363C83, 0x0009005B, (s32)evt_item_entry, 0x00000000,
    0x0000007C, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000008, 0xFFFFFFFF, 0x00000000, 0x00000006,
    0x00000021, 0x0003005B, (s32)evt_npc_get_drop_item, (s32)"me",
    0xFE363C83, 0x00020019, 0xFE363C83, 0x00000000,
    0x0009005B, (s32)evt_item_entry, 0x00000000, 0xFE363C83,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000009,
    0xFFFFFFFF, 0x00000000, 0x00000021, 0x0002005B,
    (s32)evt_npc_getback_item_entry, (s32)"me", 0x0003005B, (s32)evt_npc_get_drop_fixitem,
    (s32)"me", 0xFE363C83, 0x00020019, 0xFE363C83,
    0x00000000, 0x0009005B, (s32)evt_item_entry, 0x00000000,
    0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000009, 0xFFFFFFFF, 0x00000000, 0x00000021,
    0x0005005B, (s32)evt_npc_set_position, (s32)"me", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x00000002, 0x00000001,
};
s32 killer_cannon_init_event[13] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_set_unitwork,
    (s32)"me", 0x00000000, 0xFE363C80, 0x00000002,
    0x00000001,
};
s32 killer_cannon_move_event[157] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_get_unitwork,
    (s32)"me", 0x00000000, 0xFE363C80, 0x0003005B,
    (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"KTA_S_1", 0x00020032,
    0xFE363C8A, 0x00000000, 0x00010003, 0x00000000,
    0x0001000A, 0x00000BB8, 0x00010003, 0x00000001,
    0x0001000A, 0x00000064, 0x00020035, 0xFE363C8A,
    0x00000001, 0x00020018, 0xFE363C8A, 0x00000002,
    0x00020032, 0xFE363C8A, 0x00000000, 0x00000021,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"KTA_A_1",
    0x0001000A, 0x000000C8, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C82, 0xFE363C83, 0xFE363C84,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_KILLER_CANNON1", 0xFE363C82,
    0xFE363C83, 0xFE363C84, 0x00000000, 0x0001000A,
    0x000000C8, 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C81, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C82, 0xFE363C83, 0xFE363C84, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C82, 0xFE363C84, 0xFE363C81,
    0x0000001E, 0x00020035, 0xFE363C83, 0x0000001E,
    0x000F005B, (s32)evt_eff, (s32)"", (s32)"kemuri_test",
    0x00000012, 0xFE363C82, 0xFE363C83, 0xFE363C84,
    0xF24A8080, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C81,
    0x0003005B, (s32)killer_make_name, 0xFE363C8A, 0xFE363C80,
    0x0003005B, (s32)evt_npc_set_ry, 0xFE363C80, 0xFE363C81,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C82,
    0xFE363C83, 0xFE363C84, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C82, 0xFE363C84, 0xFE363C81, 0x0000001E,
    0x00020035, 0xFE363C83, 0x0000000B, 0x0003005B,
    (s32)killer_make_name, 0xFE363C8A, 0xFE363C80, 0x0005005B,
    (s32)evt_npc_set_position, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xFE363C84, 0x0003005B, (s32)killer_make_name, 0xFE363C8A,
    0xFE363C80, 0x0003005B, (s32)evt_npc_set_anim, 0xFE363C80,
    (s32)"KIL_S_1", 0x0003005B, (s32)killer_make_name, 0xFE363C8A,
    0xFE363C80, 0x0004005B, (s32)evt_npc_change_interrupt, 0xFE363C80,
    0x00000001, (s32)&killer_move_event, 0x0003005B, (s32)killer_make_name,
    0xFE363C8A, 0xFE363C80, 0x0002005B, (s32)evt_npc_restart_regular_event,
    0xFE363C80, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 sambo_move_event[22] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00100010, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SNB_S_1", 0x00010003, 0x00000000,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 sambo_find_event[33] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SNB_S_1",
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0008005B, (s32)evt_npc_homing_target, (s32)"me",
    (s32)"mario", 0x00000000, 0xF24C0A80, 0xF24A9680,
    0xF24B6A80, 0x00000004, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 sambo_lost_event[22] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SNB_S_1",
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 sambo_return_event[30] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SNB_S_1",
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0005005B,
    (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000000, 0xF24C0A80,
    0x00000004, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 sinnosuke_init_event[26] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000008, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00100410, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00000002, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x10000000,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x00000002, 0x00000001,
};
s32 sinnosuke_move_event[21] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100410, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"SIS_Z_3", 0x00010003, 0x00000000, 0x00010009,
    0x00000001, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 sinnosuke_find_event[146] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"SIS_Z_3", 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00020032, 0xFE363C8F, 0x00000000, 0x00010005,
    0x00000000, 0x00010009, 0x00000001, 0x00020035,
    0xFE363C8F, 0x0000001E, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00000000,
    0x0002001D, 0xFE363C8F, 0x0000010E, 0x00000007,
    0x00000021, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIS_S_1", 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x0000010E, 0x00000000,
    0x00010005, 0x00000000, 0x00010009, 0x00000001,
    0x00020035, 0xFE363C8F, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0xFE363C8F,
    0x00000000, 0x0002001D, 0xFE363C8F, 0x00000168,
    0x00000007, 0x00000021, 0x00000006, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00010005, 0x00000000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"SIS_A_1a", 0x0001000A,
    0x0000012C, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x0000000D, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24D9A80, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIS_S_1", 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0001000A, 0x000000C8,
    0x00000006, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 sinnosuke_lost_event[36] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SIS_S_1",
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry,
    (s32)"me", 0x0001000A, 0x000003E8, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 sinnosuke_return_event[94] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SIS_W_1",
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24AF280, 0x00000004, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIS_S_1", 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00020032, 0xFE363C8F, 0x00000000, 0x00010005,
    0x00000000, 0x00010009, 0x00000001, 0x00020035,
    0xFE363C8F, 0x0000001E, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00000000,
    0x0002001D, 0xFE363C8F, 0x0000010E, 0x00000007,
    0x00000021, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIS_Z_3", 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x0000010E, 0x00000000,
    0x00010005, 0x00000000, 0x00010009, 0x00000001,
    0x00020035, 0xFE363C8F, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0xFE363C8F,
    0x00000000, 0x0002001D, 0xFE363C8F, 0x00000168,
    0x00000007, 0x00000021, 0x00000006, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 sinemon_init_event[26] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000008, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00100410, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_status_onoff, 0x00000001, (s32)"me", 0x00000002,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x00000002, 0x00000001,
};
s32 sinemon_move_event[26] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100410, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00020000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIN_Z_3", 0x00010003, 0x00000000,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 sinemon_find_event[146] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"SIN_Z_3", 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00020032, 0xFE363C8F, 0x00000000, 0x00010005,
    0x00000000, 0x00010009, 0x00000001, 0x00020035,
    0xFE363C8F, 0x0000001E, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00000000,
    0x0002001D, 0xFE363C8F, 0x0000010E, 0x00000007,
    0x00000021, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIN_S_1", 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x0000010E, 0x00000000,
    0x00010005, 0x00000000, 0x00010009, 0x00000001,
    0x00020035, 0xFE363C8F, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0xFE363C8F,
    0x00000000, 0x0002001D, 0xFE363C8F, 0x00000168,
    0x00000007, 0x00000021, 0x00000006, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00010005, 0x00000000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"SIN_A_1a", 0x0001000A,
    0x0000012C, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x0000000E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24D9A80, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIN_S_1", 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0001000A, 0x000000C8,
    0x00000006, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me",
    0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 sinemon_lost_event[36] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SIN_S_1",
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry,
    (s32)"me", 0x0001000A, 0x000003E8, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 sinemon_return_event[94] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"SIN_W_1",
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x00000000,
    0xF24AF280, 0x00000004, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIN_S_1", 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00020032, 0xFE363C8F, 0x00000000, 0x00010005,
    0x00000000, 0x00010009, 0x00000001, 0x00020035,
    0xFE363C8F, 0x0000001E, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0xFE363C8F, 0x00000000,
    0x0002001D, 0xFE363C8F, 0x0000010E, 0x00000007,
    0x00000021, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"SIN_Z_3", 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x0000010E, 0x00000000,
    0x00010005, 0x00000000, 0x00010009, 0x00000001,
    0x00020035, 0xFE363C8F, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0xFE363C8F,
    0x00000000, 0x0002001D, 0xFE363C8F, 0x00000168,
    0x00000007, 0x00000021, 0x00000006, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 togedaruma_move_event[49] = {
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0001005E, (s32)&enemy_common_flag_init_event,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x00010003, 0x00000000, 0x0001000A, 0x000001F4,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x000007D0, 0xF24B1A80,
    0x00000004, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 togedaruma_find_event[61] = {
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C80,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x0000000F,
    0x0000006E, 0x00010005, 0x00000000, 0x00010009,
    0x00000001, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0x0000002D, 0x00000000, 0x00000006,
    0x00000070, 0x0008005B, (s32)evt_npc_homing_target, (s32)"me",
    (s32)"mario", 0x00000000, 0xF24C5A80, 0xF24AA280,
    0xF24B6A80, 0x00000004, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 togedaruma_lost_event[38] = {
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 togedaruma_return_event[44] = {
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x00000000, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24C0A80, 0x0007005B,
    (s32)evt_npc_move_position, (s32)"me", 0xFE363C80, 0xFE363C82,
    0x00000000, 0xF24B1A80, 0x00000004, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 barriern_init_event[37] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0006005B, (s32)evt_npc_slave_entry,
    (s32)"me", 0x00000000, (s32)"c_baria", 0xFE363C80,
    (s32)&enemy_slave_common_dead_event, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x20000000, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_0", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"slave_0", 0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000020, 0x00000002,
    0x00000001,
};
s32 barriern_move_event[89] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000080,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"S_1", 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"me", 0x00200000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0", 0x00020000,
    0x00010003, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"S_3", 0x00010009, 0x0000003C,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x00010009, 0x0000003C, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80,
    0xF24E3A80, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24C0A80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000001E, 0x00000032, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000007D0, 0xF24B1A80, 0x00000000, 0x00000000,
    0x00000001, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 barriern2_move_event[70] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00020000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0",
    0x00020000, 0x00010003, 0x00000000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"S_3", 0x00010009,
    0x0000003C, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"S_1", 0x00010009, 0x0000003C, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83,
    0xF24C5A80, 0xF24E3A80, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0000001E, 0x00000032, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000003E8, 0xF24C5A80, 0x00000000,
    0x00000000, 0x00000001, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 barriern_dead_event[17] = {
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0xFE363C8A, 0x0002005B, (s32)evt_snd_sfxoff, 0xFE363C8A,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0001005E, (s32)&enemy_common_dead_event, 0x00000002,
    0x00000001,
};
s32 barriern_find_event[189] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x00010003, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"A_2", 0x00010009, 0x0000001E,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"A_3",
    0x00010009, 0x0000000A, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"A_4", 0x0000006E, 0x00020034,
    0xFE363C8F, 0x00000000, 0x00010005, 0x0000003C,
    0x0002003A, 0xFE363C8F, 0xF24A7A9E, 0x0005005B,
    (s32)evt_npc_set_scale, (s32)"slave_0", 0x00000001, 0xFE363C8F,
    0x00000001, 0x00010009, 0x00000001, 0x00000006,
    0x00000070, 0x0003005B, (s32)evt_npc_set_anim, (s32)"slave_0",
    (s32)"Z_3", 0x0003005B, (s32)evt_npc_blur_onoff, 0x00000001,
    (s32)"slave_0", 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"slave_0", 0x00000080, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_FLD_ENM_BARRI_ATTACK1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_BARRI_ATTACK2",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C8A,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0xFE363C8A, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF2524A80,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0",
    0x00000020, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0",
    0x00000011, 0x000A005B, (s32)evt_npc_glide_position, (s32)"slave_0",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000004E2,
    0x0000000A, 0x00000000, 0x00000000, 0x00000001,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000020, 0x0004005B,
    (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000, 0xFE363C8A,
    0x0002005B, (s32)evt_snd_sfxoff, 0xFE363C8A, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"S_3", 0x00010009,
    0x0000001E, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"S_1", 0x00010009, 0x0000001E, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};

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

s32 barriern_lost_event[17] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_3",
    0x00010009, 0x0000003C, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"S_1", 0x00010009, 0x0000003C,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 barriern_return_event[42] = {
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24C0A80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000001E, 0x00000032, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000007D0, 0xF24B1A80, 0x00000000, 0x00000000,
    0x00000001, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 barriern_blow_event[27] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000020, 0x0004005B,
    (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000, 0xFE363C8A,
    0x0002005B, (s32)evt_snd_sfxoff, 0xFE363C8A, 0x0001005E,
    (s32)&enemy_common_blow_event, 0x00000002, 0x00000001,
};
s32 piders_move_event[111] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"PAI_S_1", 0x0000006E, 0x00010005, 0x00000000,
    0x00010009, 0x00000001, 0x0001005B, (s32)piders_draw_yawn,
    0x00000006, 0x00000070, 0x0000006E, 0x00010005,
    0x00000000, 0x0005005B, (s32)evt_npc_get_home_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020032,
    0xFE363C83, 0xFE363C81, 0x00020036, 0xFE363C81,
    0x0000000A, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000096,
    0x00000000, 0x00000000, 0x00000004, 0x00000000,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C83, 0xFE363C82, 0x00000096, 0x00000000,
    0x00000000, 0x00000004, 0x00000000, 0x00010009,
    0x00000064, 0x00000006, 0x00000070, 0x0000006E,
    0x0005005B, (s32)evt_npc_set_rotate, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00010005, 0x00000014,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0x00000002, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00010005, 0x00000000, 0x00010005,
    0x00000028, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0xFFFFFFFE, 0x00000000, 0x00010009,
    0x00000001, 0x00000006, 0x00010005, 0x00000028,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0x00000002, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00000006, 0x00000070, 0x00010003,
    0x00000000, 0x00010009, 0x00000001, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 piders_find_event[206] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000400, 0x0000006E, 0x00010005, 0x00000000,
    0x00010009, 0x00000001, 0x0001005B, (s32)piders_draw_yawn,
    0x00000006, 0x00000070, 0x0005005B, (s32)evt_npc_set_rotate,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x0000006B, 0x00010005, 0x0000000A, 0x0005005B,
    (s32)evt_npc_add_rotate, (s32)"me", 0x00000000, 0x00000006,
    0x00000000, 0x00010009, 0x00000001, 0x00000006,
    0x00010005, 0x00000016, 0x0005005B, (s32)evt_npc_add_rotate,
    (s32)"me", 0x00000000, 0xFFFFFFFB, 0x00000000,
    0x00010009, 0x00000001, 0x00000006, 0x00010005,
    0x00000014, 0x0005005B, (s32)evt_npc_add_rotate, (s32)"me",
    0x00000000, 0x00000004, 0x00000000, 0x00010009,
    0x00000001, 0x00000006, 0x00010005, 0x00000010,
    0x0005005B, (s32)evt_npc_add_rotate, (s32)"me", 0x00000000,
    0xFFFFFFFD, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00010005, 0x0000000E, 0x0005005B,
    (s32)evt_npc_add_rotate, (s32)"me", 0x00000000, 0x00000002,
    0x00000000, 0x00010009, 0x00000001, 0x00000006,
    0x00010005, 0x0000000A, 0x0005005B, (s32)evt_npc_add_rotate,
    (s32)"me", 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00010009, 0x00000001, 0x00000006, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x0000006D, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAIDA_MOVE1", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x0002005B,
    (s32)piders_get_height, 0xFE363C8F, 0x00020032, 0xFE363C8E,
    0xFE363C8F, 0x00020038, 0xFE363C8E, 0x00000003,
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00020032, 0xFE363C83,
    0xFE363C81, 0x00020036, 0xFE363C81, 0xFE363C8F,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000000,
    0x00000000, 0x00000004, 0x00000000, 0x00010009,
    0x0000003C, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAIDA_MOVE2",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00020035, 0xFE363C81, 0xFE363C8E, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000002BC, 0x00000000, 0x00000014,
    0x00000004, 0x00000000, 0x00020035, 0xFE363C81,
    0xFE363C8E, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAIDA_MOVE2",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000002BC, 0x00000000,
    0x00000014, 0x00000004, 0x00000000, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAIDA_MOVE2", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C83, 0xFE363C82,
    0x00000258, 0x00000000, 0x00000000, 0x00000004,
    0x00000000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 piders_blow_event[13] = {
    0x0000006E, 0x00010005, 0x00000000, 0x00010009,
    0x00000001, 0x0001005B, (s32)piders_draw_yawn, 0x00000006,
    0x00000070, 0x0001005E, (s32)&enemy_common_blow_event, 0x00000002,
    0x00000001,
};
s32 pakkun_attack_event[74] = {
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_FLD_ENM_PAKKUN_ATTACK1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"PKF_A_1A", 0x0001000A, 0x000000C8, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PKF_A_1B", 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0", 0x00000020,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000012,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_facedirection_add,
    (s32)"me", 0xFE363C80, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0001000A, 0x000000C8, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PKF_A_1C", 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000020,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0001000A, 0x000000C8,
    0x00000002, 0x00000001,
};
s32 pakkun_init_event[34] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000000, (s32)"c_pakflwr", 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"slave_0", (s32)"パックンフラワー",
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000020,
    0x00000002, 0x00000001,
};
s32 pakkun_move_event[17] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00200000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"PKF_S_1", 0x00010003, 0x00000000, 0x00010009,
    0x00000001, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 pakkun_find_event[106] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00200000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAKKUN_MOVE1", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00000020, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"PKF_E_2", 0x00010009,
    0x0000000A, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)evt_sub_random, 0x0000000A, 0xFE363C83, 0x0002005B,
    (s32)evt_mario_get_mov_spd, 0xFE363C84, 0x0002003C, 0xFE363C84,
    0xF24A7C80, 0x0002003A, 0xFE363C83, 0xFE363C84,
    0x0002003A, 0xFE363C83, 0x0000001E, 0x0004005B,
    (s32)evt_npc_facedirection_add, (s32)"mario", 0xFE363C80, 0xFE363C83,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x000000C8, 0x00000000, 0x00000004,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAKKUN_MOVE2", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"PKF_E_1", 0x00010009, 0x0000000A,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000020, 0x00010009, 0x00000014, 0x0001005E,
    (s32)&pakkun_attack_event, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 pakkun_return_event[81] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00200000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_PAKKUN_MOVE2", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"PKF_E_1", 0x00010009, 0x0000000A,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000020, 0x00010009, 0x00000014, 0x0001005E,
    (s32)&pakkun_attack_event, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00000020, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"PKF_E_2", 0x00010009, 0x0000000A,
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000000C8,
    0x00000000, 0x00000004, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"PKF_E_1", 0x00010009, 0x0000000A,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000020, 0x00010009, 0x00000019, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 dokugassun_init_event[14] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x10000000,
    0x00000002, 0x00000001,
};
s32 dokugassun_move_event[153] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_sub_random, 0x00000168,
    0xFE363C83, 0x00010003, 0x00000000, 0x0001000A,
    0x000001F4, 0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83,
    0xF24C5A80, 0xF24E3A80, 0x0002005B, (s32)evt_npc_sound_data_reset,
    (s32)"me", 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000000A, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000001,
    0xF24A9280, 0x00000000, 0x00000002, 0x0007005B,
    (s32)evt_npc_sound_data_set, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000000A, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000001, 0xF24A6280, 0x00000000,
    0x00000002, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000000A, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000001,
    0xF24A9280, 0x00000000, 0x00000002, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000000A, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000001, 0xF24A9280, 0x00000000,
    0x00000002, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 dokugassun_find_event[69] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x00000400,
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0001000A, 0x000000C8, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C80,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C81, 0xFE363C83, 0xFE363C80, 0xF24D9A80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x0000000A, 0x000001F4, 0x0003005B,
    (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x000003E8, 0x00000001, 0x00000000,
    0x00000004, 0x00000001, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 dokugassun_lost_event[27] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 dokugassun_return_event[158] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"S_1", 0x0001000A, 0x000001F4, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"W_1", 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0002005B, (s32)evt_npc_sound_data_reset,
    (s32)"me", 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000000A, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000001,
    0xF24A6280, 0x00000000, 0x00000002, 0x0007005B,
    (s32)evt_npc_sound_data_set, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000000A, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000001, 0xF24A9280, 0x00000000,
    0x00000002, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000000A, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000001,
    0xF24A6280, 0x00000000, 0x00000002, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000000A, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000001, 0xF24A9280, 0x00000000,
    0x00000002, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 basabasa_init_event[17] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100418, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x00000002,
    0x00000001,
};
s32 basabasa_move_event[67] = {
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C83, 0xFE363C84, 0xFE363C85,
    0x0002001A, 0xFE363C84, 0xFE363C81, 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B, (s32)evt_set_dir_to_home,
    (s32)"me", 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0005005B, (s32)evt_npc_get_home_position, (s32)"me",
    0xFE363C84, 0xFE363C81, 0xFE363C85, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF249B280,
    0x00000000, 0x00000002, 0x00000021, 0x00010003,
    0x00000000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"BSA_S_2", 0x00010009, 0x0000003C, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 basabasa_find_event[69] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00200000, 0x0002005B, (s32)evt_npc_set_run_anim, (s32)"me",
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C81, 0xFE363C82, 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C81, 0xFE363C83, 0xFE363C80, 0xF24C0A80,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C85, 0xFE363C83, 0x00000000, 0x0000004B,
    0xF2484A80, 0x00000000, 0x00000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 basabasa_lost_event[31] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 basabasa_return_event[44] = {
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0005005B, (s32)evt_npc_get_home_position,
    (s32)"me", 0xFE363C84, 0xFE363C81, 0xFE363C85,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000000,
    0xF249B280, 0x00000000, 0x00000002, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 basabasa2_move_event[148] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_sub_random, 0x00000168,
    0xFE363C83, 0x00010003, 0x00000000, 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000050, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000032, 0xF24A5280,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000050,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000032, 0xF24AA280, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000050, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000032, 0xF24A5280,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000050,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000032, 0xF24AA280, 0x00000000, 0x00000003,
    0x00010004, 0x00000000, 0x00000002, 0x00000001,
};
s32 basabasa2_find_event[60] = {
    0x0002005B, (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C81, 0xFE363C83, 0xFE363C80,
    0xF24C0A80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000050, 0x000001F4,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000000, 0x0000004B,
    0xF2493A80, 0x00000000, 0x00000000, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 basabasa2_lost_event[31] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 basabasa2_return_event[140] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me",
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000050, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000000, 0xF24A5280, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000050, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000000,
    0xF24AA280, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000050, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0x00000000, 0xF24A5280, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000050, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000001F4, 0x00000000,
    0xF24AA280, 0x00000000, 0x00000003, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 teresa_move_event[165] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"me", 0x00200000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000020, 0x0006005B,
    (s32)evt_npc_set_color, (s32)"me", 0x000000FF, 0x000000FF,
    0x000000FF, 0x000000FF, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x00010003, 0x00000000,
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me",
    0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80,
    0xF24E3A80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24A5280, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24AA280, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24A5280, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000032, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24AA280, 0x00000000,
    0x00000003, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 teresa_find_event[60] = {
    0x0002005B, (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C81, 0xFE363C83, 0xFE363C80,
    0xF24C0A80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000032, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000000, 0x0000004B,
    0xF249DA80, 0x00000000, 0x00000000, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 teresa_lost_event[31] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 teresa_return_event[104] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me",
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x00200000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000020, 0x00020032, 0xFE363C80, 0x000000FF,
    0x00010005, 0x00000010, 0x0006005B, (s32)evt_npc_set_color,
    (s32)"me", 0x000000FF, 0x000000FF, 0x000000FF,
    0xFE363C80, 0x00020036, 0xFE363C80, 0x00000010,
    0x0001000A, 0x0000001E, 0x00000006, 0x0006005B,
    (s32)evt_npc_set_color, (s32)"me", 0x000000FF, 0x000000FF,
    0x000000FF, 0x00000000, 0x0001000A, 0x000001F4,
    0x0005005B, (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020032, 0xFE363C80, 0x00000000, 0x00010005,
    0x00000010, 0x00020035, 0xFE363C80, 0x00000010,
    0x0002001B, 0xFE363C80, 0x000000FF, 0x00020032,
    0xFE363C80, 0x000000FF, 0x00000021, 0x0006005B,
    (s32)evt_npc_set_color, (s32)"me", 0x000000FF, 0x000000FF,
    0x000000FF, 0xFE363C80, 0x0001000A, 0x0000001E,
    0x00000006, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"me", 0x00000020, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00200000, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 bubble_move_event[159] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_sub_random, 0x00000168,
    0xFE363C83, 0x00010003, 0x00000000, 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000014, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24A6680, 0x00000000,
    0x00000003, 0x0007005B, (s32)evt_npc_sound_data_set, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000014, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24A8E80, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000014, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24A6680, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000014, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24A8E80, 0x00000000, 0x00000003, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 bubble_find_event[63] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B,
    (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C80,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C81, 0xFE363C83, 0xFE363C80, 0xF24C0A80,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x00000014, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x00000000, 0x0000005A, 0xF24A7A80,
    0x00000000, 0x00000002, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 bubble_lost_event[34] = {
    0x0002005B, (s32)evt_npc_sound_data_reset, (s32)"me", 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 bubble_return_event[151] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me",
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0002005B,
    (s32)evt_npc_sound_data_reset, (s32)"me", 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000014,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A6680, 0x00000000, 0x00000003,
    0x0007005B, (s32)evt_npc_sound_data_set, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000014, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A8E80,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000014,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A6680, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000014, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A8E80,
    0x00000000, 0x00000003, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 hbom_move_event[41] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x00010003, 0x00000000,
    0x0001000A, 0x000001F4, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80,
    0xF24E3A80, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24C0A80, 0x0007005B,
    (s32)evt_npc_move_position, (s32)"me", 0xFE363C80, 0xFE363C82,
    0x000007D0, 0xF24ACA80, 0x00000004, 0x0001000A,
    0x000003E8, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 hbom_find_event[54] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0000012C, 0x00000000,
    0xF24ACA80, 0x00000000, 0x0008005B, (s32)evt_npc_homing_target,
    (s32)"me", (s32)"mario", 0x00000000, 0xF24B6A80,
    0xF24AA280, 0xF24B6A80, 0x00000004, 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 hbom_lost_event[31] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 hbom_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24AF280, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 zakowiz_init_event[38] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0006005B, (s32)evt_npc_slave_entry,
    (s32)"me", 0x00000000, (s32)"c_zako_m", 0xFE363C80,
    (s32)&enemy_slave_common_dead_event, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x20000000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, 0xFE363C80, 0x00000020, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"slave_0", (s32)"B_1A", 0x0003005B,
    (s32)evt_npc_set_width, (s32)"slave_0", 0x0000000A, 0x0003005B,
    (s32)evt_npc_set_height, (s32)"slave_0", 0x0000000A, 0x0004005B,
    (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000, 0x00000001,
    0x00000002, 0x00000001,
};
s32 zakowiz_move_event[70] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000080, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1", 0x0003005B,
    (s32)evt_sub_random, 0x00000168, 0xFE363C83, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x00010003, 0x00000000, 0x0001000A, 0x000001F4,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x000007D0, 0xF24B1A80,
    0x00000004, 0x0002005B, (s32)npc_check_wall_stop, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x00000001, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x00000021, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 zakowiz_find_event[159] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"A_1B", 0x0001000A,
    0x000001F4, 0x0000006E, 0x0001000A, 0x000002BC,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x00000070, 0x0005005B, (s32)evt_npc_set_rotate, (s32)"slave_0",
    0x00000000, 0x00000000, 0x00000000, 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x00020035,
    0xFE363C81, 0x00000019, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_0", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0",
    0x00000080, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"slave_0", (s32)"B_1A", 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0", 0x00000020,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x0000000C,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x000003E8, 0x00000000,
    0xF24BA680, 0x00000000, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"slave_0", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000F005B, (s32)evt_eff, (s32)"", (s32)"kemuri_test",
    0x00000010, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xF24A7E80, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0003005B, (s32)evt_npc_set_anim, (s32)"slave_0",
    (s32)"B_2", 0x0001000A, 0x0000012C, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_0", 0x00000000, 0xFFFFFC18,
    0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"me", 0x00200000, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 zakowiz_lost_event[18] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 zakowiz_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 zakowiz_blow_event[19] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0001005E,
    (s32)&enemy_common_blow_event, 0x00000002, 0x00000001,
};
s32 hannya_move_event[37] = {
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x00010003, 0x00000000, 0x0001000A, 0x000003E8,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x000007D0, 0xF24B1A80,
    0x00000004, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 hannya_find_event[86] = {
    0x0002005B, (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_HANNYA_MOVE1", 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0x00000000, 0x0005005B, (s32)evt_npc_get_home_position,
    (s32)"me", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C81, 0xFE363C83,
    0xFE363C80, 0xF24C0A80, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x0000000B, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"A_3", 0x0007005B,
    (s32)evt_npc_move_position, (s32)"me", 0xFE363C81, 0xFE363C83,
    0x00000000, 0xF24D4A80, 0x00000006, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x00000400,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000,
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 hannya_lost_event[23] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 hannya_return_event[29] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000003E8, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000000,
    (s32)"me", 0x00000002, 0x0005005B, (s32)evt_npc_get_home_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000BB8, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 mahoon_init_event[120] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0002005B, (s32)mahoon_get_groupname,
    0xFE363C8A, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000000, 0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"slave_0", (s32)"MAH_S_2A",
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0006005B, (s32)evt_npc_slave_entry,
    (s32)"me", 0x00000001, 0xFE363C8A, 0xFE363C80,
    (s32)&enemy_slave_common_dead_event, 0x0003005B, (s32)evt_npc_set_anim, (s32)"slave_1",
    (s32)"MAH_S_2A", 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_1",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0006005B,
    (s32)evt_npc_slave_entry, (s32)"me", 0x00000002, 0xFE363C8A,
    0xFE363C80, (s32)&enemy_slave_common_dead_event, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"slave_2", (s32)"MAH_S_2A", 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_2", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0006005B, (s32)evt_npc_slave_entry, (s32)"me", 0x00000003,
    0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"slave_3", (s32)"MAH_S_2A", 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_3", 0x00000000, 0xFFFFFC18,
    0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"me", 0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"slave_0", 0x00020000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_1", 0x00020000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_2",
    0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"slave_3", 0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000020, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_1", 0x00000020,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_2",
    0x00000020, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"slave_3", 0x00000020, 0x00000002, 0x00000001,
};
s32 mahoon_move_event[259] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0000006E, 0x00010005, 0x00000000,
    0x00010009, 0x00000001, 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"slave_0", 0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"slave_1", 0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"slave_2", 0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"slave_3", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x00020035, 0xFE363C81,
    0x00000005, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24A1680, 0x00020035,
    0xFE363C81, 0x00000005, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x00020035, 0xFE363C81, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_2", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24A1680, 0x00020035, 0xFE363C81, 0x0000001E,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_3", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000006, 0x00000070,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x00010003, 0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x0001000A, 0x000001F4, 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000014, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24A6680, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000014, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24A8E80, 0x00000000, 0x00000003, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x0006005B,
    (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000014, 0x000001F4, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000032, 0xF24A6680, 0x00000000,
    0x00000003, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000014, 0x000001F4,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000032,
    0xF24A8E80, 0x00000000, 0x00000003, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 mahoon_dead_event[24] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"slave_1", 0x00000080, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_2", 0x00000080, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_3", 0x00000080,
    0x0001005E, (s32)&enemy_common_dead_event, 0x00000002, 0x00000001,
};
s32 mahoon_find_event[177] = {
    0x0000006E, 0x00010005, 0x00000000, 0x00010009,
    0x00000001, 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_0",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_1",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_2",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_3",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x00020035, 0xFE363C81, 0x00000005,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24A1680, 0x00020035, 0xFE363C81,
    0x00000005, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x00020035,
    0xFE363C81, 0x0000001E, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_2", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24A1680,
    0x00020035, 0xFE363C81, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_3", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000006, 0x00000070, 0x0002005B,
    (s32)evt_npc_set_run_anim, (s32)"me", 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C80,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x0005005B, (s32)evt_npc_get_home_position,
    (s32)"me", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C81, 0xFE363C83,
    0xFE363C80, 0xF24C0A80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x00000014,
    0x000001F4, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C81, 0xFE363C82, 0xFE363C83, 0x00000000,
    0x00000087, 0xF24A7A80, 0x00000000, 0x00000000,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 mahoon_lost_event[142] = {
    0x0000006E, 0x00010005, 0x00000000, 0x00010009,
    0x00000001, 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_0",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_1",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_2",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_3",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x00020035, 0xFE363C81, 0x00000005,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24A1680, 0x00020035, 0xFE363C81,
    0x00000005, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x00020035,
    0xFE363C81, 0x0000001E, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_2", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24A1680,
    0x00020035, 0xFE363C81, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_3", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000006, 0x00000070, 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 mahoon_return_event[251] = {
    0x0000006E, 0x00010005, 0x00000000, 0x00010009,
    0x00000001, 0x0003005B, (s32)evt_npc_get_dir, (s32)"me",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_0",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_1",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_2",
    0xFE363C83, 0x0003005B, (s32)evt_npc_set_ry, (s32)"slave_3",
    0xFE363C83, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ADE80, 0x00020035, 0xFE363C81, 0x00000005,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24A1680, 0x00020035, 0xFE363C81,
    0x00000005, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80,
    0xFE363C82, 0xFE363C83, 0xF24ADE80, 0x00020035,
    0xFE363C81, 0x0000001E, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_2", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24A1680,
    0x00020035, 0xFE363C81, 0x0000001E, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_3", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000006, 0x00000070, 0x0002005B,
    (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me", 0x0002005B,
    (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000014,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A6680, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000014, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A8E80,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000014,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A6680, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000014, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A8E80,
    0x00000000, 0x00000003, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 kamec_init_event[54] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x0002005B, (s32)mahoon_get_groupname,
    0xFE363C8A, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000000, 0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
    0x20000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x00000020, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, 0xFE363C80, 0x00020000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"slave_0", (s32)"KMK_Z_1", 0x0003005B,
    (s32)evt_npc_set_width, (s32)"slave_0", 0x0000000A, 0x0003005B,
    (s32)evt_npc_set_height, (s32)"slave_0", 0x0000000A, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000080,
    0x00000002, 0x00000001,
};
s32 kamec_move_event[177] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0003005B, (s32)evt_sub_random, 0x00000168,
    0xFE363C83, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"slave_0", 0x00000080, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000020, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"KMK_S_2", 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x00010003, 0x00000000, 0x0002005B, (s32)evt_npc_set_stay_anim,
    (s32)"me", 0x0001000A, 0x000001F4, 0x0002005B,
    (s32)evt_npc_set_walk_anim, (s32)"me", 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000032, 0xF24A6680,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000032, 0xF24A8E80, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x00000032, 0xF24A6680,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000032, 0xF24A8E80, 0x00000000, 0x00000003,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000400, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 kamec_find_event[150] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"KMK_A_3A", 0x0001000A,
    0x000002BC, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KMK_A_3B", 0x0001000A, 0x00000096, 0x0000006E,
    0x0001000A, 0x000003E8, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"KMK_S_2", 0x00000070, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"slave_0", 0x00000000, 0x00000000,
    0x00000000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24ACA80, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C83, 0xFE363C84,
    0xFE363C85, 0x00020035, 0xFE363C84, 0x00000014,
    0x0008005B, (s32)kamec_fire_magic, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0xFE363C84, 0xFE363C85,
    0xFE363C8A, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C81, 0x00000014, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"slave_0", 0x00000020, 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x0000000C, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"slave_0", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000003E8, 0x00000000, 0xF24A7A80,
    0x00000000, 0x00000003, 0x0002005B, (s32)kamec_fire_magic_delete,
    0xFE363C8A, 0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0",
    0x00000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"slave_0", 0x00000020, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_0", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00200000, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 kamec_lost_event[31] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x000C005B,
    (s32)evt_eff_fukidashi, 0x00000001, 0x00000000, (s32)"me",
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000001E,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 kamec_return_event[150] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_set_walk_anim, (s32)"me",
    0x0002005B, (s32)evt_set_dir_to_home, (s32)"me", 0x0003005B,
    (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A6680,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A8E80, 0x00000000, 0x00000003,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ADE80,
    0x0006005B, (s32)evt_fbat_trans_floor_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000032, 0x000001F4, 0x000A005B,
    (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000001F4, 0x00000000, 0xF24A6680,
    0x00000000, 0x00000003, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24ADE80, 0x0006005B, (s32)evt_fbat_trans_floor_position,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000032,
    0x000001F4, 0x000A005B, (s32)evt_npc_glide_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000001F4,
    0x00000000, 0xF24A8E80, 0x00000000, 0x00000003,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000400, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 kamec_blow_event[19] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0001005E,
    (s32)&enemy_common_blow_event, 0x00000002, 0x00000001,
};
s32 kamec2_init_event[54] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100418, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x0002005B, (s32)mahoon_get_groupname,
    0xFE363C8A, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000000, 0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
    0x20000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x00000020, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, 0xFE363C80, 0x00020000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"slave_0", (s32)"KMK_Z_1", 0x0003005B,
    (s32)evt_npc_set_width, (s32)"slave_0", 0x0000000A, 0x0003005B,
    (s32)evt_npc_set_height, (s32)"slave_0", 0x0000000A, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000080,
    0x00000002, 0x00000001,
};
s32 kamec2_move_event[58] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0003005B, (s32)evt_sub_random,
    0x00000168, 0xFE363C83, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000080, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000020,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"KMK_S_1",
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00200000, 0x00010003, 0x00000000, 0x0001000A,
    0x000001F4, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_get_loiter_dir, 0xFE363C83, 0xF24C5A80, 0xF24E3A80,
    0x0005005B, (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82,
    0xFE363C83, 0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position,
    (s32)"me", 0xFE363C80, 0xFE363C82, 0x000007D0,
    0xF24B1A80, 0x00000004, 0x00010004, 0x00000000,
    0x00000002, 0x00000001,
};
s32 kamec2_find_event[153] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"KMK_A_2A", 0x0001000A,
    0x000002BC, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"KMK_A_2B", 0x0001000A, 0x00000096, 0x0000006E,
    0x0001000A, 0x000003E8, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"KMK_S_1", 0x00000070, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"slave_0", 0x00000000, 0x00000000,
    0x00000000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C81, 0x0000001E, 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ACA80,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_mario_get_pos,
    0x00000000, 0xFE363C83, 0xFE363C84, 0xFE363C85,
    0x00020035, 0xFE363C84, 0x00000014, 0x0008005B,
    (s32)kamec_fire_magic, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xFE363C83, 0xFE363C84, 0xFE363C85, 0xFE363C8A,
    0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00020035, 0xFE363C81,
    0x00000014, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"slave_0", 0x00000020, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"slave_0", 0x0000000C, 0x000A005B, (s32)evt_npc_glide_position,
    (s32)"slave_0", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000003E8, 0x00000000, 0xF24A7A80, 0x00000000,
    0x00000003, 0x0002005B, (s32)kamec_fire_magic_delete, 0xFE363C8A,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 kamec2_lost_event[31] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me",
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x0001000A, 0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt,
    (s32)"me", 0x00000002, 0x00000001,
};
s32 kamec2_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 kamec2_blow_event[19] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0001005E,
    (s32)&enemy_common_blow_event, 0x00000002, 0x00000001,
};
s32 hbross_init_event[36] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0002005B, (s32)mahoon_get_groupname,
    0xFE363C8A, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000000, 0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
    0x20000000, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x00000020, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"slave_0", (s32)"BRO_A_1B", 0x0003005B, (s32)evt_npc_set_width,
    (s32)"slave_0", 0x0000000A, 0x0003005B, (s32)evt_npc_set_height,
    (s32)"slave_0", 0x0000000A, 0x00000002, 0x00000001,
};
s32 hbross_move_event[57] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"slave_0", 0x00000080, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"BRO_S_1", 0x0003005B,
    (s32)evt_sub_random, 0x00000168, 0xFE363C83, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0003005B, (s32)evt_sub_random, 0x00000168, 0xFE363C83,
    0x00010003, 0x00000000, 0x0001000A, 0x000001F4,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_get_loiter_dir,
    0xFE363C83, 0xF24C5A80, 0xF24E3A80, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0xF24C0A80, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x000007D0, 0xF24B1A80,
    0x00000004, 0x00010004, 0x00000000, 0x00000002,
    0x00000001,
};
s32 hbross_find_event[141] = {
    0x000C005B, (s32)evt_eff_fukidashi, 0x00000001, 0x00000000,
    (s32)"me", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000001E, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0000006E,
    0x00010003, 0x00000000, 0x0005005B, (s32)evt_npc_add_rotate,
    (s32)"slave_0", 0x00000000, 0x00000000, 0x0000001E,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00000070, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"BRO_A_1A", 0x0001000A, 0x00000190, 0x0000006E,
    0x0001000A, 0x000001F4, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"BRO_S_1", 0x00000070, 0x0005005B,
    (s32)evt_npc_set_rotate, (s32)"slave_0", 0x00000000, 0x00000000,
    0x00000000, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C81, 0x00000019, 0x0003005B, (s32)evt_npc_get_dir,
    (s32)"me", 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24ACA80,
    0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"slave_0", 0x00000080, 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"slave_0", 0x00000020, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"slave_0", 0x0000000C, 0x0009005B, (s32)evt_npc_jump_position,
    (s32)"slave_0", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000003E8, 0x00000000, 0xF24BA680, 0x00000000,
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 hbross_lost_event[18] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0001000A,
    0x000003E8, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 hbross_return_event[37] = {
    0x0002005B, (s32)evt_npc_set_stay_anim, (s32)"me", 0x0001000A,
    0x000001F4, 0x0002005B, (s32)evt_set_dir_to_home, (s32)"me",
    0x0003005B, (s32)evt_npc_get_dir, (s32)"me", 0xFE363C83,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0xF24C0A80,
    0x0007005B, (s32)evt_npc_move_position, (s32)"me", 0xFE363C80,
    0xFE363C82, 0x00000000, 0xF24B1A80, 0x00000004,
    0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002,
    0x00000001,
};
s32 hbross_blow_event[19] = {
    0x0003005B, (s32)evt_npc_set_attack_mode, (s32)"slave_0", 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x0005005B, (s32)evt_npc_set_position, (s32)"slave_0",
    0x00000000, 0xFFFFFC18, 0x00000000, 0x0001005E,
    (s32)&enemy_common_blow_event, 0x00000002, 0x00000001,
};

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
        *(s32*)((u8*)eff->userdata + 0x30) = 1;
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

s32 wanwan_init_event[64] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0002005B,
    (s32)mahoon_get_groupname, 0xFE363C8A, 0x0006005B, (s32)evt_npc_slave_entry,
    (s32)"me", 0x00000000, 0xFE363C8A, 0xFE363C80,
    (s32)&enemy_slave_common_dead_event, 0x0003005B, (s32)evt_npc_set_anim, 0xFE363C80,
    (s32)"WAN_S_1B", 0x0001005E, (s32)&wanwan_init_event_sub, 0x0006005B,
    (s32)evt_npc_slave_entry, (s32)"me", 0x00000001, 0xFE363C8A,
    0xFE363C80, (s32)&enemy_slave_common_dead_event, 0x0003005B, (s32)evt_npc_set_anim,
    0xFE363C80, (s32)"WAN_S_1B", 0x0001005E, (s32)&wanwan_init_event_sub,
    0x0006005B, (s32)evt_npc_slave_entry, (s32)"me", 0x00000002,
    0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event, 0x0003005B,
    (s32)evt_npc_set_anim, 0xFE363C80, (s32)"WAN_S_1B", 0x0001005E,
    (s32)&wanwan_init_event_sub, 0x0006005B, (s32)evt_npc_slave_entry, (s32)"me",
    0x00000003, 0xFE363C8A, 0xFE363C80, (s32)&enemy_slave_common_dead_event,
    0x0003005B, (s32)evt_npc_set_anim, 0xFE363C80, (s32)"WAN_Z_2",
    0x0001005E, (s32)&wanwan_init_event_sub, 0x00000002, 0x00000001,
};
s32 wanwan_init_event_sub[64] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
    0x00000020, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    0xFE363C80, 0x00020000, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, 0xFE363C80, 0x00000010, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, 0xFE363C80, 0x00000200,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, 0xFE363C80,
    0x00000400, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    0xFE363C80, 0x00000800, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, 0xFE363C80, 0x01000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80, 0x00200000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
    0x00000040, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    0xFE363C80, 0x00040000, 0x0005005B, (s32)evt_npc_get_home_position,
    (s32)"me", 0xFE363C81, 0xFE363C82, 0xFE363C83,
    0x0005005B, (s32)evt_npc_set_position, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xFE363C83, 0x00000002, 0x00000001,
};
s32 wanwan_move_event[151] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000400, 0x0003005B,
    (s32)evt_npc_set_attack_mode, (s32)"me", 0x00000000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"WAN_S_1A", 0x0000006E,
    0x00010005, 0x00000000, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x0002001D, 0xFE363C80,
    0xFE363C8A, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0x0000010E, 0x00000020, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0x0000005A, 0x00000021, 0x00010009,
    0x00000001, 0x00000006, 0x00000070, 0x0000006E,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00200000, 0x0001000A, 0x000005DC, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00200000,
    0x00000070, 0x0000006E, 0x00010005, 0x00000000,
    0x0001005B, (s32)chain_main, 0x00010009, 0x00000001,
    0x00000006, 0x00000070, 0x0000006E, 0x00010005,
    0x00000000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"WAN_A_1", 0x0001000A, 0x000003E8, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"WAN_S_1A", 0x0001000A,
    0x000001F4, 0x00000006, 0x00000070, 0x00010003,
    0x00000000, 0x0005005B, (s32)evt_npc_get_home_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0003005B,
    (s32)evt_sub_random, 0x00000168, 0xFE363C83, 0x0005005B,
    (s32)evt_npc_add_dirdist, 0xFE363C80, 0xFE363C82, 0xFE363C83,
    0x00000032, 0x0003005B, (s32)evt_sub_random, 0x0000000F,
    0xFE363C84, 0x00020035, 0xFE363C84, 0x0000000F,
    0x00020032, 0xFE363C85, 0xFE363C84, 0x00020032,
    0xFE363C86, 0xFE363C84, 0x00020037, 0xFE363C86,
    0x00000011, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_WAN_MOVE1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0xFE363C86, 0x00000050,
    0xFE363C85, 0x00000001, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_FLD_ENM_WAN_LANDING1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00010009, 0x00000001, 0x00010004,
    0x00000000, 0x00000002, 0x00000001,
};
s32 wanwan_find_event[90] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00200000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"WAN_A_1", 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x0003005B, (s32)evt_npc_set_attack_mode,
    (s32)"me", 0x0000000C, 0x0000006E, 0x00010005,
    0x00000000, 0x0001005B, (s32)chain_main, 0x00010009,
    0x00000001, 0x00000006, 0x00000070, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_WAN_MOVE2",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_FLD_ENM_WAN_ATTACK2", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_home_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C83, 0x0005005B, (s32)evt_npc_add_dirdist,
    0xFE363C80, 0xFE363C82, 0xFE363C83, 0x00000064,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00000258,
    0xF24A7A80, 0x00000000, 0x00000006, 0x0001000A,
    0x000005DC, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};

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

s32 kuriboo2D_init_event[22] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x08000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000000, (s32)"me",
    0x00000004, 0x0002005B, (s32)limitSpd, 0xF250BA80,
    0x00000002, 0x00000001,
};
s32 kuriboo2D_move_event[72] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x00010003, 0x00000000, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00001000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xF1194D80,
    0xF1194D80, 0x0002005B, (s32)camGetPos, 0xFE363C81,
    0x00020036, 0xFE363C80, 0xFE363C81, 0x0002001C,
    0xFE363C80, 0x000000FA, 0x00010004, 0x00000001,
    0x00000021, 0x00010009, 0x00000001, 0x00010004,
    0x00000000, 0x00010003, 0x00000001, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"W_1", 0x00010003,
    0x00000002, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_facedirection_add, (s32)"me", 0xFE363C80, 0x000003E8,
    0x0002005B, (s32)getSpdWalk, 0xFE363C83, 0x0007005B,
    (s32)evt_npc_move_position, (s32)"me", 0xFE363C80, 0xFE363C82,
    0x00000000, 0xFE363C83, 0x00000004, 0x0002005B,
    (s32)evt_npc_reverse_ry, (s32)"me", 0x00010009, 0x00000001,
    0x00010004, 0x00000002, 0x00000002, 0x00000001,
};
s32 kuriboo2D_find_event[70] = {
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0002001D, 0xFE363C81,
    0xFFFFFF9C, 0x000C005B, (s32)evt_eff_fukidashi, 0x00000001,
    0x00000000, (s32)"me", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000001E, 0x00000021, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"R_1", 0x0004005B,
    (s32)evt_get_target_dir, (s32)"me", (s32)"mario", 0xFE363C80,
    0x0003005B, (s32)evt_npc_set_ry, (s32)"me", 0xFE363C80,
    0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me", 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_npc_facedirection_add, (s32)"me",
    0xFE363C80, 0x000003E8, 0x0002005B, (s32)getSpdRun,
    0xFE363C83, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000FA0, 0xFE363C83,
    0x00000002, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"S_1", 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 zako2D_init_event[27] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x08000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000001, (s32)"me",
    0x00200000, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000000,
    (s32)"me", 0x00000004, 0x0002005B, (s32)limitSpd,
    0xF250BA80, 0x00000002, 0x00000001,
};
s32 zako2D_move_event[244] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x00010003, 0x00000000, 0x0004005B,
    (s32)evt_npc_status_onoff, 0x00000001, (s32)"me", 0x00001000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xF1194D80, 0xF1194D80, 0x0002005B, (s32)camGetPos,
    0xFE363C81, 0x00020036, 0xFE363C80, 0xFE363C81,
    0x0002001C, 0xFE363C80, 0x000000FA, 0x00010004,
    0x00000001, 0x00000021, 0x00010009, 0x00000001,
    0x00010004, 0x00000000, 0x00010003, 0x00000001,
    0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000,
    0x00000000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"Z_1", 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00000010, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00020000,
    0x00010005, 0x00000023, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00001000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00020035, 0xFE363C81, 0x00000001,
    0x0005005B, (s32)evt_npc_set_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00010009, 0x00000001,
    0x00000006, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00000010, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00020000, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"W_1", 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_npc_facedirection_add, (s32)"me",
    0xFE363C80, 0x00000032, 0x0009005B, (s32)evt_npc_jump_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x000001F4, 0xF24C0A80, 0xF24AF280, 0x00000001,
    0x00000021, 0x0004005B, (s32)evt_npc_get_unitwork, (s32)"me",
    0x00000000, 0xFE363C80, 0x00020018, 0xFE363C80,
    0x00000002, 0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me",
    0x00000000, 0x00000000, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"S_1", 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x00020000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0009005B, (s32)evt_npc_jump_position, (s32)"me",
    0xFE363C80, 0xFFFFFF9C, 0xFE363C82, 0x00000000,
    0xF24C0A80, 0xF24A7A80, 0x00000001, 0x00000021,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000000, (s32)"me",
    0x00001000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"W_1", 0x00010003, 0x0000000A, 0x0001006F,
    0xFE363C8F, 0x00010005, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_npc_facedirection_add, (s32)"me",
    0xFE363C80, 0x000003E8, 0x0002005B, (s32)getSpdWalk,
    0xFE363C83, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000000, 0xFE363C83,
    0x00000004, 0x0002005B, (s32)evt_npc_reverse_ry, (s32)"me",
    0x00010009, 0x00000001, 0x00000006, 0x00000070,
    0x00010003, 0x0000000B, 0x0002005B, (s32)cliffCheck,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x0001005F, 0xFE363C8F, 0x0002005B, (s32)evt_npc_reverse_ry,
    (s32)"me", 0x00010009, 0x00000001, 0x00010004,
    0x0000000A, 0x00000021, 0x00010009, 0x00000001,
    0x00010004, 0x0000000B, 0x00000002, 0x00000001,
};
s32 zako2D_find_event[74] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00000400, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0002001D,
    0xFE363C81, 0xFFFFFF9C, 0x000C005B, (s32)evt_eff_fukidashi,
    0x00000001, 0x00000000, (s32)"me", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000001E, 0x00000021,
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"A_1",
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_facedirection_add, (s32)"me", 0xFE363C80, 0x000003E8,
    0x0002005B, (s32)getSpdRun, 0xFE363C83, 0x0007005B,
    (s32)evt_npc_move_position, (s32)"me", 0xFE363C80, 0xFE363C82,
    0x00000BB8, 0xFE363C83, 0x00000002, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1", 0x00010003,
    0x00000063, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 dokan2D_init_event[19] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x000000A0,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000000, (s32)"me",
    0x00000004, 0x00000002, 0x00000001,
};
s32 dokan2D_regl_event[48] = {
    0x00010003, 0x00000000, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xF1194D80, 0xF1194D80,
    0x0002005B, (s32)camGetPos, 0xFE363C81, 0x00020036,
    0xFE363C80, 0xFE363C81, 0x0002001C, 0xFE363C80,
    0x000000FA, 0x00010004, 0x00000001, 0x00000021,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00010003, 0x00000001, 0x0002005B, (s32)dokanCheck,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000000,
    0x00010004, 0x00000063, 0x00000021, 0x0001005B,
    (s32)zakoEntryDokan, 0x00010009, 0x00000078, 0x00010004,
    0x00000001, 0x00010003, 0x00000063, 0x0002005B,
    (s32)evt_npc_delete, (s32)"me", 0x00000002, 0x00000001,
};
s32 fall2D_init_event[19] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me", 0x000000A0,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000000, (s32)"me",
    0x00000004, 0x00000002, 0x00000001,
};
s32 fall2D_regl_event[34] = {
    0x00010003, 0x00000000, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xF1194D80, 0xF1194D80,
    0x0002005B, (s32)camGetPos, 0xFE363C81, 0x00020036,
    0xFE363C80, 0xFE363C81, 0x0002001C, 0xFE363C80,
    0x000000FA, 0x00010004, 0x00000001, 0x00000021,
    0x00010009, 0x00000001, 0x00010004, 0x00000000,
    0x00010003, 0x00000001, 0x0001005B, (s32)zakoEntryFall,
    0x00010009, 0x00000078, 0x00010004, 0x00000001,
    0x00000002, 0x00000001,
};
s32 zakoM2D_init_event[37] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x08000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000000, (s32)"me",
    0x00000004, 0x0002005B, (s32)limitSpd, 0xF250BA80,
    0x0006005B, (s32)evt_npc_slave_entry, (s32)"me", 0x00000000,
    (s32)"c_zako_km", 0xFE363C80, (s32)&enemy_slave_common_dead_event, 0x0005005B,
    (s32)evt_npc_set_position, (s32)"slave_0", 0x00000000, 0xFFFFFC18,
    0x00000000, 0x0001005B, (s32)set_bottle_flag_init, 0x00000002,
    0x00000001,
};
s32 zakoM2D_move_event[95] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x00010003, 0x00000002, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00001000, 0x0004005B,
    (s32)evt_npc_status_check, (s32)"me", 0x00000004, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x00000000, 0x00010004,
    0x00000003, 0x00000021, 0x00010009, 0x00000001,
    0x00010004, 0x00000002, 0x00010003, 0x00000003,
    0x0000006E, 0x00010005, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000064, 0x00000000,
    0x00000000, 0x00010009, 0x00000001, 0x00000006,
    0x00000070, 0x00010003, 0x00000000, 0x0004005B,
    (s32)evt_npc_status_onoff, 0x00000001, (s32)"me", 0x00001000,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xF1194D80, 0xF1194D80, 0x0002005B, (s32)camGetPos,
    0xFE363C81, 0x00020036, 0xFE363C80, 0xFE363C81,
    0x0002001C, 0xFE363C80, 0x000000FA, 0x00010004,
    0x00000001, 0x00000021, 0x00010009, 0x00000001,
    0x00010004, 0x00000000, 0x00010003, 0x00000001,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x00010009, 0x00000001, 0x00010004,
    0x00000001, 0x00000002, 0x00000001,
};
s32 zakoM2D_find_event[179] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"slave_0", (s32)"B_1",
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_npc_facedirection_add,
    (s32)"me", 0xFE363C80, 0x0000000A, 0x00020035,
    0xFE363C81, 0x00000014, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_0", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"slave_0",
    0x00000020, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000,
    (s32)"slave_0", 0x00000080, 0x0002001D, 0xFE363C8A,
    0x000000C8, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_npc_facedirection_add, (s32)"me", 0xFE363C80, 0x00000064,
    0x0009005B, (s32)evt_npc_jump_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0xF24F2A80,
    0xF24F2A80, 0x00000000, 0x00000020, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_npc_facedirection_add, (s32)"me",
    0xFE363C80, 0xFE363C8A, 0x0002003C, 0xFE363C8A,
    0xF24A8080, 0x0009005B, (s32)evt_npc_jump_position, (s32)"slave_0",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0xFE363C8A, 0xF24F2A80, 0x00000000, 0x00000021,
    0x0005005B, (s32)evt_npc_get_position, (s32)"slave_0", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_CONDITION_FIRE1", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_ENM_3RD2_CRASH1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00020036, 0xFE363C81, 0x00000005, 0x00020035,
    0xFE363C82, 0x000000C8, 0x000F005B, (s32)evt_eff,
    (s32)"", (s32)"kemuri_test", 0x00000010, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0xF24A7E80, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00010005, 0x0000000C,
    0x000F005B, (s32)evt_eff64, (s32)"", (s32)"glass_n64",
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0xF24A7E80, 0x0000001E, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000006, 0x0000006B, 0x0005005B, (s32)unk_800ece0c,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000003C,
    0x0000006D, 0x0003005B, (s32)evt_npc_set_anim, (s32)"slave_0",
    (s32)"B_2", 0x00010009, 0x0000000A, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0", 0x00000080,
    0x00010009, 0x0000003C, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"slave_0", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000020, 0x00000002, 0x00000001,
};
s32 unk_evt_803497e8[88] = {
    0x0000006E, 0x00010005, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0007005B, (s32)evt_npc_move_position, (s32)"me",
    0xFE363C80, 0xFE363C82, 0x00000064, 0x00000000,
    0x00000000, 0x00010009, 0x00000001, 0x00000006,
    0x00000070, 0x0004005B, (s32)evt_get_target_dir, (s32)"me",
    (s32)"mario", 0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry,
    (s32)"me", 0xFE363C80, 0x0005005B, (s32)evt_mario_get_pos,
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (s32)evt_npc_get_position, (s32)"me", 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x0002003B, 0xFE363C8A,
    0xFE363C80, 0x0002001A, 0xFE363C8A, 0x00000000,
    0x0002003C, 0xFE363C8A, 0xF24A7680, 0x00000021,
    0x0002001A, 0xFE363C8A, 0x0000001E, 0x00020034,
    0xFE363C8A, 0xF24AF280, 0x00000021, 0x0003005B,
    (s32)evt_npc_set_anim, (s32)"me", (s32)"A_1", 0x00010009,
    0x00000040, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"A_2", 0x00010009, 0x00000018, 0x0000006E,
    0x00010009, 0x00000018, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"S_1", 0x00000070, 0x0001005C,
    (s32)&zakoM2D_find_event, 0x00010009, 0x00000064, 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 gesso2D_init_event[34] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"me", 0x08000000, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me", 0x00000400,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000000, (s32)"me",
    0x00020000, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x10000000, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00200000, 0x0004005B,
    (s32)evt_npc_status_onoff, 0x00000000, (s32)"me", 0x00000004,
    0x00000002, 0x00000001,
};
s32 gesso2D_move_event[262] = {
    0x0003005B, (s32)evt_npc_set_anim, (s32)"me", (s32)"S_1",
    0x00010003, 0x00000000, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000001, (s32)"me", 0x00001000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xF1194D80,
    0xF1194D80, 0x0002005B, (s32)camGetPos, 0xFE363C81,
    0x00020036, 0xFE363C80, 0xFE363C81, 0x0002001C,
    0xFE363C80, 0x000000FA, 0x00010004, 0x00000001,
    0x00000021, 0x00010009, 0x00000001, 0x00010004,
    0x00000000, 0x0003005B, (s32)evt_npc_set_anim, (s32)"me",
    (s32)"S_1", 0x00010003, 0x00000001, 0x00020034,
    0xFE363C81, 0xF24A7A80, 0x00020034, 0xFE363C8D,
    0xF24A7A80, 0x00010005, 0x00000000, 0x0005005B,
    (s32)evt_npc_get_position, (s32)"me", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x00010009,
    0x00000001, 0x00020036, 0xFE363C80, 0xFE363C8A,
    0x0002001D, 0xFE363C80, 0x0000012C, 0x00010004,
    0x00000005, 0x00000021, 0x0002001C, 0xFE363C80,
    0xFFFFFED4, 0x00010004, 0x00000005, 0x00000021,
    0x0002001A, 0xFE363C81, 0xFE363C8B, 0x00000007,
    0x00000021, 0x0002001A, 0xFE363C81, 0x0000005A,
    0x00000007, 0x00000021, 0x00010003, 0x00000005,
    0x0002005B, (s32)gesso_ground_check, 0xFE363C8F, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020034, 0xFE363C8E, 0xFE363C81, 0x0002003B,
    0xFE363C8E, 0xFE363C8D, 0x0002003A, 0xFE363C8D,
    0xF24A7AE6, 0x0002001D, 0xFE363C8D, 0xF24A7E80,
    0x00020034, 0xFE363C8D, 0xF24A7E80, 0x00000021,
    0x0005005B, (s32)evt_npc_set_position, (s32)"me", 0xFE363C80,
    0xFE363C8E, 0xFE363C82, 0x00000021, 0x00000006,
    0x0004005B, (s32)evt_get_target_dir, (s32)"me", (s32)"mario",
    0xFE363C80, 0x0003005B, (s32)evt_npc_set_ry, (s32)"me",
    0xFE363C80, 0x0005005B, (s32)evt_npc_get_position, (s32)"me",
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x0004005B,
    (s32)evt_npc_facedirection_add, (s32)"me", 0xFE363C8A, 0x00000064,
    0x00020035, 0xFE363C8B, 0x00000032, 0x0002001B,
    0xFE363C8B, 0x0000015E, 0x00020032, 0xFE363C8B,
    0x0000015E, 0x00000021, 0x00010005, 0x0000003C,
    0x0002005B, (s32)gesso_ground_check, 0xFE363C8F, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0005005B, (s32)evt_npc_get_position,
    (s32)"me", 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020034, 0xFE363C8E, 0xFE363C81, 0x0002003B,
    0xFE363C8E, 0xFE363C8D, 0x0002003A, 0xFE363C8D,
    0xF24A7AE6, 0x0002001D, 0xFE363C8D, 0xF24A7E80,
    0x00020034, 0xFE363C8D, 0xF24A7E80, 0x00000021,
    0x0005005B, (s32)evt_npc_set_position, (s32)"me", 0xFE363C80,
    0xFE363C8E, 0xFE363C82, 0x00000021, 0x00010009,
    0x00000001, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"A_1", 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_KUPPA_GESSO_MOVE1", 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00000000, 0x0004005B, (s32)evt_npc_set_unitwork, (s32)"me",
    0x00000000, 0x00000000, 0x0001006F, 0xFE363C8F,
    0x000A005B, (s32)evt_npc_glide_position, (s32)"me", 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x000007D0, 0xF24B4280,
    0x0000000A, 0x00000004, 0x00000000, 0x0004005B,
    (s32)evt_npc_set_unitwork, (s32)"me", 0x00000000, 0x00000001,
    0x00000070, 0x00010005, 0x00000000, 0x0004005B,
    (s32)evt_npc_get_unitwork, (s32)"me", 0x00000000, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000007,
    0x00000021, 0x0002005B, (s32)upperCheck, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x00000001, 0x0001005F,
    0xFE363C8F, 0x00000007, 0x00000021, 0x00010009,
    0x00000001, 0x00000006, 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"me", (s32)"S_1", 0x00010004, 0x00000001,
    0x00000002, 0x00000001,
};
s32 enemy_event_dead_event[10] = {
    0x00020032, 0xFE363C8A, (s32)"me", 0x0001005E,
    (s32)&enemy_event_dead_event_sub, 0x0002005B, (s32)evt_npc_return_interrupt, (s32)"me",
    0x00000002, 0x00000001,
};
s32 enemy_slave_common_dead_event[8] = {
    0x0002005B, (s32)evt_npc_delete, (s32)"me", 0x0002005B,
    (s32)evt_npc_return_interrupt, (s32)"me", 0x00000002, 0x00000001,
};
s32 enemy_common_flag_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x10120418, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 testenemynpc_init_event[4] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x00000002, 0x00000001,
};
s32 npc_urouro_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000600, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 kuriboo_init_event[4] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x00000002, 0x00000001,
};
s32 patakuri_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 patapata_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00110018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 patamet_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 chorobon_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00120018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 pansy_init_event[4] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x00000002, 0x00000001,
};
s32 twinkling_pansy_init_event[4] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x00000002, 0x00000001,
};
s32 karon_dead_event[9] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0001005E, (s32)&enemy_common_dead_event, 0x00000002,
    0x00000001,
};
s32 sambo_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00000008, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"me", 0x00100410, 0x00000002, 0x00000001,
};
s32 togedaruma_init_event[4] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x00000002, 0x00000001,
};
s32 piders_init_event[9] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000000, (s32)"me", 0x00020000, 0x00000002,
    0x00000001,
};
s32 basabasa_dead_event[4] = {
    0x0001005E, (s32)&enemy_common_dead_event, 0x00000002, 0x00000001,
};
s32 basabasa2_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 basabasa2_dead_event[4] = {
    0x0001005E, (s32)&enemy_common_dead_event, 0x00000002, 0x00000001,
};
s32 teresa_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 teresa_dead_event[4] = {
    0x0001005E, (s32)&enemy_common_dead_event, 0x00000002, 0x00000001,
};
s32 bubble_init_event[12] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"me",
    0x00100018, 0x0004005B, (s32)evt_npc_status_onoff, 0x00000001,
    (s32)"me", 0x00000002, 0x00000002, 0x00000001,
};
s32 hbom_init_event[4] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x00000002, 0x00000001,
};
s32 zakowiz_dead_event[9] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0001005E, (s32)&enemy_common_dead_event, 0x00000002,
    0x00000001,
};
s32 hannya_init_event[9] = {
    0x0001005E, (s32)&enemy_common_flag_init_event, 0x0004005B, (s32)evt_npc_status_onoff,
    0x00000000, (s32)"me", 0x00000002, 0x00000002,
    0x00000001,
};
s32 kamec_dead_event[9] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0001005E, (s32)&enemy_common_dead_event, 0x00000002,
    0x00000001,
};
s32 kamec2_dead_event[9] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0001005E, (s32)&enemy_common_dead_event, 0x00000002,
    0x00000001,
};
s32 hbross_dead_event[9] = {
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"slave_0",
    0x00000080, 0x0001005E, (s32)&enemy_common_dead_event, 0x00000002,
    0x00000001,
};
s32 zakoM2D_dead_event[9] = {
    0x0004005B, (s32)evt_npc_status_onoff, 0x00000000, (s32)"slave_0",
    0x00000004, 0x0001005E, (s32)&enemy_2d_common_dead_event, 0x00000002,
    0x00000001,
};

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
        *(f32*)((u8*)eff->userdata + 0x78) = t;
        *(f32*)((u8*)eff->userdata + 0x7C) = t;
        *(u8*)((u8*)eff->userdata + 0x10) =
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
