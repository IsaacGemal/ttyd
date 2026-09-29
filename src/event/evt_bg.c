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
