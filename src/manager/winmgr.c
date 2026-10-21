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
#include "driver/icondrv_gx.h"
#include "mario/mariost.h"
#include "evt/evt_lottery.h"
#include "evt/evt_johoya.h"
#include "evt/evt_badgeshop.h"
#include "window/win_item.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "window/win_main.h"
#include <dolphin/pad.h>

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

#define PI 3.14159265358979323846264338327950288419716939937510

// Retain the descriptor tables and constants referenced by other units.
#pragma force_active on
// Named read-only objects follow the original small-data order.
const char labelname_kur[] = "_kur";
const char labelname_nok[] = "_nok";
const char labelname_win[] = "_win";
const char labelname_yos[] = "_yos";
const char labelname_viv[] = "_viv";
const char labelname_bom[] = "_bom";
const char labelname_chu[] = "_chu";
const char labelname_hp[] = "_hp";
const char labelname_fp[] = "_fp";
const char labelname_bp[] = "_bp";
const char status_name_hp[] = "\x82\x67\x82\x6f";
const char status_name_fp[] = "\x82\x65\x82\x6f";
const char status_name_bp[] = "\x82\x61\x82\x6f";
const GXColor waku1_color = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor waku3a_color = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor waku3b_color = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor waku4_color = { 0x2b, 0xa4, 0xff, 0x00 };
const GXColor waku5_color = { 0x2b, 0xa4, 0xff, 0x00 };
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

static WinMgrWork work;
static s16* mario_status_point_table[3];
static WinMgrWork* wp = &work;
u16 party_id_table[8] = { PARTNER_GOOMBELLA, PARTNER_KOOPS, PARTNER_FLURRIE, PARTNER_YOSHI, PARTNER_VIVIAN, PARTNER_BOBBERY, PARTNER_MSMOWZ, PARTNER_NONE };
static u16 party_icon_table[8] = { 0x155, 0x156, 0x15F, 0x158, 0x160, 0x157, 0x161, 0 };
static const char* party_labelname_table[7] = { labelname_kur, labelname_nok, labelname_win, labelname_yos, labelname_viv, labelname_bom, labelname_chu };
static s32 mario_status_icon_table[3] = { 0x19F, 0x1A0, 0x1A1 };
static const char* mario_status_labelname_table[3] = { labelname_hp, labelname_fp, labelname_bp };
static const char* mario_status_name_table[3] = { status_name_hp, status_name_fp, status_name_bp };
static s32 mario_status_henka_table[3] = { 5, 5, 3 };
s32 mario_status_max_table[3] = { 200, 200, 99 };

