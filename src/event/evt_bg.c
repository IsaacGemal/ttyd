#include "driver/bgdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80422fe8;

static inline GXColor makeColor(s32 r, s32 g, s32 b, s32 a) {
    GXColor color = unk_80422fe8;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

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

    bgSetColor(makeColor(r, g, b, a));
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
