// lecture_button_set_pos. A lecture helper: see rel/common/evt_lect.h.
#include "evt/evt_cmd.h"
#include "rel/common/evt_lect.h"

static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    LectureButton* btn = &lbtn[idx];

    btn->position = (Vec){x, y, z};
    return EVT_RETURN_DONE;
}
