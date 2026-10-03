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
extern const GXColor dat_8041c118;
extern const GXColor dat_8041c11c;
extern const GXColor dat_8041c120;
extern const GXColor dat_8041c140;
extern const f32 float_neg200_8041c17c;
extern const f32 float_neg100_8041c180;
extern const f32 float_neg60_8041c16c;
extern const f32 float_55_8041c170;
extern const f32 float_neg40_8041c184;
extern const f32 float_3p1416_8041c14c;
extern const f32 float_0p1_8041c174;


extern const f32 float_0_8041c144;
extern const f32 float_10_8041c15c;
extern const f32 float_15_8041c18c;
extern const f32 float_155_8041c188;
extern const f32 float_0p75_8041c190;
extern const f32 float_20_8041c168;
extern const f32 float_164_8041c194;
extern const f32 float_24_8041c198;
extern const f32 float_1_8041c178;
// Constants used by DrawSubIconSub (declared here so the symbols are emitted).
extern const f32 float_255_8041c1cc;
extern const f32 float_144_8041c1d0;
extern const f32 float_neg152_8041c1d4;
extern const f32 float_neg136_8041c1d8;
extern const f32 float_152_8041c1dc;
extern const f32 float_136_8041c1e0;
extern const f32 float_neg119_8041c1e4;
extern const f32 float_2_8041c1e8;
extern const f32 float_0p5_8041c1c8;
extern const f32 float_neg104_8041c1ec;
extern const f32 float_22_8041c1f0;
extern const f32 float_120_8041c1f4;
extern const f32 float_72_8041c1f8;
extern const f32 float_25_8041c1fc;
extern const f32 float_30_8041c200;
extern const f32 float_26_8041c204;
extern const f32 float_92_8041c208;
extern const f32 float_100_8041c20c;
extern const f32 float_76_8041c210;
extern const f32 float_78_8041c214;
extern const f32 float_4_8041c1a8;
extern const f32 float_0p8_8041c1a4;
extern const f32 float_28_8041c1ac;
extern const f32 float_3p0039_8041c138;
extern const GXColor dat_8041c130;
extern const GXColor dat_8041c134;
extern GXColor unk_80422ff0;

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

extern const Vec vec3_802ef168;
extern const Vec vec3_802ef174;
// In .rodata: `const` makes MWCC emit lis/addi instead of @sda21 loads.
extern const Vec vec3_802ef180;
extern const Vec vec3_802ef18c;

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
void DrawMultiItemWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuDispEntry* entries;
    f32 outX;
    f32 outY;
    u16 arr24[7];
    Vec vec;
    f32 scales[5];
    s32 arr88[7];
    s32 arr6C[7];
    s32 arr50[7];
    s32 arr34[7];
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
            arr88[i] = entries[idx].field_0x0;
            if (entries[idx].field_0xC != 0) {
                arr24[i] = entries[idx].field_0xC;
            } else {
                arr24[i] = 0x18B;
            }
            arr6C[i] = entries[idx].field_0x4;
            arr50[i] = entries[idx].field_0x10;
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

// This declaration order (and the split window/cursor assignment below) is what
// makes MWCC place the locals at the original frame offsets and hand out
// r30/r29/r28 to window/cursor/entries; the natural order does not match.
void DrawChangePartyWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window;
    BattleCursor* cursor;
    BattleMenuDispEntry* entries;
    f32 outX;
    f32 outY;
    u16 arr24[7];
    Vec vec;
    f32 scales[5];
    s32 arr88[7];
    s32 arr6C[7];
    s32 arr50[7];
    s32 arr34[7];
    Vec centers[5];
    Mtx mats[5];
    Mtx rot[7];
    s32 i;

    window = (BattleMenuWork*)wp->commandMenu.field_55C;
    cursor = window->cursor;
    BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuDispEntry*)((u8*)wp + 0x1aac);
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            BattleMenuDispEntry* entry = &entries[idx];
            arr88[i] = entry->field_0x4;
            if (entry->field_0xC != 0) {
                arr24[i] = entry->field_0xC;
            } else {
                arr24[i] = 0x18B;
            }
            arr6C[i] = (s32)entry->field_0x8;
            arr50[i] = 0;
            arr34[i] = 0;
        }
    }
    DrawMainIcon(window, (void*)((u8*)wp + 0x1724), mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, (void*)entries, 1);
    vec = vec3_802ef180;
    DrawMenuCursorAndScrollArrow(&vec);
    if ((u32)entries[cursor->field_0].field_0x10 != 0) {
        DrawMenuHelpWin((const char*)entries[cursor->field_0].field_0x10, outX, outY);
    }
}

// This declaration order, and the function-scope `entry`, are what make MWCC
// pack the frame at the original offsets (outX/outY at 0x14/0x10, arr88 at 0x88,
// ...) and hand r10/r9 to i/entry; the natural order does not.
void DrawOperationWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuDispEntryOp* entries;
    f32 scales[5];
    s32 arr88[7];
    s32 arr6C[7];
    Vec centers[5];
    u16 arr24[7];
    f32 outX;
    Mtx rot[7];
    s32 arr50[7];
    Mtx mats[5];
    s32 arr34[7];
    Vec vec;
    f32 outY;

    BattleMenuDispEntryOp* entry;
    s32 i;
    BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuDispEntryOp*)((u8*)wp + 0x19e8);
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            entry = &entries[idx];
            arr88[i] = entry->field_0x4;
            if (entries[i].field_0x10 != 0) {
                arr24[i] = entry->field_0x10;
            } else {
                arr24[i] = 0x18B;
            }
            arr6C[i] = entry->field_0xC;
            arr50[i] = entry->field_0x18;
            arr34[i] = 0;
        }
    }
    DrawMainIcon(window, (void*)((u8*)wp + 0x1724), mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, 0, 0);
    vec = vec3_802ef174;
    DrawMenuCursorAndScrollArrow(&vec);
    if ((u32)entries[cursor->field_0].field_0x14 != 0) {
        DrawMenuHelpWin((const char*)entries[cursor->field_0].field_0x14, outX, outY);
    }
}

void DrawWeaponWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleCommand* cmd = &wp->commandMenu;
    BattleMenuWork* window = (BattleMenuWork*)cmd->field_55C;
    BattleCursor* cursor = window->cursor;
    BattleUnit* unit;
    BattleWeapon* sel;
    f32 outX;
    f32 outY;
    u16 arr24[7];
    Vec vec;
    f32 scales[5];
    s32 arr88[7];
    s32 arr6C[7];
    s32 arr50[7];
    s32 arr34[7];
    Vec centers[5];
    Mtx mats[5];
    Mtx rot[7];
    s32 i;
    const char* msg;

    unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    sel = ((BattleMenuWeaponEntry*)window->field_0x8)[cursor->field_0].weapon;
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->field_0x98 + i;
        if (idx < cursor->unk8) {
            arr88[i] = ((BattleMenuWeaponEntry*)window->field_0x8)[idx].field_0x4;
            arr24[i] = *(u16*)((u8*)cmd + idx * 0x1c + 0x8c);
            arr6C[i] = ((BattleMenuWeaponEntry*)window->field_0x8)[idx].field_0x8;
            if (((BattleMenuWeaponEntry*)window->field_0x8)[idx].weapon != 0) {
                arr50[i] = BtlUnit_GetWeaponCost(unit, ((BattleMenuWeaponEntry*)window->field_0x8)[idx].weapon);
            } else {
                arr50[i] = 0;
            }
            if (((BattleMenuWeaponEntry*)window->field_0x8)[idx].weapon != 0) {
                arr34[i] = *(u8*)((u8*)((BattleMenuWeaponEntry*)window->field_0x8)[idx].weapon + 0x12);
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
            BattleMenuWeaponEntry* entry = (BattleMenuWeaponEntry*)window->field_0x8;
            msg = itemDataTable[entry[cursor->field_0].field_0x14].description;
        } else {
            msg = *(const char**)((u8*)sel + 0xC);
        }
    } else {
        BattleMenuWeaponEntry* entry = (BattleMenuWeaponEntry*)window->field_0x8;
        msg = itemDataTable[entry[cursor->field_0].field_0x14].description;
    }
    if (msg != 0) {
        DrawMenuHelpWin(msg, outX, outY);
    }
}
#pragma dont_inline reset

extern const f32 float_0p2_8041c148;
extern const f32 float_180_8041c150;
extern const f32 float_270_8041c154;
extern const f32 float_90_8041c158;
extern const f32 float_360_8041c160;
extern const f32 float_0p017453_8041c164;

// Register allocation only: every instruction, call, constant and branch matches the target;
// the remaining diffs are FPR/GPR colours. Case 5's angles are built from the explicit
// `pi * deg` numerators (a70/a84) before the `/ 180` divisions; that fixes the original's
// interleaved `fmuls, fmuls, fdivs, fdivs` schedule, and writing case 4's `0.2 * n` as
// `n * 0.2` puts its i2f magic/double in f4/f3 like the target (98.50% -> 99.77%).
// Case 1 now computes `d` as "load field_0x18 into a temp then subf" (`s32 d =
// window->field_0x18; d = window->cursor->field_0 - d;`), which moves the field_0x18 load
// from r4 to r3 and the `subf` to the target's `subf r4,r3,r0`; writing case 5's product as
// `(f32)n * float_0p2` (not `float_0p2 * (f32)n`) re-colours the i2f/0p2 set (99.79%).
// Still differing: case 1 reloads `window->cursor` into r3 where the target uses r5; in case 4
// only `v84` (f2 vs f3) and the `float_180`/`float_0p2` pair (f5/f2 vs f2/f5) are swapped;
// case 5 keeps `pi`/`float_180` in f6/f5 where the target has f3/f2, so its i2f magic/double/
// 0.2 sit in f4/f3/f2 instead of f5/f4/f6.
// NONMATCHING (99.79%): see above.
void DrawMainMenu(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    f32 ringX;
    f32 ringY;
    Vec vecB[5];
    Mtx mtxA[5];
    f32 scales[5];
    Mtx transMtx;
    Mtx rotMtx;
    Mtx scaleMtx;
    f32 cur;
    f32 angle;
    s32 i;

    camGetPtr(8);
    switch (window->field_0xC) {
    case 0:
        {
            s32 count = 0;
            for (i = 0; i < cursor->unk8; i++) {
                if (window->field_0x34[i] == 3) {
                    count++;
                }
            }
            if (count == cursor->unk8) {
                window->field_0xC = 1;
            }
        }
        break;
    case 1:
        window->field_0x14 = 1;
        {
            s32 d = window->field_0x18;
            d = window->cursor->field_0 - d;
            if (d == 1 || -(window->cursor->unk8 - 1) == d) {
                window->field_0xC = 2;
                window->field_0x14 = 0;
                window->field_0x18 = window->cursor->field_0;
                for (i = 0; i < cursor->unk8; i++) {
                    window->field_0x34[i] = 4;
                    window->field_0x48[i] = 0;
                    window->field_0x70[i] = window->field_0x84[i];
                    window->field_0x84[i]--;
                    if (window->field_0x84[i] == -1) {
                        window->field_0x84[i] = cursor->unk8 - 1;
                    }
                }
            } else if (d == -1 || window->cursor->unk8 - 1 == d) {
                window->field_0xC = 2;
                window->field_0x14 = 0;
                window->field_0x18 = window->cursor->field_0;
                for (i = 0; i < cursor->unk8; i++) {
                    window->field_0x34[i] = 5;
                    window->field_0x48[i] = 0;
                    window->field_0x70[i] = window->field_0x84[i];
                    window->field_0x84[i]++;
                    if (window->field_0x84[i] == cursor->unk8) {
                        window->field_0x84[i] = 0;
                    }
                }
            }
        }
        break;
    case 2:
        {
            s32 count = 0;
            for (i = 0; i < cursor->unk8; i++) {
                if (window->field_0x34[i] == 3) {
                    count++;
                }
            }
            if (count == cursor->unk8) {
                window->field_0xC = 1;
            }
        }
        break;
    case 0xA:
        window->field_0xC = 1;
        window->field_0x14 = 1;
        break;
    }

    window->field_0x1C = 0;
    for (i = 0; i < cursor->unk8; i++) {
        switch (window->field_0x34[i]) {
        case 0:
            window->field_0x48[i]--;
            if (window->field_0x48[i] <= 0) {
                window->field_0x34[i] = 1;
                window->field_0x48[i] = 0;
            }
            break;
        case 1:
            window->field_0x20[i] |= 1;
            {
                cur = float_0p2_8041c148 * (f32)window->field_0x48[i] + float_0_8041c144;
            }
            angle = float_3p1416_8041c14c * kakutable[cursor->unk8 - 3][window->field_0x84[i]] / float_180_8041c150;
            {
                if (cur >= angle) {
                    cur = angle;
                }
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, cur);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                if (i == cursor->field_0) {
                    f32 r = float_90_8041c158 * (f32)window->field_0x48[i] / float_10_8041c15c + float_270_8041c154;
                    if (r >= float_360_8041c160) {
                        r = float_0_8041c144;
                    }
                    PSMTXRotRad(rotMtx, 'x', float_0p017453_8041c164 * r);
                    PSMTXTrans(transMtx, float_20_8041c168 + vecB[i].x, vecB[i].y, vecB[i].z);
                    PSMTXConcat(transMtx, rotMtx, mtxA[i]);
                    window->field_0x20[i] |= 2;
                } else {
                    f32 r = float_90_8041c158 * (f32)window->field_0x48[i] / float_10_8041c15c + float_270_8041c154;
                    if (r >= float_360_8041c160) {
                        r = float_0_8041c144;
                    }
                    PSMTXRotRad(rotMtx, 'x', float_0p017453_8041c164 * r);
                    PSMTXTrans(transMtx, vecB[i].x, vecB[i].y, vecB[i].z);
                    PSMTXConcat(transMtx, rotMtx, mtxA[i]);
                }
                scales[i] = 0.0f;
                if (cur >= angle && window->field_0x48[i] >= 10) {
                    window->field_0x34[i] = 3;
                    window->field_0x48[i] = -1;
                }
                window->field_0x48[i]++;
            }
            break;
        case 3:
            angle = float_3p1416_8041c14c * kakutable[cursor->unk8 - 3][window->field_0x84[i]] / float_180_8041c150;
            GetRingCenter(&vecB[i]);
            GetRingOffset(&ringX, &ringY, angle);
            vecB[i].x += ringX;
            vecB[i].y += ringY;
            vecB[i].z = ringY;
            if (i == cursor->field_0) {
                PSMTXTrans(mtxA[i], float_20_8041c168 + vecB[i].x, vecB[i].y, vecB[i].z);
                window->field_0x20[i] |= 2;
            } else {
                PSMTXTrans(mtxA[i], vecB[i].x, vecB[i].y, vecB[i].z);
            }
            scales[i] = 0.0f;
            break;
        case 4:
            {
                f32 v70 = kakutable[cursor->unk8 - 3][window->field_0x70[i]];
                f32 v84 = kakutable[cursor->unk8 - 3][window->field_0x84[i]];
                f32 cur;
                if (v84 == 0.0f) {
                    v84 = float_360_8041c160;
                }
                cur = (f32)window->field_0x48[i] * float_0p2_8041c148 + float_3p1416_8041c14c * v70 / float_180_8041c150;
                angle = float_3p1416_8041c14c * v84 / float_180_8041c150;
                if (cur >= angle) {
                    window->field_0x34[i] = 3;
                    cur = angle;
                    window->field_0x48[i] = -1;
                }
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, cur);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                PSMTXTrans(mtxA[i], vecB[i].x, vecB[i].y, vecB[i].z);
                scales[i] = 0.0f;
                window->field_0x20[i] &= ~2;
                window->field_0x48[i]++;
            }
            break;
        case 5:
            {
                f32 v70;
                f32 v84;
                f32 cur;
                f32 a70;
#ifdef MUST_MATCH
                f32 a84;
#endif
                v70 = kakutable[cursor->unk8 - 3][window->field_0x70[i]];
                if (v70 == 0.0f) {
                    v70 = float_360_8041c160;
                }
                v84 = kakutable[cursor->unk8 - 3][window->field_0x84[i]];
#ifdef MUST_MATCH
                // Compute both `pi * deg` numerators before the divisions: the target's
                // fmuls, fmuls, fdivs, fdivs schedule only comes out of this order.
                a70 = float_3p1416_8041c14c * v70;
                a84 = float_3p1416_8041c14c * v84;
                cur = a70 / float_180_8041c150 - (f32)window->field_0x48[i] * float_0p2_8041c148;
                angle = a84 / float_180_8041c150;
#else
                a70 = float_3p1416_8041c14c * v70 / float_180_8041c150;
                angle = float_3p1416_8041c14c * v84 / float_180_8041c150;
                cur = a70 - float_0p2_8041c148 * (f32)window->field_0x48[i];
#endif
                if (cur <= angle) {
                    window->field_0x34[i] = 3;
                    cur = angle;
                    window->field_0x48[i] = -1;
                }
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, cur);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                PSMTXTrans(mtxA[i], vecB[i].x, vecB[i].y, vecB[i].z);
                scales[i] = 0.0f;
                window->field_0x20[i] &= ~2;
                window->field_0x48[i]++;
            }
            break;
        case 0xA:
            window->field_0x20[i] |= 1;
            window->field_0x20[i] |= 2;
            GetRingCenter(&vecB[i]);
            GetRingOffset(&ringX, &ringY, 0.0f);
            vecB[i].x += ringX;
            vecB[i].y += ringY;
            vecB[i].z = ringY;
            vecB[i].x = intplGetValue(4, window->field_0x48[i], 12, float_neg60_8041c16c, vecB[i].x);
            vecB[i].y = intplGetValue(4, window->field_0x48[i], 12, float_55_8041c170, vecB[i].y);
            PSMTXTrans(mtxA[i], float_20_8041c168 + vecB[i].x, vecB[i].y, vecB[i].z);
            scales[i] = intplGetValue(0, window->field_0x48[i], 12, float_360_8041c160, float_0_8041c144);
            window->field_0x48[i]++;
            if (window->field_0x48[i] > 12) {
                window->field_0x34[i] = 3;
            }
            break;
        case 0xB:
            window->field_0x20[i] |= 1;
            window->field_0x20[i] &= ~2;
            angle = float_3p1416_8041c14c * kakutable[cursor->unk8 - 3][window->field_0x84[i]] / float_180_8041c150;
            {
                f32 s;
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, angle);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                s = float_0p1_8041c174 * (f32)window->field_0x48[i];
                if (s >= float_1_8041c178) {
                    window->field_0x34[i] = 3;
                }
                PSMTXScale(scaleMtx, s, s, float_1_8041c178);
                PSMTXTrans(transMtx, vecB[i].x, vecB[i].y, vecB[i].z);
                PSMTXConcat(transMtx, scaleMtx, mtxA[i]);
                scales[i] = 0.0f;
                window->field_0x48[i]++;
            }
            break;
        }
    }

    DrawMainIcon(window, window->field_0x4, mtxA, vecB, scales);
    DrawMenuPartyChangeButton();
    DrawMenuPinchMark();
}

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

