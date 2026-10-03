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
extern const GXColor dat_8041cf4c;
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
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))


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

const GXColor dat_8041cf90 = {0xFF, 0xFF, 0xFF, 0xFF};
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

// One name-plate half: a textured quad from (x,y) to (x+w,y-h), with the texture
// stretched across it. The scales are f64 because the original uses the double
// __fabs() (not the fabsf() of windowdrv.c); the (f32) conversion happens at the
// division, matching the target's frsp placement.
static inline void winNameQuad(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041d070);
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

extern const GXColor dat_8041d018;
extern const GXColor dat_8041d01c;
extern const GXColor dat_8041d020;
extern const GXColor dat_8041d024;
extern const GXColor dat_8041d028;
extern const GXColor dat_8041d02c;
extern const GXColor dat_8041d030;
extern const GXColor dat_8041d034;
extern const GXColor dat_8041d038;
extern const GXColor dat_8041d03c;
extern const GXColor dat_8041d040;
extern const GXColor dat_8041d044;
extern const GXColor dat_8041d048;
extern const GXColor dat_8041d04c;
extern const GXColor dat_8041d050;
extern const GXColor dat_8041d054;
extern const GXColor dat_8041d058;
extern const GXColor dat_8041d05c;
extern const GXColor dat_8041d060;
extern const f32 float_neg240_8041d078;
extern const f32 float_4_8041d07c;
extern const f32 float_208_8041d080;
extern const f32 float_56_8041d084;
extern const f32 float_72_8041d088;
extern const f32 float_3_8041d08c;
extern const f32 float_88_8041d094;
extern const f32 float_104_8041d098;
extern const f32 float_120_8041d09c;
extern const f32 float_8_8041d0a0;
extern const f32 float_32_8041d0a4;
extern const f32 float_304_8041d0a8;
extern const f32 float_240_8041d0ac;
extern const f32 float_80_8041d0b0;
extern const f32 float_210_8041d0b4;
extern const f32 float_7_8041d0b8;
extern const f32 float_360_8041d0bc;
extern const f32 float_20_8041d0c0;
extern const f32 float_230_8041d0c4;
extern const f32 float_24_8041d0c8;

static inline Vec mail_vec(const Vec* base, f32 x, f32 y) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    return v;
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
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, (int (*)(void*, void*))compare_func4);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, (int (*)(void*, void*))compare_func4_r);
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
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, (int (*)(void*, void*))compare_func2);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, (int (*)(void*, void*))compare_func2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

// The loop both comparators run: find `tag` in the 26-entry table, formatting each
// candidate into `buf`. Written as an inlined helper because inlining it twice is
// what reproduces the target's callee-saved register allocation (table base/pointer
// in r26/r27, tag/counter in r28/r29).
static inline s32 winRootFindTag(const char* const* table, const char* tag, u8* buf) {
    s32 i;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf, str_802F4510, table[i]);
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
int compare_func2_r(const void* a, const void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(((u8*)a)[1]);
    TattleEntry* eb = battleGetUnitMonosiriPtr(((u8*)b)[1]);
    s32 i = winRootFindTag(tbl, ea->locationTag, buf_a);
    s32 j = winRootFindTag(tbl, eb->locationTag, buf_b);

    if (i < j) {
        return 1;
    } else if (i > j) {
        return -1;
    } else {
        return 0;
    }
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
int compare_func2(const void* a, const void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(((u8*)a)[1]);
    TattleEntry* eb = battleGetUnitMonosiriPtr(((u8*)b)[1]);
    s32 i = winRootFindTag(tbl, ea->locationTag, buf_a);
    s32 j = winRootFindTag(tbl, eb->locationTag, buf_b);

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
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, (int (*)(void*, void*))compare_func5_1);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, (int (*)(void*, void*))compare_func5_1_r);
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
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, (int (*)(void*, void*))N_compare_func4_1);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, (int (*)(void*, void*))N_compare_func4_1_r);
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


// winWazaGX's copy of winNameQuad. The only difference from winNameQuad is the
// nested `tw` local around the width division: with the division inlined MWCC gives
// winWazaGX's callee-saved FPRs a different colouring (w/w75/h in f26/f27/f17
// instead of retail's f22/f23/f31) and the whole function cascades. Naming the
// texture width first makes MWCC colour it exactly as retail. No other function uses
// this copy, so winNameQuad stays untouched for the rest of the unit.
static inline void wazaQuad(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
#ifdef MUST_MATCH
    // The named width temporary is what reproduces retail's FPR colouring; the
    // natural inlined form is under #else.
    { f32 tw = (f32)GXGetTexObjWidth(&texObj);
    PSMTXScale(mtx, (f32)sx / tw, syf, float_1_8041d070); }
#else
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041d070);
#endif
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

