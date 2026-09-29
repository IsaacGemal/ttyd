#include "evt_yuuminigame.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "rel/yuu/yuu.h"
#include "eff/eff_minigame.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "driver/dispdrv.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(main_evt)
    CALL(evt_yuuminigame_main_func)
    RETURN
EVT_END()

static s32 dataadr[32];
s32 stackt[4];
s32 stackp;

void evt_yuuminigame_disp_func(CameraId cameraId, void* param) {
    YuuminigameWork* work = param;
    work->disp(cameraId, param);
}

USER_FUNC(evt_yuuminigame_main_func) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    if (work->flags & 1) {
        work->flags |= 2;
        return 2;
    }
    if (work->childEvent != NULL) {
        if (evtCheckID(work->childEvent->eventId)) {
            goto disp;
        }
        work->childEvent = NULL;
    }
    work->main(event, isFirstCall);
disp:
    dispEntry(8, 1000.0f, 0, evt_yuuminigame_disp_func, work);
    return 0;
}

USER_FUNC(evt_yuuminigame_alloc) {
    s32* args = event->args;
    YuuminigameWork* work = _mapAlloc(sizeof(YuuminigameWork));
    memset(work, 0, sizeof(YuuminigameWork));
    evtSetValue(event, args[0], (s32)work);
    work->type = evtGetValue(event, args[1]);
    work->event = evtEntry(main_evt, 0, 0);
    work->event->lwData[0] = (s32)work;
    work->childEvent = NULL;
    switch (work->type) {
        case 0:
            work->init = yuuminigame_init_1hikoki;
            work->main = yuuminigame_main_1hikoki;
            work->disp = yuuminigame_disp_1hikoki;
            work->data = _mapAlloc(0x18);
            memset(work->data, 0, 0x18);
            break;
        case 1:
            work->init = yuuminigame_init_2fune;
            work->main = yuuminigame_main_2fune;
            work->disp = yuuminigame_disp_2fune;
            work->data = _mapAlloc(0x3C);
            memset(work->data, 0, 0x3C);
            break;
        case 2:
            work->init = yuuminigame_init_3roll;
            work->main = yuuminigame_main_3roll;
            work->disp = yuuminigame_disp_3roll;
            work->data = _mapAlloc(0x2C);
            memset(work->data, 0, 0x2C);
            break;
        case 3:
            work->init = yuuminigame_init_4slit;
            work->main = yuuminigame_main_4slit;
            work->disp = yuuminigame_disp_4slit;
            work->data = _mapAlloc(0x28);
            memset(work->data, 0, 0x28);
            yuunpc_yuuminigame4_ptr = work;
            break;
    }
    work->init(event, isFirstCall);
    return 2;
}

USER_FUNC(evt_yuuminigame_free) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    if (work == NULL) {
        return 2;
    }
    if (isFirstCall) {
        work->flags |= 1;
    }
    if (!(work->flags & 2)) {
        return 0;
    }
    switch (work->type) {
        case 3:
            yuunpc_yuuminigame4_ptr = NULL;
            break;
    }
    if (work->data != NULL) {
        _mapFree(work->data);
    }
    _mapFree(work);
    evtSetValue(event, args[0], 0);
    return 2;
}

USER_FUNC(yuuminigame_get_dataadr) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    if (index >= 32) {
        evtSetValue(event, args[1], 0);
    } else {
        evtSetValue(event, args[1], dataadr[index]);
    }
    return 2;
}

USER_FUNC(yuuminigame_set_dataadr) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    if (index < 32) {
        dataadr[index] = evtGetValue(event, args[1]);
    }
    return 2;
}

USER_FUNC(eff_call_minigame) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    effMiniGameEntry(type, x, y, z);
    return 2;
}

USER_FUNC(yuu_atai_push) {
    s32* args = event->args;
    if (stackp < 4) {
        stackt[stackp] = evtGetValue(event, args[0]);
        stackp++;
    }
    return 2;
}

USER_FUNC(yuu_atai_pop) {
    s32* args = event->args;
    if (stackp > 0) {
        stackp--;
        evtSetValue(event, args[0], stackt[stackp]);
    }
    return 2;
}

USER_FUNC(yuu_atai_init) {
    stackp = 0;
    return 2;
}
// clang-format on
