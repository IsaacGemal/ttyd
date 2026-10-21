// lecture_button_set_icon_id, lecture_button_set_alpha, lecture_button_flag_on_off, lecture_button_set_seq,
// lecture_button_get_icon_id and lecture_button_seq_wait. Lecture helpers: see rel/common/evt_lect.h.
#include "evt/evt_cmd.h"
#include "rel/common/evt_lect.h"

static USER_FUNC(lecture_button_set_icon_id) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    btn->iconId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    btn->alpha = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);
    s32 on = evtGetValue(event, args[2]);
    LectureButton* btn = &lbtn[idx];

    if (on) {
        btn->flags |= mask;
    } else {
        btn->flags &= ~mask;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    btn->seq = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_get_icon_id) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    evtSetValue(event, args[1], btn->seq);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 seq = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];

    return (btn->seq != seq) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}
