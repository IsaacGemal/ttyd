#include "evt_yuucounter.h"
#include "evt/evt_cmd.h"
#include "evt/evt_yuugijou.h"
#include "evt_yuunpc.h"
#include "forcedata.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

const u32 dat_yuu_00014dd0 = 0xFFFFFFFF;
const u32 dat_yuu_00014dd4 = 0xFFFFFF99;

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(main_evt)
    CALL(evt_yuucounter_main_func)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_yuu_00019728)
    CALL(unk_yuu_00009d5c, 1, 4)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_yuu_00019740)
    RETURN
EVT_END()

static u8 yuunpc_wp[0x4];
static YuuCounterWork* _wp;
USER_FUNC(unk_yuu_00009d5c) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    s32 mask = evtGetValue(event, args[1]);
    YuuCounterWork* work = _wp;

    if (flag != 0) {
        work->flags2 |= mask;
    } else {
        work->flags2 &= ~mask;
    }
    return EVT_RETURN_DONE;
}

// The case-0, case-2 and sub-second loops position each digit at `i * 30` (`i * 15`); loop strength
// reduction turns that into the original's x-offset temporaries, which colour after the named locals
// (a named `xoff` counter takes a higher register and pushes `work` to r31). Case 1 keeps its named
// counter, stepped in the for-header so its add schedules before the loop compare. The declaration
// order of the integer locals sets the remaining callee-saved registers. The case-0
// `marioGetPtr()->position.x` value is read through a `MarioWork*` local so the FP add/multiply land
// in the original's f0/f1/f2 rotation.
void evt_yuucounter_disp_func(CameraId cameraId, void* param) {
    s32 i3;
    s32 digit0, digit1, digit2;
    s32 skip2;
    s32 i2;
    s32 subsec;
    s32 i0;
    s32 icon;
    s32 seconds;
    s32 skip0;
    s32 xoff1;
    s32 time;
    s32 i1;
    YuuCounterWork* work;
    s32 digits[4];
    GXColor digitColor;
    GXColor iconColor;
    Mtx mtxTemp;
    Mtx mtxScale;
    f32 scale;
    Mtx mtx;
    f32 xbase;
    GXColor color;

    work = param;
    color = *(GXColor*)&dat_yuu_00014dd0;
    iconColor = *(GXColor*)&dat_yuu_00014dd0;
    digitColor = *(GXColor*)&dat_yuu_00014dd0;

    switch (work->mode) {
    case 0:
        if (!(work->flags2 & 2)) {
#ifdef MUST_MATCH
            MarioWork* mario = marioGetPtr();
            f32 x = mario->position.x;
#else
            f32 x = marioGetPtr()->position.x;
#endif
            work->time = (s32)(200.0f * (1300.0f + x));
        }
        scale = 2.5f;
        icon = 0x202;
        xbase = -50.0f;
        if (work->time < 0) {
            return;
        }
        break;
    case 1:
        if (!(work->flags2 & 2)) {
            if (work->time >= 0) {
                if (work->flags2 & 4) {
                    work->flags2 &= ~4;
                    work->last = gp->renderTime;
                }
                work->accum += (s32)gp->renderTime - (s32)work->last;
                work->last = gp->renderTime;
                work->time = (s32)((s64)work->limit - work->accum / (OS_TIMER_CLOCK / 1000));
            } else {
                work->time = -1;
            }
        }
        scale = 3.0f;
        icon = 0x201;
        xbase = -42.0f;
        break;
    case 2:
        if (!(work->flags2 & 2)) {
            Vec pos = marioGetPtr()->position;
            work->time = yuunpc_juni(&pos);
        }
        scale = 2.5f;
        icon = -1;
        xbase = -46.0f;
        break;
    }

    if (work->flags2 & 1) {
        return;
    }

    time = work->time;
    if (time < 0) {
        time = 0;
    }
    seconds = time / 1000;
    subsec = time % 1000;

    PSMTXTrans(mtx, work->x, work->y, 0.0f);
    PSMTXScale(mtxScale, scale, 1.0f, 1.0f);
    PSMTXConcat(mtx, mtxScale, mtxTemp);
    iconDispGxCol(mtxTemp, 0x10, 0x1F6, *(GXColor*)&dat_yuu_00014dd4);

    switch (work->mode) {
    case 0: {
        digits[0] = seconds / 100;
        seconds %= 100;
        digits[1] = seconds / 10;
        digits[2] = seconds % 10;

        skip0 = 1;
        for (i0 = 0; i0 < 3; i0++) {
            digit0 = digits[i0];
            if (digit0 != 0 || i0 == 2) {
                skip0 = 0;
            }
            if (!skip0) {
                PSMTXTrans(mtxTemp, xbase + (f32)(i0 * 30), 8.0f, 0.0f);
                PSMTXConcat(mtx, mtxTemp, mtxTemp);
                iconDispGxCol(mtxTemp, 0x10, (digit0 + 0x1F7) & 0xFFFF, digitColor);
            }
        }
        break;
    }
    case 1: {
        digits[0] = seconds / 60;
        seconds %= 60;
        digits[1] = seconds / 10;
        digits[2] = seconds % 10;

        for (i1 = 0, xoff1 = 0; i1 < 3; i1++, xoff1 += 30) {
            digit1 = digits[i1];
            PSMTXTrans(mtxTemp, xbase + (f32)xoff1 - (f32)((i1 == 0) << 4), 8.0f, 0.0f);
            PSMTXConcat(mtx, mtxTemp, mtxTemp);
            iconDispGxCol(mtxTemp, 0x10, (digit1 + 0x1F7) & 0xFFFF, digitColor);
        }

        PSMTXTrans(mtxTemp, 8.0f + xbase, 12.0f, 0.0f);
        PSMTXScale(mtxScale, 2.0f, 2.0f, 2.0f);
        PSMTXConcat(mtxTemp, mtxScale, mtxTemp);
        PSMTXConcat(mtx, mtxTemp, mtxTemp);
        iconDispGxCol(mtxTemp, 0x10, icon & 0xFFFF, iconColor);
        break;
    }
    case 2: {
        s32 val2 = time + 1;
        s32 rem = val2 % 100000;
        digits[0] = rem / 1000;
        rem %= 1000;
        digits[1] = rem / 100;
        rem %= 100;
        digits[2] = rem / 10;
        digits[3] = rem % 10;

        skip2 = 1;
        for (i2 = 0; i2 < 4; i2++) {
            digit2 = digits[i2];
            if (digit2 != 0 || i2 == 3) {
                skip2 = 0;
            }
            if (!skip2) {
                PSMTXTrans(mtxTemp, xbase + (f32)(i2 * 30), 8.0f, 0.0f);
                PSMTXConcat(mtx, mtxTemp, mtxTemp);
                iconDispGxCol(mtxTemp, 0x10, (digit2 + 0x1F7) & 0xFFFF, digitColor);
            }
        }
        break;
    }
    }

    if (icon != -1) {
        PSMTXTrans(mtxTemp, 90.0f + xbase - 12.0f, 10.0f, 0.0f);
        PSMTXConcat(mtx, mtxTemp, mtxTemp);
        iconDispGxCol(mtxTemp, 0x10, icon & 0xFFFF, iconColor);
    }

    switch (work->mode) {
    case 0:
    case 1:
        digits[0] = subsec / 100;
        digits[1] = subsec % 100 / 10;
        for (i3 = 0; i3 < 2; i3++) {
            PSMTXTrans(mtxTemp, 90.0f + xbase + (f32)(i3 * 15), 8.0f, 0.0f);
            PSMTXScale(mtxScale, 0.5f, 0.5f, 0.5f);
            PSMTXConcat(mtxTemp, mtxScale, mtxTemp);
            PSMTXConcat(mtx, mtxTemp, mtxTemp);
            iconDispGxCol(mtxTemp, 0x10, (digits[i3] + 0x1F7) & 0xFFFF, digitColor);
        }
        break;
    case 2:
        break;
    }
}

