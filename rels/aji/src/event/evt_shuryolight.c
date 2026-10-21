#include "evt_shuryolight.h"
#include "rel/common/evt_shuryolight.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "driver/animdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(evt_shuryolight_delete);
static USER_FUNC(evt_shuryolight_run_tue_evt);
static USER_FUNC(evt_shuryolight_run_head_evt);
static USER_FUNC(evt_shuryolight_run_power_evt);
static USER_FUNC(evt_shuryolight_get_status);
static USER_FUNC(evt_shuryolight_get_anim_time);
static USER_FUNC(evt_shuryolight_wait_anim_end);
static USER_FUNC(evt_shuryolight_set_anim);
static s32 shuryolightRunEvt(void* evtCode, s32 eventId);
static void shuryolightSetAnim(s32 index, s32 anim);
static void shuryolight_callback(void* userData, s32 groupId, Mtx mtx);

static ShuryolightWork* slhp;

// clang-format off
extend_data(112,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(113,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static char* green_light_anim[5] = {
    "L_1C", "L_1B", "L_1D", "L_1A", "L_1E",
};

static char* yellow_light_anim[5] = {
    "L_2C", "L_2B", "L_2D", "L_2A", "L_2E",
};

static char* red_light_anim[5] = {
    "L_3C", "L_3B", "L_3D", "L_3A", "L_3E",
};

static char* orange_light_anim[5] = {
    "L_4C", "L_4B", "L_4D", "L_4A", "L_4E",
};

EVT_BEGIN(shuryolight_pow_normal_2_data_36ED8)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 0, 1)
        CALL(evt_sub_random, 3000, LW(0))
        ADD(LW(0), 3000)
        WAIT_MS(LW(0))
        CALL(evt_sub_random, 100, LW(0))
        DIVIDE(LW(0), 30)
        IF_INT_GREATER(LW(0), 0)
            LOOP(LW(0))
                CALL(evt_shuryolight_set_anim, 0, 3)
                WAIT_MS(80)
                CALL(evt_shuryolight_set_anim, 0, 1)
                WAIT_MS(80)
            END_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(shuryolight_pow_tenmetu)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 0, 1)
        CALL(evt_sub_random, 3000, LW(0))
        CALL(evt_sub_random, 1000, LW(1))
        ADD(LW(0), LW(1))
        ADD(LW(0), 700)
        WAIT_MS(LW(0))
        CALL(evt_shuryolight_set_anim, 0, 3)
        WAIT_MS(200)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_head_normal_2_data_37004)
    SET_VALUE(LW(0), 3)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, LW(0), 1)
        WAIT_MS(400)
        CALL(evt_shuryolight_set_anim, LW(0), 3)
        SUBTRACT(LW(0), 1)
        IF_INT_LESS(LW(0), 1)
            SET_VALUE(LW(0), 3)
            WAIT_MS(400)
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(shuryolight_head_tenmetu)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 1, 4)
        CALL(evt_shuryolight_set_anim, 2, 4)
        CALL(evt_shuryolight_set_anim, 3, 4)
        CALL(evt_shuryolight_wait_anim_end, 1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(shuryolight_head_off)
    CALL(evt_shuryolight_set_anim, 1, 3)
    CALL(evt_shuryolight_set_anim, 2, 3)
    CALL(evt_shuryolight_set_anim, 3, 3)
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_tue_normal_2_data_37104)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 4, 0)
        CALL(evt_shuryolight_wait_anim_end, 4)
        CALL(evt_shuryolight_set_anim, 4, 1)
        WAIT_MS(1000)
        CALL(evt_shuryolight_set_anim, 4, 2)
        CALL(evt_shuryolight_wait_anim_end, 4)
        CALL(evt_shuryolight_set_anim, 4, 3)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

#include "rel/common/shuryolight.h"

