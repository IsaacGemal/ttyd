#include "pmario_sound.h"
#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include <dolphin/types.h>
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

extern WinRootVecs str_802F3FE8;
extern const GXColor dat_8041ced8;
extern const GXColor dat_8041cedc;
extern const GXColor dat_8041cee0;
extern const GXColor dat_8041cee4;
extern const GXColor dat_8041d014;
extern const f32 float_2_8041d090;
extern const f32 float_neg1p3963_8041d0dc;
extern const f32 float_neg0p5_8041d0e0;
extern const GXColor dat_8041cee8;
extern const GXColor dat_8041ceec;
extern const GXColor dat_8041cef0;
extern const GXColor dat_8041cef4;
extern const GXColor dat_8041cef8;
extern const GXColor dat_8041cefc;
extern const GXColor float_neg0p0047648_8041cf00;
extern const GXColor dat_8041cf04;
extern const GXColor dat_8041cf08;
extern const GXColor dat_8041cf0c;
extern const GXColor dat_8041cf10;
extern const GXColor dat_8041cf14;
extern const GXColor dat_8041cf18;
extern const GXColor dat_8041cf1c;
extern const GXColor dat_8041cf20;
extern const GXColor dat_8041cf24;
extern const f32 float_18_8041d1b8;
extern const f32 float_320_8041d074;
extern const f32 float_570_8041d1bc;
extern const f32 float_288_8041d16c;
extern const f32 float_neg285_8041d1c0;
extern const f32 float_160_8041d1c4;
extern const f32 float_neg160_8041d1c8;
extern const f32 float_285_8041d1cc;
extern const f32 float_144_8041d1d0;
extern const f32 float_neg144_8041d1d4;

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

typedef struct SortTable {
    const char* name;                 // 0x0
    void (*func)(WindowWork* work);    // 0x4
} SortTable;

extern SortTable* sort_data[8];
extern const f32 float_neg1000_8041d0cc;
extern const f32 float_0_8041d068;
extern const f32 float_neg500_8041d0d0;
extern const f32 float_300_8041d0d4;
extern const f32 float_neg100_8041d0d8;
extern const f32 float_neg600_8041d0fc;
extern const f32 float_30_8041d100;
extern const f32 float_40_8041d0e8;
extern const f32 float_0p125_8041d104;
extern const f32 float_0p5_8041d06c;
extern const f32 float_1_8041d070;
extern ItemData itemDataTable[];

static char msgbuf[0x3C];

const char str_802F4510[] = "menu_monosiri_%s";
const char str_802F44F0[] = "btl_un_hatena";
const char str_802F4500[] = "btl_un_rampell";

static const char* const tbl[] = {
    "nara", "gor",  "tik", "shiga", "hei", "nok", "stn", "gon", "win",
    "hou",  "mri",  "tou", "usu",   "gra", "jin", "muj", "dou", "hom",
    "eki",  "rsh",  "pik", "sin",   "bom", "moo", "aji", "las",
};

const GXColor dat_8041d064 = {0xFF, 0xFF, 0xFF, 0xFF};


s32 winSortWait(WindowWork* work) {
    return WW_S32(work, 0x17C) != 0;
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
        GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041d064);
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