void DrawSubMenuCommonProcess(f32* outX, f32* outY, Mtx* mtxA, Vec* vecB, f32* scales, Mtx* mtxC) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleCursor* mainCursor;
    s32 maxCount;
    s32 idx;
    s32 i;
    s32 field0;
    f32 ringX;
    f32 ringY;
    f32 angle;
    f32 scale;
    Mtx transMtx;
    Mtx scaleMtx;

    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == 0xDE) {
        mainCursor = (BattleCursor*)((u8*)wp + 0x1bb4);
    } else {
        mainCursor = (BattleCursor*)((u8*)wp + 0x1bc0);
    }
    {
        s32 n = cursor->unk8;
        if (n < 6) {
            maxCount = n;
        } else {
            maxCount = 6;
        }
    }
    idx = cursor->field_4;
    field0 = cursor->field_0;
    if (field0 <= cursor->field_4) {
        cursor->field_4 = field0 - 1;
    } else if (field0 >= cursor->field_4 + maxCount - 1) {
        cursor->field_4 = field0 - maxCount + 2;
    }
    if (cursor->field_4 < 0) {
        cursor->field_4 = 0;
    }
    if (cursor->field_4 >= cursor->unk8 - maxCount) {
        cursor->field_4 = cursor->unk8 - maxCount;
    }

    *outX = float_neg200_8041c17c;
    *outY = float_neg100_8041c180;

    {
    s32 type = window->field_0xC;
    switch (type) {
    case 0:
        window->field_0x14 = 1;
        window->field_0x98 = (f32)cursor->field_4;
        {
            s32 c1 = 0;
            for (i = 0; i < mainCursor->unk8; i++) {
                if (window->field_0x34[i] == 1 || window->field_0x34[i] == 0xB) {
                    c1++;
                }
            }
            {
                s32 c2;
                for (i = 0, c2 = 0; i < 7; i++) {
                    if (window->field_0xC0[i] == 2) {
                        c2++;
                    }
                }
                if (c1 == mainCursor->unk8 && c2 == 7) {
                    window->field_0xC = 2;
                }
            }
        }
        DrawSubMenuCommonProcessSub1(window, cursor, idx, maxCount);
        break;
    case 2:
        window->field_0x14 = 1;
        window->field_0x98 = (f32)cursor->field_4;
        DrawSubMenuCommonProcessSub1(window, cursor, idx, maxCount);
        break;
    case 0xA:
    case 0x14:
    case 0x1E:
    case 0x28:
        if (type == 0xA || type == 0x14) {
            window->field_0x14 = 1;
        }
        {
            s32 c2 = 0;
            for (i = 0; i < 7; i++) {
                if (window->field_0xC0[i] == 2) {
                    c2++;
                }
            }
            if (c2 == 7) {
                window->field_0xC = 2;
                window->field_0x98 = (f32)cursor->field_4;
            }
        }
        DrawSubMenuCommonProcessSub1(window, cursor, idx, maxCount);
        break;
    }
    }

    window->field_0x1C = 0;
    for (i = 0; i < mainCursor->unk8; i++) {
        switch (window->field_0x34[i]) {
        case 0:
            window->field_0x20[i] |= 1;
            window->field_0x20[i] |= 2;
            GetRingCenter(&vecB[i]);
            vecB[i].x = intplGetValue(4, window->field_0x48[i], 12, vecB[i].x, float_neg60_8041c16c);
            vecB[i].y = intplGetValue(4, window->field_0x48[i], 12, vecB[i].y, float_55_8041c170);
            PSMTXTrans(mtxA[i], 20.0f + vecB[i].x, vecB[i].y, vecB[i].z);
            scales[i] = intplGetValue(0, window->field_0x48[i], 12, 0.0f, 360.0f);
            if (scales[i] > 360.0f) {
                scales[i] -= 360.0f;
            }
            if (window->field_0x48[i] >= 3) {
                window->field_0x1C = 1;
            }
            window->field_0x48[i]++;
            if (window->field_0x48[i] > 12) {
                window->field_0x34[i] = 1;
            }
            break;
        case 1:
            window->field_0x20[i] |= 1;
            window->field_0x20[i] |= 2;
            PSMTXTrans(mtxA[i], float_neg40_8041c184, float_55_8041c170, 0.0f);
            scales[i] = 360.0f;
            window->field_0x1C = 1;
            break;
        case 0xA:
            window->field_0x20[i] |= 1;
            window->field_0x20[i] &= ~2;
            angle = float_3p1416_8041c14c * kakutable[mainCursor->unk8 - 3][window->field_0x84[i]] / 180.0f;
            GetRingCenter(&vecB[i]);
            GetRingOffset(&ringX, &ringY, angle);
            vecB[i].x += ringX;
            vecB[i].y += ringY;
            vecB[i].z = ringY;
            scale = 1.0f - float_0p1_8041c174 * (f32)window->field_0x48[i];
            if (scale <= 0.0f) {
                window->field_0x34[i] = 0xB;
            }
            PSMTXScale(scaleMtx, scale, scale, 1.0f);
            PSMTXTrans(transMtx, vecB[i].x, vecB[i].y, vecB[i].z);
            PSMTXConcat(transMtx, scaleMtx, mtxA[i]);
            scales[i] = 0.0f;
            window->field_0x48[i]++;
            break;
        case 0xB:
            window->field_0x20[i] &= ~1;
            break;
        }
    }

    for (i = 0; i < 7; i++) {
        s32 state = window->field_0xC0[i];
        switch (state) {
        case 0:
            window->field_0xA4[i] &= ~1;
            window->field_0x114[i] = 0xFF;
            break;
        case 1:
            window->field_0xA4[i] &= ~1;
            window->field_0x114[i] = 0xFF;
            if (window->field_0xDC[i] >= 0 && i != 6) {
                f32 rad = intplGetValue(0, window->field_0xDC[i], 12, 90.0f, 0.0f);
                window->field_0xA4[i] |= 1;
                PSMTXRotRad(mtxC[i], 'y', 0.017453292f * rad);
            }
            window->field_0xDC[i]++;
            if (window->field_0xDC[i] > 12) {
                window->field_0xC0[i] = 2;
            }
            break;
        case 2:
            window->field_0x114[i] = 0xFF;
            if (i != 6) {
                window->field_0xA4[i] |= 1;
                PSMTXIdentity(mtxC[i]);
            } else {
                window->field_0xA4[i] &= ~1;
            }
            break;
        case 0xA:
        case 0x14:
        case 0x1E:
        case 0x28:
            {
                s32 steps;
                if (state == 0xA || state == 0x14) {
                    steps = 12;
                }
                if (state == 0x1E || state == 0x28) {
                    steps = (cursor->unk8 - 6) / 2 + 12;
                }
                window->field_0xA4[i] |= 1;
                PSMTXIdentity(mtxC[i]);
                window->field_0x98 = intplGetValue(4, window->field_0xDC[i], steps, window->field_0x9C, window->field_0xA0);
                window->field_0xDC[i]++;
                if (window->field_0xDC[i] > steps) {
                    window->field_0xC0[i] = 2;
                    window->field_0xDC[i] = 0;
                }
            }
            break;
        }
    }
}

