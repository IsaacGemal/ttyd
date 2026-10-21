// lect_mario_ido_sub and lect_mario_ido_sum. Lecture helpers: see rel/common/evt_lect.h.
#include "evt/evt_cmd.h"
#include "mario/mario.h"

#include <dolphin/os.h>

static f32 lect_mario_ido_sub(f32 elapsed, f32 duration, f32 start, f32 end) {
    f32 t = duration - elapsed;

    return (t * start + elapsed * end) / duration;
}

static USER_FUNC(lect_mario_ido_sum) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 duration = evtGetValue(event, args[0]);
    s32 elapsed;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(0), mario->position.x);
        evtSetFloat(event, LW(1), mario->position.y);
        evtSetFloat(event, LW(2), mario->position.z);
    }
    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > duration) {
        elapsed = duration;
    }
    mario->position.x = lect_mario_ido_sub(elapsed, duration, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    mario->position.y = lect_mario_ido_sub(elapsed, duration, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    mario->position.z = lect_mario_ido_sub(elapsed, duration, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));
    if (elapsed >= duration) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
