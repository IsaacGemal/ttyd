#include <dolphin/types.h>
#include <string.h>
#include <stddef.h>
#include "matching.h"
#include "window/win_log.h"
#include "window/win_root.h"
#include "win/win_main.h"
#include "battle/battle_monosiri.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "dolphin/gx.h"
#include "dolphin/mtx.h"
#include "math_ext.h"
#include "eff/eff_starstone.h"
#include "mario/mario_pouch.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "driver/swdrv.h"
#include "data/item_data.h"
#include "pmario_sound.h"
#include "mario/mario.h"
#include <stdio.h>
#include "manager/filemgr.h"
#include "mario/mariost.h"
#include "sdk/texPalette.h"
#include "system.h"
#include "memory.h"
#pragma pool_data on // the original addresses its .rodata through one base register

// The monosiri (tattle) viewer work struct. Not declared in any header yet;
// fields recovered from monosiriInit / monosiriMain / monosiriExit.
typedef struct MonosiriWork {
    s32 state;         // 0x00
    s32 unitKind;      // 0x04
    FileEntry* file1;  // 0x08
    FileEntry* file2;  // 0x0C
    f32 x;             // 0x10
    f32 y;             // 0x14
    f32 targetX;       // 0x18
    f32 targetY;       // 0x1C
    s32 poseId;        // 0x20
    s32 poseId2;       // 0x24
    void* alloc;       // 0x28
    s32 alpha;         // 0x2C
} MonosiriWork;


// win texture ids of the tattle-map location pins
typedef enum {
    WIN_LOG_TEX_PIN_OFF = 0xB8,
    WIN_LOG_TEX_PIN_SEL = 0xB9,
    WIN_LOG_TEX_PIN_ON = 0xBA,
} WinLogTex;

typedef struct WinLogEntry {
    f32 curX;             // 0xD78
    f32 curY;             // 0xD7C
    f32 targetX;          // 0xD80
    f32 targetY;          // 0xD84
    s32 state;            // 0xD88
    s32 timer;            // 0xD8C
    const char* name;     // 0xD90
    u32 field_0x1C;       // 0xD94
} WinLogEntry;
typedef struct WinLogWork {
    u8 pad_0[0xD6C];
    s32 sel;              // 0xD6C
    s32 count;            // 0xD70
    u8 pad_74[4];         // 0xD74
    WinLogEntry entries[1]; // 0xD78
} WinLogWork;
#define SFX(id) psndSFXOn((const char*)(id))
typedef struct TubuDt {
    s16 x;           // 0x0
    s16 y;           // 0x2
    u8 visible;      // 0x4
    u8 flagIdx;      // 0x5
    u16 minProgress; // 0x6
    const char* id;  // 0x8
} TubuDt;
#define LOGWIN_CHECK(field, off) typedef char logwin_##field##_check[(offsetof(LogWin, field) == (off)) ? 1 : -1]
extern TubuDt tubuDt[0x5D];
extern s32 stone_tbl[7];
TubuDt tubuDt[0x5D] = {
    { 14, -34, 0x01, 0x00, 0x0003, "gor" },
    { -4, -31, 0x00, 0x02, 0x000C, "gor" },
    { -20, -31, 0x00, 0x02, 0x000C, "gor" },
    { -33, -38, 0x01, 0x02, 0x000C, "tik" },
    { 118, 30, 0x01, 0x00, 0x0017, "hei" },
    { 134, 22, 0x00, 0x00, 0x001A, "hei" },
    { 150, 14, 0x00, 0x00, 0x001A, "hei" },
    { 176, 9, 0x00, 0x00, 0x001A, "hei" },
    { 200, 6, 0x01, 0x00, 0x001A, "nok" },
    { 136, 39, 0x00, 0x03, 0x0023, "hei" },
    { 152, 48, 0x00, 0x03, 0x0023, "hei" },
    { 165, 53, 0x00, 0x03, 0x0023, "hei" },
    { 165, 68, 0x01, 0x03, 0x0023, "gon" },
    { 0, 36, 0x01, 0x00, 0x0047, "win" },
    { 0, 56, 0x00, 0x00, 0x004A, "win" },
    { 0, 75, 0x01, 0x00, 0x004A, "mri" },
    { 16, 44, 0x00, 0x00, 0x0051, "win" },
    { 27, 53, 0x00, 0x00, 0x0051, "win" },
    { 42, 65, 0x01, 0x00, 0x0051, "hou" },
    { 30, -17, 0x00, 0x00, 0x007F, "gor" },
    { 36, 0, 0x00, 0x00, 0x007F, "gor" },
    { 46, 15, 0x00, 0x00, 0x007F, "gor" },
    { 55, 32, 0x00, 0x00, 0x007F, "gor" },
    { 74, 45, 0x00, 0x00, 0x007F, "gor" },
    { 83, 61, 0x00, 0x00, 0x007F, "gor" },
    { 83, 75, 0x00, 0x00, 0x007F, "gor" },
    { 83, 93, 0x00, 0x00, 0x007F, "gor" },
    { 72, 101, 0x00, 0x00, 0x007F, "gor" },
    { 71, 114, 0x00, 0x00, 0x007F, "gor" },
    { 71, 132, 0x00, 0x00, 0x007F, "gor" },
    { 80, 141, 0x00, 0x00, 0x007F, "gor" },
    { 97, 150, 0x01, 0x00, 0x007F, "tou" },
    { -181, -90, 0x01, 0x00, 0x00B2, "usu" },
    { -163, -90, 0x00, 0x04, 0x00B8, "usu" },
    { -145, -90, 0x00, 0x04, 0x00B8, "usu" },
    { -135, -77, 0x00, 0x04, 0x00B8, "usu" },
    { -143, -63, 0x00, 0x04, 0x00B8, "usu" },
    { -159, -63, 0x00, 0x04, 0x00B8, "usu" },
    { -176, -63, 0x00, 0x04, 0x00B8, "usu" },
    { -191, -63, 0x00, 0x04, 0x00B8, "usu" },
    { -206, -63, 0x00, 0x04, 0x00B8, "usu" },
    { -228, -63, 0x01, 0x04, 0x00B8, "gra" },
    { -228, -44, 0x00, 0x00, 0x00C1, "gra" },
    { -216, -31, 0x00, 0x00, 0x00C1, "gra" },
    { -198, -27, 0x00, 0x00, 0x00C1, "gra" },
    { -181, -29, 0x00, 0x00, 0x00C1, "gra" },
    { -160, -33, 0x00, 0x00, 0x00C1, "gra" },
    { -139, -33, 0x01, 0x00, 0x00C1, "jin" },
    { 13, -56, 0x00, 0x00, 0x00E8, "gor" },
    { 13, -70, 0x00, 0x00, 0x00E8, "gor" },
    { 12, -84, 0x00, 0x00, 0x00E8, "gor" },
    { -3, -94, 0x00, 0x00, 0x00E8, "gor" },
    { -14, -104, 0x00, 0x00, 0x00E8, "gor" },
    { -31, -115, 0x00, 0x00, 0x00E8, "gor" },
    { -45, -128, 0x00, 0x00, 0x00E8, "gor" },
    { -48, -143, 0x00, 0x00, 0x00E8, "gor" },
    { -38, -158, 0x00, 0x00, 0x00E8, "gor" },
    { -11, -163, 0x01, 0x00, 0x00E8, "muj" },
    { 7, -163, 0x00, 0x00, 0x00F3, "muj" },
    { 21, -163, 0x00, 0x00, 0x00F3, "muj" },
    { 41, -163, 0x01, 0x00, 0x00F3, "dou" },
    { 36, -34, 0x00, 0x00, 0x0136, "gor" },
    { 53, -27, 0x00, 0x00, 0x0136, "gor" },
    { 69, -12, 0x00, 0x00, 0x0136, "gor" },
    { 85, 1, 0x00, 0x00, 0x0136, "gor" },
    { 109, 7, 0x00, 0x00, 0x0136, "gor" },
    { 132, 4, 0x00, 0x00, 0x0136, "gor" },
    { 149, -8, 0x00, 0x00, 0x0136, "gor" },
    { 158, -25, 0x01, 0x00, 0x0136, "eki" },
    { 143, -40, 0x00, 0x00, 0x014E, "eki" },
    { 133, -52, 0x00, 0x00, 0x014E, "eki" },
    { 132, -66, 0x00, 0x00, 0x014E, "eki" },
    { 132, -82, 0x00, 0x00, 0x014E, "eki" },
    { 155, -90, 0x01, 0x00, 0x014E, "pik" },
    { 175, -90, 0x00, 0x00, 0x0150, "pik" },
    { 193, -91, 0x00, 0x00, 0x0150, "pik" },
    { 211, -91, 0x01, 0x00, 0x0150, "sin" },
    { -167, 17, 0x01, 0x00, 0x0161, "bom" },
    { -167, 37, 0x00, 0x00, 0x0166, "bom" },
    { -167, 50, 0x00, 0x00, 0x0166, "bom" },
    { -167, 64, 0x00, 0x00, 0x0166, "bom" },
    { -167, 82, 0x00, 0x00, 0x0166, "bom" },
    { -167, 99, 0x00, 0x00, 0x0166, "bom" },
    { -167, 116, 0x00, 0x00, 0x0166, "bom" },
    { -167, 133, 0x00, 0x00, 0x0166, "bom" },
    { -167, 151, 0x01, 0x00, 0x0166, "moo" },
    { -171, 169, 0x00, 0x00, 0x0169, "moo" },
    { -185, 177, 0x00, 0x00, 0x0169, "moo" },
    { -200, 171, 0x00, 0x00, 0x0169, "moo" },
    { -209, 159, 0x01, 0x00, 0x0169, "aji" },
    { -33, -50, 0x00, 0x00, 0x017E, "gor" },
    { -33, -62, 0x00, 0x00, 0x017E, "gor" },
    { -33, -74, 0x01, 0x00, 0x017E, "las" },};
s32 stone_tbl[7] = {0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78};
extern const u8 enemy_monoshiri_sort_table[];
#define LOG_ADD_TAB(w, tabType, msg)                                                  \
    do {                                                                              \
        LogRow* r_ = &(w)->rows[(w)->count];                                          \
        r_->type = (tabType);                                                         \
        r_->timer = 0;                                                                \
        r_->phase = 0;                                                                \
        r_->name = (msg);                                                             \
        (w)->count++;                                                                 \
        for (k_ = 0; k_ < (w)->count; k_++) {                                         \
            (w)->rows[k_].curX = 0.0f;                                    \
            (w)->rows[k_].targetX = 0.0f;                                 \
            {                                                                         \
                f32 a_ = 45.0f * winLogI2F((w)->count, base, &e7cScratch);   \
                f32 b_ = 45.0f * winLogI2F(k_, base, &macScratch);           \
                f32 y_ = a_ / 2.0f - b_;                                 \
                (w)->rows[k_].curY = y_;                                              \
                (w)->rows[k_].targetY = y_;                                           \
            }                                                                         \
        }                                                                             \
    } while (0)