void BattleMenuDisp_ActSelect_Init(BattleWork* wp, BattleCursor* a, void* b, u32 flags) {
    s32 i;
    s32 idx;
    BattleMenuWork* w;

    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
    }
    wp->commandMenu.field_55C = BattleAlloc(0x124);
    if (flags & 1) {
        w = (BattleMenuWork*)wp->commandMenu.field_55C;
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
        w = (BattleMenuWork*)wp->commandMenu.field_55C;
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

// The original emits a real `bl` to this function from every caller, so it must
// be marked don't-inline itself (a surrounding region only stops its functions
// being inlined elsewhere).
#pragma dont_inline on
// Instruction scheduling / register allocation only. Splitting the first block's sums
// (`ysum = float_15; ysum += pos->y;`) makes MWCC form the sum in place in f2 exactly
// like the target; a single-expression form materialises it in a fresh register. The
// residual: `pos->y` lands in f0 where the target has f1 (so the 0.0 load is scheduled
// after pos->x); the second block has the same rotation; the tail uses the `f32 ny`
// temp and schedules the final fnmsubs differently. All 140 valid orderings of the
// first block's load/add/store statements compile identically, as do single-expression,
// local-constant, shared-temp and volatile forms and opt_propagation/opt_lifetimes/
// opt_common_subs/opt_dead_assignments off; only the register rotation is left, so this
// is permuter work. Re-verified: 140 first-block orderings, a joint cross product of
// block1/block2/tail forms, a 324-combination single/rev/split sum sweep (max 87.98%), the
// volatile/local/literal `zero` forms, compound-literal/designated-initializer/array forms,
// a hand-ordered whole-function rewrite under `#pragma scheduling off` (49.57%, and it
// moves `pos` to r29), every `-O` level, GC compilers 2.0/2.5/2.6/2.7 (identical 88.09%),
// and the `optimize_for_size`/`scheduling`/opt_* pragmas (all lower; function-scope zero
// hangs MWCC). Per-region diff counts are 8/8/11 and the best tail-only form still leaves
// 8 scheduling diffs. The scheduler never hoists the `float_0` load into f0 above
// `pos->x`, so `pos->y`/`pos->x` land in f0 instead of the target's f1.
// Exhaustive first- and second-block linear-extension searches (all 30
// dependency-respecting orderings each, in split and single-statement sum forms) and a
// tail sweep were tried. The tail breakthrough: writing the product as a signed negated temp
// `f32 ny = float_24 * -(f32)n; vec.y = pos->y + ny;` (rather than `vec.y = pos->y - ny`)
// drops the tail diffs from 11 to 10 and lifts the whole function to 89.63%; the direct
// `pos->y - 24*(f32)n` form emits the target's `fnmsubs` but re-schedules the `vec.x`
// load (88.04%), and `(f32)n * 24`, `-ny + pos->y`, function-scope `ny`, `px`/`py` locals,
// four store orders and every `vec.x` position all score lower. Per-region diff counts are
// now 8/8/10; the remaining 8+8 are the first/second block's `pos->y`-in-f0 rotation.
// NONMATCHING (89.63%): see above.
void DrawMenuCursorAndScrollArrow(Vec* pos) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    Vec vec;
    f32 f31;
    s32 n;

    f31 = intplGetValue(4, window->field_0x11C, 6, float_0_8041c144, float_10_8041c15c);
    window->field_0x11C++;
    if (window->field_0x11C >= 12) {
        window->field_0x11C = 0;
    }
    if (cursor->field_4 > 0) {
        f32 ysum;
        f32 xsum;
        ysum = float_15_8041c18c;
        ysum += pos->y;
        xsum = float_155_8041c188;
        xsum += pos->x;
        vec.y = ysum + f31;
        vec.x = xsum;
        vec.z = float_0_8041c144;
        iconDispGx(&vec, 0x10, 0x1B5, float_0p75_8041c190);
    }
    if (cursor->field_4 < cursor->unk8 - 6) {
        f32 ysum = float_20_8041c168 + pos->y - float_164_8041c194;
        f32 xsum = float_155_8041c188 + pos->x;
        vec.y = ysum - f31;
        vec.x = xsum;
        vec.z = float_0_8041c144;
        iconDispGx(&vec, 0x10, 0x1B6, float_0p75_8041c190);
    }
    vec.x = pos->x;
    n = cursor->field_0 - cursor->field_4;
    if (n < 0) {
        n = 0;
    }
    if (n >= 6) {
        n = 5;
    }
    {
        f32 ny = float_24_8041c198 * -(f32)n;
        vec.y = pos->y + ny;
        vec.z = float_0_8041c144;
    }
    iconDispGx(&vec, 0x14, 0x1F0, float_1_8041c178);
}
#pragma dont_inline reset

