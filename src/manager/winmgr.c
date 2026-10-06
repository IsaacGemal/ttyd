#include "manager/fontmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/winmgr.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "memory.h"
#include "system.h"
#include "data/item_data.h"
#include <stdio.h>
#include <string.h>
#include "driver/windowdrv.h"
#include "mario/mario_party.h"
#include "driver/msgdrv.h"
#include "driver/icondrv.h"
#include "mario/mariost.h"
#include "evt/evt_lottery.h"
#include "evt/evt_johoya.h"
#include "evt/evt_badgeshop.h"
#include "window/win_item.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "window/win_main.h"

// MUST_MATCH: preserve named scalar loads after their definitions instead of folding them to literals.
#ifdef MUST_MATCH
#define WINMGR_F32(name) (*(f32*)&(name))
#else
#define WINMGR_F32(name) (name)
#endif

extern u16 menu_skip_list[]; // Keep unsized: retail forms its address with lis/addi instead of @sda21.
extern ItemData itemDataTable[];

typedef struct WinMgrSelectTbl {
    u16 numDescriptors;
    WinMgrDesc* descriptors;
} WinMgrSelectTbl;

typedef struct WinMgrSelectItem {
    u16 field_0x0;
    u16 field_0x2;
} WinMgrSelectItem;

#define PI 3.14159265358979323846264338327950288419716939937510

// Retain the descriptor tables and constants referenced by other units.
#pragma force_active on
// Named read-only objects follow the original small-data order.
const char str_804219B8[] = "_kur";
const char str_804219C0[] = "_nok";
const char str_804219C8[] = "_win";
const char str_804219D0[] = "_yos";
const char str_804219D8[] = "_viv";
const char str_804219E0[] = "_bom";
const char str_804219E8[] = "_chu";
const char str_804219F0[] = "_hp";
const char str_804219F4[] = "_fp";
const char str_804219F8[] = "_bp";
const char str_804219FC[] = "\x82\x67\x82\x6f";
const char str_80421A04[] = "\x82\x65\x82\x6f";
const char str_80421A0C[] = "\x82\x61\x82\x6f";
const GXColor dat_80421a14 = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor dat_80421a18 = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor dat_80421a1c = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor dat_80421a20 = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor dat_80421a24 = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor dat_80421a28 = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_80421a2c = { 0xA0, 0xA0, 0xA0, 0xFF };
const GXColor dat_80421a30 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a34 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a38 = { 0xF0, 0xF0, 0xF0, 0xFF };
const GXColor dat_80421a3c = { 0xF0, 0xF0, 0xF0, 0xFF };
const GXColor dat_80421a40 = { 0xF0, 0xF0, 0xF0, 0xFF };
const GXColor dat_80421a44 = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_80421a48 = { 0xA0, 0xA0, 0xA0, 0xFF };
const GXColor dat_80421a4c = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a50 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a54 = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_80421a58 = { 0xA0, 0xA0, 0xA0, 0xFF };
const GXColor dat_80421a5c = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a60 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a64 = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_80421a68 = { 0xA0, 0xA0, 0xA0, 0xFF };
const GXColor dat_80421a6c = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a70 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80421a74 = { 0xF0, 0xF0, 0xF0, 0xFF };
const char str_80421A78[] = "help";


static WinMgrWork work;
static s16* mario_status_point_table[3];
static WinMgrWork* wp = &work;
u16 party_id_table[8] = { 1, 2, 5, 4, 6, 3, 7, 0 };
static u16 party_icon_table[8] = { 0x155, 0x156, 0x15F, 0x158, 0x160, 0x157, 0x161, 0 };
static const char* party_labelname_table[7] = { str_804219B8, str_804219C0, str_804219C8, str_804219D0, str_804219D8, str_804219E0, str_804219E8 };
static s32 mario_status_icon_table[3] = { 0x19F, 0x1A0, 0x1A1 };
static const char* mario_status_labelname_table[3] = { str_804219F0, str_804219F4, str_804219F8 };
static const char* mario_status_name_table[3] = { str_804219FC, str_80421A04, str_80421A0C };
static s32 mario_status_henka_table[3] = { 5, 5, 3 };
s32 mario_status_max_table[3] = { 200, 200, 99 };

WinMgrDesc select_desc[] = {
    { { 0, 0, 0, 0, 0, 0, 0, 1 }, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp },
    { { 0, 0, 0, 2, 0, 0, 0, 0 }, 8, -280, 120, 240, 45, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3 },
    { { 0, 0, 0, 0, 0, 0, 0, 1 }, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp },
    { { 0, 0, 0, 2, 0, 0, 0, 0 }, 8, -280, 120, 240, 69, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3 },
};

WinMgrDesc select_party_desc[] = {
    { { 0, 0, 0, 0, 0, 0, 0, 4 }, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_party },
    { { 0, 0, 0, 2, 0, 0, 0, 0 }, 8, -280, 120, 240, 69, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3_party },
};

WinMgrDesc select_mario_desc[] = {
    { { 0, 0, 0, 0, 0, 0, 0, 5 }, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_mario },
    { { 0, 0, 0, 2, 0, 0, 0, 0 }, 8, -280, 120, 240, 69, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3_mario },
};

WinMgrDesc select_luigi_desc[] = {
    { { 0, 0, 0, 0, 0, 0, 0, 1 }, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_luigi },
    { { 0, 0, 0, 2, 0, 0, 0, 0 }, 8, -280, 120, 240, 45, { 255, 255, 255, 255 }, NULL, select_disp2 },
};

WinMgrDesc select_johoya_perfect_desc[] = {
    { { 0, 0, 0, 0, 0, 0, 0, 1 }, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_luigi },
    { { 0, 0, 0, 2, 0, 0, 0, 0 }, 8, -280, 120, 240, 45, { 255, 255, 255, 255 }, NULL, select_disp2 },
};

static WinMgrSelectTbl select_desc_tbl[] = {
    { 3, select_desc },
    { 3, select_desc },
    { 3, select_desc },
    { 3, select_desc },
    { 3, select_desc },
    { 3, select_desc },
    { 3, select_party_desc },
    { 3, select_party_desc },
    { 3, select_party_desc },
    { 3, select_mario_desc },
    { 3, select_mario_desc },
    { 3, select_desc },
    { 3, select_desc },
    { 3, &select_desc[3] },
    { 3, select_desc },
    { 3, select_desc },
    { 3, &select_desc[3] },
    { 2, select_johoya_perfect_desc },
    { 2, select_luigi_desc },
};

// Vector and message data share the retail rodata base.
const Vec vec3_802ffa98 = { 0, 0, 0 };
const Vec vec3_802ffaa4 = { 0.5, 0.5, 0.5 };
const Vec vec3_802ffab0 = { 0, 0, 0 };
const Vec vec3_802ffabc = { 1, 1, 1 };
const Vec vec3_802ffac8 = { 0, 0, 0 };
const Vec vec3_802ffad4 = { 1, 1, 1 };
// Keep these existing non-const vectors in rodata: retail reloads them after winIconSet in each loop.
__declspec(section ".rodata") Vec vec3_802ffae0 = { 0, 0, 0 };
__declspec(section ".rodata") Vec vec3_802ffaec = { 0.15, 0.15, 0.15 };
const Vec vec3_802ffaf8 = { 0, 0, 0 };
const Vec vec3_802ffb04 = { 1, 1, 1 };
const Vec vec3_802ffb10 = { 0, 0, 0 };
const Vec vec3_802ffb1c = { 0, 0, 0 };
const Vec vec3_802ffb28 = { 0, 0, 0 };
const Vec vec3_802ffb34 = { 0, 0, 1 };
const Vec vec3_802ffb40 = { 0, 0, 1 };
const Vec vec3_802ffb4c = { 0, 0, 1 };
const Vec vec3_802ffb58 = { 0, 0, 0 };
const Vec vec3_802ffb64 = { 0, 1, 1 };
const Vec vec3_802ffb70 = { 0, 0, 0 };
const Vec vec3_802ffb7c = { 0.55, 0.55, 0.55 };
const Vec vec3_802ffb88 = { 0, 0, 0 };
const Vec vec3_802ffb94 = { 0.55, 0.55, 0.55 };
const Vec vec3_802ffba0 = { 0, 0, 0 };
const Vec vec3_802ffbac = { 1, 1, 1 };
const Vec vec3_802ffbb8 = { 0, 0, 0 };
const Vec vec3_802ffbc4 = { 1, 1, 1 };
const Vec vec3_802ffbd0 = { 0, 0, 1 };
const Vec vec3_802ffbdc = { 0, 0, 1 };
const Vec vec3_802ffbe8 = { 0, 0, 1 };
const Vec vec3_802ffbf4 = { 0, 0, 0 };
const Vec vec3_802ffc00 = { 1, 1, 1 };
const Vec vec3_802ffc0c = { 0, 0, 0 };
const Vec vec3_802ffc18 = { 0.55, 0.55, 0.55 };
const Vec vec3_802ffc24 = { 0, 0, 0 };
const Vec vec3_802ffc30 = { 1, 1, 1 };
const Vec vec3_802ffc3c = { 0, 0, 0 };
const Vec vec3_802ffc48 = { 1, 1, 1 };
const Vec vec3_802ffc54 = { 0, 0, 0 };
const Vec vec3_802ffc60 = { -0.55, 0.55, 0.55 };
const Vec vec3_802ffc6c = { 0, 0, 0 };
const Vec vec3_802ffc78 = { 1, 1, 1 };
const Vec vec3_802ffc84 = { 0, 0, 0 };
const Vec vec3_802ffc90 = { 1, 1, 1 };
const Vec vec3_802ffc9c = { 0, 0, 1 };
const Vec vec3_802ffca8 = { 0, 0, 1 };
const Vec vec3_802ffcb4 = { 0, 0, 1 };
const Vec vec3_802ffcc0 = { 0, 0, 0 };
const Vec vec3_802ffccc = { 0.8, 0.8, 0.8 };
const Vec vec3_802ffcd8 = { 0, 0, 0 };
const Vec vec3_802ffce4 = { 1, 1, 1 };
const Vec vec3_802ffcf0 = { 0, 0, 0 };
const Vec vec3_802ffcfc = { 1, 1, 1 };
const Vec vec3_802ffd08 = { 0, 0, 0 };
const Vec vec3_802ffd14 = { 0, 0, 1 };
const Vec vec3_802ffd20 = { 0, 0, 1 };
const Vec vec3_802ffd2c = { 0, 0, 1 };
const char str_802FFD38[] = "name_party0";
const char str_802FFD44[] = "name_party1";
const char str_802FFD50[] = "name_party4";
const char str_802FFD5C[] = "name_party5";
const char str_802FFD68[] = "name_party2";
const char str_802FFD74[] = "name_party6";
const char str_802FFD80[] = "helperr2\000\000\000";
const char str_802FFD8C[] = "helperr";
const char str_802FFD94[] = "%s_levelup%s\000\000\000";
const char str_802FFDA4[] = "%s_leveldown%s\000";
const char str_802FFDB4[] = "lankup\000";
const char str_802FFDBC[] = "lankdown\000\000\000";
const char str_802FFDC8[] = "%s_%s_lank%d%s\000";
const char str_802FFDD8[] = "msg_window_select_1";
const char str_802FFDEC[] = "msg_window_select_2";
const char str_802FFE00[] = "msg_window_select_3";
const char str_802FFE14[] = "msg_window_select_4";
const char str_802FFE28[] = "msg_window_select_11\000\000\000";
const char str_802FFE40[] = "msg_window_select_12\000\000\000";
const char str_802FFE58[] = "msg_window_select_9";
const char str_802FFE6C[] = "msg_window_select_9_1\000\000";
const char str_802FFE84[] = "msg_window_select_6";
const char str_802FFE98[] = "msg_window_select_10\000\000\000";
const char str_802FFEB0[] = "msg_window_select_8";
const char str_802FFEC4[] = "msg_window_select_7";
const char str_802FFED8[] = "msg_window_title_5\000";
const char str_802FFEEC[] = "msg_window_title_6\000";

