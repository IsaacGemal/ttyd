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
#include "pmario_sound.h"
#include "statuswindow.h"

extern ItemData itemDataTable[];
// .sdata
u16 party_id_table[8] = { 1, 2, 5, 4, 6, 3, 7, 0 };

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
f32 float_neg144_80421aa8 = -144.0f;
f32 float_0p6_80421a8c = 0.6f;
f32 float_30_80421ab0 = 30.0f;
f32 float_255_80421ab4 = 255.0f;
f32 float_0p017453_80421ab8 = 0.017453292f;
f32 float_20_80421abc = 20.0f;
f32 float_150_80421ac0 = 150.0f;
f32 float_8_80421ac4 = 8.0f;
f32 float_100_80421ac8 = 100.0f;
f32 float_120_80421acc = 120.0f;
f32 float_85_80421ad0 = 85.0f;

// .rodata
const Vec vec3_802ffa98 = { 0, 0, 0 };
const Vec vec3_802ffaa4 = { 0.5, 0.5, 0.5 };
const Vec vec3_802ffab0 = { 0, 0, 0 };
const Vec vec3_802ffabc = { 1, 1, 1 };
const Vec vec3_802ffac8 = { 0, 0, 0 };
const Vec vec3_802ffad4 = { 1, 1, 1 };
const Vec vec3_802ffae0 = { 0, 0, 0 };
const Vec vec3_802ffaec = { 0.15, 0.15, 0.15 };
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
const f64 double_to_int_802fff00 = 4503601774854144.0;
const char str_802FFF08[] = "msg_window_title_9\000";
const char str_802FFF1C[] = "msg_window_title_10";
const char str_802FFF30[] = "msg_window_title_7\000";
const char str_802FFF44[] = "msg_window_title_8\000";
const f64 double_to_int_mask_802fff58 = 4503599627370496.0;
const char str_802FFF60[] = "msg_window_title_2\000";
const char str_802FFF74[] = "msg_window_title_3\000";
const char str_802FFF88[] = "msg_window_title_4\000";
const char str_802FFF9C[] = "msg_window_title_1\000";
const char str_802FFFB0[] = "%s_%04d";
const char str_802FFFB8[] = "jperfect_title_%04d";
const char str_802FFFCC[] = "msg_lottery_number\000";

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

//.sdata
// .sdata2
char str_804219B8[] = "_kur";
char str_804219C0[] = "_nok";
char str_804219C8[] = "_win";
char str_804219D0[] = "_yos";
char str_804219D8[] = "_viv";
char str_804219E0[] = "_bom";
char str_804219E8[] = "_chu";
char str_804219F0[] = "_hp";
char str_804219F4[] = "_fp";
char str_804219F8[] = "_bp";
char str_80421A78[] = "help";

//.sdata
static char* party_labelname_table[7] = { str_804219B8, str_804219C0, str_804219C8, str_804219D0, str_804219D8, str_804219E0, str_804219E8 };
static char* mario_status_labelname_table[3] = { str_804219F0, str_804219F4, str_804219F8 };

//.data
WinMgrSelectTbl select_desc_tbl[] = { { 3, NULL } };

//.sbss
static WinMgrWork work;
//.sdata
static WinMgrWork* wp = &work;

// local prototypes

void winMgrInit(void) {
    wp->mNumEntries = 32;
    wp->mEntries = (WinMgrEntry*)__memAlloc(HEAP_DEFAULT, wp->mNumEntries * sizeof(WinMgrEntry));
    memset(wp->mEntries, 0, wp->mNumEntries * sizeof(WinMgrEntry));
}

void winMgrReInit(void) {
    memset(wp->mEntries, 0, wp->mNumEntries * sizeof(WinMgrEntry));
}

// TODO: finish
// NONMATCHING (98.56%): 3 missing, plus register differences.
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

