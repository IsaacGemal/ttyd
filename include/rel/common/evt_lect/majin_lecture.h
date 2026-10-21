// dai_moveti_set, dai_moveti_get, lect_slit_check, evtflash, lect_push_r_check and evt_nomal_cange: the helpers of the
// units with a curse lecture (*_majin_lecture). See rel/common/evt_lect.h.
#include "driver/fadedrv.h"
#include "evt/evt_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "system.h"

#include <dolphin/os.h>

static USER_FUNC(dai_moveti_set) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    daiv = (Vec){x, y, z};
    return EVT_RETURN_DONE;
}

static USER_FUNC(dai_moveti_get) {
    s32* args = event->args;

    evtSetFloat(event, args[0], daiv.x);
    evtSetFloat(event, args[1], daiv.y);
    evtSetFloat(event, args[2], daiv.z);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_slit_check) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evtflash) {
    if (isFirstCall) {
        event->userdata[2] = 0;
    }

    switch (event->userdata[2]) {
        case 0:
            fadeEntry(FADE_OUT_BLACK, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            event->userdata[2]++;
            // fallthrough
        case 1:
            if (fadeIsFinish() != TRUE) {
                return EVT_RETURN_BLOCK;
            }
            event->userdata[0] = 60;
            event->savedTime = (s32)gp->renderTime;
            event->userdata[2]++;
            if (event->userdata[0] == 0) {
                event->userdata[2]++;
            }
            return EVT_RETURN_BLOCK;
        case 2:
            if ((u32)OSTicksToMilliseconds((u32)gp->renderTime - (u32)event->savedTime) <
                (u32)event->userdata[0]) {
                return EVT_RETURN_BLOCK;
            }
            event->userdata[2]++;
            return EVT_RETURN_BLOCK;
        case 3:
            fadeEntry(FADE_IN_BLACK, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            event->userdata[2]++;
            // fallthrough
        case 4:
            if (fadeIsFinish() != TRUE) {
                return EVT_RETURN_BLOCK;
            }
            event->userdata[0] = 60;
            event->savedTime = (s32)gp->renderTime;
            event->userdata[2]++;
            if (event->userdata[0] == 0) {
                event->userdata[2]++;
            }
            return EVT_RETURN_BLOCK;
        case 5:
            return (u32)OSTicksToMilliseconds((u32)gp->renderTime - (u32)event->savedTime) <
                           (u32)event->userdata[0]
                       ? EVT_RETURN_BLOCK
                       : EVT_RETURN_DONE;
        default:
            return EVT_RETURN_DONE;
    }
}

static USER_FUNC(lect_push_r_check) {
    s32* args = event->args;

    if (keyGetButton(0) & 0x20) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_nomal_cange) {
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        unk_8009f9c8();
        if (mario->flags & MARIO_FLAG_PAPER_MODE) {
            motSlitCancel3();
        }
    }
    if (!marioRollKeyDisable()) {
        return EVT_RETURN_BLOCK;
    }
    return (mario->dispFlags & 0x01000000) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}