#ifdef MUST_MATCH
// Match-only: the loop counter is a helper local, so the backend numbers it as a temp and can
// fold its zero into the one already materialised for the header stores (the original keeps the
// counter and that zero in r5). The msg is read through msg_help so the name pointer stays r0.
static inline void winLogAddTab0(LogWin* w, s32 tabType, const char** msg_help, s32 msgIdx) {
    LogRow* r = &w->rows[w->count];
    s32 k;
    r->type = tabType;
    r->timer = 0;
    r->phase = 0;
    r->name = msg_help[msgIdx];
    w->count++;
    for (k = 0; k < w->count; k++) {
        w->rows[k].curX = 0.0f;
        w->rows[k].targetX = 0.0f;
        {
            f32 a = 45.0f * ((f32)w->count);
            f32 b = 45.0f * ((f32)k);
            f32 y = a / 2.0f - b;
            w->rows[k].curY = y;
            w->rows[k].targetY = y;
        }
    }
}
#define LOG_ADD_TAB_0(w, tabType, msg) winLogAddTab0(w, tabType, msg_help, 0)
#define LOG_ADD_1(w, tabType, msg) winLogAddTab0(w, tabType, msg_help, 1)
#define LOG_ADD_2(w, tabType, msg) winLogAddTab0(w, tabType, msg_help, 2)
#define LOG_ADD_3(w, tabType, msg) winLogAddTab0(w, tabType, msg_help, 3)
#define LOG_ADD_4(w, tabType, msg) winLogAddTab0(w, tabType, msg_help, 4)
#else
#define LOG_ADD_TAB_0(w, tabType, msg) LOG_ADD_TAB(w, tabType, msg)
#define LOG_ADD_1(w, tabType, msg) LOG_ADD_TAB(w, tabType, msg)
#define LOG_ADD_2(w, tabType, msg) LOG_ADD_TAB(w, tabType, msg)
#define LOG_ADD_3(w, tabType, msg) LOG_ADD_TAB(w, tabType, msg)
#define LOG_ADD_4(w, tabType, msg) LOG_ADD_TAB(w, tabType, msg)
#endif
#define LOG_TEXPAL() (*(TEXPalette**)work->winTpl->data)
extern const GXColor unk_80423050;
#define HALF_FOV_TAN 0.21816615760326385
typedef struct TattleCamEntry {
    s32 kind;
    f32 x;
    f32 y;
    f32 scale;
} TattleCamEntry;

// Appends a tab to the menu and re-centres every tab row.


#define winLogI2F(v, base, scratch) ((f32)(v))

void winLogInit(WindowWork* work) {
    LogWin* w = (LogWin*)work;
    static const char* msg_help[5] = {"msg_menu_kiroku_map", "msg_menu_kiroku_stone", "msg_menu_kiroku_badge", "msg_menu_kiroku_ryori", "msg_menu_kiroku_mono"};
    const char* area;
    s32 i;
    s32 k_;
    f64 e7cScratch;
    f64 macScratch;

    ((LogWin*)work)->state = 0;
    w->sel = 0;
    w->count = 0;
    LOG_ADD_TAB_0(w, 0, msg_help[0]);
    if (swByteGet(0) >= 0x13) {
        LOG_ADD_1(w, 1, msg_help[1]);
    }
    if (swByteGet(0) >= 7) {
        LOG_ADD_4(w, 4, msg_help[4]);
    }
    if (swByteGet(0) >= 0x14) {
        LOG_ADD_2(w, 2, msg_help[2]);
    }
    if (swGet(0x40) != 0) {
        LOG_ADD_3(w, 3, msg_help[3]);
    }
    w->stoneNum = 0;
    for (i = 0; i < 7; i++) {
        if (pouchCheckItem(stone_tbl[i]) != 0) {
            w->stoneNum++;
        }
    }
    for (i = 0; i < 7; i++) {
        EffectEntry* e;

        if (i < w->stoneNum) {
            e = w->stones[i] =
                effStarStoneEntry(i, 0.0f, -1000.f, 0.0f, 2.0f);
        } else {
            e = w->stones[i] = effStarStoneEntry(i + 7, 0.0f, -1000.f, 0.0f,
                                                 2.0f);
            ((effStarStoneData*)e->userdata)->tevReg0R = 0x20;
            ((effStarStoneData*)e->userdata)->tevReg0G = 0x20;
            ((effStarStoneData*)e->userdata)->tevReg0B = 0x20;
        }
        ((effStarStoneData*)e->userdata)->camId = 8;
        ((effStarStoneData*)e->userdata)->flags &= ~4;
        ((effStarStoneData*)e->userdata)->flags |= 0x20;
    }
    w->mapTpl = NULL;
    w->mapMarioPose = -1;
    w->mapSel = -1;
    w->mapPick = 0;
    w->mapCy = 0.0f;
    w->mapCx = 0.0f;
    w->e2c = 0.0f;
    w->e28 = 0.0f;
    w->e34 = -480.f;
    w->e30 = -480.f;
    w->mapScrollY = 0.0f;
    w->mapScrollX = 0.0f;
    area = gp->currentAreaName;
    if (strcmp(gp->currentMapName, "pik_02") == 0
        || strcmp(gp->currentMapName, "pik_03") == 0) {
        area = "sin";
    }
    if (strcmp(gp->currentMapName, "win_04") == 0
        || strcmp(gp->currentMapName, "win_05") == 0) {
        area = "hou";
    }
    if (strncmp(gp->currentMapName, "rsh", 3) == 0) {
        if (swByteGet(0) < 0x136) {
            area = "gor";
        } else if (swByteGet(0) < 0x14E || swGet(0xDFB) != 0) {
            area = "eki";
        } else if (swGet(0xDFC) != 0) {
            area = "pik";
        } else {
            switch (swByteGet(0x366)) {
            case 0:
                area = "gor";
                break;
            case 1:
                area = "eki";
                break;
            case 2:
                area = "pik";
                break;
            case 3:
                area = "gor";
                break;
            }
        }
    }
    if (strncmp(gp->currentMapName, "hom", 3) == 0) {
        area = "eki";
    }
    if (strncmp(gp->currentMapName, "jon", 3) == 0) {
        area = "tik";
    }
    if (strcmp(gp->currentMapName, "gra_06") == 0) {
        area = "jin";
    }
    if (strcmp(gp->currentMapName, "muj_10") == 0
        || strcmp(gp->currentMapName, "muj_11") == 0
        || strcmp(gp->currentMapName, "muj_12") == 0) {
        area = "dou";
    }
    for (i = 0; (u32)i < 0x5D; i++) {
        if (area == NULL || strcmp(area, tubuDt[i].id) == 0) {
            break;
        }
    }
    if ((u32)i < 0x5D) {
        w->mapPick = i;
    }
    w->mapCx = 304.0f + (f32)tubuDt[w->mapPick].x;
    w->mapCy = 240.f + (f32)tubuDt[w->mapPick].y;
    {
        s32 ox = (s32)((w->mapCx + w->mapScrollX) - 230.f);
        s32 oy = (s32)((w->mapCy + w->mapScrollY) - 120.f);

        w->mapScrollX -= (f32)ox;
        w->mapScrollY -= (f32)oy;
    }
    if (w->mapScrollX > 0.0f) {
        w->mapScrollX = 0.0f;
    }
    if (w->mapScrollY > 0.0f) {
        w->mapScrollY = 0.0f;
    }
    if (w->mapScrollX < -148.0f) {
        w->mapScrollX = -148.0f;
    }
    if (w->mapScrollY < -240.f) {
        w->mapScrollY = -240.f;
    }
    w->stoneSel = 0;
    {
        f32 e7c = 51.42857f * winLogI2F(w->stoneSel, base, &e7cScratch);
        w->e7c = e7c;
        w->e80 = e7c;
    }
    w->e68 = 0.0f;
    w->e64 = 0.0f;
    w->e70 = -480.f;
    w->e6c = -480.f;
    w->e78 = 0.0f;
    w->e74 = 0.0f;
    w->cardCnt = 0;
    for (i = 0xF0; i < 0xF0 + 0x63; i++) {
        if (itemDataTable[i].type_sort_order != -1) {
            w->cardIcon[w->cardCnt++] = i - 0xF0;
        }
    }
    qqsort(w->cardIcon, w->cardCnt, 2, compare_func5_2);
    w->cardSel = 0;
    w->e88 = 320.f;
    w->e84 = 320.f;
    w->e90 = -240.f;
    w->e8c = -240.f;
    w->cardPage = 0;
    w->ea4 = 0.0f;
    w->ea0 = 0.0f;
    w->eac = -200.f;
    w->ea8 = -200.f;
    w->eb4 = -300.f;
    w->eb0 = -300.f;
    w->badgeCnt = 0;
    for (i = 0xB3; i < 0xB3 + 0x39; i++) {
        if (itemDataTable[i].type_sort_order != -1) {
            w->badgeIcon[w->badgeCnt++] = i - 0xB3;
        }
    }
    qqsort(w->badgeIcon, w->badgeCnt, 2, compare_func6_2);
    w->badgeSel = 0;
    w->f84 = 320.f;
    w->f80 = 320.f;
    w->f8c = -240.f;
    w->f88 = -240.f;
    w->badgePage = 0;
    w->fa0 = 0.0f;
    w->f9c = 0.0f;
    w->fa8 = -200.f;
    w->fa4 = -200.f;
    w->fb0 = -300.f;
    w->fac = -300.f;
    w->charCnt = 0;
    for (i = 1; i < 0xD8; i++) {
        if (battleGetUnitMonosiriPtr(i)->tattleLogTag != NULL) {
            if (evtGetValue(NULL, GSW(0)) < 0x187) {
                if ((u32)(i - 0x85) <= 1 || i == 0x87) {
                    continue;
                }
            } else {
                if ((u32)(i - 0x1F) <= 1 || i == 0x4D) {
                    continue;
                }
            }
            if (evtGetValue(NULL, GSW(0)) < 0x190) {
                if (i == 0x90) {
                    continue;
                }
            } else {
                if (i == 0x3F) {
                    continue;
                }
            }
            w->chars[w->charCnt].pad = enemy_monoshiri_sort_table[i];
            w->chars[w->charCnt++].id = i;
        }
    }
    {
        s32 a;
        s32 b;

        for (a = 0; a < w->charCnt - 1; a++) {
            for (b = a + 1; b < w->charCnt; b++) {
                if (w->chars[a].pad > w->chars[b].pad) {
                    LogChar t = w->chars[a];
                    w->chars[a] = w->chars[b];
                    w->chars[b] = t;
                }
            }
        }
    }
    w->charSel = 0;
    w->x102c = 0.0f;
    w->x1028 = 0.0f;
    w->x1034 = 0.0f;
    w->x1030 = 0.0f;
    w->charPage = 0;
    w->x104c = 0.0f;
    w->x1048 = 0.0f;
    w->seenCnt = 0;
    for (i = 1; i < 0xD8; i++) {
        if (battleGetUnitMonosiriPtr(i)->tattleLogTag != NULL && (u32)(i - 0x1F) > 1 && i != 0x4D && i != 0x3F
            && swGet(i + 0x117A) != 0) {
            w->seenCnt++;
        }
    }
}