#ifdef MUST_MATCH
// MUST_MATCH: this inline keeps the descriptor 0x14 load before 0x10 with retail registers.
static inline s32 get10(WinMgrDesc* d) { return d->field_0x10; }
#endif

static inline void winMgrDispWaku1(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->field_0x18 + (entry->field_0x20 - 150) / 2),
                           (f32)(entry->field_0x1C + 14), 150.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku3a(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->field_0x18 + 10), (f32)(entry->field_0x1C + 14),
                           100.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku3b(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->field_0x18 + entry->field_0x20 / 2 - 10),
                           (f32)(entry->field_0x1C + 14), 100.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku4(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->field_0x18 + (entry->field_0x20 * 3 / 5 - 120) * 2 / 3),
                           (f32)(entry->field_0x1C + 14), 120.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku5(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(mtx, 0, color,
                           (f32)(entry->field_0x18 + entry->field_0x20 * 3 / 5 + (entry->field_0x20 * 2 / 5 - 85) / 3),
                           (f32)(entry->field_0x1C + 14), 85.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku0(Mtx mtx, WinMgrEntry* entry) {
    GXColor color = { 0, 0, 0, 0 };
    color.r = entry->description->field_0x1C[0];
    color.g = entry->description->field_0x1C[1];
    color.b = entry->description->field_0x1C[2];
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)entry->field_0x18, (f32)entry->field_0x1C,
                           (f32)entry->field_0x20, (f32)entry->field_0x24, 20.0f);
}

// Emit the display prefix immediately so its conversions and literals pool in retail order.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

void select_disp3_mario(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;

    pouchGetPtr();
    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        const char* help;
        char buf[0x48];

        if (items[select->field_0xC].field_0x0 & 2) {
            if (items[select->field_0xC].field_0x0 & 4) {
                help = str_802FFD80;
            } else {
                help = str_802FFD8C;
            }
        } else {
            help = str_80421A78;
        }
        switch (select->field_0x4) {
        case 9:
            sprintf(buf, str_802FFD94, help, mario_status_labelname_table[items[select->field_0xC].field_0x2]);
            break;
        case 10:
        default:
            sprintf(buf, str_802FFDA4, help, mario_status_labelname_table[items[select->field_0xC].field_0x2]);
            break;
        }
        entry->field_0x34 = (s32)msgSearch(buf);
        winMgrHelpDraw(entry);
    }
}

void select_disp3_party(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    PouchData* pouch = pouchGetPtr();
    u16 id;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        const char* help;
        const char* lank;
        char buf[0x48];

        if (items[select->field_0xC].field_0x0 & 2) {
            help = str_802FFD8C;
        } else {
            help = str_80421A78;
        }
        if (select->field_0x4 == 6 || select->field_0x4 == 7) {
            lank = str_802FFDB4;
        } else {
            lank = str_802FFDBC;
        }
        id = items[select->field_0xC].field_0x2;
        sprintf(buf, str_802FFDC8, help, lank,
                pouch->partyData[party_id_table[id]].techLevel + 1, party_labelname_table[id]);
        entry->field_0x34 = (s32)msgSearch(buf);
        winMgrHelpDraw(entry);
    }
}

void select_disp3(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        u16 id = items[select->field_0xC].field_0x2;
        ItemData* item = &itemDataTable[id];

        entry->field_0x34 = (s32)msgSearch(item->description);
        winMgrHelpDraw(entry);
    }
}

