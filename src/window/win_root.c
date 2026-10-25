#include "pmario_sound.h"
#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include <dolphin/types.h>
#include "math_ext.h"
#include <string.h>
#include "driver/mapdrv.h"
#include "system.h"
#include "window/win_root.h"
#include "data/mail_data.h"
#include "window/win_item.h"
#include "window/win_badge.h"
#include "window/win_mario.h"
#include "window/win_party.h"
#include "window/win_log.h"
#include "battle/battle_monosiri.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_lottery.h"
#include <stdio.h>


typedef struct SortTable {
    const char* name;                 // 0x0
    void (*func)(WindowWork* work);    // 0x4
} SortTable;

#define LOGWIN(w) ((LogWin*)(w))

extern ItemData itemDataTable[];

extern SortTable* sort_data[8];

// The tattle log's enemy order (win_log.c reads it), indexed by unit kind.
u8 enemy_monoshiri_sort_table[0xE8] = {
    0x00, 0x01, 0x02, 0x03, 0x2F, 0x30, 0x94, 0x2A, 0x62, 0x95, 0x96, 0x12, 0x24, 0x24, 0x0A, 0x0B,
    0x32, 0x14, 0x4D, 0x54, 0x33, 0x32, 0x15, 0x63, 0x43, 0x38, 0x4F, 0x3E, 0x6B, 0x6E, 0x6F, 0x79,
    0x7A, 0x7B, 0x75, 0x99, 0x01, 0x0C, 0x0D, 0x36, 0x1B, 0x1D, 0x4E, 0x56, 0x2C, 0x2D, 0x23, 0x0E,
    0x0F, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x2B, 0x18, 0x19, 0x1A, 0x5A, 0x13, 0x50, 0x51, 0x29,
    0x66, 0x67, 0x04, 0x05, 0x06, 0x3C, 0x3D, 0x52, 0x1F, 0x20, 0x40, 0x47, 0x48, 0x68, 0x9A, 0x9B,
    0x9C, 0x9D, 0x9E, 0x9F, 0x4A, 0x4B, 0x34, 0x35, 0x39, 0x21, 0x5B, 0x5C, 0x57, 0x69, 0xA0, 0xA1,
    0xA2, 0xA3, 0xA4, 0x77, 0x6B, 0x6B, 0x6B, 0x44, 0x37, 0x22, 0x49, 0x6A, 0xA5, 0xA6, 0xA7, 0xA8,
    0x45, 0x3A, 0x53, 0x70, 0x71, 0x72, 0x73, 0x6C, 0x6D, 0x76, 0xA9, 0x41, 0x4C, 0x5D, 0x5E, 0x59,
    0x5F, 0x5F, 0x16, 0x17, 0x64, 0x79, 0x7A, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
    0x29, 0x28, 0x78, 0x74, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x07, 0x08, 0x09, 0x10, 0x11, 0x2E, 0x1C,
    0x1E, 0x60, 0x3B, 0x31, 0x3F, 0x55, 0x46, 0x42, 0x58, 0x61, 0x61, 0x65, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0x00,
};


// The per-quad texture setup/scale of the kirinuki window: same shape as
// nameent.c's NAME_KIRINUKI_SCALE. `yscale` is a local of the caller, shared
// across the quads like nameKirinukiGX's.
#define WIN_KIRINUKI_SCALE(id_, W, H) \
    { \
        GXTexObj texObj; \
        Mtx mtx; \
        f32 xs; \
        f64 hh; \
        f64 ww; \
        TEXGetGXTexObjFromPalette(*work->winTpl->data, &texObj, (id_)); \
        GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0); \
        GXLoadTexObj(&texObj, 0); \
        GXSetNumTexGens(1); \
        GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D); \
        hh = __fabs(H); \
        ww = __fabs(W); \
        yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
        xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj); \
        PSMTXScale(mtx, xs, yscale, 1.0f); \
        GXLoadTexMtxImm(mtx, 0x1E, 1); \
    }


#define WIN_MAIL_TEX(w) (*(w)->winTpl->data)

// The loop both comparators run: find `tag` in the 26-entry table, formatting each
// candidate into `buf`. Written as an inlined helper because inlining it twice is
// what reproduces the target's callee-saved register allocation (table base/pointer
// in r26/r27, tag/counter in r28/r29).

// ---- winHakoGX ------------------------------------------------------------
// The item/badge container window ("hako" = box). It draws a scrolling grid of
// item tiles, an overlay of item icons, and the bottom status bar. Every frame
// piece is an inlined winBookQuad.

// One piece of winWazaGX's frame: a textured quad from (x,y) to (x+w,y-h). The
// texture width is named (`tw`) before the width division, as MWCC colours retail's
// callee-saved FPRs only with it.
static inline void wazaQuad(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette(*work->winTpl->data, &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), (f32)sy / (f32)GXGetTexObjHeight(&texObj), 1.0f);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// ---- winSortGX ------------------------------------------------------------

// Draws one textured 1x1-uv quad. The original inlines this at every call site,
// which is why each panel gets its own GXTexObj/Mtx stack slots.
static inline void winSortPanel(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f64 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette(*work->winTpl->data, &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj),
               (f32)sy / (f32)GXGetTexObjHeight(&texObj), 1.0f);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - (f32)h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - (f32)h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

#define WIN_BOOK_TEX(w) (*(w)->winTpl->data)
extern const char** luigi_book_list[];

// Per-tab slide-in/slide-out animation. winRootTabStep is a plain static defined after winRootMain: MWCC
// inlines it and still generates its body first, so its 185.0f pools ahead of winRootMain's 1000.0f in
// .sdata2 (the linker strips the unreferenced body).
static void winRootTabStep(WinRootTab* tab, s32 i);
static inline void winRootTabMain(WindowWork* work) {
    s32 i;

    for (i = 0; i < 5; i++) {
        winRootTabStep(&work->tabs[i], i);
    }
}


// Main menu. Builds the visible tab set into the work block, then runs the
// per-state logic and the common per-tab animation. The string array is the
// unit's static msg_help object.
s32 winRootMain(WindowWork* work) {
    static const char* msg_help[5] = {
        "msg_menu_mario", "msg_menu_nakama", "msg_menu_mochimono", "msg_menu_badge", "msg_menu_kiroku",
    };
    s32 result;
    WinRootTab* tab;
    s32 j;
    const char* str;

    switch (work->rootState) {
    case 0:
        psndSFXOn((const char*)0x2002A);
        work->tabCount = 0;
        tab = &work->tabs[work->tabCount];
        tab->id = 0;
        tab->state = 0;
        tab->y = 1000.0f;
        tab->x = 1000.0f;
        tab->str = msg_help[0];
        work->tabCount++;
        if (swByteGet(0) >= 7 || partyChkJoin(1) != 0) {
            tab = &work->tabs[work->tabCount];
            tab->id = 1;
            tab->state = 0;
            tab->y = 1000.0f;
            tab->x = 1000.0f;
            tab->str = msg_help[1];
            work->tabCount++;
        }
        tab = &work->tabs[work->tabCount];
        tab->id = 2;
        tab->state = 0;
        tab->y = 1000.0f;
        tab->x = 1000.0f;
        tab->str = msg_help[2];
        work->tabCount++;
        if (swByteGet(0) >= 0x14 || pouchGetHaveBadgeCnt() != 0) {
            tab = &work->tabs[work->tabCount];
            tab->id = 3;
            tab->state = 0;
            tab->y = 1000.0f;
            tab->x = 1000.0f;
            tab->str = msg_help[3];
            work->tabCount++;
        }
        tab = &work->tabs[work->tabCount];
        tab->id = 4;
        tab->state = 0;
        tab->y = 1000.0f;
        tab->x = 1000.0f;
        tab->str = msg_help[4];
        work->tabCount++;
        for (j = 0; j < work->tabCount; j++) {
            if (work->tabs[j].id == work->openTabId) {
                work->tabSel = j;
            }
        }
        work->recs[0].state = -1;
        work->recs[0].y = 1000.0f;
        work->recs[0].x = 1000.0f;
        work->recs[0].tabId = -1;
        work->recs[1].state = -1;
        work->recs[1].y = 1000.0f;
        work->recs[1].x = 1000.0f;
        work->recs[1].tabId = -1;
        work->recs[2].state = -1;
        work->recs[2].y = 1000.0f;
        work->recs[2].x = 1000.0f;
        work->recs[2].tabId = -1;
        work->recs[3].state = -1;
        work->recs[3].y = 1000.0f;
        work->recs[3].x = 1000.0f;
        work->recs[3].tabId = -1;
        work->recs[4].state = -1;
        work->recs[4].y = 1000.0f;
        work->recs[4].x = 1000.0f;
        work->recs[4].tabId = -1;
        work->recs[0].y = 1000.0f;
        work->recs[0].x = 1000.0f;
        work->recs[0].state = 0;
        work->recs[0].tabId = work->tabs[work->tabSel].id;
        work->msgState = 0;
        work->msgY = 1000.0f;
        work->msgX = 1000.0f;
        work->msgTag = NULL;
        work->nameTag = NULL;
        work->lastMsgTag = NULL;
        winMarioInit(work);
        winPartyInit(work);
        winItemInit(work);
        winBadgeInit(work);
        winLogInit(work);
        {
            f32 pos = (f32)(work->tabSel * 0x69 - 0x10E);

            work->targetX = pos;
            work->posX = pos;
        }
        work->posY = 300.0f;
        work->targetY = 185.0f;
        work->sortTargetX = -1000.0f;
        work->sortX = -1000.0f;
        work->sortTargetY = -1000.0f;
        work->sortY = -1000.0f;
        str = work->tabs[work->tabSel].str;
        winMsgEntry(work, 0, str, NULL);
        work->rootState = 0xA;
        switch (work->openTabId) {
        case 1:
            winPartyInit2(work);
            work->rootState = 0xC;
            work->flags |= 0x4000;
            break;
        case 2:
            winItemInit2(work);
            work->rootState = 0xD;
            work->flags |= 0x4000;
            break;
        case 3:
            winBadgeInit2(work);
            work->rootState = 0xE;
            work->flags |= 0x4000;
            break;
        case 4:
            winLogInit2(work);
            work->rootState = 0xF;
            work->flags |= 0x4000;
            break;
        }
        if (gp->flags & 0x1000) {
            work->tabCount = 0;
            tab = &work->tabs[work->tabCount];
            tab->id = 1;
            tab->state = 0;
            tab->y = 1000.0f;
            tab->x = 1000.0f;
            tab->str = msg_help[1];
            work->tabCount++;
            winPartyInit2(work);
            work->rootState = 0xC;
            work->tabSel = 0;
        }
        work->fbCopy = smartAlloc(GXGetTexBufferSize(gp->fbWidth, gp->efbHeight, 4, 0, 0), 1);
        work->fbCopyReady = 0;
        dispEntry(CAMERA_2D, 1000.0f, 0, (DispCallback)winRootFbCopy, work);
        break;
    case 10:
        if (work->buttonTrg & 0x1200) {
            if (work->recs[0].state == 0x64) {
                work->rootState = 0x14;
            }
            break;
        }
        if (work->buttonTrg & 0x100) {
            if (work->tabs[work->tabSel].id != 1 || work->partyCount != 0) {
                psndSFXOn((const char*)0x20012);
                work->rootState = work->tabs[work->tabSel].id + 0xB;
                switch (work->tabs[work->tabSel].id) {
                case 0:
                    winMarioInit2(work);
                    break;
                case 1:
                    winPartyInit2(work);
                    break;
                case 2:
                    winItemInit2(work);
                    break;
                case 3:
                    winBadgeInit2(work);
                    break;
                case 4:
                    winLogInit2(work);
                    break;
                }
            }
            break;
        }
        if (work->dirRep & 0x4000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            work->tabSel--;
            if (work->tabSel < 0) {
                work->tabSel = work->tabCount - 1;
            }
            for (j = 4; j > 0; j--) {
                work->recs[j] = work->recs[j - 1];
            }
            work->recs[0].y = 1000.0f;
            work->recs[0].x = 1000.0f;
            work->recs[0].state = 0x1E;
            work->recs[0].tabId = work->tabs[work->tabSel].id;
            work->targetX = (f32)(work->tabSel * 0x69 - 0x10E);
            str = work->tabs[work->tabSel].str;
            winMsgEntry(work, 0, str, NULL);
        } else if (work->dirRep & 0x8000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            work->tabSel++;
            if (work->tabSel >= work->tabCount) {
                work->tabSel = 0;
            }
            for (j = 4; j > 0; j--) {
                work->recs[j] = work->recs[j - 1];
            }
            work->recs[0].y = 1000.0f;
            work->recs[0].x = 1000.0f;
            work->recs[0].state = 0x14;
            work->recs[0].tabId = work->tabs[work->tabSel].id;
            work->targetX = (f32)(work->tabSel * 0x69 - 0x10E);
            str = work->tabs[work->tabSel].str;
            winMsgEntry(work, 0, str, NULL);
        }
        break;
    case 100:
        if (work->recs[work->tabSel].state == 0x64) {
            work->rootState = 0xA;
        }
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        switch (work->rootState) {
        case 11:
            result = winMarioMain(work);
            break;
        case 12:
            result = winPartyMain(work);
            break;
        case 13:
            result = winItemMain(work);
            break;
        case 14:
            result = winBadgeMain(work);
            break;
        case 15:
            result = winLogMain(work);
            break;
        }
        if ((gp->flags & 0x1000) && result == -1) {
            result = -2;
        }
        if (result == -1) {
            work->targetX = (f32)(work->tabSel * 0x69 - 0x10E);
            work->targetY = 185.0f;
            work->rootState = 0xA;
            str = work->tabs[work->tabSel].str;
            winMsgEntry(work, 0, str, NULL);
            work->flags &= ~0x4000;
        } else if (result == -2) {
            work->rootState = 0x14;
            work->flags &= ~0x4000;
        }
        break;
    case 20:
        psndSFXOn((const char*)0x2002B);
        for (j = 0; j < work->tabCount; j++) {
            work->tabs[j].state = 0xA;
        }
        work->recs[0].state = 0xA;
        work->recs[1].state = 0xA;
        work->recs[2].state = 0xA;
        work->recs[3].state = 0xA;
        work->recs[4].state = 0xA;
        work->msgState = 0xA;
        N_mapDispOn();
        camDispOn(CAMERA_3D);
        work->rootState++;
        break;
    default:
        if (work->fbCopy != NULL) {
            smartFree(work->fbCopy);
            work->fbCopy = NULL;
            work->fbCopyReady = 0;
        }
        work->rootState++;
        if (work->rootState > 0x32) {
            winMarioExit(work);
            winPartyExit(work);
            winItemExit(work);
            winBadgeExit(work);
            winLogExit(work);
            if (work->partyChange != 0 && work->partySel != work->lastPartySel) {
                return -2;
            }
            if (work->useItemId != 0) {
                return -3;
            }
            return -1;
        }
        break;
    }

    winRootTabMain(work);
    winBgMain(work);
    winMsgMain(work);
    winMarioMain2(work);
    winPartyMain2(work);
    winItemMain2(work);
    winBadgeMain2(work);
    winLogMain2(work);
    winSortMain(work);
    if (work->rootState < 0x14) {
        if (work->flags & 0x4000) {
            work->posX += (work->targetX - work->posX) / 4.0f;
            work->posY += (work->targetY - work->posY) / 4.0f;
        } else {
            work->posX += (work->targetX - work->posX) / 4.0f;
            work->posY += (work->targetY + (work->tabs[work->tabSel].y - 185.0f) - work->posY) / 4.0f;
        }
    } else if (work->posY > 180.0f) {
            work->posX += (work->targetX - work->posX) / 4.0f;
            work->posY += (work->targetY + (work->tabs[work->tabSel].y - 185.0f) - work->posY) / 4.0f;
    } else {
            work->posX += (work->targetX + work->recs[0].x - work->posX) / 4.0f;
            work->posY += (work->targetY - work->posY) / 4.0f;
    }
    work->posX = (f32)(work->posX +
        __fabs(2.0f * (f32)sin((f32)gp->retraceCount / 8.0f)));
    work->sortX += (work->sortTargetX - work->sortX) / 4.0f;
    work->sortY += (work->sortTargetY - work->sortY) / 4.0f;
    dispEntry(CAMERA_2D, 1000.0f, 0, (DispCallback)winRootFbDraw, work);
    return 0;
}



