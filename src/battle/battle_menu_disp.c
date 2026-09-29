#include "sdk/texPalette.h"
#include "data/item_data.h"
#include "manager/filemgr.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "driver/windowdrv.h"
#include "battle/battle_menu_disp.h"
#include "system.h"
#include "manager/fontmgr.h"
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"
#include <stdio.h>
#include "math_ext.h"

extern BattleWork* _battleWorkPointer;
extern ItemData itemDataTable[];


// Layout of the battle menu scratch buffer allocated by
// InitSubMenuCommonProcess (0x124 bytes). Only the fields this file touches
// are declared; the rest stay opaque.
typedef struct BattleMenuWork {
    BattleCursor* cursor;   // 0x0
    void* field_0x4;        // 0x4
    void* field_0x8;        // 0x8
    s32 field_0xC;          // 0xC
    s32 field_0x10;         // 0x10
    u8 field_0x14;          // 0x14
    u8 field_0x15[0x18 - 0x15];
    s32 field_0x18;         // 0x18
    s32 field_0x1C;         // 0x1C
    s32 field_0x20[5];      // 0x20
    s32 field_0x34[5];      // 0x34
    s32 field_0x48[10];     // 0x48
    s32 field_0x70[5];      // 0x70
    s32 field_0x84[5];      // 0x84
    f32 field_0x98;         // 0x98
    f32 field_0x9C;         // 0x9C
    f32 field_0xA0;         // 0xA0
    s32 field_0xA4[7];      // 0xA4
    s32 field_0xC0[7];      // 0xC0
    s32 field_0xDC[7];      // 0xDC
    s32 field_0xF8[7];      // 0xF8
    u8 field_0x114[0x11C - 0x114];
    s32 field_0x11C;        // 0x11C
    f32 field_0x120;        // 0x120
} BattleMenuWork;





//.sbss
s8 seq;

//.sdata
GXColor seleItemCoordCol = {0xFF, 0xC0, 0xC0, 0xCF};

typedef struct BattleMenuDispEntry {
    s32 field_0x0;   // 0x0
    s32 field_0x4;   // 0x4
    void* field_0x8; // 0x8
    u16 field_0xC;   // 0xC
    u16 field_0xE;   // 0xE
    s32 field_0x10;  // 0x10
    s32 field_0x14;  // 0x14
} BattleMenuDispEntry; // 0x18

typedef struct BattleMenuDispEntryOp {
    s32 field_0x0;  // 0x0
    s32 field_0x4;  // 0x4
    s32 field_0x8;  // 0x8
    s32 field_0xC;  // 0xC
    u16 field_0x10; // 0x10
    u16 field_0x12; // 0x12
    s32 field_0x14; // 0x14
    s32 field_0x18; // 0x18
} BattleMenuDispEntryOp; // 0x1C

// Per-weapon entries behind BattleMenuWork.field_8 (stride 0x1C).
typedef struct BattleMenuWeaponEntry {
    BattleWeapon* weapon;  // 0x0
    s32 field_0x4;         // 0x4
    s32 field_0x8;         // 0x8
    s32 field_0xC;         // 0xC
    u16 field_0x10;        // 0x10
    u8 field_0x12;         // 0x12
    u8 field_0x13[0x14 - 0x13];
    s32 field_0x14;        // 0x14
    s32 field_0x18;        // 0x18
} BattleMenuWeaponEntry; // 0x1C

extern BattleWeapon ItemWeaponData_CookingItem;

extern Vec vec3_802ef168;
extern Vec vec3_802ef174;
extern Vec vec3_802ef180;
extern Vec vec3_802ef18c;

// Stack scratch block shared by the battle-menu draw workers. The original
// build keeps these arrays in one contiguous frame region; listing them as
// separate locals lets MWCC reorder them, so they are grouped here.
typedef struct BattleMenuDispScratch {
    f32 out0x10;    // 0x0
    f32 out0x14;    // 0x4
    Vec pos;        // 0x8
    u16 arr24[7];   // 0x14
    s32 arr34[7];   // 0x24
    s32 arr50[7];   // 0x40
    s32 arr6C[7];   // 0x5C
    s32 arr88[7];   // 0x78
    f32 scales[5];  // 0x94
    Vec centers[5]; // 0xA8
    Mtx mats[5];    // 0xE4
    Mtx rot[7];     // 0x1D4
} BattleMenuDispScratch;