void select_main3(WinMgrEntry* entry) {
    u16 width = 0;

    if (!(entry->flags & 4)) {
        if ((const char*)entry->field_0x34 != NULL) {
            FontGetMessageWidthLine((const char*)entry->field_0x34, &width);
            entry->field_0x3C = width + 1;
        }
        if (entry->field_0x3C > 2) {
            if (keyGetDirTrg(0) & 0x100) {
                entry->field_0x38 -= 2;
                if (entry->field_0x38 < 0) {
                    entry->field_0x38 = 0;
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if ((entry->field_0x3C + 1) / 2 - 1 > (entry->field_0x38 + 1) / 2) {
                    entry->field_0x38 += 2;
                }
            }
        }
    }
}

void select_disp2(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    const char* msg;
    u16 width;
    u16 x;

    msg = msgSearch(str_802FFDD8);
    switch (select->field_0x4) {
    case 3:
    case 12:
        msg = msgSearch(str_802FFDEC);
        break;
    case 4:
        msg = msgSearch(str_802FFE00);
        break;
    case 5:
        msg = msgSearch(str_802FFE14);
        break;
    case 6:
    case 7:
        msg = msgSearch(str_802FFE28);
        break;
    case 8:
        msg = msgSearch(str_802FFE40);
        break;
    case 9:
        msg = msgSearch(str_802FFE58);
        break;
    case 10:
        msg = msgSearch(str_802FFE6C);
        break;
    case 11:
    case 14:
    case 15:
        msg = msgSearch(str_802FFE84);
        break;
    case 13:
    case 16:
        msg = msgSearch(str_802FFE98);
        break;
    case 17:
        msg = msgSearch(str_802FFEB0);
        break;
    case 18:
        msg = msgSearch(str_802FFEC4);
        break;
    }
    if (!(wp->mEntries[select->descriptorId[1]].flags & 4)) {
        x = (entry->field_0x20 - FontGetMessageWidthLine(msg, &width)) / 2;

        FontDrawStart();
        FontDrawMessage(entry->field_0x18 + x,
                        entry->field_0x1C - (entry->field_0x24 - (width + 1) * 24) / 2, msg);
    }
}

// The first native signed int-to-float conversion pools the .rodata+0x468 double here.
void select_disp_luigi(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    s32 i;
    InformantInfo* info;
    Mtx mtx1;
    Mtx mtx2;
    char buf[0x20];
    Vec posRow;
    Vec tmpRow;
    Vec scaleRow;
    Vec posEdge;
    Vec tmpEdge;
    Vec scaleEdge;
    Vec posTitle;
    Vec tmpTitle;
    Vec scaleTitle;
    Vec posLuigi;
    Vec posMain;
    Vec posUp;
    Vec posDown;
    u32 scissorLeft;
    u32 scissorTop;
    u32 scissorWidth;
    u32 scissorHeight;
    GXColor colorBlack = dat_80421a64;
    GXColor colorGray = dat_80421a68;
    GXColor colorWhite = dat_80421a6c;
    GXColor fogColor;
    InformantWork* jdt = &_jdt;
    const char* msg;
    u16 width;
    u16 x;
    f32 zero;

    if (wp->mEntries[select->descriptorId[0]].flags & 4) {
        return;
    }

    fogColor = dat_80421a70;
#ifdef MUST_MATCH
    // MUST_MATCH: the original stores the fog colour in fogColor's own slot (0x18) and passes a
    // pointer to it; with the by-value GXSetFog prototype MWCC copies it to a call temp at 0xc and
    // the loop/edge/title colour locals move to 0x18/0x14/0x10. The cast only changes the calling
    // convention (GXSetFog reads the colour, never writes it) and reproduces the original slots.
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(0, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->field_0x18 + 0x130, 0x112 - entry->field_0x1C, entry->field_0x20, entry->field_0x24 - 0x32);

    zero = 0.0f;
    info = jdt->info;
    for (i = 0; i < select->field_0x34; i++, info++) {
        char saved;
        f32 row = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
        f32 bound = select->field_0x1C + (zero + row);

        if ((f32)entry->field_0x1C < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->field_0x1C - entry->field_0x24) > bound + 32.0f) {
            continue;
        }

        winFontInit();
        saved = ((char*)info->initEndPos)[info->tagEndPos];
        ((char*)info->initEndPos)[info->tagEndPos] = 0;
        {
            GXColor color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
            f32 rowY;

            tmpRow = vec3_802ffcc0;
            scaleRow = vec3_802ffccc;
            tmpRow.x = (f32)(entry->field_0x18 + 0x19);
            rowY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpRow.y = select->field_0x1C + (zero + rowY);
            posRow = tmpRow;
            winFontSet(&posRow, &scaleRow, &color, (const char*)info->initEndPos);
        }
        ((char*)info->initEndPos)[info->tagEndPos] = saved;

        if (select->field_0x4 == 0x11 && !(((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1)) {
            GXColor color;
            f32 edgeY;

            sprintf(buf, "%3d", info->unkA);
            winFontInit();
            color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
            tmpEdge = vec3_802ffcd8;
            scaleEdge = vec3_802ffce4;
            tmpEdge.x = (f32)(entry->field_0x18 + 0xf5);
            edgeY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpEdge.y = select->field_0x1C + (zero + edgeY);
            posEdge = tmpEdge;
            winFontSet(&posEdge, &scaleEdge, &color, buf);
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    switch (select->field_0x4) {
    case 0x11:
        msg = msgSearch(str_802FFED8);
        break;
    case 0x12:
    default:
        msg = msgSearch(str_802FFEEC);
        break;
    }

    width = FontGetMessageWidth(msg);
    x = (entry->field_0x20 - width) / 2;
    winFontInit();
    {
        GXColor color = colorWhite;
        tmpTitle = vec3_802ffcf0;
        scaleTitle = vec3_802ffcfc;
        tmpTitle.x = (f32)(entry->field_0x18 + x);
        tmpTitle.y = (f32)(entry->field_0x1C + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

    switch (select->field_0x4) {
    case 0x11:
        PSMTXTrans(mtx1, (f32)(entry->field_0x18 + 0x10c), (f32)(entry->field_0x1C - 0x12), 0.0f);
        PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
        PSMTXConcat(mtx1, mtx2, mtx1);
        iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a74);
        posLuigi = vec3_802ffd08;
        posLuigi.x = (f32)(entry->field_0x18 + 0x10c);
        posLuigi.y = (f32)(entry->field_0x1C - 0xc);
        iconDispGx(&posLuigi, 0x10, 0x18b, 0.6f);
        break;
    }

    posMain = vec3_802ffd14;
    posMain.x = select->field_0x14;
    posMain.y = select->field_0x18;
    iconDispGx(&posMain, 0x14, 0x1f0, 1.0f);

    if (select->field_0x34 > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->field_0x10 != 0) {
            posUp = vec3_802ffd20;
            // Division by two pools 0.5f while preserving retail multiplication operand order.
            posUp.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posUp.y = (f32)(entry->field_0x1C - 0x24);
            iconDispGx(&posUp, 0x10, 0x1b5, 0.6f);
        }
        if (select->field_0x10 != select->field_0x34 - 8) {
            posDown = vec3_802ffd2c;
            posDown.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posDown.y = (f32)(entry->field_0x1C - entry->field_0x24 - 0xc);
            iconDispGx(&posDown, 0x10, 0x1b6, 0.6f);
        }
    }
}

const char str_802FFF08[] = "msg_window_title_9\000";
const char str_802FFF1C[] = "msg_window_title_10";
const char str_802FFF30[] = "msg_window_title_7\000";
const char str_802FFF44[] = "msg_window_title_8\000";

// Every call, constant, branch, stack slot and store order matches. The (f32)int conversion double
// is pooled at .rodata+0x468 by select_disp_luigi's native conversion. The outer loop
// keeps one induction variable and writes the row as `i * 0x18`, so the compiler strength-reduces
// both offsets from one base and initialises the row offset with a copy (`mr r27,r28`). The
// `if (x == width) { x = x; }` guards after the two title x computations are dead (x is not
// changed) but keep those divisions scheduled before the following winFontInit, as in the target.
// The target's dead `lwz r0,0x38(r24); cmpwi r0,0` before the first winFontInit is reproduced by
// `if (select->field_0x38) { zero = zero; }` (the dead store is removed, the compare stays).
void select_disp_mario(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    s32 i;
    Vec posName, tmpName, scaleName;
    Vec posIcon, tmpIcon, scaleIcon;
    Vec posPoint, tmpPoint, scalePoint;
    Vec posHenka, tmpHenka, scaleHenka;
    Vec posIcon2, tmpIcon2, scaleIcon2;
    Vec posTitle, tmpTitle, scaleTitle;
    Vec posTitle2, tmpTitle2, scaleTitle2;
    Vec posMain;
    Vec posUp;
    Vec posDown;
    u32 scissorLeft;
    u32 scissorTop;
    u32 scissorWidth;
    u32 scissorHeight;
    char buf[4];
    GXColor colorBlack;
    GXColor colorGray;
    GXColor colorWhite;
    GXColor colorIcon;
    GXColor colorTitle;
    GXColor fogColor;
    const char* msg;
    u16 width;
    u16 x;
    f32 zero;
    s32 status;

    pouchGetPtr();
    colorBlack = dat_80421a54;
    colorGray = dat_80421a58;
    colorWhite = dat_80421a5c;
    colorTitle = dat_80421a5c;
    colorIcon = dat_80421a5c;

    if (wp->mEntries[select->descriptorId[0]].flags & 4) {
        return;
    }

    fogColor = dat_80421a60;
#ifdef MUST_MATCH
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(0, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->field_0x18 + 0x130, 0x112 - entry->field_0x1C, entry->field_0x20, entry->field_0x24 - 0x32);

    zero = 0.0f;
    for (i = 0; i < select->field_0x34; i++) {
        f32 row;
        f32 bound;

        row = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
        bound = select->field_0x1C + (zero + row);
        status = ((WinMgrSelectItem*)select->field_0x30)[i].field_0x2;
        if ((f32)entry->field_0x1C < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->field_0x1C - entry->field_0x24) > bound + 32.0f) {
            continue;
        }

        if (*(volatile s32*)&select->field_0x38) {
            zero = zero;
        }
        winFontInit();
        {
            GXColor color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
            f32 rowY;

            tmpName = vec3_802ffbf4;
            scaleName = vec3_802ffc00;
            tmpName.x = (f32)(entry->field_0x18 + 0x3c);
            rowY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpName.y = select->field_0x1C + (zero + rowY);
            posName = tmpName;
            winFontSet(&posName, &scaleName, &color, mario_status_name_table[status]);
        }

        if (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        {
            GXColor color = colorIcon;
            f32 rowY;

            tmpIcon = vec3_802ffc0c;
            scaleIcon = vec3_802ffc18;
            tmpIcon.x = (f32)(entry->field_0x18 + 0x23);
            rowY = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
            tmpIcon.y = select->field_0x1C + (zero + rowY);
            posIcon = tmpIcon;
            winIconSet(mario_status_icon_table[status], &posIcon, &scaleIcon, &color);
        }

        sprintf(buf, "%3d", mario_status_point_table[status][0]);

        winFontInit();
        {
            GXColor color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
            f32 rowY;

            tmpPoint = vec3_802ffc24;
            scalePoint = vec3_802ffc30;
            tmpPoint.x = (f32)(entry->field_0x18 + 0xa0);
            rowY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpPoint.y = select->field_0x1C + (zero + rowY);
            posPoint = tmpPoint;
            winFontSet(&posPoint, &scalePoint, &color, buf);
        }

        if (!(((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 2)) {
            switch (select->field_0x4) {
            case 9:
                sprintf(buf, "%3d", mario_status_point_table[status][0] + mario_status_henka_table[status]);
                break;
            case 10:
            default:
                sprintf(buf, "%3d", mario_status_point_table[status][0] - mario_status_henka_table[status]);
                break;
            }

            winFontInit();
            {
                GXColor color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
                f32 rowY;

                tmpHenka = vec3_802ffc3c;
                scaleHenka = vec3_802ffc48;
                tmpHenka.x = (f32)(entry->field_0x18 + 0xf0);
                rowY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
                tmpHenka.y = select->field_0x1C + (zero + rowY);
                posHenka = tmpHenka;
                winFontSet(&posHenka, &scaleHenka, &color, buf);
            }

            winIconInit();
            {
                GXColor color = colorIcon;
                f32 rowY;

                tmpIcon2 = vec3_802ffc54;
                scaleIcon2 = vec3_802ffc60;
                tmpIcon2.x = (f32)(entry->field_0x18 + 0xdc);
                rowY = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
                tmpIcon2.y = select->field_0x1C + (zero + rowY);
                posIcon2 = tmpIcon2;
                winIconSet(0x163, &posIcon2, &scaleIcon2, &color);
            }
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    msg = msgSearch(str_802FFF08);
    width = FontGetMessageWidth(msg);
    x = (0x78 - width) / 2;
    // Dead guard (x is unchanged); it keeps this division scheduled before winFontInit, as the target has it.
    if (x == width) {
        x = x;
    }
    winFontInit();
    {
        GXColor color = colorTitle;

        tmpTitle = vec3_802ffc6c;
        scaleTitle = vec3_802ffc78;
        tmpTitle.x = (f32)(entry->field_0x18 + (entry->field_0x20 * 3 / 5 - 0x78) * 2 / 3 + x);
        tmpTitle.y = (f32)(entry->field_0x1C + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

    msg = msgSearch(str_802FFF1C);
    width = FontGetMessageWidth(msg);
    x = (0x55 - width) / 2;
    // Dead guard (x is unchanged); it keeps this division scheduled before winFontInit, as the target has it.
    if (x == width) {
        x = x;
    }
    winFontInit();
    {
        GXColor color = colorTitle;

        tmpTitle2 = vec3_802ffc84;
        scaleTitle2 = vec3_802ffc90;
        tmpTitle2.x = (f32)(entry->field_0x18 + entry->field_0x20 * 3 / 5 + (entry->field_0x20 * 2 / 5 - 0x55) / 3 + x);
        tmpTitle2.y = (f32)(entry->field_0x1C + 0xe);
        posTitle2 = tmpTitle2;
        winFontSetEdge(&posTitle2, &scaleTitle2, &color, msg);
    }

    posMain = vec3_802ffc9c;
    posMain.x = select->field_0x14;
    posMain.y = select->field_0x18;
    iconDispGx(&posMain, 0x14, 0x1f0, 1.0f);

    if (select->field_0x34 > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->field_0x10 != 0) {
            posUp = vec3_802ffca8;
            posUp.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posUp.y = (f32)(entry->field_0x1C - 0x24);
            iconDispGx(&posUp, 0x10, 0x1b5, 0.6f);
        }
        if (select->field_0x10 != select->field_0x34 - 8) {
            posDown = vec3_802ffcb4;
            posDown.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posDown.y = (f32)(entry->field_0x1C - entry->field_0x24 - 0xc);
            iconDispGx(&posDown, 0x10, 0x1b6, 0.6f);
        }
    }
}


// Complete reconstruction; all 48 calls, the four jump tables, the techLevel loop bound
// (recomputed every arrow iteration) and the name-switch phi match. Two target behaviours had
// to be reproduced explicitly: (1) the name/icon switches have no `default:` (a value > 6 falls
// through with the result variable uninitialised), and that is what makes MWCC materialise each
// switch result in r23/r24/r25 (a `mr` per case plus the join `mr r3,r25`); merging case 6 with
// default coalesces them away. (2) The target reloads several in-loop vec constants every
// iteration where MWCC would hoist them: vec3_802ffb7c/88/94 are read through `const volatile` (assignments marked "anti-hoist"
// below), which restores the reloads and the target's callee-saved set (stmw r17). The per-block
// statement order is also chosen to match the target's schedule: the icon block computes
// tmpIcon.x before rowY, the arrow block computes tmpArrow.x before rowY, the arrow loop declares
// `off` before `k`, and both loop increments are ordered to match the target's `addi` sequence.
// The frame (0x1d0), the callee-saved set (stmw r17) and every stack slot match; the icon block and
// both title blocks match register for register. The .rodata i2f pool double is placed at the
// original's +0x468 by select_disp_luigi's native conversion. The colorWhite/colorWhite2 store order is
// fixed by declaring both GXColor bare and assigning them in reverse at the top of the body; the
// first title reads colorWhite2 (same constant as colorWhite) so both titles load 0x24 like the
// target.
// The outer loop keeps one induction variable (`for (i = 0; ...)`) and writes the row as `i * 0x18`;
// that is what makes the compiler strength-reduce both offsets from one base and initialise the item
// offset with a copy (`mr r28,r27`) instead of a shift. The `if (x == width) { x = x; }` guards
// after the two title x computations are dead (x is not changed) but keep those divisions scheduled
// before the following winFontInit, as in the target. The target's dead `lwz r0,0x38(r30); cmpwi
// r0,0x0` before the first winFontInit is reproduced by `if (select->field_0x38) { zero = zero; }`
// (the dead store is removed, the compare stays).
void select_disp_party(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    PouchData* pouch = pouchGetPtr();
    s32 i;
    Vec posName, tmpName, scaleName, scaleTmp;
    Vec posIcon, tmpIcon, scaleIcon;
    Vec posArrow, tmpArrow, scaleArrow;
    Vec posTitle, tmpTitle, scaleTitle;
    Vec posTitle2, tmpTitle2, scaleTitle2;
    Vec posMain;
    Vec posUp;
    Vec posDown;
    u32 scissorLeft;
    u32 scissorTop;
    u32 scissorWidth;
    u32 scissorHeight;
    GXColor colorBlack = dat_80421a44;
    GXColor colorGray = dat_80421a48;
    GXColor colorWhite;
    GXColor colorWhite2;
    GXColor fogColor;
    const char* msg;
    u16 width;
    u16 x;
    f32 zero;
    f32 scale;

    colorWhite2 = dat_80421a4c;
    colorWhite = dat_80421a4c;
    if (wp->mEntries[select->descriptorId[0]].flags & 4) {
        return;
    }

    fogColor = dat_80421a50;
#ifdef MUST_MATCH
    // MUST_MATCH: the original stores the fog colour in fogColor's own slot and passes a pointer
    // to it; with the by-value GXSetFog prototype MWCC copies it to a call temp. Same as select_disp.
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(0, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->field_0x18 + 0x130, 0x112 - entry->field_0x1C, entry->field_0x20, entry->field_0x24 - 0x32);

    zero = 0.0f;
    for (i = 0; i < select->field_0x34; i++) {
        f32 row;
        f32 bound;
        u16 w;

        row = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
        bound = select->field_0x1C + (zero + row);
        if ((f32)entry->field_0x1C < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->field_0x1C - entry->field_0x24) > bound + 32.0f) {
            continue;
        }

        if (*(volatile s32*)&select->field_0x38) {
            zero = zero;
        }
        winFontInit();
        {
            GXColor color;
            f32 rowY;
            const char* n1;
            const char* n2;
            const char* n3;

            tmpName = vec3_802ffb58;
            tmpName.x = (f32)(entry->field_0x18 + 0x3c);
            rowY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpName.y = select->field_0x1C + (zero + rowY);
            posName = tmpName;
            scaleTmp = vec3_802ffb64;

            switch (((WinMgrSelectItem*)select->field_0x30)[i].field_0x2) {
            case 0:
                n1 = msgSearch(str_802FFD38);
                break;
            case 1:
                n1 = msgSearch(str_802FFD44);
                break;
            case 2:
                n1 = msgSearch(str_802FFD50);
                break;
            case 3:
                n1 = pouchGetYoshiName();
                break;
            case 4:
                n1 = msgSearch(str_802FFD5C);
                break;
            case 5:
                n1 = msgSearch(str_802FFD68);
                break;
            case 6:
                n1 = msgSearch(str_802FFD74);
                break;
            }

            w = FontGetMessageWidth(n1);
            if (w > 0xaa) {
                switch (((WinMgrSelectItem*)select->field_0x30)[i].field_0x2) {
                case 0:
                    n2 = msgSearch(str_802FFD38);
                    break;
                case 1:
                    n2 = msgSearch(str_802FFD44);
                    break;
                case 2:
                    n2 = msgSearch(str_802FFD50);
                    break;
                case 3:
                    n2 = pouchGetYoshiName();
                    break;
                case 4:
                    n2 = msgSearch(str_802FFD5C);
                    break;
                case 5:
                    n2 = msgSearch(str_802FFD68);
                    break;
                case 6:
                    n2 = msgSearch(str_802FFD74);
                    break;
                }
                scale = 170.0f / (f32)FontGetMessageWidth(n2);
            } else {
                scale = 1.0f;
            }
            scaleTmp.x = scale;
            scaleName = scaleTmp;

            color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;

            switch (((WinMgrSelectItem*)select->field_0x30)[i].field_0x2) {
            case 0:
                n3 = msgSearch(str_802FFD38);
                break;
            case 1:
                n3 = msgSearch(str_802FFD44);
                break;
            case 2:
                n3 = msgSearch(str_802FFD50);
                break;
            case 3:
                n3 = pouchGetYoshiName();
                break;
            case 4:
                n3 = msgSearch(str_802FFD5C);
                break;
            case 5:
                n3 = msgSearch(str_802FFD68);
                break;
            case 6:
                n3 = msgSearch(str_802FFD74);
                break;
            }
            winFontSet(&posName, &scaleName, &color, n3);
        }

        if (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        {
            f32 rowY;
            s32 icon;
            GXColor color;

            tmpIcon = vec3_802ffb70;
            tmpIcon.x = (f32)(entry->field_0x18 + 0x23);
            rowY = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
            tmpIcon.y = select->field_0x1C + (zero + rowY);
            posIcon = tmpIcon;
            scaleIcon = *(const volatile Vec*)&vec3_802ffb7c;

            color = colorWhite;
            icon = party_icon_table[((WinMgrSelectItem*)select->field_0x30)[i].field_0x2];
            if (icon == 0x158) {
                switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                case 0:
                    icon = 0x158;
                    break;
                case 1:
                    icon = 0x159;
                    break;
                case 2:
                    icon = 0x15a;
                    break;
                case 3:
                    icon = 0x15b;
                    break;
                case 4:
                    icon = 0x15c;
                    break;
                case 5:
                    icon = 0x15d;
                    break;
                case 6:
                default:
                    icon = 0x15e;
                    break;
                }
            }
            winIconSet(icon, &posIcon, &scaleIcon, &color);
        }

        {
            s32 off;
            s32 k;

            for (k = 0, off = 0; k < pouch->partyData[party_id_table[((WinMgrSelectItem*)select->field_0x30)[i].field_0x2]].techLevel; off += 0x1b, k++) {
                if (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) {
                    winIconGrayInit();
                } else {
                    winIconInit();
                }
                {
                    GXColor c = colorWhite;
                    f32 rowY;

                    scaleArrow = *(const volatile Vec*)&vec3_802ffb94;
                    tmpArrow = *(const volatile Vec*)&vec3_802ffb88;
                    tmpArrow.x = (f32)(entry->field_0x18 + 0xfd + off);
                    rowY = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
                    tmpArrow.y = select->field_0x1C + (zero + rowY);
                    posArrow = tmpArrow;
                    winIconSet(0x102, &posArrow, &scaleArrow, &c);
                }
            }
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    msg = msgSearch(str_802FFF30);
    width = FontGetMessageWidth(msg);
    x = (0x78 - width) / 2;
    // Dead guard (x is unchanged); it keeps this division scheduled before winFontInit, as the target has it.
    if (x == width) {
        x = x;
    }
    winFontInit();
    {
        GXColor color = colorWhite2;

        tmpTitle = vec3_802ffba0;
        scaleTitle = vec3_802ffbac;
        tmpTitle.x = (f32)(entry->field_0x18 + (entry->field_0x20 * 3 / 5 - 0x78) * 2 / 3 + x);
        tmpTitle.y = (f32)(entry->field_0x1C + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

    msg = msgSearch(str_802FFF44);
    width = FontGetMessageWidth(msg);
    x = (0x55 - width) / 2;
    // Dead guard (x is unchanged); it keeps this division scheduled before winFontInit, as the target has it.
    if (x == width) {
        x = x;
    }
    winFontInit();
    {
        GXColor color = colorWhite2;

        tmpTitle2 = vec3_802ffbb8;
        scaleTitle2 = vec3_802ffbc4;
        tmpTitle2.x = (f32)(entry->field_0x18 + entry->field_0x20 * 3 / 5 + (entry->field_0x20 * 2 / 5 - 0x55) / 3 + x);
        tmpTitle2.y = (f32)(entry->field_0x1C + 0xe);
        posTitle2 = tmpTitle2;
        winFontSetEdge(&posTitle2, &scaleTitle2, &color, msg);
    }

    posMain = vec3_802ffbd0;
    posMain.x = select->field_0x14;
    posMain.y = select->field_0x18;
    iconDispGx(&posMain, 0x14, 0x1f0, 1.0f);

    if (select->field_0x34 > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->field_0x10 != 0) {
            posUp = vec3_802ffbdc;
            posUp.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posUp.y = (f32)(entry->field_0x1C - 0x24);
            iconDispGx(&posUp, 0x10, 0x1b5, 0.6f);
        }
        if (select->field_0x10 != select->field_0x34 - 8) {
            posDown = vec3_802ffbe8;
            posDown.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posDown.y = (f32)(entry->field_0x1C - entry->field_0x24 - 0xc);
            iconDispGx(&posDown, 0x10, 0x1b6, 0.6f);
        }
    }
}

const char str_802FFF60[] = "msg_window_title_2\000";
const char str_802FFF74[] = "msg_window_title_3\000";
const char str_802FFF88[] = "msg_window_title_4\000";
const char str_802FFF9C[] = "msg_window_title_1\000";
const char str_802FFFB0[] = "%s_%04d";
const char str_802FFFB8[] = "jperfect_title_%04d";
const char str_802FFFCC[] = "msg_lottery_number\000";

const char str_80421A9C[] = "%d";

// The item loop matches register for register (entry r31, select r25, rodata r28, itemDataTable r29,
// 0x4330 r30, counter r24, rowoff r26, itemoff r27, item r22, value r23). The dots loop matches too:
// vec3_802ffae0/vec3_802ffaec are declared non-const (section ".rodata") above so MWCC cannot prove
// the winIconSet call leaves them alone and therefore reloads them inside the loop in caller-saved
// registers (r10/r11/r12) instead of hoisting them into callee-saved r21/r27/r29 and pulling r20/r21
// into the frame; the scaleDot/tmpDot assignments also have to sit before the dotX/dotY computation
// to reproduce the original schedule.
// Native conversions in select_disp_luigi and select_disp_party pool the signed/unsigned doubles
// at .rodata+0x468/+0x4c0. The dead `lwz r0, 0x38(r25); cmpwi r0, 0x0` pair at the top of the
// item loop is reproduced by `if (*(volatile s32*)&select->field_0x38) { zero = zero; }`.
void select_disp(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    s32 i;
    s32 value;
    ItemData* item;
    s32 hasValue;
    Mtx mtx1;
    Mtx mtx2;
    char buf[0x20];
    Vec posRow;
    Vec tmpRow;
    Vec scaleRow;
    Vec posName;
    Vec tmpName;
    Vec scaleName;
    Vec posEdge;
    Vec tmpEdge;
    Vec scaleEdge;
    Vec posDot;
    Vec tmpDot;
    Vec scaleDot;
    Vec posTitle;
    Vec tmpTitle;
    Vec scaleTitle;
    Vec posIcon3;
    Vec posIcon13;
    Vec posIcon16;
    Vec posMain;
    Vec posUp;
    Vec posDown;
    u32 scissorLeft;
    u32 scissorTop;
    u32 scissorWidth;
    u32 scissorHeight;
    GXColor colorBlack = dat_80421a28;
    GXColor colorGray = dat_80421a2c;
    GXColor colorWhite;
    GXColor colorWhite2;
    GXColor fogColor;
    const char* msg;
    u16 width;
    u16 x;
    f32 zero;
    f32 textWidth;

    colorWhite2 = dat_80421a30;
    colorWhite = dat_80421a30;
    if (wp->mEntries[select->descriptorId[0]].flags & 4) {
        return;
    }

    fogColor = dat_80421a34;
#ifdef MUST_MATCH
    // MUST_MATCH: the original stores the fog colour in fogColor's own slot and passes a pointer
    // to it; with the by-value GXSetFog prototype MWCC copies it to a call temp. Same as select_disp_luigi.
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(0, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->field_0x18 + 0x130, 0x112 - entry->field_0x1C, entry->field_0x20, entry->field_0x24 - 0x32);

    zero = 0.0f;
    for (i = 0; i < select->field_0x34; i++) {
        f32 row;
        f32 bound;
        item = &itemDataTable[((WinMgrSelectItem*)select->field_0x30)[i].field_0x2];
        row = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
        bound = select->field_0x1C + (zero + row);
        if ((f32)entry->field_0x1C < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->field_0x1C - entry->field_0x24) > bound + 32.0f) {
            continue;
        }
        if (*(volatile s32*)&select->field_0x38) {
            zero = zero;
        }
        if (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        {
            GXColor color = colorWhite;
            f32 rowY;

            tmpRow = vec3_802ffa98;
            scaleRow = vec3_802ffaa4;
            tmpRow.x = (f32)(entry->field_0x18 + 0x23);
            rowY = (f32)(entry->field_0x1C - 0x2c - i * 0x18);
            tmpRow.y = select->field_0x1C + (zero + rowY);
            posRow = tmpRow;
            winIconSet(*(u16*)&item->icon_id, &posRow, &scaleRow, &color);
        }
        winFontInit();
        {
            GXColor color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
            f32 nameY;

            tmpName = vec3_802ffab0;
            scaleName = vec3_802ffabc;
            tmpName.x = (f32)(entry->field_0x18 + 0x3c);
            nameY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpName.y = select->field_0x1C + (zero + nameY);
            posName = tmpName;
            winFontSet(&posName, &scaleName, &color, msgSearch(item->name));
        }

        hasValue = 0;
        switch (select->field_0x4) {
        case 11:
            value = ((ItemDataView*)item)->buy_price;
            hasValue = 1;
            break;
        case 3:
        case 12:
            value = (u16)item->sell_price;
            hasValue = 1;
            break;
        case 13:
            value = (u16)item->star_piece_price;
            hasValue = 1;
            break;
        case 14:
            value = ((ItemDataView*)item)->buy_price * 12 / 10;
            hasValue = 1;
            break;
        case 15:
            value = ((ItemDataView*)item)->buy_price * (evtGetValue(0, 0xF5DE06A9) + 11) / 10;
            hasValue = 1;
            break;
        case 16:
            value = (((ItemDataView*)item)->buy_price * 2 + 2) / 3;
            hasValue = 1;
            break;
        }
        if (hasValue) {
            GXColor color;
            f32 edgeX;
            f32 edgeY;

            sprintf(buf, str_80421A9C, value);
            textWidth = (f32)FontGetMessageWidth(buf);
            winFontInit();
            color = (((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 & 1) ? colorGray : colorBlack;
            scaleEdge = vec3_802ffad4;
            tmpEdge = vec3_802ffac8;
            edgeX = (f32)(entry->field_0x18 + entry->field_0x20 - 0xa) - textWidth;
            edgeY = (f32)(entry->field_0x1C - 0x20 - i * 0x18);
            tmpEdge.x = edgeX;
            tmpEdge.y = select->field_0x1C + (zero + edgeY);
            posEdge = tmpEdge;
            winFontSet(&posEdge, &scaleEdge, &color, buf);
        }

    }

    if (select->field_0x38) {
        winIconInit();
        for (i = 2; i < 0x1e; i++) {
            GXColor color = colorBlack;
            f32 dotX;
            f32 dotY;

            scaleDot = vec3_802ffaec;
            tmpDot = vec3_802ffae0;
            dotX = (f32)(entry->field_0x18 + (i * entry->field_0x20) / 31);
            dotY = (f32)(entry->field_0x1C - 0x38) + select->field_0x1C;
            tmpDot.x = dotX;
            tmpDot.y = dotY;
            posDot = tmpDot;
            winIconSet(0x1b7, &posDot, &scaleDot, &color);
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    switch (select->field_0x4) {
    case 2:
    case 11:
    case 12:
    case 13:
        msg = msgSearch(str_802FFF60);
        break;
    case 14:
        msg = msgSearch(str_802FFF74);
        break;
    case 16:
        msg = msgSearch(str_802FFF88);
        break;
    case 17:
        msg = msgSearch(str_802FFED8);
        break;
    default:
        msg = msgSearch(str_802FFF9C);
        break;
    }

    width = FontGetMessageWidth(msg);
    x = (entry->field_0x20 - width) / 2;
    winFontInit();
    {
        GXColor color = colorWhite2;

        tmpTitle = vec3_802ffaf8;
        scaleTitle = vec3_802ffb04;
        tmpTitle.x = (f32)(entry->field_0x18 + x);
        tmpTitle.y = (f32)(entry->field_0x1C + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

        switch (select->field_0x4) {
        case 3:
        case 11:
        case 12:
        case 14:
        case 15:
            PSMTXTrans(mtx1, (f32)(entry->field_0x18 + 0x10c), (f32)(entry->field_0x1C - 0x12), 0.0f);
            PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
            PSMTXConcat(mtx1, mtx2, mtx1);
            iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a38);
            posIcon3 = vec3_802ffb10;
            posIcon3.x = (f32)(entry->field_0x18 + 0x10c);
            posIcon3.y = (f32)(entry->field_0x1C - 0xc);
            iconDispGx(&posIcon3, 0x10, 0x18b, 0.6f);
            break;
        case 13:
            PSMTXTrans(mtx1, (f32)(entry->field_0x18 + 0x10c), (f32)(entry->field_0x1C - 0x12), 0.0f);
            PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
            PSMTXConcat(mtx1, mtx2, mtx1);
            iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a3c);
            posIcon13 = vec3_802ffb1c;
            posIcon13.x = (f32)(entry->field_0x18 + 0x10c);
            posIcon13.y = (f32)(entry->field_0x1C - 0xc);
            iconDispGx(&posIcon13, 0x10, 0x18d, 0.6f);
            break;
        case 16:
            PSMTXTrans(mtx1, (f32)(entry->field_0x18 + 0x10c), (f32)(entry->field_0x1C - 0x12), 0.0f);
            PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
            PSMTXConcat(mtx1, mtx2, mtx1);
            iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a40);
            posIcon16 = vec3_802ffb28;
            posIcon16.x = (f32)(entry->field_0x18 + 0x10c);
            posIcon16.y = (f32)(entry->field_0x1C - 0xc);
            iconDispGx(&posIcon16, 0x10, 0x13f, 0.6f);
            break;
        }

    posMain = vec3_802ffb34;
    posMain.x = select->field_0x14;
    posMain.y = select->field_0x18;
    iconDispGx(&posMain, 0x14, 0x1f0, 1.0f);

    if (select->field_0x34 > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->field_0x10 != 0) {
            posUp = vec3_802ffb40;
            posUp.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posUp.y = (f32)(entry->field_0x1C - 0x24);
            iconDispGx(&posUp, 0x10, 0x1b5, 0.6f);
        }
        if (select->field_0x10 != select->field_0x34 - 8) {
            posDown = vec3_802ffb4c;
            posDown.x = (f32)entry->field_0x18 + (f32)entry->field_0x20 / 2.0f;
            posDown.y = (f32)(entry->field_0x1C - entry->field_0x24 - 0xc);
            iconDispGx(&posDown, 0x10, 0x1b6, 0.6f);
        }
    }
}

const f32 float_24_80421aa0 = 24.0f;
const f32 float_6_80421aa4 = 6.0f;
const f32 float_neg144_80421aa8 = -144.0f;

// Defer this middle group in reverse retail order so winMgrSelectEntry can inline its real callees.
#ifdef MUST_MATCH
#pragma defer_codegen on
#endif

void winMgrInit(void) {
    wp->mNumEntries = 32;
    wp->mEntries = (WinMgrEntry*)__memAlloc(HEAP_DEFAULT, wp->mNumEntries * sizeof(WinMgrEntry));
    memset(wp->mEntries, 0, wp->mNumEntries * sizeof(WinMgrEntry));
}

void winMgrReInit(void) {
    memset(wp->mEntries, 0, wp->mNumEntries * sizeof(WinMgrEntry));
}

void winMgrDisp(CameraId camId, void* param) {
    Mtx rotmtx, scalemtx;
    WinMgrEntry* entry = (WinMgrEntry*)param;

    PSMTXRotRad(rotmtx, 'z', entry->rotation * 0.017453292f);
    PSMTXScale(scalemtx, entry->scale, entry->scale, entry->scale);
    PSMTXConcat(rotmtx, scalemtx, scalemtx);

    winMgrDispWaku0(scalemtx, entry);

    switch (*(s32*)(entry->description->field_0x0 + 4)) {
    case 0:
    case 2:
        break;
    case 1:
        winMgrDispWaku1(scalemtx, entry, &dat_80421a14);
        break;
    case 3:
        winMgrDispWaku3a(scalemtx, entry, &dat_80421a18);
        winMgrDispWaku3b(scalemtx, entry, &dat_80421a1c);
        break;
    case 4:
    case 5:
        winMgrDispWaku4(scalemtx, entry, &dat_80421a20);
        winMgrDispWaku5(scalemtx, entry, &dat_80421a24);
        break;
    }

    if (entry->description->disp) {
        ((void (*)(WinMgrEntry*))entry->description->disp)(entry);
    }
}

void winMgrMain(void) {
    WinMgrEntry* entry;
    int i;

    for (i = 0, entry = wp->mEntries; i < (s32)wp->mNumEntries; i++, entry++) {
        if (entry->flags & 1) {
            winMgrSeq(entry);
            if (entry->description->main) {
                entry->description->main(entry);
            }
            if (entry->flags & 2) {
                dispEntry((CameraId)entry->description->camId, (f32)(entry->priority + 300), 0, winMgrDisp, entry);
            }
        }
    }
}

void winMgrSeq(WinMgrEntry* entry) {
    entry->flags |= 4;

    switch (*(s32*)entry->description->field_0x0) {
    case 0:
        switch (entry->field_0x4) {
        case 0:
            entry->flags &= ~4;
            break;
        case 1:
            entry->flags |= 2;
            entry->rotation = intplGetValue(0xc, entry->field_0x8, 8, 30.0f, 0.0f);
            entry->scale = intplGetValue(0xc, entry->field_0x8, 8, 0.0f, 1.0f);
            *(s32*)&entry->field_0xC = 0xff;
            entry->field_0x8++;
            if (entry->field_0x8 > 8) {
                entry->field_0x4 = 0;
            }
            break;
        case 2:
            entry->rotation = intplGetValue(0xc, entry->field_0x8, 8, 0.0f, 30.0f);
            entry->scale = intplGetValue(0xc, entry->field_0x8, 8, 1.0f, 0.0f);
            *(s32*)&entry->field_0xC = 0xff;
            entry->field_0x8++;
            if (entry->field_0x8 > 8) {
                entry->field_0x4 = 0;
                entry->flags &= ~2;
                if (entry->flags & 8) {
                    entry->flags &= ~1;
                }
            }
            break;
        }
        break;
    case 1:
        switch (entry->field_0x4) {
        case 0:
            entry->flags &= ~4;
            break;
        case 1:
            entry->flags |= 2;
            entry->rotation = 0.0f;
            entry->scale = intplGetValue(0xc, entry->field_0x8, 8, 0.0f, 1.0f);
            *(s32*)&entry->field_0xC = (s32)intplGetValue(0xc, entry->field_0x8, 8, 0.0f, 255.0f);
            entry->field_0x8++;
            if (entry->field_0x8 > 8) {
                entry->field_0x4 = 0;
            }
            break;
        case 2:
            entry->rotation = 0.0f;
            entry->scale = 1.0f;
            *(s32*)&entry->field_0xC = (s32)intplGetValue(0xc, entry->field_0x8, 8, 255.0f, 0.0f);
            entry->field_0x8++;
            if (entry->field_0x8 > 8) {
                entry->field_0x4 = 0;
                entry->flags &= ~2;
                if (entry->flags & 8) {
                    entry->flags &= ~1;
                }
            }
            break;
        }
        break;
    case 2:
        switch (entry->field_0x4) {
        case 0:
            entry->flags &= ~4;
            break;
        case 1:
            entry->flags |= 2;
            entry->rotation = 0.0f;
            entry->scale = 1.0f;
            *(s32*)&entry->field_0xC = 0xff;
            entry->field_0x4 = 0;
            break;
        case 2:
            entry->flags &= ~2;
            entry->rotation = 0.0f;
            entry->scale = 1.0f;
            *(s32*)&entry->field_0xC = 0xff;
            entry->field_0x4 = 0;
            if (entry->flags & 8) {
                entry->flags &= ~1;
            }
            break;
        }
        break;
    }
}

s32 winMgrEntry(WinMgrDesc* desc) {
    WinMgrEntry* entry;
    int i;

    for (i = 0, entry = wp->mEntries; i < (s32)wp->mNumEntries; i++, entry++) {
        if (!(entry->flags & 1)) {
            break; // this entry is available
        }
    }
    entry->flags = 0;
    entry->flags |= 1;

    entry->field_0x4 = 0;
    entry->field_0x8 = 0;
    entry->description = desc;
    entry->field_0x18 = entry->description->field_0xC;
    entry->field_0x1C = entry->description->field_0x10;
    entry->field_0x20 = entry->description->field_0x14;
    entry->field_0x24 = entry->description->field_0x18;

    entry->priority = 0;

    entry->param = NULL;

    entry->field_0x38 = 0;
    entry->field_0x3C = 0;

    return i; // only return the entry id
}

void winMgrSetParam(s32 id, void* param) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->param = param;
    }
}

void winMgrOpen(s32 id) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->field_0x4 = 1;
        entry->field_0x8 = 0;
        entry->field_0x38 = 0;
        entry->field_0x3C = 0;
        entry->field_0x34 = 0;
    }
}

void winMgrClose(s32 id) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->field_0x4 = 2;
        entry->field_0x8 = 0;
    }
}

void winMgrCloseAutoDelete(s32 id) {
    WinMgrEntry* entry; // r4

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->field_0x4 = 2;
        entry->field_0x8 = 0;
        entry->flags |= 8;
    }
}

void winMgrDelete(s32 id) {
    wp->mEntries[id].flags &= ~1;
}

BOOL winMgrAction(s32 id) {
    return (wp->mEntries[id].flags >> 2) & 1;
}

void winMgrSetSize(s32 id, s32 a2, s32 a3, s32 a4, s32 a5) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->field_0x18 = a2;
        entry->field_0x1C = a3;
        entry->field_0x20 = a4;
        entry->field_0x24 = a5;
    }
}

void winMgrSetPriority(s32 id, s32 priority) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->priority = priority;
    }
}

WinMgrEntry* winMgrGetPtr(s32 id) {
    return &wp->mEntries[id];
}

void winMgrHelpInit(WinMgrEntry* entry) {
    entry->field_0x38 = 0;
    entry->field_0x3C = 0;
    entry->field_0x34 = 0;
}

void winMgrHelpMain(WinMgrEntry* entry) {
    u16 width = 0;

    if (!(entry->flags & 4)) {
        if ((const char*)entry->field_0x34 != NULL) {
            FontGetMessageWidthLine((const char*)entry->field_0x34, &width);
            entry->field_0x3C = width + 1;
        }
        if (entry->field_0x3C > 2) {
            if (keyGetDirTrg(0) & 0x100) {
                entry->field_0x38 -= 2;
                if (entry->field_0x38 < 0) {
                    entry->field_0x38 = 0;
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if ((entry->field_0x3C + 1) / 2 - 1 > (entry->field_0x38 + 1) / 2) {
                    entry->field_0x38 += 2;
                }
            }
        }
    }
}

#pragma dont_inline on
void winMgrHelpDraw(WinMgrEntry* entry) {
    Mtx mtx1;
    Mtx mtx2;
    char buf[0x400];
    lottery_info* lottery;
    s16 n;

    if (entry->flags & 4) {
        return;
    }
    GXSetScissor(0, 0x180, 0x260, 0x38);
    if (strcmp((const char*)entry->field_0x34, msgSearch(str_802FFFCC)) == 0) {
        lottery = lotteryGetPtr();
        n = *(s16*)&lottery->field_0x2[0x20 - 0x2];
        sprintf(buf, (const char*)entry->field_0x34, n);
        FontDrawStart();
        FontDrawMessage(-0xd2, (s32)(28.0f * entry->field_0x38 + WINMGR_F32(float_neg144_80421aa8)), buf);
    } else {
        FontDrawStart();
        FontDrawMessage(-0xd2, (s32)(28.0f * entry->field_0x38 + WINMGR_F32(float_neg144_80421aa8)), (const char*)entry->field_0x34);
    }
    GXSetScissor(0, 0, 0x260, 0x1e0);
    if (entry->field_0x3C > 2) {
        s32 row = entry->field_0x38;
        if (row == 0) {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->field_0x18 + entry->field_0x20 - 0xC),
                           (f32)(entry->field_0x1C - entry->field_0x24 - 4), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B6);
            }
        } else if (row + 2 == entry->field_0x3C) {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->field_0x18 + entry->field_0x20 - 0xC),
                           (f32)(entry->field_0x1C - entry->field_0x24 + 0x2C), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B5);
            }
        } else {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->field_0x18 + entry->field_0x20 - 0xC),
                           (f32)(entry->field_0x1C - entry->field_0x24 + 0x2C), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B5);
                PSMTXTrans(mtx1, (f32)(entry->field_0x18 + entry->field_0x20 - 0xC),
                           (f32)(entry->field_0x1C - entry->field_0x24 - 4), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B6);
            }
        }
        PSMTXTrans(mtx1, (f32)(entry->field_0x18 + entry->field_0x20 - 0xC),
                   (f32)(entry->field_0x1C - entry->field_0x24 + 0xA), 0.0f);
        PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
        PSMTXConcat(mtx1, mtx2, mtx1);
        iconDispGx2(mtx1, 0x10, 0x70);
    }
}

