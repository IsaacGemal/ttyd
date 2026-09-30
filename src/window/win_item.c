#include "window/win_item.h"
#include "window/win_root.h"
#include "data/item_data.h"
#include <dolphin/gx.h>
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
extern const f32 float_20_8041d4e8;
extern const f32 float_105_8041d4fc;
extern const f32 float_120_8041d530;
extern const f32 float_150_8041d52c;
extern const f32 float_210_8041d538;
extern const f32 float_215_8041d53c;
extern const f32 float_415_8041d534;
extern const f32 float_4_8041d540;
extern const GXColor dat_8041d468;
extern const GXColor dat_8041d46c;
extern const GXColor dat_8041d470;
extern const GXColor dat_8041d474;
extern const GXColor dat_8041d478;
extern const GXColor dat_8041d47c;
extern const f32 float_100_8041d4e4;
extern const f32 float_10_8041d4ec;
extern const f32 float_240_8041d4f0;
extern const f32 float_125_8041d4f8;
extern const f32 float_250_8041d500;
extern const f32 float_18_8041d504;
extern const f32 float_110_8041d508;
extern const f32 float_113_8041d50c;
extern const f32 float_90_8041d510;
extern const f32 float_15_8041d514;
extern const f32 float_0p7_8041d518;
extern const f32 float_220_8041d51c;
extern const f32 float_92_8041d520;
extern const f32 float_30_8041d524;

extern const Vec vec3_802f5288;
extern const Vec vec3_802f5294;
extern const Vec vec3_802f52a0;
extern const Vec vec3_802f52ac;
extern const Vec vec3_802f52b8;
extern const Vec vec3_802f52c4;
extern const Vec vec3_802f52d0;
extern const Vec vec3_802f52dc;
extern const Vec vec3_802f52e8;
extern const Vec vec3_802f52f4;
extern const Vec vec3_802f5300;
extern const Vec vec3_802f530c;
extern const Vec vec3_802f5318;
extern const Vec vec3_802f5324;
extern const Vec vec3_802f5330;
extern const Vec vec3_802f533c;
extern const Vec vec3_802f5348;
extern const Vec vec3_802f5354;
extern const Vec vec3_802f5360;
extern const Vec vec3_802f536c;
extern const Vec vec3_802f5378;
extern const Vec vec3_802f5384;
extern const Vec vec3_802f5390;
extern const char str_802F55F4[];
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

// The original reaches the six Vecs and the int->float double through one base register
// (r31 = str_802F4CE8) plus displacements, which MWCC only does once win_item's .rodata is
// defined in this unit; declared extern, each symbol gets its own lis/addi.
// NONMATCHING (65.91%): only the .rodata addressing and the register/stack shifts it causes differ.
void winItemDisp(CameraId camId, WindowWork* work, s32 idx) {
    GXColor color = dat_8041d468;
    f32 c;
    f32 b;
    f32 a;
    f32 x = WW_F32(work, idx * 0x14 + 0xC4);
    f32 y = WW_F32(work, idx * 0x14 + 0xC8);
    s32 ofs;
    s32 i;

    winBgGX(work, 2, x, y);
    winKirinukiGX(work, 0, x - float_150_8041d52c, float_20_8041d4e8 + (float_120_8041d530 + y), float_415_8041d534,
                  float_210_8041d538);

    a = float_105_8041d4fc + y;
    b = float_4_8041d540 + a;
    c = (x - float_215_8041d53c) - float_4_8041d540;

    for (i = 0, ofs = 0; i < 2; i++, ofs += 0x28) {
        if (i == WW_S32(work, 0x210)) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            {
                Vec translate = vec3_802f5288;

                translate.x = x - float_215_8041d53c;
                translate.y = float_20_8041d4e8 + (a - (f32)ofs);
                winTexSet(0xB3, translate, vec3_802f5294, dat_8041d46c);
            }
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            {
                Vec translate = vec3_802f52a0;

                translate.x = c;
                translate.y = float_20_8041d4e8 + (b - (f32)ofs);
                winTexSet_x2(i + 0x18, 0xB3, translate, vec3_802f52ac, color);
            }
        } else {
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            {
                Vec translate = vec3_802f52b8;

                translate.x = x - float_215_8041d53c;
                translate.y = float_20_8041d4e8 + (a - (f32)ofs);
                winTexSet_x2(i + 0x18, 0xB3, translate, vec3_802f52c4, dat_8041d470);
            }
        }
    }

    item_disp(work, x, y);
}

