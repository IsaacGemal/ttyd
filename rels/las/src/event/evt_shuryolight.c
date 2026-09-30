#include "evt_shuryolight.h"
#include "driver/animdrv.h"
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(evt_shuryolight_wait_anim_end);
static USER_FUNC(evt_shuryolight_set_anim);

// Per-light state owned by evt_shuryolight_init and handed to shuryolight_callback as its display user data.
typedef struct ShuryolightEntry {
    NpcEntry* npc;    // 0x0
    char** animTable; // 0x4
    s32 animIdx;      // 0x8
    s32 groupIdx;     // 0xC
    s32 shapeIdx;     // 0x10
} ShuryolightEntry;

// The 0x70-byte block allocated by evt_shuryolight_init; slhp points at it.
typedef struct ShuryolightWork {
    s32 powerEvtId;               // 0x0
    s32 headEvtId;                // 0x4
    s32 tueEvtId;                 // 0x8
    ShuryolightEntry entries[5];  // 0xC
} ShuryolightWork;

static ShuryolightWork* slhp;

// clang-format off
extend_data(188,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(189,
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

EVT_BEGIN(shuryolight_pow_normal_17_data_53DB0)
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

EVT_BEGIN(shuryolight_pow_tenmetu_17_data_53E60)
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

EVT_BEGIN(shuryolight_head_normal_17_data_53EDC)
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

EVT_BEGIN(shuryolight_head_tenmetu_17_data_53F54)
    LOOP(0)
        CALL(evt_shuryolight_set_anim, 1, 4)
        CALL(evt_shuryolight_set_anim, 2, 4)
        CALL(evt_shuryolight_set_anim, 3, 4)
        CALL(evt_shuryolight_wait_anim_end, 1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_head_off_17_data_53FA4)
    CALL(evt_shuryolight_set_anim, 1, 3)
    CALL(evt_shuryolight_set_anim, 2, 3)
    CALL(evt_shuryolight_set_anim, 3, 3)
    RETURN
EVT_END()

EVT_BEGIN(shuryolight_tue_normal_17_data_53FDC)
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

// clang-format on

// Display callback for the light pose: find the entry whose group is being drawn and draw its plane shape.
static void shuryolight_callback(void* userData, s32 groupId, Mtx mtx) {
    ShuryolightEntry* entries = userData;
    s32 i;

    for (i = 0; i < 5; i++) {
        if (groupId == entries[i].groupIdx) {
            break;
        }
    }
    if (i != 5) {
        animPoseMain(entries[i].npc->poseId);
        animPoseDrawShape(entries[i].npc->poseId, entries[i].shapeIdx);
    }
}

static void shuryolightSetAnim(s32 index, s32 anim) {
    ShuryolightEntry* entry = &slhp->entries[index];

    entry->animIdx = anim;
    animPoseSetAnim(entry->npc->poseId, entry->animTable[anim], TRUE);
}

static s32 shuryolightRunEvt(void* evtCode, s32 id) {
    if (evtCheckID(id)) {
        evtDeleteID(id);
    }
    return evtEntryType(evtCode, 0, 0, 0)->eventId;
}

USER_FUNC(evt_shuryolight_init_17_text_10B60) {
    NpcEntry* mainNpc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));
    NpcEntry* npc;
    ShuryolightEntry* entries;

    npcEntry("light_power", "c_shuryo");
    npcEntry("light_head_1", "c_shuryo");
    npcEntry("light_head_2", "c_shuryo");
    npcEntry("light_head_3", "c_shuryo");
    npcEntry("light_tue_1", "c_shuryo");

    slhp = _mapAlloc(0x70);
    slhp->powerEvtId = 0;
    slhp->headEvtId = 0;
    slhp->tueEvtId = 0;
    entries = slhp->entries;

    npc = npcNameToPtr("light_power");
    npc->flags |= 0xa0;
    entries[0].npc = npc;
    entries[0].animTable = green_light_anim;
    entries[0].groupIdx = animPoseGetGroupIdx(mainNpc->poseId, "locator6");
    entries[0].shapeIdx = animPoseGetShapeIdx(npc->poseId, "pPlaneShape34");
    shuryolightSetAnim(0, 1);

    npc = npcNameToPtr("light_head_1");
    npc->flags |= 0xa0;
    entries[1].npc = npc;
    entries[1].animTable = orange_light_anim;
    entries[1].groupIdx = animPoseGetGroupIdx(mainNpc->poseId, "locator9");
    entries[1].shapeIdx = animPoseGetShapeIdx(npc->poseId, "pPlaneShape37");
    shuryolightSetAnim(1, 3);

    npc = npcNameToPtr("light_head_2");
    npc->flags |= 0xa0;
    entries[2].npc = npc;
    entries[2].animTable = red_light_anim;
    entries[2].groupIdx = animPoseGetGroupIdx(mainNpc->poseId, "locator8");
    entries[2].shapeIdx = animPoseGetShapeIdx(npc->poseId, "pPlaneShape36");
    shuryolightSetAnim(2, 3);

    npc = npcNameToPtr("light_head_3");
    npc->flags |= 0xa0;
    entries[3].npc = npc;
    entries[3].animTable = yellow_light_anim;
    entries[3].groupIdx = animPoseGetGroupIdx(mainNpc->poseId, "locator7");
    entries[3].shapeIdx = animPoseGetShapeIdx(npc->poseId, "pPlaneShape35");
    shuryolightSetAnim(3, 3);

    npc = npcNameToPtr("light_tue_1");
    npc->flags |= 0xa0;
    entries[4].npc = npc;
    entries[4].animTable = red_light_anim;
    entries[4].groupIdx = animPoseGetGroupIdx(mainNpc->poseId, "locator5");
    entries[4].shapeIdx = animPoseGetShapeIdx(npc->poseId, "pPlaneShape36");
    shuryolightSetAnim(4, 1);

    animPoseSetDispCallBack(mainNpc->poseId, shuryolight_callback, entries);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_set_anim) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 anim = evtGetValue(event, args[1]);
    ShuryolightEntry* entry = &slhp->entries[index];

    entry->animIdx = anim;
    animPoseSetAnim(entry->npc->poseId, entry->animTable[anim], TRUE);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_wait_anim_end) {
    s32 index = evtGetValue(event, event->args[0]);
    ShuryolightEntry* entry = &slhp->entries[index];

    if (animPoseGetLoopTimes(entry->npc->poseId) < 1.0f) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_get_anim_time) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    ShuryolightEntry* entry = &slhp->entries[index];
    AnimData* data = animPoseGetAnimDataPtr(entry->npc->poseId);
    f32 start = *(f32*)(*(u8**)((u8*)data + 0x24) + 0x4);
    f32 end = *(f32*)(*(u8**)((u8*)data + 0x24) + 0x8);
    f32 time = 1000.0f * (end - start) / 60.0f;

    evtSetValue(event, args[1], (s32)time);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_get_status) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    ShuryolightEntry* entry = &slhp->entries[index];

    evtSetValue(event, args[1], entry->animIdx);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_run_evt_17_text_10828) {
    s32 id;
    s32* args = event->args;
    u32 power = evtGetValue(event, args[0]);
    u32 head = evtGetValue(event, args[1]);
    u32 tue = evtGetValue(event, args[2]);
    if (power != 0) {
        id = slhp->powerEvtId;
        if (evtCheckID(id)) {
            evtDeleteID(id);
        }
        slhp->powerEvtId = evtEntryType((void*)power, 0, 0, 0)->eventId;
    }
    if (head != 0) {
        id = slhp->headEvtId;
        if (evtCheckID(id)) {
            evtDeleteID(id);
        }
        slhp->headEvtId = evtEntryType((void*)head, 0, 0, 0)->eventId;
    }
    if (tue != 0) {
        id = slhp->tueEvtId;
        if (evtCheckID(id)) {
            evtDeleteID(id);
        }
        slhp->tueEvtId = evtEntryType((void*)tue, 0, 0, 0)->eventId;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_shuryolight_run_power_evt) {
#ifdef MUST_MATCH
    // declaration order puts evtCode in r30 and id in r31, as the original
    s32 id;
    s32 evtCode;
    evtCode = evtGetValue(event, event->args[0]);
    id = slhp->powerEvtId;
#else
    s32 evtCode = evtGetValue(event, event->args[0]);
    s32 id = slhp->powerEvtId;
#endif

    if (evtCheckID(id)) {
        evtDeleteID(id);
    }
    slhp->powerEvtId = evtEntryType((void*)evtCode, 0, 0, 0)->eventId;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_run_head_evt_17_text_10718) {
#ifdef MUST_MATCH
    s32 id;
    s32 evtCode;
    evtCode = evtGetValue(event, event->args[0]);
    id = slhp->headEvtId;
#else
    s32 evtCode = evtGetValue(event, event->args[0]);
    s32 id = slhp->headEvtId;
#endif

    if (evtCheckID(id)) {
        evtDeleteID(id);
    }
    slhp->headEvtId = evtEntryType((void*)evtCode, 0, 0, 0)->eventId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_shuryolight_run_tue_evt) {
#ifdef MUST_MATCH
    s32 id;
    s32 evtCode;
    evtCode = evtGetValue(event, event->args[0]);
    id = slhp->tueEvtId;
#else
    s32 evtCode = evtGetValue(event, event->args[0]);
    s32 id = slhp->tueEvtId;
#endif

    if (evtCheckID(id)) {
        evtDeleteID(id);
    }
    slhp->tueEvtId = evtEntryType((void*)evtCode, 0, 0, 0)->eventId;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_shuryolight_delete_17_text_10588) {
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
    return EVT_RETURN_DONE;
}