USER_FUNC(evt_shuryolight_init_2_text_B870) {
    NpcEntry* npc;
    NpcEntry* light;
    ShuryolightEntry* lights;

    npc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));
    npcEntry("light_power", "c_shuryo");
    npcEntry("light_head_1", "c_shuryo");
    npcEntry("light_head_2", "c_shuryo");
    npcEntry("light_head_3", "c_shuryo");
    npcEntry("light_tue_1", "c_shuryo");

    slhp = _mapAlloc(0x70);
    slhp->powerEvtId = 0;
    slhp->headEvtId = 0;
    slhp->tueEvtId = 0;
    lights = slhp->entries;

    light = npcNameToPtr("light_power");
    light->flags |= 0xa0;
    lights[0].npc = light;
    lights[0].animTable = green_light_anim;
    lights[0].groupIdx = animPoseGetGroupIdx(npc->poseId, "locator6");
    lights[0].shapeIdx = animPoseGetShapeIdx(light->poseId, "pPlaneShape34");
    shuryolightSetAnim(0, 1);

    light = npcNameToPtr("light_head_1");
    light->flags |= 0xa0;
    lights[1].npc = light;
    lights[1].animTable = orange_light_anim;
    lights[1].groupIdx = animPoseGetGroupIdx(npc->poseId, "locator9");
    lights[1].shapeIdx = animPoseGetShapeIdx(light->poseId, "pPlaneShape37");
    shuryolightSetAnim(1, 3);

    light = npcNameToPtr("light_head_2");
    light->flags |= 0xa0;
    lights[2].npc = light;
    lights[2].animTable = red_light_anim;
    lights[2].groupIdx = animPoseGetGroupIdx(npc->poseId, "locator8");
    lights[2].shapeIdx = animPoseGetShapeIdx(light->poseId, "pPlaneShape36");
    shuryolightSetAnim(2, 3);

    light = npcNameToPtr("light_head_3");
    light->flags |= 0xa0;
    lights[3].npc = light;
    lights[3].animTable = yellow_light_anim;
    lights[3].groupIdx = animPoseGetGroupIdx(npc->poseId, "locator7");
    lights[3].shapeIdx = animPoseGetShapeIdx(light->poseId, "pPlaneShape35");
    shuryolightSetAnim(3, 3);

    light = npcNameToPtr("light_tue_1");
    light->flags |= 0xa0;
    lights[4].npc = light;
    lights[4].animTable = red_light_anim;
    lights[4].groupIdx = animPoseGetGroupIdx(npc->poseId, "locator5");
    lights[4].shapeIdx = animPoseGetShapeIdx(light->poseId, "pPlaneShape36");
    shuryolightSetAnim(4, 1);

    animPoseSetDispCallBack(npc->poseId, shuryolight_callback, lights);
    return 2;
}

USER_FUNC(evt_shuryolight_set_anim) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 anim = evtGetValue(event, args[1]);

    shuryolightSetAnim(index, anim);
    return 2;
}

USER_FUNC(evt_shuryolight_wait_anim_end) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    NpcEntry** npc = &slhp->entries[idx].npc;
    s32 poseId = (*npc)->poseId;

    return animPoseGetLoopTimes(poseId) < 1.0f ? 0 : 2;
}

USER_FUNC(evt_shuryolight_get_anim_time) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    NpcEntry** npc = &slhp->entries[idx].npc;
    AnimData* data = animPoseGetAnimDataPtr((*npc)->poseId);
    f32 time = 1000.0f * (*(f32*)((u8*)data->clip + 8) - *(f32*)((u8*)data->clip + 4)) / 60.0f;

    evtSetValue(event, args[1], (s32)time);
    return 2;
}

USER_FUNC(evt_shuryolight_get_status) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32* anim = &slhp->entries[idx].animIdx;

    evtSetValue(event, args[1], *anim);
    return 2;
}

USER_FUNC(evt_shuryolight_run_evt_2_text_B538) {
    s32* args = event->args;
    void* power = (void*)evtGetValue(event, args[0]);
    void* head = (void*)evtGetValue(event, args[1]);
    void* tue = (void*)evtGetValue(event, args[2]);

    if (power != NULL) {
        slhp->powerEvtId = shuryolightRunEvt(power, slhp->powerEvtId);
    }
    if (head != NULL) {
        slhp->headEvtId = shuryolightRunEvt(head, slhp->headEvtId);
    }
    if (tue != NULL) {
        slhp->tueEvtId = shuryolightRunEvt(tue, slhp->tueEvtId);
    }
    return 2;
}

USER_FUNC(evt_shuryolight_run_power_evt) {
    void* evt = (void*)evtGetValue(event, event->args[0]);

    slhp->powerEvtId = shuryolightRunEvt(evt, slhp->powerEvtId);
    return 2;
}

USER_FUNC(evt_shuryolight_run_head_evt) {
    void* evt = (void*)evtGetValue(event, event->args[0]);

    slhp->headEvtId = shuryolightRunEvt(evt, slhp->headEvtId);
    return 2;
}

USER_FUNC(evt_shuryolight_run_tue_evt) {
    void* evt = (void*)evtGetValue(event, event->args[0]);

    slhp->tueEvtId = shuryolightRunEvt(evt, slhp->tueEvtId);
    return 2;
}

USER_FUNC(evt_shuryolight_delete) {
    s32 i;

    if (slhp->powerEvtId != 0) {
        evtDeleteID(slhp->powerEvtId);
        slhp->powerEvtId = 0;
    }
    if (slhp->headEvtId != 0) {
        evtDeleteID(slhp->headEvtId);
        slhp->headEvtId = 0;
    }
    if (slhp->tueEvtId != 0) {
        evtDeleteID(slhp->tueEvtId);
        slhp->tueEvtId = 0;
    }
    for (i = 0; i < 5; i++) {
        npcDeleteGroup(slhp->entries[i].npc);
    }
    _mapFree(slhp);
    slhp = NULL;
    return 2;
}
// clang-format on