// 100%: the 45.0f * (f32)a product is held in a named temp so MWCC colors the
// long-lived int->float conversion double f7 (as the original does) instead of
// f6; the entry address is formed with real WinLogEntry pointer indexing, which
// makes the compiler emit `add rWork, rIndex` in the original's operand order.
// The conversion double still pools as an anonymous @271 literal instead of the
// named 4503601774854144.0 (project-wide limitation, not scored).
void winLogInit2(WindowWork* work) {
    LogWin* w = (LogWin*)work;
    f32 v;
    const char* name;
    s32 a;
    s32 b;

    work->targetX = -128.0f;
    a = w->count;
    b = w->sel;
    {
        f32 t = 45.0f * (f32)a;
        work->targetY = 8.0f + (t / 2.0f - 45.0f * (f32)b);
    }
    w->e88 = 320.f;
    w->e84 = 320.f;
    w->e90 = -240.f;
    w->e8c = -240.f;
    v = w->cardPage * 0xE8;
    w->ea4 = v;
    w->ea0 = v;
    w->eac = -200.f;
    w->ea8 = -200.f;
    w->eb4 = -300.f;
    w->eb0 = -300.f;
    w->f84 = 320.f;
    w->f80 = 320.f;
    w->f8c = -240.f;
    w->f88 = -240.f;
    v = w->badgePage * 0xE8;
    w->fa0 = v;
    w->f9c = v;
    w->fa8 = -200.f;
    w->fa4 = -200.f;
    w->fb0 = -300.f;
    w->fac = -300.f;
    name = w->rows[w->sel].name;
    winMsgEntry(work, 0, name, NULL);
}

void winLogExit(WindowWork* work) {
    s32 i;

    if (work->mapTpl != NULL) {
        fileFree(work->mapTpl);
        work->mapTpl = NULL;
    }
    if (work->mapMarioPose != -1) {
        animPoseRelease(work->mapMarioPose);
        work->mapMarioPose = -1;
    }
    for (i = 0; i < 7; i++) {
        effSoftDelete(((LogWin*)work)->stones[i]);
    }
}

// winGetMapTplName as the compiler inlined it into winLogMain, sharing the caller's string base register.



// The fields of WindowWork that winLogMain touches (win_main.h only declares the first 0x1210 bytes).

LOGWIN_CHECK(rows, 0xD74);
LOGWIN_CHECK(mapSel, 0xE14);
LOGWIN_CHECK(f84, 0xF84);
LOGWIN_CHECK(badgeIcon, 0xFB4);
LOGWIN_CHECK(charCnt, 0x1038);
LOGWIN_CHECK(chars, 0x1050);
LOGWIN_CHECK(mono, 0x1200);



// The map table: one entry per "tubu" map region. The id field points into
// the .sdata2 region-name pool; entry order is the original's address order.