void unk_8014d5dc(s32 unused, WindowWork* work) {
    GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
    GXSetTexCopyDst(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(**(void***)((u8*)work + 0x34), GX_FALSE);
    GXTexModeSync();
    WW_S32(work, 0x38) = 1;
    N_mapDispOff();
    camDispOff(4);
}

void winMsgEntry(WindowWork* work, s32 a1, const char* str, s32 a3) {
    u16 width;
    BOOL changed = FALSE;

    if (str != NULL) {
        if (WW_PTR(work, 0x144) != (void*)str) {
            changed = TRUE;
        } else if (strcmp(msgbuf, str) != 0) {
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
            strncpy(msgbuf, str, 0x3C);
        }
    }
}

// Full-screen GX quad. The original reloads each position constant into f0
// immediately before its FIFO store instead of CSE-ing the constants across the
// four vertices, so the vertex writes under MUST_MATCH read the named floats
// through a volatile view (the natural GXPosition3f32 form is in #else).
void winZClear(void) {
    Mtx mtx;

    PSMTXTrans(mtx, float_0_8041d068, float_0_8041d068, float_neg1000_8041d0cc);
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
#ifdef MUST_MATCH
    // volatile reads stop MWCC from CSE-ing the position constants across the
    // four vertices; the original reloads each one into f0 before its store.
    GXWGFifo.f32 = *(volatile f32*)&float_neg500_8041d0d0;
    GXWGFifo.f32 = *(volatile f32*)&float_300_8041d0d4;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_300_8041d0d4;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_neg100_8041d0d8;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_neg500_8041d0d0;
    GXWGFifo.f32 = *(volatile f32*)&float_neg100_8041d0d8;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
#else
    GXPosition3f32(float_neg500_8041d0d0, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_neg100_8041d0d8, float_0_8041d068);
    GXPosition3f32(float_neg500_8041d0d0, float_neg100_8041d0d8, float_0_8041d068);
#endif
    GXEnd();
    GXSetColorUpdate(1);
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
    WW_F32(work, 0x164) = float_neg1000_8041d0cc;
    WW_F32(work, 0x16C) = a;
    WW_F32(work, 0x168) = b;
    WW_F32(work, 0x170) = b;
    WW_S32(work, 0x17C) = 1;
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

// NONMATCHING (80.65%): the instruction multiset matches the original; only MWCC's
// scheduling differs (the original hoists `lwz r4, 0x8(r4)` above the prologue
// saves, ours emits it after them).
int N_compare_func4_1(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// NONMATCHING (80.65%): same schedule-only difference as N_compare_func4_1 (ids
// are s16, the original `extsh`s both).
int N_compare_func4_1_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

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

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func5_1(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func5_1_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func6_1(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func6_1_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

// NONMATCHING (92.86%): 1 missing, 1 extra.
int compare_func4(void* a, void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sa, sb);
}

// NONMATCHING (92.31%): 1 missing, 1 extra.
int compare_func4_r(void* a, void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sb, sa);
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

void sort_8_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func4);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func4_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
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

int compare_func3_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_b, (char*)buf_a);
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
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_a, (char*)buf_b);
}

void sort_7_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func2);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
// NONMATCHING (94.86%): only MWCC register allocation (table base/tag/index FPRs)
// and the compiler-generated table symbol differ.
int compare_func2_r(void* a, void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));
    const char* tag_a = ea->locationTag;
    const char* tag_b;
    s32 i;
    s32 j;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf_a, str_802F4510, tbl[i]);
        if (strcmp((char*)buf_a, tag_a) == 0) {
            break;
        }
    }
    if ((u32)i >= 0x1A) {
        i = 0;
    }

    tag_b = eb->locationTag;
    for (j = 0; (u32)j < 0x1A; j++) {
        sprintf((char*)buf_b, str_802F4510, tbl[j]);
        if (strcmp((char*)buf_b, tag_b) == 0) {
            break;
        }
    }
    if ((u32)j >= 0x1A) {
        j = 0;
    }

    if (i < j) {
        return 1;
    } else if (i > j) {
        return -1;
    } else {
        return 0;
    }
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
// NONMATCHING (94.86%): only MWCC register allocation (table base/tag/index FPRs)
// and the compiler-generated table symbol differ.
int compare_func2(void* a, void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));
    const char* tag_a = ea->locationTag;
    const char* tag_b;
    s32 i;
    s32 j;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf_a, str_802F4510, tbl[i]);
        if (strcmp((char*)buf_a, tag_a) == 0) {
            break;
        }
    }
    if ((u32)i >= 0x1A) {
        i = 0;
    }

    tag_b = eb->locationTag;
    for (j = 0; (u32)j < 0x1A; j++) {
        sprintf((char*)buf_b, str_802F4510, tbl[j]);
        if (strcmp((char*)buf_b, tag_b) == 0) {
            break;
        }
    }
    if ((u32)j >= 0x1A) {
        j = 0;
    }

    if (i > j) {
        return 1;
    } else if (i < j) {
        return -1;
    } else {
        return 0;
    }
}

