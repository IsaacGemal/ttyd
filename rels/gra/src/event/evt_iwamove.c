#include "evt_iwamove.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "driver/hitdrv.h"
#include "mario/mario.h"
#include "party.h"
#include "statuswindow.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

USER_FUNC(evt_iwamove_main) {
    u32* iwa = (u32*)event->lwData[0];
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        return 0;
    }

    switch ((s32)iwa[2]) {
    case 0:
        if (mario->hitobjPush == hitNameToPtr("A_h_iwa")) {
            iwa[1] = 0x50;
            iwa[2] = iwa[2] + 1;
            marioKeyOff();
            partyKeyOff();
            marioChgPose("M_O_1");
            mario->dispFlags |= 2;
            statusWinForceOpen();
        }
        break;
    case 1:
        if (keyGetStickX(0) > 10) {
            if ((s32)(iwa[1]--) == 0) {
                iwa[2] = iwa[2] + 1;
            }
        } else {
            iwa[2] = iwa[2] - 1;
            mario->dispFlags &= ~2;
            marioChgPose("M_S_1");
            marioKeyOn();
            partyKeyOn();
            statusWinForceOff();
        }
        break;
    case 2:
        if (iwa[4] != 0) {
            evtEntry((void*)iwa[4], 0, 0);
        }
        statusWinForceOff();
        iwa[2] = iwa[2] + 1;
        break;
    case 3:
        break;
    }
    return 0;
}

EVT_BEGIN(evt_iwamove_main_evt)
    CALL(evt_iwamove_main)
    RETURN
EVT_END()

u32 iwamove[5];

USER_FUNC(evt_iwamove_init) {
    iwamove[0] = 0;
    return 2;
}

USER_FUNC(evt_iwamove_ent) {
    EventEntry* entry;

    iwamove[4] = event->args[0];
    iwamove[2] = 0;
    entry = evtEntry(evt_iwamove_main_evt, 0, 0);
    iwamove[3] = (u32)entry;
    entry->lwData[0] = (s32)iwamove;
    return 2;
}
// clang-format on
#pragma force_active off
