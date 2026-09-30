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
    return 2;
}

// NONMATCHING (95.65%): 174 of 502 instructions differ: 160 with different operands, 1 replaced, 7 missing, 6 extra
void evt_yuucounter_disp_func(CameraId cameraId, void* param) {
    s32 digit;
    s32 xoff;
    s32 xoff0, xoff1, xoff2, xoff3;
    s32 digits[4];
    s32 digit0, digit1, digit2;
    s32 i0, i1, i2, i3;
    GXColor iconColor;
    s32 icon;
    s32 subsec;
    f32 scale;
    Mtx mtx;
    s32 skip0, skip2;
    GXColor digitColor;
    Mtx mtxScale;
    s32 skip;
    Mtx mtxTemp;
    s32 time;
    s32 i;
    s32 seconds;
    f32 xbase;
    GXColor color;
    YuuCounterWork* work;
    work = param;
    color = *(GXColor*)&dat_yuu_00014dd0;
    iconColor = *(GXColor*)&dat_yuu_00014dd0;
    digitColor = *(GXColor*)&dat_yuu_00014dd0;

    switch (work->mode) {
    case 0:
        if (!(work->flags2 & 2)) {
            work->time = (s32)(200.0f * (1300.0f + marioGetPtr()->position.x));
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
        s32 val0 = seconds;
        digits[0] = val0 / 100;
        val0 %= 100;
        digits[1] = val0 / 10;
        digits[2] = val0 % 10;

        skip0 = 1;
        xoff0 = 0;
        for (i0 = 0; i0 < 3; i0++) {
            digit0 = digits[i0];
            if (digit0 != 0 || i0 == 2) {
                skip0 = 0;
            }
            if (!skip0) {
                PSMTXTrans(mtxTemp, xbase + (f32)xoff0, 8.0f, 0.0f);
                PSMTXConcat(mtx, mtxTemp, mtxTemp);
                iconDispGxCol(mtxTemp, 0x10, (u16)(digit0 + 0x1F7), digitColor);
            }
            xoff0 += 30;
        }
        break;
    }
    case 1: {
        s32 val1 = seconds;
        digits[0] = val1 / 60;
        val1 %= 60;
        digits[1] = val1 / 10;
        digits[2] = val1 % 10;

        xoff1 = 0;
        for (i1 = 0; i1 < 3; i1++) {
            digit1 = digits[i1];
            PSMTXTrans(mtxTemp, xbase + (f32)xoff1 - (f32)((i1 == 0) << 4), 8.0f, 0.0f);
            PSMTXConcat(mtx, mtxTemp, mtxTemp);
            iconDispGxCol(mtxTemp, 0x10, (u16)(digit1 + 0x1F7), digitColor);
            xoff1 += 30;
        }

        PSMTXTrans(mtxTemp, 8.0f + xbase, 12.0f, 0.0f);
        PSMTXScale(mtxScale, 2.0f, 2.0f, 2.0f);
        PSMTXConcat(mtxTemp, mtxScale, mtxTemp);
        PSMTXConcat(mtx, mtxTemp, mtxTemp);
        iconDispGxCol(mtxTemp, 0x10, (u16)icon, iconColor);
        break;
    }
    case 2: {
        s32 val2 = time + 1;
        val2 %= 100000;
        digits[0] = val2 / 1000;
        val2 %= 1000;
        digits[1] = val2 / 100;
        val2 %= 100;
        digits[2] = val2 / 10;
        digits[3] = val2 % 10;

        skip2 = 1;
        xoff2 = 0;
        for (i2 = 0; i2 < 4; i2++) {
            digit2 = digits[i2];
            if (digit2 != 0 || i2 == 3) {
                skip2 = 0;
            }
            if (!skip2) {
                PSMTXTrans(mtxTemp, xbase + (f32)xoff2, 8.0f, 0.0f);
                PSMTXConcat(mtx, mtxTemp, mtxTemp);
                iconDispGxCol(mtxTemp, 0x10, (u16)(digit2 + 0x1F7), digitColor);
            }
            xoff2 += 30;
        }
        break;
    }
    }

    if (icon != -1) {
        PSMTXTrans(mtxTemp, 90.0f + xbase - 12.0f, 10.0f, 0.0f);
        PSMTXConcat(mtx, mtxTemp, mtxTemp);
        iconDispGxCol(mtxTemp, 0x10, (u16)icon, iconColor);
    }

    switch (work->mode) {
    case 0:
    case 1:
        digits[0] = subsec / 100;
        digits[1] = subsec % 100 / 10;
        xoff3 = 0;
        for (i3 = 0; i3 < 2; i3++) {
            PSMTXTrans(mtxTemp, 90.0f + xbase + (f32)xoff3, 8.0f, 0.0f);
            PSMTXScale(mtxScale, 0.5f, 0.5f, 0.5f);
            PSMTXConcat(mtxTemp, mtxScale, mtxTemp);
            PSMTXConcat(mtx, mtxTemp, mtxTemp);
            iconDispGxCol(mtxTemp, 0x10, (u16)(digits[i3] + 0x1F7), digitColor);
            xoff3 += 15;
        }
        break;
    }
}

USER_FUNC(evt_yuucounter_main_func) {
    YuuCounterWork* work = (YuuCounterWork*)event->lwData[0];

    if (work->flags & 1) {
        work->flags |= 2;
        return 2;
    }
    dispEntry(CAMERA_2D, 1000.0f, 0, evt_yuucounter_disp_func, work);
    return 0;
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
        unk_8022439c((s32)unk_evt_yuu_00019728);
    } else {
        unk_8022439c((s32)unk_evt_yuu_00019740);
    }
    return 2;
}

USER_FUNC(evt_yuucounter_free) {
    YuuCounterWork* work = _wp;

    if (work == NULL) {
        return 2;
    }
    if (isFirstCall != 0) {
        work->flags |= 1;
    }
    if (!(work->flags & 2)) {
        return 0;
    }
    _mapFree(_wp);
    _wp = NULL;
    return 2;
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