void sort_6_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_1);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_1_r);
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

void sort_5_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_1);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
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

void sort_4_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_1);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
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

void sort_2_2_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 3 : 10);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_2_1_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 2 : 9);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
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
        WW_F32(work, 0x16C) = float_neg600_8041d0fc;
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
        WW_F32(work, 0x16C) = float_neg600_8041d0fc;
    }

    if (WW_S32(work, 0x17C) != 0) {
        BOOL changed = FALSE;
        const char* msg;
        u16 width;

        WW_F32(work, 0x158) = float_30_8041d100 + WW_F32(work, 0x16C);
        WW_F32(work, 0x15C) = (WW_F32(work, 0x170) - float_40_8041d0e8) -
                              (f32)(WW_S32(work, 0x174) * 0x28);
        msg = sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].name;
        if (msg != NULL) {
            if (WW_PTR(work, 0x144) != (void*)msg) {
                changed = TRUE;
            } else if (strcmp(msgbuf, msg) != 0) {
                changed = TRUE;
            }
            if (changed) {
                WW_S32(work, 0x138) = 0;
                WW_PTR(work, 0x13C) = (void*)msg;
                WW_S32(work, 0x140) = 0;
                FontGetMessageWidthLine((const char*)msgSearch((const char*)WW_PTR(work, 0x13C)), &width);
                WW_S32(work, 0x148) = 0;
                WW_S32(work, 0x14C) = width + 1;
                WW_PTR(work, 0x144) = (void*)msg;
                strncpy(msgbuf, msg, 0x3C);
            }
        }
    }

    {
#ifdef MUST_MATCH
        // Two deltas (the natural form shares one): a single temp makes MWCC swap
        // the first fmadd's difference and accumulator registers.
        f32 d1;
        f32 d2;
        d1 = WW_F32(work, 0x16C) - WW_F32(work, 0x164);
        WW_F32(work, 0x164) += d1 * float_0p125_8041d104;
        d2 = WW_F32(work, 0x170) - WW_F32(work, 0x168);
        WW_F32(work, 0x168) += d2 * float_0p125_8041d104;
#else
        WW_F32(work, 0x164) += float_0p125_8041d104 * (WW_F32(work, 0x16C) - WW_F32(work, 0x164));
        WW_F32(work, 0x168) += float_0p125_8041d104 * (WW_F32(work, 0x170) - WW_F32(work, 0x168));
#endif
    }
}

extern const f32 float_neg800_8041d1b0;
extern const f32 float_neg170_8041d1b4;

void winMsgMain(WindowWork* work) {
    switch (WW_S32(work, 0x130)) {
        case 0:
            WW_F32(work, 0x128) = float_neg800_8041d1b0;
            WW_F32(work, 0x12C) = float_neg170_8041d1b4;
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 1: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, float_neg800_8041d1b0, float_0_8041d068);
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
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, float_0_8041d068, float_neg800_8041d1b0);
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

extern const f32 float_800_8041d1d8;
extern const f32 float_640_8041d1dc;
extern const f32 float_neg640_8041d1e0;

