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

#ifdef MUST_MATCH
// Retail shares one base register for this unit's literal and template pool.
#pragma pool_data on
#endif

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))

typedef struct SortTable {
    const char* name;                 // 0x0
    void (*func)(WindowWork* work);    // 0x4
} SortTable;

// Menu tab and record blocks embedded in WindowWork at 0x48/0xC0.
typedef struct WinRootTab {
    s32 id;          // 0x0
    f32 x;           // 0x4
    f32 y;           // 0x8
    s32 state;       // 0xC
    s32 counter;     // 0x10
    const char* str; // 0x14
} WinRootTab;

typedef struct WinRootRec {
    u32 a; // 0x0
    u32 b; // 0x4
    u32 c; // 0x8
    s32 d; // 0xC
    u32 e; // 0x10
} WinRootRec;

typedef struct WinRoot {
    u8 pad0[0x48];
    WinRootTab tabs[5]; // 0x48
    WinRootRec recs[5]; // 0xC0
} WinRoot;

#define WROOT(w) ((WinRoot*)(w))

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
        TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, (id_)); \
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


static inline Vec mail_vec(const Vec* base, f32 x, f32 y) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    return v;
}

// A mail entry as winMailDisp reads it (mail_data.h returns it untyped).
typedef struct WinMailEntry {
    u16 flags;           // 0x0: 1/2/4 pick the envelope colour, 0x1000 has a picture
    u8 field_0x2[0x6];   // 0x2
    const char* name;    // 0x8
    const char* body;    // 0xC
    s32 picture;         // 0x10
} WinMailEntry;

// winFontSetLabel and winIconSet take their Vec and GXColor arguments by value (each call copies them);
// win_main.h declares pointers, which compiles to the same callee.
// winFontSetLabel takes its Vecs and colour by pointer; the original passes them by value here (caller copies plus a
// pointer, ABI-identical), and only the cast reproduces its argument scheduling.
#ifdef MUST_MATCH
#define winMailLabelV ((void (*)(Vec, Vec, GXColor, const char*))winFontSetLabel)
#else
static inline void winMailLabelV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSetLabel(&pos, &scale, &color, msg);
}
#endif
static inline void winMailIconV(s32 id, Vec pos, Vec scale, GXColor color) {
    winIconSet(id, &pos, &scale, &color);
}
#define WIN_MAIL_TEX(w) (**(TEXPalette***)(*(u8**)((u8*)(w) + 0x28) + 0xA0))

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

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
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

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
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


static inline void winMsgEdgeV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSetEdge(&pos, &scale, &color, msg);
}
static inline void winMsgFontV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSet(&pos, &scale, &color, msg);
}

#define WIN_BOOK_TEX(w) (**(TEXPalette***)(*(u8**)((u8*)(w) + 0x28) + 0xA0))
extern const char** luigi_book_list[];
// winFontSetLabel takes its Vec and GXColor arguments by value (each call copies them); win_main.h declares
// pointers, which compiles to the same callee.
// winFontSetLabel takes its Vecs and colour by pointer; the original passes them by value here (caller copies plus a
// pointer, ABI-identical), and only the cast reproduces its argument scheduling.
#ifdef MUST_MATCH
#define winFontSetLabelV ((void (*)(Vec, Vec, GXColor, const char*))winFontSetLabel)
#define winFontSetV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSet)
#else
static inline void winFontSetLabelV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSetLabel(&pos, &scale, &color, msg);
}
static inline void winFontSetV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSet(&pos, &scale, &color, msg);
}
#endif

// Per-tab slide-in/slide-out animation. winRootTabStep is a plain static defined after winRootMain: MWCC
// inlines it and still generates its body first, so its 185.0f pools ahead of winRootMain's 1000.0f in
// .sdata2 (the linker strips the unreferenced body).
static void winRootTabStep(WinRootTab* tab, s32 i);
static inline void winRootTabMain(WindowWork* work) {
    s32 i;

    for (i = 0; i < 5; i++) {
        winRootTabStep(&WROOT(work)->tabs[i], i);
    }
}