// The Disp wrappers must emit a real call, not an inline expansion of these.
#pragma dont_inline on
// NONMATCHING (0.00%): 9 instructions differ, 4 missing, 91 extra, plus register differences.
void DrawMultiItemWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuDispEntry* entries;
    f32 outX;
    f32 outY;
    Vec vec;
    u16 arr24[7];
    s32 arr88[7];
    s32 arr6C[7];
    s32 arr50[7];
    s32 arr34[7];
    f32 scales[5];
    Vec centers[5];
    Mtx mats[5];
    Mtx rot[7];
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuDispEntry*)((u8*)wp + 0x1b6c);
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            BattleMenuDispEntry* entry = &entries[idx];
            arr88[i] = entry->field_0x0;
            arr24[i] = entry->field_0xC != 0 ? entry->field_0xC : 0x18B;
            arr6C[i] = entry->field_0x4;
            arr50[i] = entry->field_0x10;
            arr34[i] = 0;
        }
    }
    DrawMainIcon(window, (void*)((u8*)wp + 0x1724), mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, 0, 0);
    vec = vec3_802ef18c;
    DrawMenuCursorAndScrollArrow(&vec);
    if (entries[cursor->field_0].field_0x8 != 0) {
        DrawMenuHelpWin((const char*)entries[cursor->field_0].field_0x8, outX, outY);
    }
}

// NONMATCHING (0.00%): 24 instructions differ, 7 missing, 97 extra, plus register differences.
void DrawChangePartyWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuDispEntry* entries;
    BattleMenuDispScratch scratch;
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuDispEntry*)((u8*)wp + 0x1aac);
    DrawSubMenuCommonProcess(&scratch.out0x14, &scratch.out0x10, scratch.mats, scratch.centers, scratch.scales, scratch.rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            BattleMenuDispEntry* entry = &entries[idx];
            scratch.arr88[i] = entry->field_0x4;
            scratch.arr24[i] = entry->field_0xC != 0 ? entry->field_0xC : 0x18B;
            scratch.arr6C[i] = (s32)entry->field_0x8;
            scratch.arr50[i] = 0;
            scratch.arr34[i] = 0;
        }
    }
    DrawMainIcon(window, (void*)((u8*)wp + 0x1724), scratch.mats, scratch.centers, scratch.scales);
    DrawSubIcon(window, scratch.rot, scratch.arr88, scratch.arr24, scratch.arr6C, scratch.arr50, scratch.arr34, (void*)((u8*)wp + 0x1aac), 1);
    scratch.pos = vec3_802ef180;
    DrawMenuCursorAndScrollArrow(&scratch.pos);
    if (entries[cursor->field_0].field_0x10 != 0) {
        DrawMenuHelpWin((const char*)entries[cursor->field_0].field_0x10, scratch.out0x14, scratch.out0x10);
    }
}

// NONMATCHING (0.00%): 23 instructions differ, 6 missing, 96 extra, plus register differences.
void DrawOperationWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuDispEntryOp* entries;
    BattleMenuDispScratch scratch;
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuDispEntryOp*)((u8*)wp + 0x19e8);
    DrawSubMenuCommonProcess(&scratch.out0x14, &scratch.out0x10, scratch.mats, scratch.centers, scratch.scales, scratch.rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            BattleMenuDispEntryOp* entry = &entries[idx];
            scratch.arr88[i] = entry->field_0x4;
            scratch.arr24[i] = entries[i].field_0x10 != 0 ? entry->field_0x10 : 0x18B;
            scratch.arr6C[i] = entry->field_0xC;
            scratch.arr50[i] = entry->field_0x18;
            scratch.arr34[i] = 0;
        }
    }
    DrawMainIcon(window, (void*)((u8*)wp + 0x1724), scratch.mats, scratch.centers, scratch.scales);
    DrawSubIcon(window, scratch.rot, scratch.arr88, scratch.arr24, scratch.arr6C, scratch.arr50, scratch.arr34, 0, 0);
    scratch.pos = vec3_802ef174;
    DrawMenuCursorAndScrollArrow(&scratch.pos);
    if (entries[cursor->field_0].field_0x14 != 0) {
        DrawMenuHelpWin((const char*)entries[cursor->field_0].field_0x14, scratch.out0x14, scratch.out0x10);
    }
}

