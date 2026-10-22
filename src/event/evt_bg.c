#include "driver/bgdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

USER_FUNC(evt_bg_disp_onoff) {
    if (evtGetValue(event, event->args[0]) != 0) {
        bgDispOn();
    } else {
        bgDispOff();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_bg_set_color) {
    s32* args = event->args;
    s32 r = evtGetValue(event, args[0]);
    s32 g = evtGetValue(event, args[1]);
    s32 b = evtGetValue(event, args[2]);
    s32 a = evtGetValue(event, args[3]);

    bgSetColor((GXColor){ r, g, b, a });
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_bg_auto_scroll_onoff) {
    if (evtGetValue(event, event->args[0]) != 0) {
        bgAutoScrollOff();
    } else {
        bgAutoScrollOn();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_bg_set_scrl_offset) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    bgSetScrlOffset(x, y);
    return EVT_RETURN_DONE;
}
