// effitemgetchange_cam, effitemgetchange_cam_ptr, effitemget_open and effitemget_open_ptr. Lecture helpers: see
// rel/common/evt_lect.h.
#include "driver/effdrv.h"
#include "eff/eff_itemget.h"
#include "evt/evt_cmd.h"

static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)name);

    ((effItemGetData*)effect->userdata)->camId = camId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* effect = (EffectEntry*)evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);

    ((effItemGetData*)effect->userdata)->camId = camId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    EffectEntry* effect = effNameToPtr((const char*)name);

    ((effItemGetData*)effect->userdata)->scaleMode = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open_ptr) {
    s32* args = event->args;
    EffectEntry* effect = (EffectEntry*)evtGetValue(event, args[0]);

    ((effItemGetData*)effect->userdata)->scaleMode = 1;
    return EVT_RETURN_DONE;
}