// NONMATCHING (30.09%): 8 instructions differ, 9 missing, 90 extra, plus register differences.
void DrawWeaponWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuWeaponEntry* entries;
    BattleUnit* unit;
    BattleWeapon* sel;
    f32 outX;
    f32 outY;
    Vec vec;
    u16 arr24[7];
    s32 arr88[7];
    s32 arr6C[7];
    s32 arr50[7];
    s32 arr34[7];
    f32 scales[5];
    Vec centers[5];
    Mtx mats[5];
    Mtx rot[7];
    s32 i;
    const char* msg;

    unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuWeaponEntry*)window->field_0x8;
    sel = entries[cursor->field_0].weapon;
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            BattleMenuWeaponEntry* entry = &entries[idx];
            arr88[i] = entry->field_0x4;
            arr24[i] = *(u16*)((u8*)wp + 0x171c + idx * 0x1c + 0x8c);
            arr6C[i] = entry->field_0x8;
            if (entry->weapon != 0) {
                arr50[i] = BtlUnit_GetWeaponCost(unit, entry->weapon);
            } else {
                arr50[i] = 0;
            }
            if (entry->weapon != 0) {
                arr34[i] = *(u8*)((u8*)entry->weapon + 0x12);
            } else {
                arr34[i] = 0;
            }
        }
    }
    DrawMainIcon(window, (void*)((u8*)wp + 0x1724), mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, 0, 0);
    vec = vec3_802ef168;
    DrawMenuCursorAndScrollArrow(&vec);
    if (sel != 0) {
        s32 id = *(s32*)((u8*)sel + 0x8);
        if (id != 0) {
            msg = itemDataTable[id].description;
        } else if (sel == &ItemWeaponData_CookingItem) {
            msg = itemDataTable[entries[cursor->field_0].field_0x14].description;
        } else {
            msg = *(const char**)((u8*)sel + 0xC);
        }
    } else {
        msg = itemDataTable[entries[cursor->field_0].field_0x14].description;
    }
    if (msg != 0) {
        DrawMenuHelpWin(msg, outX, outY);
    }
}
#pragma dont_inline reset

void SelectedItemCoordinateColorUpDate(void) {
    switch (seq) {
        case 0:
            if (++seleItemCoordCol.g == 0xFF) {
                seq = 1;
            }
            break;
        case 1:
            if (--seleItemCoordCol.r == 0xC0) {
                seq = 2;
            }
            break;
        case 2:
            if (++seleItemCoordCol.b == 0xFF) {
                seq = 3;
            }
            break;
        case 3:
            if (--seleItemCoordCol.g == 0xC0) {
                seq = 4;
            }
            break;
        case 4:
            if (++seleItemCoordCol.r == 0xFF) {
                seq = 5;
            }
            break;
        case 5:
            if (--seleItemCoordCol.b == 0xC0) {
                seq = 0;
            }
            break;
    }
}

BOOL BattleMenuKeyOKInACT(BattleWork* wp) {
    if (wp->commandMenu.unk540[0x18] == 0 &&
        ((BattleMenuWork*)wp->commandMenu.field_55C)->field_0x14 == 0) {
        return FALSE;
    }
    return TRUE;
}

void BattleMenuDisp_MultiItem_Init(BattleWork* wp, u32 flags) {
    BattleMenuWork* window;
    BattleCursor* cursor;
    InitSubMenuCommonProcess(&window, &cursor, &wp->commandMenu.cursor[12]);
    if (flags & 1) {
        InitSubMenuCommonProcess2(window, cursor);
    } else {
        InitSubMenuCommonProcess3(window, cursor);
    }
}

void BattleMenuDisp_ChangePartySelect_Init(BattleWork* wp, u32 flags) {
    BattleMenuWork* window;
    BattleCursor* cursor;
    InitSubMenuCommonProcess(&window, &cursor, &wp->commandMenu.cursor[10]);
    if (flags & 1) {
        InitSubMenuCommonProcess3(window, cursor);
    } else {
        InitSubMenuCommonProcess2(window, cursor);
    }
}