USER_FUNC(evt_yuucounter_main_func) {
    YuuCounterWork* work = (YuuCounterWork*)event->lwData[0];

    if (work->flags & 1) {
        work->flags |= 2;
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, 1000.0f, 0, evt_yuucounter_disp_func, work);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_yuucounter_alloc) {
    s32* args = event->args;
    YuuCounterWork* work;
    s32 value;

    work = _mapAlloc(sizeof(YuuCounterWork));
    memset(work, 0, sizeof(YuuCounterWork));
    _wp = work;
    work->x = evtGetFloat(event, args[0]);
    work->y = evtGetFloat(event, args[1]);
    work->mode = evtGetValue(event, args[2]);
    value = evtGetValue(event, args[3]);
    switch (work->mode) {
    case 1:
        work->limit = value * 1000;
        work->last = gp->renderTime;
        work->accum = 0;
        work->time = work->limit;
        break;
    case 2:
        work->value = value;
        break;
    }
    work->event = evtEntry(main_evt, 0, 0);
    work->event->lwData[0] = (s32)work;
    if (work->mode == 1) {
        yuuSetWindowEvt((s32)unk_evt_yuu_00019728);
    } else {
        yuuSetWindowEvt((s32)unk_evt_yuu_00019740);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_yuucounter_free) {
    YuuCounterWork* work = _wp;

    if (work == NULL) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall != 0) {
        work->flags |= 1;
    }
    if (!(work->flags & 2)) {
        return EVT_RETURN_BLOCK;
    }
    _mapFree(_wp);
    _wp = NULL;
    return EVT_RETURN_DONE;
}

s32 yuucounter_get_data(void) {
    if (_wp == NULL) {
        return 0;
    }
    return _wp->time;
}

void yuucounter_set_data(s32 data) {
    if (_wp != NULL) {
        _wp->time = data;
    }
}

void yuucounter_flag_on(u32 mask) {
    if (_wp != NULL) {
        _wp->flags2 |= mask;
    }
}

void yuucounter_flag_off(u32 mask) {
    if (_wp != NULL) {
        _wp->flags2 &= ~mask;
    }
}

s32 yuucounter_flag(s32 mask) {
    if (_wp == NULL) {
        return 0;
    }
    return _wp->flags2 & mask;
}
// clang-format on