WinMgrDesc select_desc[] = {
    { 0, 1, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp },
    { 2, 0, 8, -280, 120, 240, 45, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { 0, 0, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3 },
    { 0, 1, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp },
    { 2, 0, 8, -280, 120, 240, 69, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { 0, 0, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3 },
};

WinMgrDesc select_party_desc[] = {
    { 0, 4, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_party },
    { 2, 0, 8, -280, 120, 240, 69, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { 0, 0, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3_party },
};

WinMgrDesc select_mario_desc[] = {
    { 0, 5, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_mario },
    { 2, 0, 8, -280, 120, 240, 69, { 255, 255, 255, 255 }, NULL, select_disp2 },
    { 0, 0, 8, -240, -130, 500, 80, { 255, 255, 255, 255 }, select_main3, select_disp3_mario },
};

WinMgrDesc select_luigi_desc[] = {
    { 0, 1, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_luigi },
    { 2, 0, 8, -280, 120, 240, 45, { 255, 255, 255, 255 }, NULL, select_disp2 },
};

WinMgrDesc select_johoya_perfect_desc[] = {
    { 0, 1, 8, -24, 116, 300, 240, { 255, 255, 255, 255 }, select_main, select_disp_luigi },
    { 2, 0, 8, -280, 120, 240, 45, { 255, 255, 255, 255 }, NULL, select_disp2 },
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
const char party_name_msg0[] = "name_party0";
const char party_name_msg1[] = "name_party1";
const char party_name_msg4[] = "name_party4";
const char party_name_msg5[] = "name_party5";
const char party_name_msg2[] = "name_party2";
const char party_name_msg6[] = "name_party6";

// Keeps the descriptor 0x14 load before 0x10 with retail registers.
static inline s32 get10(WinMgrDesc* d) { return d->y; }

static inline void winMgrDispWaku1(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = entry->alpha;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->x + (entry->width - 150) / 2),
                           (f32)(entry->y + 14), 150.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku3a(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = entry->alpha;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->x + 10), (f32)(entry->y + 14),
                           100.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku3b(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = entry->alpha;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->x + entry->width / 2 - 10),
                           (f32)(entry->y + 14), 100.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku4(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = entry->alpha;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)(entry->x + (entry->width * 3 / 5 - 120) * 2 / 3),
                           (f32)(entry->y + 14), 120.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku5(Mtx mtx, WinMgrEntry* entry, const GXColor* base) {
    GXColor color = *base;
    color.a = entry->alpha;
    windowDispGX2_Waku_col(mtx, 0, color,
                           (f32)(entry->x + entry->width * 3 / 5 + (entry->width * 2 / 5 - 85) / 3),
                           (f32)(entry->y + 14), 85.0f, 28.0f, 8.0f);
}

static inline void winMgrDispWaku0(Mtx mtx, WinMgrEntry* entry) {
    GXColor color = { 0, 0, 0, 0 };
    color.r = entry->description->color[0];
    color.g = entry->description->color[1];
    color.b = entry->description->color[2];
    color.a = entry->alpha;
    windowDispGX2_Waku_col(mtx, 0, color, (f32)entry->x, (f32)entry->y,
                           (f32)entry->width, (f32)entry->height, 20.0f);
}

// Emit the display prefix immediately so its conversions and literals pool in retail order.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

void select_disp3_mario(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;

    pouchGetPtr();
    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = select->items;
        const char* help;
        char buf[0x48];

        if (items[select->cursor].flags & 2) {
            if (items[select->cursor].flags & 4) {
                help = "helperr2";
            } else {
                help = "helperr";
            }
        } else {
            help = "help";
        }
        switch (select->kind) {
        case 9:
            sprintf(buf, "%s_levelup%s", help, mario_status_labelname_table[items[select->cursor].id]);
            break;
        case 10:
        default:
            sprintf(buf, "%s_leveldown%s", help, mario_status_labelname_table[items[select->cursor].id]);
            break;
        }
        entry->message = msgSearch(buf);
        winMgrHelpDraw(entry);
    }
}

void select_disp3_party(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    PouchData* pouch = pouchGetPtr();
    u16 id;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = select->items;
        const char* help;
        const char* lank;
        char buf[0x48];

        if (items[select->cursor].flags & 2) {
            help = "helperr";
        } else {
            help = "help";
        }
        if (select->kind == 6 || select->kind == 7) {
            lank = "lankup";
        } else {
            lank = "lankdown";
        }
        id = items[select->cursor].id;
        sprintf(buf, "%s_%s_lank%d%s", help, lank,
                pouch->partyData[party_id_table[id]].techLevel + 1, party_labelname_table[id]);
        entry->message = msgSearch(buf);
        winMgrHelpDraw(entry);
    }
}

void select_disp3(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = select->items;
        u16 id = items[select->cursor].id;
        ItemData* item = &itemDataTable[id];

        entry->message = msgSearch(item->description);
        winMgrHelpDraw(entry);
    }
}

void select_main3(WinMgrEntry* entry) {
    u16 width = 0;

    if (!(entry->flags & 4)) {
        if (entry->message != NULL) {
            FontGetMessageWidthLine(entry->message, &width);
            entry->scrollRows = width + 1;
        }
        if (entry->scrollRows > 2) {
            if (keyGetDirTrg(0) & 0x100) {
                entry->scrollPos -= 2;
                if (entry->scrollPos < 0) {
                    entry->scrollPos = 0;
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if ((entry->scrollRows + 1) / 2 - 1 > (entry->scrollPos + 1) / 2) {
                    entry->scrollPos += 2;
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

    msg = msgSearch("msg_window_select_1");
    switch (select->kind) {
    case 3:
    case 12:
        msg = msgSearch("msg_window_select_2");
        break;
    case 4:
        msg = msgSearch("msg_window_select_3");
        break;
    case 5:
        msg = msgSearch("msg_window_select_4");
        break;
    case 6:
    case 7:
        msg = msgSearch("msg_window_select_11");
        break;
    case 8:
        msg = msgSearch("msg_window_select_12");
        break;
    case 9:
        msg = msgSearch("msg_window_select_9");
        break;
    case 10:
        msg = msgSearch("msg_window_select_9_1");
        break;
    case 11:
    case 14:
    case 15:
        msg = msgSearch("msg_window_select_6");
        break;
    case 13:
    case 16:
        msg = msgSearch("msg_window_select_10");
        break;
    case 17:
        msg = msgSearch("msg_window_select_8");
        break;
    case 18:
        msg = msgSearch("msg_window_select_7");
        break;
    }
    if (!(wp->mEntries[select->descriptorId[1]].flags & 4)) {
        x = (entry->width - FontGetMessageWidthLine(msg, &width)) / 2;

        FontDrawStart();
        FontDrawMessage(entry->x + x,
                        entry->y - (entry->height - (width + 1) * 24) / 2, msg);
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
    // The original stores the fog colour in fogColor's own slot (0x18) and passes a
    // pointer to it; with the by-value GXSetFog prototype MWCC copies it to a call temp at 0xc and
    // the loop/edge/title colour locals move to 0x18/0x14/0x10. The cast only changes the calling
    // convention (GXSetFog reads the colour, never writes it) and reproduces the original slots.
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->x + 0x130, 0x112 - entry->y, entry->width, entry->height - 0x32);

    zero = 0.0f;
    info = jdt->info;
    for (i = 0; i < select->numItems; i++, info++) {
        char saved;
        f32 row = (f32)(entry->y - 0x2c - i * 0x18);
        f32 bound = select->baseY + (zero + row);

        if ((f32)entry->y < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->y - entry->height) > bound + 32.0f) {
            continue;
        }

        winFontInit();
        saved = ((char*)info->initEndPos)[info->tagEndPos];
        ((char*)info->initEndPos)[info->tagEndPos] = 0;
        {
            GXColor color = (select->items[i].flags & 1) ? colorGray : colorBlack;
            f32 rowY;

            tmpRow = vec3_802ffcc0;
            scaleRow = vec3_802ffccc;
            tmpRow.x = (f32)(entry->x + 0x19);
            rowY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpRow.y = select->baseY + (zero + rowY);
            posRow = tmpRow;
            winFontSet(&posRow, &scaleRow, &color, (const char*)info->initEndPos);
        }
        ((char*)info->initEndPos)[info->tagEndPos] = saved;

        if (select->kind == 0x11 && !(select->items[i].flags & 1)) {
            GXColor color;
            f32 edgeY;

            sprintf(buf, "%3d", info->unkA);
            winFontInit();
            color = (select->items[i].flags & 1) ? colorGray : colorBlack;
            tmpEdge = vec3_802ffcd8;
            scaleEdge = vec3_802ffce4;
            tmpEdge.x = (f32)(entry->x + 0xf5);
            edgeY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpEdge.y = select->baseY + (zero + edgeY);
            posEdge = tmpEdge;
            winFontSet(&posEdge, &scaleEdge, &color, buf);
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    switch (select->kind) {
    case 0x11:
        msg = msgSearch("msg_window_title_5");
        break;
    case 0x12:
    default:
        msg = msgSearch("msg_window_title_6");
        break;
    }

    width = FontGetMessageWidth(msg);
    x = (entry->width - width) / 2;
    winFontInit();
    {
        GXColor color = colorWhite;
        tmpTitle = vec3_802ffcf0;
        scaleTitle = vec3_802ffcfc;
        tmpTitle.x = (f32)(entry->x + x);
        tmpTitle.y = (f32)(entry->y + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

    switch (select->kind) {
    case 0x11:
        PSMTXTrans(mtx1, (f32)(entry->x + 0x10c), (f32)(entry->y - 0x12), 0.0f);
        PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
        PSMTXConcat(mtx1, mtx2, mtx1);
        iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a74);
        posLuigi = vec3_802ffd08;
        posLuigi.x = (f32)(entry->x + 0x10c);
        posLuigi.y = (f32)(entry->y - 0xc);
        iconDispGx(0.6f, &posLuigi, 0x10, 0x18b);
        break;
    }

    posMain = vec3_802ffd14;
    posMain.x = select->iconX;
    posMain.y = select->iconY;
    iconDispGx(1.0f, &posMain, 0x14, 0x1f0);

    if (select->numItems > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->scrollTop != 0) {
            posUp = vec3_802ffd20;
            // Division by two pools 0.5f while preserving retail multiplication operand order.
            posUp.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posUp.y = (f32)(entry->y - 0x24);
            iconDispGx(0.6f, &posUp, 0x10, 0x1b5);
        }
        if (select->scrollTop != select->numItems - 8) {
            posDown = vec3_802ffd2c;
            posDown.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posDown.y = (f32)(entry->y - entry->height - 0xc);
            iconDispGx(0.6f, &posDown, 0x10, 0x1b6);
        }
    }
}

// Every call, constant, branch, stack slot and store order matches. The (f32)int conversion double
// is pooled at .rodata+0x468 by select_disp_luigi's native conversion. The outer loop
// keeps one induction variable and writes the row as `i * 0x18`, so the compiler strength-reduces
// both offsets from one base and initialises the row offset with a copy (`mr r27,r28`).
// The target's dead `lwz r0,0x38(r24); cmpwi r0,0` before the first winFontInit is reproduced by
// `if (select->firstItem) { zero = zero; }` (the dead store is removed, the compare stays).
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
    GXColor colorIcon;
    GXColor colorTitle;
    GXColor fogColor;
    const char* msg;
    u16 x;
    f32 zero;
    s32 status;

    pouchGetPtr();
    colorBlack = dat_80421a54;
    colorGray = dat_80421a58;
    colorTitle = dat_80421a5c;
    colorIcon = dat_80421a5c;

    if (wp->mEntries[select->descriptorId[0]].flags & 4) {
        return;
    }

    fogColor = dat_80421a60;
#ifdef MUST_MATCH
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->x + 0x130, 0x112 - entry->y, entry->width, entry->height - 0x32);

    zero = 0.0f;
    for (i = 0; i < select->numItems; i++) {
        f32 row;
        f32 bound;

        row = (f32)(entry->y - 0x2c - i * 0x18);
        bound = select->baseY + (zero + row);
        status = select->items[i].id;
        if ((f32)entry->y < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->y - entry->height) > bound + 32.0f) {
            continue;
        }

#ifdef MUST_MATCH
        // Reproduces the target's dead `lwz r0,0x38; cmpwi r0,0` before winFontInit: the self-store is
        // removed but the compare stays. The natural code has no such compare.
        if (*(volatile s32*)&select->firstItem) {
            zero = zero;
        }
#endif
        winFontInit();
        {
            GXColor color = (select->items[i].flags & 1) ? colorGray : colorBlack;
            f32 rowY;

            tmpName = vec3_802ffbf4;
            scaleName = vec3_802ffc00;
            tmpName.x = (f32)(entry->x + 0x3c);
            rowY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpName.y = select->baseY + (zero + rowY);
            posName = tmpName;
            winFontSet(&posName, &scaleName, &color, mario_status_name_table[status]);
        }

        if (select->items[i].flags & 1) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        {
            GXColor color = colorIcon;
            f32 rowY;

            tmpIcon = vec3_802ffc0c;
            scaleIcon = vec3_802ffc18;
            tmpIcon.x = (f32)(entry->x + 0x23);
            rowY = (f32)(entry->y - 0x2c - i * 0x18);
            tmpIcon.y = select->baseY + (zero + rowY);
            posIcon = tmpIcon;
            winIconSet(mario_status_icon_table[status], &posIcon, &scaleIcon, &color);
        }

        sprintf(buf, "%3d", mario_status_point_table[status][0]);

        winFontInit();
        {
            GXColor color = (select->items[i].flags & 1) ? colorGray : colorBlack;
            f32 rowY;

            tmpPoint = vec3_802ffc24;
            scalePoint = vec3_802ffc30;
            tmpPoint.x = (f32)(entry->x + 0xa0);
            rowY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpPoint.y = select->baseY + (zero + rowY);
            posPoint = tmpPoint;
            winFontSet(&posPoint, &scalePoint, &color, buf);
        }

        if (!(select->items[i].flags & 2)) {
            switch (select->kind) {
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
                GXColor color = (select->items[i].flags & 1) ? colorGray : colorBlack;
                f32 rowY;

                tmpHenka = vec3_802ffc3c;
                scaleHenka = vec3_802ffc48;
                tmpHenka.x = (f32)(entry->x + 0xf0);
                rowY = (f32)(entry->y - 0x20 - i * 0x18);
                tmpHenka.y = select->baseY + (zero + rowY);
                posHenka = tmpHenka;
                winFontSet(&posHenka, &scaleHenka, &color, buf);
            }

            winIconInit();
            {
                GXColor color = colorIcon;
                f32 rowY;

                tmpIcon2 = vec3_802ffc54;
                scaleIcon2 = vec3_802ffc60;
                tmpIcon2.x = (f32)(entry->x + 0xdc);
                rowY = (f32)(entry->y - 0x2c - i * 0x18);
                tmpIcon2.y = select->baseY + (zero + rowY);
                posIcon2 = tmpIcon2;
                winIconSet(0x163, &posIcon2, &scaleIcon2, &color);
            }
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    msg = msgSearch("msg_window_title_9");
    x = (0x78 - FontGetMessageWidth(msg)) / 2;
    winFontInit();
    {
        GXColor color = colorTitle;

        tmpTitle = vec3_802ffc6c;
        scaleTitle = vec3_802ffc78;
        tmpTitle.x = (f32)(entry->x + (entry->width * 3 / 5 - 0x78) * 2 / 3 + x);
        tmpTitle.y = (f32)(entry->y + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

    msg = msgSearch("msg_window_title_10");
    x = (0x55 - FontGetMessageWidth(msg)) / 2;
    winFontInit();
    {
        GXColor color = colorTitle;

        tmpTitle2 = vec3_802ffc84;
        scaleTitle2 = vec3_802ffc90;
        tmpTitle2.x = (f32)(entry->x + entry->width * 3 / 5 + (entry->width * 2 / 5 - 0x55) / 3 + x);
        tmpTitle2.y = (f32)(entry->y + 0xe);
        posTitle2 = tmpTitle2;
        winFontSetEdge(&posTitle2, &scaleTitle2, &color, msg);
    }

    posMain = vec3_802ffc9c;
    posMain.x = select->iconX;
    posMain.y = select->iconY;
    iconDispGx(1.0f, &posMain, 0x14, 0x1f0);

    if (select->numItems > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->scrollTop != 0) {
            posUp = vec3_802ffca8;
            posUp.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posUp.y = (f32)(entry->y - 0x24);
            iconDispGx(0.6f, &posUp, 0x10, 0x1b5);
        }
        if (select->scrollTop != select->numItems - 8) {
            posDown = vec3_802ffcb4;
            posDown.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posDown.y = (f32)(entry->y - entry->height - 0xc);
            iconDispGx(0.6f, &posDown, 0x10, 0x1b6);
        }
    }
}


// Complete reconstruction; all 48 calls, the four jump tables, the techLevel loop bound
// (recomputed every arrow iteration) and the name-switch phi match. Two target behaviours had
// to be reproduced explicitly: (1) the name/icon switches have no `default:` (a value > 6 falls
// through with the result variable uninitialised), and that is what makes MWCC materialise each
// switch result in r23/r24/r25 (a `mr` per case plus the join `mr r3,r25`); merging case 6 with
// default coalesces them away. (2) The target reloads several in-loop vec constants every
// iteration where MWCC would hoist them: vec3_802ffb88/94 are read through `const volatile` (assignments marked "anti-hoist"
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
// offset with a copy (`mr r28,r27`) instead of a shift. The target's dead `lwz r0,0x38(r30); cmpwi
// r0,0x0` before the first winFontInit is reproduced by `if (select->firstItem) { zero = zero; }`
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
    // The original stores the fog colour in fogColor's own slot and passes a pointer
    // to it; with the by-value GXSetFog prototype MWCC copies it to a call temp. Same as select_disp.
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->x + 0x130, 0x112 - entry->y, entry->width, entry->height - 0x32);

    zero = 0.0f;
    for (i = 0; i < select->numItems; i++) {
        f32 row;
        f32 bound;
        u16 w;

        row = (f32)(entry->y - 0x2c - i * 0x18);
        bound = select->baseY + (zero + row);
        if ((f32)entry->y < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->y - entry->height) > bound + 32.0f) {
            continue;
        }

#ifdef MUST_MATCH
        // Reproduces the target's dead `lwz r0,0x38; cmpwi r0,0` before winFontInit: the self-store is
        // removed but the compare stays. The natural code has no such compare.
        if (*(volatile s32*)&select->firstItem) {
            zero = zero;
        }
#endif
        winFontInit();
        {
            GXColor color;
            f32 rowY;
            const char* n1;
            const char* n2;
            const char* n3;

            tmpName = vec3_802ffb58;
            tmpName.x = (f32)(entry->x + 0x3c);
            rowY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpName.y = select->baseY + (zero + rowY);
            posName = tmpName;
            scaleTmp = vec3_802ffb64;

            switch (select->items[i].id) {
            case 0:
                n1 = msgSearch(party_name_msg0);
                break;
            case 1:
                n1 = msgSearch(party_name_msg1);
                break;
            case 2:
                n1 = msgSearch(party_name_msg4);
                break;
            case 3:
                n1 = pouchGetYoshiName();
                break;
            case 4:
                n1 = msgSearch(party_name_msg5);
                break;
            case 5:
                n1 = msgSearch(party_name_msg2);
                break;
            case 6:
                n1 = msgSearch(party_name_msg6);
                break;
            }

            w = FontGetMessageWidth(n1);
            if (w > 0xaa) {
                switch (select->items[i].id) {
                case 0:
                    n2 = msgSearch(party_name_msg0);
                    break;
                case 1:
                    n2 = msgSearch(party_name_msg1);
                    break;
                case 2:
                    n2 = msgSearch(party_name_msg4);
                    break;
                case 3:
                    n2 = pouchGetYoshiName();
                    break;
                case 4:
                    n2 = msgSearch(party_name_msg5);
                    break;
                case 5:
                    n2 = msgSearch(party_name_msg2);
                    break;
                case 6:
                    n2 = msgSearch(party_name_msg6);
                    break;
                }
                scale = 170.0f / (f32)FontGetMessageWidth(n2);
            } else {
                scale = 1.0f;
            }
            scaleTmp.x = scale;
            scaleName = scaleTmp;

            color = (select->items[i].flags & 1) ? colorGray : colorBlack;

            switch (select->items[i].id) {
            case 0:
                n3 = msgSearch(party_name_msg0);
                break;
            case 1:
                n3 = msgSearch(party_name_msg1);
                break;
            case 2:
                n3 = msgSearch(party_name_msg4);
                break;
            case 3:
                n3 = pouchGetYoshiName();
                break;
            case 4:
                n3 = msgSearch(party_name_msg5);
                break;
            case 5:
                n3 = msgSearch(party_name_msg2);
                break;
            case 6:
                n3 = msgSearch(party_name_msg6);
                break;
            }
            winFontSet(&posName, &scaleName, &color, n3);
        }

        if (select->items[i].flags & 1) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        {
            f32 rowY;
            s32 icon;
            GXColor color;

            tmpIcon = vec3_802ffb70;
            tmpIcon.x = (f32)(entry->x + 0x23);
            rowY = (f32)(entry->y - 0x2c - i * 0x18);
            tmpIcon.y = select->baseY + (zero + rowY);
            posIcon = tmpIcon;
            scaleIcon = vec3_802ffb7c;

            color = colorWhite;
            icon = party_icon_table[select->items[i].id];
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

            for (k = 0, off = 0; k < pouch->partyData[party_id_table[select->items[i].id]].techLevel; off += 0x1b, k++) {
                if (select->items[i].flags & 1) {
                    winIconGrayInit();
                } else {
                    winIconInit();
                }
                {
                    GXColor c = colorWhite;
                    f32 rowY;

#ifdef MUST_MATCH
                    // anti-hoist: retail reloads the Vec in the loop
                    scaleArrow = *(const volatile Vec*)&vec3_802ffb94;
                    tmpArrow = *(const volatile Vec*)&vec3_802ffb88;
#else
                    scaleArrow = vec3_802ffb94;
                    tmpArrow = vec3_802ffb88;
#endif
                    tmpArrow.x = (f32)(entry->x + 0xfd + off);
                    rowY = (f32)(entry->y - 0x2c - i * 0x18);
                    tmpArrow.y = select->baseY + (zero + rowY);
                    posArrow = tmpArrow;
                    winIconSet(0x102, &posArrow, &scaleArrow, &c);
                }
            }
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    msg = msgSearch("msg_window_title_7");
    x = (0x78 - FontGetMessageWidth(msg)) / 2;
    winFontInit();
    {
        GXColor color = colorWhite2;

        tmpTitle = vec3_802ffba0;
        scaleTitle = vec3_802ffbac;
        tmpTitle.x = (f32)(entry->x + (entry->width * 3 / 5 - 0x78) * 2 / 3 + x);
        tmpTitle.y = (f32)(entry->y + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

    msg = msgSearch("msg_window_title_8");
    x = (0x55 - FontGetMessageWidth(msg)) / 2;
    winFontInit();
    {
        GXColor color = colorWhite2;

        tmpTitle2 = vec3_802ffbb8;
        scaleTitle2 = vec3_802ffbc4;
        tmpTitle2.x = (f32)(entry->x + entry->width * 3 / 5 + (entry->width * 2 / 5 - 0x55) / 3 + x);
        tmpTitle2.y = (f32)(entry->y + 0xe);
        posTitle2 = tmpTitle2;
        winFontSetEdge(&posTitle2, &scaleTitle2, &color, msg);
    }

    posMain = vec3_802ffbd0;
    posMain.x = select->iconX;
    posMain.y = select->iconY;
    iconDispGx(1.0f, &posMain, 0x14, 0x1f0);

    if (select->numItems > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->scrollTop != 0) {
            posUp = vec3_802ffbdc;
            posUp.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posUp.y = (f32)(entry->y - 0x24);
            iconDispGx(0.6f, &posUp, 0x10, 0x1b5);
        }
        if (select->scrollTop != select->numItems - 8) {
            posDown = vec3_802ffbe8;
            posDown.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posDown.y = (f32)(entry->y - entry->height - 0xc);
            iconDispGx(0.6f, &posDown, 0x10, 0x1b6);
        }
    }
}

// The item loop matches register for register (entry r31, select r25, rodata r28, itemDataTable r29,
// 0x4330 r30, counter r24, rowoff r26, itemoff r27, item r22, value r23). The dots loop matches too:
// vec3_802ffae0/vec3_802ffaec are declared non-const (section ".rodata") above so MWCC cannot prove
// the winIconSet call leaves them alone and therefore reloads them inside the loop in caller-saved
// registers (r10/r11/r12) instead of hoisting them into callee-saved r21/r27/r29 and pulling r20/r21
// into the frame; the scaleDot/tmpDot assignments also have to sit before the dotX/dotY computation
// to reproduce the original schedule.
// Native conversions in select_disp_luigi and select_disp_party pool the signed/unsigned doubles
// at .rodata+0x468/+0x4c0. The dead `lwz r0, 0x38(r25); cmpwi r0, 0x0` pair at the top of the
// item loop is reproduced by `if (*(volatile s32*)&select->firstItem) { zero = zero; }`.
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
    // The original stores the fog colour in fogColor's own slot and passes a pointer
    // to it; with the by-value GXSetFog prototype MWCC copies it to a call temp. Same as select_disp_luigi.
    ((void (*)(s32, f32, f32, f32, f32, GXColor*))GXSetFog)(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, &fogColor);
#else
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
    GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
    GXSetScissor(entry->x + 0x130, 0x112 - entry->y, entry->width, entry->height - 0x32);

    zero = 0.0f;
    for (i = 0; i < select->numItems; i++) {
        f32 row;
        f32 bound;
        item = &itemDataTable[select->items[i].id];
        row = (f32)(entry->y - 0x2c - i * 0x18);
        bound = select->baseY + (zero + row);
        if ((f32)entry->y < bound - 32.0f) {
            continue;
        }
        if ((f32)(entry->y - entry->height) > bound + 32.0f) {
            continue;
        }
#ifdef MUST_MATCH
        // Reproduces the target's dead `lwz r0,0x38; cmpwi r0,0` before winFontInit: the self-store is
        // removed but the compare stays. The natural code has no such compare.
        if (*(volatile s32*)&select->firstItem) {
            zero = zero;
        }
#endif
        if (select->items[i].flags & 1) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        {
            GXColor color = colorWhite;
            f32 rowY;

            tmpRow = vec3_802ffa98;
            scaleRow = vec3_802ffaa4;
            tmpRow.x = (f32)(entry->x + 0x23);
            rowY = (f32)(entry->y - 0x2c - i * 0x18);
            tmpRow.y = select->baseY + (zero + rowY);
            posRow = tmpRow;
            winIconSet(*(u16*)&item->icon_id, &posRow, &scaleRow, &color);
        }
        winFontInit();
        {
            GXColor color = (select->items[i].flags & 1) ? colorGray : colorBlack;
            f32 nameY;

            tmpName = vec3_802ffab0;
            scaleName = vec3_802ffabc;
            tmpName.x = (f32)(entry->x + 0x3c);
            nameY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpName.y = select->baseY + (zero + nameY);
            posName = tmpName;
            winFontSet(&posName, &scaleName, &color, msgSearch(item->name));
        }

        hasValue = 0;
        switch (select->kind) {
        case 11:
            value = ((ItemDataView*)item)->buy_price;
            hasValue = 1;
            break;
        case 3:
        case 12:
            value = item->sell_price;
            hasValue = 1;
            break;
        case 13:
            value = item->star_piece_price;
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

            sprintf(buf, "%d", value);
            textWidth = (f32)FontGetMessageWidth(buf);
            winFontInit();
            color = (select->items[i].flags & 1) ? colorGray : colorBlack;
            scaleEdge = vec3_802ffad4;
            tmpEdge = vec3_802ffac8;
            edgeX = (f32)(entry->x + entry->width - 0xa) - textWidth;
            edgeY = (f32)(entry->y - 0x20 - i * 0x18);
            tmpEdge.x = edgeX;
            tmpEdge.y = select->baseY + (zero + edgeY);
            posEdge = tmpEdge;
            winFontSet(&posEdge, &scaleEdge, &color, buf);
        }

    }

    if (select->firstItem) {
        winIconInit();
        for (i = 2; i < 0x1e; i++) {
            GXColor color = colorBlack;
            f32 dotX;
            f32 dotY;

            scaleDot = vec3_802ffaec;
            tmpDot = vec3_802ffae0;
            dotX = (f32)(entry->x + (i * entry->width) / 31);
            dotY = (f32)(entry->y - 0x38) + select->baseY;
            tmpDot.x = dotX;
            tmpDot.y = dotY;
            posDot = tmpDot;
            winIconSet(0x1b7, &posDot, &scaleDot, &color);
        }
    }

    GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);

    switch (select->kind) {
    case 2:
    case 11:
    case 12:
    case 13:
        msg = msgSearch("msg_window_title_2");
        break;
    case 14:
        msg = msgSearch("msg_window_title_3");
        break;
    case 16:
        msg = msgSearch("msg_window_title_4");
        break;
    case 17:
        msg = msgSearch("msg_window_title_5");
        break;
    default:
        msg = msgSearch("msg_window_title_1");
        break;
    }

    width = FontGetMessageWidth(msg);
    x = (entry->width - width) / 2;
    winFontInit();
    {
        GXColor color = colorWhite2;

        tmpTitle = vec3_802ffaf8;
        scaleTitle = vec3_802ffb04;
        tmpTitle.x = (f32)(entry->x + x);
        tmpTitle.y = (f32)(entry->y + 0xe);
        posTitle = tmpTitle;
        winFontSetEdge(&posTitle, &scaleTitle, &color, msg);
    }

        switch (select->kind) {
        case 3:
        case 11:
        case 12:
        case 14:
        case 15:
            PSMTXTrans(mtx1, (f32)(entry->x + 0x10c), (f32)(entry->y - 0x12), 0.0f);
            PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
            PSMTXConcat(mtx1, mtx2, mtx1);
            iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a38);
            posIcon3 = vec3_802ffb10;
            posIcon3.x = (f32)(entry->x + 0x10c);
            posIcon3.y = (f32)(entry->y - 0xc);
            iconDispGx(0.6f, &posIcon3, 0x10, 0x18b);
            break;
        case 13:
            PSMTXTrans(mtx1, (f32)(entry->x + 0x10c), (f32)(entry->y - 0x12), 0.0f);
            PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
            PSMTXConcat(mtx1, mtx2, mtx1);
            iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a3c);
            posIcon13 = vec3_802ffb1c;
            posIcon13.x = (f32)(entry->x + 0x10c);
            posIcon13.y = (f32)(entry->y - 0xc);
            iconDispGx(0.6f, &posIcon13, 0x10, 0x18d);
            break;
        case 16:
            PSMTXTrans(mtx1, (f32)(entry->x + 0x10c), (f32)(entry->y - 0x12), 0.0f);
            PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
            PSMTXConcat(mtx1, mtx2, mtx1);
            iconDispGxCol(mtx1, 0x10, 0x211, dat_80421a40);
            posIcon16 = vec3_802ffb28;
            posIcon16.x = (f32)(entry->x + 0x10c);
            posIcon16.y = (f32)(entry->y - 0xc);
            iconDispGx(0.6f, &posIcon16, 0x10, 0x13f);
            break;
        }

    posMain = vec3_802ffb34;
    posMain.x = select->iconX;
    posMain.y = select->iconY;
    iconDispGx(1.0f, &posMain, 0x14, 0x1f0);

    if (select->numItems > 8 && (gp->retraceCount & 0x1F) < 20) {
        if (select->scrollTop != 0) {
            posUp = vec3_802ffb40;
            posUp.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posUp.y = (f32)(entry->y - 0x24);
            iconDispGx(0.6f, &posUp, 0x10, 0x1b5);
        }
        if (select->scrollTop != select->numItems - 8) {
            posDown = vec3_802ffb4c;
            posDown.x = (f32)entry->x + (f32)entry->width / 2.0f;
            posDown.y = (f32)(entry->y - entry->height - 0xc);
            iconDispGx(0.6f, &posDown, 0x10, 0x1b6);
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

    switch (entry->description->frameStyle) {
    case 0:
    case 2:
        break;
    case 1:
        winMgrDispWaku1(scalemtx, entry, &waku1_color);
        break;
    case 3:
        winMgrDispWaku3a(scalemtx, entry, &waku3a_color);
        winMgrDispWaku3b(scalemtx, entry, &waku3b_color);
        break;
    case 4:
    case 5:
        winMgrDispWaku4(scalemtx, entry, &waku4_color);
        winMgrDispWaku5(scalemtx, entry, &waku5_color);
        break;
    }

    if (entry->description->disp) {
        entry->description->disp(entry);
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

    switch (entry->description->openStyle) {
    case 0:
        switch (entry->state) {
        case 0:
            entry->flags &= ~4;
            break;
        case 1:
            entry->flags |= 2;
            entry->rotation = intplGetValue(0xc, entry->animStep, 8, 30.0f, 0.0f);
            entry->scale = intplGetValue(0xc, entry->animStep, 8, 0.0f, 1.0f);
            entry->alpha = 0xff;
            entry->animStep++;
            if (entry->animStep > 8) {
                entry->state = 0;
            }
            break;
        case 2:
            entry->rotation = intplGetValue(0xc, entry->animStep, 8, 0.0f, 30.0f);
            entry->scale = intplGetValue(0xc, entry->animStep, 8, 1.0f, 0.0f);
            entry->alpha = 0xff;
            entry->animStep++;
            if (entry->animStep > 8) {
                entry->state = 0;
                entry->flags &= ~2;
                if (entry->flags & 8) {
                    entry->flags &= ~1;
                }
            }
            break;
        }
        break;
    case 1:
        switch (entry->state) {
        case 0:
            entry->flags &= ~4;
            break;
        case 1:
            entry->flags |= 2;
            entry->rotation = 0.0f;
            entry->scale = intplGetValue(0xc, entry->animStep, 8, 0.0f, 1.0f);
            entry->alpha = (s32)intplGetValue(0xc, entry->animStep, 8, 0.0f, 255.0f);
            entry->animStep++;
            if (entry->animStep > 8) {
                entry->state = 0;
            }
            break;
        case 2:
            entry->rotation = 0.0f;
            entry->scale = 1.0f;
            entry->alpha = (s32)intplGetValue(0xc, entry->animStep, 8, 255.0f, 0.0f);
            entry->animStep++;
            if (entry->animStep > 8) {
                entry->state = 0;
                entry->flags &= ~2;
                if (entry->flags & 8) {
                    entry->flags &= ~1;
                }
            }
            break;
        }
        break;
    case 2:
        switch (entry->state) {
        case 0:
            entry->flags &= ~4;
            break;
        case 1:
            entry->flags |= 2;
            entry->rotation = 0.0f;
            entry->scale = 1.0f;
            entry->alpha = 0xff;
            entry->state = 0;
            break;
        case 2:
            entry->flags &= ~2;
            entry->rotation = 0.0f;
            entry->scale = 1.0f;
            entry->alpha = 0xff;
            entry->state = 0;
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

    entry->state = 0;
    entry->animStep = 0;
    entry->description = desc;
    entry->x = entry->description->x;
    entry->y = entry->description->y;
    entry->width = entry->description->width;
    entry->height = entry->description->height;

    entry->priority = 0;

    entry->param = NULL;

    entry->scrollPos = 0;
    entry->scrollRows = 0;

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
        entry->state = 1;
        entry->animStep = 0;
        entry->scrollPos = 0;
        entry->scrollRows = 0;
        entry->message = 0;
    }
}

void winMgrClose(s32 id) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->state = 2;
        entry->animStep = 0;
    }
}

void winMgrCloseAutoDelete(s32 id) {
    WinMgrEntry* entry; // r4

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->state = 2;
        entry->animStep = 0;
        entry->flags |= 8;
    }
}

void winMgrDelete(s32 id) {
    wp->mEntries[id].flags &= ~1;
}

BOOL winMgrAction(s32 id) {
    return (wp->mEntries[id].flags >> 2) & 1;
}

void winMgrSetSize(s32 id, s32 x, s32 y, s32 width, s32 height) {
    WinMgrEntry* entry;

    entry = &wp->mEntries[id];
    if (entry->flags & 1) {
        entry->x = x;
        entry->y = y;
        entry->width = width;
        entry->height = height;
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
    entry->scrollPos = 0;
    entry->scrollRows = 0;
    entry->message = 0;
}

void winMgrHelpMain(WinMgrEntry* entry) {
    u16 width = 0;

    if (!(entry->flags & 4)) {
        if (entry->message != NULL) {
            FontGetMessageWidthLine(entry->message, &width);
            entry->scrollRows = width + 1;
        }
        if (entry->scrollRows > 2) {
            if (keyGetDirTrg(0) & 0x100) {
                entry->scrollPos -= 2;
                if (entry->scrollPos < 0) {
                    entry->scrollPos = 0;
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if ((entry->scrollRows + 1) / 2 - 1 > (entry->scrollPos + 1) / 2) {
                    entry->scrollPos += 2;
                }
            }
        }
    }
}

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
    if (strcmp(entry->message, msgSearch("msg_lottery_number\000")) == 0) {
        lottery = lotteryGetPtr();
        n = lottery->mTicketNum;
        sprintf(buf, entry->message, n);
        FontDrawStart();
        FontDrawMessage(-0xd2, (s32)(28.0f * entry->scrollPos + WINMGR_F32(float_neg144_80421aa8)), buf);
    } else {
        FontDrawStart();
        FontDrawMessage(-0xd2, (s32)(28.0f * entry->scrollPos + WINMGR_F32(float_neg144_80421aa8)), entry->message);
    }
    GXSetScissor(0, 0, 0x260, 0x1e0);
    if (entry->scrollRows > 2) {
        s32 row = entry->scrollPos;
        if (row == 0) {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->x + entry->width - 0xC),
                           (f32)(entry->y - entry->height - 4), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B6);
            }
        } else if (row + 2 == entry->scrollRows) {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->x + entry->width - 0xC),
                           (f32)(entry->y - entry->height + 0x2C), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B5);
            }
        } else {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->x + entry->width - 0xC),
                           (f32)(entry->y - entry->height + 0x2C), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B5);
                PSMTXTrans(mtx1, (f32)(entry->x + entry->width - 0xC),
                           (f32)(entry->y - entry->height - 4), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B6);
            }
        }
        PSMTXTrans(mtx1, (f32)(entry->x + entry->width - 0xC),
                   (f32)(entry->y - entry->height + 0xA), 0.0f);
        PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
        PSMTXConcat(mtx1, mtx2, mtx1);
        iconDispGx2(mtx1, 0x10, 0x70);
    }
}