#pragma dont_inline reset
WinMgrSelect* winMgrSelectEntry(s32 a1, s32 a2, BOOL a3) {
    PouchData* pouch = pouchGetPtr();
    InformantWork* jdt = &_jdt;
    WinMgrSelect* select;
    WinMgrSelectTbl* table;
    s32 descId;
    s32 i;
    WinMgrDesc* desc;
    s32 j;
    s32 type;
    u16 item;

    select = (WinMgrSelect*)__memAlloc(HEAP_DEFAULT, 0x3c);
    memset(select, 0, 0x3c);

    select->field_0x4 = a1;
    descId = a1;
    select->field_0xC = 0;
    select->field_0x10 = 0;
    select->field_0x18 = 0.0f;
    select->field_0x14 = 0.0f;
    select->field_0x1C = 0.0f;
    select->field_0x38 = a2;

    if (a3) {
        *(u16*)&select->flags |= 0x100;
    }

    if (descId < 0) {
        descId = 0;
    }
    desc = select_desc_tbl[descId].descriptors;
    select->numDescriptors = (s32)select_desc_tbl[descId].numDescriptors;
    for (i = 0; i < select->numDescriptors; i++) {
        select->descriptorId[i] = winMgrEntry(desc++);
        winMgrSetParam(select->descriptorId[i], select);
    }

    desc = select_desc_tbl[descId].descriptors;
    select->field_0x14 = (f32)desc->field_0xC;
    select->field_0x18 = (f32)(desc->field_0x10 - 54);

    type = select->field_0x4;
    switch (type) {
    case 0:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x1e8);
        memset((void*)select->field_0x30, 0, 0x1e8);
        select->field_0x34 = 0;
        if (a2 != 0) {
            {
                WinMgrSelectItem* it = &((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++];
                it->field_0x2 = a2;
            }
        }
        for (i = 0; i < 0x79; i++) {
            item = pouchKeyItem(i);
            if (item != 0) {
                for (j = 0; j < 0x13; j++) {
                    if (item == menu_skip_list[j]) {
                        break;
                    }
                }
                if (j >= 0x13) {
                    ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = item;
                }
            }
        }
        break;
    case 1:
    case 3:
    case 4:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x54);
        memset((void*)select->field_0x30, 0, 0x54);
        select->field_0x34 = 0;
        if (a2 != 0) {
            {
                WinMgrSelectItem* it = &((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++];
                it->field_0x2 = a2;
            }
        }
        for (i = 0; i < 0x14; i++) {
            item = pouchHaveItem(i);
            if (item != 0) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = item;
            }
        }
        break;
    case 2:
    case 12:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x324);
        memset((void*)select->field_0x30, 0, 0x324);
        select->field_0x34 = 0;
        if (a2 != 0) {
            {
                WinMgrSelectItem* it = &((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++];
                it->field_0x2 = a2;
            }
        }
        for (i = 0; i < 0xc8; i++) {
            item = pouchHaveBadge(i);
            if (item != 0) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = item;
            }
        }
        break;
    case 5:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x84);
        memset((void*)select->field_0x30, 0, 0x84);
        select->field_0x34 = 0;
        if (a2 != 0) {
            {
                WinMgrSelectItem* it = &((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++];
                it->field_0x2 = a2;
            }
        }
        for (i = 0; i < 0x20; i++) {
            item = pouchKeepItem(i);
            if (item != 0) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = item;
            }
        }
        break;
    case 6:
    case 7:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x1c);
        memset((void*)select->field_0x30, 0, 0x1c);
        select->field_0x34 = 0;
        for (i = 0; party_id_table[i] != 0; i++) {
            if (partyChkJoin(party_id_table[i])) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x2 = i;
                if (evtGetValue(0, 0xF8406022) == 0) {
                    if (pouch->partyData[party_id_table[i]].techLevel >= 1) {
                        ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x0 |= 3;
                    }
                } else {
                    if (pouch->partyData[party_id_table[i]].techLevel >= 2) {
                        ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x0 |= 3;
                    }
                }
                select->field_0x34++;
            }
        }
        break;
    case 8:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x1c);
        memset((void*)select->field_0x30, 0, 0x1c);
        select->field_0x34 = 0;
        for (i = 0; party_id_table[i] != 0; i++) {
            if (partyChkJoin(party_id_table[i])) {
                if (party_id_table[i] == evtGetValue(0, 0xFD050F81)) {
                    continue;
                }
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x2 = i;
                if (pouch->partyData[party_id_table[i]].techLevel == 0) {
                    ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x0 |= 3;
                }
                select->field_0x34++;
            }
        }
        break;
    case 9:
        mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
        mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
        mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0xc);
        memset((void*)select->field_0x30, 0, 0xc);
        for (i = 0; i < 3; i++) {
            s32 canDown = 0;
            s32 canUp = 0;

            ((WinMgrSelectItem*)select->field_0x30)[i].field_0x2 = i;
            for (j = 0; j < 3; j++) {
                if (((WinMgrSelectItem*)select->field_0x30)[i].field_0x2 != j) {
                    if (*mario_status_point_table[j] - mario_status_henka_table[j] <= 0) {
                        canDown++;
                    }
                } else if (*mario_status_point_table[j] + mario_status_henka_table[j] > mario_status_max_table[j]) {
                    canUp = 1;
                }
            }
            if (canDown == 2 || canUp != 0) {
                if (canUp != 0) {
                    ((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 |= 7;
                } else {
                    ((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 |= 3;
                }
            }
        }
        select->field_0x34 = 3;
        break;
    case 10:
        mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
        mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
        mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 8);
        memset((void*)select->field_0x30, 0, 8);
        select->field_0x34 = 0;
        for (i = 0; i < 3; i++) {
            if (i == evtGetValue(0, 0xFD050F81)) {
                continue;
            }
            ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x2 = i;
            if (*mario_status_point_table[i] - mario_status_henka_table[i] <= 0) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x0 |= 3;
            }
            select->field_0x34++;
        }
        break;
    case 11:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x18c);
        memset((void*)select->field_0x30, 0, 0x18c);
        select->field_0x34 = 0;
        for (i = 0; i < 0x63; i++) {
            s16 v = (s16)badgeShop_get(bdsw->bargain_table, (s16)(i + 0xf0));

            if (v - badgeShop_ThrowCheck(i + 0xf0) > 0) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = i + 0xf0;
            }
        }
        qqsort((void*)select->field_0x30, select->field_0x34, 4, (int (*)(void*, void*))unk_802379e4);
        break;
    case 13:
        {
            s32 max = getBadgeStarmaniacTableMaxCount();

            select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, max * 4);
            memset((void*)select->field_0x30, 0, max * 4);
            select->field_0x34 = 0;
            for (i = 0; i < 0x63; i++) {
                s32 v = (s16)badgeShop_get(bdsw->starmaniac_table, (s16)(i + 0xf0));

                if (v - badgeShop_ThrowCheck(i + 0xf0) > 0) {
                    ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x2 = i + 0xf0;
                    if (itemDataTable[0xf0 + i].star_piece_price > pouchGetStarPiece()) {
                        ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34].field_0x0 |= 3;
                    }
                    select->field_0x34++;
                }
            }
            qqsort((void*)select->field_0x30, select->field_0x34, 4, (int (*)(void*, void*))unk_80237a2c);
        }
        break;
    case 14:
        select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, 0x10);
        memset((void*)select->field_0x30, 0, 0x10);
        select->field_0x34 = 0;
        for (i = 0; i < 4; i++) {
            s32 v = evtGetValue(0, 0xF5DE01F6 + i);

            if (badge_bottakuru_table[v] != 0) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = badge_bottakuru_table[v];
            }
        }
        break;
    case 15:
        {
            s32 max = getBadgeBottakuru100TableMaxCount();

            select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, max * 4);
            memset((void*)select->field_0x30, 0, max * 4);
            select->field_0x34 = 0;
            for (i = 0; i < max; i++) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = badge_bottakuru100_table[i];
            }
        }
        break;
    case 16:
        {
            s32 max = getBadgeBteresaTableMaxCount();

            select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, max * 4);
            memset((void*)select->field_0x30, 0, max * 4);
            select->field_0x34 = 0;
            for (i = 0; i < 0x152; i++) {
                if ((s16)badgeShop_get(bdsw->bteresa_table, (s16)(i + 1)) > 0) {
                    ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = i + 1;
                }
            }
        }
        break;
    case 17:
    case 18:
        {
            u8* unk = (type == 0x11) ? jdt->unk14.unkD : jdt->unk14.unk1A;

            select->field_0x34 = jdt->count;
            select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, (select->field_0x34 + 1) * 4);
            memset((void*)select->field_0x30, 0, (select->field_0x34 + 1) * 4);
            for (i = 0; i < select->field_0x34; i++) {
                ((WinMgrSelectItem*)select->field_0x30)[i].field_0x2 = i;
                if (johoya_get(unk, jdt->info[i].storyId) != 0) {
                    ((WinMgrSelectItem*)select->field_0x30)[i].field_0x0 |= 1;
                }
            }
        }
        break;
    default:
        if (a1 < 0) {
            s32* list = (s32*)a1;
            s32 n = 0;

            while (list[n++] != -1)
                ;
            select->field_0x30 = (s32)__memAlloc(HEAP_DEFAULT, (n + 1) * 4);
            memset((void*)select->field_0x30, 0, (n + 1) * 4);
            select->field_0x34 = 0;
            if (a2 != 0) {
                WinMgrSelectItem* it = &((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++];

                it->field_0x2 = a2;
            }
            for (i = 0; list[i] != -1; ) {
                ((WinMgrSelectItem*)select->field_0x30)[select->field_0x34++].field_0x2 = list[i++];
            }
        }
        break;
    }

    if (select->field_0x34 < 8) {
        s32 f20 = desc->field_0x14;
#ifdef MUST_MATCH
        s32 f1C = get10(desc);
#else
        s32 f1C = desc->field_0x10;
#endif
        s32 f18 = desc->field_0xC;
        s32 f24 = desc->field_0x18 - (8 - select->field_0x34) * 0x18;
        WinMgrEntry* e = &wp->mEntries[select->descriptorId[0]];

        if (e->flags & 1) {
            e->field_0x18 = f18;
            e->field_0x1C = f1C;
            e->field_0x20 = f20;
            e->field_0x24 = f24;
        }
    }
    return select;
}