// The waza window frame: eleven textured quads (a 3x3 border plus inner strips),
// each an inlined wazaQuad. They come in three rows: the top bar at y (height
// 32), the body at y-32 (height h-48), and the bottom bar at y-h+16 (height 8/16).
//
// Every instruction, register and stack slot now matches retail. The only `!!`
// rows left are the anonymous compiler-pooled double constants (`lfd r31+0x180`
// vs retail's `r31+0x540` for 32.0, and the i2f magic double `r31+0xe8` vs
// `r31+0x4e0`); those live in the unit's shared .rodata and only settle once the
// other functions in this unit define their own .rodata (retail's base object
// `str_802F3FE8` has ~0x360 more bytes in front of them). They clear on link.
// NONMATCHING (99.97%): anonymous compiler-pool double offsets only (unit-wide .rodata order).
void winWazaGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041cf90);
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

// ---- winSortGX ------------------------------------------------------------
extern const f32 float_240_8041d0ac;
extern const f32 float_304_8041d0a8;
extern const f32 float_16_8041d0e4;
extern const f32 float_20_8041d0c0;
extern const f32 float_24_8041d0c8;
extern const f32 float_128_8041d0ec;
extern const f32 float_3_8041d08c;
extern const f32 float_6_8041d0f8;
extern const f32 float_8_8041d0a0;
extern const GXColor dat_8041d004;
extern const GXColor dat_8041d008;
extern const GXColor dat_8041d00c;
extern const GXColor dat_8041d010;
const Vec vec3_802f42cc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f42d8 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f42e4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f42f0 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f42fc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f4308 = { 1.0f, 1.0f, 1.0f };

static inline Vec winSortMkVec(Vec base, f32 x, f32 y) {
    Vec v = base;
    v.x = x;
    v.y = y;
    return v;
}

// Draws one textured 1x1-uv quad. The original inlines this at every call site,
// which is why each panel gets its own GXTexObj/Mtx stack slots.
static inline void winSortPanel(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f64 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj),
               (f32)sy / (f32)GXGetTexObjHeight(&texObj), float_1_8041d070);
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

// The panels, sort-name tex selection and winTexSet/winTexSet_x2 loop all reproduce the
// target: `h` is passed as f64 so MWCC emits the target's `lfd double_24/40/16; fabs; frsp`
// scale idiom, and `w` as f32 (lfs + fabs + frsp). The six vec3_802f42xx constants are
// defined as unit-local `const Vec` (all {0,0,0}/{1,1,1}, extracted from the target's
// .rodata), which lets MWCC emit the target's rodata-base-relative loads. What is left is
// register colouring and the offsets of the anonymous doubles (MWCC emits ours at
// rodata+0xe8..0x100; the target's str_802F3FE8 blob puts them at +0x4e0..0x500, because
// this unit's other .rodata (str_802F3FE8 and the vec3_802f40xx..42xx the unwritten
// functions need) is not defined here yet).
// NONMATCHING (96.33%): anonymous-double .rodata offsets + a few register-colouring diffs.
void winSortGX(WindowWork* work) {
    f32 x = WW_F32(work, 0x164);
    f32 y = WW_F32(work, 0x168);
    s32 i;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041d004);
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
        GXSetScissor((u32)(float_16_8041d0e4 +
                           (float_16_8041d0e4 + (float_304_8041d0a8 + WW_F32(work, 0x244)))),
                     (u32)(float_40_8041d0e8 + (float_240_8041d0ac - WW_F32(work, 0x24C))), 0x200,
                     0xC0);
    }

    winSortPanel(work, 0xA5, x, y, float_128_8041d0ec, 24.0);
    winSortPanel(work, 0xA3, x + float_128_8041d0ec, y, 24.0f, 24.0);

    for (i = 0; i < WW_S32(work, 0x178); i++) {
        winSortPanel(work, 0xA6, x + 136.0f, y - float_24_8041d0c8 - (f32)(i * 0x28), 16.0f,
                     40.0);
    }

    winSortPanel(work, 0xA4, x + float_128_8041d0ec,
                 y - float_24_8041d0c8 - (f32)(WW_S32(work, 0x178) * 0x28), 24.0f,
                 24.0);
    winSortPanel(work, 0xA7, x, y - float_24_8041d0c8 - (f32)(WW_S32(work, 0x178) * 0x28),
                 float_128_8041d0ec, 24.0);
    winSortPanel(work, 0xA8, x, y - float_24_8041d0c8, 136.0f,
                 (f64)(WW_S32(work, 0x178) * 0x28));

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
            row = y - float_20_8041d0c0 - (f32)(i * 0x28) - float_16_8041d0e4 - float_8_8041d0a0;
            winTexSet(0xB6, winSortMkVec(vec3_802f42cc, x + 83.0f, row), vec3_802f42d8, dat_8041d008);
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            winTexSet_x2(tex, 0xB6, winSortMkVec(vec3_802f42e4, x + 78.0f, float_6_8041d0f8 + row),
                         vec3_802f42f0, dat_8041d00c);
        } else {
            f32 row;

            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            row = y - float_20_8041d0c0 - (f32)(i * 0x28) - float_16_8041d0e4 - float_8_8041d0a0;
            winTexSet_x2(tex, 0xB6, winSortMkVec(vec3_802f42fc, x + 84.0f, row), vec3_802f4308,
                         dat_8041d010);
        }
    }

    if (WW_S32(work, 0x184) == 7) {
        GXSetScissor(0, 0, 0x260, 0x1E0);
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