WinMgrSelect* winMgrSelectEntry(s32 kind, s32 firstItem, BOOL canCancel) {
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

    select->kind = kind;
    descId = kind;
    select->cursor = 0;
    select->scrollTop = 0;
    select->iconY = 0.0f;
    select->iconX = 0.0f;
    select->baseY = 0.0f;
    select->firstItem = firstItem;

    if (canCancel) {
        select->flags |= 0x100;
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
    select->iconX = (f32)desc->x;
    select->iconY = (f32)(desc->y - 54);

    type = select->kind;
    switch (type) {
    case 0:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x1e8);
        memset(select->items, 0, 0x1e8);
        select->numItems = 0;
        if (firstItem != 0) {
            {
                WinMgrSelectItem* it = &select->items[select->numItems++];
                it->id = firstItem;
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
                    select->items[select->numItems++].id = item;
                }
            }
        }
        break;
    case 1:
    case 3:
    case 4:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x54);
        memset(select->items, 0, 0x54);
        select->numItems = 0;
        if (firstItem != 0) {
            {
                WinMgrSelectItem* it = &select->items[select->numItems++];
                it->id = firstItem;
            }
        }
        for (i = 0; i < 0x14; i++) {
            item = pouchHaveItem(i);
            if (item != 0) {
                select->items[select->numItems++].id = item;
            }
        }
        break;
    case 2:
    case 12:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x324);
        memset(select->items, 0, 0x324);
        select->numItems = 0;
        if (firstItem != 0) {
            {
                WinMgrSelectItem* it = &select->items[select->numItems++];
                it->id = firstItem;
            }
        }
        for (i = 0; i < 0xc8; i++) {
            item = pouchHaveBadge(i);
            if (item != 0) {
                select->items[select->numItems++].id = item;
            }
        }
        break;
    case 5:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x84);
        memset(select->items, 0, 0x84);
        select->numItems = 0;
        if (firstItem != 0) {
            {
                WinMgrSelectItem* it = &select->items[select->numItems++];
                it->id = firstItem;
            }
        }
        for (i = 0; i < 0x20; i++) {
            item = pouchKeepItem(i);
            if (item != 0) {
                select->items[select->numItems++].id = item;
            }
        }
        break;
    case 6:
    case 7:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x1c);
        memset(select->items, 0, 0x1c);
        select->numItems = 0;
        for (i = 0; party_id_table[i] != 0; i++) {
            if (partyChkJoin(party_id_table[i])) {
                select->items[select->numItems].id = i;
                if (evtGetValue(0, 0xF8406022) == 0) {
                    if (pouch->partyData[party_id_table[i]].techLevel >= 1) {
                        select->items[select->numItems].flags |= 3;
                    }
                } else {
                    if (pouch->partyData[party_id_table[i]].techLevel >= 2) {
                        select->items[select->numItems].flags |= 3;
                    }
                }
                select->numItems++;
            }
        }
        break;
    case 8:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x1c);
        memset(select->items, 0, 0x1c);
        select->numItems = 0;
        for (i = 0; party_id_table[i] != 0; i++) {
            if (partyChkJoin(party_id_table[i])) {
                if (party_id_table[i] == evtGetValue(0, 0xFD050F81)) {
                    continue;
                }
                select->items[select->numItems].id = i;
                if (pouch->partyData[party_id_table[i]].techLevel == 0) {
                    select->items[select->numItems].flags |= 3;
                }
                select->numItems++;
            }
        }
        break;
    case 9:
        mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
        mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
        mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0xc);
        memset(select->items, 0, 0xc);
        for (i = 0; i < 3; i++) {
            s32 canDown = 0;
            s32 canUp = 0;

            select->items[i].id = i;
            for (j = 0; j < 3; j++) {
                if (select->items[i].id != j) {
                    if (*mario_status_point_table[j] - mario_status_henka_table[j] <= 0) {
                        canDown++;
                    }
                } else if (*mario_status_point_table[j] + mario_status_henka_table[j] > mario_status_max_table[j]) {
                    canUp = 1;
                }
            }
            if (canDown == 2 || canUp != 0) {
                if (canUp != 0) {
                    select->items[i].flags |= 7;
                } else {
                    select->items[i].flags |= 3;
                }
            }
        }
        select->numItems = 3;
        break;
    case 10:
        mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
        mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
        mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 8);
        memset(select->items, 0, 8);
        select->numItems = 0;
        for (i = 0; i < 3; i++) {
            if (i == evtGetValue(0, 0xFD050F81)) {
                continue;
            }
            select->items[select->numItems].id = i;
            if (*mario_status_point_table[i] - mario_status_henka_table[i] <= 0) {
                select->items[select->numItems].flags |= 3;
            }
            select->numItems++;
        }
        break;
    case 11:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x18c);
        memset(select->items, 0, 0x18c);
        select->numItems = 0;
        for (i = 0; i < BADGE_COUNT; i++) {
            s32 v = badgeShop_get(bdsw->bargain_table, (s16)(i + BADGE_MIN));

            if (v - badgeShop_ThrowCheck(i + BADGE_MIN) > 0) {
                select->items[select->numItems++].id = i + BADGE_MIN;
            }
        }
        qqsort(select->items, select->numItems, 4, winMgrSelectCmpTypeSort);
        break;
    case 13:
        {
            s32 max = getBadgeStarmaniacTableMaxCount();

            select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, max * 4);
            memset(select->items, 0, max * 4);
            select->numItems = 0;
            for (i = 0; i < BADGE_COUNT; i++) {
#ifdef MUST_MATCH
                // No-op cast: the value is already s16, but the cast changes the object.
                s32 v = (s16)badgeShop_get(bdsw->starmaniac_table, (s16)(i + BADGE_MIN));
#else
                s32 v = badgeShop_get(bdsw->starmaniac_table, (s16)(i + BADGE_MIN));
#endif

                if (v - badgeShop_ThrowCheck(i + BADGE_MIN) > 0) {
                    select->items[select->numItems].id = i + BADGE_MIN;
                    if (itemDataTable[BADGE_MIN + i].star_piece_price > pouchGetStarPiece()) {
                        select->items[select->numItems].flags |= 3;
                    }
                    select->numItems++;
                }
            }
            qqsort(select->items, select->numItems, 4, winMgrSelectCmpStarPrice);
        }
        break;
    case 14:
        select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, 0x10);
        memset(select->items, 0, 0x10);
        select->numItems = 0;
        for (i = 0; i < 4; i++) {
            s32 v = evtGetValue(0, 0xF5DE01F6 + i);

            if (badge_bottakuru_table[v] != 0) {
                select->items[select->numItems++].id = badge_bottakuru_table[v];
            }
        }
        break;
    case 15:
        {
            s32 max = getBadgeBottakuru100TableMaxCount();

            select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, max * 4);
            memset(select->items, 0, max * 4);
            select->numItems = 0;
            for (i = 0; i < max; i++) {
                select->items[select->numItems++].id = badge_bottakuru100_table[i];
            }
        }
        break;
    case 16:
        {
            s32 max = getBadgeBteresaTableMaxCount();

            select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, max * 4);
            memset(select->items, 0, max * 4);
            select->numItems = 0;
            for (i = 0; i < 0x152; i++) {
                if (badgeShop_get(bdsw->bteresa_table, (s16)(i + 1)) > 0) {
                    select->items[select->numItems++].id = i + 1;
                }
            }
        }
        break;
    case 17:
    case 18:
        {
            u8* unk = (type == 0x11) ? jdt->unk14.unkD : jdt->unk14.unk1A;

            select->numItems = jdt->count;
            select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, (select->numItems + 1) * 4);
            memset(select->items, 0, (select->numItems + 1) * 4);
            for (i = 0; i < select->numItems; i++) {
                select->items[i].id = i;
                if (johoya_get(unk, jdt->info[i].storyId) != 0) {
                    select->items[i].flags |= 1;
                }
            }
        }
        break;
    default:
        if (kind < 0) {
            s32* list = (s32*)kind;
            s32 n = 0;

            while (list[n++] != -1)
                ;
            select->items = (WinMgrSelectItem*)__memAlloc(HEAP_DEFAULT, (n + 1) * 4);
            memset(select->items, 0, (n + 1) * 4);
            select->numItems = 0;
            if (firstItem != 0) {
                WinMgrSelectItem* it = &select->items[select->numItems++];

                it->id = firstItem;
            }
            for (i = 0; list[i] != -1; ) {
                select->items[select->numItems++].id = list[i++];
            }
        }
        break;
    }

    if (select->numItems < 8) {
        s32 f20 = desc->width;
        s32 f1C = get10(desc);
        s32 f18 = desc->x;
        s32 f24 = desc->height - (8 - select->numItems) * 0x18;
        WinMgrEntry* e = &wp->mEntries[select->descriptorId[0]];

        if (e->flags & 1) {
            e->x = f18;
            e->y = f1C;
            e->width = f20;
            e->height = f24;
        }
    }
    return select;
}