s32 unk_80237a2c(void* a, void* b) {
    u32 va = itemDataTable[((WinMgrSelectItem*)a)->field_0x2].star_piece_price;
    u32 vb = itemDataTable[((WinMgrSelectItem*)b)->field_0x2].star_piece_price;

    if (va > vb) {
        return 1;
    }
    return va < vb ? -1 : 0;
}

s32 unk_802379e4(void* a, void* b) {
    s32 va = itemDataTable[((WinMgrSelectItem*)a)->field_0x2].type_sort_order;
    s32 vb = itemDataTable[((WinMgrSelectItem*)b)->field_0x2].type_sort_order;

    if (va > vb) {
        return 1;
    }
    return va < vb ? -1 : 0;
}

s32 winMgrSelect(WinMgrSelect* select) {
    u16 flags = *(u16*)&select->flags;

    if (flags & 0x1000) {
        if (flags & 0x2000) {
            return -1;
        }
        if (select->field_0x38 != 0 && select->field_0xC == 0 && (flags & 0x100)) {
            return -1;
        }
        {
            WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
            return items[select->field_0xC].field_0x2;
        }
    }
    return 0;
}

s32 winMgrSelectOther(WinMgrSelect* select, EventEntry* event) {
    u16 flags = *(u16*)&select->flags;
    WinMgrSelectItem* items;
    InformantWork* jdt = &_jdt;
    s32 itemId;
    const char* msg;
    char buf[0x40];

    if (flags & 0x1000) {
        if (flags & 0x2000) {
            return -1;
        }

        items = (WinMgrSelectItem*)select->field_0x30;
        itemId = items[select->field_0xC].field_0x2;

        switch (select->field_0x4) {
        case 6:
        case 7:
            event->lwData[1] = party_id_table[itemId];
            switch (itemId) {
            case 0:
                msg = msgSearch(str_802FFD38);
                break;
            case 1:
                msg = msgSearch(str_802FFD44);
                break;
            case 2:
                msg = msgSearch(str_802FFD50);
                break;
            case 3:
                msg = pouchGetYoshiName();
                break;
            case 4:
                msg = msgSearch(str_802FFD5C);
                break;
            case 5:
                msg = msgSearch(str_802FFD68);
                break;
            case 6:
                msg = msgSearch(str_802FFD74);
                break;
            }
            event->lwData[3] = (s32)msg;
            break;
        case 8:
            event->lwData[2] = party_id_table[itemId];
            switch (itemId) {
            case 0:
                msg = msgSearch(str_802FFD38);
                break;
            case 1:
                msg = msgSearch(str_802FFD44);
                break;
            case 2:
                msg = msgSearch(str_802FFD50);
                break;
            case 3:
                msg = pouchGetYoshiName();
                break;
            case 4:
                msg = msgSearch(str_802FFD5C);
                break;
            case 5:
                msg = msgSearch(str_802FFD68);
                break;
            case 6:
                msg = msgSearch(str_802FFD74);
                break;
            }
            event->lwData[4] = (s32)msg;
            break;
        case 9:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)mario_status_name_table[itemId];
            event->lwData[5] = *mario_status_point_table[0];
            event->lwData[6] = *mario_status_point_table[1];
            event->lwData[7] = *mario_status_point_table[2];
            event->lwData[5 + itemId] += mario_status_henka_table[itemId];
            break;
        case 10:
            event->lwData[1] = itemId;
            event->lwData[3] = (s32)mario_status_name_table[itemId];
            event->lwData[5 + itemId] -= mario_status_henka_table[itemId];
            break;
        case 11:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = ((ItemDataView*)itemDataTable)[itemId].buy_price;
            event->lwData[4] = itemDataTable[itemId].bp_cost;
            break;
        case 12:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = (u16)itemDataTable[itemId].sell_price;
            event->lwData[4] = select->field_0xC;
            break;
        case 13:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = (u16)itemDataTable[itemId].star_piece_price;
            event->lwData[4] = itemDataTable[itemId].bp_cost;
            break;
        case 14:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = ((ItemDataView*)itemDataTable)[itemId].buy_price * 12 / 10;
            event->lwData[4] = itemDataTable[itemId].bp_cost;
            break;
        case 15:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = ((ItemDataView*)itemDataTable)[itemId].buy_price * (evtGetValue(0, 0xF5DE06A9) + 11) / 10;
            event->lwData[4] = itemDataTable[itemId].bp_cost;
            break;
        case 16:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = (((ItemDataView*)itemDataTable)[itemId].buy_price * 2 + 2) / 3;
            event->lwData[4] = itemDataTable[itemId].bp_cost;
            break;
        case 17:
            sprintf((char*)jdt->infoEnd, str_802FFFB0, jdt->prefix, jdt->info[itemId].storyId);
            sprintf(buf, str_802FFFB8, jdt->info[itemId].storyId);
            *(s32*)&jdt->unk10 = itemId;
            msg = msgSearch(buf);
            event->lwData[1] = (s32)jdt->infoEnd;
            event->lwData[2] = (s32)msg;
            event->lwData[3] = strlen(msg);
            if (johoya_get(jdt->unk14.unkD, jdt->info[itemId].storyId) != 0) {
                event->lwData[4] = 0;
            } else {
                event->lwData[4] = jdt->info[itemId].unkA;
            }
            break;
        case 18:
            sprintf((char*)jdt->infoEnd, str_802FFFB0, jdt->prefix, jdt->info[itemId].storyId);
            *(s32*)&jdt->unk10 = itemId;
            event->lwData[1] = (s32)jdt->infoEnd;
            event->lwData[2] = jdt->info[itemId].initEndPos;
            event->lwData[3] = jdt->info[itemId].tagEndPos;
            if (johoya_get(jdt->unk14.unk1A, jdt->info[itemId].storyId) != 0) {
                event->lwData[4] = 0;
            } else {
                event->lwData[4] = 1;
            }
            break;
        }
        return 1;
    }
    return 0;
}