void BattleMenuDisp_Operation_Init(BattleWork* wp, u32 flags) {
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    BattleMenuWork* window;
    BattleCursor* cursor;
    BattleCursor* menu;

    if (unit->currentKind == 0xDE) {
        menu = &wp->commandMenu.cursor[8];
    } else {
        menu = &wp->commandMenu.cursor[9];
    }
    InitSubMenuCommonProcess(&window, &cursor, menu);
    if (flags & 1) {
        InitSubMenuCommonProcess2(window, cursor);
    } else {
        InitSubMenuCommonProcess3(window, cursor);
    }
}

void BattleMenuDisp_WeaponSelect_Init(BattleWork* wp, BattleCursor* menu, void* arg, u32 flags) {
    BattleMenuWork* window = 0;
    BattleCursor* cursor = 0;
    InitSubMenuCommonProcess(&window, &cursor, menu);
    window->field_0x8 = arg;
    if (flags & 1) {
        InitSubMenuCommonProcess2(window, cursor);
    } else {
        InitSubMenuCommonProcess3(window, cursor);
    }
}

// These are called, never inlined, in the original build. The unit's configure
// entry still uses the default `-inline auto`, so keep them out of line here.
#pragma dont_inline on
void InitSubMenuCommonProcess(BattleMenuWork** window, BattleCursor** cursor, BattleCursor* menu) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
    }
    wp->commandMenu.field_55C = BattleAlloc(0x124);
    *window = wp->commandMenu.field_55C;
    (*window)->cursor = menu;
    (*window)->field_0xC = 0;
    (*window)->field_0x10 = 0;
    (*window)->field_0x14 = 0;
    (*window)->field_0x18 = menu->field_0;
    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == 0xDE) {
        *cursor = &wp->commandMenu.cursor[0];
    } else {
        *cursor = &wp->commandMenu.cursor[1];
    }
    (*window)->field_0x98 = 0.0f;
    for (i = 0; i < 7; i++) {
        (*window)->field_0xA4[i] = 0;
        (*window)->field_0xC0[i] = 1;
        (*window)->field_0xDC[i] = -i;
        (*window)->field_0xF8[i] = 0;
    }
}

void InitSubMenuCommonProcess3(BattleMenuWork* window, BattleCursor* cursor) {
    s32 i;
    s32 idx = cursor->field_0;

    for (i = 0; i < cursor->unk8; i++) {
        idx++;
        if (idx >= cursor->unk8) {
            idx = 0;
        }
        window->field_0x48[idx] = 0;
        window->field_0x84[idx] = i;
        window->field_0x20[idx] = 0;
        window->field_0x20[idx] |= 1;
        if (i == cursor->unk8 - 1) {
            window->field_0x34[idx] = 1;
        } else {
            window->field_0x34[idx] = 0xB;
        }
    }
}

void InitSubMenuCommonProcess2(BattleMenuWork* window, BattleCursor* cursor) {
    s32 i;
    s32 idx = cursor->field_0;

    for (i = 0; i < cursor->unk8; i++) {
        idx++;
        if (idx >= cursor->unk8) {
            idx = 0;
        }
        window->field_0x48[idx] = 0;
        window->field_0x84[idx] = i;
        window->field_0x20[idx] = 0;
        window->field_0x20[idx] |= 1;
        if (i == cursor->unk8 - 1) {
            window->field_0x34[idx] = 0;
        } else {
            window->field_0x34[idx] = 0xA;
        }
    }
}
#pragma dont_inline reset

void battleMenuDispInit(void) {
	BattleWork* wp = _battleWorkPointer;
	char path[256];

	sprintf(path, "%s/battle/common/battle_common.tpl", getMarioStDvdRoot());
	wp->menuTex = fileAlloc(path, 4);
}

void battleMenuDispEnd(void) {
	fileFree(_battleWorkPointer->menuTex);
}

void BattleMenuDisp_MultiItem_Main(void) {}
void BattleMenuDisp_MultiItem_Disp(BattleWork* wp) {
    DrawMultiItemWin();
}
void BattleMenuDisp_MultiItem_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