int winMgrSelectCmpStarPrice(const void* a, const void* b) {
    u32 va = itemDataTable[((const WinMgrSelectItem*)a)->id].star_piece_price;
    u32 vb = itemDataTable[((const WinMgrSelectItem*)b)->id].star_piece_price;

    if (va > vb) {
        return 1;
    }
    return va < vb ? -1 : 0;
}

int winMgrSelectCmpTypeSort(const void* a, const void* b) {
    s32 va = itemDataTable[((const WinMgrSelectItem*)a)->id].type_sort_order;
    s32 vb = itemDataTable[((const WinMgrSelectItem*)b)->id].type_sort_order;

    if (va > vb) {
        return 1;
    }
    return va < vb ? -1 : 0;
}

s32 winMgrSelect(WinMgrSelect* select) {
    u16 flags = select->flags;

    if (flags & 0x1000) {
        if (flags & 0x2000) {
            return -1;
        }
        if (select->firstItem != 0 && select->cursor == 0 && (flags & 0x100)) {
            return -1;
        }
        {
            WinMgrSelectItem* items = select->items;
            return items[select->cursor].id;
        }
    }
    return 0;
}

s32 winMgrSelectOther(WinMgrSelect* select, EventEntry* event) {
    u16 flags = select->flags;
    WinMgrSelectItem* items;
    InformantWork* jdt = &_jdt;
    s32 itemId;
    const char* msg;
    char buf[0x40];

    if (flags & 0x1000) {
        if (flags & 0x2000) {
            return -1;
        }

        items = select->items;
        itemId = items[select->cursor].id;

        switch (select->kind) {
        case 6:
        case 7:
            event->lwData[1] = party_id_table[itemId];
            switch (itemId) {
            case 0:
                msg = msgSearch(party_name_msg0);
                break;
            case 1:
                msg = msgSearch(party_name_msg1);
                break;
            case 2:
                msg = msgSearch(party_name_msg4);
                break;
            case 3:
                msg = pouchGetYoshiName();
                break;
            case 4:
                msg = msgSearch(party_name_msg5);
                break;
            case 5:
                msg = msgSearch(party_name_msg2);
                break;
            case 6:
                msg = msgSearch(party_name_msg6);
                break;
            }
            event->lwData[3] = (s32)msg;
            break;
        case 8:
            event->lwData[2] = party_id_table[itemId];
            switch (itemId) {
            case 0:
                msg = msgSearch(party_name_msg0);
                break;
            case 1:
                msg = msgSearch(party_name_msg1);
                break;
            case 2:
                msg = msgSearch(party_name_msg4);
                break;
            case 3:
                msg = pouchGetYoshiName();
                break;
            case 4:
                msg = msgSearch(party_name_msg5);
                break;
            case 5:
                msg = msgSearch(party_name_msg2);
                break;
            case 6:
                msg = msgSearch(party_name_msg6);
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
            event->lwData[3] = itemDataTable[itemId].sell_price;
            event->lwData[4] = select->cursor;
            break;
        case 13:
            event->lwData[1] = itemId;
            event->lwData[2] = (s32)itemDataTable[itemId].id;
            event->lwData[3] = itemDataTable[itemId].star_piece_price;
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
            sprintf((char*)jdt->infoEnd, "%s_%04d", jdt->prefix, jdt->info[itemId].storyId);
            sprintf(buf, "jperfect_title_%04d", jdt->info[itemId].storyId);
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
            sprintf((char*)jdt->infoEnd, "%s_%04d", jdt->prefix, jdt->info[itemId].storyId);
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
    // The original took the pointer as `void*` and cast it to WinMgrSelect*; that cast makes the
    // compiler copy the incoming r3 through r0 before the first call (asm: mr r0, r3 / mr r31, r0).
    // Keep the header's WinMgrSelect* signature and reproduce the copy with the same no-op cast.
    WinMgrSelect* s = (WinMgrSelect*)(void*)select;
    __memFree(HEAP_DEFAULT, s->items);
    __memFree(HEAP_DEFAULT, s);
}

s32 winMgrSelectCursor(WinMgrSelect* select) {
    return select->cursor;
}

// select_main emits first, then MWCC flushes the deferred group immediately after it.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

void select_main(WinMgrEntry* entry) {
    s32 count;
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    s32 newIndex;
    s32 off;
    s32 i;

    switch (select->state) {
    case 0:
        for (i = 0; i < (count = select->numDescriptors); i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            if (e->flags & 1) {
                e->state = 1;
                e->animStep = 0;
                e->scrollPos = 0;
                e->scrollRows = 0;
                e->message = 0;
            }
        }
        if (count >= 3) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[2]];

            e->scrollPos = 0;
            e->scrollRows = 0;
            e->message = 0;
        }
        psndSFXOn((const char*)0x2002A);
        statusWinForceOpen();
        select->state++;
        break;
    case 1:
        if (wp->mEntries[select->descriptorId[0]].flags & 4) {
            break;
        }
        newIndex = select->cursor;
        if (keyGetDirRep(0) & 0x2000) {
            select->cursor++;
        }
        if (keyGetDirRep(0) & 0x1000) {
            select->cursor--;
        }
        if (select->cursor >= select->numItems) {
            if (keyGetDirTrg(0) & 0x2000) {
                select->cursor = 0;
                select->scrollTop = 0;
            } else {
                select->cursor--;
            }
        }
        if (select->cursor < 0) {
            if (keyGetDirTrg(0) & 0x1000) {
                select->cursor = select->numItems - 1;
                off = select->numItems - 8;
                if (off > 0) {
                    select->scrollTop = off;
                }
            } else {
                select->cursor++;
            }
        }
        if (newIndex < select->cursor) {
            if (select->cursor > 5 && select->scrollTop < select->numItems - 8) {
                select->scrollTop++;
            }
        } else if (newIndex > select->cursor) {
            if (select->cursor - select->scrollTop < 3 && select->scrollTop > 0) {
                select->scrollTop--;
            }
        }
        if (newIndex != select->cursor) {
            psndSFXOn((const char*)0x20005);
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_A) {
            if (select->items[select->cursor].flags & 2) {
                psndSFXOn((const char*)0x20014);
            } else {
                psndSFXOn((const char*)0x20012);
                select->state++;
            }
        } else if (keyGetButtonTrg(0) & PAD_BUTTON_B) {
            if (select->flags & 0x100) {
                psndSFXOn((const char*)0x20013);
                select->flags |= 0x2000;
                select->state++;
            }
        }
        select->baseY += (WINMGR_F32(float_24_80421aa0) * select->scrollTop - select->baseY) / WINMGR_F32(float_6_80421aa4);
        select->iconX += ((f32)entry->x - select->iconX) / WINMGR_F32(float_6_80421aa4);
        select->iconY += ((f32)(entry->y - 54) - WINMGR_F32(float_24_80421aa0) * (f32)(select->cursor - select->scrollTop) -
                         select->iconY) / WINMGR_F32(float_6_80421aa4);
        break;
    case 2:
        for (i = 0; i < select->numDescriptors; i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            if (e->flags & 1) {
                e->state = 2;
                e->animStep = 0;
            }
        }
        psndSFXOn((const char*)0x2002B);
        statusWinForceOff();
        select->state++;
        break;
    case 3:
        if (wp->mEntries[select->descriptorId[0]].flags & 4) {
            break;
        }
        for (i = 0; i < select->numDescriptors; i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            e->flags &= ~1;
        }
        select->flags |= 0x1000;
        select->state++;
        break;
    case 4:
        break;
    }
}

s32 party_can_transfer_rank(EventEntry* event) {
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

s32 party_can_rank_up(EventEntry* event) {
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