static inline BOOL tubuUnlocked(TubuDt* t) {
    switch (t->flagIdx) {
    case 0:
        return FALSE;
    case 1:
        if (swGet(0xFD) != 0) {
            return TRUE;
        }
        break;
    case 2:
        if (swGet(0xFE) != 0) {
            return TRUE;
        }
        break;
    case 3:
        if (swGet(0xFF) != 0) {
            return TRUE;
        }
        break;
    case 4:
        if (swGet(0x100) != 0) {
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// Tattle log / menu main: the tabbed "kiroku" window (map, crystal stars, cards, tattle log).
s32 winLogMain(WindowWork* work) {
    LogWin* w = (LogWin*)work;
    const char* tpl;
    const char* root;
    u8* p;
    s32 i;
    u32 j;

    {
        switch (w->state) {
        case 0:
            if (w->buttonTrg & 0x200) {
                SFX(0x20013);
                return -1;
            }
            if (w->buttonTrg & 0x1000) {
                return -2;
            }
            if (w->dirRep & 0x2000) {
                s32 old = w->sel;
                w->sel = old + 1;
                if (w->sel >= w->count) {
                    w->sel = 0;
                }
                if (old != w->sel) {
                    SFX(0x20005);
                }
            } else if (w->dirRep & 0x1000) {
                s32 old = w->sel;
                w->sel = old - 1;
                if (w->sel < 0) {
                    w->sel = w->count - 1;
                }
                if (old != w->sel) {
                    SFX(0x20005);
                }
            } else if (w->buttonTrg & 0x100) {
                {
                    p = (u8*)work;
                    i = 0;
                    while (i < w->count) {
                        *(s32*)(p + offsetof(LogWin, rows) + offsetof(LogRow, phase)) = 0x1E;
                        p += 0x20;
                        i++;
                    }
                }
                w->rows[w->sel].phase = 0xA;
                w->state = w->rows[w->sel].type + 0xA;
                switch (w->rows[w->sel].type) {
                case 4:
                    break;
                case 0:
                    w->e30 = 20.f;
                    break;
                case 1:
                    w->e6c = 20.f;
                    w->e74 = 255.0f;
                    break;
                case 2:
                    w->e88 = -130.f;
                    w->e90 = 120.f;
                    w->eac = -200.f;
                    w->eb4 = 60.f;
                    break;
                case 3:
                    w->f84 = -130.f;
                    w->f8c = 120.f;
                    w->fa8 = -200.f;
                    w->fb0 = 60.f;
                    break;
                }
                SFX(0x20012);
            }
            w->targetX = -128.0f;
            {
                f32 t = 45.0f * (f32)w->count;
                w->targetY =
                    8.0f + (t / 2.0f - 45.0f * (f32)w->sel);
            }
            winMsgEntry(work, 0, w->rows[w->sel].name, NULL);
            break;
        case 10:
            tpl = winGetMapTplName();
            root = getMarioStDvdRoot();
            if (fileAsyncf(FILE_TYPE_TPL, NULL, "%s/w/jp/%s", root, tpl) != 0) {
                if (w->mapTpl == 0) {
                    tpl = winGetMapTplName();
                    root = getMarioStDvdRoot();
                    // (s32) add defeats the CSE with the fileAsyncf string above, so MWCC recomputes it
                    // into r4 (as the original) instead of pinning it in r28 across the call.
                    w->mapTpl = (FileEntry*)fileAllocf(FILE_TYPE_TPL, "%s/w/jp/%s", root, tpl);
                }
                if (w->mapMarioPose == -1) {
                    if (w->flags & 0x2000) {
                        const char* anim = "W_1";
                        w->mapMarioPose = animPoseEntry("d_mario", 0);
                        switch (marioGetColor()) {
                        case 0:
                            anim = "W_1";
                            break;
                        case 1:
                            anim = "W_15";
                            break;
                        case 2:
                            anim = "W_17";
                            break;
                        case 3:
                            anim = "W_18";
                            break;
                        }
                        animPoseSetAnim(w->mapMarioPose, anim, 1);
                    } else {
                        const char* model = "a_mario";
                        switch (marioGetColor()) {
                        case 0:
                            model = "a_mario";
                            break;
                        case 1:
                            model = "a_mario_l";
                            break;
                        case 2:
                            model = "a_mario_w";
                            break;
                        case 3:
                            model = "a_mario_wl";
                            break;
                        }
                        w->mapMarioPose = animPoseEntry(model, 0);
                        animPoseSetAnim(w->mapMarioPose, "M_W_1", 1);
                    }
                    animPoseSetMaterialFlagOn(w->mapMarioPose, 0x1800);
                }
                if (w->buttonTrg & 0x200) {
                    SFX(0x20013);
                    {
                        p = (u8*)work;
                        i = 0;
                        while (i < w->count) {
                            *(s32*)(p + offsetof(LogWin, rows) + offsetof(LogRow, phase)) = 0x28;
                            p += 0x20;
                            i++;
                        }
                    }
                    w->rows[w->sel].phase = 0x14;
                    w->state = 0;
                    w->e30 = -480.f;
                    fileFree((FileEntry*)w->mapTpl);
                    w->mapTpl = 0;
                    if (w->mapMarioPose != -1) {
                        animPoseRelease(w->mapMarioPose);
                    }
                    w->mapMarioPose = -1;
                } else if (w->buttonTrg & 0x1000) {
                    return -2;
                }
                {
                    f32 best = 10000.f;
                    f32 px = -230.f + w->mapScrollX + w->mapCx;
                    f32 py = -120.f + w->mapScrollY + w->mapCy;
                    f32 qx = 74.0f + w->mapScrollX;
                    f32 qy = 120.f + w->mapScrollY;
                    TubuDt* t = tubuDt;
                    s32 bestIdx;
                    f32 sx, sy;
                    s32 ox, oy;

                    for (j = 0; j < 0x5D; j++) {
                        t = tubuDt + j;
                        if (swByteGet(0) < (s32)t->minProgress) {
                            if (!tubuUnlocked(t)) {
                                continue;
                            }
                        }
                        if (t->visible != 0) {
                            f32 d = distABf(qx + (f32)t->x, qy + (f32)t->y, px, py);
                            if (best > d) {
                                best = d;
                                bestIdx = j;
                            }
                        }
                    }
                    if (best < 50.f) {
                        static char name[0x40];
                        static char help[0x40];
                        sprintf(name, "menu_monosiri_%s", tubuDt[bestIdx].id);
                        sprintf(help, "menu_map_%s", tubuDt[bestIdx].id);
                        if (w->mapSel != bestIdx) {
                            SFX(0x20035);
                        }
                        w->mapSel = bestIdx;
                        winMsgEntry(work, 0, help, name);
                    } else {
                        w->mapSel = -1;
                        winMsgEntry(work, 0, "msg_menu_kiroku_map", NULL);
                    }
                    sx = (f32)keyGetStickX(0) / 8.0f;
                    sy = (f32)keyGetStickY(0) / 8.0f;
                    if (0.0f == sx && 0.0f == sy) {
                        if (keyGetButton(0) & 1) {
                            sx = -8.0f;
                        } else if (keyGetButton(0) & 2) {
                            sx = 8.0f;
                        }
                        if (keyGetButton(0) & 8) {
                            sy = 8.0f;
                        } else if (keyGetButton(0) & 4) {
                            sy = -8.0f;
                        }
                    }
                    if (0.0f == sx && 0.0f == sy) {
                        s32 sel = w->mapSel;
                        if (sel != -1) {
                            sx = (304.0f + (f32)tubuDt[sel].x) - w->mapCx;
                            sy = (240.f + (f32)tubuDt[sel].y) - w->mapCy;
                            sx *= 0.2f;
                            sy *= 0.2f;
                        }
                    }
                    w->mapCx = w->mapCx + sx;
                    w->mapCy = w->mapCy + sy;
                    if (w->mapCx < 16.0f) {
                        w->mapCx = 16.0f;
                    }
                    if (w->mapCy < 8.0f) {
                        w->mapCy = 8.0f;
                    }
                    if (w->mapCx >= 604.0f) {
                        w->mapCx = 604.0f;
                    }
                    if (w->mapCy >= 468.0f) {
                        w->mapCy = 468.0f;
                    }
                    ox = (s32)((w->mapCx + w->mapScrollX) - 230.f);
                    oy = (s32)((w->mapCy + w->mapScrollY) - 120.f);
                    if ((f32)ox >= 166.0f) {
                        w->mapScrollX = w->mapScrollX - ((f32)ox - 166.0f);
                    }
                    if ((f32)ox <= -150.f) {
                        w->mapScrollX = w->mapScrollX - (150.f + (f32)ox);
                    }
                    if ((f32)oy >= 80.f) {
                        w->mapScrollY = w->mapScrollY - ((f32)oy - 80.f);
                    }
                    if ((f32)oy <= -80.f) {
                        w->mapScrollY = w->mapScrollY - (80.f + (f32)oy);
                    }
                    if (w->mapScrollX > 0.0f) {
                        w->mapScrollX = 0.0f;
                    }
                    if (w->mapScrollY > 0.0f) {
                        w->mapScrollY = 0.0f;
                    }
                    if (w->mapScrollX < -148.0f) {
                        w->mapScrollX = -148.0f;
                    }
                    if (w->mapScrollY < -240.f) {
                        w->mapScrollY = -240.f;
                    }
                    w->targetX = -230.f + w->mapScrollX + w->mapCx;
                    w->targetY = -120.f + w->mapScrollY + w->mapCy;
                    w->targetX -= 24.0f;
                    w->targetY += 20.f;
                }
            }
            break;
        case 11:
            if (w->buttonTrg & 0x200) {
                SFX(0x20013);
                {
                    p = (u8*)work;
                    i = 0;
                    while (i < w->count) {
                        *(s32*)(p + offsetof(LogWin, rows) + offsetof(LogRow, phase)) = 0x28;
                        p += 0x20;
                        i++;
                    }
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->e6c = -480.f;
                w->e74 = 0.0f;
            } else if (w->dirRep & 0x4000) {
                w->stoneSel = w->stoneSel - 1;
                while (w->stoneSel < 0) {
                    w->stoneSel = w->stoneSel + 7;
                }
                w->e7c = 51.42857f * (f32)w->stoneSel;
                while (w->e80 < w->e7c) {
                    w->e80 += 360.f;
                }
                SFX(0x209D6);
                SFX(0x20037);
            } else if (w->dirRep & 0x8000) {
                w->stoneSel = w->stoneSel + 1;
                while (w->stoneSel > 6) {
                    w->stoneSel = w->stoneSel - 7;
                }
                w->e7c = 51.42857f * (f32)w->stoneSel;
                while (w->e80 > w->e7c) {
                    w->e80 -= 360.f;
                }
                SFX(0x209D6);
                SFX(0x20037);
            } else if (w->buttonTrg & 0x1000) {
                return -2;
            }
            w->targetX = -50.f;
            w->targetY = -50.f;
            if (w->stoneSel < w->stoneNum) {
                ItemData* it = &itemDataTable[stone_tbl[w->stoneSel]];
                winMsgEntry(work, -1, it->description, it->name);
            } else {
                winMsgEntry(work, -1, "msg_menu_stone_none_help", "msg_menu_stone_none_name");
            }
            break;
        case 12: {
            s32 cnt = w->cardCnt;
            u32 trg = w->buttonTrg;
            s32 pages = (cnt + 0x1B) / 28;
            if (trg & 0x200) {
                SFX(0x20013);
                {
                    p = (u8*)work;
                    i = 0;
                    while (i < w->count) {
                        *(s32*)(p + offsetof(LogWin, rows) + offsetof(LogRow, phase)) = 0x28;
                        p += 0x20;
                        i++;
                    }
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->e88 = 330.f;
                w->e90 = -250.f;
                w->eac = -200.f;
                w->eb4 = -300.f;
            } else if (w->buttonRep & 0x40) {
                w->cardPage = w->cardPage - 1;
                if (w->cardPage < 0) {
                    w->cardPage = 0;
                }
                w->cardSel = w->cardPage * 0x1C;
                SFX(0x20035);
            } else if (w->buttonRep & 0x20) {
                w->cardPage = w->cardPage + 1;
                if (w->cardPage >= pages) {
                    w->cardPage = pages - 1;
                }
                w->cardSel = w->cardPage * 0x1C;
                SFX(0x20035);
            } else if (w->dirRep & 0x8000) {
                if (w->cardSel % 7 < 6) {
                    s32 v = w->cardSel + 1;
                    if (v < cnt) {
                        w->cardSel = v;
                    }
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x4000) {
                if (w->cardSel % 7 > 0) {
                    w->cardSel = w->cardSel - 1;
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x1000) {
                s32 cur = w->cardSel;
                s32 v = cur - 7;
                if (v >= 0 && v < cnt) {
                    w->cardSel = cur - 7;
                }
                w->cardPage = w->cardSel / 28;
                SFX(0x20035);
            } else if (w->dirRep & 0x2000) {
                s32 cur = w->cardSel;
                s32 v = cur + 7;
                if (v >= 0 && v < cnt) {
                    w->cardSel = cur + 7;
                } else if (cur + (7 - w->cardSel % 7) < cnt) {
                    w->cardSel = cnt - 1;
                }
                w->cardPage = w->cardSel / 28;
                SFX(0x20035);
            } else if (trg & 0x10) {
                winSortEntry(work, 4, -310.f, 150.f);
                w->savedState = w->state;
                SFX(0x20012);
                w->state = 0x3E8;
            } else if (trg & 0x1000) {
                return -2;
            }
            w->targetX = (f32)(((w->cardSel % 28) % 7) * 0x38 - 0x8C);
            w->targetY = (f32)(0x5A - ((w->cardSel % 28) / 7) * 0x38);
            if (swGet(w->cardIcon[w->cardSel] + 0x80) != 0) {
                ItemData* it = &itemDataTable[w->cardIcon[w->cardSel]];
                winMsgEntry(work, -2, (it + 240)->menu_description, (it + 240)->name);
            } else {
                winMsgEntry(work, -2, "msg_menu_stone_none_help", "msg_menu_stone_none_name");
            }
            break;
        }
        case 13: {
            s32 cnt = w->badgeCnt;
            u32 trg = w->buttonTrg;
            s32 pages = (cnt + 0x1B) / 28;
            if (trg & 0x200) {
                SFX(0x20013);
                {
                    p = (u8*)work;
                    i = 0;
                    while (i < w->count) {
                        *(s32*)(p + offsetof(LogWin, rows) + offsetof(LogRow, phase)) = 0x28;
                        p += 0x20;
                        i++;
                    }
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->f84 = 330.f;
                w->f8c = -250.f;
                w->fa8 = -200.f;
                w->fb0 = -300.f;
            } else if (w->buttonRep & 0x40) {
                w->badgePage = w->badgePage - 1;
                if (w->badgePage < 0) {
                    w->badgePage = 0;
                }
                w->badgeSel = w->badgePage * 0x1C;
                SFX(0x20035);
            } else if (w->buttonRep & 0x20) {
                w->badgePage = w->badgePage + 1;
                if (w->badgePage >= pages) {
                    w->badgePage = pages - 1;
                }
                w->badgeSel = w->badgePage * 0x1C;
                SFX(0x20035);
            } else if (w->dirRep & 0x8000) {
                if (w->badgeSel % 7 < 6) {
                    s32 v = w->badgeSel + 1;
                    if (v < cnt) {
                        w->badgeSel = v;
                    }
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x4000) {
                if (w->badgeSel % 7 > 0) {
                    w->badgeSel = w->badgeSel - 1;
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x1000) {
                s32 cur = w->badgeSel;
                s32 v = cur - 7;
                if (v >= 0 && v < cnt) {
                    w->badgeSel = cur - 7;
                }
                w->badgePage = w->badgeSel / 28;
                SFX(0x20035);
            } else if (w->dirRep & 0x2000) {
                s32 cur = w->badgeSel;
                s32 v = cur + 7;
                if (v >= 0 && v < cnt) {
                    w->badgeSel = cur + 7;
                } else if (cur + (7 - w->badgeSel % 7) < cnt) {
                    w->badgeSel = cnt - 1;
                }
                w->badgePage = w->badgeSel / 28;
                SFX(0x20035);
            } else if (trg & 0x10) {
                winSortEntry(work, 5, -310.f, 150.f);
                w->savedState = w->state;
                SFX(0x20012);
                w->state = 0x3E8;
            } else if (trg & 0x1000) {
                return -2;
            }
            w->targetX = (f32)(((w->badgeSel % 28) % 7) * 0x38 - 0x8C);
            w->targetY = (f32)(0x5A - ((w->badgeSel % 28) / 7) * 0x38);
            if (swGet(w->badgeIcon[w->badgeSel] + 0x41) != 0) {
                ItemData* it = &itemDataTable[w->badgeIcon[w->badgeSel]];
                winMsgEntry(work, -2, (it + 179)->menu_description, (it + 179)->name);
            } else {
                winMsgEntry(work, -2, "msg_menu_stone_none_help", "msg_menu_stone_none_name");
            }
            break;
        }
        case 0xE: {
            u32 trg = w->buttonTrg;
            s32 pages = (w->charCnt + 0xF) / 16;
            if (trg & 0x200) {
                SFX(0x20013);
                {
                    p = (u8*)work;
                    i = 0;
                    while (i < w->count) {
                        *(s32*)(p + offsetof(LogWin, rows) + offsetof(LogRow, phase)) = 0x28;
                        p += 0x20;
                        i++;
                    }
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->x102c = 320.f;
                w->x1034 = -240.f;
            } else if (w->buttonRep & 0x40) {
                w->charPage = w->charPage - 1;
                if (w->charPage < 0) {
                    w->charPage = 0;
                }
                w->charSel = w->charPage * 0x10;
                SFX(0x20035);
            } else if (w->buttonRep & 0x20) {
                w->charPage = w->charPage + 1;
                if (w->charPage >= pages) {
                    w->charPage = pages - 1;
                }
                w->charSel = w->charPage * 0x10;
                SFX(0x20035);
            } else if (w->dirRep & 0xC000) {
                s32 cur = w->charSel;
                if (cur % 16 < 8) {
                    w->charSel = cur + 8;
                } else {
                    w->charSel = cur - 8;
                }
                if (w->charSel >= w->charCnt) {
                    w->charSel = w->charCnt - 1;
                }
                if (w->charSel < 0) {
                    w->charSel = 0;
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x1000) {
                s32 cur = w->charSel;
                if (cur % 8 == 0) {
                    w->charSel = cur - 9;
                } else {
                    w->charSel = cur - 1;
                }
                if (w->charSel < 0) {
                    w->charSel = 0;
                }
                w->charPage = w->charSel / 16;
                SFX(0x20035);
            } else if (w->dirRep & 0x2000) {
                s32 cur = w->charSel;
                if (cur % 8 == 7) {
                    w->charSel = cur + 9;
                } else {
                    w->charSel = cur + 1;
                }
                if (w->charSel >= w->charCnt) {
                    w->charSel = w->charCnt - 1;
                }
                w->charPage = w->charSel / 16;
                SFX(0x20035);
            } else if (trg & 0x100) {
                if (swGet(w->chars[w->charSel].id + 0x117A) != 0) {
                    SFX(0x20012);
                    w->state = 0x15;
                    w->mono = monosiriInit(w->chars[w->charSel].id, -60.f, 150.f);
                }
            } else if (trg & 0x10) {
                winSortEntry(work, 6, -310.f, 150.f);
                w->savedState = w->state;
                SFX(0x20012);
                w->state = 0x3E9;
            } else if (trg & 0x1000) {
                return -2;
            }
            w->targetX = (f32)((w->charSel % 16 / 8) * 0xD8 - 0xB4);
            w->targetY = (f32)(0x6A - (w->charSel % 16 % 8) * 0x18);
            if (swGet(w->chars[w->charSel].id + 0x117A) != 0) {
                winMsgEntry(work, 0,
                            battleGetUnitMonosiriPtr(w->chars[w->charSel].id)->tattleLogTag, NULL);
            } else {
                winMsgEntry(work, 0, "msg_menu_monosiri_enemy", NULL);
            }
    
            break;
        }
        case 21:
            if (w->buttonTrg & 0x200) {
                SFX(0x20013);
                w->state = 0x16;
                monosiriClose(w->mono);
            } else if (w->buttonTrg & 0x1000) {
                monosiriExit(w->mono);
                return -2;
            } else {
                monosiriMain(w->mono);
            }
            break;
        case 22:
            if (monosiriCloseWait(w->mono)) {
                w->state = 0xE;
                monosiriExit(w->mono);
            } else {
                monosiriMain(w->mono);
            }
            break;
        case 0x3E8:
        case 0x3E9:
            if (winSortWait(work) == 0) {
                w->state = w->savedState;
            }
            if (w->buttonTrg & 0x1000) {
                return -2;
            }
            break;
        }
    }
    return 0;
}


// A single tattle-log row inside WindowWork, at 0xD78 with stride 0x20.


// Advances the tattle-log rows (state machine) and eases the panel
// springs. The per-block `dN`
// temps give the diff-first fmadd operand order; `ep` steps the entry base so
// the prologue copy lands first.
void winLogMain2(WindowWork* work) {
    WinLogWork* w = (WinLogWork*)work;
    u8* ep = (u8*)work;
    LogWin* lw = (LogWin*)work;
    s32 i;

    for (i = 0; i < w->count; i++, ep += 0x20) {
        WinLogEntry* e = (WinLogEntry*)(ep + 0xD78);

        switch (e->state) {
        case 0:
            break;
        case 10:
            e->targetX = 0.0f;
            e->targetY = 140.f;
            e->timer = 0;
            e->state++;
            break;
        case 11:
            if (++e->timer > 8) {
                e->targetX = -160.f;
                e->targetY = 140.f;
                e->timer = 0;
                e->state++;
            }
            break;
        case 20:
            e->targetX = 0.0f;
            e->targetY = 140.f;
            e->timer = 0;
            e->state++;
            break;
        case 21:
            if (++e->timer > 8) {
                e->targetX = 0.0f;
                {
                    f32 prod = 45.0f * (f32)w->count;
                    e->targetY = prod / 2.0f - 45.0f * (f32)i;
                }
                e->timer = 0;
                e->state++;
            }
            break;
        case 30:
            e->targetX = 0.0f;
            e->targetY = -160.f;
            e->timer = 0;
            e->state++;
            break;
        case 40:
            e->targetX = 0.0f;
            {
                f32 prod = 45.0f * (f32)w->count;
                e->targetY = prod / 2.0f - 45.0f * (f32)i;
            }
            e->timer = 0;
            e->state++;
            break;
        case 41:
            break;
        }
        {
            f32 dx, dy;
            dx = e->targetX - e->curX;
            e->curX += dx / 8.0f;
            dy = e->targetY - e->curY;
            e->curY += dy / 8.0f;
        }
    }

    {
        f32 d1 = lw->e28 - lw->e2c;
        lw->e2c += d1 / 8.0;
    }
    {
        f32 d2 = lw->e30 - lw->e34;
        lw->e34 += d2 / 8.0;
    }
    {
        f32 d3 = lw->e64 - lw->e68;
        lw->e68 += d3 / 8.0;
    }
    {
        f32 d4 = lw->e6c - lw->e70;
        lw->e70 += d4 / 8.0;
    }
    {
        f32 d5 = lw->e74 - lw->e78;
        lw->e78 += d5 / 8.0;
    }
    {
        f32 d6 = lw->e7c - lw->e80;
        lw->e80 += d6 / 4.0f;
    }
    lw->ea4 = (f32)(lw->cardPage * 0xE8);
    lw->ea0 += (lw->ea4 - lw->ea0) / 6.0f;
    lw->e84 += (lw->e88 - lw->e84) / 6.0f;
    lw->e8c += (lw->e90 - lw->e8c) / 6.0f;
    lw->ea8 += (lw->eac - lw->ea8) / 6.0f;
    lw->eb0 += (lw->eb4 - lw->eb0) / 6.0f;
    lw->fa0 = (f32)(lw->badgePage * 0xE8);
    lw->f9c += (lw->fa0 - lw->f9c) / 6.0f;
    lw->f80 += (lw->f84 - lw->f80) / 6.0f;
    lw->f88 += (lw->f8c - lw->f88) / 6.0f;
    lw->fa4 += (lw->fa8 - lw->fa4) / 6.0f;
    lw->fac += (lw->fb0 - lw->fac) / 6.0f;
}

static inline Vec logMkVec(Vec base, f32 x, f32 y) {
    base.x = x;
    base.y = y;
    return base;
}

static inline Vec logMkVecYX(Vec base, f32 y, f32 x) {
    base.x = x;
    base.y = y;
    return base;
}

// Draws one page of the log window: map, crystal stars, cards, badges, the tattle viewer and the tab strip.
void winLogDisp(CameraId camId, WindowWork* work, s32 idx) {
    LogWin* w = (LogWin*)work;
    static const GXColor white0 = {0xFF, 0xFF, 0xFF, 0xFF};
    GXColor white3;
    GXColor white;
    f32 x = work->recs[idx].b;
    f32 y = work->recs[idx].c;
    s32 i;

    white = white0;
    white3 = white0;

    winBgGX(work, 4, x, y);
    if (y + w->e34 > -480.f) {
        mapGX(work, x + w->e2c, y + w->e34);
    }
    if (y + w->e70 > -480.f) {
        winTexInit(LOG_TEXPAL());
        winTexSet(0x24, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x + w->e68, y + w->e70), (Vec){6e+01f, 1.0f, 1.0f},
                  (GXColor){0xFF, 0xFF, 0xFF, w->e78});
        for (i = 0; i < 7; i++) {
            Vec p = (Vec){0.0f, 0.0f, 1.7e+02f};
            Mtx rx;
            Mtx ry;

            PSMTXRotRad(rx, 'x', 0.43633232f);
            PSMTXRotRad(ry, 'y', 0.017453292f * (51.42857f *
                (f32)i
                - w->e80));
            PSMTXMultVec(ry, &p, &p);
            PSMTXMultVec(rx, &p, &p);
            winKageGX(work, p.x + (x + w->e68), (p.y + (y + w->e70)) - 30.f, p.z,
                      1.0f, &(GXColor){0x00, 0x00, 0x00, 0x80});
            ((effStarStoneData*)w->stones[i]->userdata)->x = p.x + (x + w->e68);
            ((effStarStoneData*)w->stones[i]->userdata)->y = 20.f + (p.y + (y + w->e70));
            ((effStarStoneData*)w->stones[i]->userdata)->z = p.z;
        }
        for (i = 0; i < 7; i++) {
            effStarStoneDraw(8, w->stones[i]);
        }
    }
    if (w->e84 < 320.f && w->e8c > -240.f) {
        winHakoGX(work, 0, x + w->e84, y + w->e8c);
        winTexInit(LOG_TEXPAL());
        winTexSet(0x95, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x + w->ea8, y + w->eb0), (Vec){1.0f, 1.0f, 1.0f}, white);
        if (swGet(w->cardIcon[w->cardSel] + 0x80) != 0) {
            winIconInit();
            winIconSet(itemDataTable[w->cardIcon[w->cardSel] + 240].icon_id,
                       logMkVec((Vec){0.0f, 0.0f, 0.0f}, x + w->ea8, (y + w->eb0) - 4.0f),
                       (Vec){1.0f, 1.0f, 1.0f}, white);
        }
    }
    if (w->f80 < 320.f && w->f88 > -240.f) {
        winHakoGX(work, 1, x + w->f80, y + w->f88);
        winTexInit(LOG_TEXPAL());
        winTexSet(0x96, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x + w->fa4, y + w->fac), (Vec){1.0f, 1.0f, 1.0f}, white);
        if (swGet(w->badgeIcon[w->badgeSel] + 0x41) != 0) {
            winIconInit();
            winIconSet(itemDataTable[w->badgeIcon[w->badgeSel] + 179].icon_id,
                       logMkVec((Vec){0.0f, 0.0f, 0.0f}, x + w->fa4, (y + w->fac) - 10.f),
                       (Vec){1.0f, 1.0f, 1.0f}, white);
        }
    }
    if (w->state == 0xE || (u32)(w->state - 0x14) <= 2 || w->state == 0x3E9) {
        monoshiriGX(work, x - 170.f, 130.f + y);
    }
    for (i = 0; i < w->count; i++) {
        if (i == w->sel) {
            winTexInit(LOG_TEXPAL());
            winTexSet(0xB5, logMkVecYX((Vec){0.0f, 0.0f, 0.0f}, y + w->rows[i].curY, x + w->rows[i].curX),
                      (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x00, 0x00, 0x00, 0x80});
            winTexInit_x2(LOG_TEXPAL());
            winTexSet_x2(w->rows[i].type + 0x1F, 0xB5,
                         (Vec){(x + w->rows[i].curX) - 8.0f, 8.0f + (y + w->rows[i].curY), 0.0f},
                         (Vec){1.0f, 1.0f, 1.0f}, white3);
        } else {
            winTexInit_x2(LOG_TEXPAL());
            winTexSet_x2(w->rows[i].type + 0x1F, 0xB5,
                         logMkVecYX((Vec){0.0f, 0.0f, 0.0f}, y + w->rows[i].curY, x + w->rows[i].curX),
                         (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x80, 0x80, 0x80, 0xFF});
        }
    }
}

// The last arm's original codegen keeps swByteGet's result in r3 and computes
// the `>= 0x15C` value before the branch, so the C default-result/override
// ordering below is needed to reproduce it (see rule 7).
const char* winGetMapTplName(void) {
    if (swByteGet(0) < 0x13) {
        return "map0.tpl";
    }
    if (swByteGet(0) < 0x41) {
        return "map1.tpl";
    }
    if (swByteGet(0) < 0x79) {
        return "map2.tpl";
    }
    if (swByteGet(0) < 0xAF) {
        return "map3.tpl";
    }
    if (swByteGet(0) < 0xDE) {
        return "map4.tpl";
    }
    if (swByteGet(0) < 0x110) {
        return "map5.tpl";
    }
    if (swByteGet(0) < 0x15C) {
        return "map6.tpl";
    }
    return "map7.tpl";
}

void mapGX(WindowWork* work, f32 x, f32 y) {
    static f32 pos_tbl[7][2] = {
        {165.0f, 68.0f}, {0.0f, 75.0f}, {97.0f, 150.0f}, {-139.0f, -33.0f}, {41.0f, -163.0f}, {211.0f, -91.0f}, {-209.0f, 159.0f},
    };
    LogWin* w = (LogWin*)work;
    f32 top;
    f32 tx = 74.0f + (x + w->mapScrollX);
    static const GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    GXColor c0;
    f32 ty = 120.f + (y + w->mapScrollY);
    GXColor c1 = white;
    f32 bounce;
    f32 d;
    s32 i;

    c0 = white;
    if (w->mapTpl != NULL) {
        top = 120.f + y;
        GXSetScissor((u32)(((304.0f + x) - 230.f) - 2.0f),
                     (u32)((240.f - top) - 2.0f), 0x1D0, 0xF4);
        winTexInit((TEXPalette*)w->mapTpl->data[0]);
        winTexSet(0, logMkVec((Vec){0.0f, 0.0f, 0.0f}, tx, ty), (Vec){1.0f, 1.0f, 1.0f}, c1);
        winTexInit(LOG_TEXPAL());
        for (i = 0; (u32)i < 0x5D; i++) {
            TubuDt* t = &tubuDt[i];

            if (swByteGet(0) < (s32)t->minProgress) {
                if (!tubuUnlocked(t)) {
                    continue;
                }
            }
            if (w->mapSel == i) {
                GXColor c = c0;

                winTexSet(((gp->retraceCount >> 4) & 1) ? WIN_LOG_TEX_PIN_SEL : WIN_LOG_TEX_PIN_ON, (Vec){tx + (f32)t->x, ty + (f32)t->y, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, c);
            } else {
                GXColor c = c0;

                winTexSet((t->visible != 0) ? WIN_LOG_TEX_PIN_ON : WIN_LOG_TEX_PIN_OFF, (Vec){tx + (f32)t->x, ty + (f32)t->y, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, c);
            }
        }
        {
            s32 sw = swByteGet(0);
            s32 icon = -1;
            s32 k;

            if (sw >= 0x13 && sw < 0x37) {
                icon = 0x197;
                k = 0;
            } else if (sw >= 0x41 && sw < 0x70) {
                icon = 0x198;
                k = 1;
            } else if (sw >= 0x79 && sw < 0xA4) {
                icon = 0x19A;
                k = 2;
            } else if (sw >= 0xAF && sw < 0xD5) {
                icon = 0x19C;
                k = 3;
            } else if (sw >= 0xDE && sw < 0x105) {
                icon = 0x19D;
                k = 4;
            } else if (sw >= 0x110 && sw < 0x152) {
                icon = 0x199;
                k = 5;
            } else if (sw >= 0x15C && sw < 0x175) {
                icon = 0x19B;
                k = 6;
            }
            if (icon != -1) {
                winIconInit();
                winIconSet(icon, logMkVec((Vec){0.0f, 0.0f, 0.0f}, tx + pos_tbl[k][0], 45.0f + (ty + pos_tbl[k][1])), (Vec){1.0f, 1.0f, 1.0f}, c1);
            }
        }
        if (w->mapMarioPose != -1) {
            AnimData* anim;
            f32 scale;
            Mtx m;
            s32 n;

            animPoseGetAnimPosePtr(work->marioPose);
            anim = animPoseGetAnimDataPtr(work->marioPose);
            scale = 1.5f;
            if (w->flags & 0x2000) {
                scale = 3.0f;
            }
            PSMTXTrans(m, tx + (f32)tubuDt[w->mapPick].x, ty + (f32)tubuDt[w->mapPick].y, 0.0f);
            n = (s32)anim->clip->field_0x8;
            animPoseSetLocalTime(w->mapMarioPose, (f32)(gp->retraceCount % n));
            animPoseMain(w->mapMarioPose);
            animPoseDrawMtx(w->mapMarioPose, m, 1, 0.0f, scale);
            animPoseDrawMtx(w->mapMarioPose, m, 2, 0.0f, scale);
            animPoseDrawMtx(w->mapMarioPose, m, 3, 0.0f, scale);
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBF, logMkVec((Vec){0.0f, 0.0f, 0.0f}, (x - 230.f) - 4.0f, y), (Vec){1.0f, 32.0f, 1.0f}, c1);
        winTexSet(0xBF, logMkVec((Vec){0.0f, 0.0f, 0.0f}, 4.0f + (230.f + x), y), (Vec){1.0f, 32.0f, 1.0f}, c1);
        winTexSet(0xBF, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x, 4.0f + top), (Vec){59.5f, 1.0f, 1.0f}, c1);
        winTexSet(0xBF, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x, (y - 120.f) - 4.0f), (Vec){59.5f, 1.0f, 1.0f}, c1);
#ifdef MUST_MATCH
        // Match-only: keep __fabs's double result in f0 (the original does
        // `fabs f0, f0; frsp f29, f0`); without the explicit f64 temp MWCC
        // folds the abs into the callee-saved f29 (`fabs f29, f0`).
        {
            f32 b = 2.0f * (f32)sin((f32)gp->retraceCount / 8.0f);
            f64 c = __fabs((f64)b);
            bounce = (f32)c;
        }
#else
        bounce = (f32)__fabs(2.0f * (f32)sin((f32)gp->retraceCount / 8.0f));
#endif
        winTexInit(LOG_TEXPAL());
        if (-240.f == w->mapScrollY) {
            d = 0.0f;
        } else {
            d = bounce;
        }
        winTexSet(0xBD, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x, top + d), (Vec){1.0f, 1.0f, 1.0f}, c1);
        if (-148.0f == w->mapScrollX) {
            d = 0.0f;
        } else {
            d = bounce;
        }
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBC, logMkVec((Vec){0.0f, 0.0f, 0.0f}, 220.f + x + d, y), (Vec){1.0f, 1.0f, 1.0f}, c1);
        if (0.0f == w->mapScrollY) {
            d = 0.0f;
        } else {
            d = bounce;
        }
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBE, logMkVec((Vec){0.0f, 0.0f, 0.0f}, x, (y - 120.f) - d), (Vec){1.0f, 1.0f, 1.0f}, c1);
        if (0.0f == w->mapScrollX) {
            d = 0.0f;
        } else {
            d = bounce;
        }
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBB, logMkVec((Vec){0.0f, 0.0f, 0.0f}, (x - 220.f) - d, y), (Vec){1.0f, 1.0f, 1.0f}, c1);
    }
}

// Behaviour complete, checked against the asm. Declaring the shared rodata base non-const and
// disabling loop-invariant motion stopped MWCC hoisting the vec3 loads; inlining the int->float
// offsets and the CSE'd `440.f + x` value (bx) into the winTexSet/winIconSet calls
// made the allocator rematerialise them as the original does, and `buf[0x100]` reproduces the
// original's 0x390 frame and every Vec/buf stack offset. The five winFontSet and three winIconSet
// calls take their Vec/Vec/GXColor by value; all 33 calls and
// every immediate match. Writing the rodata strings as plain literals (the AGENT_BRIEF base-
// register recipe) makes MWCC pool them on the same base register as the vec3_* LV reads, so the
// extra `addi base, pool, 0` and the r23/r24 colouring are gone and the instruction count is exact.
void monoshiriGX(WindowWork* work, f32 x, f32 y) {
    static const GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    static const GXColor black = {0x00, 0x00, 0x00, 0xFF};
    s32 i;
    LogWin* w = (LogWin*)work;
    GXColor c8c;
    GXColor c4;
    GXColor c8b;
    const char* msg;
    s32 wdt;
    char buf[0x100];
    char buf2[0x20];

    c4 = white;
    c8b = black;
    c8c = black;
    winBookGX(work, 1, x, y);
    winKirinukiGX(work, 0, x - 110.f, y - 160.f, 100.f, 50.f);
    {
        const char* pmsg = msgSearch("msg_menu_monosiri_kazu");
        wdt = (s32)(0.75f * (f32)FontGetMessageWidth(pmsg));
        winFontInit();
        {
            winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, (x - 108.0f) + (f32)((100 - (u16)wdt) / 2),
                                y - 166.0f), (Vec){0.75f, 0.75f, 0.75f}, c8b, pmsg);
        }
    }
    sprintf(buf, "%d\201^%d", w->seenCnt, w->charCnt);
    wdt = (s32)(0.75f * (f32)FontGetMessageWidth(buf));
    {
        winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, (x - 108.0f) + (f32)((100 - (u16)wdt) / 2),
                            y - 184.0f), (Vec){0.75f, 0.75f, 0.75f}, c8b, buf);
    }
    winFontInit();
    for (i = 0; i < 16; i++) {
        s32 id = w->chars[i + w->charPage * 16].id;
        TattleEntry* tattle = battleGetUnitMonosiriPtr(id);
        

        if (i + w->charPage * 16 >= w->charCnt) {
            break;
        }
        if (swGet(id + 0x117A) != 0) {
            if (id == 0x4D || id == 0x87) {
                if (evtGetValue(NULL, GSW(0)) < 0xD2) {
                    msg = msgSearch("btl_un_hatena");
                } else {
                    msg = msgSearch("btl_un_rampell");
                }
            } else {
                msg = msgSearch(tattle->enemyTag);
            }
        } else {
            msg = "\201H\201H\201H";
        }
        sprintf(buf2, "%d\x81\x44", (s32)enemy_monoshiri_sort_table[id]);
        
        {
            winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, ((24.0f + x) - 5.0f) + (f32)(((i / 8) * 0xD8)), (y - 16.0f) - (f32)((i % 8) * 0x18)), (Vec){0.75f, 0.75f, 0.75f}, c8c, buf2);
        }
        {
            winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, (56.0f + x) + (f32)(((i / 8) * 0xD8)), (y - 16.0f) - (f32)((i % 8) * 0x18)), (Vec){0.75f, 0.75f, 0.75f}, c8c, msg);
        }
    }
    if (w->charPage > 0) {
        f32 bx;

        winTexInit(LOG_TEXPAL());
        bx = 440.f + x;
        winTexSet(0x17, logMkVec((Vec){0.0f, 0.0f, 0.0f}, bx, 18.0f + y), (Vec){-1.0f, -1.0f, -1.0f}, c4);
        winIconInit();
        {
            winIconSet(0x7E, logMkVec((Vec){0.0f, 0.0f, 0.0f}, bx, y), (Vec){0.6f, 0.6f, 0.6f}, c4);
        }
    }
    if (w->charPage < (w->charCnt + 0xF) / 16 - 1) {
        f32 bx;

        winTexInit(LOG_TEXPAL());
        bx = 440.f + x;
        winTexSet(0x17, logMkVec((Vec){0.0f, 0.0f, 0.0f}, bx, (y - 15.0f) - 240.f), (Vec){1.0f, 1.0f, 1.0f}, c4);
        winIconInit();
        {
            winIconSet(0x80, logMkVec((Vec){0.0f, 0.0f, 0.0f}, bx, y - 240.f), (Vec){0.6f, 0.6f, 0.6f}, c4);
        }
    }
    msg = msgSearch("msg_menu_sort_narabikae");
    {
        f32 fw = 0.7f * (f32)FontGetMessageWidth(msg);
        f32 ox;

        winFontInit();
        ox = x + (420.f - fw);
        {
            winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, ox, y - 210.f), (Vec){0.7f, 0.7f, 0.7f}, c8b, msg);
        }
        winIconInit();
        {
            winIconSet(0x217, logMkVec((Vec){0.0f, 0.0f, 0.0f}, ox - 30.f, y - 220.f), (Vec){0.6f, 0.6f, 0.6f}, c4);
        }
    }
}

