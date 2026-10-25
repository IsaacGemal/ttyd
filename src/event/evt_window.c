#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/winmgr.h"
#include "win/win_main.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "driver/icondrv.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include "driver/msgdrv.h"
#include "memory.h"
#include "system.h"
#include "nameent.h"
#include <dolphin/mtx.h>
#include <string.h>
#include "statuswindow.h"
#include "evt/evt_window.h"

// Locally declared prototypes (move to headers later).

// Work structs allocated by the coin / one-message events.
typedef struct WinCoinWork {
    s32 id;    // 0x0
    s32 count; // 0x4
    s32 tick;  // 0x8
    s32 type;  // 0xC
} WinCoinWork;

typedef struct WinOneMsgWork {
    s32 state;     // 0x0
    s32 msgType;   // 0x4, picks the message (oneMessageDisp) and the sound
    s32 id;        // 0x8
} WinOneMsgWork;

// .sbss
s32 unk_80418b38;

// .sdata
WinMgrDesc one_msg_desc = {
    0, 0,                     // openStyle, frameStyle
    8,                          // camId
    0xFFFFFF51,                 // x
    0x78,                       // y
    0x15E,                      // width
    0x2A,                       // height
    {0xD7, 0xD0, 0x88, 0xFF},   // color
    NULL,                       // main
    (void*)oneMessageDisp,      // disp
};

WinMgrDesc coin_desc = {
    2, 0,                     // openStyle, frameStyle
    8,                          // camId
    0xFFFFFF06,                 // x
    0xFFFFFFB5,                 // y
    0x96,                       // width
    0x2D,                       // height
    {0xF0, 0xF0, 0xF0, 0xFF},   // color
    NULL,                       // main
    (void*)coin_disp,           // disp
};

// NOTE: functions are deliberately written in reverse order. With the build's
// `-inline auto,deferred`, mwcc emits functions in reverse source order, so
// this reproduces the original object's .text layout. The float constants of
// coin_disp are written as literals so the compiler pools them in .sdata2 in
// the original order instead of emitting extra anonymous copies.

