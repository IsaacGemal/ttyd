#include "evt_goryuugijo.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "driver/dispdrv.h"
#include "eff/eff_minigame.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "rel/gor/gor_03.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

void evt_goryuugijo_disp_func(CameraId cameraId, void* param) {
    GoryuugijoWork* work = param;

    work->disp(cameraId, param);
}

USER_FUNC(evt_goryuugijo_main_func) {
    GoryuugijoWork* work = (GoryuugijoWork*)event->lwData[0];

    if (work->flags & 1) {
        work->flags |= 2;
        return 2;
    }
    if (work->field_0x04 != NULL) {
        if (evtCheckID(work->field_0x04->eventId)) {
            goto disp; // a running child event skips main(); a structured form repeats the call or the test
        }
        work->field_0x04 = NULL;
    }
    work->main(event, isFirstCall);
disp:
    dispEntry(8, 1000.0f, 0, evt_goryuugijo_disp_func, work);
    return 0;
}

static EVT_BEGIN(main_evt)
    CALL(evt_goryuugijo_main_func)
    RETURN
EVT_END()

USER_FUNC(evt_goryuugijo_alloc) {
    s32* args = event->args;
    GoryuugijoWork* work;
    s32 mode;

    work = _mapAlloc(0x20);
    memset(work, 0, 0x20);
    evtSetValue(event, args[0], (s32)work);
    mode = evtGetValue(event, args[1]);
    work->mode = mode;
    work->entry = evtEntry(main_evt, 0, 0);
    work->entry->lwData[0] = (s32)work;
    work->field_0x04 = NULL;
    switch (work->mode) {
    case 0:
        work->init = goryuugijo_init_montemedal;
        work->main = goryuugijo_main_montemedal;
        work->disp = goryuugijo_disp_montemedal;
        work->data = _mapAlloc(4);
        memset(work->data, 0, 4);
        break;
    case 1:
        work->init = (UserFunction)goryuugijo_init_slot;
        work->main = goryuugijo_main_slot;
        work->disp = goryuugijo_disp_slot;
        work->data = _mapAlloc(0x78);
        memset(work->data, 0, 0x78);
        break;
    }
    work->init(event, isFirstCall);
    return 2;
}

USER_FUNC(evt_goryuugijo_free) {
    s32* args = event->args;
    GoryuugijoWork* work = (GoryuugijoWork*)evtGetValue(event, args[0]);

    if (work == NULL) {
        return 2;
    }
    if (isFirstCall) {
        work->flags |= 1;
    }
    if (!(work->flags & 2)) {
        return 0;
    }
    if (work->data != NULL) {
        _mapFree(work->data);
    }
    _mapFree(work);
    evtSetValue(event, args[0], 0);
    return 2;
}

USER_FUNC(eff_call_yuugijo) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);

    effMiniGameEntry(type, x, y, z);
    return 2;
}
// clang-format on