extern const f32 float_250_8041d1a4;
extern const f32 float_10_8041d1a0;
extern const f32 float_500_8041d1a8;
extern const f32 float_neg130_8041d1ac;
extern const f32 float_120_8041d09c;
extern const f32 float_8_8041d0a0;
extern const f32 float_16_8041d0e4;
extern const f32 float_32_8041d0a4;
extern const f32 float_240_8041d0ac;
extern const f32 float_24_8041d0c8;
extern const f32 float_14_8041d180;
extern const f32 float_80_8041d0b0;
extern const f32 float_20_8041d0c0;
extern const f32 float_4_8041d07c;
extern const GXColor dat_8041cf28;
extern const GXColor dat_8041cf2c;
extern const GXColor dat_8041cf30;
extern const GXColor dat_8041cf34;
extern const GXColor dat_8041cf38;
extern const GXColor dat_8041cf3c;
extern const GXColor dat_8041cf40;
extern const GXColor dat_8041cf44;

static inline void winMsgDispIcon(s32 iconId, Vec translate, Vec* scale, GXColor color) {
    winIconSet(iconId, &translate, scale, &color);
}

static inline void winMsgDispTex(Vec translate, Vec* scale, GXColor color) {
    winTexSet(0x25, translate, *scale, color);
}

static inline void winMsgDispEdge(Vec pos, Vec* scale, GXColor color, const char* msg) {
    winFontSetEdge(&pos, scale, &color, msg);
}

static inline void winMsgDispText(Vec pos, Vec* scale, GXColor color, const char* msg) {
    winFontSet(&pos, scale, &color, msg);
}

static inline void winMsgDispLabel(Vec pos, Vec* scale, GXColor color, const char* msg) {
    winFontSetLabel(&pos, scale, &color, msg);
}

// Same shape as root_vec (defined with winRootDisp below), but winMsgDisp is emitted
// before it. Builds a by-value Vec from a base Vec in the str_802F3FE8 rodata block.
static inline Vec winMsgDispVec(const Vec* base, f32 x, f32 y) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    return v;
}

// str_802F3FE8 is the menu-display rodata block; WinRootVecs (win_root.h) names only
// its first six Vecs (v0..v5). winMsgDisp reads later Vecs at fixed offsets, reached
// here by raw offset from the named base. Offset 0x5d0 is the str_802F45B8 string.
#define WMD_VEC(off) (*(Vec*)((u8*)&str_802F3FE8 + (off)))

