#include "manager/fontmgr.h"
#include "manager/winmgr.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "memory.h"
#include "system.h"
#include "data/item_data.h"
#include <string.h>

extern ItemData itemDataTable[];
extern s32 partyChkJoin(s32 id);
extern u16 party_id_table[];
extern const char* msgSearch(const char* tag);
extern void windowDispGX2_Waku_col(Mtx mtx, s16 gxTexMapID, GXColor color, f32 x, f32 y, f32 width, f32 height, f32 curve);

// .sbss2
GXColor unk_80423330;

// .sdata2
GXColor dat_80421a14 = { 0x2b, 0xa4, 0xff, 0x00 };
GXColor dat_80421a18 = { 0x2b, 0xa4, 0xff, 0x00 };
GXColor dat_80421a1c = { 0x2b, 0xa4, 0xff, 0x00 };
GXColor dat_80421a20 = { 0x2b, 0xa4, 0xff, 0x00 };
GXColor dat_80421a24 = { 0x2b, 0xa4, 0xff, 0x00 };

// .sdata2
f32 float_0_80421a80 = 0.0f;
f32 float_1_80421a90 = 1.0f;
f32 float_28_80421aac = 28.0f;
f32 float_30_80421ab0 = 30.0f;
f32 float_255_80421ab4 = 255.0f;
f32 float_0p017453_80421ab8 = 0.017453292f;
f32 float_20_80421abc = 20.0f;
f32 float_150_80421ac0 = 150.0f;
f32 float_8_80421ac4 = 8.0f;
f32 float_100_80421ac8 = 100.0f;
f32 float_120_80421acc = 120.0f;
f32 float_85_80421ad0 = 85.0f;

#define PI 3.14159265358979323846264338327950288419716939937510

typedef struct WinMgrSelectTbl {
    s16 numDescriptors; // 0x0
    // will pad 2 bytes
    WinMgrDesc* descriptors; // 0x4
} WinMgrSelectTbl;

typedef struct WinMgrSelectItem {
    u16 field_0x0; // 0x0
    u16 field_0x2; // 0x2
} WinMgrSelectItem;

//.data
WinMgrSelectTbl select_desc_tbl[] = { { 3, NULL } };

//.sbss
static WinMgrWork work;
//.sdata
static WinMgrWork* wp = &work;

// local prototypes
void winMgrDisp(CameraId camId, void* param);
void winMgrSeq(WinMgrEntry* entry);

void winMgrInit(void) {
    wp->mNumEntries = 32;
    wp->mEntries = (WinMgrEntry*)__memAlloc(HEAP_DEFAULT, wp->mNumEntries * sizeof(WinMgrEntry));
    memset(wp->mEntries, 0, wp->mNumEntries * sizeof(WinMgrEntry));
}

void winMgrReInit(void) {
    memset(wp->mEntries, 0, wp->mNumEntries * sizeof(WinMgrEntry));
}