void BattleMenuDisp_ChangePartySelect_Main(void) {}
void BattleMenuDisp_ChangePartySelect_Disp(BattleWork* wp) {
    DrawChangePartyWin();
}
void BattleMenuDisp_ChangePartySelect_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

void BattleMenuDisp_Operation_Main(void) {}
void BattleMenuDisp_Operation_Disp(BattleWork* wp) {
    DrawOperationWin();
}
void BattleMenuDisp_Operation_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

void BattleMenuDisp_WeaponSelect_Main(void) {}
void BattleMenuDisp_WeaponSelect_Disp(BattleWork* wp) {
    DrawWeaponWin();
}
void BattleMenuDisp_WeaponSelect_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

void BattleMenuDisp_ActSelect_Main(void) {}
void BattleMenuDisp_ActSelect_Disp(BattleWork* wp) {
    DrawMainMenu();
}
void BattleMenuDisp_ActSelect_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

void DrawSubMenuCommonProcessSub1(BattleMenuWork* window, BattleCursor* a, s32 idx, s32 mode) {
    s32 i;

    if (mode == 6 && a->unk8 > 6 && idx == a->unk8 - 6 && a->field_4 == 0 && a->field_0 != 1) {
        window->field_0xC = 0x1E;
        window->field_0x10 = 0;
        window->field_0x9C = (f32)(a->unk8 - 6);
        window->field_0xA0 = 0.0f;
        for (i = 0; i < 7; i++) {
            window->field_0xC0[i] = 0x1E;
            window->field_0xDC[i] = 0;
        }
    } else if (mode == 6 && a->unk8 > 6 && idx == 0 && a->field_4 == a->unk8 - 6 && a->field_0 != 5) {
        window->field_0xC = 0x28;
        window->field_0x10 = 0;
        window->field_0x9C = 0.0f;
        window->field_0xA0 = (f32)(a->unk8 - 6);
        for (i = 0; i < 7; i++) {
            window->field_0xC0[i] = 0x28;
            window->field_0xDC[i] = 0;
        }
    } else if (a->field_4 == idx + 1) {
        window->field_0xC = 0xA;
        window->field_0x10 = 0;
        window->field_0x9C = (f32)idx;
        window->field_0xA0 = (f32)(idx + 1);
        for (i = 0; i < 7; i++) {
            window->field_0xC0[i] = 0xA;
            window->field_0xDC[i] = 0;
        }
    } else if (a->field_4 == idx - 1) {
        window->field_0xC = 0x14;
        window->field_0x10 = 0;
        window->field_0x9C = (f32)idx;
        window->field_0xA0 = (f32)(idx - 1);
        for (i = 0; i < 7; i++) {
            window->field_0xC0[i] = 0x14;
            window->field_0xDC[i] = 0;
        }
    }
}

// NONMATCHING (99.11%): in the non-flag branch MWCC rotates window/idx/i through
// r8/r6/r7 where the original uses r6/r7/r8; no source form found that fixes it.
void BattleMenuDisp_ActSelect_Init(BattleWork* wp, BattleCursor* a, void* b, u32 flags) {
    s32 i;
    s32 idx;

    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
    }
    wp->commandMenu.field_55C = BattleAlloc(0x124);
    if (flags & 1) {
        BattleMenuWork* w = (BattleMenuWork*)wp->commandMenu.field_55C;
        w->cursor = a;
        w->field_0x4 = b;
        w->field_0xC = 0xA;
        w->field_0x14 = 0;
        w->field_0x18 = a->field_0;
        idx = a->field_0;
        for (i = 0; i < a->unk8; i++) {
            if (idx == a->field_0) {
                w->field_0x34[idx] = 0xA;
            } else {
                w->field_0x34[idx] = 0xB;
            }
            w->field_0x48[idx] = 0;
            w->field_0x84[idx] = a->unk8 - 1 + i;
            if (w->field_0x84[idx] >= a->unk8) {
                w->field_0x84[idx] -= a->unk8;
            }
            w->field_0x20[idx] = 0;
            idx++;
            if (idx >= a->unk8) {
                idx = 0;
            }
        }
    } else {
        BattleMenuWork* w = (BattleMenuWork*)wp->commandMenu.field_55C;
        w->cursor = a;
        w->field_0x4 = b;
        w->field_0xC = 0;
        w->field_0x14 = 0;
        w->field_0x18 = a->field_0;
        idx = a->field_0;
        for (i = 0; i < a->unk8; i++) {
            idx++;
            if (idx >= a->unk8) {
                idx = 0;
            }
            w->field_0x34[idx] = 0;
            w->field_0x48[idx] = i * 10 / 2;
            w->field_0x84[idx] = i;
            w->field_0x20[idx] = 0;
        }
    }
}