// The message/icon/text logic matches instruction-for-instruction through the waku quad,
// the icon block and the 0x140 message block: the FP registers are coloured like the
// original (f28=x, f29=y, f30=dy, f31=dx, f27=dy2, f26=width) and the frame is 0x620
// (the sprintf buffer is 0x404 bytes so the int->float scratch lands at 0x5a8). Two things
// still differ. (1) The `(f32)FontGetMessageWidth` int->float conversion pools its magic
// double anonymously (@NNN@) and so needs its own `lis`, which steals r3 and shifts the
// integer colours; the original reaches the same value as `lfd f4, 0x4e0(r31)` from the
// str_802F3FE8 base — that offset is double_to_int_mask_802f44c8. Defining a same-valued
// `const f64` does not help (MWCC still emits a fresh copy), and defining the whole
// .rodata block does not either. (2) The by-value Vec/GXColor argument copies for the
// winTexSet / winIconSet / winFontSet* calls land in different stack slots: the original
// gives each call its own block descending from 0x198 (pos at 0x198, 0x174, 0x150, ...;
// scale 0x18 below; colour 0x2c, 0x28, ...), ours pack all the scales together and all
// the positions together. The score is insensitive to declaration/assignment order.
// NONMATCHING (64.98%): see the notes above.
void winMsgDisp(WindowWork* work, f32 x, f32 y) {
    u8* vp = (u8*)&str_802F3FE8;
    GXColor iconColor = dat_8041cf28;
    GXColor textColor = dat_8041cf2c;
    GXColor wakuColor = dat_8041cf30;
    char buf[0x404];
    f32 dy;
    f32 dx;
    f32 dy2;
    Vec s_tex;
    Vec s_icon;
    Vec s_text;
    Vec s_label;
    Vec s_sel0;
    Vec s_sel1;
    Vec s_sel2;
    Vec s_sel3;
    Vec s_arrow;

    dx = x - float_250_8041d1a4 + float_10_8041d1a0 - float_10_8041d1a0;
    dy = float_40_8041d0e8 + y;
    windowDispGX_Waku_col(0, wakuColor, dx, dy, float_500_8041d1a8, float_80_8041d0b0,
                          float_20_8041d0c0);

    if (WW_S32(work, 0x138) != 0 && WW_S32(work, 0x138) > 0) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        s_tex = *(Vec*)(vp + 0xa8);
        winMsgDispTex(winMsgDispVec((Vec*)(vp + 0x9c), dx, dy), &s_tex, iconColor);
        winIconInit();
        s_icon = *(Vec*)(vp + 0xc0);
        winMsgDispIcon(*(u16*)((u8*)&itemDataTable[WW_S32(work, 0x138)] + 0x20),
                       winMsgDispVec((Vec*)(vp + 0xb4), dx, dy), &s_icon,
                       iconColor);
    }

    if (WW_PTR(work, 0x140) != NULL) {
        const char* msg;
        f32 width;

        dx = (WW_S32(work, 0x138) == -2) ? float_neg130_8041d1ac : float_0_8041d068;
        msg = msgSearch((const char*)WW_PTR(work, 0x140));
        width = (f32)FontGetMessageWidth(msg);
        dy2 = float_16_8041d0e4 + dy + float_8_8041d0a0;
        winNameGX(work, 1, x - float_120_8041d09c + dx, dy2, float_240_8041d0ac,
                  float_32_8041d0a4);
        winFontInit();
        winMsgDispEdge(winMsgDispVec((Vec*)(vp + 0xcc),
                                     dx + ((float_240_8041d0ac - width) * float_0p5_8041d06c +
                                            (x - float_120_8041d09c)),
                                     dy2 - float_4_8041d07c),
                       (Vec*)(vp + 0xd8), iconColor, msg);
    }

    if (WW_PTR(work, 0x13c) != NULL) {
        f32 ty = dy - float_14_8041d180;

        GXSetScissor(0, (u32)(float_240_8041d0ac - ty), 0x260, 0x38);
        if (strcmp((const char*)WW_PTR(work, 0x13c),
                   (const char*)(vp + 0x5d0)) == 0) {
            s16 num = *(s16*)((u8*)lotteryGetPtr() + 0x20);

            sprintf(buf, (const char*)msgSearch((const char*)WW_PTR(work, 0x13c)), num);
            winFontInit();
            s_text = *(Vec*)(vp + 0xf0);
            winMsgDispText(winMsgDispVec((Vec*)(vp + 0xe4),
                                         x - float_250_8041d1a4 + float_40_8041d0e8 - float_10_8041d1a0,
                                         ty + (f32)(WW_S32(work, 0x148) * 0x1c)),
                           &s_text, textColor, buf);
        } else {
            winFontInit();
            s_label = *(Vec*)(vp + 0x108);
            winMsgDispLabel(winMsgDispVec((Vec*)(vp + 0xfc),
                                          x - float_250_8041d1a4 + float_40_8041d0e8 - float_10_8041d1a0,
                                          ty + (f32)(WW_S32(work, 0x148) * 0x1c)),
                            &s_label, textColor,
                            (const char*)WW_PTR(work, 0x13c));
        }
        GXSetScissor(0, 0, 0x260, 0x1e0);
    }

    if (WW_S32(work, 0x14c) > 2) {
        s32 sel = WW_S32(work, 0x148);
        s32 num = WW_S32(work, 0x14c);

        winIconInit();
        if (sel == 0) {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                s_sel0 = *(Vec*)(vp + 0x120);
                winMsgDispIcon(0x1b6,
                               winMsgDispVec((Vec*)(vp + 0x114),
                                             float_250_8041d1a4 + x - float_10_8041d1a0,
                                             y - float_24_8041d0c8),
                               &s_sel0, dat_8041cf34);
            }
        } else if (sel / 2 == (num + 1) / 2 - 1) {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                s_sel1 = *(Vec*)(vp + 0x138);
                winMsgDispIcon(0x1b5,
                               winMsgDispVec((Vec*)(vp + 0x12c),
                                             float_250_8041d1a4 + x - float_10_8041d1a0,
                                             y + float_24_8041d0c8),
                               &s_sel1, dat_8041cf38);
            }
        } else {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                s_sel2 = *(Vec*)(vp + 0x150);
                winMsgDispIcon(0x1b5,
                               winMsgDispVec((Vec*)(vp + 0x144),
                                             float_250_8041d1a4 + x - float_10_8041d1a0,
                                             y + float_24_8041d0c8),
                               &s_sel2, dat_8041cf3c);
                s_sel3 = *(Vec*)(vp + 0x168);
                winMsgDispIcon(0x1b6,
                               winMsgDispVec((Vec*)(vp + 0x15c),
                                             float_250_8041d1a4 + x - float_10_8041d1a0,
                                             y - float_24_8041d0c8),
                               &s_sel3, dat_8041cf40);
            }
        }
        s_arrow = *(Vec*)(vp + 0x180);
        winMsgDispIcon(0x70,
                       winMsgDispVec((Vec*)(vp + 0x174),
                                     float_250_8041d1a4 + x - float_10_8041d1a0,
                                     y),
                       &s_arrow, dat_8041cf44);
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
                WW_F32(p, 0xC4) = float_800_8041d1d8;
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 1: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, float_800_8041d1d8, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 10:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 11: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, float_0_8041d068, float_800_8041d1d8);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
            case 20:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 21: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_640_8041d1dc, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 30:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 31: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_neg640_8041d1e0, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 40:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 41: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_0_8041d068, float_neg640_8041d1e0);
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