void* monosiriInit(BattleUnitType kind, f32 x, f32 y) {
    MonosiriWork* w = _mapAlloc(sizeof(MonosiriWork));

    memset(w, 0, sizeof(MonosiriWork));
    w->state = 0;
    w->unitKind = kind;
    w->targetX = x;
    w->targetY = y;
    w->x = 500.f;
    w->y = -300.f;
    w->alpha = 0;
    w->poseId = -1;
    w->poseId2 = -1;
    return w;
}

// The original addresses these .rodata objects as offsets from one base symbol.


// 100%: matches through the unit's rodata base. The {jp, us} language table
// lives at "msg_menu_kiroku_map"+0x350; reading it through `base` (instead of a local
// initialiser) makes the compiler emit the original's `lwz r3, 0x350(r31)`.
// `inBattle` is computed before the two table reads and as `!(x == 0)` so the
// boolean lands in r3 and is copied to r28 exactly as the original does
// (semantically identical to `x != 0`, just different MWCC colouring).
void monosiriMain(void* work) {
    MonosiriWork* w = work;
    TattleEntry* tattle;
    GlobalWork* g = gp;
    s32 inBattle = !(g->inBattle == 0);
    const char* lang[2] = { "jp", "us" };
    GXTexObj texObj;
    s32 size;


    if (w == NULL) {
        return;
    }

    switch (w->state) {
    case 0:
        if (fileAsyncf(FILE_TYPE_TPL, NULL, "%s/w/%s/win.tpl", getMarioStDvdRoot(), lang[g->language]) != 0) {
            if (fileAsyncf(FILE_TYPE_RAW, NULL, "%s/w/jp/mono.dat", getMarioStDvdRoot()) != 0) {
                w->file1 = fileAllocf(FILE_TYPE_TPL, "%s/w/%s/win.tpl", getMarioStDvdRoot(), lang[gp->language]);
                w->file2 = fileAllocf(FILE_TYPE_RAW, "%s/w/jp/mono.dat", getMarioStDvdRoot());
                w->state++;
                TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &texObj, 0x37);
                size = GXGetTexBufferSize(GXGetTexObjWidth(&texObj), GXGetTexObjHeight(&texObj), 5, 0, 0);
                w->alloc = smartAlloc(size, 0);
            }
        }
        break;
    case 1:
        tattle = battleGetUnitMonosiriPtr(w->unitKind);
        if (animGroupBaseAsync(tattle->modelName, inBattle, NULL)) {
            if (w->unitKind != 0x6B || animGroupBaseAsync("c_n_moa_a", inBattle, NULL)) {
                w->alpha += 0x14;
                if (w->alpha > 0xFF) {
                    w->alpha = 0xFF;
                }
                if (w->poseId == -1) {
                    if (w->unitKind == 0x6B) {
                        w->poseId = animPoseEntry(tattle->modelName, inBattle);
                        animPoseSetAnim(w->poseId, tattle->poseName, TRUE);
                        animPoseWorldPositionEvalOn(w->poseId);
                        animPoseMain(w->poseId);
                        w->poseId2 = animPoseEntry("c_n_moa_a", inBattle);
                        animPoseSetAnim(w->poseId2, "S_1", TRUE);
                        animPoseMain(w->poseId2);
                    } else {
                        w->poseId = animPoseEntry(tattle->modelName, inBattle);
                        animPoseSetAnim(w->poseId, tattle->poseName, TRUE);
                        animPoseMain(w->poseId);
                    }
                    dispEntry(2, 0.0f, 6, capture, w);
                }
            }
        }
        break;
    case 2:
        w->targetX = 600.f;
        w->targetY = -400.f;
        break;
    }

    w->x += (w->targetX - w->x) / 6.0f;
    w->y += (w->targetY - w->y) / 6.0f;
    if (w->state > 0) {
        dispEntry(8, 0.0f, 0, monosiri_disp, w);
    }
}