// The original reuses its `tag` parameter as the message pointer (`mr. r31, r3`
// after msgSearch); writing the result back into `tag` reproduces the register
// allocation, and the copy loop needs `*dp = *p; p++; dp++;` for the p/dp
// increment order.
void DrawMenuHelpWin(const char* tag, f32 x, f32 y) {
    GXColor color = dat_8041c140;
    s8 text[4][0x30];
    s8* p;
    s8* dp;
    s32 line;
    s32 i;

    windowDispGX_Waku_col(0, color, x, y, 400.0f, 50.0f, 10.0f);
    FontDrawStart();
    FontDrawColorIDX(0);
    FontDrawScale(0.8f);
    if ((tag = msgSearch(tag)) != 0) {
        for (i = 0; i < 4; i++) {
            text[i][0] = 0;
        }
        p = (s8*)tag;
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
            *dp = *p;
            p++;
            dp++;
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
        f31 = (screen1.x + screen2.x) / 2.0f;
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

// dont_inline: keep deferred inlining from pulling same-TU helpers into DrawSubIcon/DrawMainIcon.
#pragma dont_inline on
void DrawSubIcon(BattleMenuWork* window, Mtx* mtxBase, s32* a, u16* b, s32* c, s32* d, s32* e, void* table, s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    BattleCursor* cursor;
    Mtx mtx;
    Mtx result[7];
    f32 f31;
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    cursor = window->cursor;
    f31 = window->field_0x98 - (f32)(s32)window->field_0x98;
    f31 = f31 * 24.0f;
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

// Builds the text colour the sub icons tint their labels with.
static inline GXColor SubIconFontColor(GXColor c, s32 alpha) {
    c.a = alpha;
    return c;
}

// NONMATCHING (99.94%): the instruction sequence matches; every remaining diff is the compiler's .rodata
// pool anchor. The original bases the ABI-pool at dat_802ef068 (the unit's first .rodata object) and reaches
// the i2f doubles at +0x130/+0x1b8 and the four msgSearch strings at +0x170/+0x184/+0x198/+0x1a4; our pool
// anchors the anonymous ...rodata.0 and its order/content differs, so those offsets differ. That is file-wide:
// dat_802ef068/dat_802ef0e8/the four vec3_802ef1xx are still extern here, and this file's source is in the
// .fn/asm order while the compiler emits the reverse, so the pool order only lines up once the functions are
// reordered (fn_order.py) and the unit data is defined. The other 6 diffs are register/stack allocation: the
// i2f int temp in r4 instead of r3, the cursor->field_0 compare in r3/r0 instead of r0/r3, and the
// SubIconFontColor GXColor temp/color stack slots 0x30/0x14 swapped. The s32->u8/u16 arg masks were removed
// via MUST_MATCH casts. Verified: no symbol-only `~~` diff, so every float_/GXColor constant used here is the
// original's; the field/struct reads match.
void DrawSubIconSub(BattleMenuWork* window, s32 index, Mtx mtx, s32 a, u16 b, s32 c, s32 d, s32 e, void* table,
                    s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    BattleCursor* cursor = window->cursor;
    CameraEntry* cam = camGetPtr(CAMERA_2D);
    GXTexObj texObj;
    Mtx mtxScale;
    Mtx mtxTrans;
    Mtx mtxOut;
    s32 digitsD[3];
    s32 posD[3];
    s32 digitsE[3];
    s32 posE[3];
    s32 digitsH[3];
    s32 posH[3];
    char textD[2];
    char textE[2];
    char textH[2];
    f32 frac;
    s32 alpha;
    u8 cr;
    u8 cg;
    u8 cb;
    s32 ca;
    s32 count;
    s32 i;

    frac = window->field_0x98 - (f32)(s32)window->field_0x98;
    alpha = 0xFF;
    if (index == 0) {
        alpha = (s32)(float_255_8041c1cc * (float_1_8041c178 - frac));
    }
    if (index == 6) {
        alpha = (s32)(float_255_8041c1cc * frac);
    }
    if ((s32)window->field_0x98 + index == cursor->field_0) {
        cr = seleItemCoordCol.r;
        cg = seleItemCoordCol.g;
        cb = seleItemCoordCol.b;
        ca = alpha;
    } else {
        cb = 0xFF;
        cg = 0xFF;
        cr = 0xFF;
        ca = (s32)(float_144_8041c1d0 * (f32)(u8)alpha / float_255_8041c1cc);
    }
    btlDispGXInit2D();
    PSMTXConcat(cam->view, mtx, mtxOut);
    GXLoadPosMtxImm(mtxOut, 0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, 2);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
    // btlDispGXQuads2D's header declares its alpha as u8, but the original passes a 32-bit value
    // with no truncation; casting `a` to s32 restores that ABI.
    ((void (*)(f32, f32, f32, f32, u8, u8, u8, s32))btlDispGXQuads2D)(
        float_neg152_8041c1d4, float_24_8041c198, float_neg136_8041c1d8, float_0_8041c144, cr, cg, cb, ca);
    ((void (*)(f32, f32, f32, f32, u8, u8, u8, s32))btlDispGXQuads2D)(
        float_152_8041c1dc, float_24_8041c198, float_136_8041c1e0, float_0_8041c144, cr, cg, cb, ca);
#else
    btlDispGXQuads2D(float_neg152_8041c1d4, float_24_8041c198, float_neg136_8041c1d8, float_0_8041c144, cr,
                     cg, cb, ca);
    btlDispGXQuads2D(float_152_8041c1dc, float_24_8041c198, float_136_8041c1e0, float_0_8041c144, cr, cg, cb,
                     ca);
#endif
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, 3);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
    ((void (*)(f32, f32, f32, f32, u8, u8, u8, s32))btlDispGXQuads2D)(
        float_neg136_8041c1d8, float_24_8041c198, float_136_8041c1e0, float_0_8041c144, cr, cg, cb, ca);
#else
    btlDispGXQuads2D(float_neg136_8041c1d8, float_24_8041c198, float_136_8041c1e0, float_0_8041c144, cr, cg, cb,
                     ca);
#endif
    PSMTXTrans(mtxTrans, float_neg119_8041c1e4, float_2_8041c1e8, float_0_8041c144);
    PSMTXScale(mtxScale, float_0p5_8041c1c8, float_0p5_8041c1c8, float_1_8041c178);
    PSMTXConcat(mtx, mtxTrans, mtxOut);
    PSMTXConcat(mtxOut, mtxScale, mtxOut);
    if (b == 0) {
        b = 0x18B;
    }
    if (a == 1) {
#ifdef MUST_MATCH
        // iconDispGxCol's header declares iconId as s32, but the original passes the u16 `b` with no
        // zero-extension; casting iconId to u16 restores that.
        ((void (*)(Mtx, s16, u16, GXColor))iconDispGxCol)(mtxOut, 0x10, b,
                                                          SubIconFontColor(dat_8041c130, alpha));
#else
        iconDispGxCol(mtxOut, 0x10, b, SubIconFontColor(dat_8041c130, alpha));
#endif
    } else {
#ifdef MUST_MATCH
        ((void (*)(Mtx, s16, u16, GXColor))iconDispGxCol)(mtxOut, 0x30, b,
                                                          SubIconFontColor(dat_8041c134, alpha));
#else
        iconDispGxCol(mtxOut, 0x30, b, SubIconFontColor(dat_8041c134, alpha));
#endif
    }
    FontDrawStart();
    {
        GXColor color = (a == 1) ? SubIconFontColor(unk_80422ff0, alpha)
                                 : SubIconFontColor(*(GXColor*)&float_3p0039_8041c138, alpha);
        FontDrawColor(&color);
    }
    PSMTXTrans(mtxTrans, float_neg104_8041c1ec, float_22_8041c1f0, float_0_8041c144);
    if (flag == 1) {
        f32 scale;
        if (FontGetMessageWidth((char*)c) > 0x96) {
            scale = float_120_8041c1f4 / (f32)FontGetMessageWidth((char*)c);
        } else {
            scale = float_0p8_8041c1a4;
        }
        PSMTXScale(mtxScale, scale, float_0p8_8041c1a4, float_0p8_8041c1a4);
    } else {
        PSMTXScale(mtxScale, float_0p8_8041c1a4, float_0p8_8041c1a4, float_0p8_8041c1a4);
    }
    PSMTXConcat(mtx, mtxTrans, mtxOut);
    PSMTXConcat(mtxOut, mtxScale, mtxOut);
    FontDrawStringMtx(mtxOut, (char*)c);

    if (d != 0) {
        s32 h = d / 100;
        s32 t;
        s32 o;
        s32 rem;
        rem = d - h * 100;
        t = rem / 10;
        o = rem - t * 10;
        digitsD[0] = o;
        digitsD[1] = t;
        digitsD[2] = h;
        if (h > 0) {
            posD[0] = 0x1C;
            posD[1] = 0x0E;
            posD[2] = 0;
            count = 3;
        } else if (t > 0) {
            posD[0] = 0x15;
            posD[1] = 0x07;
            posD[2] = 0;
            count = 2;
        } else {
            posD[0] = 0x0E;
            posD[1] = 0;
            posD[2] = 0;
            count = 1;
        }
        PSMTXScale(mtxScale, float_1_8041c178, float_1_8041c178, float_1_8041c178);
        for (i = 0; i < count; i++) {
            textD[0] = (char)(digitsD[i] + 0x30);
            textD[1] = 0;
            PSMTXTrans(mtxTrans, float_4_8041c1a8 + (float_72_8041c1f8 + (f32)posD[i]), float_25_8041c1fc,
                       float_0_8041c144);
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textD);
        }
        PSMTXTrans(mtxTrans, float_120_8041c1f4, float_22_8041c1f0, float_0_8041c144);
        PSMTXScale(mtxScale, float_0p8_8041c1a4, float_0p8_8041c1a4, float_0p8_8041c1a4);
        PSMTXConcat(mtx, mtxTrans, mtxOut);
        PSMTXConcat(mtxOut, mtxScale, mtxOut);
        FontDrawStringMtx(mtxOut, msgSearch("btl_cost_disp_FP"));
    }

    if (e != 0) {
        s32 h = e / 100;
        s32 t;
        s32 o;
        s32 rem;
        rem = e - h * 100;
        t = rem / 10;
        o = rem - t * 10;
        digitsE[0] = o;
        digitsE[1] = t;
        digitsE[2] = h;
        if (h > 0) {
            posE[0] = 0x1C;
            posE[1] = 0x0E;
            posE[2] = 0;
            count = 3;
        } else if (t > 0) {
            posE[0] = 0x15;
            posE[1] = 0x07;
            posE[2] = 0;
            count = 2;
        } else {
            posE[0] = 0x0E;
            posE[1] = 0;
            posE[2] = 0;
            count = 1;
        }
        PSMTXScale(mtxScale, float_1_8041c178, float_1_8041c178, float_1_8041c178);
        for (i = 0; i < count; i++) {
            textE[0] = (char)(digitsE[i] + 0x30);
            textE[1] = 0;
            PSMTXTrans(mtxTrans, float_4_8041c1a8 + (float_72_8041c1f8 + (f32)posE[i]), float_25_8041c1fc,
                       float_0_8041c144);
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textE);
        }
        PSMTXTrans(mtxTrans, float_120_8041c1f4, float_22_8041c1f0, float_0_8041c144);
        PSMTXScale(mtxScale, float_0p8_8041c1a4, float_0p8_8041c1a4, float_0p8_8041c1a4);
        PSMTXConcat(mtx, mtxTrans, mtxOut);
        PSMTXConcat(mtxOut, mtxScale, mtxOut);
        FontDrawStringMtx(mtxOut, msgSearch("btl_cost_disp_AP"));
    }

    if (table != 0) {
        // HP and max HP are the two s16 halves of the entry's field_0x14 word.
        s32 count2;
        s16 hp;
        s16 maxHp;
        BattleMenuDispEntry* entry;
        s32 h;
        s32 t;
        s32 o;
        s32 rem;
        s32 h2;
        s32 t2;
        s32 o2;
        s32 rem2;
        entry = (BattleMenuDispEntry*)((u8*)table + (index + (s32)window->field_0x98) * 0x18);
        {
            // field_0x14 holds HP in its low half and max HP in its high half as two s16s.
            s16* halves = (s16*)entry;
            hp = halves[10];
            maxHp = halves[11];
        }
        PSMTXTrans(mtxTrans, float_30_8041c200, float_22_8041c1f0, float_0_8041c144);
        PSMTXScale(mtxScale, float_0p8_8041c1a4, float_0p8_8041c1a4, float_0p8_8041c1a4);
        PSMTXConcat(mtx, mtxTrans, mtxOut);
        PSMTXConcat(mtxOut, mtxScale, mtxOut);
        FontDrawStringMtx(mtxOut, msgSearch("btl_disp_HP"));
        h = hp / 100;
        rem = hp - h * 100;
        t = rem / 10;
        o = rem - t * 10;
        digitsH[0] = o;
        digitsH[1] = t;
        digitsH[2] = h;
        if (h > 0) {
            posH[0] = 0x1C;
            posH[1] = 0x0E;
            posH[2] = 0;
            count2 = 3;
        } else if (t > 0) {
            posH[0] = 0x15;
            posH[1] = 0x07;
            posH[2] = 0;
            count2 = 2;
        } else {
            posH[0] = 0x0E;
            posH[1] = 0;
            posH[2] = 0;
            count2 = 1;
        }
        PSMTXScale(mtxScale, float_1_8041c178, float_1_8041c178, float_1_8041c178);
        for (i = 0; i < count2; i++) {
            textH[0] = (char)(digitsH[i] + 0x30);
            textH[1] = 0;
            if ((u32)gp->language == 0) {
                PSMTXTrans(mtxTrans, float_26_8041c204 + (float_30_8041c200 + (f32)posH[i]), float_25_8041c1fc,
                           float_0_8041c144);
            } else {
                PSMTXTrans(mtxTrans, float_28_8041c1ac + (float_30_8041c200 + (f32)posH[i]), float_25_8041c1fc,
                           float_0_8041c144);
            }
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textH);
        }
        if ((u32)gp->language == 0) {
            PSMTXTrans(mtxTrans, float_92_8041c208, float_22_8041c1f0, float_0_8041c144);
        } else {
            PSMTXTrans(mtxTrans, float_100_8041c20c, float_22_8041c1f0, float_0_8041c144);
        }
        PSMTXScale(mtxScale, float_0p8_8041c1a4, float_0p8_8041c1a4, float_0p8_8041c1a4);
        PSMTXConcat(mtx, mtxTrans, mtxOut);
        PSMTXConcat(mtxOut, mtxScale, mtxOut);
        FontDrawStringMtx(mtxOut, msgSearch("btl_disp_slash"));
        h2 = maxHp / 100;
        rem2 = maxHp - h2 * 100;
        t2 = rem2 / 10;
        o2 = rem2 - t2 * 10;
        digitsH[0] = o2;
        digitsH[1] = t2;
        digitsH[2] = h2;
        if (h2 > 0) {
            posH[0] = 0x1C;
            posH[1] = 0x0E;
            posH[2] = 0;
            count2 = 3;
        } else if (t2 > 0) {
            posH[0] = 0x15;
            posH[1] = 0x07;
            posH[2] = 0;
            count2 = 2;
        } else {
            posH[0] = 0x0E;
            posH[1] = 0;
            posH[2] = 0;
            count2 = 1;
        }
        PSMTXScale(mtxScale, float_1_8041c178, float_1_8041c178, float_1_8041c178);
        for (i = 0; i < count2; i++) {
            textH[0] = (char)(digitsH[i] + 0x30);
            textH[1] = 0;
            if ((u32)gp->language == 0) {
                PSMTXTrans(mtxTrans, float_76_8041c210 + (float_30_8041c200 + (f32)posH[i]), float_25_8041c1fc,
                           float_0_8041c144);
            } else {
                PSMTXTrans(mtxTrans, float_78_8041c214 + (float_30_8041c200 + (f32)posH[i]), float_25_8041c1fc,
                           float_0_8041c144);
            }
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textH);
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

void DrawMainIconHukidasi(BattleMenuWork* window, void* entryPtr, Mtx* mtx, f32 scale) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuDispEntry* entry = entryPtr;
    CameraEntry* cam = camGetPtr(8);
    GXColor color;
    GXTexObj texObj;
    Mtx rotMtx;
    Mtx scaleMtx;
    Mtx transMtx;
    Mtx resultMtx;
    f32 f31;
    f32 f30;
    u16 iconId;
    const char* msg;

    btlDispGXInit2D();
    GXSetTevColor(1, dat_8041c118);
    GXSetNumTevStages(1);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xA, 2, 0xF);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 5, 4, 7);
    GXSetTevOrder(0, 0, 0, 4);
    PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
    PSMTXTrans(transMtx, 0.0f, -5.0f, 0.0f);
    f31 = 0.017453292f * scale;
    PSMTXRotRad(rotMtx, 'y', f31);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    PSMTXConcat(cam->view, resultMtx, resultMtx);
    GXLoadPosMtxImm(resultMtx, 0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, 1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    if (scale >= 0.0f && scale <= 180.0f) {
        f30 = 4.0f - 8.0f * scale / 180.0f;
    }
    if (scale >= 180.0f && scale <= 360.0f) {
        f30 = -4.0f + 8.0f * (scale - 180.0f) / 180.0f;
    }

    if (window->field_0x1C == 0) {
        btlDispGXQuads2D(64.0f + f30, 76.0f, -64.0f + f30, -4.0f, 0, 0, 0, 0x80);
        btlDispGXQuads2D(64.0f, 80.0f, -64.0f, 0.0f, 0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        btlDispGXQuads2D(-64.0f + f30, 76.0f, 64.0f + f30, -4.0f, 0, 0, 0, 0x80);
        btlDispGXQuads2D(-64.0f, 80.0f, 64.0f, 0.0f, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    PSMTXTrans(transMtx, -67.0f, 20.0f, 0.0f);
    PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
    PSMTXRotRad(rotMtx, 'y', f31);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    iconId = entry->field_0xC;
    if (iconId == 0) {
        iconId = 0x18B;
    }
    if (entry->field_0x4 == 1) {
#ifdef MUST_MATCH
        // iconDispGx2's header says s16 but the original reads a zero-extended u16 (no extsh).
        ((void (*)(Mtx, s16, u16))iconDispGx2)(resultMtx, 0x10, iconId);
#else
        iconDispGx2(resultMtx, 0x10, iconId);
#endif
    } else {
#ifdef MUST_MATCH
        ((void (*)(Mtx, s16, u16))iconDispGx2)(resultMtx, 0x30, iconId);
#else
        iconDispGx2(resultMtx, 0x30, iconId);
#endif
    }

    msg = (const char*)entry->field_0x8;
    PSMTXTrans(transMtx, 0.0f, 52.5f, 0.0f);
    PSMTXScale(scaleMtx, 0.9f, 0.9f, 0.9f);
    PSMTXRotRad(rotMtx, 'y', f31);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    FontDrawStart();
    if (entry->field_0x4 == 1) {
        color = dat_8041c11c;
    } else {
        color = dat_8041c120;
    }
#ifdef MUST_MATCH
    // FontDrawColor's header says GXColor* but the original takes GXColor by value (which MWCC
    // lowers to a caller-allocated copy); the cast restores that ABI.
    ((void (*)(GXColor))FontDrawColor)(color);
#else
    FontDrawColor(&color);
#endif
    FontDrawStringCenterMtx(resultMtx, msg);
}

// Quad corner / texture-coordinate tables used by DrawMainIconCircle (0x80 each).
typedef struct MainIconCirclePoint {
    f32 x;
    f32 y;
} MainIconCirclePoint;

typedef struct MainIconCircleTable {
    MainIconCirclePoint v[4][4];
} MainIconCircleTable;

extern const MainIconCircleTable dat_802ef068;
extern const MainIconCircleTable dat_802ef0e8;
extern const GXColor dat_8041c124;
extern const GXColor dat_8041c128;
extern const GXColor dat_8041c12c;

#ifdef MUST_MATCH
// Passing the colour through this by-value inline helper (instead of a named local) is what
// makes MWCC place the GXColor argument temp in the original's stack slot.
static inline GXColor MainIconCircleColor(GXColor base, u8 a) {
    base.r = a;
    base.g = a;
    base.b = a;
    return base;
}
#endif

void DrawMainIconCircle(BattleMenuWork* window, void* entryPtr, Mtx* mtx, Vec* pos, f32 scale) {
    BattleMenuDispEntry* entry = entryPtr;
    BattleWork* wp = _battleWorkPointer;
    CameraEntry* cam;
    MainIconCircleTable posTable;
    MainIconCircleTable texCoord;
    Mtx rotMtx;
    Mtx scaleMtx;
    Mtx transMtx;
    Mtx resultMtx;
    GXTexObj texObj;
    f32 radians;
    f32 offset;
    f32 shadowY;
    f32 size;
    u8 alpha;
    s32 iconId;
    s32 i;
    s32 j;

    cam = camGetPtr(8);
    alpha = (u8)(0xC0 - (s32)(1.5f * pos->z));
    btlDispGXInit2D();
    GXSetTevColor(1, dat_8041c124);
    GXSetNumTevStages(1);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xA, 2, 0xF);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 5, 4, 7);
    GXSetTevOrder(0, 0, 0, 4);
    size = 1.3f - 0.005f * pos->z;
    PSMTXScale(scaleMtx, size, size, 1.0f);
    radians = 0.017453292f * scale;
    PSMTXRotRad(rotMtx, 'y', radians);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    PSMTXConcat(cam->view, resultMtx, resultMtx);
    GXLoadPosMtxImm(resultMtx, 0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, 0);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    if (scale >= 0.0f && scale <= 180.0f) {
        offset = (4.0f - 8.0f * scale / 180.0f) / 1.3f;
    }
    if (scale >= 180.0f && scale <= 360.0f) {
        offset = (-4.0f + 8.0f * (scale - 180.0f) / 180.0f) / 1.3f;
    }
    posTable = dat_802ef068;
    texCoord = dat_802ef0e8;
    shadowY = -3.0769231f;
    for (i = 0; i < 4; i++) {
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        for (j = 0; j < 7; j += 2) {
            btlDispGXPoint2D(offset + ((f32*)posTable.v[i])[j], shadowY + ((f32*)posTable.v[i])[j + 1], 0, 0, 0,
                             0x80, ((f32*)texCoord.v[i])[j], ((f32*)texCoord.v[i])[j + 1]);
        }
    }
    for (i = 0; i < 4; i++) {
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        for (j = 0; j < 7; j += 2) {
            btlDispGXPoint2D(((f32*)posTable.v[i])[j], ((f32*)posTable.v[i])[j + 1], alpha, alpha, alpha, 0xFF,
                             ((f32*)texCoord.v[i])[j], ((f32*)texCoord.v[i])[j + 1]);
        }
    }
    PSMTXTrans(transMtx, 0.0f, 7.5f, 0.0f);
    size = 0.66f * (1.3f - 0.005f * pos->z);
    PSMTXScale(scaleMtx, size, size, 1.0f);
    PSMTXRotRad(rotMtx, 'y', radians);
    PSMTXIdentity(rotMtx);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    iconId = entry->field_0xC;
    if ((u16)iconId == 0) {
        iconId = 0x18B;
    }
    if (entry->field_0x4 == 1) {
#ifdef MUST_MATCH
        iconDispGxCol(resultMtx, 0x10, iconId, MainIconCircleColor(dat_8041c128, alpha));
#else
        GXColor color = dat_8041c128;
        color.r = alpha;
        color.g = alpha;
        color.b = alpha;
        iconDispGxCol(resultMtx, 0x10, iconId, color);
#endif
    } else {
#ifdef MUST_MATCH
        iconDispGxCol(resultMtx, 0x30, iconId, MainIconCircleColor(dat_8041c12c, alpha));
#else
        GXColor color = dat_8041c12c;
        color.r = alpha;
        color.g = alpha;
        color.b = alpha;
        iconDispGxCol(resultMtx, 0x30, iconId, color);
#endif
    }
}

// The original emits real `bl GetRingOffset`/`bl GetRingCenter` calls from
// DrawSubMenuCommonProcess (and DrawMainMenu), so keep them out of line.
#pragma dont_inline on
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
#pragma dont_inline reset