// bubble sort of the 0xc-byte records at work+0x404. Writing the swap as a
// `SortRec` (3-word) copy `t = *pa; *pa = *pb; *pb = t;` reproduces the original's
// interleaved load/store sequence and its stack spills at 0x14/0x18/0x1c exactly.
// NONMATCHING (96.69%): the instruction sequence is identical; only MWCC's
// register coloring differs (the original keeps loop counters i/j in r5/r6 and the
// table base in r11, ours uses r10/r11 and r9; the temps shift with them).
void sort_4_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 0xC;

            while (j < WW_S32(work, 0xD64)) {
                s32 swap = 0;
                ItemData* da = &itemDataTable[(s16)*(u32*)(pa + 0x40C)];
                ItemData* db = &itemDataTable[(s16)*(u32*)(pb + 0x40C)];

                if (da->bp_cost > db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order > db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    SortRec t = *(SortRec*)(pa + 0x404);
                    *(SortRec*)(pa + 0x404) = *(SortRec*)(pb + 0x404);
                    *(SortRec*)(pb + 0x404) = t;
                }
                pb += 0xC;
                j++;
            }
            pa += 0xC;
        }
    } else {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 0xC;

            while (j < WW_S32(work, 0xD64)) {
                s32 swap = 0;
                ItemData* da = &itemDataTable[(s16)*(u32*)(pa + 0x40C)];
                ItemData* db = &itemDataTable[(s16)*(u32*)(pb + 0x40C)];

                if (da->bp_cost < db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order < db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    SortRec t = *(SortRec*)(pa + 0x404);
                    *(SortRec*)(pa + 0x404) = *(SortRec*)(pb + 0x404);
                    *(SortRec*)(pb + 0x404) = t;
                }
                pb += 0xC;
                j++;
            }
            pa += 0xC;
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

// NONMATCHING (98.13%): same bubble sort as sort_4_3 but over the s16 array at
// work+0xeb8 (index +0xF0). Loops/branches match; only MWCC's register assignment
// differs (original: i=r12, j=r31, pb=r6; ours: i=r6, j=r8, pb=r9).
void sort_5_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 2;

            while (j < WW_S32(work, 0xE94)) {
                s16 la = *(s16*)(pa + 0xEB8);
                s16 lb = *(s16*)(pb + 0xEB8);
                ItemData* da = &itemDataTable[(s16)((s32)la + 0xF0)];
                ItemData* db = &itemDataTable[(s16)((s32)lb + 0xF0)];
                s32 swap = 0;

                if (da->bp_cost > db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order > db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(pa + 0xEB8) = lb;
                    *(s16*)(pb + 0xEB8) = la;
                }
                pb += 2;
                j++;
            }
            pa += 2;
        }
    } else {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 2;

            while (j < WW_S32(work, 0xE94)) {
                s16 la = *(s16*)(pa + 0xEB8);
                s16 lb = *(s16*)(pb + 0xEB8);
                ItemData* da = &itemDataTable[(s16)((s32)la + 0xF0)];
                ItemData* db = &itemDataTable[(s16)((s32)lb + 0xF0)];
                s32 swap = 0;

                if (da->bp_cost < db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order < db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(pa + 0xEB8) = lb;
                    *(s16*)(pb + 0xEB8) = la;
                }
                pb += 2;
                j++;
            }
            pa += 2;
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

// Picks the row colour. Keeping this as an inline helper (rather than the same
// if/else written at the call site) is what makes MWCC home the loop colour in the
// original's 0x8 slot.
static inline GXColor root_color(WindowWork* work, s32 j) {
    if (WW_S32(work, 0x40) == j) {
        return dat_8041cee0;
    }
    return dat_8041cee4;
}

// Builds winTexSet's by-value Vec argument at the call site. Taking the base Vec
// by pointer and doing `v = *base; v.x = x; v.y = y` reproduces the original's
// `translate = vp->vN; translate.x = ..; translate.y = ..` cadence (it loads
// base.x/y/z), while returning it inline keeps the argument evaluation order.
// Without this the loop/tail temporaries and by-value copies land in different
// stack slots. A named local in the caller scores 63%.
static inline Vec root_vec(const Vec* base, f32 x, f32 y) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    return v;
}