// NONMATCHING (85.42%): 7 instructions differ, 5 missing, 5 extra, plus register differences.
void DrawMenuCursorAndScrollArrow(Vec* pos) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    Vec vec;
    f32 f31;
    s32 n;

    f31 = intplGetValue(4, window->field_0x11C, 6, 0.0f, 10.0f);
    window->field_0x11C++;
    if (window->field_0x11C >= 12) {
        window->field_0x11C = 0;
    }
    if (cursor->field_4 > 0) {
        vec.x = 155.0f + pos->x;
        vec.y = 15.0f + pos->y + f31;
        vec.z = 0.0f;
        iconDispGx(&vec, 0x10, 0x1B5, 0.75f);
    }
    if (cursor->unk8 - 6 > cursor->field_4) {
        vec.x = 155.0f + pos->x;
        vec.y = 20.0f + pos->y - 164.0f - f31;
        vec.z = 0.0f;
        iconDispGx(&vec, 0x10, 0x1B6, 0.75f);
    }
    n = cursor->field_0 - cursor->field_4;
    if (n < 0) {
        n = 0;
    }
    if (n >= 6) {
        n = 5;
    }
    vec.x = pos->x;
    vec.y = pos->y - 24.0f * (f32)n;
    vec.z = 0.0f;
    iconDispGx(&vec, 0x14, 0x1F0, 1.0f);
}

// NONMATCHING (97.44%): MWCC coalesces sp/p (`cmplwi` instead of `mr. r31,r3`),
// shifting p/line to r3/r4; everything else is register renaming.
void DrawMenuHelpWin(const char* tag, f32 x, f32 y) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0xCF};
    s8 text[4][0x30];
    s8* sp = (s8*)tag;
    s8* p;
    s8* dp;
    s32 line;
    s32 i;

    windowDispGX_Waku_col(0, color, x, y, 400.0f, 50.0f, 10.0f);
    FontDrawStart();
    FontDrawColorIDX(0);
    FontDrawScale(0.8f);
    sp = (s8*)msgSearch((char*)sp);
    if (sp != 0) {
        for (i = 0; i < 4; i++) {
            text[i][0] = 0;
        }
        p = sp;
        dp = text[0];
        line = 0;
        for (;;) {
            if (*p == '<') {
                do {
                    p++;
                } while (*p != '>');
                p++;
            }
            if (*p == 0) {
                *dp = 0;
                break;
            }
            if (*p == '\n') {
                *dp = 0;
                line++;
                dp = text[line];
                p++;
                continue;
            }
            *dp++ = *p++;
        }
        if (text[0][0] != 0) {
            FontDrawScale(0.8f);
        }
        FontDrawString((char*)text[0], x + 20.0f, y - 4.0f);
        if (text[1][0] != 0) {
            FontDrawScale(0.8f);
        }
        FontDrawString((char*)text[1], x + 20.0f, y - 28.0f);
        if (text[2][0] != 0) {
            FontDrawScale(0.8f);
        }
        FontDrawString((char*)text[2], x + 20.0f, y - 52.0f);
    } else {
        FontDrawString("\x82\xB9\x82\xC2\x82\xDF\x82\xA2\x82\xCD\x81\x40\x82\xC8\x82\xA2\x82\xCC\x82\xBE\x81\x49\x81\x49", x + 10.0f, y - 4.0f);
    }
}

