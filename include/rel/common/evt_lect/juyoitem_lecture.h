// juyoitem_get_no, juyoitem_end, juyoitem_gogo, juyoitem_goato, lect_mobj_set_nosysblend, lect_push_a_check and
// lect_push_b_check: the helpers of the units with a key-item lecture (*_juyoitem_lecture). See rel/common/evt_lect.h.
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/itemdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_cmd.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mariost.h"
#include "system.h"

#include <dolphin/pad.h>
#include <string.h>

static USER_FUNC(juyoitem_get_no) {
    s32* args = event->args;
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    evtSetValue(event, args[0], item->itemId);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_end) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];
    EffectEntry* effect = (EffectEntry*)event->lwData[9];

    effSoftDelete(effect);
    marioSetCamId(CAMERA_3D);
    iconFlagOff(item->name, 0x200);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_gogo) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    iconFlagOff(item->name, 0x200);
    itemSetPosition(item, 0.0f, -1000.0f, 0.0f);
    marioChgStayMotion();
    marioStSystemLevel(0);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_goato) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    iconFlagOn(item->name, 2);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_mobj_set_nosysblend) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* obj;

    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    animPoseSetMaterialFlagOn(obj->poseId, 0x1800);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_push_a_check) {
    s32* args = event->args;

    if (keyGetButton(0) & PAD_BUTTON_A) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_push_b_check) {
    s32* args = event->args;

    if (keyGetButton(0) & PAD_BUTTON_B) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}