// First loop, switch and inter-call sequence match, as do the second loop's and the
// trailing winTexSet calls' instructions in order. `root_vec` took it from 63.77 to
// ~98%. Still differs in the bgcolor load (ours hoists it two instructions earlier)
// and the stack slots MWCC picks for the loop/tail by-value argument copies.
// NONMATCHING (97.74%): bgcolor load scheduling and by-value argument-copy stack slots.
void winRootDisp(CameraId camId, WindowWork* work) {
    WinRootVecs* vp = &str_802F3FE8;
    GXColor bgcolor = dat_8041ced8;
    s32 i;
    s32 j;
    u8* p;

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

    p = (u8*)work;
    for (j = 0; j < WW_S32(work, 0x44); j++) {
        GXColor color;

        color = root_color(work, j);

        winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        winTexSet_x2(WW_S32(p, 0x48) + 1, 0xB2,
                     root_vec(&vp->v4, WW_F32(p, 0x4C), WW_F32(p, 0x50)), vp->v5, color);
        p += 0x18;
    }

    winMailDisp(work);
    winMsgDisp(work, WW_F32(work, 0x128), WW_F32(work, 0x12C));
    winSortGX(work);

    if (!(work->flags & 0x8000)) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));

        {
            f32 two = float_2_8041d090;
            winTexSet(0, root_vec(&vp->v0, two + WW_F32(work, 0x150), WW_F32(work, 0x154) - two), vp->v1,
                      dat_8041cedc);
        }

        winTexSet(0, root_vec(&vp->v2, WW_F32(work, 0x150), WW_F32(work, 0x154)), vp->v3, bgcolor);
    }
}