// The tab slide-in/out animation run every frame.
static void winRootTabStep(WinRootTab* tab, s32 i) {
    switch (tab->state) {
    case 0:
        tab->x = (f32)(i * 0x69 - 0xCD);
        tab->y = 300.0f;
        tab->counter = i * 4;
        tab->state++;
        break;
    case 1:
    case 11: {
        s32 c = tab->counter - 1;

        tab->counter = c;
        if (c < 0) {
            tab->counter = 0;
            tab->state++;
        }
        break;
    }
    case 2: {
        s32 c = tab->counter;

        tab->counter = c + 1;
        tab->y = intplGetValue(0xB, c, 0xA, 300.0f, 185.0f);
        if (tab->counter > 0xA) {
            tab->state++;
        }
        break;
    }
    case 10:
        tab->counter = (4 - i) * 4;
        tab->state++;
        break;
    case 12: {
        s32 c = tab->counter;

        tab->counter = c + 1;
        tab->y = intplGetValue(0xB, c, 0xA, 180.0f, 300.0f);
        if (tab->counter > 0xA) {
            tab->state++;
        }
        break;
    }
    case 13:
        break;
    }
}

static inline void winRootTabDisp(WindowWork* work);

void winRootDisp(CameraId camId, WindowWork* work) {
    GXColor bgcolor = {0xFF, 0xFF, 0xFF, 0xFF};
    s32 i;

    if (work->rootState == 0) {
        return;
    }

    for (i = 4; i >= 0; i--) {
        WinRootRec* e = &work->recs[i];
        s32 state = e->state;

        if (state == -1) {
            continue;
        }
        if (i > 0 && work->recs[i - 1].state == 0x64) {
            continue;
        }
        if (i > 0 && (u32)(state - 0xA) <= 1) {
            continue;
        }

        switch (e->tabId) {
            case 0:
                winMarioDisp(camId, work, i);
                break;
            case 1:
                winPartyDisp(camId, work, i);
                break;
            case 2:
                winItemDisp(camId, work, i);
                break;
            case 3:
                winBadgeDisp(camId, work, i);
                break;
            case 4:
                winLogDisp(camId, work, i);
                break;
        }
    }

    winRootTabDisp(work);

    winMailDisp(work);
    winMsgDisp(work, work->msgX, work->msgY);
    winSortGX(work);

    if (!(work->flags & 0x8000)) {
        winTexInit(*work->winTpl->data);

        winTexSet(0, (Vec){2.0f + work->posX, work->posY - 2.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                  (GXColor){0x00, 0x00, 0x00, 0x80});
        winTexSet(0, (Vec){work->posX, work->posY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, bgcolor);
    }
}

// The menu tabs, the selected one bright.
static inline void winRootTabDisp(WindowWork* work) {
    s32 j;

    for (j = 0; j < work->tabCount; j++) {
        WinRootTab* tab = &work->tabs[j];
        GXColor color;

        if (work->tabSel == j) {
            color = (GXColor){0xFF, 0xFF, 0xFF, 0xFE};
        } else {
            color = (GXColor){0x80, 0x80, 0x80, 0xFE};
        }
        winTexInit_x2(*work->winTpl->data);
        winTexSet_x2(tab->id + 1, 0xB2, (Vec){tab->x, tab->y, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, color);
    }
}

// Switch over each of the 5 recs[i].state. The explicit empty case -1 below (a redirect
// target of cases 11 and 41) is what makes MWCC pick 0 as the [0,10) search-tree
// pivot; without it the tree pivots on 1.
void winBgMain(WindowWork* work) {
    s32 i;

    for (i = 0; i < 5; i++) {
        switch (work->recs[i].state) {
            case 0:
                work->recs[i].x = 800.0f;
                work->recs[i].y = 0.0f;
                work->recs[i].step = 0;
                work->recs[i].state = work->recs[i].state + 1;
                break;
            case 1: {
                s32 step = work->recs[i].step;
                work->recs[i].step = step + 1;
                work->recs[i].x = intplGetValue(0xB, step, 0x10, 800.0f, 0.0f);
                if (work->recs[i].step > 0x10) {
                    work->recs[i].state = 0x64;
                }
                break;
            }
            case 10:
                work->recs[i].y = 0.0f;
                work->recs[i].step = 0;
                work->recs[i].state = work->recs[i].state + 1;
                break;
            case 11: {
                s32 step = work->recs[i].step;
                work->recs[i].step = step + 1;
                work->recs[i].x = intplGetValue(0xB, step, 0x10, 0.0f, 800.0f);
                if (work->recs[i].step > 0x10) {
                    work->recs[i].state = -1;
                }
                break;
            }
            case 20:
                work->recs[i].y = 0.0f;
                work->recs[i].step = 0;
                work->recs[i].state = work->recs[i].state + 1;
                break;
            case 21: {
                s32 step = work->recs[i].step;
                work->recs[i].step = step + 1;
                work->recs[i].x = intplGetValue(0xB, step, 0x18, 640.0f, 0.0f);
                if (work->recs[i].step > 0x18) {
                    work->recs[i].state = 0x64;
                }
                break;
            }
            case 30:
                work->recs[i].y = 0.0f;
                work->recs[i].step = 0;
                work->recs[i].state = work->recs[i].state + 1;
                break;
            case 31: {
                s32 step = work->recs[i].step;
                work->recs[i].step = step + 1;
                work->recs[i].x = intplGetValue(0xB, step, 0x18, -640.0f, 0.0f);
                if (work->recs[i].step > 0x18) {
                    work->recs[i].state = 0x64;
                }
                break;
            }
            case 40:
                work->recs[i].y = 0.0f;
                work->recs[i].step = 0;
                work->recs[i].state = work->recs[i].state + 1;
                break;
            case 41: {
                s32 step = work->recs[i].step;
                work->recs[i].step = step + 1;
                work->recs[i].x = intplGetValue(0xB, step, 0x18, 0.0f, -640.0f);
                if (work->recs[i].step > 0x18) {
                    work->recs[i].state = -1;
                }
                break;
            }
            case -1:
            case 100:
                break;
            default:
                break;
        }
    }
}

void winBgGX(WindowWork* work, s32 type, f32 x, f32 y) {
    GXTexObj texObj;
    Mtx mtx;
    f32 scaleY;

    switch (type) {
        case 0:
            GXSetTevColor(1, (GXColor){0xC8, 0x66, 0x60, 0xFF});
            GXSetTevColor(2, (GXColor){0xE1, 0xA5, 0xA1, 0xFF});
            GXSetTevColor(3, (GXColor){0xF0, 0xCD, 0xCB, 0xFF});
            break;
        case 1:
            GXSetTevColor(1, (GXColor){0xC8, 0x6B, 0x96, 0xFF});
            GXSetTevColor(2, (GXColor){0xDB, 0xAB, 0xC6, 0xFF});
            GXSetTevColor(3, (GXColor){0xEC, 0xD1, 0xE1, 0xFF});
            break;
        case 2:
            GXSetTevColor(1, (GXColor){0xBB, 0x9C, 0x21, 0xFF});
            GXSetTevColor(2, (GXColor){0xD7, 0xCD, 0x76, 0xFF});
            GXSetTevColor(3, (GXColor){0xEA, 0xE5, 0xAD, 0xFF});
            break;
        case 3:
            GXSetTevColor(1, (GXColor){0x2F, 0xA5, 0x30, 0xFF});
            GXSetTevColor(2, (GXColor){0xA2, 0xD2, 0x6E, 0xFF});
            GXSetTevColor(3, (GXColor){0xCB, 0xE7, 0xA7, 0xFF});
            break;
        case 4:
            GXSetTevColor(1, (GXColor){0x28, 0x7E, 0xC5, 0xFF});
            GXSetTevColor(2, (GXColor){0x78, 0xB3, 0xD6, 0xFF});
            GXSetTevColor(3, (GXColor){0xAF, 0xD6, 0xEA, 0xFF});
            break;
    }

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetTevSwapMode(0, 0, 0);

    PSMTXTrans(mtx, x, 18.0f + y, 0.0f);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    TEXGetGXTexObjFromPalette(*work->winTpl->data, &texObj, 7);
    GXInitTexObjLOD(&texObj, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = 320.0f / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, 570.0f / (f32)GXGetTexObjWidth(&texObj), scaleY, 1.0f);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(-285.0f, 160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-285.0f, -160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.0f, -160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.0f, 160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(0.0f, 160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(0.0f, -160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(285.0f, -160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(285.0f, 160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    TEXGetGXTexObjFromPalette(*work->winTpl->data, &texObj, 6);
    GXInitTexObjLOD(&texObj, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = 288.0f / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, 570.0f / (f32)GXGetTexObjWidth(&texObj), scaleY, 1.0f);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXSetTevColorIn(0, 4, 6, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 3);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(-285.0f, 144.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-285.0f, -144.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(285.0f, -144.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(285.0f, 144.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winMsgMain(WindowWork* work) {
    switch (work->msgState) {
        case 0:
            work->msgX = -800.0f;
            work->msgY = -170.0f;
            work->msgStep = 0;
            work->msgState = work->msgState + 1;
            break;
        case 1: {
            s32 step = work->msgStep;
            work->msgStep = step + 1;
            work->msgX = intplGetValue(0xB, step, 0x10, -800.0f, 0.0f);
            if (work->msgStep > 0x10) {
                work->msgState = work->msgState + 1;
            }
            break;
        }
        case 2:
            if (keyGetDirTrg(0) & 0x100) {
                work->msgLine = work->msgLine - 2;
                if (work->msgLine < 0) {
                    work->msgLine = 0;
                } else {
                    psndSFXOn((const char*)0x20006);
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if (work->msgLine / 2 < (work->msgLines + 1) / 2 - 1) {
                    work->msgLine = work->msgLine + 2;
                    psndSFXOn((const char*)0x20006);
                }
            }
            break;
        case 10:
            work->msgStep = 0;
            work->msgState = work->msgState + 1;
            break;
        case 11: {
            s32 step = work->msgStep;
            work->msgStep = step + 1;
            work->msgX = intplGetValue(0xB, step, 0x10, 0.0f, -800.0f);
            if (work->msgStep > 0x10) {
                work->msgState = work->msgState + 1;
            }
            break;
        }
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 12:
        default:
            break;
    }
}

// Most of the 10/40/120/240/250 position sums stay literals: MWCC pools each literal at the named
// object's slot, so the code is the same either way (objdiff reports only a symbol-only diff); a couple
// of sites read the named objects directly.
// The 0x140 pointer is loaded once and shared by the NULL test and msgSearch; ty, ix and width are
// function-scope floats, and ty and fy are assigned inside the call arguments that use them.
void winMsgDisp(WindowWork* work, f32 x, f32 y) {
    char buf[0x400];
    GXColor iconColor = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
    GXColor fontColor = (GXColor){0x00, 0x00, 0x00, 0xFF};
#ifdef MUST_MATCH
    // Semantically x - 250.0f; the original computes it as +10 then -10.
    f32 fx = (10.0f + (x - 250.0f)) - 10.0f;
#else
    f32 fx = x - 250.0f;
#endif
    f32 fy = 40.0f + y;
    f32 ty;
    f32 ix;
    f32 width;

    windowDispGX_Waku_col(fx, fy, 500.0f, 80.0f, 20.0f, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});

#ifdef MUST_MATCH
    // The `!= 0` test is implied by `> 0`, but it changes the object.
    if (work->msgItemId != 0 && work->msgItemId > 0) {
#else
    if (work->msgItemId > 0) {
#endif
        winTexInit(*work->winTpl->data);
        winTexSet(0x25, (Vec){ fx, fy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor);
        winIconInit();
        winIconSet(itemDataTable[work->msgItemId].icon_id, (Vec){ fx, fy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor);
    }

    if (work->nameTag != NULL) {
        const char* msg;
        f32 edge;

        edge = (work->msgItemId == -2) ? -130.0f : 0.0f;
        msg = msgSearch(work->nameTag);
        width = FontGetMessageWidth(msg);
        winNameGX(work, 1, (x - 120.0f) + edge, ty = 8.0f + (16.0f + fy), 240.0f, 32.0f);
        winFontInit();
        winFontSetEdge((Vec){ edge + ((240.0f - width) / 2.0f + (x - 120.0f)), ty - 4.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor, msg);
    }

    if (work->msgTag != NULL) {
        GXSetScissor(0, (u32)(240.0f - (fy -= 14.0f)), 0x260, 0x38);
        if (strcmp(work->msgTag, "msg_lottery_number") == 0) {
            sprintf(buf, msgSearch(work->msgTag), lotteryGetPtr()->mTicketNum);
            winFontInit();
            winFontSet((Vec){ (40.0f + (x - 250.0f)) - 10.0f, fy + (f32)(work->msgLine * 0x1C), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, fontColor, buf);
        } else {
            winFontInit();
            winFontSetLabel((Vec){ (40.0f + (x - 250.0f)) - 10.0f, fy + (f32)(work->msgLine * 0x1C), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, fontColor, work->msgTag);
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }

    if (work->msgLines > 2) {
        s32 i;

        winIconInit();
        i = work->msgLine;
        if (i == 0) {
            if ((gp->retraceCount & 0x1F) < 0x14) winIconSet(0x1B6, (Vec){ (250.0f + x) - 10.0f, (40.0f + (y - 40.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        } else if (i / 2 == (work->msgLines + 1) / 2 - 1) {
            if ((gp->retraceCount & 0x1F) < 0x14) winIconSet(0x1B5, (Vec){ (250.0f + x) - 10.0f, 24.0f + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        } else if ((gp->retraceCount & 0x1F) < 0x14) {
            ix = (250.0f + x) - 10.0f;
            winIconSet(0x1B5, (Vec){ ix, 24.0f + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            winIconSet(0x1B6, (Vec){ ix, (40.0f + (y - 40.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        }
        winIconSet(0x70, (Vec){ (250.0f + x) - 10.0f, 40.0f + (y - 40.0f), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
}

void winMsgEntry(WindowWork* work, s32 a1, const char* str, const char* a3) {
    static char _tmp[0x40];
    u16 width;
    BOOL changed = FALSE;

    if (str != NULL) {
        if (work->lastMsgTag != str) {
            changed = TRUE;
        } else if (strcmp(_tmp, str) != 0) {
            changed = TRUE;
        }
        if (changed) {
            work->msgItemId = a1;
            work->msgTag = str;
            work->nameTag = a3;
            FontGetMessageWidthLine(msgSearch(work->msgTag), &width);
            work->msgLine = 0;
            work->msgLines = width + 1;
            work->lastMsgTag = str;
            strncpy(_tmp, str, 0x3C);
        }
    }
}

#define WIN_KIRINUKI_VTX(px, py, s, t) \
    GXPosition3f32((px), (py), 0.0f); \
    GXTexCoord2f32((s), (t))

// The 9920-byte body reconstructed from the asm; the frame (0x8b0, saving f19..f31) matches. The quads are written
// out (the shape of nameent.c's nameKirinukiGX): each derived edge is a named local computed right after the
// GXBegin of the first quad that uses it and reused by the later quads; `size`/`size16` hold the tile sizes, which
// gives the original's operand order (a literal goes on the left of `+`) and keeps the sign quad's `signX + 16`
// apart from the bar's `16 + signX`. Each branch has its own block locals. Every texture id (if0 B,9,D,A,8,C,8,E;
// else B,9,D,A,8,C,8,F,8,A,8,C,8,E), scale and rectangle was checked against the asm.
void winKirinukiGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    f32 size = 8.0f;
    f32 size16 = 16.0f;
    f32 barW = 74.0f;
    f32 bar2W = 66.0f;
    f32 yscale;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f,
             (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    if (type == 0) {
        f32 midX;
        f32 midW;
        f32 midR;
        f32 rightX;
        f32 rightR;
        f32 midH;
        f32 botY;
        f32 botB;

        WIN_KIRINUKI_SCALE(0xB, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(x, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, y - size, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y - size, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x9, midW = w - size - size, 8.0);
        GXBegin(0x80, 0, 4);
        midX = size + x;
        midR = midX + midW;
        WIN_KIRINUKI_VTX(midX, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(midX, y - size, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, y - size, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, y, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xD, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        rightX = x + w - size;
        rightR = rightX + size;
        WIN_KIRINUKI_VTX(rightX, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rightX, y - size, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, y - size, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, y, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xA, 8.0, midH = h - size - size);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(x, y - size, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, y - size - midH, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y - size - midH, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y - size, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, w - size, midH);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(midX, y - size, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(midX, y - size - midH, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(midX + (w - size), y - size - midH, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(midX + (w - size), y - size, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xC, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        botY = y - (h - size);
        botB = botY - size;
        WIN_KIRINUKI_VTX(x, botY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, botB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, botB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, botY, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, midW, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(midX, botY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(midX, botB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, botB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, botY, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xE, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(rightX, botY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rightX, botB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, botB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, botY, 1.0f, 0.0f);
    } else {
        f32 midX;
        f32 rightX;
        f32 rightR;
        f32 midR;
        f32 midH;
        f32 midB;
        f32 botY;
        f32 botB;
        f32 signX;
        f32 signY;
        f32 signB;
        f32 barX;
        f32 barR;
        f32 edgeX;
        f32 row2B;
        f32 edgeR;
        f32 y3;
        f32 y3b;

        WIN_KIRINUKI_SCALE(0xB, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(x, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, y - size, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y - size, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x9, w - size - size, 8.0);
        GXBegin(0x80, 0, 4);
        midX = size + x;
        WIN_KIRINUKI_VTX(midX, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(midX, y - size, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(midX + (w - size - size), y - size, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(midX + (w - size - size), y, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xD, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        rightX = x + w - size;
        rightR = rightX + size;
        WIN_KIRINUKI_VTX(rightX, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rightX, y - size, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, y - size, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, y, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xA, 8.0, midH = h - size - size);
        GXBegin(0x80, 0, 4);
        midB = y - size - midH;
        WIN_KIRINUKI_VTX(x, y - size, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, midB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, midB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, y - size, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, w - size, midH);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(midX, y - size, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(midX, midB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX((midR = midX + (w - size)), midB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, y - size, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xC, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        botY = y - (h - size);
        botB = botY - size;
        WIN_KIRINUKI_VTX(x, botY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, botB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, botB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + size, botY, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, w - size, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(midX, botY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(midX, botB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, botB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(midR, botY, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xF, 16.0, 8.0);
        GXBegin(0x80, 0, 4);
        signX = x + (w - 90.0f);
        signY = y - h;
        signB = signY - size;
        WIN_KIRINUKI_VTX(signX, signY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(signX, signB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(signX + size16, signB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(signX + size16, signY, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, barW, 8.0);
        GXBegin(0x80, 0, 4);
        barX = 16.0f + signX;
        barR = barX + barW;
        WIN_KIRINUKI_VTX(barX, signY, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(barX, signB, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(barR, signB, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(barR, signY, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xA, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        edgeX = size + signX;
        row2B = signB - size;
        edgeR = edgeX + size;
        WIN_KIRINUKI_VTX(edgeX, signB, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(edgeX, row2B, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(edgeR, row2B, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(edgeR, signB, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, barW, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(barX, signB, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(barX, row2B, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(barR, row2B, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(barR, signB, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xC, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        y3 = signY - 16.0f;
        y3b = y3 - size;
        WIN_KIRINUKI_VTX(edgeX, y3, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(edgeX, y3b, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(edgeR, y3b, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(edgeR, y3, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0x8, bar2W, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(barX, y3, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(barX, y3b, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(barX + bar2W, y3b, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(barX + bar2W, y3, 1.0f, 0.0f);

        WIN_KIRINUKI_SCALE(0xE, 8.0, 8.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(rightX, y3, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rightX, y3b, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, y3b, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rightR, y3, 1.0f, 0.0f);
    }
}


// The name plate used by win_mario's partner menu: two quads per type, written out like winKirinukiGX's.
// type 0/1 are the 32px-tall bar with a 16px cap on the right, type 2 the 24px bar with an 8px cap.
// `yscale` is shared by all the quads (the scale macro's caller local), and each branch has its own
// cap `size` and right-cap edges.
void winNameGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    f32 yscale;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    if (type == 0) {
        f32 rx;
        f32 rr;
        f32 size = 16.0f;
        WIN_KIRINUKI_SCALE(0x13, w - size, 32.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(x, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, y - 32.0f, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + (w - size), y - 32.0f, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + (w - size), y, 1.0f, 0.0f);
        WIN_KIRINUKI_SCALE(0x14, 16.0, 32.0);
        GXBegin(0x80, 0, 4);
        rx = x + w - size;
        rr = rx + size;
        WIN_KIRINUKI_VTX(rx, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rx, y - 32.0f, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rr, y - 32.0f, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rr, y, 1.0f, 0.0f);
    } else if (type == 1) {
        f32 rx;
        f32 rr;
        f32 size = 16.0f;
        WIN_KIRINUKI_SCALE(0x15, w - size, 32.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(x, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, y - 32.0f, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + (w - size), y - 32.0f, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + (w - size), y, 1.0f, 0.0f);
        WIN_KIRINUKI_SCALE(0x16, 16.0, 32.0);
        GXBegin(0x80, 0, 4);
        rx = x + w - size;
        rr = rx + size;
        WIN_KIRINUKI_VTX(rx, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rx, y - 32.0f, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rr, y - 32.0f, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rr, y, 1.0f, 0.0f);
    } else if (type == 2) {
        f32 rx;
        f32 rr;
        f32 size = 8.0f;
        WIN_KIRINUKI_SCALE(0x78, w - size, 24.0);
        GXBegin(0x80, 0, 4);
        WIN_KIRINUKI_VTX(x, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(x, y - 24.0f, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + (w - size), y - 24.0f, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(x + (w - size), y, 1.0f, 0.0f);
        WIN_KIRINUKI_SCALE(0x79, 8.0, 24.0);
        GXBegin(0x80, 0, 4);
        rx = x + w - size;
        rr = rx + size;
        WIN_KIRINUKI_VTX(rx, y, 0.0f, 0.0f);
        WIN_KIRINUKI_VTX(rx, y - 24.0f, 0.0f, 1.0f);
        WIN_KIRINUKI_VTX(rr, y - 24.0f, 1.0f, 1.0f);
        WIN_KIRINUKI_VTX(rr, y, 1.0f, 0.0f);
    }
}

// One textured piece of a book frame: (x, y) top-left, w x h, the texture stretched across it.
static inline void winBookQuad(TEXPalette* pal, s32 texId, f32 x, f32 y, f64 w, f64 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette(pal, &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), (f32)sy / (f32)GXGetTexObjHeight(&texObj), 1.0f);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - (f32)h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + (f32)w, y - (f32)h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + (f32)w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winBookGX(WindowWork* work, s32 type, f32 x, f32 y) {
    f32 yofs;
    s32 i;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    yofs = type == 0 ? 0.0f : 32.0f;

    // the page area and the top edge
    winBookQuad(WIN_BOOK_TEX(work), 0x33, x + 8.0f, y - 8.0f, 432.0, 256.0f - yofs);
    winBookQuad(WIN_BOOK_TEX(work), 0x28, x, y, 16.0, 8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x2C, x + 16.0f, y, 184.0, 8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x26, x + 16.0f + 184.0f, y, 48.0, 16.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x2D, x + 16.0f + 184.0f + 48.0f, y, 184.0, 8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x2A, x + 16.0f + 184.0f + 48.0f + 184.0f, y, 16.0, 8.0);

    // the sides and the spine
    winBookQuad(WIN_BOOK_TEX(work), 0x30, x, y - 8.0f, 8.0, 256.0f - yofs);
    winBookQuad(WIN_BOOK_TEX(work), 0x32, x + 16.0f + 184.0f + 8.0f, y - 16.0f, 16.0, 248.0f - yofs);
    winBookQuad(WIN_BOOK_TEX(work), 0x31, x + 16.0f + 184.0f + 48.0f + 184.0f + 8.0f, y - 8.0f, 8.0, 256.0f - yofs);

    // the bottom edge
    winBookQuad(WIN_BOOK_TEX(work), 0x29, x, y - 8.0f - 256.0f + yofs, 16.0, 16.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x2E, x + 16.0f, y - 8.0f - 256.0f + yofs, 184.0, 16.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x27, x + 16.0f + 184.0f, y - 8.0f - 256.0f + yofs, 48.0, 24.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x2F, x + 16.0f + 184.0f + 48.0f, y - 8.0f - 256.0f + yofs, 184.0, 16.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x2B, x + 16.0f + 184.0f + 48.0f + 184.0f, y - 8.0f - 256.0f + yofs, 16.0, 16.0);

    if (type == 1) {
        for (i = 0; i < 8; i++) {
            winBookQuad(WIN_BOOK_TEX(work), 0x34, x + 16.0f + (f32)((i % 2) * 216), y - 32.0f - (f32)((i / 2) * 48), 192.0, 32.0);
        }
    }

    if (type == 0) {
        s32 pose;
        AnimData* anim;
        f32 labelX;

        winFontInit();
        labelX = 20.0f + x;
        winFontSetLabel((Vec){labelX, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, (GXColor){0x00, 0x00, 0x00, 0xFF},
                        luigi_book_list[work->bookId][work->bookPage * 2]);
        winFontSetLabel((Vec){220.0f + labelX, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f},
                        (GXColor){0x00, 0x00, 0x00, 0xFF},
                        luigi_book_list[work->bookId][work->bookPage * 2 + 1]);

        // Luigi, reading: the closing pose on the last page
        pose = L_msgGetWork()->poseId;
        anim = animPoseGetAnimDataPtr(pose);
        if (work->bookPage < work->bookPages - 1) {
            animPoseSetAnim(pose, "HOS_C_1", 0);
        } else {
            animPoseSetAnim(pose, "HOS_E_1", 0);
        }
        animPoseSetLocalTime(pose, gp->retraceCount % (s32)anim->clip->end);
        animPoseMain(pose);
        animPoseDraw(pose, 425.0f + x, y - 275.0f, 0.0f, 180.0f, 5.0f, 2);
    }
}

void winHalfBookGX(TEXPalette* pal, f32 x, f32 y, s32 a4) {
    Mtx mtx;
    s32 i;

    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            GXSetChanMatColor(4, (GXColor){0x00, 0x00, 0x00, 0x80});
            GXSetBlendMode(1, 4, 5, 0);
            GXSetZCompLoc(1);
            GXSetAlphaCompare(7, 0, 0, 7, 0);
            GXSetZMode(0, 7, 0);
            PSMTXTrans(mtx, 8.0f, -8.0f, 0.0f);
            PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx, mtx);
            GXLoadPosMtxImm(mtx, 0);
            GXSetCurrentMtx(0);
        } else {
            GXSetChanMatColor(4, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            GXSetBlendMode(0, 1, 0, 0);
            GXSetZCompLoc(0);
            GXSetAlphaCompare(6, 0x80, 1, 0, 0);
            GXSetZMode(0, 7, 0);
            GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
            GXSetCurrentMtx(0);
        }

        winBookQuad(pal, 0x33, x + 8.0f, y - 8.0f, 320.0, 248.0);
        winBookQuad(pal, 0x28, x, y, 16.0, 8.0);
        winBookQuad(pal, 0x35, x + 16.0f, y, 248.0, 8.0);
        winBookQuad(pal, 0x26, x + 16.0f + 248.0f, y, 48.0, 16.0);
        winBookQuad(pal, 0x3B, x + 16.0f + 248.0f + 48.0f, y, 24.0, 16.0);
        winBookQuad(pal, 0x30, x, y - 8.0f, 8.0, 248.0);
        winBookQuad(pal, 0x32, x + 16.0f + 248.0f + 8.0f, y - 16.0f, 16.0, 240.0);
        winBookQuad(pal, 0x36, x + 16.0f, y - 200.0f, 248.0, 40.0);
        winBookQuad(pal, 0x3A, x, y - 256.0f, 16.0, 8.0);
        winBookQuad(pal, 0x38, x + 32.0f, y - 256.0f, -16.0, 8.0);
        winBookQuad(pal, 0x38, x + 32.0f, y - 256.0f, 240.0, 8.0);
        winBookQuad(pal, 0x39, x + 272.0f, y - 256.0f, 16.0, 8.0);
        winBookQuad(pal, 0x38, x + 288.0f, y - 256.0f, 16.0, 8.0);
        winBookQuad(pal, 0x38, x + 320.0f, y - 256.0f, -16.0, 8.0);
        winBookQuad(pal, 0x38, x + 320.0f, y - 256.0f, 16.0, 8.0);
        winBookQuad(pal, 0x3C, x + 328.0f, y - 16.0f, 8.0, 240.0);
    }
}

// The mail window frame: nineteen textured quads (outer border and inner panels), each
// an inlined winBookQuad; the drawn rectangles are f32 sums of the caller's x/y.
void winMailGX(WindowWork* work, f32 x, f32 y) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    winBookQuad(WIN_BOOK_TEX(work), 0x4D, x, y, 32.0f, 24.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x63, x + 32.0f, y, 344.0f, 24.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x64, x + 200.0f, y, 16.0f, 24.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x55, x + 32.0f, y - 24.0f, 344.0f, 8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x4E, 16.0f + (40.0f + (112.0f + (16.0f + (48.0f + (40.0f + (72.0f + (x + 32.0f))))))), y, 40.0f, 24.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x51, x, y - 24.0f, 16.0f, 248.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x55, x + 16.0f, y - 24.0f, 16.0f, 248.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x55, 16.0f + (248.0f + (8.0f + (72.0f + (16.0f + (16.0f + x))))), y - 24.0f, 16.0f, 248.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x51, 16.0f + (16.0f + (248.0f + (8.0f + (72.0f + (16.0f + (16.0f + x)))))), y - 24.0f, 16.0f, 248.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x52, 16.0f + (16.0f + (16.0f + (248.0f + (8.0f + (72.0f + (16.0f + (16.0f + x))))))), y - 24.0f, 8.0f, 248.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x5C, x + 32.0f, y - 32.0f, 344.0f, 240.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x55, x + 32.0f, y - 248.0f, 344.0f, 24.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x4F, x, y - 272.0f, 32.0f, 32.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x53, x + 32.0f, y - 272.0f, 344.0f, 32.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x50, 344.0f + (x + 32.0f), y - 272.0f, 40.0f, 32.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x54, x + 16.0f, (y - 24.0f) - 8.0f, 8.0f, -8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x54, 8.0f + (384.0f + x), (y - 24.0f) - 8.0f, -8.0f, -8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x54, x + 16.0f, y - 264.0f, 8.0f, 8.0);
    winBookQuad(WIN_BOOK_TEX(work), 0x54, 8.0f + (384.0f + x), y - 264.0f, -8.0f, 8.0);

    GXSetScissor((u32)(32.0f + (304.0f + x)), (u32)(1.0f + (32.0f + (240.0f - y))), 0x158, 0xC8);
}

// Section 1 draws the scrolling page of the box. Its visibility test uses `row`, which
// the row loops leave at 4; on page 0 the original reads it before any write (retail's
// `mulli r4, r25, 0x38` at the top of the page loop), so the natural build sets it to 0.
// The icon, texture and font calls take their Vecs and colours by value, built as compound literals.
// Tile ids are written `type * 13 + 0xNN` at each use and the page count `(count + 0x1B) / 28` at each
// loop bound; MWCC hoists both out of the loops, which is where retail's twelve ids and page count come from.
void winHakoGX(WindowWork* work, s32 type, f32 x, f32 y) {
    f32 scroll;
#ifdef MUST_MATCH
    s32 row; // the original reads it in page 0's visibility test before the row loop sets it
#else
    s32 row = 0;
#endif
    s32 count;
    s32 page;
    s32 cursor;
    s32 texBase;
    s32 col;
    s32 id;

    if (type == 0) {
        scroll = LOGWIN(work)->cardScrollPos;
        count = LOGWIN(work)->cardCnt;
        cursor = LOGWIN(work)->cardPage;
    } else {
        scroll = LOGWIN(work)->badgeScrollPos;
        count = LOGWIN(work)->badgeCnt;
        cursor = LOGWIN(work)->badgePage;
    }

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);
    GXSetScissor((u32)(304.0f + x), (u32)(240.0f - y), 0x190, 0xE4);

    texBase = type * 0xD;
    for (page = 0; page < (count + 0x1B) / 28; page++) {
        f32 vis = (scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8);

        if (vis > 400.0f || vis < -400.0f) {
            continue;
        }
        if ((texBase != 0 && page == 2) || (texBase != 1 && page == 3)) {
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x7B, x, (y + scroll) - (f32)(page * 0xE8), 56.0f, 56.0f);
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x7D, x, ((y - 56.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x7E, x + 8.0f, ((y - 56.0f) + scroll) - (f32)(page * 0xE8), 48.0f, 8.0f);
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x85, x + 56.0f, (y + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x84, x + 56.0f, ((y - 8.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 48.0f);
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x81, x + 56.0f, ((y - 56.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                break; // retail leaves the page loop once the special page is drawn (b .L_80154348 at 0x8015386C)
            } else {

                for (col = 0; col < 7; col++) {
                    for (row = 0; row < 4; row++) {
                        winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x7B + ((col + 7 * row) & 1),
                                 x + (f32)(col * 0x38),
                                 (scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8),
                                 56.0f, 56.0f);
                    }
                }
                for (col = 0; col < 7; col++) {
                    if (col == 0) {
                        id = type * 13 + 0x7D;
                    } else if (col & 1) {
                        id = type * 13 + 0x7F;
                    } else {
                        id = type * 13 + 0x86;
                    }
                    winBookQuad(WIN_BOOK_TEX(work), id, x + (f32)(col * 0x38), ((y - 224.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                    if ((col & 1) == 0) {
                        id = type * 13 + 0x7E;
                    } else {
                        id = type * 13 + 0x80;
                    }
                    winBookQuad(WIN_BOOK_TEX(work), id, x + (f32)(col * 0x38) + 8.0f, ((y - 224.0f) + scroll) - (f32)(page * 0xE8), 48.0f, 8.0f);
                }
                for (col = 0; col < 4; col++) {
                    if (col == 0) {
                        id = type * 13 + 0x85;
                    } else if (col & 1) {
                        id = type * 13 + 0x83;
                    } else {
                        id = type * 13 + 0x87;
                    }
                    winBookQuad(WIN_BOOK_TEX(work), id, x + 392.0f, (scroll + (y - (f32)(col * 0x38))) - (f32)(page * 0xE8), 8.0f, 8.0f);
                    if ((col & 1) == 0) {
                        id = type * 13 + 0x84;
                    } else {
                        id = type * 13 + 0x82;
                    }
                    winBookQuad(WIN_BOOK_TEX(work), id, x + 392.0f, (scroll + ((y - (f32)(col * 0x38)) - 8.0f)) - (f32)(page * 0xE8), 8.0f, 48.0f);
                }
                winBookQuad(WIN_BOOK_TEX(work), type * 13 + 0x81, x + 392.0f, ((y - 224.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
            }
    }

    // Section 2: the empty-slot tiles. Drawn when the item is not yet obtained.
    winTexInit(*work->winTpl->data);
    for (page = 0; page < (count + 0x1B) / 28; page++) {
        f32 vis2 = (scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8);
        if (vis2 > 400.0f || vis2 < -400.0f) {
            continue;
        }
        for (row = 0; row < 4; row++) {
            for (col = 0; col < 7; col++) {
                s32 idx = page * 0x1C + row * 7 + col;
                if (idx < count) {
                    if (texBase == 0) {
                        s16 itemId = LOGWIN(work)->cardIcon[idx];
                        if (swGet(itemId + 0x80) != 0) {
                            continue;
                        }
                    } else {
                        s16 itemId = LOGWIN(work)->badgeIcon[idx];
                        if (swGet(itemId + 0x41) != 0) {
                            continue;
                        }
                    }
                    winTexSet(0x97, (Vec){((x + (f32)(col * 0x38)) + 24.0f) + 4.0f, ((scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8)) - 28.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            }
        }
    }

    // Section 3: the item icons, drawn when the item is obtained.
    winIconInit();
    for (page = 0; page < (count + 0x1B) / 28; page++) {
        for (row = 0; row < 4; row++) {
            for (col = 0; col < 7; col++) {
                s32 idx = page * 0x1C + row * 7 + col;
                if (idx < count) {
                    s32 iconId;

                    if (texBase == 0) {
                        if (swGet(LOGWIN(work)->cardIcon[idx] + 0x80) == 0) {
                            continue;
                        }
                        iconId = itemDataTable[LOGWIN(work)->cardIcon[idx] + 0xF0].icon_id;
                    } else {
                        if (swGet(LOGWIN(work)->badgeIcon[idx] + 0x41) == 0) {
                            continue;
                        }
                        iconId = itemDataTable[LOGWIN(work)->badgeIcon[idx] + 0xB3].icon_id;
                    }
                    winIconSet(iconId, (Vec){((x + (f32)(col * 0x38)) + 24.0f) + 4.0f, ((scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8)) - 28.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            }
        }
    }

    // Bottom status bar.
    GXSetScissor(0, 0, 0x260, 0x1E0);
    if (cursor > 0) {
        f32 bx;

        winTexInit(*work->winTpl->data);
        bx = 395.0f + x;
        winTexSet(0x17, (Vec){bx, 15.0f + y, 0.0f}, (Vec){-1.0f, -1.0f, -1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        winIconInit();
        winIconSet(0x7E, (Vec){bx, y, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
    if (count > cursor * 0x1C + 0x1C) {
        f32 bx;

        winTexInit(*work->winTpl->data);
        bx = 395.0f + x;
        winTexSet(0x17, (Vec){bx, y - 235.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        winIconInit();
        winIconSet(0x80, (Vec){bx, y - 220.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
    {
        const char* msg;
        f32 w0;
        f32 fx;

        msg = msgSearch("msg_menu_sort_narabikae");
        w0 = 0.7f * (f32)FontGetMessageWidth(msg);
        winFontInit();
        fx = 380.0f - w0;
        winFontSet((Vec){x + fx, y - 228.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, (GXColor){0x00, 0x00, 0x00, 0xFF}, msg);
        winIconInit();
        winIconSet(0x217, (Vec){(x + fx) - 30.0f, y - 237.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
}

// The waza window frame: eleven textured quads (a 3x3 border plus inner strips),
// each an inlined wazaQuad. They come in three rows: the top bar at y (height
// 32), the body at y-32 (height h-48), and the bottom bar at y-h+16 (height 8/16).
// Pool-base offsets now reproduce retail; this function matches at 100%.
void winWazaGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    {
    f32 w75 = 0.75f * w;
    wazaQuad(work, 0x98, x, y, w75, 32.0f);
    wazaQuad(work, 0x9A, x + w75, y, (w - w75) - 16.0f, 32.0f);
    wazaQuad(work, 0x9B, x + (w - 16.0f), y, 16.0f, 32.0f);
    wazaQuad(work, 0xA2, x, y - 32.0f, w75, (h - 32.0f) - 16.0f);
    wazaQuad(work, 0x9E, x + w75, y - 32.0f, (w - w75) - 8.0f, (h - 32.0f) - 16.0f);
    wazaQuad(work, 0xA1, x + (w - 8.0f), y - 32.0f, 8.0f, (h - 32.0f) - 16.0f);
    wazaQuad(work, 0x9E, x + w75, y - (h - 16.0f), (w - w75) - 8.0f, 8.0f);
    wazaQuad(work, 0xA1, x + (w - 8.0f), y - (h - 16.0f), 8.0f, 8.0f);
    wazaQuad(work, 0x9C, x, y - (h - 16.0f), w75, 16.0f);
    wazaQuad(work, 0x9F, x + w75, y - (h - 8.0f), (w - w75) - 16.0f, 8.0f);
    wazaQuad(work, 0x9D, x + (w - 16.0f), y - (h - 16.0f), 16.0f, 16.0f);
    }
}

void sort_1_1_func(WindowWork* work) {
    s32 item = work->itemSel[0];
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    {
        s32 type = 7;
        if (work->sortReverseNext == 0) {
            type = 0;
        }
        pouchSortItem(type);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeSkipList(work);
    work->itemSel[0] = 0;
    work->itemScrollRow[0] = work->itemSel[0] / 10;
}

void sort_1_2_func(WindowWork* work) {
    s32 item = work->itemSel[0];
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    pouchSortItem(work->sortReverseNext == 0 ? 1 : 8);
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeSkipList(work);
    work->itemSel[0] = 0;
    work->itemScrollRow[0] = work->itemSel[0] / 10;
}

void sort_2_1_func(WindowWork* work) {
    pouchSortItem(work->sortReverseNext == 0 ? 2 : 9);
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeSkipList(work);
    work->itemSel[1] = 0;
    work->itemScrollRow[1] = work->itemSel[1] / 10;
}

void sort_2_2_func(WindowWork* work) {
    pouchSortItem(work->sortReverseNext == 0 ? 3 : 10);
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeSkipList(work);
    work->itemSel[1] = 0;
    work->itemScrollRow[1] = work->itemSel[1] / 10;
}

void sort_3_1_func(WindowWork* work) {
    s32 badge = work->badgeSel[0];
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(work->sortReverseNext == 0 ? 4 : 11);
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeEquipList(work);
    work->badgeSel[0] = 0;
    work->badgeScrollRow[0] = work->badgeSel[0] / 8;
}

void sort_3_2_func(WindowWork* work) {
    s32 badge = work->badgeSel[0];
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(work->sortReverseNext == 0 ? 5 : 12);
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeEquipList(work);
    work->badgeSel[0] = 0;
    work->badgeScrollRow[0] = work->badgeSel[0] / 8;
}

void sort_3_3_func(WindowWork* work) {
    s32 badge = work->badgeSel[0];
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(work->sortReverseNext == 0 ? 6 : 13);
    pouchSortItem(work->sortReverseNext == 0 ? 6 : 13);
    work->sortReverseNext = 1 - work->sortReverseNext;
    winMakeEquipList(work);
    work->badgeSel[0] = 0;
    work->badgeScrollRow[0] = work->badgeSel[0] / 8;
}

// 1:1 (the `const` parameters reproduce the original's hoisted `lwz r4,0x8(r4)` prologue load).
int N_compare_func4_1(const void* a, const void* b) {
    const WinEquipRec* pa = a;
    const WinEquipRec* pb = b;
    char buf_a[128];
    char buf_b[128];
    s16 id_b = (s16)pb->badgeId;
    s16 id_a = (s16)pa->badgeId;

    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_a].name), buf_a);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_b].name), buf_b);
    return strcmp(buf_a, buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as N_compare_func4_1).
int N_compare_func4_1_r(const void* a, const void* b) {
    const WinEquipRec* pa = a;
    const WinEquipRec* pb = b;
    char buf_a[128];
    char buf_b[128];
    s16 id_b = (s16)pb->badgeId;
    s16 id_a = (s16)pa->badgeId;

    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_a].name), buf_a);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_b].name), buf_b);
    return strcmp(buf_b, buf_a);
}

int N_compare_func4_2(const void* a, const void* b) {
    const WinEquipRec* pa = a;
    const WinEquipRec* pb = b;
    s32 id_a = (s16)pa->badgeId;
    s32 id_b = (s16)pb->badgeId;
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int N_compare_func4_2_r(const void* a, const void* b) {
    const WinEquipRec* pa = a;
    const WinEquipRec* pb = b;
    s32 id_a = (s16)pa->badgeId;
    s32 id_b = (s16)pb->badgeId;
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_4_1_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(work->equipList, work->equipListCount, 0xC, N_compare_func4_1);
    } else {
        qqsort(work->equipList, work->equipListCount, 0xC, N_compare_func4_1_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    work->badgeSel[1] = 0;
    work->badgeScrollRow[1] = work->badgeSel[1] / 8;
}

void sort_4_2_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(work->equipList, work->equipListCount, 0xC, N_compare_func4_2);
    } else {
        qqsort(work->equipList, work->equipListCount, 0xC, N_compare_func4_2_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    work->badgeSel[1] = 0;
    work->badgeScrollRow[1] = work->badgeSel[1] / 8;
}

// Bubble sort of the equipped-badge list by BP cost. Indexing the records
// (`equipList[i]`) rather than walking `pa`/`pb` pointers is what makes MWCC
// strength-reduce them to the target's pa=r4/pb=r12 induction variables and colour
// the item-table base r11; the pointer form leaves those in r9/r10/r11. The record
// copy `t = *pa; *pa = *pb; *pb = t;` reproduces the original's interleaved
// load/store sequence and its stack spills at 0x14/0x18/0x1c exactly.
void sort_4_3_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        s32 ida;
        s32 idb;
        s32 i;
        s32 j;
        s32 swap;

        for (i = 0; i < work->equipListCount - 1; i++) {
            j = i + 1;
            while (j < work->equipListCount) {
                ida = (s16)work->equipList[i].badgeId;
                idb = (s16)work->equipList[j].badgeId;
                swap = 0;

                if (itemDataTable[ida].bp_cost > itemDataTable[idb].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ida].bp_cost == itemDataTable[idb].bp_cost && itemDataTable[ida].type_sort_order > itemDataTable[idb].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    WinEquipRec t = work->equipList[i];
                    work->equipList[i] = work->equipList[j];
                    work->equipList[j] = t;
                }
                j++;
            }
        }
    } else {
        s32 ida;
        s32 idb;
        s32 i;
        s32 j;
        s32 swap;

        for (i = 0; i < work->equipListCount - 1; i++) {
            j = i + 1;
            while (j < work->equipListCount) {
                ida = (s16)work->equipList[i].badgeId;
                idb = (s16)work->equipList[j].badgeId;
                swap = 0;

                if (itemDataTable[ida].bp_cost < itemDataTable[idb].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ida].bp_cost == itemDataTable[idb].bp_cost && itemDataTable[ida].type_sort_order < itemDataTable[idb].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    WinEquipRec t = work->equipList[i];
                    work->equipList[i] = work->equipList[j];
                    work->equipList[j] = t;
                }
                j++;
            }
        }
    }

    work->sortReverseNext = 1 - work->sortReverseNext;
    work->badgeSel[1] = 0;
    work->badgeScrollRow[1] = work->badgeSel[1] / 8;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lha` prologue load).
int compare_func5_1(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    char buf_a[128];
    char buf_b[128];
    s16 id_a = (s16)(*pa + 0xF0);
    s16 id_b = (s16)(*pb + 0xF0);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_a].name), buf_a);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_b].name), buf_b);
    return strcmp(buf_a, buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func5_1).
int compare_func5_1_r(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    char buf_a[128];
    char buf_b[128];
    s16 id_a = (s16)(*pa + 0xF0);
    s16 id_b = (s16)(*pb + 0xF0);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_a].name), buf_a);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_b].name), buf_b);
    return strcmp(buf_b, buf_a);
}

int compare_func5_2(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    s16 id_a = (s16)(*pa + 0xF0);
    s16 id_b = (s16)(*pb + 0xF0);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_2_r(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    s16 id_a = (s16)(*pa + 0xF0);
    s16 id_b = (s16)(*pb + 0xF0);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_5_1_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(LOGWIN(work)->cardIcon, LOGWIN(work)->cardCnt, 2, compare_func5_1);
    } else {
        qqsort(LOGWIN(work)->cardIcon, LOGWIN(work)->cardCnt, 2, compare_func5_1_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->cardSel = 0;
    LOGWIN(work)->cardPage = LOGWIN(work)->cardSel / 28;
}

void sort_5_2_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(LOGWIN(work)->cardIcon, LOGWIN(work)->cardCnt, 2, compare_func5_2);
    } else {
        qqsort(LOGWIN(work)->cardIcon, LOGWIN(work)->cardCnt, 2, compare_func5_2_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->cardSel = 0;
    LOGWIN(work)->cardPage = LOGWIN(work)->cardSel / 28;
}

// Same bubble sort as sort_4_3 but over LogWin.cardIcon (itemDataTable index = entry + 0xF0).
// Indexing the array (`cardIcon[i]`) rather than walking `pa`/`pb` pointers
// lets MWCC strength-reduce them to the same pa=r7/pb=r6
// induction variables the target uses. The per-branch declaration order of the
// locals reproduces the target's register colours (j=r31/swap=r30 in the ascending
// branch, swap=r30/j=r31 in the descending one); the orders differ between the two
// branches only for that reason.
void sort_5_3_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        s16 la;
        s16 lb;
        s32 i;
        s32 j;
        s16 ia;
        s16 ib;
        s32 swap;

        for (i = 0; i < LOGWIN(work)->cardCnt - 1; i++) {
            j = i + 1;
            while (j < LOGWIN(work)->cardCnt) {
                la = LOGWIN(work)->cardIcon[i];
                lb = LOGWIN(work)->cardIcon[j];
                ia = (s16)((s32)la + 0xF0);
                ib = (s16)((s32)lb + 0xF0);
                swap = 0;

                if (itemDataTable[ia].bp_cost > itemDataTable[ib].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ia].bp_cost == itemDataTable[ib].bp_cost && itemDataTable[ia].type_sort_order > itemDataTable[ib].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    LOGWIN(work)->cardIcon[i] = lb;
                    LOGWIN(work)->cardIcon[j] = la;
                }
                j++;
            }
        }
    } else {
        s16 la;
        s16 lb;
        s32 i;
        s32 swap;
        s32 j;
        s16 ia;
        s16 ib;

        for (i = 0; i < LOGWIN(work)->cardCnt - 1; i++) {
            j = i + 1;
            while (j < LOGWIN(work)->cardCnt) {
                la = LOGWIN(work)->cardIcon[i];
                lb = LOGWIN(work)->cardIcon[j];
                ia = (s16)((s32)la + 0xF0);
                ib = (s16)((s32)lb + 0xF0);
                swap = 0;

                if (itemDataTable[ia].bp_cost < itemDataTable[ib].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ia].bp_cost == itemDataTable[ib].bp_cost && itemDataTable[ia].type_sort_order < itemDataTable[ib].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    LOGWIN(work)->cardIcon[i] = lb;
                    LOGWIN(work)->cardIcon[j] = la;
                }
                j++;
            }
        }
    }

    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->cardSel = 0;
    LOGWIN(work)->cardPage = LOGWIN(work)->cardSel / 28;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lha` prologue load).
int compare_func6_1(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    char buf_a[128];
    char buf_b[128];
    s16 id_a = (s16)(*pa + 0xB3);
    s16 id_b = (s16)(*pb + 0xB3);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_a].name), buf_a);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_b].name), buf_b);
    return strcmp(buf_a, buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func6_1).
int compare_func6_1_r(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    char buf_a[128];
    char buf_b[128];
    s16 id_a = (s16)(*pa + 0xB3);
    s16 id_b = (s16)(*pb + 0xB3);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_a].name), buf_a);
    pouchHiraganaToKatakana(msgSearch(itemDataTable[id_b].name), buf_b);
    return strcmp(buf_b, buf_a);
}

int compare_func6_2(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    s16 id_a = (s16)(*pa + 0xB3);
    s16 id_b = (s16)(*pb + 0xB3);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func6_2_r(const void* a, const void* b) {
    const s16* pa = a;
    const s16* pb = b;
    s16 id_a = (s16)(*pa + 0xB3);
    s16 id_b = (s16)(*pb + 0xB3);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_6_1_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(LOGWIN(work)->badgeIcon, LOGWIN(work)->badgeCnt, 2, compare_func6_1);
    } else {
        qqsort(LOGWIN(work)->badgeIcon, LOGWIN(work)->badgeCnt, 2, compare_func6_1_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->badgeSel = 0;
    LOGWIN(work)->badgePage = LOGWIN(work)->badgeSel / 28;
}

void sort_6_2_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(LOGWIN(work)->badgeIcon, LOGWIN(work)->badgeCnt, 2, compare_func6_2);
    } else {
        qqsort(LOGWIN(work)->badgeIcon, LOGWIN(work)->badgeCnt, 2, compare_func6_2_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->badgeSel = 0;
    LOGWIN(work)->badgePage = LOGWIN(work)->badgeSel / 28;
}

int compare_func1(const void* a, const void* b) {
    const u8* pa = a;
    const u8* pb = b;

    if (*pa > *pb) {
        return 1;
    } else if (*pa < *pb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func1_r(const void* a, const void* b) {
    const u8* pa = a;
    const u8* pb = b;
    if (*pa < *pb) {
        return 1;
    } else if (*pa > *pb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_7_1_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(LOGWIN(work)->chars, LOGWIN(work)->charCnt, 2, compare_func1);
    } else {
        qqsort(LOGWIN(work)->chars, LOGWIN(work)->charCnt, 2, compare_func1_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->charSel = 0;
    LOGWIN(work)->charPage = LOGWIN(work)->charSel / 16;
}

static inline s32 winRootFindTag(const char* tag, char* buf) {
    static const char* tbl[] = {
        "nara", "gor",  "tik", "shiga", "hei", "nok", "stn", "gon", "win",
        "hou",  "mri",  "tou", "usu",   "gra", "jin", "muj", "dou", "hom",
        "eki",  "rsh",  "pik", "sin",   "bom", "moo", "aji", "las",
    };
    s32 i;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf(buf, "menu_monosiri_%s", tbl[i]);
        if (strcmp(buf, tag) == 0) {
            break;
        }
    }
    if ((u32)i >= 0x1A) {
        i = 0;
    }
    return i;
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
int compare_func2(const void* a, const void* b) {
    const LogChar* pa = a;
    const LogChar* pb = b;
    char buf_a[64];
    char buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(pa->id);
    TattleEntry* eb = battleGetUnitMonosiriPtr(pb->id);
    s32 i = winRootFindTag(ea->locationTag, buf_a);
    s32 j = winRootFindTag(eb->locationTag, buf_b);

    if (i > j) {
        return 1;
    } else if (i < j) {
        return -1;
    } else {
        return 0;
    }
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
int compare_func2_r(const void* a, const void* b) {
    const LogChar* pa = a;
    const LogChar* pb = b;
    char buf_a[64];
    char buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(pa->id);
    TattleEntry* eb = battleGetUnitMonosiriPtr(pb->id);
    s32 i = winRootFindTag(ea->locationTag, buf_a);
    s32 j = winRootFindTag(eb->locationTag, buf_b);

    if (i < j) {
        return 1;
    } else if (i > j) {
        return -1;
    } else {
        return 0;
    }
}

void sort_7_2_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func1);
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func2);
    } else {
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func1_r);
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func2_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->charSel = 0;
    LOGWIN(work)->charPage = LOGWIN(work)->charSel / 16;
}

int compare_func3(const void* a, const void* b) {
    const LogChar* pa = a;
    const LogChar* pb = b;
    char buf_a[128];
    char buf_b[128];
    TattleEntry* ea = battleGetUnitMonosiriPtr(pa->id);
    TattleEntry* eb = battleGetUnitMonosiriPtr(pb->id);

    pouchHiraganaToKatakana(msgSearch(ea->enemyTag), buf_a);
    pouchHiraganaToKatakana(msgSearch(eb->enemyTag), buf_b);

    if (pa->id == 0x4D || pa->id == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            pouchHiraganaToKatakana(msgSearch("btl_un_hatena"), buf_a);
        } else {
            pouchHiraganaToKatakana(msgSearch("btl_un_rampell"), buf_a);
        }
    }
    if (pb->id == 0x4D || pb->id == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            pouchHiraganaToKatakana(msgSearch("btl_un_hatena"), buf_b);
        } else {
            pouchHiraganaToKatakana(msgSearch("btl_un_rampell"), buf_b);
        }
    }

    return strcmp(buf_a, buf_b);
}

int compare_func3_r(const void* a, const void* b) {
    const LogChar* pa = a;
    const LogChar* pb = b;
    char buf_a[128];
    char buf_b[128];
    TattleEntry* ea = battleGetUnitMonosiriPtr(pa->id);
    TattleEntry* eb = battleGetUnitMonosiriPtr(pb->id);

    pouchHiraganaToKatakana(msgSearch(ea->enemyTag), buf_a);
    pouchHiraganaToKatakana(msgSearch(eb->enemyTag), buf_b);

    if (pa->id == 0x4D || pa->id == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            pouchHiraganaToKatakana(msgSearch("btl_un_hatena"), buf_a);
        } else {
            pouchHiraganaToKatakana(msgSearch("btl_un_rampell"), buf_a);
        }
    }
    if (pb->id == 0x4D || pb->id == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            pouchHiraganaToKatakana(msgSearch("btl_un_hatena"), buf_b);
        } else {
            pouchHiraganaToKatakana(msgSearch("btl_un_rampell"), buf_b);
        }
    }

    return strcmp(buf_b, buf_a);
}

void sort_7_3_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func1);
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func3);
    } else {
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func1_r);
        qqsort(work->logChars, LOGWIN(work)->charCnt, 2, compare_func3_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    LOGWIN(work)->charSel = 0;
    LOGWIN(work)->charPage = LOGWIN(work)->charSel / 16;
}

// The mail list (work->mailList) holds byte pairs: display number, then mail id.
// 1:1 (the `const` parameters reproduce the target's hoisted `lbz r3,0x1(r3)` prologue load).
int compare_func4(const void* a, const void* b) {
    const u8* pa = a;
    const u8* pb = b;
    WinMailEntry* ma = mailGetPtr(pa[1]);
    WinMailEntry* mb = mailGetPtr(pb[1]);
    const char* sa = msgSearch(ma->name);
    const char* sb = msgSearch(mb->name);

    return strcmp(sa, sb);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func4).
int compare_func4_r(const void* a, const void* b) {
    const u8* pa = a;
    const u8* pb = b;
    WinMailEntry* ma = mailGetPtr(pa[1]);
    WinMailEntry* mb = mailGetPtr(pb[1]);
    const char* sa = msgSearch(ma->name);
    const char* sb = msgSearch(mb->name);

    return strcmp(sb, sa);
}

void sort_8_1_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(work->mailList, work->mailCount, 2, compare_func4);
    } else {
        qqsort(work->mailList, work->mailCount, 2, compare_func4_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    work->mailSel = 0;
    work->mailPage = work->mailSel / 5;
}

int compare_func5(const void* a, const void* b) {
    const u8* pa = a;
    const u8* pb = b;
    if (*pa > *pb) {
        return 1;
    } else if (*pa < *pb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_r(const void* a, const void* b) {
    const u8* pa = a;
    const u8* pb = b;
    if (*pa < *pb) {
        return 1;
    } else if (*pa > *pb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_8_2_func(WindowWork* work) {
    if (work->sortReverseNext == 0) {
        qqsort(work->mailList, work->mailCount, 2, compare_func5);
    } else {
        qqsort(work->mailList, work->mailCount, 2, compare_func5_r);
    }
    work->sortReverseNext = 1 - work->sortReverseNext;
    work->mailSel = 0;
    work->mailPage = work->mailSel / 5;
}

// The two trailing lerps use separate deltas so MWCC keeps the first fmadd's
// difference/accumulator in the original's FPRs.
void winSortMain(WindowWork* work) {
    u32 trg = work->buttonTrg;
    u32 rep = work->dirRep;

    if (work->sortWait == 0) {
        trg = 0;
        rep = 0;
    }

    if (trg & 0x200) {
        psndSFXOn((const char*)0x20013);
        work->sortWait = 0;
        work->sortTargetX = -600.0f;
    } else if (rep & 0x1000) {
        psndSFXOn((const char*)0x20005);
        work->sortSel = work->sortSel - 1;
        if (work->sortSel < 0) {
            work->sortSel = work->sortCount - 1;
        }
    } else if (rep & 0x2000) {
        psndSFXOn((const char*)0x20005);
        work->sortSel = work->sortSel + 1;
        if (work->sortSel >= work->sortCount) {
            work->sortSel = 0;
        }
    } else if (trg & 0x100) {
        psndSFXOn((const char*)0x20012);
        sort_data[work->sortKind][work->sortSel].func(work);
    } else if (trg & 0x1000) {
        work->sortWait = 0;
        work->sortTargetX = -600.0f;
    }

    if (work->sortWait != 0) {
        work->targetX = 30.0f + work->sortTargetX;
        work->targetY = (work->sortTargetY - 40.0f) -
                              (f32)(work->sortSel * 0x28);
        winMsgEntry(work, 0, sort_data[work->sortKind][work->sortSel].name, NULL);
    }

    {
        work->sortX += (work->sortTargetX - work->sortX) / 8.0f;
        work->sortY += (work->sortTargetY - work->sortY) / 8.0f;
    }
}

// Descriptor labels pool before the sort draw templates, in retail order.
static SortTable sort_1[] = {
    {"msg_menu_sort_aiueo", sort_1_1_func},
    {"msg_menu_sort_kind", sort_1_2_func},
    {NULL, NULL},
};
static SortTable sort_2[] = {
    {"msg_menu_sort_aiueo", sort_2_1_func},
    {"msg_menu_sort_kind", sort_2_2_func},
    {NULL, NULL},
};
static SortTable sort_3[] = {
    {"msg_menu_sort_aiueo", sort_3_1_func},
    {"msg_menu_sort_kind", sort_3_2_func},
    {"msg_menu_sort_bp", sort_3_3_func},
    {NULL, NULL},
};
static SortTable sort_4[] = {
    {"msg_menu_sort_aiueo", sort_4_1_func},
    {"msg_menu_sort_kind", sort_4_2_func},
    {"msg_menu_sort_bp", sort_4_3_func},
    {NULL, NULL},
};
static SortTable sort_5[] = {
    {"msg_menu_sort_aiueo", sort_5_1_func},
    {"msg_menu_sort_kind", sort_5_2_func},
    {"msg_menu_sort_bp", sort_5_3_func},
    {NULL, NULL},
};
static SortTable sort_6[] = {
    {"msg_menu_sort_aiueo", sort_6_1_func},
    {"msg_menu_sort_kind", sort_6_2_func},
    {NULL, NULL},
};
static SortTable sort_7[] = {
    {"msg_menu_sort_number", sort_7_1_func},
    {"msg_menu_sort_place", sort_7_2_func},
    {"msg_menu_sort_aiueo", sort_7_3_func},
    {NULL, NULL},
};
static SortTable sort_8[] = {
    {"msg_menu_sort_aiueo", sort_8_1_func},
    {"msg_menu_sort_number", sort_8_2_func},
    {NULL, NULL},
};

SortTable* sort_data[8] = {sort_1, sort_2, sort_3, sort_4, sort_5, sort_6, sort_7, sort_8};


void winSortGX(WindowWork* work) {
    f32 x = work->sortX;
    f32 y = work->sortY;
    f32 w = 128.0f;
    f32 w2 = 136.0f;
    s32 i;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(8)->view, 0);
    GXSetCurrentMtx(0);
    if (work->sortKind == 7) {
        GXSetScissor((u32)(16.0f +
                           (16.0f + (304.0f + work->mailX))),
                     (u32)(40.0f + (240.0f - work->mailY)), 0x200,
                     0xC0);
    }

    winSortPanel(work, 0xA5, x, y, w, 24.0);
    winSortPanel(work, 0xA3, x + w, y, 24.0f, 24.0);
    for (i = 0; i < work->sortCount; i++) {
        winSortPanel(work, 0xA6, x + w + 8.0f, y - 24.0f - (f32)(i * 0x28), 16.0f, 40.0);
    }
    winSortPanel(work, 0xA4, x + w, y - 24.0f - (f32)(work->sortCount * 0x28), 24.0f, 24.0);
    winSortPanel(work, 0xA7, x, y - 24.0f - (f32)(work->sortCount * 0x28), w, 24.0);
    winSortPanel(work, 0xA8, x, y - 24.0f, w2, (f32)(work->sortCount * 0x28));

    for (i = 0; i < work->sortCount; i++) {
        const SortTable* list = sort_data[work->sortKind];
        s32 tex = 0xAA;

        if (strcmp(list[i].name, "msg_menu_sort_aiueo") == 0) {
            tex = 0xA9;
        } else if (strcmp(list[i].name, "msg_menu_sort_kind") == 0) {
            tex = 0xAC;
        } else if (strcmp(list[i].name, "msg_menu_sort_bp") == 0) {
            tex = 0xAA;
        } else if (strcmp(list[i].name, "msg_menu_sort_number") == 0) {
            tex = 0xAD;
        } else if (strcmp(list[i].name, "msg_menu_sort_place") == 0) {
            tex = 0xAE;
        } else {
            tex = 0xAA;
        }

        if (i == work->sortSel) {
            f32 row;

            winTexInit(*work->winTpl->data);
            row = y - 20.0f - (f32)(i * 0x28) - 16.0f - 8.0f;
            winTexSet(0xB6, (Vec){x + 64.0f + 3.0f + 16.0f, row, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x00, 0x00, 0x00, 0x80});
            winTexInit_x2(*work->winTpl->data);
            winTexSet_x2(tex, 0xB6, (Vec){x + 64.0f + 16.0f + 4.0f - 6.0f, 6.0f + row, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        } else {
            f32 row;

            winTexInit_x2(*work->winTpl->data);
            row = y - 20.0f - (f32)(i * 0x28) - 16.0f - 8.0f;
            winTexSet_x2(tex, 0xB6, (Vec){x + 64.0f + 16.0f + 4.0f, row, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x80, 0x80, 0x80, 0xFF});
        }
    }

    if (work->sortKind == 7) {
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }
}

void winSortEntry(WindowWork* work, s32 idx, f32 a, f32 b) {
    SortTable* p = sort_data[idx];
    s32 count = 0;

    while (p->name != NULL) {
        p++;
        count++;
    }

    work->sortKind = idx;
    work->sortCount = count;
    work->sortSel = 0;
    work->sortReverseNext = 0;
    work->sortX = -1000.0f;
    work->sortTargetX = a;
    work->sortY = b;
    work->sortTargetY = b;
    work->sortWait = 1;
}

s32 winSortWait(WindowWork* work) {
    return work->sortWait != 0;
}

// Kage (paper) window quad. The vertex constants are literals so MWCC reloads each
// through f0 into the FIFO as the original does; the same-literal GXSetFog zero lets
// MWCC land it in f1 after saving x. The int->f32 magic double still pools anonymously.
void winKageGX(WindowWork* work, f32 x, f32 y, f32 w, f32 h, GXColor* color) {
    GXTexObj texObj;
    Mtx mtx4;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 scaleY;

    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 7, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetTevSwapMode(0, 0, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    TEXGetGXTexObjFromPalette(*work->winTpl->data, &texObj, 0x7A);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(1, *color);
    PSMTXTrans(mtx1, x, y, w);
    scaleY = h * (2.0f * (f32)GXGetTexObjHeight(&texObj));
    PSMTXScale(mtx2, h * (2.0f * (f32)GXGetTexObjWidth(&texObj)), scaleY, 1.0f);
    PSMTXRotRad(mtx3, 0x78, -1.3962634f);
    PSMTXConcat(mtx1, mtx3, mtx4);
    PSMTXConcat(mtx4, mtx2, mtx4);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx4, mtx4);
    GXLoadPosMtxImm(mtx4, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winZClear(void) {
    Mtx mtx;

    PSMTXTrans(mtx, 0.0f, 0.0f, -1000.0f);
    PSMTXConcat(camGetCurPtr()->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetAlphaUpdate(0);
    GXSetColorUpdate(0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevOp(0, 4);
    GXSetNumTexGens(0);
    GXSetZMode(1, 7, 1);
    GXSetBlendMode(0, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxDesc(9, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(-500.0f, 300.0f, 0.0f);
    GXPosition3f32(0.0f, 300.0f, 0.0f);
    GXPosition3f32(0.0f, -100.0f, 0.0f);
    GXPosition3f32(-500.0f, -100.0f, 0.0f);
    GXEnd();
    GXSetColorUpdate(1);
}

// win texture ids of the read/unread mark
typedef enum {
    WIN_TEX_MAIL_UNREAD = 0x5D,
    WIN_TEX_MAIL_READ = 0x5E,
} WinMailTex;

// The mail window: the list of letters (an envelope in the mail's colour, a read/unread mark, the two-digit
// number and the title per row), or with one open (state 0xCA) its header, the scrolling body with its
// picture, and the scroll arrows.
void winMailDisp(WindowWork* work) {
    s32 i;

    if (work->mailX < 320.0f && work->mailY > -240.0f) {
        winMailGX(work, work->mailX, work->mailY);

        if (work->itemState != 0xCA) {
            for (i = 0; i < work->mailCount; i++) {
                s32 id = work->mailList[1 + i * 2];
                s32 num = work->mailList[i * 2];
                WinMailEntry* mail = mailGetPtr(id);
                GXColor color;
                GXColor black = {0x00, 0x00, 0x00, 0xFF};
                GXColor white;

                if (mail->flags & 1) {
                    color = (GXColor){0xD7, 0xAC, 0xBC, 0xFF};
                } else if (mail->flags & 2) {
                    color = (GXColor){0xD3, 0xD7, 0xAC, 0xFF};
                } else if (mail->flags & 4) {
                    color = (GXColor){0xAC, 0xC2, 0xD7, 0xFF};
                } else {
                    color = (GXColor){0xE1, 0xE1, 0xE1, 0xFF};
                }
                winTexInit(WIN_MAIL_TEX(work));
                winTexSet(0x77,
                          (Vec){4.0f + (208.0f + work->mailX), work->mailPagePos + ((work->mailY - 56.0f) - (f32)(i * 40)), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, color);
                white = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                winTexSet(pouchCheckMail(id) == 2 ? WIN_TEX_MAIL_READ : WIN_TEX_MAIL_UNREAD,
                          (Vec){72.0f + work->mailX, work->mailPagePos + (((work->mailY - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, white);
                winTexSet((num + 1) / 10 + 0x65,
                          (Vec){2.0f + (88.0f + work->mailX), work->mailPagePos + (((work->mailY - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, white);
                winTexSet((num + 1) % 10 + 0x65,
                          (Vec){2.0f + (104.0f + work->mailX), work->mailPagePos + (((work->mailY - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, white);
                winFontInit();
                winFontSetLabel((Vec){120.0f + work->mailX, work->mailPagePos + (8.0f + ((work->mailY - 56.0f) - (f32)(i * 40))), 0.0f},
                                (Vec){0.75f, 0.75f, 0.75f}, black, mail->name);
            }
            GXSetScissor(0, 0, 0x260, 0x1E0);
        } else {
            s32 sel = work->mailSel;
            s32 num = work->mailList[sel * 2];
            GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
            GXColor black = {0x00, 0x00, 0x00, 0xFF};
            WinMailEntry* mail = mailGetPtr(work->mailList[1 + sel * 2]);
            GXColor color;
            u32 top;

            if (mail->flags & 1) {
                color = (GXColor){0xD7, 0xAC, 0xBC, 0xFF};
            } else if (mail->flags & 2) {
                color = (GXColor){0xD3, 0xD7, 0xAC, 0xFF};
            } else if (mail->flags & 4) {
                color = (GXColor){0xAC, 0xC2, 0xD7, 0xFF};
            } else {
                color = (GXColor){0xE1, 0xE1, 0xE1, 0xFF};
            }

            // the header: envelope, opened mark, number and title
            winTexInit(WIN_MAIL_TEX(work));
            winTexSet(0x77,
                      (Vec){4.0f + (208.0f + work->mailX), work->mailY - 56.0f, 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, color);
            winTexSet(0x5E,
                      (Vec){72.0f + work->mailX, work->mailY - 56.0f, 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winTexSet((num + 1) / 10 + 0x65,
                      (Vec){2.0f + (88.0f + work->mailX), (work->mailY - 56.0f) - 3.0f, 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winTexSet((num + 1) % 10 + 0x65,
                      (Vec){2.0f + (104.0f + work->mailX), (work->mailY - 56.0f) - 3.0f, 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winFontInit();
            winFontSetLabel((Vec){120.0f + work->mailX, 8.0f + (work->mailY - 56.0f), 0.0f},
                            (Vec){0.75f, 0.75f, 0.75f}, black, mail->name);

            // the body, clipped to the page and scrolled by whole lines
            top = 72.0f + (240.0f - work->mailY);
            GXSetScissor(32.0f + (304.0f + work->mailX), top, 0x158, 0xB1);
            winTexInit(WIN_MAIL_TEX(work));
            winTexSet(0x77,
                      (Vec){4.0f + (208.0f + work->mailX), (work->mailY - 56.0f) - 80.0f, 0.0f},
                      (Vec){1.0f, 7.5f, 1.0f}, color);
            winFontInit();
            winFontSetLabel((Vec){80.0f + work->mailX, (work->mailY - 80.0f) + (f32)(work->mailScroll * 21), 0.0f},
                            (Vec){0.75f, 0.75f, 0.75f}, (GXColor){0x00, 0x00, 0x00, 0xFF}, mail->body);
            if (mail->flags & 0x1000) {
                winTexInit(*work->mailTpl->data);
                winTexSet(mail->picture,
                          (Vec){210.0f + work->mailX, 7.0f + ((work->mailY - (f32)((work->mailLines - 1) * 21))
                                                       + (f32)(work->mailScroll * 21)), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            }
            GXSetScissor(0, 0, 0x260, 0x1E0);

            // scroll arrows (blinking) and the scroll bar, when the body is longer than a page
            if (work->mailLines > 8) {
                winIconInit();
                if (work->mailScroll == 0) {
                    if ((gp->retraceCount & 0x1F) < 20) {
                        winIconSet(0x1B6,
                                   (Vec){360.0f + work->mailX, (20.0f + (work->mailY - 230.0f)) - 24.0f, 0.0f},
                                   (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                    }
                } else if (work->mailScroll + 6 == work->mailLines) {
                    if ((gp->retraceCount & 0x1F) < 20) {
                        winIconSet(0x1B5,
                                   (Vec){360.0f + work->mailX, 24.0f + (20.0f + (work->mailY - 230.0f)), 0.0f},
                                   (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                    }
                } else if ((gp->retraceCount & 0x1F) < 20) {
                    winIconSet(0x1B5,
                               (Vec){360.0f + work->mailX, 24.0f + (20.0f + (work->mailY - 230.0f)), 0.0f},
                               (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                    winIconSet(0x1B6,
                               (Vec){360.0f + work->mailX, (20.0f + (work->mailY - 230.0f)) - 24.0f, 0.0f},
                               (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
                winIconSet(0x70,
                           (Vec){360.0f + work->mailX, 20.0f + (work->mailY - 230.0f), 0.0f},
                           (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            }
        }
    }
}

void winRootFbCopy(s32 unused, WindowWork* work) {
    GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
    GXSetTexCopyDst(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(work->fbCopy->address, GX_FALSE);
    GXTexModeSync();
    work->fbCopyReady = 1;
    N_mapDispOff();
    camDispOff(4);
}

// Full-screen textured quad. Writing the vertex constants as literals makes MWCC
// reload them per FIFO store (named externs let it common the load), which is what
// the original does; the local `half` gives the original's value*constant operand
// order. The int->f32 magic double still pools anonymously, which objdiff accepts.

void winRootFbDraw(s32 unused, WindowWork* work) {
    GXTexObj texObj;
    f32 w;
    f32 h;

    if (work->fbCopyReady != 0) {
        GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        GXSetNumChans(0);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xFF);
        GXSetTevOp(0, 3);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        GXSetBlendMode(1, 4, 5, 0);
        GXSetZCompLoc(1);
        GXSetAlphaCompare(7, 0, 0, 7, 0);
        GXSetZMode(0, 7, 0);
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        GXSetCurrentMtx(0);
        GXInitTexObj(&texObj, work->fbCopy->address, gp->fbWidth, gp->efbHeight, 4, 0, 0, 0);
        GXLoadTexObj(&texObj, 0);
        GXSetCullMode(0);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(13, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 13, 1, 4, 0);
        GXBegin(0x80, 0, 4);
        w = (f32)gp->fbWidth;
        h = (f32)gp->efbHeight;
        {
            f32 half = 0.5f;
            GXPosition3f32(-w * half, -h * half, 0.0f);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(w * half, -h * half, 0.0f);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(w * half, h * half, 0.0f);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(-w * half, h * half, 0.0f);
            GXTexCoord2f32(0.0f, 0.0f);
        }
    }
}

// Scalar objects whose loads must retain the original named-constant behaviour.
