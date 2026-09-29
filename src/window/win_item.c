#include "window/win_item.h"
#include "window/win_root.h"
#include "window/win_main.h"
#include "win/win_main.h"
#include "manager/winmgr.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "system.h"

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x1210 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_S16(w, off) (*(s16*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

extern u16 menu_skip_list[18];
extern const f32 float_190_8041d544;
extern const f32 float_0p25_8041d548;
extern const f32 float_6_8041d54c;
extern const f32 float_neg275_8041d554;
extern const f32 float_320_8041d528;
extern const f32 float_neg240_8041d4f4;
extern const f32 float_0_8041d4ac;
extern const char str_802F560C[];
extern const char str_802F5620[];
extern const char str_802F5638[];
extern const char str_802F55C4[];
extern const f32 float_0p5_8041d4a8;
extern const Vec vec3_802f55ac;
extern const Vec vec3_802f55b8;
extern const GXColor dat_8041d4a4;

typedef struct ItemWorkTail {
    u8 pad[0x2E8];
    u16 skipList[0x79];
    s16 count;
} ItemWorkTail;

static WinMgrDesc window_desc[2];

// NONMATCHING (99.58%): the int->float magic constant is still the local @169@
// instead of double_to_int_802f55e0 (resolves once the pool is complete), and
// three fmadds have their two multiplicands swapped (register allocation).
void winItemMain2(WindowWork* work) {
    WW_F32(work, 0x22C) = float_190_8041d544 * (f32)WW_S32(work, 0x21C);
    WW_F32(work, 0x224) += (WW_F32(work, 0x22C) - WW_F32(work, 0x224)) * float_0p25_8041d548;

    WW_F32(work, 0x230) = float_190_8041d544 * (f32)WW_S32(work, 0x220);
    WW_F32(work, 0x228) += (WW_F32(work, 0x230) - WW_F32(work, 0x228)) * float_0p25_8041d548;

    WW_F32(work, 0x234) += (WW_F32(work, 0x238) - WW_F32(work, 0x234)) / float_6_8041d54c;
    WW_F32(work, 0x23C) += (WW_F32(work, 0x240) - WW_F32(work, 0x23C)) / float_6_8041d54c;
    WW_F32(work, 0x244) += (WW_F32(work, 0x248) - WW_F32(work, 0x244)) / float_6_8041d54c;
    WW_F32(work, 0x24C) += (WW_F32(work, 0x250) - WW_F32(work, 0x24C)) / float_6_8041d54c;

    WW_F32(work, 0x264) = (f32)(WW_S32(work, 0x25C) * 200);
    WW_F32(work, 0x260) += (WW_F32(work, 0x264) - WW_F32(work, 0x260)) * float_0p25_8041d548;
}

void winItemExit(WindowWork* work) {
    winMgrDelete(WW_S32(work, 0x1208));
    winMgrDelete(WW_S32(work, 0x120C));
}

void winItemInit2(WindowWork* work) {
    f32 t;

    WW_F32(work, 0x158) = float_neg275_8041d554;
    WW_F32(work, 0x15C) = (f32)(0x7D - WW_S32(work, 0x210) * 0x28);

    t = float_190_8041d544 * (f32)WW_S32(work, 0x21C);
    WW_F32(work, 0x22C) = t;
    WW_F32(work, 0x224) = t;

    t = float_190_8041d544 * (f32)WW_S32(work, 0x220);
    WW_F32(work, 0x230) = t;
    WW_F32(work, 0x228) = t;

    WW_F32(work, 0x238) = float_320_8041d528;
    WW_F32(work, 0x234) = float_320_8041d528;
    WW_F32(work, 0x240) = float_neg240_8041d4f4;
    WW_F32(work, 0x23C) = float_neg240_8041d4f4;
    WW_F32(work, 0x248) = float_320_8041d528;
    WW_F32(work, 0x244) = float_320_8041d528;
    WW_F32(work, 0x250) = float_neg240_8041d4f4;
    WW_F32(work, 0x24C) = float_neg240_8041d4f4;

    t = (f32)(WW_S32(work, 0x258) * 0xC8);
    WW_F32(work, 0x264) = t;
    WW_F32(work, 0x260) = t;

    winMsgEntry(work, 0, WW_S32(work, 0x210) == 0 ? str_802F560C : str_802F5620, 0);
    WW_S32(work, 0x2D4) = 0;
}

// NONMATCHING (98.90%): instruction sequence matches. Only callee-saved
// register allocation differs: the key-list loop's counter and table pointer land
// in r5/r3 instead of the original's r3/r5 (the mail-loop pointer/index are right).
void winItemInit(WindowWork* work) {
    u8* p;
    s32 i;
    u16 item;
    s32 j;

    WW_S32(work, 0x20C) = 0;
    WW_S32(work, 0x210) = 0;
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = 0;
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = 0;
    WW_F32(work, 0x238) = float_320_8041d528;
    WW_F32(work, 0x234) = float_320_8041d528;
    WW_F32(work, 0x240) = float_neg240_8041d4f4;
    WW_F32(work, 0x23C) = float_neg240_8041d4f4;
    WW_F32(work, 0x248) = float_320_8041d528;
    WW_F32(work, 0x244) = float_320_8041d528;
    WW_F32(work, 0x250) = float_neg240_8041d4f4;
    WW_F32(work, 0x24C) = float_neg240_8041d4f4;

    if ((s32)pouchCheckItem(0x36) == 0) {
        pouchGetItem(0x36);
    }
    WW_S32(work, 0x254) = pouchReceiveMailCount();

    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = 0;
    WW_F32(work, 0x264) = float_0_8041d4ac;
    WW_F32(work, 0x260) = float_0_8041d4ac;

    {
        p = (u8*)work;
        for (i = 0; i < WW_S32(work, 0x254); i++) {
            u8* pouch;
            s32 off;
            p[0x26E] = WW_S32(work, 0x254) - i - 1;
            pouch = (u8*)pouchGetPtr();
            off = 0x519 + (WW_S32(work, 0x254) - i);
            p[0x26F] = pouch[off];
            p += 2;
        }
    }

    WW_S32(work, 0x2D4) = 0;
    WW_S16(work, 0x3DA) = 0;
    for (i = 0; i < 0x79; i++) {
        item = pouchKeyItem(i);

        if (item == 0) {
            continue;
        }
        for (j = 0; j < 0x12; j++) {
            if (item == menu_skip_list[j]) {
                break;
            }
        }
        if ((u32)j >= 0x12) {
            ItemWorkTail* w = (ItemWorkTail*)work;
            w->skipList[w->count] = item;
            w->count++;
        }
    }

    WW_S32(work, 0x1208) = winMgrEntry(&window_desc[0]);
    WW_S32(work, 0x120C) = winMgrEntry(&window_desc[1]);
    winMgrSetPriority(WW_S32(work, 0x1208), -0xC8);
    winMgrSetPriority(WW_S32(work, 0x120C), -0x64);
    fileAsyncf(4, NULL, str_802F5638, getMarioStDvdRoot());
}

void winMakeSkipList(WindowWork* work) {
    s32 i;
    s32 j;

    WW_S16(work, 0x3DA) = 0;
    for (i = 0; i < 0x79; i++) {
        u16 item = pouchKeyItem(i);

        if (item == 0) {
            continue;
        }
        for (j = 0; j < 0x12; j++) {
            if (item == menu_skip_list[j]) {
                break;
            }
        }
        if ((u32)j >= 0x12) {
            ItemWorkTail* w = (ItemWorkTail*)work;
            w->skipList[w->count] = item;
            w->count++;
        }
    }
}

// The original reaches vec3_802f55ac/b8, str_802F4CE8 and the two int->float doubles through one base
// register (r31 = str_802F4CE8) plus displacements (0x8c4, 0x8d0, ...). MWCC only does that when the
// symbols are defined in this unit (known section offsets); declared extern it emits lis/addi per symbol,
// so entry lands in r31 instead of r29 and the frame is 0x60 instead of 0x70. Resolves once win_item's
// .rodata is decompiled.
// NONMATCHING (68.23%): only the .rodata addressing and the register/frame shift it causes differ.
void itemUseDisp2(WinMgrEntry* entry) {
    WindowWork* work = winGetPtr();
    s32 id = WW_S32(work, 0x120C);
    const char* msg;
    f32 width;
    Vec scale;
#ifdef MUST_MATCH
    Vec tmp;
#endif
    Vec pos;
    GXColor color;

    color = dat_8041d4a4;
    if (!winMgrAction(id)) {
        msg = msgSearch(str_802F55C4);
        width = (f32)FontGetMessageWidth(msg);
        winFontInit();
        scale = vec3_802f55b8;
#ifdef MUST_MATCH
        // A separate temp plus the struct copy is what reproduces the original's 0x1c scratch and
        // the lwz/stw (not lfs/stfs) copy of pos into its final slot.
        tmp = vec3_802f55ac;
        tmp.x = (f32)entry->field_0x18 + ((f32)entry->field_0x20 - width) * float_0p5_8041d4a8;
        pos = tmp;
#else
        pos = vec3_802f55ac;
        pos.x = (f32)entry->field_0x18 + ((f32)entry->field_0x20 - width) * float_0p5_8041d4a8;
#endif
        winFontSet(&pos, &scale, &color, msg);
    }
}

static WinMgrDesc window_desc[2] = {
    {
        {0},
        8, // camId
        -0x46,
        0x96,
        0x15E,
        0x108,
        {0xF0, 0xF0, 0xF0, 0xFF},
        NULL,
        (void*)itemUseDisp,
    },
    {
        {0, 0, 0, 2},
        8, // camId
        -0x118,
        0x91,
        0xC8,
        0x32,
        {0xFF, 0x99, 0x00, 0xFF},
        NULL,
        (void*)itemUseDisp2,
    },
};