// Kage (paper) window quad. The vertex constants are literals so MWCC reloads each
// through f0 into the FIFO as the original does; the same-literal GXSetFog zero lets
// MWCC land it in f1 after saving x. The int->f32 magic double still pools anonymously.
void winKageGX(WindowWork* work, void* color, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx4;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 scaleY;

    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041d014);
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
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(1, *(GXColor*)color);
    PSMTXTrans(mtx1, x, y, w);
    scaleY = h * (float_2_8041d090 * (f32)GXGetTexObjHeight(&texObj));
    PSMTXScale(mtx2, h * (float_2_8041d090 * (f32)GXGetTexObjWidth(&texObj)), scaleY, float_1_8041d070);
    PSMTXRotRad(mtx3, 0x78, float_neg1p3963_8041d0dc);
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

#ifdef MUST_MATCH
// MUST_MATCH: the original reloads each float constant immediately before its volatile
// FIFO store; MWCC otherwise CSEs the constants into f1-f4 and reuses them. Loading
// through a volatile pointer forces the per-store reload (same idiom as party.c).
#define BG_F32(v) (GXWGFifo.f32 = *(volatile f32*)&(v))
#define BG_POS(x, y, z) do { BG_F32(x); BG_F32(y); BG_F32(z); } while (0)
#define BG_TEX(u, v) do { BG_F32(u); BG_F32(v); } while (0)
#else
#define BG_POS(x, y, z) GXPosition3f32(x, y, z)
#define BG_TEX(u, v) GXTexCoord2f32(u, v)
#endif

void winBgGX(WindowWork* work, s32 type, f32 x, f32 y) {
    GXTexObj texObj;
    Mtx mtx;
    f32 scaleY;

    switch (type) {
        case 0:
            GXSetTevColor(1, dat_8041cee8);
            GXSetTevColor(2, dat_8041ceec);
            GXSetTevColor(3, dat_8041cef0);
            break;
        case 1:
            GXSetTevColor(1, dat_8041cef4);
            GXSetTevColor(2, dat_8041cef8);
            GXSetTevColor(3, dat_8041cefc);
            break;
        case 2:
            GXSetTevColor(1, float_neg0p0047648_8041cf00);
            GXSetTevColor(2, dat_8041cf04);
            GXSetTevColor(3, dat_8041cf08);
            break;
        case 3:
            GXSetTevColor(1, dat_8041cf0c);
            GXSetTevColor(2, dat_8041cf10);
            GXSetTevColor(3, dat_8041cf14);
            break;
        case 4:
            GXSetTevColor(1, dat_8041cf18);
            GXSetTevColor(2, dat_8041cf1c);
            GXSetTevColor(3, dat_8041cf20);
            break;
    }

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041cf24);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetTevSwapMode(0, 0, 0);

    PSMTXTrans(mtx, x, float_18_8041d1b8 + y, float_0_8041d068);
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
    GXInitTexObjLOD(&texObj, 0, 0, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = float_320_8041d074 / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, float_570_8041d1bc / (f32)GXGetTexObjWidth(&texObj), scaleY, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXBegin(0x80, 0, 4);
    BG_POS(float_neg285_8041d1c0, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_0_8041d068);
    BG_POS(float_neg285_8041d1c0, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_1_8041d070);
    BG_POS(float_0_8041d068, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_1_8041d070);
    BG_POS(float_0_8041d068, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_0_8041d068);

    GXBegin(0x80, 0, 4);
    BG_POS(float_0_8041d068, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_0_8041d068);
    BG_POS(float_0_8041d068, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_0_8041d068);

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 6);
    GXInitTexObjLOD(&texObj, 0, 0, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = float_288_8041d16c / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, float_570_8041d1bc / (f32)GXGetTexObjWidth(&texObj), scaleY, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXSetTevColorIn(0, 4, 6, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 3);

    GXBegin(0x80, 0, 4);
    BG_POS(float_neg285_8041d1c0, float_144_8041d1d0, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_0_8041d068);
    BG_POS(float_neg285_8041d1c0, float_neg144_8041d1d4, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_neg144_8041d1d4, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_144_8041d1d0, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_0_8041d068);
}