// Draws the item window: the held/key-item grid, the selected item's large
// icon and name, the overflow arrows and the sort label. The two int->float pool
// doubles come out as anonymous @NNN@ instead of double_to_int_802f55e0 /
// double_to_int_mask_802f55d8 (the .rodata block is extern in this unit; same
// limitation as winItemDisp).
// NONMATCHING (68.53%): 73 instructions inserted and 54 deleted against the original, plus
// register colouring (work lands in r22 instead of r31).
void item_disp(WindowWork* work, f32 x, f32 y) {
    GXColor white = dat_8041d474;
    GXColor dark = dat_8041d478;
    GXColor gray = dat_8041d47c;
    f32 base = float_100_8041d4e4 + y;
    f32 f22;
    f32 f25;
    f32 f26;
    s32 count;
    s32 i;

    if (WW_S32(work, 0x210) == 0) {
        count = pouchGetHaveItemCnt();
    } else {
        count = WW_S16(work, 0x3DA);
    }

    GXSetScissor(0, 0x6D, 0x260, 0xC0);

    for (i = 0; i < count; i++) {
        s32 item;
        s32 half = i / 2;
        s32 parity = i % 2;
        s32 colored;
        s32 grayFlag;

        if (WW_S32(work, 0x210) == 0) {
            item = pouchHaveItem(i);
        } else {
            item = *(s16*)((u8*)work + i * 2 + 0x2E8);
        }
        f25 = *(f32*)((u8*)work + WW_S32(work, 0x210) * 4 + 0x224);

        colored = (WW_S32(work, 0x210) != 0) || (itemDataTable[item].usable_locations & 4);
        f26 = base - (f32)(half * 0x26);
        grayFlag = (colored == 0);
        f22 = f25 + (float_20_8041d4e8 + (f26 - float_10_8041d4ec));
        if (f22 > float_240_8041d4f0) {
            continue;
        }
        if (f22 < float_neg240_8041d4f4) {
            break;
        }

        if (grayFlag) {
            winIconGrayInit();
        } else {
            winIconInit();
        }

        {
            GXColor iconColor = white;
            Vec translate = vec3_802f52d0;
            Vec scale;
#ifdef MUST_MATCH
            // The original calls winIconSet with by-value Vecs (one copy per struct);
            // the shared header declares Vec* (see report), so copy them here to match.
            Vec tcopy;
            Vec scopy;
#endif

            if (WW_S32(work, 0x210) == 0) {
                scale = vec3_802f52dc;
            } else {
                scale = vec3_802f52e8;
            }

            translate.x = (x - float_125_8041d4f8) + (f32)(parity * 0xCC);
            translate.y = f22;
#ifdef MUST_MATCH
            tcopy = translate;
            scopy = scale;
            winIconSet(*(u16*)&itemDataTable[item].icon_id, &tcopy, &scopy, &iconColor);
#else
            winIconSet(*(u16*)&itemDataTable[item].icon_id, &translate, &scale, &iconColor);
#endif
        }
        winFontInit();
        {
            GXColor color = grayFlag ? gray : dark;
            Vec translate = vec3_802f52f4;
            Vec scale = vec3_802f5300;

            translate.x = (x - float_105_8041d4fc) + (f32)(parity * 0xCC);
            translate.y = f25 + (float_20_8041d4e8 + f26);
            winFontSetPitch(&translate, &scale, &color, float_20_8041d4e8,
                            msgSearch(itemDataTable[item].name));
        }
    }

    GXSetScissor(0, 0, 0x260, 0x1E0);

    {
        if (WW_S32(work, 0x21C + WW_S32(work, 0x210) * 4) > 0) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            f22 = float_250_8041d500 + x;
            {
                Vec translate = vec3_802f530c;
                Vec scale = vec3_802f5318;

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (float_18_8041d504 + (float_110_8041d508 + y));
                winTexSet(0x17, translate, scale, white);
            }
            winIconInit();
            {
                Vec translate = vec3_802f5324;
                Vec scale = vec3_802f5330;
#ifdef MUST_MATCH
                Vec tcopy;
                Vec scopy;
#endif

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (float_113_8041d50c + y);
#ifdef MUST_MATCH
                tcopy = translate;
                scopy = scale;
                winIconSet(0x7E, &tcopy, &scopy, &white);
#else
                winIconSet(0x7E, &translate, &scale, &white);
#endif
            }
        }
        if (count > WW_S32(work, 0x21C + WW_S32(work, 0x210) * 4) * 10 + 10) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            f22 = float_250_8041d500 + x;
            {
                Vec translate = vec3_802f533c;
                Vec scale = vec3_802f5348;

                translate.x = f22;
                translate.y = float_20_8041d4e8 + ((y - float_90_8041d510) - float_15_8041d514);
                winTexSet(0x17, translate, scale, white);
            }
            winIconInit();
            {
                Vec translate = vec3_802f5354;
                Vec scale = vec3_802f5360;
#ifdef MUST_MATCH
                Vec tcopy;
                Vec scopy;
#endif

                translate.x = f22;
                translate.y = float_20_8041d4e8 + (y - float_90_8041d510);
#ifdef MUST_MATCH
                tcopy = translate;
                scopy = scale;
                winIconSet(0x80, &tcopy, &scopy, &white);
#else
                winIconSet(0x80, &translate, &scale, &white);
#endif
            }
        }
    }

    {
        const char* msg = msgSearch(str_802F55F4);
        f32 w = float_0p7_8041d518 * (f32)FontGetMessageWidth(msg);

        winFontInit();
        f22 = x + (float_220_8041d51c - w);
        {
            Vec translate = vec3_802f536c;
            Vec scale = vec3_802f5378;

            translate.x = f22;
            translate.y = float_20_8041d4e8 + ((y - float_92_8041d520) - float_10_8041d4ec);
            winFontSet(&translate, &scale, &dark, msgSearch(str_802F55F4));
        }
        winIconInit();
        {
            Vec translate = vec3_802f5384;
            Vec scale = vec3_802f5390;
#ifdef MUST_MATCH
            Vec tcopy;
            Vec scopy;
#endif

            translate.x = f22 - float_30_8041d524;
            translate.y = float_20_8041d4e8 + ((y - float_100_8041d4e4) - float_10_8041d4ec);
#ifdef MUST_MATCH
            tcopy = translate;
            scopy = scale;
            winIconSet(0x217, &tcopy, &scopy, &white);
#else
            winIconSet(0x217, &translate, &scale, &white);
#endif
        }
    }

    if (WW_F32(work, 0x234) < float_320_8041d528 && WW_F32(work, 0x23C) > float_neg240_8041d4f4) {
        winBookGX(work, 0);
    }
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