// TODO: finish
void winMgrDisp(CameraId camId, void* param) {
    Mtx rotmtx, scalemtx;
    WinMgrEntry* entry = (WinMgrEntry*)param;
    GXColor color;

    PSMTXRotRad(rotmtx, 'z', entry->rotation * 0.017453292f);
    PSMTXScale(scalemtx, entry->scale, entry->scale, entry->scale);
    PSMTXConcat(rotmtx, scalemtx, scalemtx);

    color = unk_80423330;
    color.r = entry->description->field_0x1C[0];
    color.g = entry->description->field_0x1C[1];
    color.b = entry->description->field_0x1C[2];
    color.a = *(s32*)&entry->field_0xC;
    windowDispGX2_Waku_col(scalemtx, 0, color, (f32)entry->field_0x18, (f32)entry->field_0x1C,
                           (f32)entry->field_0x20, (f32)entry->field_0x24, 20.0f);

    switch (*(s32*)(entry->description->field_0x0 + 4)) {
    case 1:
        color = dat_80421a14;
        color.a = *(s32*)&entry->field_0xC;
        windowDispGX2_Waku_col(scalemtx, 0, color, (f32)(entry->field_0x18 + (entry->field_0x20 - 150) / 2),
                               (f32)(entry->field_0x1C + 14), 150.0f, 28.0f, 8.0f);
        break;
    case 3:
        color = dat_80421a18;
        color.a = *(s32*)&entry->field_0xC;
        windowDispGX2_Waku_col(scalemtx, 0, color, (f32)(entry->field_0x18 + 10), (f32)(entry->field_0x1C + 14),
                               100.0f, 28.0f, 8.0f);
        color = dat_80421a1c;
        color.a = *(s32*)&entry->field_0xC;
        windowDispGX2_Waku_col(scalemtx, 0, color, (f32)(entry->field_0x18 + entry->field_0x20 / 2 - 10),
                               (f32)(entry->field_0x1C + 14), 100.0f, 28.0f, 8.0f);
        break;
    case 4:
    case 5:
        color = dat_80421a20;
        color.a = *(s32*)&entry->field_0xC;
        windowDispGX2_Waku_col(scalemtx, 0, color,
                               (f32)(entry->field_0x18 + (entry->field_0x20 * 3 / 5 - 120) * 2 / 3),
                               (f32)(entry->field_0x1C + 14), 120.0f, 28.0f, 8.0f);
        color = dat_80421a24;
        color.a = *(s32*)&entry->field_0xC;
        windowDispGX2_Waku_col(scalemtx, 0, color,
                               (f32)(entry->field_0x18 + entry->field_0x20 * 3 / 5 + (entry->field_0x20 * 2 / 5 - 85) / 3),
                               (f32)(entry->field_0x1C + 14), 85.0f, 28.0f, 8.0f);
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

// Keep winMgrHelpDraw out of line: the original calls it rather than inlining.
#pragma dont_inline on
void winMgrHelpDraw(WinMgrEntry* entry) {
}
#pragma dont_inline reset

s32 unk_80237448(WinMgrSelect* select) {
    return select->field_0xC;
}

void winMgrSelectDelete(s32 select) {
    __memFree(HEAP_DEFAULT, (void*)((WinMgrSelect*)select)->field_0x30);
    __memFree(HEAP_DEFAULT, (void*)select);
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

s32 unk_802379e4(void* a, void* b) {
    s32 va = itemDataTable[((WinMgrSelectItem*)a)->field_0x2].type_sort_order;
    s32 vb = itemDataTable[((WinMgrSelectItem*)b)->field_0x2].type_sort_order;

    if (va > vb) {
        return 1;
    }
    return va < vb ? -1 : 0;
}

s32 unk_80237a2c(void* a, void* b) {
    u32 va = itemDataTable[((WinMgrSelectItem*)a)->field_0x2].star_piece_price;
    u32 vb = itemDataTable[((WinMgrSelectItem*)b)->field_0x2].star_piece_price;

    if (va > vb) {
        return 1;
    }
    return va < vb ? -1 : 0;
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

void select_disp3(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        u16 id = items[select->field_0xC].field_0x2;

        entry->field_0x34 = (s32)msgSearch(itemDataTable[id].description);
        winMgrHelpDraw(entry);
    }
}

WinMgrSelect* winMgrSelectEntry(s32 a1, s32 a2, BOOL a3) {
    WinMgrSelectTbl* table;
    WinMgrSelect* select;
    // WinMgrDesc* desc;
    PouchData* pouch;
    s32 descId, i, id;

    pouch = pouchGetPtr();
    select = (WinMgrSelect*)__memAlloc(HEAP_DEFAULT, sizeof(WinMgrSelect));
    memset(select, 0, sizeof(WinMgrSelect));

    select->field_0x4 = a1;

    select->field_0xC = 0;
    descId = a1;
    select->field_0x10 = 0;

    select->field_0x18 = 0.0f;
    select->field_0x14 = 0.0f;
    select->field_0x1C = 0.0f;

    select->field_0x38 = a2;

    if (a3) {
        select->flags |= 0x100;
    }
    if (a1 < 0) {
        descId = 0;
    }
    table = &select_desc_tbl[descId];
    select->numDescriptors = (s32)table->numDescriptors;

    for (i = 0; i < select->numDescriptors; i++) {
        id = winMgrEntry(&table->descriptors[i]);
        select->descriptorId[i] = id;
        winMgrSetParam(id, select);
    }
    select->field_0x14 = (f32)table->descriptors->field_0xC;
    select->field_0x18 = (f32)(table->descriptors->field_0x10 - 54);
    // TODO: finish
    /*switch (select->field_0x4) {

    }





    */
    return select;
}

// qqsort callback, sub_80237A2C
// qqsort callback, sub_802379E4
