#include "evt/evt_cmd.h"

#include <dolphin/os.h>

#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"

// Globals owned by this translation unit (not yet in a header). Metrowerks emits .sbss in reverse
// declaration order, so these are declared in the reverse of the original address order.
Vec stPt;
Vec stAt;
Vec stMpos;
Vec stPpos;
Vec stXpos;
f32 stMdir1;
f32 stMdir2;
f32 stMdir3;
f32 stMdir4;
f32 stPdir1;
f32 stPdir2;
f32 stPdir3;
f32 stPdir4;
f32 stXdir1;
f32 stXdir2;
f32 stXdir3;
f32 stXdir4;
s32 lect_work;

USER_FUNC(lect_set_systemlevel) {
    s32* args = event->args;

    marioStSystemLevel(evtGetValue(event, args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_test_systemlevel) {
    marioStGetSystemLevel();
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_cam_save) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    PartyEntry* extraParty = partyGetPtr(marioGetExtraPartyId());

    stPt = cam->cameraPos;
    stAt = cam->cameraAt;
    stMpos = mario->position;
    stMdir1 = mario->dispDir;
    stMdir2 = mario->dispReqDir;
    stMdir3 = mario->directionView;
    stMdir4 = mario->moveDir;

    if (party != NULL) {
        stPpos = party->position;
        stPdir1 = party->dispDir;
        stPdir2 = party->dispReqDir;
        stPdir3 = party->dir;
        stPdir4 = party->moveDir;
    }
    if (extraParty != NULL) {
        stXpos = extraParty->position;
        stXdir1 = extraParty->dispDir;
        stXdir2 = extraParty->dispReqDir;
        stXdir3 = extraParty->dir;
        stXdir4 = extraParty->moveDir;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_cam_load) {
    s32* args = event->args;
    s32 time = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    PartyEntry* extraParty = partyGetPtr(marioGetExtraPartyId());

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = stPt;
    cam->storedAt = stAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = 0xB;

    mario->position = stMpos;
    mario->dispDir = stMdir1;
    mario->dispReqDir = stMdir2;
    mario->directionView = stMdir3;
    mario->moveDir = stMdir4;

    if (party != NULL) {
        party->position = stPpos;
        party->dispDir = stPdir1;
        party->dispReqDir = stPdir2;
        party->dir = stPdir3;
        party->moveDir = stPdir4;
    }
    if (extraParty != NULL) {
        extraParty->position = stXpos;
        extraParty->dispDir = stXdir1;
        extraParty->dispReqDir = stXdir2;
        extraParty->dir = stXdir3;
        extraParty->moveDir = stXdir4;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_juyoitem_rel_set) {
    s32* args = event->args;

    lect_work = evtGetValue(event, args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_juyoitem_rel_get) {
    s32* args = event->args;

    evtSetValue(event, args[0], lect_work);
    return EVT_RETURN_DONE;
}

// .sdata
s32 all_juyoitem_lecture[] = {
    CALL(lect_juyoitem_rel_get, LW(0))
    CALL_CHILD(LW(0))
    0x00000002,
    0x00000001,
};
