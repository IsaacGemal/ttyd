// lect_status_off and lect_status_on. Lecture helpers: see rel/common/evt_lect.h.
#include "evt/evt_cmd.h"
#include "statuswindow.h"

static USER_FUNC(lect_status_off) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_status_on) {
    statusWinForceCloseClear();
    return EVT_RETURN_DONE;
}