void monosiriClose(void* work) {
    MonosiriWork* w = work;

    if (w != NULL) {
        w->state = 2;
    }
}

BOOL monosiriCloseWait(void* work) {
    MonosiriWork* w = work;

    if (w == NULL) {
        return TRUE;
    }
    if (w->x > 500.f && w->y < -300.f) {
        return TRUE;
    }
    return FALSE;
}

void monosiriExit(void* work) {
    MonosiriWork* w = work;

    if (w != NULL) {
        if (w->file1 != NULL) {
            fileFree(w->file1);
            w->file1 = NULL;
        }
        if (w->file2 != NULL) {
            fileFree(w->file2);
            w->file2 = NULL;
        }
        if (w->poseId != -1) {
            animPoseRelease(w->poseId);
            w->poseId = -1;
        }
        if (w->poseId2 != -1) {
            animPoseRelease(w->poseId2);
            w->poseId2 = -1;
        }
        if (w->alloc != NULL) {
            smartFree(w->alloc);
            w->alloc = NULL;
        }
        _mapFree(w);
    }
}

// Renders a tattle model into the viewer's texture: frames the model's bounding box, draws the pose (and the swarm
// for the moamoa tattle) and copies the EFB into the viewer's image.
// The original reads two dead zero vectors at base+0x358/0x364 into scratch slots before overwriting them from
// AnimData; the MUST_MATCH block below reproduces that copy so the frame layout matches. The scratch slots are read
// through the unit's existing `vec3_...` rodata names so the base stays live in r30. `tmp` doubles as the identity
// matrix, as in the original.
void capture(CameraId camId, void* param) {
    MonosiriWork* w = param;
    AnimData* ad;
    Vec mn;
    Vec mx;
    f32 cx;
    f32 cy;
    GXTexObj obj;
    f32 vp[6];
    f32 proj[7];
    u32 sl;
    u32 st;
    u32 sw;
    u32 sh;
    u16 tw;
    u16 th;
    Mtx44 persp;
    Mtx view;
    Mtx tmp;
    Mtx scaleM;
    f32 dist;
    Vec q;
    Vec p;
    Mtx m;
    s32 i;
    TattleCamEntry* tbl;
    s32 n;
    f32 ox;
    f32 oy;
    f32 os;
    CameraEntry saved;
    u16 bl;
    u16 bt;
    u16 br;
    u16 bb;

    ad = animPoseGetAnimDataPtr(w->poseId);
    mn = (Vec){ad->field_0x44, ad->field_0x48, ad->field_0x4C};
    mx = (Vec){ad->field_0x50, ad->field_0x54, ad->field_0x58};
    cx = (mx.x + mn.x) / 2.0f;
    cy = (mx.y + mn.y) / 2.0f;
    GXGetViewportv(vp);
    GXGetProjectionv(proj);
    GXGetScissor(&sl, &st, &sw, &sh);
    TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &obj, 0x37);
    tw = GXGetTexObjWidth(&obj);
    th = GXGetTexObjHeight(&obj);
    GXSetViewport(0.0f, 0.0f, (f32)tw, (f32)th, 0.0f, 1.0f);
    GXSetScissor(1, 1, tw - 2, th - 2);
    C_MTXPerspective(persp, 25.0f, 1.2666667f, 1.0f, 1000.f);
    GXSetProjection(persp, 0);
    if ((mx.x - mn.x) / (f32)tw > (mx.y - mn.y) / (f32)th) {
        dist = (mx.x - cx) / (f32)tan(HALF_FOV_TAN);
    } else {
        dist = ((608.0f * (mx.y - cy)) / 480.f) / (f32)tan(HALF_FOV_TAN);
    }
    C_MTXLookAt(view, &(Vec){cx, cy, dist}, &(Vec){0.0f, 1.0f, 0.0f}, &(Vec){cx, cy, 0.0f});
    {
        tbl = (TattleCamEntry*)w->file2->data[0];
        n = (u32)w->file2->data[1] >> 4;
        for (i = 0; i < n; i++, tbl++) {
            if (tbl->kind == w->unitKind) {
                break;
            }
        }
        if (i < n) {
            ox = tbl->x;
            oy = tbl->y;
            os = tbl->scale;
        } else {
            ox = 0.0f;
            os = 1.0f;
            oy = ox;
        }
    }
    PSMTXTrans(tmp, ox, oy, 0.0f);
    PSMTXScale(scaleM, os, os, os);
    PSMTXConcat(tmp, scaleM, tmp);
    PSMTXConcat(view, tmp, view);
    sysWaitDrawSync();
    GXClearBoundingBox();
    saved = *camGetPtr(camId);
    PSMTXCopy(view, camGetCurPtr()->view);
    if (animPoseGetVivianType(w->poseId) != 0) {
        animPoseVivianMain(w->poseId, &(Vec){0.0f, 0.0f, 0.0f});
    }
    animPoseSetMaterialFlagOn(w->poseId, 0x1800);
    animPosePeraOff(w->poseId);
    PSMTXIdentity(tmp);
    animPoseDrawMtx(w->poseId, tmp, 1, 0.0f, 1.0f);
    animPoseDrawMtx(w->poseId, tmp, 2, 0.0f, 1.0f);
    animPoseDrawMtx(w->poseId, tmp, 3, 0.0f, 1.0f);
    animPosePeraOn(w->poseId);
    if (w->unitKind == 0x6B) {
        Vec* verts = (Vec*)animPoseGetAnimPosePtr(w->poseId)->groupWorldPos->address;
        s32* counts = (s32*)((smartEntry*)smartAlloc(moamoa_chain_count * 4, 3))->address;
        s32* order = (s32*)((smartEntry*)smartAlloc(0x7D0, 3))->address;
        int j;

        memset(counts, 0, moamoa_chain_count * 4);
        for (j = 0; j < 0x1F4; j++) {
            s32 k = j % moamoa_chain_count;

            order[j] = counts[k]++;
        }
        animPoseSetMaterialFlagOn(w->poseId2, 0x1800);
        animPosePeraOff(w->poseId2);
        for (j = 0; j < 0x1F4; j++) {
            s32 k = j % moamoa_chain_count;
            f32 t = (f32)order[j] / (f32)counts[k];
            Vec* a = &verts[animPoseGetGroupIdx(w->poseId, g_moamoa_chain_tbl[k].name)];
            Vec* b = &verts[animPoseGetGroupIdx(w->poseId, g_moamoa_chain_tbl[k].anim)];

            PSVECScale(a, &p, t);
            PSVECScale(b, &q, 1.0f - t);
            PSVECAdd(&p, &q, &p);
            PSMTXTrans(m, p.x + (f32)(5 - irand(10)), p.y + (f32)(5 - irand(10)), p.z + (f32)(5 - irand(10)));
            animPoseDrawMtx(w->poseId2, m, 1, 0.0f, 1.0f);
            animPoseDrawMtx(w->poseId2, m, 2, 0.0f, 1.0f);
            animPoseDrawMtx(w->poseId2, m, 3, 0.0f, 1.0f);
        }
        animPosePeraOn(w->poseId2);
    }
    PSMTXCopy(saved.view, camGetCurPtr()->view);
    sysWaitDrawSync();
    GXReadBoundingBox(&bl, &bt, &br, &bb);
    GXSetTexCopySrc(0, 0, tw, th);
    GXSetTexCopyDst(tw, th, 5, 0);
    GXCopyTex(*(void**)w->alloc, 1);
    GXPixModeSync();
    GXSetViewport(vp[0], vp[1], vp[2], vp[3], vp[4], vp[5]);
    GXSetProjectionv(proj);
    GXSetScissor(sl, st, sw, sh);
}