// Main menu. Builds the visible tab set into the work block, then runs the
// per-state logic and the common per-tab animation. The string array is the
// unit's static msg_help object; the byte-pointer casts reproduce the original
// `add base,base,idx; lwz disp(base)` reads of tabs[]/recs[].
#pragma opt_common_subs off // keep each field load's own `add base,base,idx` instead of hoisting &work->tabs
s32 winRootMain(WindowWork* work) {
    static const char* msg_help[5] = {
        "msg_menu_mario", "msg_menu_nakama", "msg_menu_mochimono", "msg_menu_badge", "msg_menu_kiroku",
    };
    s32 result;
    WinRootTab* tab;
    s32 j;
    const char* str;

    switch (WW_S32(work, 0x24)) {
    case 0:
        psndSFXOn((const char*)0x2002A);
        WW_S32(work, 0x44) = 0;
        tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
        tab->id = 0;
        tab->state = 0;
        tab->y = 1000.0f;
        tab->x = 1000.0f;
        tab->str = msg_help[0];
        WW_S32(work, 0x44)++;
        if (swByteGet(0) >= 7 || partyChkJoin(1) != 0) {
            tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
            tab->id = 1;
            tab->state = 0;
            tab->y = 1000.0f;
            tab->x = 1000.0f;
            tab->str = msg_help[1];
            WW_S32(work, 0x44)++;
        }
        tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
        tab->id = 2;
        tab->state = 0;
        tab->y = 1000.0f;
        tab->x = 1000.0f;
        tab->str = msg_help[2];
        WW_S32(work, 0x44)++;
        if (swByteGet(0) >= 0x14 || pouchGetHaveBadgeCnt() != 0) {
            tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
            tab->id = 3;
            tab->state = 0;
            tab->y = 1000.0f;
            tab->x = 1000.0f;
            tab->str = msg_help[3];
            WW_S32(work, 0x44)++;
        }
        tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
        tab->id = 4;
        tab->state = 0;
        tab->y = 1000.0f;
        tab->x = 1000.0f;
        tab->str = msg_help[4];
        WW_S32(work, 0x44)++;
        for (j = 0; j < WW_S32(work, 0x44); j++) {
            if (WROOT(work)->tabs[j].id == WW_S32(work, 0x1C)) {
                WW_S32(work, 0x40) = j;
            }
        }
        WW_S32(work, 0xCC) = -1;
        WW_F32(work, 0xC8) = 1000.0f;
        WW_F32(work, 0xC4) = 1000.0f;
        WW_S32(work, 0xC0) = -1;
        WW_S32(work, 0xE0) = -1;
        WW_F32(work, 0xDC) = 1000.0f;
        WW_F32(work, 0xD8) = 1000.0f;
        WW_S32(work, 0xD4) = -1;
        WW_S32(work, 0xF4) = -1;
        WW_F32(work, 0xF0) = 1000.0f;
        WW_F32(work, 0xEC) = 1000.0f;
        WW_S32(work, 0xE8) = -1;
        WW_S32(work, 0x108) = -1;
        WW_F32(work, 0x104) = 1000.0f;
        WW_F32(work, 0x100) = 1000.0f;
        WW_S32(work, 0xFC) = -1;
        WW_S32(work, 0x11C) = -1;
        WW_F32(work, 0x118) = 1000.0f;
        WW_F32(work, 0x114) = 1000.0f;
        WW_S32(work, 0x110) = -1;
        WW_F32(work, 0xC8) = 1000.0f;
        WW_F32(work, 0xC4) = 1000.0f;
        WW_S32(work, 0xCC) = 0;
        WW_S32(work, 0xC0) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id;
        WW_S32(work, 0x130) = 0;
        WW_F32(work, 0x12C) = 1000.0f;
        WW_F32(work, 0x128) = 1000.0f;
        WW_S32(work, 0x13C) = 0;
        WW_S32(work, 0x140) = 0;
        WW_S32(work, 0x144) = 0;
        winMarioInit(work);
        winPartyInit(work);
        winItemInit(work);
        winBadgeInit(work);
        winLogInit(work);
        {
            f32 pos = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);

            WW_F32(work, 0x158) = pos;
            WW_F32(work, 0x150) = pos;
        }
        WW_F32(work, 0x154) = 300.0f;
        WW_F32(work, 0x15C) = 185.0f;
        WW_F32(work, 0x16C) = -1000.0f;
        WW_F32(work, 0x164) = -1000.0f;
        WW_F32(work, 0x170) = -1000.0f;
        WW_F32(work, 0x168) = -1000.0f;
        str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
        winMsgEntry(work, 0, str, 0);
        WW_S32(work, 0x24) = 0xA;
        switch (WW_S32(work, 0x1C)) {
        case 1:
            winPartyInit2(work);
            WW_S32(work, 0x24) = 0xC;
            work->flags |= 0x4000;
            break;
        case 2:
            winItemInit2(work);
            WW_S32(work, 0x24) = 0xD;
            work->flags |= 0x4000;
            break;
        case 3:
            winBadgeInit2(work);
            WW_S32(work, 0x24) = 0xE;
            work->flags |= 0x4000;
            break;
        case 4:
            winLogInit2(work);
            WW_S32(work, 0x24) = 0xF;
            work->flags |= 0x4000;
            break;
        }
        if (gp->flags & 0x1000) {
            WW_S32(work, 0x44) = 0;
            tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
            tab->id = 1;
            tab->state = 0;
            tab->y = 1000.0f;
            tab->x = 1000.0f;
            tab->str = msg_help[1];
            WW_S32(work, 0x44)++;
            winPartyInit2(work);
            WW_S32(work, 0x24) = 0xC;
            WW_S32(work, 0x40) = 0;
        }
        WW_PTR(work, 0x34) = smartAlloc(GXGetTexBufferSize(gp->fbWidth, gp->efbHeight, 4, 0, 0), 1);
        WW_S32(work, 0x38) = 0;
        dispEntry(CAMERA_2D, 1000.0f, 0, (DispCallback)unk_8014d5dc, work);
        break;
    case 10:
        if (work->buttonTrg & 0x1200) {
            if (WW_S32(work, 0xCC) == 0x64) {
                WW_S32(work, 0x24) = 0x14;
            }
            break;
        }
        if (work->buttonTrg & 0x100) {
            if (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id != 1 || WW_S32(work, 0x1E0) != 0) {
                psndSFXOn((const char*)0x20012);
                WW_S32(work, 0x24) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id + 0xB;
                switch (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id) {
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
            WW_S32(work, 0x40)--;
            if (WW_S32(work, 0x40) < 0) {
                WW_S32(work, 0x40) = WW_S32(work, 0x44) - 1;
            }
            for (j = 4; j > 0; j--) {
                WROOT(work)->recs[j] = WROOT(work)->recs[j - 1];
            }
            WW_F32(work, 0xC8) = 1000.0f;
            WW_F32(work, 0xC4) = 1000.0f;
            WW_S32(work, 0xCC) = 0x1E;
            WW_S32(work, 0xC0) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id;
            WW_F32(work, 0x158) = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);
            str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
            winMsgEntry(work, 0, str, 0);
        } else if (work->dirRep & 0x8000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            WW_S32(work, 0x40)++;
            if (WW_S32(work, 0x40) >= WW_S32(work, 0x44)) {
                WW_S32(work, 0x40) = 0;
            }
            for (j = 4; j > 0; j--) {
                WROOT(work)->recs[j] = WROOT(work)->recs[j - 1];
            }
            WW_F32(work, 0xC8) = 1000.0f;
            WW_F32(work, 0xC4) = 1000.0f;
            WW_S32(work, 0xCC) = 0x14;
            WW_S32(work, 0xC0) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id;
            WW_F32(work, 0x158) = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);
            str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
            winMsgEntry(work, 0, str, 0);
        }
        break;
    case 100:
        if (((WinRootRec*)((u8*)WROOT(work)->recs + WW_S32(work, 0x40) * 0x14))->d == 0x64) {
            WW_S32(work, 0x24) = 0xA;
        }
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        switch (WW_S32(work, 0x24)) {
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
            WW_F32(work, 0x158) = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);
            WW_F32(work, 0x15C) = 185.0f;
            WW_S32(work, 0x24) = 0xA;
            str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
            winMsgEntry(work, 0, str, 0);
            work->flags &= ~0x4000;
        } else if (result == -2) {
            WW_S32(work, 0x24) = 0x14;
            work->flags &= ~0x4000;
        }
        break;
    case 20:
        psndSFXOn((const char*)0x2002B);
        for (j = 0; j < WW_S32(work, 0x44); j++) {
            WROOT(work)->tabs[j].state = 0xA;
        }
        WW_S32(work, 0xCC) = 0xA;
        WW_S32(work, 0xE0) = 0xA;
        WW_S32(work, 0xF4) = 0xA;
        WW_S32(work, 0x108) = 0xA;
        WW_S32(work, 0x11C) = 0xA;
        WW_S32(work, 0x130) = 0xA;
        N_mapDispOn();
        camDispOn(CAMERA_3D);
        WW_S32(work, 0x24)++;
        break;
    default:
        if (WW_PTR(work, 0x34) != NULL) {
            smartFree((smartEntry*)WW_PTR(work, 0x34));
            WW_PTR(work, 0x34) = NULL;
            WW_S32(work, 0x38) = 0;
        }
        WW_S32(work, 0x24)++;
        if (WW_S32(work, 0x24) > 0x32) {
            winMarioExit(work);
            winPartyExit(work);
            winItemExit(work);
#ifdef MUST_MATCH
            // winBadgeExit's header says void(void), but the original passes work in r3.
            ((void (*)(WindowWork*))winBadgeExit)(work);
#else
            winBadgeExit();
#endif
            winLogExit(work);
            if (WW_S32(work, 0x19C) != 0 && WW_S32(work, 0x1DC) != WW_S32(work, 0x1FC)) {
                return -2;
            }
            if (WW_S32(work, 0x2D4) != 0) {
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
    if (WW_S32(work, 0x24) < 0x14) {
        if (work->flags & 0x4000) {
            WW_F32(work, 0x150) += (WW_F32(work, 0x158) - WW_F32(work, 0x150)) / 4.0f;
            WW_F32(work, 0x154) += (WW_F32(work, 0x15C) - WW_F32(work, 0x154)) / 4.0f;
        } else {
            WW_F32(work, 0x150) += (WW_F32(work, 0x158) - WW_F32(work, 0x150)) / 4.0f;
            WW_F32(work, 0x154) += (WW_F32(work, 0x15C) + (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->y - 185.0f) - WW_F32(work, 0x154)) / 4.0f;
        }
    } else if (WW_F32(work, 0x154) > 180.0f) {
            WW_F32(work, 0x150) += (WW_F32(work, 0x158) - WW_F32(work, 0x150)) / 4.0f;
            WW_F32(work, 0x154) += (WW_F32(work, 0x15C) + (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->y - 185.0f) - WW_F32(work, 0x154)) / 4.0f;
    } else {
            WW_F32(work, 0x150) += (WW_F32(work, 0x158) + WW_F32(work, 0xC4) - WW_F32(work, 0x150)) / 4.0f;
            WW_F32(work, 0x154) += (WW_F32(work, 0x15C) - WW_F32(work, 0x154)) / 4.0f;
    }
    WW_F32(work, 0x150) = (f32)(WW_F32(work, 0x150) +
        __fabs(2.0f * (f32)sin((f32)gp->retraceCount / 8.0f)));
    WW_F32(work, 0x164) += (WW_F32(work, 0x16C) - WW_F32(work, 0x164)) / 4.0f;
    WW_F32(work, 0x168) += (WW_F32(work, 0x170) - WW_F32(work, 0x168)) / 4.0f;
    dispEntry(CAMERA_2D, 1000.0f, 0, (DispCallback)unk_8014d338, work);
    return 0;
}

#pragma opt_common_subs reset


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

    if (WW_S32(work, 0x24) == 0) {
        return;
    }

    for (i = 4; i >= 0; i--) {
        u8* e = (u8*)work + i * 0x14;
        s32 state = WW_S32(e, 0xCC);

        if (state == -1) {
            continue;
        }
        if (i > 0 && WW_S32(e, 0xB8) == 0x64) {
            continue;
        }
        if (i > 0 && (u32)(state - 0xA) <= 1) {
            continue;
        }

        switch (WW_S32(e, 0xC0)) {
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
    winMsgDisp(work, WW_F32(work, 0x128), WW_F32(work, 0x12C));
    winSortGX(work);

    if (!(work->flags & 0x8000)) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));

        {
            f32 two = 2.0f;
            winTexSet(0, (Vec){two + WW_F32(work, 0x150), WW_F32(work, 0x154) - two, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                      (GXColor){0x00, 0x00, 0x00, 0x80});
        }
        winTexSet(0, (Vec){WW_F32(work, 0x150), WW_F32(work, 0x154), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, bgcolor);
    }
}

// The menu tabs, the selected one bright.
static inline void winRootTabDisp(WindowWork* work) {
    s32 j;

    for (j = 0; j < WW_S32(work, 0x44); j++) {
        WinRootTab* tab = &WROOT(work)->tabs[j];
        GXColor color;

        if (WW_S32(work, 0x40) == j) {
            color = (GXColor){0xFF, 0xFF, 0xFF, 0xFE};
        } else {
            color = (GXColor){0x80, 0x80, 0x80, 0xFE};
        }
        winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        winTexSet_x2(tab->id + 1, 0xB2, (Vec){tab->x, tab->y, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, color);
    }
}

// 5-page switch over work->0xcc. The explicit empty case -1 below (a redirect
// target of cases 11 and 41) is what makes MWCC pick 0 as the [0,10) search-tree
// pivot; without it the tree pivots on 1.
void winBgMain(WindowWork* work) {
    WindowWork* p = work;
    s32 i;

    for (i = 0; i < 5; i++) {
        switch (WW_S32(p, 0xCC)) {
            case 0:
                WW_F32(p, 0xC4) = 800.0f;
                WW_F32(p, 0xC8) = 0.0f;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 1: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, 800.0f, 0.0f);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 10:
                WW_F32(p, 0xC8) = 0.0f;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 11: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, 0.0f, 800.0f);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
            case 20:
                WW_F32(p, 0xC8) = 0.0f;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 21: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, 640.0f, 0.0f);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 30:
                WW_F32(p, 0xC8) = 0.0f;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 31: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, -640.0f, 0.0f);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 40:
                WW_F32(p, 0xC8) = 0.0f;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 41: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, 0.0f, -640.0f);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
