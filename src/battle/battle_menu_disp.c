#include "battle/battle_menu_disp.h"
#include "system.h"
#include "manager/fontmgr.h"
#include "battle/battle_disp.h"

extern BattleWork* _battleWorkPointer;
extern int sprintf(char* str, const char* format, ...);
extern f64 cos(f64 x);
extern f64 sin(f64 x);
extern void windowDispGX_Waku_col(s32 texMap, GXColor* color, f32 x, f32 y, f32 h, f32 w, f32 curve);
extern void FontDrawStart(void);
extern void FontDrawScale(f32 scale);
extern void FontDrawString(const char* str, f32 x, f32 y);
extern const char* msgSearch(const char* tag);
extern void btlGetScreenPoint(Vec* pos, Vec* out);

void DrawMultiItemWin(void);
void DrawChangePartyWin(void);
void DrawOperationWin(void);
void DrawWeaponWin(void);
void DrawMainMenu(void);

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
    u8 field_0x114[0x124 - 0x114];
} BattleMenuWork;

void InitSubMenuCommonProcess(BattleMenuWork** window, BattleCursor** cursor, BattleCursor* menu);
void InitSubMenuCommonProcess2(BattleMenuWork* window, BattleCursor* cursor);
void InitSubMenuCommonProcess3(BattleMenuWork* window, BattleCursor* cursor);

//.sbss
s8 seq;

//.sdata
GXColor seleItemCoordCol = {0xFF, 0xC0, 0xC0, 0xCF};

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

// NONMATCHING (85.16%): 1 instruction differs, 1 missing, 3 extra.
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

// NONMATCHING (83.10%): 1 instruction differs, 1 missing, 3 extra, plus register differences.
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
// NONMATCHING (7.97%): 11 instructions differ, 6 missing, 53 extra, plus register differences.
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

// NONMATCHING (49.16%): 4 instructions differ, 22 missing, 46 extra, plus register differences.
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

// NONMATCHING (80.50%): 1 instruction differs, 5 missing, 12 extra, plus register differences.
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
            {
                s32 v = a->unk8 + i;
                w->field_0x84[idx] = v - 1;
            }
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

// NONMATCHING (90.09%): 6 instructions differ, 6 missing, 1 extra, plus register differences.
void DrawMenuHelpWin(const char* tag, f32 x, f32 y) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0xCF};
    s8 text[4][0x30];
    s8* sp = (s8*)tag;
    s8* p;
    s8* dp;
    s32 line;
    s32 i;

    windowDispGX_Waku_col(0, &color, x, y, 400.0f, 50.0f, 10.0f);
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
        FontDrawString("\x82\xB9\x82\xC2\x82\xDF\x82\xA2\x82\xCD\x81\x40\x82\xC8\x82\xA2\x82\xCC\x82\xBE\x81\x49\x81\x49",
                       x + 10.0f, y - 4.0f);
    }
}

void DrawMenuPartyChangeButton_Sub(void* tex, f32 x, f32 y, f32 angle) {
    Mtx mtx;

    PSMTXRotRad(mtx, 'y', 0.017453f * angle);
    PSMTXTransApply(mtx, mtx, x, y, 0.0f);
    {
        GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
        btlDispTexPlane2(mtx, (u32)tex, color, 0);
    }
}

// NONMATCHING (83.93%): 1 instruction differs, 1 missing, 3 extra, plus register differences.
void GetRingOffset(f32* x, f32* y, f32 angle) {
    f32 a = 4.7124f - angle;
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
