#include "pmario_sound.h"
#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
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
extern ItemData itemDataTable[];

static char msgbuf[0x3C];


s32 winSortWait(WindowWork* work) {
    return WW_S32(work, 0x17C) != 0;
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

// NONMATCHING (90.37%): GX quad vertex loop; MWCC CSEs the position constants
// into f1/f2/f3 and reloads f0 only, while the original reloads f0 per vertex.
// Non-const externs made it worse (88.47%).
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
    GXPosition3f32(float_neg500_8041d0d0, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_neg100_8041d0d8, float_0_8041d068);
    GXPosition3f32(float_neg500_8041d0d0, float_neg100_8041d0d8, float_0_8041d068);
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

// NONMATCHING (99.88%): only the pooled int->float magic constant differs
// (double_to_int_802f44d0 vs a local @773@; resolves once the pool is complete).
// The two trailing fmadds now match (0.125 multiply with the constant on the left).
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

    WW_F32(work, 0x164) += float_0p125_8041d104 * (WW_F32(work, 0x16C) - WW_F32(work, 0x164));
    WW_F32(work, 0x168) += float_0p125_8041d104 * (WW_F32(work, 0x170) - WW_F32(work, 0x168));
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

// NONMATCHING (99.27%): 5-page switch over work->0xcc. All bodies match; MWCC's
// sparse-switch comparison tree differs in the [0,10) subtree (original pivots
// on 0 then 2, ours on 1 then 0).
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
            case 100:
                break;
            default:
                break;
        }
        p = (WindowWork*)((u8*)p + 0x14);
    }
}

// NONMATCHING (84.78%): bubble sort of the 0xc-byte records at work+0x404. pb now
// starts as work + j*0xc like the original; still, the original spills the three
// swap temporaries to 0x14/0x18/0x1c and uses different volatile registers.
void sort_4_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 0xC;

            while (j < WW_S32(work, 0xD64)) {
                ItemData* da = &itemDataTable[(s16)*(u32*)(pa + 0x40C)];
                ItemData* db = &itemDataTable[(s16)*(u32*)(pb + 0x40C)];
                s32 swap = 0;

                if (da->bp_cost > db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order > db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    u32 t0 = *(u32*)(pa + 0x404);
                    u32 t1 = *(u32*)(pa + 0x408);
                    u32 t2 = *(u32*)(pa + 0x40C);
                    *(u32*)(pa + 0x404) = *(u32*)(pb + 0x404);
                    *(u32*)(pa + 0x408) = *(u32*)(pb + 0x408);
                    *(u32*)(pa + 0x40C) = *(u32*)(pb + 0x40C);
                    *(u32*)(pb + 0x404) = t0;
                    *(u32*)(pb + 0x408) = t1;
                    *(u32*)(pb + 0x40C) = t2;
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
                ItemData* da = &itemDataTable[(s16)*(u32*)(pa + 0x40C)];
                ItemData* db = &itemDataTable[(s16)*(u32*)(pb + 0x40C)];
                s32 swap = 0;

                if (da->bp_cost < db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order < db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    u32 t0 = *(u32*)(pa + 0x404);
                    u32 t1 = *(u32*)(pa + 0x408);
                    u32 t2 = *(u32*)(pa + 0x40C);
                    *(u32*)(pa + 0x404) = *(u32*)(pb + 0x404);
                    *(u32*)(pa + 0x408) = *(u32*)(pb + 0x408);
                    *(u32*)(pa + 0x40C) = *(u32*)(pb + 0x40C);
                    *(u32*)(pb + 0x404) = t0;
                    *(u32*)(pb + 0x408) = t1;
                    *(u32*)(pb + 0x40C) = t2;
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