void winMgrSelectDelete(WinMgrSelect* select) {
#ifdef MUST_MATCH
    // The original took the pointer as `void*` and cast it to WinMgrSelect*; that cast makes the
    // compiler copy the incoming r3 through r0 before the first call (asm: mr r0, r3 / mr r31, r0).
    // Keep the header's WinMgrSelect* signature and reproduce the copy with the same no-op cast.
    WinMgrSelect* s = (WinMgrSelect*)(void*)select;
#else
    WinMgrSelect* s = select;
#endif
    __memFree(HEAP_DEFAULT, (void*)s->field_0x30);
    __memFree(HEAP_DEFAULT, s);
}

s32 unk_80237448(WinMgrSelect* select) {
    return select->field_0xC;
}

// select_main emits first, then MWCC flushes the deferred group immediately after it.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

#pragma opt_common_subs off
void select_main(WinMgrEntry* entry) {
    s32 count;
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    s32 newIndex;
    s32 off;
    s32 i;

    switch ((*(s32*)select->field_0x8)) {
    case 0:
        for (i = 0; i < (count = select->numDescriptors); i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            if (e->flags & 1) {
                e->field_0x4 = 1;
                e->field_0x8 = 0;
                e->field_0x38 = 0;
                e->field_0x3C = 0;
                e->field_0x34 = 0;
            }
        }
        if (count >= 3) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[2]];

            e->field_0x38 = 0;
            e->field_0x3C = 0;
            e->field_0x34 = 0;
        }
        psndSFXOn((const char*)0x2002A);
        statusWinForceOpen();
        (*(s32*)select->field_0x8)++;
        break;
    case 1:
        if (wp->mEntries[select->descriptorId[0]].flags & 4) {
            break;
        }
        newIndex = select->field_0xC;
        if (keyGetDirRep(0) & 0x2000) {
            select->field_0xC++;
        }
        if (keyGetDirRep(0) & 0x1000) {
            select->field_0xC--;
        }
        if (select->field_0xC >= select->field_0x34) {
            if (keyGetDirTrg(0) & 0x2000) {
                select->field_0xC = 0;
                select->field_0x10 = 0;
            } else {
                select->field_0xC--;
            }
        }
        if (select->field_0xC < 0) {
            if (keyGetDirTrg(0) & 0x1000) {
                select->field_0xC = select->field_0x34 - 1;
                off = select->field_0x34 - 8;
                if (off > 0) {
                    select->field_0x10 = off;
                }
            } else {
                select->field_0xC++;
            }
        }
        if (newIndex < select->field_0xC) {
            if (select->field_0xC > 5 && select->field_0x10 < select->field_0x34 - 8) {
                select->field_0x10++;
            }
        } else if (newIndex > select->field_0xC) {
            if (select->field_0xC - select->field_0x10 < 3 && select->field_0x10 > 0) {
                select->field_0x10--;
            }
        }
        if (newIndex != select->field_0xC) {
            psndSFXOn((const char*)0x20005);
        }
        if (keyGetButtonTrg(0) & 0x100) {
            if (((WinMgrSelectItem*)select->field_0x30)[select->field_0xC].field_0x0 & 2) {
                psndSFXOn((const char*)0x20014);
            } else {
                psndSFXOn((const char*)0x20012);
                (*(s32*)select->field_0x8)++;
            }
        } else if (keyGetButtonTrg(0) & 0x200) {
            if (*(u16*)&select->flags & 0x100) {
                psndSFXOn((const char*)0x20013);
                *(u16*)&select->flags |= 0x2000;
                (*(s32*)select->field_0x8)++;
            }
        }
        select->field_0x1C = select->field_0x1C + (WINMGR_F32(float_24_80421aa0) * select->field_0x10 - select->field_0x1C) / WINMGR_F32(float_6_80421aa4);
        select->field_0x14 = select->field_0x14 + ((f32)entry->field_0x18 - select->field_0x14) / WINMGR_F32(float_6_80421aa4);
        select->field_0x18 = select->field_0x18 +
            ((f32)(entry->field_0x1C - 54) - WINMGR_F32(float_24_80421aa0) * (f32)(select->field_0xC - select->field_0x10) -
             select->field_0x18) / WINMGR_F32(float_6_80421aa4);
        break;
    case 2:
        for (i = 0; i < select->numDescriptors; i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            if (e->flags & 1) {
                e->field_0x4 = 2;
                e->field_0x8 = 0;
            }
        }
        psndSFXOn((const char*)0x2002B);
        statusWinForceOff();
        (*(s32*)select->field_0x8)++;
        break;
    case 3:
        if (wp->mEntries[select->descriptorId[0]].flags & 4) {
            break;
        }
        for (i = 0; i < select->numDescriptors; i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            e->flags &= ~1;
        }
        *(u16*)&select->flags |= 0x1000;
        (*(s32*)select->field_0x8)++;
        break;
    case 4:
        break;
    }
}

