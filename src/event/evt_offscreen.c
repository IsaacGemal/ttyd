#include "driver/offscreendrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

USER_FUNC(evt_offscreen_set_texture_fmt) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 fmt = evtGetValue(event, args[1]);
    s32 unk = evtGetValue(event, args[2]);
    OffscreenEntry* entry = offscreenNameToPtr((const char*)name);

    entry->unk3C = fmt;
    entry->unk40 = unk;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_offscreen_get_boundingbox2) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;
    s32 leftArg = args[1];
    s32 topArg = args[2];
    s32 rightArg = args[3];
    s32 bottomArg = args[4];
    BOOL ok;

    id = offscreenNameToId((const char*)id);
#ifdef MUST_MATCH
    // Zeroing the locals is only here to reproduce the original's `li r0,0; sth ...` schedule;
    // offscreenGetBoundingBox_BeforeFrame fills all four when it returns TRUE.
    left = 0;
    top = 0;
    right = 0;
    bottom = 0;
#endif
    if (isFirstCall) {
        event->userdata[0] = 5;
    }
    ok = offscreenGetBoundingBox_BeforeFrame(id, &left, &top, &right, &bottom);
    event->userdata[0]--;
    if (!ok && event->userdata[0] != 0) {
        return EVT_RETURN_BLOCK;
    }
    evtSetValue(event, leftArg, left);
    evtSetValue(event, topArg, top);
    evtSetValue(event, rightArg, right - left);
    evtSetValue(event, bottomArg, bottom - top);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_offscreen_get_boundingbox) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;
    s32 leftArg = args[1];
    s32 topArg = args[2];
    s32 rightArg = args[3];
    s32 bottomArg = args[4];

    id = offscreenNameToId((const char*)id);
    if (!offscreenGetBoundingBox_BeforeFrame(id, &left, &top, &right, &bottom)) {
        return EVT_RETURN_BLOCK;
    }
    evtSetValue(event, leftArg, left);
    evtSetValue(event, topArg, top);
    evtSetValue(event, rightArg, right - left);
    evtSetValue(event, bottomArg, bottom - top);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_offscreen_delete) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);

    offscreenDelete((const char*)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_offscreen_entry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);

    offscreenEntry((const char*)name);
    return EVT_RETURN_DONE;
}