USER_FUNC(evt_win_item_select) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);
    WinMgrSelect* select;
    s32 result;
    s32 field;

    if (isFirstCall) {
        event->userdata[0] = (s32)winMgrSelectEntry(value, 0, 1);
        marioKeyOff();
    }
    select = (WinMgrSelect*)event->userdata[0];
    result = winMgrSelect(select);
    if (result == 0) {
        return EVT_RETURN_BLOCK;
    }
    field = winMgrSelectCursor(select);
    winMgrSelectDelete(select);
    marioKeyOn();
    evtSetValue(event, args[2], result);
    if (flag != 0) {
        evtSetValue(event, args[3], field);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_other_select) {
    s32 value = evtGetValue(event, event->args[0]);
    WinMgrSelect* select;
    s32 result;

    if (isFirstCall) {
        event->userdata[0] = (s32)winMgrSelectEntry(value, 0, 1);
        marioKeyOff();
    }
    select = (WinMgrSelect*)event->userdata[0];
    result = winMgrSelectOther(select, event);
    if (result == 0) {
        return EVT_RETURN_BLOCK;
    }
    winMgrSelectDelete(select);
    marioKeyOn();
    if (result == -1) {
        event->lwData[0] = 0;
    } else {
        event->lwData[0] = 1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_item_maketable) {
    s32* args = event->args;
    s32* table = (s32*)evtGetValue(event, args[0]);
    s32* p;
    s32* out;
    s32 count;
    s32 i;
    s32 n;

    count = 0;
    p = table;
    while (TRUE) {
        if (*p == -1) {
            break;
        }
        p++;
        count++;
    }
    if (count == 0) {
        evtSetValue(event, args[1], 0);
        return EVT_RETURN_DONE;
    }
    out = _mapAlloc((count + 1) * 4);
    for (i = 0, n = 0; i < count; i++) {
        if (pouchCheckItem(table[i]) != 0) {
            out[n] = table[i];
            n++;
        }
    }
    out[n] = -1;
    if (n == 0) {
        out = NULL;
    }
    evtSetValue(event, args[1], (s32)out);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_item_desttable) {
    _mapFree((void*)evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_nameent_on) {
    s32 type = evtGetValue(event, event->args[0]);

    if (nameEntPrepare() == 0) {
        return EVT_RETURN_BLOCK;
    }
    nameEntOn(type);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_nameent_wait) {
    return nameEntWait() ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_win_nameent_name) {
    s32* args = event->args;
    char* name = nameEntGetName();

    evtSetValue(event, args[0], (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_nameent_off) {
    nameEntOff();
    return EVT_RETURN_DONE;
}

void oneMessageDisp(WinMgrEntry* entry) {
    WinOneMsgWork* work = entry->param;
    const char* msg;

    if (winMgrAction(work->id) == 0) {
        GXColor color;
        u16 width;

        switch (work->msgType) {
        case 0:
            msg = msgSearch("msg_you_gotta_mail\0");
            break;
        }
        FontDrawStart();
        color = (GXColor){0xFF, 0x00, 0x00, 0xFF};
        FontDrawColor(&color);
        width = FontGetMessageWidth(msg);
        FontDrawMessage(-width / 2, entry->y - 8, msg);
    }
}

USER_FUNC(evt_win_one_message) {
    WinOneMsgWork* work;
    s32 value = evtGetValue(event, event->args[0]);

    if (isFirstCall) {
        work = (WinOneMsgWork*)(event->userdata[0] = (s32)_mapAlloc(0xC));
        memset(work, 0, 0xC);
        work->id = winMgrEntry(&one_msg_desc);
        work->state = 0;
        work->msgType = value;
        winMgrSetParam(work->id, work);
    }
    work = (WinOneMsgWork*)event->userdata[0];
    switch (work->state) {
    case 0:
        winMgrOpen(work->id);
        work->state++;
        break;
    case 1:
        if (winMgrAction(work->id) == 0) {
            work->state++;
        }
        break;
    case 2:
        if (keyGetButtonTrg(0) & 0x100) {
            switch (work->msgType) {
            case 0:
                psndSFXOn_3D("SFX_MAIL_RECEPTION1", &marioGetPtr()->position);
                break;
            }
            work->state++;
        }
        break;
    case 3:
        winMgrClose(work->id);
        work->state++;
        break;
    case 4:
        if (winMgrAction(work->id) == 0) {
            work->state++;
        }
        break;
    case 5:
        winMgrDelete(work->id);
        _mapFree(work);
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

void coin_disp(WinMgrEntry* entry) {
    WinCoinWork* work;
    s32 pouchCount;
    u16 iconId;
    s32 diff;
    Mtx mtx1;
    Mtx mtx2;
    GXColor color;

    pouchGetPtr();
    work = entry->param;
    if (entry->flags & 4) {
        return;
    }
    switch (work->type) {
    case 0:
        pouchCount = pouchGetCoin();
        iconId = 0x18B;
        break;
    case 1:
        pouchCount = pouchGetSuperCoin();
        iconId = 0x102;
        break;
    case 2:
        pouchCount = pouchGetStarPiece();
        iconId = 0x18D;
        break;
    }
    PSMTXTrans(mtx1, -220.0f, -113.0f, 0.0f);
    PSMTXScale(mtx2, 0.7f, 0.7f, 0.7f);
    PSMTXConcat(mtx1, mtx2, mtx1);
    iconDispGx2(mtx1, 0x10, iconId);
    PSMTXTrans(mtx1, -192.0f, -113.0f, 0.0f);
    iconDispGx2(mtx1, 0x10, 0x1D6);
    PSMTXTrans(mtx1, -126.0f, -113.0f, 0.0f);
    color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
    iconNumberDispGx(mtx1, work->count, 0, color);
    diff = pouchCount - work->count;
    if (diff != 0) {
        diff /= 10;
        if (diff == 0) {
            diff = (pouchCount > work->count) ? 1 : -1;
        }
        if (work->tick & 1) {
            work->count += diff;
            statusWinForceUpdateCoin();
        }
        work->tick = work->tick + 1;
        work->tick %= 1200;
        if (unk_80418b38 == 0) {
            psndSFXOn((const char*)0x2005B);
            unk_80418b38 = 7;
        }
    }
    if (unk_80418b38 > 0) {
        unk_80418b38--;
    }
}

USER_FUNC(evt_win_coin_on) {
    s32* args = event->args;
    WinCoinWork* work;
    s32 type;

    work = _mapAlloc(0x10);
    type = evtGetValue(event, args[0]);
    work->type = type;
    work->id = winMgrEntry(&coin_desc);
    work->tick = 0;
    switch (type) {
    case 0:
        work->count = pouchGetCoin();
        break;
    case 1:
        work->count = pouchGetSuperCoin();
        break;
    case 2:
        work->count = pouchGetStarPiece();
        break;
    }
    winMgrSetParam(work->id, work);
    winMgrOpen(work->id);
    evtSetValue(event, args[1], (s32)work);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_coin_off) {
    WinCoinWork* work = (WinCoinWork*)evtGetValue(event, event->args[0]);
    s32 pouchCount;

    switch (work->type) {
    case 0:
        pouchCount = pouchGetCoin();
        break;
    case 1:
        pouchCount = pouchGetSuperCoin();
        break;
    case 2:
        pouchCount = pouchGetStarPiece();
        break;
    }
    if (work->count != pouchCount) {
        return EVT_RETURN_BLOCK;
    }
    winMgrCloseAutoDelete(work->id);
    _mapFree(work);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_win_coin_wait) {
    WinCoinWork* work = (WinCoinWork*)evtGetValue(event, event->args[0]);
    s32 pouchCount;

    switch (work->type) {
    case 0:
        pouchCount = pouchGetCoin();
        break;
    case 1:
        pouchCount = pouchGetSuperCoin();
        break;
    case 2:
        pouchCount = pouchGetStarPiece();
        break;
    }
    return (work->count != pouchCount) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(mainwindow_openenable) {
    winOpenEnable();
    return EVT_RETURN_DONE;
}

USER_FUNC(mainwindow_opendisable) {
    winOpenDisable();
    return EVT_RETURN_DONE;
}