// NONMATCHING (96.63%): 2 instructions differ, 3 missing, 3 extra, plus register differences.
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
// NONMATCHING (0%): just written.
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
        FontDrawMessage(-0xd2, (s32)(28.0f * entry->field_0x38 + float_neg144_80421aa8), buf);
    } else {
        FontDrawStart();
        FontDrawMessage(-0xd2, (s32)(28.0f * entry->field_0x38 + float_neg144_80421aa8), (const char*)entry->field_0x34);
    }
    GXSetScissor(0, 0, 0x260, 0x1e0);
    if (entry->field_0x3C > 2) {
        if (entry->field_0x38 == 0) {
            if ((gp->retraceCount & 0x1F) < 20) {
                PSMTXTrans(mtx1, (f32)(entry->field_0x18 + entry->field_0x20 - 0xC),
                           (f32)(entry->field_0x1C - entry->field_0x24 - 4), 0.0f);
                PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
                PSMTXConcat(mtx1, mtx2, mtx1);
                iconDispGx2(mtx1, 0x10, 0x1B6);
            }
        } else if (entry->field_0x38 + 2 == entry->field_0x3C) {
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

s32 unk_80237448(WinMgrSelect* select) {
    return select->field_0xC;
}

// NONMATCHING (82.06%): 2 missing, 1 extra, plus register differences.
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

// NONMATCHING (97.81%): same instructions; only register numbers or operands differ.
void select_disp3(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        u16 id = items[select->field_0xC].field_0x2;

        entry->field_0x34 = (s32)msgSearch(itemDataTable[id].description);
        winMgrHelpDraw(entry);
    }
}

// NONMATCHING (10.64%): 5 instructions differ, 829 missing, 11 extra, plus register differences.
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

// NONMATCHING (0%): just written.
void select_main(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    s32 newIndex;
    s32 off;
    s32 i;

    switch ((*(s32*)select->field_0x8)) {
    case 0:
        for (i = 0; i < select->numDescriptors; i++) {
            WinMgrEntry* e = &wp->mEntries[select->descriptorId[i]];

            if (e->flags & 1) {
                e->field_0x4 = 1;
                e->field_0x8 = 0;
                e->field_0x38 = 0;
                e->field_0x3C = 0;
                e->field_0x34 = 0;
            }
        }
        if (select->numDescriptors >= 3) {
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
        select->field_0x1C = select->field_0x1C + (24.0f * select->field_0x10 - select->field_0x1C) / 6.0f;
        select->field_0x14 = select->field_0x14 + ((f32)entry->field_0x18 - select->field_0x14) / 6.0f;
        select->field_0x18 = select->field_0x18 +
            ((f32)(entry->field_0x1C - 54) - 24.0f * (f32)(select->field_0xC - select->field_0x10) -
             select->field_0x18) / 6.0f;
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
    }
}

// NONMATCHING (0%): just written.
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

// NONMATCHING (0%): just written.
s32 unk_802399f8(EventEntry* event) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    s32 i;
    s32 count = 0;
    s32 zeroFlag = 0;
    s32 nonZeroFlag = 0;
    s32 mode;
    s32 val;

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

// NONMATCHING (0%): just written.
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

// NONMATCHING (0%): just written.
void select_disp3_mario(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    PouchData* pouch;

    pouch = pouchGetPtr();
    (void)pouch;
    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        u16 flag = items[select->field_0xC].field_0x0;
        const char* help;
        char buf[0x48];

        if (flag & 2) {
            if (flag & 4) {
                help = str_802FFD80;
            } else {
                help = str_802FFD8C;
            }
        } else {
            help = str_80421A78;
        }
        if (select->field_0x4 == 9) {
            sprintf(buf, str_802FFD94, help, mario_status_labelname_table[items[select->field_0xC].field_0x2]);
        } else {
            sprintf(buf, str_802FFDA4, help, mario_status_labelname_table[items[select->field_0xC].field_0x2]);
        }
        entry->field_0x34 = (s32)msgSearch(buf);
        winMgrHelpDraw(entry);
    }
}

// NONMATCHING (0%): just written.
void select_disp3_party(WinMgrEntry* entry) {
    WinMgrSelect* select = (WinMgrSelect*)entry->param;
    PouchData* pouch = pouchGetPtr();
    u16 id;

    if (!(wp->mEntries[select->descriptorId[2]].flags & 4)) {
        WinMgrSelectItem* items = (WinMgrSelectItem*)select->field_0x30;
        const char* help = str_80421A78;
        const char* lank;
        char buf[0x48];

        if (items[select->field_0xC].field_0x0 & 2) {
            help = str_802FFD8C;
        }
        if (select->field_0x4 == 6 || select->field_0x4 == 7) {
            lank = str_802FFDB4;
        } else {
            lank = str_802FFDBC;
        }
        id = items[select->field_0xC].field_0x2;
        sprintf(buf, str_802FFDC8, help, party_labelname_table[id],
                pouch->partyData[party_id_table[id]].techLevel + 1, lank);
        entry->field_0x34 = (s32)msgSearch(buf);
        winMgrHelpDraw(entry);
    }
}
