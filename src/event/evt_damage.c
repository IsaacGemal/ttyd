#include "evt/evt_damage.h"

#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "system.h"
#include "evt/evt_snd.h"

// clang-format off
USER_FUNC(camdir) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 yaw = cam->viewYaw;
    HitEntry* hit = (HitEntry*)marioGetPtr()->hitobjDamageReturn;
    s32 dir = 1;

    if (hit != NULL) {
        Vec* pos = hitGetDamageReturnPos(hit);
        f32 angle = reviseAngle(yaw - angleABf((f32)event->lwData[0], (f32)event->lwData[2], pos->x, pos->z));

        if (angle > 180.0f) {
            yaw = reviseAngle(180.0f + yaw);
            dir = -1;
        }
    }

    event->lwData[3] = (s32)(1024.0f * -yaw) + 0xF24A7A80;
    event->lwData[4] = ((yaw > 90.0f && yaw < 270.0f) ? -dir : dir) * 1024 + 0xF24A7A80;

    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_gazigazi_entry)
    CALL(evt_npc_entry, STRING("ガジガジ"), STRING("c_gazigazi"))
    CALL(evt_npc_set_position, STRING("ガジガジ"), 0, -1000, 0)
    CALL(evt_npc_pera_onoff, STRING("ガジガジ"), 0)
    CALL(evt_npc_set_ry, STRING("ガジガジ"), FLOAT(0.0f))
    CALL(evt_npc_flag_onoff, 1, STRING("ガジガジ"), 33554432)
    RETURN
EVT_END()

EVT_BEGIN(evt_gazigazi_anim_loop)
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_GAJIGAJI1"), LW(0), LW(1), LW(2), 0)
    LOOP(0)
        CALL(evt_npc_set_anim, STRING("ガジガジ"), STRING("GAZ_A_1"))
        CALL(evt_npc_wait_anim, STRING("ガジガジ"), FLOAT(1.0f))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_gazigazi_appear)
    SET_VALUE(LW(10), LW(0))
    SET_VALUE(LW(11), LW(1))
    SET_VALUE(LW(12), LW(2))
    SET_VALUE(LW(13), LW(0))
    SET_VALUE(LW(14), LW(1))
    SET_VALUE(LW(15), LW(2))
    ADD(LW(10), 0)
    ADD(LW(11), -50)
    ADD(LW(12), 0)
    ADD(LW(13), 0)
    ADD(LW(14), -10)
    ADD(LW(15), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ガジガジ"), 1073741856)
    CALL(evt_npc_set_rotate_offset, STRING("ガジガジ"), 0, 20, 0)
    CALL(camdir)
    CALL(evt_npc_set_rotate, STRING("ガジガジ"), FLOAT(0.0f), LW(3), FLOAT(270.0f))
    CALL(evt_npc_set_scale, STRING("ガジガジ"), FLOAT(1.0f), FLOAT(1.0f), LW(4))
    CALL(evt_npc_set_anim, STRING("ガジガジ"), STRING("GAZ_S_1"))
    SPAWN_EVENT_GET_ID(PTR(evt_gazigazi_anim_loop), LW(9))
    BEGIN_INLINE_EVENT
        CALL(evt_gazigazi_move_position, STRING("ガジガジ"), LW(10), LW(11), LW(12), LW(13), LW(14), LW(15), 300, 1)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_gazigazi_move_position, STRING("ガジガジ"), LW(13), LW(14), LW(15), LW(10), LW(11), LW(12), 600, 0)
    DELETE_EVENT(LW(9))
    CALL(evt_npc_set_position, STRING("ガジガジ"), 0, -1000, 0)
    RETURN
EVT_END()
// clang-format on

USER_FUNC(evt_gazigazi_move_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 startX = (f32)evtGetValue(event, args[1]);
    f32 startY = (f32)evtGetValue(event, args[2]);
    f32 startZ = (f32)evtGetValue(event, args[3]);
    f32 endX = (f32)evtGetValue(event, args[4]);
    f32 endY = (f32)evtGetValue(event, args[5]);
    f32 endZ = (f32)evtGetValue(event, args[6]);
    s32 steps = evtGetValue(event, args[7]);
    s32 mode = evtGetValue(event, args[8]);
    NpcEntry* npc = evtNpcNameToPtr(event, (char*)name);
    int curStep; // int, not s32, as evt_npc.c's `int elapsed`: s32 allocates differently
    Vec pos;

    if (isFirstCall) {
        event->userdata[0] = (s32)gp->renderTime;
    }
    curStep = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]);
    if (curStep < steps) {
        startX = intplGetValue(mode, curStep, steps, startX, endX);
        startY = intplGetValue(mode, curStep, steps, startY, endY);
        startZ = intplGetValue(mode, curStep, steps, startZ, endZ);
        pos = (Vec){ startX, startY, startZ };
        npc->prevPosition = pos;
        npc->position = npc->prevPosition;
        return EVT_RETURN_BLOCK;
    }
    pos = (Vec){ endX, endY, endZ };
    npc->prevPosition = pos;
    npc->position = npc->prevPosition;
    return EVT_RETURN_DONE;
}

#pragma explicit_zero_data on
__declspec(section ".data") u32 gap_05_8036203C_data = 0;
#pragma explicit_zero_data reset