void DrawMenuPinchMark(void) {
    BattleWork* wp = _battleWorkPointer;
    CameraEntry* cam = camGetPtr(8);
    BattleUnit* unit;
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    u8 cr;
    u8 cg;
    u8 cb;
    u8 ca;
    u32 id;

    btlDispGXInit2D();
    unit = BattleGetMarioPtr(wp);
    if (unit->currentHp > 0 && unit->currentHp < unit->maxHp && unit->currentHp <= 5) {
        Vec pos;
        if (unit->currentHp == 1) {
            id = (gp->language == 0) ? 8 : 9;
            if (gp->retraceCount % 50 <= 24) {
                cr = 0xFF; cg = 0xE9; cb = 0x40; ca = 0xFF;
            } else {
                cr = 0xE9; cg = 0x52; cb = 0x2F; ca = 0xFF;
            }
        } else {
            cr = 0xFF; cg = 0xE9; cb = 0x40; ca = 0xFF;
            id = (gp->language == 0) ? 6 : 7;
        }
        TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, id);
        GXLoadTexObj(&texObj, GX_TEXMAP0);
        btlGetScreenPoint(&unit->mPosition, &pos);
        PSMTXTrans(mtx1, pos.x - 20.0f, 60.0f + pos.y, 0.0f);
        PSMTXConcat(cam->view, mtx1, mtx2);
        GXLoadPosMtxImm(mtx2, 0);
        btlDispGXQuads2D(-28.0f, 12.0f, 28.0f, -12.0f, cr, cg, cb, ca);
    }
    unit = BattleGetPartyPtr(wp);
    if (unit != 0 && unit->currentHp > 0 && unit->currentHp < unit->maxHp && unit->currentHp <= 5) {
        Vec pos;
        if (unit->currentHp == 1) {
            id = (gp->language == 0) ? 8 : 9;
            if (gp->retraceCount % 50 <= 24) {
                cr = 0xFF; cg = 0xE9; cb = 0x40; ca = 0xFF;
            } else {
                cr = 0xE9; cg = 0x52; cb = 0x2F; ca = 0xFF;
            }
        } else {
            cr = 0xFF; cg = 0xE9; cb = 0x40; ca = 0xFF;
            id = (gp->language == 0) ? 6 : 7;
        }
        TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, id);
        GXLoadTexObj(&texObj, GX_TEXMAP0);
        btlGetScreenPoint(&unit->mPosition, &pos);
        PSMTXTrans(mtx1, pos.x - 20.0f, 60.0f + pos.y, 0.0f);
        PSMTXConcat(cam->view, mtx1, mtx2);
        GXLoadPosMtxImm(mtx2, 0);
        btlDispGXQuads2D(-28.0f, 12.0f, 28.0f, -12.0f, cr, cg, cb, ca);
    }
}

// _Sub is a real call in the original; without this MWCC inlines it into DrawMenuPartyChangeButton.
#pragma dont_inline on
void DrawMenuPartyChangeButton_Sub(u32 tex, f32 x, f32 y, f32 angle) {
    Mtx mtx;

    PSMTXRotRad(mtx, 'y', 0.017453f * angle);
    PSMTXTransApply(mtx, mtx, x, y, 0.0f);
    {
        GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
        btlDispTexPlane2(mtx, tex, color, 0);
    }
}
#pragma dont_inline reset