#ifdef MUST_MATCH
            /* MUST_MATCH: -1 is a redirect target of cases 11/41; the empty case makes
               MWCC emit the retail search-tree pivot (0 then 2) instead of 1 then 0. */
            case -1:
#endif
            case 100:
                break;
            default:
                break;
        }
        p = (WindowWork*)((u8*)p + 0x14);
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

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 7);
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

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 6);
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
    switch (WW_S32(work, 0x130)) {
        case 0:
            WW_F32(work, 0x128) = -800.0f;
            WW_F32(work, 0x12C) = -170.0f;
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 1: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, -800.0f, 0.0f);
            if (WW_S32(work, 0x134) > 0x10) {
                WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            }
            break;
        }
        case 2:
            if (keyGetDirTrg(0) & 0x100) {
                WW_S32(work, 0x148) = WW_S32(work, 0x148) - 2;
                if (WW_S32(work, 0x148) < 0) {
                    WW_S32(work, 0x148) = 0;
                } else {
                    psndSFXOn((const char*)0x20006);
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if (WW_S32(work, 0x148) / 2 < (WW_S32(work, 0x14C) + 1) / 2 - 1) {
                    WW_S32(work, 0x148) = WW_S32(work, 0x148) + 2;
                    psndSFXOn((const char*)0x20006);
                }
            }
            break;
        case 10:
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 11: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, 0.0f, -800.0f);
            if (WW_S32(work, 0x134) > 0x10) {
                WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
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
    // Semantically x - 250.0f; the original computes it as +10 then -10 (MUST_MATCH codegen shaping).
    f32 fx = (10.0f + (x - 250.0f)) - 10.0f;
    f32 fy = 40.0f + y;
#else
    f32 fx = x - 250.0f;
    f32 fy = 40.0f + y;
#endif
    f32 ty;
    f32 ix;
    f32 width;

    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, fx, fy, 500.0f, 80.0f, 20.0f);

    if (WW_S32(work, 0x138) != 0 && WW_S32(work, 0x138) > 0) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        winTexSet(0x25, (Vec){ fx, fy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor);
        winIconInit();
        winMailIconV(((0, (ItemDataView*)itemDataTable)[WW_S32(work, 0x138)]).icon_id, (Vec){ fx, fy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor);
    }

    if (WW_PTR(work, 0x140) != NULL) {
        const char* msg;
        f32 edge;

        edge = (WW_S32(work, 0x138) == -2) ? -130.0f : 0.0f;
        msg = msgSearch((const char*)WW_PTR(work, 0x140));
        width = FontGetMessageWidth(msg);
        winNameGX(work, 1, (x - 120.0f) + edge, ty = 8.0f + (16.0f + fy), 240.0f, 32.0f);
        winFontInit();
        winMsgEdgeV((Vec){ edge + ((240.0f - width) / 2.0f + (x - 120.0f)), ty - 4.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor, msg);
    }

    if (WW_U32(work, 0x13C) != 0) {
        GXSetScissor(0, (u32)(240.0f - (fy -= 14.0f)), 0x260, 0x38);
        if (strcmp((const char*)WW_PTR(work, 0x13C), "msg_lottery_number") == 0) {
            // lottery_info has no declared field at 0x20; the original loads an s16 there.
            sprintf(buf, msgSearch((const char*)WW_PTR(work, 0x13C)), *(s16*)((u8*)lotteryGetPtr() + 0x20));
            winFontInit();
            winMsgFontV((Vec){ (40.0f + (x - 250.0f)) - 10.0f, fy + (f32)(WW_S32(work, 0x148) * 0x1C), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, fontColor, buf);
        } else {
            winFontInit();
            winMailLabelV((Vec){ (40.0f + (x - 250.0f)) - 10.0f, fy + (f32)(WW_S32(work, 0x148) * 0x1C), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, fontColor, (const char*)WW_PTR(work, 0x13C));
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }

    if (WW_S32(work, 0x14C) > 2) {
        s32 i;

        winIconInit();
        i = WW_S32(work, 0x148);
        if (i == 0) {
            if ((gp->retraceCount & 0x1F) < 0x14) winMailIconV(0x1B6, (Vec){ (250.0f + x) - 10.0f, (40.0f + (y - 40.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        } else if (i / 2 == (WW_S32(work, 0x14C) + 1) / 2 - 1) {
            if ((gp->retraceCount & 0x1F) < 0x14) winMailIconV(0x1B5, (Vec){ (250.0f + x) - 10.0f, 24.0f + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        } else if ((gp->retraceCount & 0x1F) < 0x14) {
            ix = (250.0f + x) - 10.0f;
            winMailIconV(0x1B5, (Vec){ ix, 24.0f + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            winMailIconV(0x1B6, (Vec){ ix, (40.0f + (y - 40.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        }
        winMailIconV(0x70, (Vec){ (250.0f + x) - 10.0f, 40.0f + (y - 40.0f), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
}

void winMsgEntry(WindowWork* work, s32 a1, const char* str, s32 a3) {
    static char _tmp[0x40];
    u16 width;
    BOOL changed = FALSE;

    if (str != NULL) {
        if (WW_PTR(work, 0x144) != (void*)str) {
            changed = TRUE;
        } else if (strcmp(_tmp, str) != 0) {
            changed = TRUE;
        }
        if (changed) {
            WW_S32(work, 0x138) = a1;
            WW_PTR(work, 0x13C) = (void*)str;
            WW_S32(work, 0x140) = a3;
            FontGetMessageWidthLine((const char*)msgSearch((const char*)WW_PTR(work, 0x13C)), &width);
            WW_S32(work, 0x148) = 0;
            WW_S32(work, 0x14C) = width + 1;
            WW_PTR(work, 0x144) = (void*)str;
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
static inline void winBookQuad(void* pal, s32 texId, f32 x, f32 y, f64 w, f64 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette((TEXPalette*)pal, &texObj, texId);
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
        winFontSetLabelV((Vec){labelX, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, (GXColor){0x00, 0x00, 0x00, 0xFF},
                         luigi_book_list[WW_S32(work, 0x2C8)][WW_S32(work, 0x2D0) * 2]);
        winFontSetLabelV((Vec){220.0f + labelX, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f},
                         (GXColor){0x00, 0x00, 0x00, 0xFF},
                         luigi_book_list[WW_S32(work, 0x2C8)][WW_S32(work, 0x2D0) * 2 + 1]);

        // Luigi, reading: the closing pose on the last page
        pose = L_msgGetWork()->poseId;
        anim = animPoseGetAnimDataPtr(pose);
        if (WW_S32(work, 0x2D0) < WW_S32(work, 0x2CC) - 1) {
            animPoseSetAnim(pose, "HOS_C_1", 0);
        } else {
            animPoseSetAnim(pose, "HOS_E_1", 0);
        }
        animPoseSetLocalTime(pose, gp->retraceCount % (s32)*(f32*)(*(u8**)((u8*)anim + 0x24) + 0x8));
        animPoseMain(pose);
        animPoseDraw(pose, 425.0f + x, y - 275.0f, 0.0f, 180.0f, 5.0f, 2);
    }
}

void winHalfBookGX(void* pal, f32 x, f32 y, s32 a4) {
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
        scroll = WW_F32(work, 0xEA0);
        count = WW_S32(work, 0xE94);
        cursor = WW_S32(work, 0xE9C);
    } else {
        scroll = WW_F32(work, 0xF9C);
        count = WW_S32(work, 0xF90);
        cursor = WW_S32(work, 0xF98);
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
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
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
                        s16 itemId = *(s16*)((u8*)work + 0xEB8 + idx * 2);
                        if (swGet(itemId + 0x80) != 0) {
                            continue;
                        }
                    } else {
                        s16 itemId = *(s16*)((u8*)work + 0xFB4 + idx * 2);
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
                        if (swGet(*(s16*)((u8*)work + 0xEB8 + idx * 2) + 0x80) == 0) {
                            continue;
                        }
                        { ItemData* it = &itemDataTable[*(s16*)((u8*)work + 0xEB8 + idx * 2)];
                        iconId = *(u16*)&it[0xF0].icon_id; }
                    } else {
                        if (swGet(*(s16*)((u8*)work + 0xFB4 + idx * 2) + 0x41) == 0) {
                            continue;
                        }
                        { ItemData* it = &itemDataTable[*(s16*)((u8*)work + 0xFB4 + idx * 2)];
                        iconId = *(u16*)&it[0xB3].icon_id; }
                    }
                    winMailIconV(iconId, (Vec){((x + (f32)(col * 0x38)) + 24.0f) + 4.0f, ((scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8)) - 28.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            }
        }
    }

    // Bottom status bar.
    GXSetScissor(0, 0, 0x260, 0x1E0);
    if (cursor > 0) {
        f32 bx;

        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        bx = 395.0f + x;
        winTexSet(0x17, (Vec){bx, 15.0f + y, 0.0f}, (Vec){-1.0f, -1.0f, -1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        winIconInit();
        winMailIconV(0x7E, (Vec){bx, y, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
    if (count > cursor * 0x1C + 0x1C) {
        f32 bx;

        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        bx = 395.0f + x;
        winTexSet(0x17, (Vec){bx, y - 235.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        winIconInit();
        winMailIconV(0x80, (Vec){bx, y - 220.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
    {
        const char* msg;
        f32 w0;
        f32 fx;

        msg = msgSearch("msg_menu_sort_narabikae");
        w0 = 0.7f * (f32)FontGetMessageWidth(msg);
        winFontInit();
        fx = 380.0f - w0;
        winFontSetV((Vec){x + fx, y - 228.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, (GXColor){0x00, 0x00, 0x00, 0xFF}, msg);
        winIconInit();
        winMailIconV(0x217, (Vec){(x + fx) - 30.0f, y - 237.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
    s32 item = WW_S32(work, 0x214);
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    {
        s32 type = 7;
        if (WW_S32(work, 0x180) == 0) {
            type = 0;
        }
        pouchSortItem(type);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = WW_S32(work, 0x214) / 10;
}

void sort_1_2_func(WindowWork* work) {
    s32 item = WW_S32(work, 0x214);
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 1 : 8);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = WW_S32(work, 0x214) / 10;
}

void sort_2_1_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 2 : 9);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_2_2_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 3 : 10);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_3_1_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 4 : 11);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_3_2_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 5 : 12);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_3_3_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 6 : 13);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 6 : 13);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

// 1:1 (the `const` parameters reproduce the original's hoisted `lwz r4,0x8(r4)` prologue load).
int N_compare_func4_1(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as N_compare_func4_1).
int N_compare_func4_1_r(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int N_compare_func4_2(void* a, void* b) {
    s32 id_a = (s16)*(s32*)((u8*)a + 8);
    s32 id_b = (s16)*(s32*)((u8*)b + 8);
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

int N_compare_func4_2_r(void* a, void* b) {
    s32 id_a = (s16)*(s32*)((u8*)a + 8);
    s32 id_b = (s16)*(s32*)((u8*)b + 8);
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
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, (int (*)(void*, void*))N_compare_func4_1);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, (int (*)(void*, void*))N_compare_func4_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

void sort_4_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_2);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

// Bubble sort of the 0xC-byte records at work+0x404 (the item id is the s16 at
// +0x40C). Writing the two record addresses as indices (`work + i*0xC`) rather than
// raw `pa`/`pb` pointers is what makes MWCC strength-reduce them to the target's
// pa=r4/pb=r12 induction variables and colour the item-table base r11; the pointer
// form leaves those in r9/r10/r11. The `SortRec` (3-word) copy `t = *pa; *pa = *pb;
// *pb = t;` reproduces the original's interleaved load/store sequence and its stack
// spills at 0x14/0x18/0x1c exactly.
void sort_4_3_func(WindowWork* work) {
    u8* base = (u8*)work;
    if (WW_S32(work, 0x180) == 0) {
        s32 ida;
        s32 idb;
        s32 i;
        s32 j;
        s32 swap;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xD64)) {
                ida = (s16)*(u32*)(base + i * 0xC + 0x40C);
                idb = (s16)*(u32*)(base + j * 0xC + 0x40C);
                swap = 0;

                if (itemDataTable[ida].bp_cost > itemDataTable[idb].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ida].bp_cost == itemDataTable[idb].bp_cost && itemDataTable[ida].type_sort_order > itemDataTable[idb].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    SortRec t = *(SortRec*)(base + i * 0xC + 0x404);
                    *(SortRec*)(base + i * 0xC + 0x404) = *(SortRec*)(base + j * 0xC + 0x404);
                    *(SortRec*)(base + j * 0xC + 0x404) = t;
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

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xD64)) {
                ida = (s16)*(u32*)(base + i * 0xC + 0x40C);
                idb = (s16)*(u32*)(base + j * 0xC + 0x40C);
                swap = 0;

                if (itemDataTable[ida].bp_cost < itemDataTable[idb].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ida].bp_cost == itemDataTable[idb].bp_cost && itemDataTable[ida].type_sort_order < itemDataTable[idb].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    SortRec t = *(SortRec*)(base + i * 0xC + 0x404);
                    *(SortRec*)(base + i * 0xC + 0x404) = *(SortRec*)(base + j * 0xC + 0x404);
                    *(SortRec*)(base + j * 0xC + 0x404) = t;
                }
                j++;
            }
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lha` prologue load).
int compare_func5_1(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func5_1).
int compare_func5_1_r(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int compare_func5_2(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
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

int compare_func5_2_r(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
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
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, (int (*)(void*, void*))compare_func5_1);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, (int (*)(void*, void*))compare_func5_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

void sort_5_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_2);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

// Same bubble sort as sort_4_3 but over the s16 array at work+0xEB8 (index +0xF0).
// Writing the two record addresses as indices (`work + i*2`) rather than the raw
// `pa`/`pb` pointers lets MWCC strength-reduce them to the same pa=r7/pb=r6
// induction variables the target uses. The per-branch declaration order of the
// locals reproduces the target's register colours (j=r31/swap=r30 in the ascending
// branch, swap=r30/j=r31 in the descending one); the orders differ between the two
// branches only for that reason.
void sort_5_3_func(WindowWork* work) {
    u8* base = (u8*)work;
    if (WW_S32(work, 0x180) == 0) {
        s16 la;
        s16 lb;
        s32 i;
        s32 j;
        s16 ia;
        s16 ib;
        s32 swap;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xE94)) {
                la = *(s16*)(base + i * 2 + 0xEB8);
                lb = *(s16*)(base + j * 2 + 0xEB8);
                ia = (s16)((s32)la + 0xF0);
                ib = (s16)((s32)lb + 0xF0);
                swap = 0;

                if (itemDataTable[ia].bp_cost > itemDataTable[ib].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ia].bp_cost == itemDataTable[ib].bp_cost && itemDataTable[ia].type_sort_order > itemDataTable[ib].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(base + i * 2 + 0xEB8) = lb;
                    *(s16*)(base + j * 2 + 0xEB8) = la;
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

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xE94)) {
                la = *(s16*)(base + i * 2 + 0xEB8);
                lb = *(s16*)(base + j * 2 + 0xEB8);
                ia = (s16)((s32)la + 0xF0);
                ib = (s16)((s32)lb + 0xF0);
                swap = 0;

                if (itemDataTable[ia].bp_cost < itemDataTable[ib].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ia].bp_cost == itemDataTable[ib].bp_cost && itemDataTable[ia].type_sort_order < itemDataTable[ib].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(base + i * 2 + 0xEB8) = lb;
                    *(s16*)(base + j * 2 + 0xEB8) = la;
                }
                j++;
            }
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lha` prologue load).
int compare_func6_1(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func6_1).
int compare_func6_1_r(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int compare_func6_2(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
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

int compare_func6_2_r(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
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
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, (int (*)(void*, void*))compare_func6_1);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, (int (*)(void*, void*))compare_func6_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xF94) = 0;
    WW_S32(work, 0xF98) = WW_S32(work, 0xF94) / 28;
}

void sort_6_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_2);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xF94) = 0;
    WW_S32(work, 0xF98) = WW_S32(work, 0xF94) / 28;
}

int compare_func1(void* a, void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func1_r(void* a, void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

void sort_7_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x1050, WW_S32(work, 0x1038), 2, compare_func1);
    } else {
        qqsort((u8*)work + 0x1050, WW_S32(work, 0x1038), 2, compare_func1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

static inline s32 winRootFindTag(const char* tag, u8* buf) {
    static const char* tbl[] = {
        "nara", "gor",  "tik", "shiga", "hei", "nok", "stn", "gon", "win",
        "hou",  "mri",  "tou", "usu",   "gra", "jin", "muj", "dou", "hom",
        "eki",  "rsh",  "pik", "sin",   "bom", "moo", "aji", "las",
    };
    s32 i;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf, "menu_monosiri_%s", tbl[i]);
        if (strcmp((char*)buf, tag) == 0) {
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
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(((u8*)a)[1]);
    TattleEntry* eb = battleGetUnitMonosiriPtr(((u8*)b)[1]);
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
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(((u8*)a)[1]);
    TattleEntry* eb = battleGetUnitMonosiriPtr(((u8*)b)[1]);
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
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, (int (*)(void*, void*))compare_func2);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, (int (*)(void*, void*))compare_func2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

int compare_func3(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_a, (char*)buf_b);
}

int compare_func3_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_b, (char*)buf_a);
}

void sort_7_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func3);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func3_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lbz r3,0x1(r3)` prologue load).
int compare_func4(const void* a, const void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sa, sb);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func4).
int compare_func4_r(const void* a, const void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sb, sa);
}

void sort_8_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, (int (*)(void*, void*))compare_func4);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, (int (*)(void*, void*))compare_func4_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
}

int compare_func5(void* a, void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_r(void* a, void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

void sort_8_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func5);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func5_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
}

// The two trailing lerps use separate deltas so MWCC keeps the first fmadd's
// difference/accumulator in the original's FPRs.
void winSortMain(WindowWork* work) {
    u32 trg = work->buttonTrg;
    u32 rep = work->dirRep;

    if (WW_S32(work, 0x17C) == 0) {
        trg = 0;
        rep = 0;
    }

    if (trg & 0x200) {
        psndSFXOn((const char*)0x20013);
        WW_S32(work, 0x17C) = 0;
        WW_F32(work, 0x16C) = -600.0f;
    } else if (rep & 0x1000) {
        psndSFXOn((const char*)0x20005);
        WW_S32(work, 0x174) = WW_S32(work, 0x174) - 1;
        if (WW_S32(work, 0x174) < 0) {
            WW_S32(work, 0x174) = WW_S32(work, 0x178) - 1;
        }
    } else if (rep & 0x2000) {
        psndSFXOn((const char*)0x20005);
        WW_S32(work, 0x174) = WW_S32(work, 0x174) + 1;
        if (WW_S32(work, 0x174) >= WW_S32(work, 0x178)) {
            WW_S32(work, 0x174) = 0;
        }
    } else if (trg & 0x100) {
        psndSFXOn((const char*)0x20012);
        sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].func(work);
    } else if (trg & 0x1000) {
        WW_S32(work, 0x17C) = 0;
        WW_F32(work, 0x16C) = -600.0f;
    }

    if (WW_S32(work, 0x17C) != 0) {
        WW_F32(work, 0x158) = 30.0f + WW_F32(work, 0x16C);
        WW_F32(work, 0x15C) = (WW_F32(work, 0x170) - 40.0f) -
                              (f32)(WW_S32(work, 0x174) * 0x28);
        winMsgEntry(work, 0, sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].name, 0);
    }

    {
        WW_F32(work, 0x164) += (WW_F32(work, 0x16C) - WW_F32(work, 0x164)) / 8.0f;
        WW_F32(work, 0x168) += (WW_F32(work, 0x170) - WW_F32(work, 0x168)) / 8.0f;
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
    f32 x = WW_F32(work, 0x164);
    f32 y = WW_F32(work, 0x168);
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
    GXLoadPosMtxImm((const f32(*)[4])((u8*)camGetPtr(8) + 0x11C), 0);
    GXSetCurrentMtx(0);
    if (WW_S32(work, 0x184) == 7) {
        GXSetScissor((u32)(16.0f +
                           (16.0f + (304.0f + WW_F32(work, 0x244)))),
                     (u32)(40.0f + (240.0f - WW_F32(work, 0x24C))), 0x200,
                     0xC0);
    }

    winSortPanel(work, 0xA5, x, y, w, 24.0);
    winSortPanel(work, 0xA3, x + w, y, 24.0f, 24.0);
    for (i = 0; i < WW_S32(work, 0x178); i++) {
        winSortPanel(work, 0xA6, x + w + 8.0f, y - 24.0f - (f32)(i * 0x28), 16.0f, 40.0);
    }
    winSortPanel(work, 0xA4, x + w, y - 24.0f - (f32)(WW_S32(work, 0x178) * 0x28), 24.0f, 24.0);
    winSortPanel(work, 0xA7, x, y - 24.0f - (f32)(WW_S32(work, 0x178) * 0x28), w, 24.0);
    winSortPanel(work, 0xA8, x, y - 24.0f, w2, (f32)(WW_S32(work, 0x178) * 0x28));

    for (i = 0; i < WW_S32(work, 0x178); i++) {
        const SortTable* list = sort_data[WW_S32(work, 0x184)];
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

        if (i == WW_S32(work, 0x174)) {
            f32 row;

            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            row = y - 20.0f - (f32)(i * 0x28) - 16.0f - 8.0f;
            winTexSet(0xB6, (Vec){x + 64.0f + 3.0f + 16.0f, row, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x00, 0x00, 0x00, 0x80});
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            winTexSet_x2(tex, 0xB6, (Vec){x + 64.0f + 16.0f + 4.0f - 6.0f, 6.0f + row, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        } else {
            f32 row;

            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            row = y - 20.0f - (f32)(i * 0x28) - 16.0f - 8.0f;
            winTexSet_x2(tex, 0xB6, (Vec){x + 64.0f + 16.0f + 4.0f, row, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x80, 0x80, 0x80, 0xFF});
        }
    }

    if (WW_S32(work, 0x184) == 7) {
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

    WW_S32(work, 0x184) = idx;
    WW_S32(work, 0x178) = count;
    WW_S32(work, 0x174) = 0;
    WW_S32(work, 0x180) = 0;
    WW_F32(work, 0x164) = -1000.0f;
    WW_F32(work, 0x16C) = a;
    WW_F32(work, 0x168) = b;
    WW_F32(work, 0x170) = b;
    WW_S32(work, 0x17C) = 1;
}

s32 winSortWait(WindowWork* work) {
    return WW_S32(work, 0x17C) != 0;
}

// Kage (paper) window quad. The vertex constants are literals so MWCC reloads each
// through f0 into the FIFO as the original does; the same-literal GXSetFog zero lets
// MWCC land it in f1 after saving x. The int->f32 magic double still pools anonymously.
void winKageGX(WindowWork* work, f32 x, f32 y, f32 w, f32 h, void* color) {
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
    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 0x7A);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(1, *(GXColor*)color);
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

    if (WW_F32(work, 0x244) < 320.0f && WW_F32(work, 0x24C) > -240.0f) {
    winMailGX(work, WW_F32(work, 0x244), WW_F32(work, 0x24C));

    if (WW_S32(work, 0x20C) != 0xCA) {
        for (i = 0; i < WW_S32(work, 0x254); i++) {
            s32 id = *((u8*)work + 0x26F + i * 2);
            s32 num = *((u8*)work + 0x26E + i * 2);
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
                      (Vec){4.0f + (208.0f + WW_F32(work, 0x244)), WW_F32(work, 0x260) + ((WW_F32(work, 0x24C) - 56.0f) - (f32)(i * 40)), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, color);
            white = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            winTexSet((s32)pouchCheckMail(id) == 2 ? WIN_TEX_MAIL_READ : WIN_TEX_MAIL_UNREAD,
                      (Vec){72.0f + WW_F32(work, 0x244), WW_F32(work, 0x260) + (((WW_F32(work, 0x24C) - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winTexSet((num + 1) / 10 + 0x65,
                      (Vec){2.0f + (88.0f + WW_F32(work, 0x244)), WW_F32(work, 0x260) + (((WW_F32(work, 0x24C) - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winTexSet((num + 1) % 10 + 0x65,
                      (Vec){2.0f + (104.0f + WW_F32(work, 0x244)), WW_F32(work, 0x260) + (((WW_F32(work, 0x24C) - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winFontInit();
            winMailLabelV((Vec){120.0f + WW_F32(work, 0x244), WW_F32(work, 0x260) + (8.0f + ((WW_F32(work, 0x24C) - 56.0f) - (f32)(i * 40))), 0.0f},
                          (Vec){0.75f, 0.75f, 0.75f}, black, mail->name);
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    } else {
        s32 sel = WW_S32(work, 0x258);
        s32 num = *((u8*)work + 0x26E + sel * 2);
        GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
        GXColor black = {0x00, 0x00, 0x00, 0xFF};
        WinMailEntry* mail = mailGetPtr(*((u8*)work + 0x26F + sel * 2));
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
                  (Vec){4.0f + (208.0f + WW_F32(work, 0x244)), WW_F32(work, 0x24C) - 56.0f, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, color);
        winTexSet(0x5E,
                  (Vec){72.0f + WW_F32(work, 0x244), WW_F32(work, 0x24C) - 56.0f, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, white);
        winTexSet((num + 1) / 10 + 0x65,
                  (Vec){2.0f + (88.0f + WW_F32(work, 0x244)), (WW_F32(work, 0x24C) - 56.0f) - 3.0f, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, white);
        winTexSet((num + 1) % 10 + 0x65,
                  (Vec){2.0f + (104.0f + WW_F32(work, 0x244)), (WW_F32(work, 0x24C) - 56.0f) - 3.0f, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, white);
        winFontInit();
        winMailLabelV((Vec){120.0f + WW_F32(work, 0x244), 8.0f + (WW_F32(work, 0x24C) - 56.0f), 0.0f},
                      (Vec){0.75f, 0.75f, 0.75f}, black, mail->name);

        // the body, clipped to the page and scrolled by whole lines
        top = 72.0f + (240.0f - WW_F32(work, 0x24C));
        GXSetScissor(32.0f + (304.0f + WW_F32(work, 0x244)), top, 0x158, 0xB1);
        winTexInit(WIN_MAIL_TEX(work));
        winTexSet(0x77,
                  (Vec){4.0f + (208.0f + WW_F32(work, 0x244)), (WW_F32(work, 0x24C) - 56.0f) - 80.0f, 0.0f},
                  (Vec){1.0f, 7.5f, 1.0f}, color);
        winFontInit();
        winMailLabelV((Vec){80.0f + WW_F32(work, 0x244), (WW_F32(work, 0x24C) - 80.0f) + (f32)(WW_S32(work, 0x268) * 21), 0.0f},
                      (Vec){0.75f, 0.75f, 0.75f}, (GXColor){0x00, 0x00, 0x00, 0xFF}, mail->body);
        if (mail->flags & 0x1000) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x2C) + 0xA0));
            winTexSet(mail->picture,
                      (Vec){210.0f + WW_F32(work, 0x244), 7.0f + ((WW_F32(work, 0x24C) - (f32)((*(u16*)((u8*)work + 0x26C) - 1) * 21))
                                                   + (f32)(WW_S32(work, 0x268) * 21)), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);

        // scroll arrows (blinking) and the scroll bar, when the body is longer than a page
        if (*(u16*)((u8*)work + 0x26C) > 8) {
            winIconInit();
            if (WW_S32(work, 0x268) == 0) {
                if ((gp->retraceCount & 0x1F) < 20) {
                    winMailIconV(0x1B6,
                                 (Vec){360.0f + WW_F32(work, 0x244), (20.0f + (WW_F32(work, 0x24C) - 230.0f)) - 24.0f, 0.0f},
                                 (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            } else if (WW_S32(work, 0x268) + 6 == *(u16*)((u8*)work + 0x26C)) {
                if ((gp->retraceCount & 0x1F) < 20) {
                    winMailIconV(0x1B5,
                                 (Vec){360.0f + WW_F32(work, 0x244), 24.0f + (20.0f + (WW_F32(work, 0x24C) - 230.0f)), 0.0f},
                                 (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            } else if ((gp->retraceCount & 0x1F) < 20) {
                winMailIconV(0x1B5,
                             (Vec){360.0f + WW_F32(work, 0x244), 24.0f + (20.0f + (WW_F32(work, 0x24C) - 230.0f)), 0.0f},
                             (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                winMailIconV(0x1B6,
                             (Vec){360.0f + WW_F32(work, 0x244), (20.0f + (WW_F32(work, 0x24C) - 230.0f)) - 24.0f, 0.0f},
                             (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            }
            winMailIconV(0x70,
                         (Vec){360.0f + WW_F32(work, 0x244), 20.0f + (WW_F32(work, 0x24C) - 230.0f), 0.0f},
                         (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        }
    }    }
}

void unk_8014d5dc(s32 unused, WindowWork* work) {
    GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
    GXSetTexCopyDst(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(**(void***)((u8*)work + 0x34), GX_FALSE);
    GXTexModeSync();
    WW_S32(work, 0x38) = 1;
    N_mapDispOff();
    camDispOff(4);
}

// Full-screen textured quad. Writing the vertex constants as literals makes MWCC
// reload them per FIFO store (named externs let it common the load), which is what
// the original does; the local `half` gives the original's value*constant operand
// order. The int->f32 magic double still pools anonymously, which objdiff accepts.

void unk_8014d338(s32 unused, WindowWork* work) {
    GXTexObj texObj;
    f32 w;
    f32 h;

    if (WW_S32(work, 0x38) != 0) {
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
        GXInitTexObj(&texObj, **(void***)((u8*)work + 0x34), gp->fbWidth, gp->efbHeight, 4, 0, 0, 0);
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