static inline GXColor monoAlphaColor(GXColor c, u8 a) {
    c.a = a;
    return c;
}
void monosiri_disp(CameraId camId, void* param) {
    f32 s;
    MonosiriWork* w = param;
    GXColor c = {0x00, 0x00, 0x00, 0xFF};
    f32 x = w->x;
    f32 y = w->y;

    if (w->file1 != NULL) {
        GXTexObj obj;
        Mtx m1;
        Mtx m2;
        Mtx m3;
        s32 i;
        f32 x0;
        f32 y0;
        f32 width = 216.0f;
        const TattleEntry* tattle;
        char buf[0x100];
        f32 tx;
        f32 wdt;
        s32 unitKind;

        winHalfBookGX((void*)w->file1->data[0], x, y, 0);
        {
            u16 tw;
            u16 th;

            TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &obj, 0x37);
            tw = GXGetTexObjWidth(&obj);
            th = GXGetTexObjHeight(&obj);
            TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &obj, 0x37);
            GXInitTexObjLOD(&obj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
            GXLoadTexObj(&obj, 0);
            GXInitTexObj(&obj, *(void**)w->alloc, tw, th, 5, 0, 0, 0);
        }
        GXInitTexObjLOD(&obj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        GXLoadTexObj(&obj, 1);
        GXSetNumTexGens(8);
        for (i = 0; i < 8; i++) {
            f32 half;
            s = 1.0f - 0.04f * (f32)(7 - i);
            half = 0.5f;

            PSMTXTrans(m1, half, half, 0.0f);
            PSMTXScale(m2, s, s, s);
            {
                f32 nhalf = -0.5f;

                PSMTXTrans(m3, nhalf, nhalf, 0.0f);
            }
            PSMTXConcat(m1, m2, m1);
            PSMTXConcat(m1, m3, m1);
            GXLoadTexMtxImm(m1, 0x1E + i * 3, 1);
            GXSetTexCoordGen2(i, 1, 4, 0x1E + i * 3, 0, 0x7D);
        }
        GXSetTevColor(1, (GXColor){0xCF, 0xBC, 0x9E, 0xFF});
        GXSetTevColor(2, monoAlphaColor(unk_80423050, w->alpha));
        GXSetNumTevStages(0xC);
        for (i = 0; i < 8; i++) {
            GXSetTevOrder(i, i, 1, 0xFF);
            GXSetTevColorOp(i, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(i, 0, 0, 0, 1, 0);
            GXSetTevColorIn(i, 0xF, 0xF, 0xF, 2);
            GXSetTevAlphaIn(i, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, (i == 0) ? GX_CA_ZERO : GX_CA_APREV);
            GXSetTevKAlphaSel(i, 7);
        }
        GXSetTevOrder(8, 7, 1, 0xFF);
        GXSetTevColorOp(8, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(8, 0, 0, 0, 1, 0);
        GXSetTevColorIn(8, 0, 8, 9, 0xF);
        GXSetTevAlphaIn(8, 0, 7, 4, 4);
        GXSetTevOrder(9, 0xFF, 0xFF, 0xFF);
        GXSetTevColorOp(9, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(9, 0, 0, 0, 1, 0);
        GXSetTevColorIn(9, 0xF, 0xF, 0xF, 0);
        GXSetTevAlphaIn(9, 7, 0, 2, 7);
        GXSetTevOrder(0xA, 7, 0, 0xFF);
        GXSetTevColorOp(0xA, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0xA, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0xA, 0xF, 8, 0, 0xF);
        GXSetTevAlphaIn(0xA, 7, 7, 7, 0);
        GXSetTevOrder(0xB, 7, 0, 0xFF);
        GXSetTevColorOp(0xB, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0xB, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0xB, 8, 0, 1, 0xF);
        GXSetTevAlphaIn(0xB, 7, 7, 7, 4);
        GXBegin(0x80, 0, 4);
        x0 = 32.0f + x;
        y0 = y - 8.0f;
        tx = y0 - 160.f;
        GXPosition3f32(x0, y0, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x0, tx, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(x0 + width, tx, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x0 + width, y0, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        unitKind = w->unitKind;
        tattle = battleGetUnitMonosiriPtr(unitKind);
        if (unitKind == 0x4D || unitKind == 0x87) {
            if (evtGetValue(NULL, GSW(0)) < 0xD2) {
                sprintf(buf, "%d\201D%s", enemy_monoshiri_sort_table[unitKind],
                        msgSearch("btl_un_hatena"));
            } else {
                sprintf(buf, "%d\201D%s", enemy_monoshiri_sort_table[unitKind],
                        msgSearch("btl_un_rampell"));
            }
        } else {
            sprintf(buf, "%d\201D%s", enemy_monoshiri_sort_table[unitKind],
                    msgSearch(tattle->enemyTag));
        }
        wdt = 0.75f * (f32)FontGetMessageWidth(buf);
        winFontInit();
        winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, (16.0f + x) + (248.0f - wdt) / 2.0f, y - 180.f),
                   (Vec){0.75f, 0.75f, 0.75f}, c, buf);
        sprintf(buf, "%s\201@%s", msgSearch("menu_monosiri_derutokoro"), msgSearch(tattle->locationTag));
        wdt = 0.75f * (f32)FontGetMessageWidth(buf);
        {
            f32 wdt2 = 248.0f - wdt;

            winFontSet(logMkVec((Vec){0.0f, 0.0f, 0.0f}, 1.0f + (wdt2 / 2.0f + (16.0f + x)),
                                y - 210.f),
                       (Vec){0.75f, 0.75f, 0.75f}, c, buf);
        }
    }
}


// The SBSS2 region holds the tattle-viewer alpha scratch used by monosiri_disp.
// A named zero-initialised object keeps it in .sbss2; the object after it is
// the section's trailing gap.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") const GXColor unk_80423050;
__declspec(section ".sdata") u8 gap_10_80423054_sbss2[4];
#pragma section ".sdata" ".sbss"

// .sdata2 colour words and short region strings, in the original's address
// order. The GXColor words are the shadow/page/tab tints the draw functions
// load as flat colours.