// NONMATCHING (99.91%): f31 = x1 + x2 then *= 0.5f puts the sum in f31; the
// original keeps it in a temp f1 and multiplies into f31 (2 instructions).
void DrawMenuPartyChangeButton(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleUnit* mario;
    BattleUnit* party;
    s32 memberId;
    f32 itemX;
    f32 itemY;
    f32 itemZ;
    Vec screen1;
    Vec screen2;
    f32 f31;
    f32 f30;

    camGetPtr(8);
    mario = BattleGetMarioPtr(wp);
    party = BattleGetPartyPtr(wp);
    if (party == 0) {
        BattleAudience_GetItemOn(&memberId, &itemX, &itemY, &itemZ, 0);
        if (memberId != -1) {
            if (window->field_0x120 > 0.0f) {
                window->field_0x120 -= 5.0f;
            } else {
                window->field_0x120 = 0.0f;
            }
            btlGetScreenPoint(&mario->mPosition, &screen1);
            DrawMenuPartyChangeButton_Sub(5, screen1.x, screen1.y - 20.0f, window->field_0x120);
        } else {
            window->field_0x120 = 90.0f;
        }
    } else {
        btlGetScreenPoint(&mario->mPosition, &screen1);
        btlGetScreenPoint(&party->mPosition, &screen2);
        f31 = screen1.x + screen2.x;
        f31 *= 0.5f;
        f30 = screen1.y;
        BattleAudience_GetItemOn(&memberId, &itemX, &itemY, &itemZ, 0);
        if (memberId != -1) {
            if (window->field_0x120 == 90.0f) {
                psndSFXOn("SFX_BTL_SYS_CAUTION_THROW1");
            }
            if (window->field_0x120 > 0.0f) {
                window->field_0x120 -= 5.0f;
            } else {
                window->field_0x120 = 0.0f;
            }
            DrawMenuPartyChangeButton_Sub(5, f31, f30 - 20.0f, window->field_0x120);
        } else {
            window->field_0x120 = 90.0f;
            if (BattleCommandCheckChangePositionEnable(wp)) {
                DrawMenuPartyChangeButton_Sub(4, f31, f30 - 20.0f, 0.0f);
            }
        }
    }
}

#pragma dont_inline on
// NONMATCHING (99.38%): same instructions; only register numbers or operands differ.
void DrawSubIcon(BattleMenuWork* window, Mtx* mtxBase, s32* a, u16* b, s32* c, s32* d, s32* e, void* table, s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    BattleCursor* cursor;
    Mtx mtx;
    Mtx result[7];
    f32 f31;
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    cursor = window->cursor;
    f31 = (window->field_0x98 - (f32)(s32)window->field_0x98) * 24.0f;
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            if (window->field_0xA4[i] & 1) {
                PSMTXTrans(mtx, 100.0f, 35.0f - 24.0f * (f32)i + f31, 0.0f);
                PSMTXConcat(mtx, mtxBase[i], result[i]);
                DrawSubIconSub(window, i, result[i], a[i], b[i], c[i], d[i], e[i], table, flag);
            }
        }
    }
}

void DrawMainIcon(BattleMenuWork* window, void* table, Mtx* mtxA, Vec* vecB, f32* scales) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    BattleCursor* cursor;
    Vec vec;
    s32 order[6];
    f32 sorted[6];
    s32 i;
    s32 j;

    if (unit->currentKind == 0xDE) {
        cursor = (BattleCursor*)((u8*)wp + 0x1bb4);
    } else {
        cursor = (BattleCursor*)((u8*)wp + 0x1bc0);
    }
    for (i = 0; i < cursor->unk8; i++) {
        order[i] = i;
        sorted[i] = vecB[i].y;
    }
    for (i = 0; i < cursor->unk8 - 1; i++) {
        for (j = i + 1; j < cursor->unk8; j++) {
            if (sorted[i] < sorted[j]) {
                f32 tf = sorted[i];
                s32 ti = order[i];
                sorted[i] = sorted[j];
                order[i] = order[j];
                sorted[j] = tf;
                order[j] = ti;
            }
        }
    }
    for (i = 0; i < cursor->unk8; i++) {
        s32 idx = order[i];
        s32 flags = window->field_0x20[idx];
        if (flags & 1) {
            if (flags & 2) {
                DrawMainIconHukidasi(window, (void*)((u8*)table + idx * 0x14), &mtxA[idx], scales[idx]);
            } else {
                vec = vecB[idx];
                DrawMainIconCircle(window, (void*)((u8*)table + idx * 0x14), &mtxA[idx], &vec, scales[idx]);
            }
        }
    }
}

#pragma dont_inline reset
void GetRingOffset(f32* x, f32* y, f32 angle) {
    f32 a = 4.712389f - angle;
    f32 c = 80.0f * (f32)cos(a);
    *x = c;
    *y = 35.0f * (f32)sin(a);
}

void GetRingCenter(Vec* out) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    btlGetScreenPoint(&unit->mPosition, out);
    out->x += 22.5f;
    out->y += 105.0f;
    out->z = 0.0f;
    if (out->x <= -180.0f) {
        out->x = -180.0f;
    }
}