#pragma opt_common_subs reset
s32 unk_802399f8(EventEntry* event) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    s32 i;
    s32 count;
    s32 zeroFlag;
    s32 nonZeroFlag;
    s32 mode;
    s32 val;

    zeroFlag = 0;
    count = 0;
    nonZeroFlag = 0;

    for (i = 0; party_id_table[i] != 0; i++) {
        if (partyChkJoin(party_id_table[i])) {
            count++;
            if (pouch->partyData[party_id_table[i]].techLevel != 0) {
                nonZeroFlag = 1;
            }
            mode = evtGetValue(event, 0xF8406022);
            if (mode == 0) {
                if (pouch->partyData[party_id_table[i]].techLevel < 1) {
                    val = 0;
                } else {
                    val = 1;
                }
            } else {
                if (pouch->partyData[party_id_table[i]].techLevel < 2) {
                    val = 0;
                } else {
                    val = 2;
                }
            }
            if (val == 0) {
                zeroFlag = 1;
            }
        }
    }
    if (count > 1 && zeroFlag && nonZeroFlag) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

s32 unk_80239b2c(EventEntry* event) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    s32 i;
    s32 mode;
    s32 val;

    for (i = 0; party_id_table[i] != 0; i++) {
        if (partyChkJoin(party_id_table[i])) {
            mode = evtGetValue(event, 0xF8406022);
            if (mode == 0) {
                if (pouch->partyData[party_id_table[i]].techLevel < 1) {
                    val = 0;
                } else {
                    val = 1;
                }
            } else {
                if (pouch->partyData[party_id_table[i]].techLevel < 2) {
                    val = 0;
                } else {
                    val = 2;
                }
            }
            if (val == 0) {
                evtSetValue(event, args[0], 1);
                return 2;
            }
        }
    }
    evtSetValue(event, args[0], 0);
    return 2;
}
