#include "bom.h"
#include "bom_01.h"
#include "bom_02.h"
#include "battle_database_bom.h"
#include "data/mapdata.h"
#include "driver/animdrv.h"
#include "driver/npcdrv.h"
#include "eff/eff_mist.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "memory.h"
#include "party.h"
#include "party/party_vivian.h"
#include "system.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

USER_FUNC(bress) {
    static f32 offset[8] = {
        0.0f, 10.0f, 24.0f, 10.0f,
        15.0f, 20.0f, 20.0f, 10.0f,
    };
    MarioWork* mario;
    PartyEntry* party0;
    PartyEntry* party1;
    NpcWork* npcWork;
    NpcEntry* npc;
    s32* timers;
    s32 i;

    mario = marioGetPtr();
    party0 = partyGetPtr(0);
    party1 = partyGetPtr(1);
    npcWork = npcGetWorkPtr();

    if (isFirstCall) {
        event->userdata[0] = 30;
        event->userdata[1] = 30;
        event->userdata[2] = 30;
        timers = _mapAlloc(npcWork->total * 4);
        event->field_0x84 = (u32)timers;
        memset(timers, 0, npcWork->total * 4);
    }

    timers = (s32*)event->field_0x84;

    if (event->userdata[0] != 0) {
        event->userdata[0]--;
    }
    if (event->userdata[1] != 0) {
        event->userdata[1]--;
    }
    if (event->userdata[2] != 0) {
        event->userdata[2]--;
    }
    for (i = 0; i < npcWork->total; i++) {
        if (timers[i] != 0) {
            timers[i]--;
        }
    }

    if (mario != NULL && event->userdata[0] == 0 && !marioChkSts(4) && !marioChkSts(0x10) && !marioChkSts(8) &&
        vivianGetStatus() != 2) {
        effMistEntry(4, 30, mario->position.x, mario->position.y + 24.0f, mario->position.z,
                     270.0f - mario->dispDir);
        event->userdata[0] = irand(60) + 60;
    }
    if (party0 != NULL && event->userdata[1] == 0 && !marioChkSts(4) && !marioChkSts(0x10) && !marioChkSts(8) &&
        vivianGetStatus() != 2) {
        effMistEntry(4, 30, party0->position.x, party0->position.y + offset[party0->currentMemberId],
                     party0->position.z, 270.0f - party0->dispDir);
        event->userdata[1] = irand(60) + 60;
    }
    if (party1 != NULL && event->userdata[2] == 0) {
        event->userdata[2] = irand(60) + 60;
    }

    npc = npcWork->entries;
    for (i = 0; i < npcWork->total; i++, npc++) {
        if ((npc->flags & 1) && timers[i] == 0) {
            AnimationPose* pose = animPoseGetAnimPosePtr(npc->poseId);

            effMistEntry(4, 30, npc->position.x, npc->position.y + npc->height / 2.0f, npc->position.z,
                         270.0f - pose->targetRotY);
            timers[i] = irand(60) + 60;
        }
    }

    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(evt_bress)
    CALL(bress)
    RETURN
EVT_END()

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("bom_00", bom_00_init_evt);
    relSetEvtAddr("bom_01", bom_01_init_evt);
    relSetEvtAddr("bom_02", bom_02_init_evt);
    relSetEvtAddr("bom_03", bom_03_init_evt);
    relSetEvtAddr("bom_04", bom_04_init_evt);
    relSetBtlAddr("bom", btlsetup_bom_tbl, btl_bom_setup_no_tbl);
    evtEntryType(evt_bress, 0, 0, 0);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}
// clang-format on
