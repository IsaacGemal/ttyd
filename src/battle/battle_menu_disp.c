#include "sdk/texPalette.h"
#include "data/item_data.h"
#include "manager/filemgr.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "driver/windowdrv.h"
#include "battle/battle_menu_disp.h"
#include "system.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"
#include <stdio.h>
#include "math_ext.h"

extern BattleWork* _battleWorkPointer;
extern ItemData itemDataTable[];


extern const f32 float_0_8041c144;

// Layout of the battle menu scratch buffer allocated by
// InitSubMenuCommonProcess (0x124 bytes). Only the fields this file touches
// are declared; the rest stay opaque.
typedef struct BattleMenuWork {
    BattleCursor* cursor;     // 0x0
    BattleCommandAction* mainIconTable; // 0x4, passed as the table to DrawMainIcon (set from the Init argument)
    void* weaponEntries;      // 0x8, BattleMenuWeaponEntry[] for the weapon menu (set from the Init argument)
    s32 phase;                // 0xC, ring/scroll state: 0, 1, 2, 0xA, 0x14, 0x1E, 0x28
    s32 field_0x10;           // 0x10
    u8 inputReady;            // 0x14, read by BattleMenuKeyOKInACT
    u8 field_0x15[0x18 - 0x15];
    s32 lastCursorPos;        // 0x18, cursor->field_0 when the ring last turned
    s32 ringMoving;           // 0x1C, set while an icon is still moving; Hukidasi mirrors its bubble on it
    s32 iconFlags[5];         // 0x20, bit 1: draw the icon, bit 2: speech-bubble style
    s32 iconState[5];         // 0x34
    s32 iconFrame[10];        // 0x48
    s32 iconFromSlot[5];      // 0x70, slot the icon rotated from (start angle of kakutable)
    s32 iconSlot[5];          // 0x84, ring slot index into kakutable
    f32 scrollPos;            // 0x98, first visible item (fractional while scrolling)
    f32 scrollFrom;           // 0x9C
    f32 scrollTo;             // 0xA0
    s32 slotFlags[7];         // 0xA4, bit 1: slot is drawn
    s32 slotState[7];         // 0xC0
    s32 slotFrame[7];         // 0xDC
    s32 field_0xF8[7];        // 0xF8
    u8 field_0x114[0x11C - 0x114];
    s32 bounceFrame;          // 0x11C, cursor arrow bounce, wraps at 12
    f32 partyBtnAngle;        // 0x120, party change button angle (90 down to 0)
} BattleMenuWork;





//.sbss
s8 seq;

//.sdata
GXColor seleItemCoordCol = {0xFF, 0xC0, 0xC0, 0xCF};

//.data
f32 kakutable[3][5] = {
    {240.0f, 120.0f, 0.0f, 0.0f, 0.0f},
    {270.0f, 180.0f, 90.0f, 0.0f, 0.0f},
    {270.0f, 203.0f, 157.0f, 90.0f, 0.0f},
};

// The multi-item table uses the same 0x18 stride with its own layout.
typedef struct BattleMenuMultiEntry {
    s32 enabled;         // 0x0
    const char* label;   // 0x4
    const char* helpMsg; // 0x8
    u16 iconId;          // 0xC
    u16 field_0xE;       // 0xE
    s32 fpCost;          // 0x10
    s32 field_0x14;      // 0x14
} BattleMenuMultiEntry; // 0x18

typedef struct BattleMenuDispEntryOp {
    s32 field_0x0;       // 0x0
    s32 enabled;         // 0x4
    s32 field_0x8;       // 0x8
    const char* label;   // 0xC
    u16 iconId;          // 0x10
    u16 field_0x12;      // 0x12
    const char* helpMsg; // 0x14
    s32 fpCost;          // 0x18
} BattleMenuDispEntryOp; // 0x1C

// Per-weapon entries behind BattleMenuWork.field_8 (stride 0x1C).
typedef struct BattleMenuWeaponEntry {
    BattleWeapon* weapon;  // 0x0
    s32 enabled;           // 0x4
    const char* label;     // 0x8
    s32 field_0xC;         // 0xC
    u16 field_0x10;        // 0x10
    u8 field_0x12;         // 0x12
    u8 field_0x13[0x14 - 0x13];
    s32 itemId;            // 0x14
    s32 field_0x18;        // 0x18
} BattleMenuWeaponEntry; // 0x1C

extern BattleWeapon ItemWeaponData_CookingItem;

// In .rodata: `const` makes MWCC emit lis/addi instead of @sda21 loads.

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

typedef struct MainIconCirclePoint {
    f32 x;
    f32 y;
} MainIconCirclePoint;
typedef struct MainIconCircleTable {
    MainIconCirclePoint v[4][4];
} MainIconCircleTable;

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
    if (wp->commandMenu.restoreState == 0 &&
        ((BattleMenuWork*)wp->commandMenu.field_55C)->inputReady == 0) {
        return FALSE;
    }
    return TRUE;
}

void battleMenuDispInit(void) {
	BattleWork* wp = _battleWorkPointer;
	char path[256];

	sprintf(path, "%s/battle/common/battle_common.tpl", getMarioStDvdRoot());
	wp->menuTex = fileAlloc(path, FILE_TYPE_TPL);
}

void battleMenuDispEnd(void) {
	fileFree(_battleWorkPointer->menuTex);
}

#pragma dont_inline on
void GetRingCenter(Vec* out) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    btlGetScreenPoint(&unit->mPosition, out);
    out->x += 22.5f;
    out->y += 105.0f;
    out->z = float_0_8041c144;
    if (out->x <= -180.0f) {
        out->x = -180.0f;
    }
}

// The original emits real `bl GetRingOffset`/`bl GetRingCenter` calls from
// DrawSubMenuCommonProcess (and DrawMainMenu), so keep them out of line.
void GetRingOffset(f32* x, f32* y, f32 angle) {
    f32 a = 4.712389f - angle;
    f32 c = 80.0f * (f32)cos(a);
    *x = c;
    *y = 35.0f * (f32)sin(a);
}
#pragma dont_inline reset

void DrawMainIcon(BattleMenuWork* window, BattleCommandAction* table, Mtx* mtxA, Vec* vecB, f32* scales) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    BattleCursor* cursor;
    Vec vec;
    s32 order[6];
    f32 sorted[6];
    s32 i;
    s32 j;

    if (unit->currentKind == UNIT_MARIO) {
        cursor = &wp->commandMenu.cursor[0];
    } else {
        cursor = &wp->commandMenu.cursor[1];
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
        s32 flags = window->iconFlags[idx];
        if (flags & 1) {
            if (flags & 2) {
                DrawMainIconHukidasi(window, &table[idx], &mtxA[idx], scales[idx]);
            } else {
                vec = vecB[idx];
                DrawMainIconCircle(window, &table[idx], &mtxA[idx], &vec, scales[idx]);
            }
        }
    }
}

void DrawMainIconHukidasi(BattleMenuWork* window, BattleCommandAction* entry, Mtx* mtx, f32 scale) {
    BattleWork* wp = _battleWorkPointer;
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
    GXSetTevColor(1, ((GXColor){ 0xE1,0xE1,0xE1,0xFF }));
    GXSetNumTevStages(1);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xA, 2, 0xF);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 5, 4, 7);
    GXSetTevOrder(0, 0, 0, 4);
    PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
    PSMTXTrans(transMtx, float_0_8041c144, -5.0f, float_0_8041c144);
    f31 = 0.017453292f * scale;
    PSMTXRotRad(rotMtx, 'y', f31);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    PSMTXConcat(cam->view, resultMtx, resultMtx);
    GXLoadPosMtxImm(resultMtx, 0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, 1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    if (scale >= float_0_8041c144 && scale <= 180.0f) {
        f30 = 4.0f - 8.0f * scale / 180.0f;
    }
    if (scale >= 180.0f && scale <= 360.0f) {
        f30 = -4.0f + 8.0f * (scale - 180.0f) / 180.0f;
    }

    if (window->ringMoving == 0) {
        btlDispGXQuads2D(64.0f + f30, 76.0f, -64.0f + f30, -4.0f, 0, 0, 0, 0x80);
        btlDispGXQuads2D(64.0f, 80.0f, -64.0f, float_0_8041c144, 0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        btlDispGXQuads2D(-64.0f + f30, 76.0f, 64.0f + f30, -4.0f, 0, 0, 0, 0x80);
        btlDispGXQuads2D(-64.0f, 80.0f, 64.0f, float_0_8041c144, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    PSMTXTrans(transMtx, -67.0f, 20.0f, float_0_8041c144);
    PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
    PSMTXRotRad(rotMtx, 'y', f31);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    iconId = entry->mIcon;
    if (iconId == 0) {
        iconId = 0x18B;
    }
    if (entry->mEnabled == 1) {
        iconDispGx2(resultMtx, 0x10, iconId);
    } else {
        iconDispGx2(resultMtx, 0x30, iconId);
    }

    msg = entry->mName;
    PSMTXTrans(transMtx, float_0_8041c144, 52.5f, float_0_8041c144);
    PSMTXScale(scaleMtx, 0.9f, 0.9f, 0.9f);
    PSMTXRotRad(rotMtx, 'y', f31);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    FontDrawStart();
    if (entry->mEnabled == 1) {
        color = ((GXColor){ 0x00,0x00,0x00,0xFF });
    } else {
        color = ((GXColor){ 0x80,0x80,0x80,0xFF });
    }
    FontDrawColor(color);
    FontDrawStringCenterMtx(resultMtx, msg);
}

// Quad corner / texture-coordinate tables used by DrawMainIconCircle (0x80 each).

void DrawMainIconCircle(BattleMenuWork* window, BattleCommandAction* entry, Mtx* mtx, Vec* pos, f32 scale) {
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
    GXSetTevColor(1, ((GXColor){ 0xE1,0xE1,0xE1,0xFF }));
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
    if (scale >= float_0_8041c144 && scale <= 180.0f) {
        offset = (4.0f - 8.0f * scale / 180.0f) / 1.3f;
    }
    if (scale >= 180.0f && scale <= 360.0f) {
        offset = (-4.0f + 8.0f * (scale - 180.0f) / 180.0f) / 1.3f;
    }
    posTable = (MainIconCircleTable){ { { { -20.0f, 40.0f }, { 0.0f, 40.0f }, { 0.0f, 20.0f }, { -20.0f, 20.0f } }, { { 20.0f, 40.0f }, { 20.0f, 20.0f }, { 0.0f, 20.0f }, { 0.0f, 40.0f } }, { { -20.0f, 20.0f }, { 0.0f, 20.0f }, { 0.0f, 0.0f }, { -20.0f, 0.0f } }, { { 0.0f, 20.0f }, { 20.0f, 20.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f } } } };
    texCoord = (MainIconCircleTable){ { { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } }, { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f } }, { { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f } }, { { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } } } };
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
    PSMTXTrans(transMtx, float_0_8041c144, 7.5f, float_0_8041c144);
    size = 0.66f * (1.3f - 0.005f * pos->z);
    PSMTXScale(scaleMtx, size, size, 1.0f);
    PSMTXRotRad(rotMtx, 'y', radians);
    PSMTXIdentity(rotMtx);
    PSMTXConcat(*mtx, rotMtx, resultMtx);
    PSMTXConcat(resultMtx, scaleMtx, resultMtx);
    PSMTXConcat(resultMtx, transMtx, resultMtx);
    iconId = entry->mIcon;
    if ((u16)iconId == 0) {
        iconId = 0x18B;
    }
    if (entry->mEnabled == 1) {
        iconDispGxCol(resultMtx, 0x10, iconId, (GXColor){ alpha, alpha, alpha, 0xFF });
    } else {
        iconDispGxCol(resultMtx, 0x30, iconId, (GXColor){ alpha, alpha, alpha, 0xFF });
    }
}

// dont_inline: keep deferred inlining from pulling same-TU helpers into DrawSubIcon/DrawMainIcon.
#pragma dont_inline on
void DrawSubIcon(BattleMenuWork* window, Mtx* mtxBase, s32* a, u16* b, s32* c, s32* d, s32* e, BattleCommandParty* table, s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    BattleCursor* cursor;
    Mtx mtx;
    Mtx result[7];
    f32 f31;
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    cursor = window->cursor;
    f31 = window->scrollPos - (f32)(s32)window->scrollPos;
    f31 = f31 * 24.0f;
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->scrollPos + i;
        if (idx < cursor->unk8) {
            if (window->slotFlags[i] & 1) {
                PSMTXTrans(mtx, 100.0f, 35.0f - 24.0f * (f32)i + f31, float_0_8041c144);
                PSMTXConcat(mtx, mtxBase[i], result[i]);
                DrawSubIconSub(window, i, result[i], a[i], b[i], c[i], d[i], e[i], table, flag);
            }
        }
    }
}
#pragma dont_inline reset

// Builds the text colour the sub icons tint their labels with.
static inline GXColor SubIconFontColor(GXColor c, s32 alpha) {
    c.a = alpha;
    return c;
}

// Sets the label colour; the colour is passed by value.
static inline void SubIconDrawColor(GXColor c) {
    FontDrawColor(c);
}

void DrawSubIconSub(BattleMenuWork* window, s32 index, Mtx mtx, s32 a, u16 b, s32 c, s32 d, s32 e, BattleCommandParty* table,
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
    s32 item;

    frac = window->scrollPos - (f32)(s32)window->scrollPos;
    alpha = 0xFF;
    if (index == 0) {
        alpha = (s32)(255.0f * (1.0f - frac));
    }
    if (index == 6) {
        alpha = (s32)(255.0f * frac);
    }
    item = (s32)window->scrollPos + index;
    if (item == cursor->field_0) {
        cr = seleItemCoordCol.r;
        cg = seleItemCoordCol.g;
        cb = seleItemCoordCol.b;
        ca = alpha;
    } else {
        cb = 0xFF;
        cg = 0xFF;
        cr = 0xFF;
        ca = (s32)(144.0f * (f32)(u8)alpha / 255.0f);
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
        (-152.0f), 24.0f, (-136.0f), float_0_8041c144, cr, cg, cb, ca);
    ((void (*)(f32, f32, f32, f32, u8, u8, u8, s32))btlDispGXQuads2D)(
        152.0f, 24.0f, 136.0f, float_0_8041c144, cr, cg, cb, ca);
#else
    btlDispGXQuads2D((-152.0f), 24.0f, (-136.0f), float_0_8041c144, cr,
                     cg, cb, ca);
    btlDispGXQuads2D(152.0f, 24.0f, 136.0f, float_0_8041c144, cr, cg, cb,
                     ca);
#endif
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->menuTex->data[0], &texObj, 3);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
    ((void (*)(f32, f32, f32, f32, u8, u8, u8, s32))btlDispGXQuads2D)(
        (-136.0f), 24.0f, 136.0f, float_0_8041c144, cr, cg, cb, ca);
#else
    btlDispGXQuads2D((-136.0f), 24.0f, 136.0f, float_0_8041c144, cr, cg, cb,
                     ca);
#endif
    PSMTXTrans(mtxTrans, (-119.0f), 2.0f, float_0_8041c144);
    PSMTXScale(mtxScale, 0.5f, 0.5f, 1.0f);
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
                                                          SubIconFontColor(((GXColor){ 0xFF,0xFF,0xFF,0x00 }), alpha));
#else
        iconDispGxCol(mtxOut, 0x10, b, SubIconFontColor(((GXColor){ 0xFF,0xFF,0xFF,0x00 }), alpha));
#endif
    } else {
#ifdef MUST_MATCH
        ((void (*)(Mtx, s16, u16, GXColor))iconDispGxCol)(mtxOut, 0x30, b,
                                                          SubIconFontColor(((GXColor){ 0xFF,0xFF,0xFF,0x00 }), alpha));
#else
        iconDispGxCol(mtxOut, 0x30, b, SubIconFontColor(((GXColor){ 0xFF,0xFF,0xFF,0x00 }), alpha));
#endif
    }
    FontDrawStart();
    {
        GXColor color;
        if (a == 1) {
            color = SubIconFontColor(((GXColor){ 0x00, 0x00, 0x00, 0x00 }), alpha);
        } else {
            color = SubIconFontColor(((GXColor){ 0x40, 0x40, 0x40, 0x00 }), alpha);
        }
        SubIconDrawColor(color);
    }
    PSMTXTrans(mtxTrans, (-104.0f), 22.0f, float_0_8041c144);
    if (flag == 1) {
        f32 scale;
        if (FontGetMessageWidth((char*)c) > 0x96) {
            scale = 120.0f / (f32)FontGetMessageWidth((char*)c);
        } else {
            scale = 0.8f;
        }
        PSMTXScale(mtxScale, scale, 0.8f, 0.8f);
    } else {
        PSMTXScale(mtxScale, 0.8f, 0.8f, 0.8f);
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
        PSMTXScale(mtxScale, 1.0f, 1.0f, 1.0f);
        for (i = 0; i < count; i++) {
            textD[0] = (char)(digitsD[i] + 0x30);
            textD[1] = 0;
            PSMTXTrans(mtxTrans, 4.0f + (72.0f + (f32)posD[i]), 25.0f,
                       float_0_8041c144);
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textD);
        }
        PSMTXTrans(mtxTrans, 120.0f, 22.0f, float_0_8041c144);
        PSMTXScale(mtxScale, 0.8f, 0.8f, 0.8f);
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
        PSMTXScale(mtxScale, 1.0f, 1.0f, 1.0f);
        for (i = 0; i < count; i++) {
            textE[0] = (char)(digitsE[i] + 0x30);
            textE[1] = 0;
            PSMTXTrans(mtxTrans, 4.0f + (72.0f + (f32)posE[i]), 25.0f,
                       float_0_8041c144);
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textE);
        }
        PSMTXTrans(mtxTrans, 120.0f, 22.0f, float_0_8041c144);
        PSMTXScale(mtxScale, 0.8f, 0.8f, 0.8f);
        PSMTXConcat(mtx, mtxTrans, mtxOut);
        PSMTXConcat(mtxOut, mtxScale, mtxOut);
        FontDrawStringMtx(mtxOut, msgSearch("btl_cost_disp_AP"));
    }

    if (table != 0) {
        s32 count2;
        s16 hp;
        s16 maxHp;
        BattleCommandParty* entry;
        s32 h;
        s32 t;
        s32 o;
        s32 rem;
        s32 h2;
        s32 t2;
        s32 o2;
        s32 rem2;
        entry = &table[index + (s32)window->scrollPos];
        hp = entry->mCurrentHP;
        maxHp = entry->mMaxHP;
        PSMTXTrans(mtxTrans, 30.0f, 22.0f, float_0_8041c144);
        PSMTXScale(mtxScale, 0.8f, 0.8f, 0.8f);
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
        PSMTXScale(mtxScale, 1.0f, 1.0f, 1.0f);
        for (i = 0; i < count2; i++) {
            textH[0] = (char)(digitsH[i] + 0x30);
            textH[1] = 0;
            if ((u32)gp->language == 0) {
                PSMTXTrans(mtxTrans, 26.0f + (30.0f + (f32)posH[i]), 25.0f,
                           float_0_8041c144);
            } else {
                PSMTXTrans(mtxTrans, 28.0f + (30.0f + (f32)posH[i]), 25.0f,
                           float_0_8041c144);
            }
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textH);
        }
        if ((u32)gp->language == 0) {
            PSMTXTrans(mtxTrans, 92.0f, 22.0f, float_0_8041c144);
        } else {
            PSMTXTrans(mtxTrans, 100.0f, 22.0f, float_0_8041c144);
        }
        PSMTXScale(mtxScale, 0.8f, 0.8f, 0.8f);
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
        PSMTXScale(mtxScale, 1.0f, 1.0f, 1.0f);
        for (i = 0; i < count2; i++) {
            textH[0] = (char)(digitsH[i] + 0x30);
            textH[1] = 0;
            if ((u32)gp->language == 0) {
                PSMTXTrans(mtxTrans, 76.0f + (30.0f + (f32)posH[i]), 25.0f,
                           float_0_8041c144);
            } else {
                PSMTXTrans(mtxTrans, 78.0f + (30.0f + (f32)posH[i]), 25.0f,
                           float_0_8041c144);
            }
            PSMTXConcat(mtx, mtxTrans, mtxOut);
            PSMTXConcat(mtxOut, mtxScale, mtxOut);
            FontDrawStringMtx(mtxOut, textH);
        }
    }
}

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
            if (window->partyBtnAngle > float_0_8041c144) {
                window->partyBtnAngle -= 5.0f;
            } else {
                window->partyBtnAngle = float_0_8041c144;
            }
            btlGetScreenPoint(&mario->mPosition, &screen1);
            DrawMenuPartyChangeButton_Sub(5, screen1.x, screen1.y - 20.0f, window->partyBtnAngle);
        } else {
            window->partyBtnAngle = 90.0f;
        }
    } else {
        btlGetScreenPoint(&mario->mPosition, &screen1);
        btlGetScreenPoint(&party->mPosition, &screen2);
        f31 = (screen1.x + screen2.x) / 2.0f;
        f30 = screen1.y;
        BattleAudience_GetItemOn(&memberId, &itemX, &itemY, &itemZ, 0);
        if (memberId != -1) {
            if (window->partyBtnAngle == 90.0f) {
                psndSFXOn("SFX_BTL_SYS_CAUTION_THROW1");
            }
            if (window->partyBtnAngle > float_0_8041c144) {
                window->partyBtnAngle -= 5.0f;
            } else {
                window->partyBtnAngle = float_0_8041c144;
            }
            DrawMenuPartyChangeButton_Sub(5, f31, f30 - 20.0f, window->partyBtnAngle);
        } else {
            window->partyBtnAngle = 90.0f;
            if (BattleCommandCheckChangePositionEnable(wp)) {
                DrawMenuPartyChangeButton_Sub(4, f31, f30 - 20.0f, float_0_8041c144);
            }
        }
    }
}

// _Sub is a real call in the original; without this MWCC inlines it into DrawMenuPartyChangeButton.
#pragma dont_inline on
void DrawMenuPartyChangeButton_Sub(u32 tex, f32 x, f32 y, f32 angle) {
    Mtx mtx;

    PSMTXRotRad(mtx, 'y', 0.017453292f * angle);
    PSMTXTransApply(mtx, mtx, x, y, float_0_8041c144);
    {
        GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
        btlDispTexPlane2(mtx, tex, color, 0);
    }
}
#pragma dont_inline reset

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
        PSMTXTrans(mtx1, pos.x - 20.0f, 60.0f + pos.y, float_0_8041c144);
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
        PSMTXTrans(mtx1, pos.x - 20.0f, 60.0f + pos.y, float_0_8041c144);
        PSMTXConcat(cam->view, mtx1, mtx2);
        GXLoadPosMtxImm(mtx2, 0);
        btlDispGXQuads2D(-28.0f, 12.0f, 28.0f, -12.0f, cr, cg, cb, ca);
    }
}

// The original reuses its `tag` parameter as the message pointer (`mr. r31, r3`
// after msgSearch); writing the result back into `tag` reproduces the register
// allocation, and the copy loop needs `*dp = *p; p++; dp++;` for the p/dp
// increment order.
void DrawMenuHelpWin(const char* tag, f32 x, f32 y) {
    GXColor color = ((GXColor){ 0xFF,0xFF,0xFF,0xCF });
    s8 text[4][0x30];
    s8* p;
    s8* dp;
    s32 line;
    s32 i;

    windowDispGX_Waku_col(x, y, 400.0f, 50.0f, 10.0f, 0, color);
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
        FontDrawString(x + 20.0f, y - 4.0f, (char*)text[0]);
        if (text[1][0] != 0) {
            FontDrawScale(0.8f);
        }
        FontDrawString(x + 20.0f, y - 28.0f, (char*)text[1]);
        if (text[2][0] != 0) {
            FontDrawScale(0.8f);
        }
        FontDrawString(x + 20.0f, y - 52.0f, (char*)text[2]);
    } else {
        FontDrawString(x + 10.0f, y - 4.0f,
                       "\x82\xB9\x82\xC2\x82\xDF\x82\xA2\x82\xCD\x81\x40\x82\xC8\x82\xA2\x82\xCC\x82\xBE\x81\x49\x81\x49");
    }
}

// The original emits a real `bl` to this function from every caller, so it must
// be marked don't-inline itself (a surrounding region only stops its functions
// being inlined elsewhere).
#pragma dont_inline on
void DrawMenuCursorAndScrollArrow(Vec pos) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    Vec vec;
    f32 bounce;
    s32 n;

    bounce = intplGetValue(4, window->bounceFrame, 6, float_0_8041c144, 10.0f);
    window->bounceFrame++;
    if (window->bounceFrame >= 12) {
        window->bounceFrame = 0;
    }
    if (cursor->field_4 > 0) {
        vec.x = 155.0f + pos.x;
        vec.y = 15.0f + pos.y + bounce;
        vec.z = float_0_8041c144;
        iconDispGx(0.75f, &vec, 0x10, 0x1B5);
    }
    if (cursor->field_4 < cursor->unk8 - 6) {
        vec.x = 155.0f + pos.x;
        vec.y = 20.0f + pos.y - 164.0f - bounce;
        vec.z = float_0_8041c144;
        iconDispGx(0.75f, &vec, 0x10, 0x1B6);
    }
    n = cursor->field_0 - cursor->field_4;
    if (n < 0) {
        n = 0;
    }
    if (n >= 6) {
        n = 5;
    }
    vec.x = pos.x;
    vec.y = pos.y - 24.0f * n;
    vec.z = float_0_8041c144;
    iconDispGx(1.0f, &vec, 0x14, 0x1F0);
}

// These are called, never inlined, in the original build. The unit's configure
// entry still uses the default `-inline auto`, so keep them out of line here.
void InitSubMenuCommonProcess(BattleMenuWork** window, BattleCursor** cursor, BattleCursor* menu) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
    }
    wp->commandMenu.field_55C = BattleAlloc(sizeof(BattleMenuWork));
    *window = wp->commandMenu.field_55C;
    (*window)->cursor = menu;
    (*window)->phase = 0;
    (*window)->field_0x10 = 0;
    (*window)->inputReady = 0;
    (*window)->lastCursorPos = menu->field_0;
    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == UNIT_MARIO) {
        *cursor = &wp->commandMenu.cursor[0];
    } else {
        *cursor = &wp->commandMenu.cursor[1];
    }
    (*window)->scrollPos = float_0_8041c144;
    for (i = 0; i < 7; i++) {
        (*window)->slotFlags[i] = 0;
        (*window)->slotState[i] = 1;
        (*window)->slotFrame[i] = -i;
        (*window)->field_0xF8[i] = 0;
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
        window->iconFrame[idx] = 0;
        window->iconSlot[idx] = i;
        window->iconFlags[idx] = 0;
        window->iconFlags[idx] |= 1;
        if (i == cursor->unk8 - 1) {
            window->iconState[idx] = 0;
        } else {
            window->iconState[idx] = 0xA;
        }
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
        window->iconFrame[idx] = 0;
        window->iconSlot[idx] = i;
        window->iconFlags[idx] = 0;
        window->iconFlags[idx] |= 1;
        if (i == cursor->unk8 - 1) {
            window->iconState[idx] = 1;
        } else {
            window->iconState[idx] = 0xB;
        }
    }
}
#pragma dont_inline reset

void DrawSubMenuCommonProcessSub1(BattleMenuWork* window, BattleCursor* a, s32 idx, s32 mode) {
    s32 i;

    if (mode == 6 && a->unk8 > 6 && idx == a->unk8 - 6 && a->field_4 == 0 && a->field_0 != 1) {
        window->phase = 0x1E;
        window->field_0x10 = 0;
        window->scrollFrom = (f32)(a->unk8 - 6);
        window->scrollTo = float_0_8041c144;
        for (i = 0; i < 7; i++) {
            window->slotState[i] = 0x1E;
            window->slotFrame[i] = 0;
        }
    } else if (mode == 6 && a->unk8 > 6 && idx == 0 && a->field_4 == a->unk8 - 6 && a->field_0 != 5) {
        window->phase = 0x28;
        window->field_0x10 = 0;
        window->scrollFrom = float_0_8041c144;
        window->scrollTo = (f32)(a->unk8 - 6);
        for (i = 0; i < 7; i++) {
            window->slotState[i] = 0x28;
            window->slotFrame[i] = 0;
        }
    } else if (a->field_4 == idx + 1) {
        window->phase = 0xA;
        window->field_0x10 = 0;
        window->scrollFrom = (f32)idx;
        window->scrollTo = (f32)(idx + 1);
        for (i = 0; i < 7; i++) {
            window->slotState[i] = 0xA;
            window->slotFrame[i] = 0;
        }
    } else if (a->field_4 == idx - 1) {
        window->phase = 0x14;
        window->field_0x10 = 0;
        window->scrollFrom = (f32)idx;
        window->scrollTo = (f32)(idx - 1);
        for (i = 0; i < 7; i++) {
            window->slotState[i] = 0x14;
            window->slotFrame[i] = 0;
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

    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == UNIT_MARIO) {
        mainCursor = &wp->commandMenu.cursor[0];
    } else {
        mainCursor = &wp->commandMenu.cursor[1];
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

    *outX = (-200.0f);
    *outY = (-100.0f);

    {
    s32 type = window->phase;
    switch (type) {
    case 0:
        window->inputReady = 1;
        window->scrollPos = (f32)cursor->field_4;
        {
            s32 c1 = 0;
            for (i = 0; i < mainCursor->unk8; i++) {
                if (window->iconState[i] == 1 || window->iconState[i] == 0xB) {
                    c1++;
                }
            }
            {
                s32 c2;
                for (i = 0, c2 = 0; i < 7; i++) {
                    if (window->slotState[i] == 2) {
                        c2++;
                    }
                }
                if (c1 == mainCursor->unk8 && c2 == 7) {
                    window->phase = 2;
                }
            }
        }
        DrawSubMenuCommonProcessSub1(window, cursor, idx, maxCount);
        break;
    case 2:
        window->inputReady = 1;
        window->scrollPos = (f32)cursor->field_4;
        DrawSubMenuCommonProcessSub1(window, cursor, idx, maxCount);
        break;
    case 0xA:
    case 0x14:
    case 0x1E:
    case 0x28:
        if (type == 0xA || type == 0x14) {
            window->inputReady = 1;
        }
        {
            s32 c2 = 0;
            for (i = 0; i < 7; i++) {
                if (window->slotState[i] == 2) {
                    c2++;
                }
            }
            if (c2 == 7) {
                window->phase = 2;
                window->scrollPos = (f32)cursor->field_4;
            }
        }
        DrawSubMenuCommonProcessSub1(window, cursor, idx, maxCount);
        break;
    }
    }

    window->ringMoving = 0;
    for (i = 0; i < mainCursor->unk8; i++) {
        switch (window->iconState[i]) {
        case 0:
            window->iconFlags[i] |= 1;
            window->iconFlags[i] |= 2;
            GetRingCenter(&vecB[i]);
            vecB[i].x = intplGetValue(4, window->iconFrame[i], 12, vecB[i].x, (-60.0f));
            vecB[i].y = intplGetValue(4, window->iconFrame[i], 12, vecB[i].y, 55.0f);
            PSMTXTrans(mtxA[i], 20.0f + vecB[i].x, vecB[i].y, vecB[i].z);
            scales[i] = intplGetValue(0, window->iconFrame[i], 12, float_0_8041c144, 360.0f);
            if (scales[i] > 360.0f) {
                scales[i] -= 360.0f;
            }
            if (window->iconFrame[i] >= 3) {
                window->ringMoving = 1;
            }
            window->iconFrame[i]++;
            if (window->iconFrame[i] > 12) {
                window->iconState[i] = 1;
            }
            break;
        case 1:
            window->iconFlags[i] |= 1;
            window->iconFlags[i] |= 2;
            PSMTXTrans(mtxA[i], (-40.0f), 55.0f, float_0_8041c144);
            scales[i] = 360.0f;
            window->ringMoving = 1;
            break;
        case 0xA:
            window->iconFlags[i] |= 1;
            window->iconFlags[i] &= ~2;
            angle = 3.1415927f * kakutable[mainCursor->unk8 - 3][window->iconSlot[i]] / 180.0f;
            GetRingCenter(&vecB[i]);
            GetRingOffset(&ringX, &ringY, angle);
            vecB[i].x += ringX;
            vecB[i].y += ringY;
            vecB[i].z = ringY;
            scale = 1.0f - 0.1f * (f32)window->iconFrame[i];
            if (scale <= float_0_8041c144) {
                window->iconState[i] = 0xB;
            }
            PSMTXScale(scaleMtx, scale, scale, 1.0f);
            PSMTXTrans(transMtx, vecB[i].x, vecB[i].y, vecB[i].z);
            PSMTXConcat(transMtx, scaleMtx, mtxA[i]);
            scales[i] = float_0_8041c144;
            window->iconFrame[i]++;
            break;
        case 0xB:
            window->iconFlags[i] &= ~1;
            break;
        }
    }

    for (i = 0; i < 7; i++) {
        s32 state = window->slotState[i];
        switch (state) {
        case 0:
            window->slotFlags[i] &= ~1;
            window->field_0x114[i] = 0xFF;
            break;
        case 1:
            window->slotFlags[i] &= ~1;
            window->field_0x114[i] = 0xFF;
            if (window->slotFrame[i] >= 0 && i != 6) {
                f32 rad = intplGetValue(0, window->slotFrame[i], 12, 90.0f, float_0_8041c144);
                window->slotFlags[i] |= 1;
                PSMTXRotRad(mtxC[i], 'y', 0.017453292f * rad);
            }
            window->slotFrame[i]++;
            if (window->slotFrame[i] > 12) {
                window->slotState[i] = 2;
            }
            break;
        case 2:
            window->field_0x114[i] = 0xFF;
            if (i != 6) {
                window->slotFlags[i] |= 1;
                PSMTXIdentity(mtxC[i]);
            } else {
                window->slotFlags[i] &= ~1;
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
                window->slotFlags[i] |= 1;
                PSMTXIdentity(mtxC[i]);
                window->scrollPos = intplGetValue(4, window->slotFrame[i], steps, window->scrollFrom, window->scrollTo);
                window->slotFrame[i]++;
                if (window->slotFrame[i] > steps) {
                    window->slotState[i] = 2;
                    window->slotFrame[i] = 0;
                }
            }
            break;
        }
    }
}

void BattleMenuDisp_ActSelect_Init(BattleWork* wp, BattleCursor* a, BattleCommandAction* b, u32 flags) {
    s32 i;
    s32 idx;
    BattleMenuWork* w;

    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
    }
    wp->commandMenu.field_55C = BattleAlloc(sizeof(BattleMenuWork));
    if (flags & 1) {
        w = (BattleMenuWork*)wp->commandMenu.field_55C;
        w->cursor = a;
        w->mainIconTable = b;
        w->phase = 0xA;
        w->inputReady = 0;
        w->lastCursorPos = a->field_0;
        idx = a->field_0;
        for (i = 0; i < a->unk8; i++) {
            if (idx == a->field_0) {
                w->iconState[idx] = 0xA;
            } else {
                w->iconState[idx] = 0xB;
            }
            w->iconFrame[idx] = 0;
            w->iconSlot[idx] = a->unk8 - 1 + i;
            if (w->iconSlot[idx] >= a->unk8) {
                w->iconSlot[idx] -= a->unk8;
            }
            w->iconFlags[idx] = 0;
            idx++;
            if (idx >= a->unk8) {
                idx = 0;
            }
        }
    } else {
        w = (BattleMenuWork*)wp->commandMenu.field_55C;
        w->cursor = a;
        w->mainIconTable = b;
        w->phase = 0;
        w->inputReady = 0;
        w->lastCursorPos = a->field_0;
        idx = a->field_0;
        for (i = 0; i < a->unk8; i++) {
            idx++;
            if (idx >= a->unk8) {
                idx = 0;
            }
            w->iconState[idx] = 0;
            w->iconFrame[idx] = i * 10 / 2;
            w->iconSlot[idx] = i;
            w->iconFlags[idx] = 0;
        }
    }
}

void BattleMenuDisp_ActSelect_Main(BattleWork* wp) {}
void BattleMenuDisp_ActSelect_Disp(BattleWork* wp) {
    DrawMainMenu();
}

void BattleMenuDisp_ActSelect_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

// How far the cursor moved since the ring was last turned.
static inline s32 menuCursorDelta(BattleMenuWork* w) {
    return w->cursor->field_0 - w->lastCursorPos;
}

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
    switch (window->phase) {
    case 0:
        {
            s32 count = 0;
            for (i = 0; i < cursor->unk8; i++) {
                if (window->iconState[i] == 3) {
                    count++;
                }
            }
            if (count == cursor->unk8) {
                window->phase = 1;
            }
        }
        break;
    case 1:
        window->inputReady = 1;
        {
            s32 d = menuCursorDelta(window);
            if (d == 1 || -(window->cursor->unk8 - 1) == d) {
                window->phase = 2;
                window->inputReady = 0;
                window->lastCursorPos = window->cursor->field_0;
                for (i = 0; i < cursor->unk8; i++) {
                    window->iconState[i] = 4;
                    window->iconFrame[i] = 0;
                    window->iconFromSlot[i] = window->iconSlot[i];
                    window->iconSlot[i]--;
                    if (window->iconSlot[i] == -1) {
                        window->iconSlot[i] = cursor->unk8 - 1;
                    }
                }
            } else if (d == -1 || window->cursor->unk8 - 1 == d) {
                window->phase = 2;
                window->inputReady = 0;
                window->lastCursorPos = window->cursor->field_0;
                for (i = 0; i < cursor->unk8; i++) {
                    window->iconState[i] = 5;
                    window->iconFrame[i] = 0;
                    window->iconFromSlot[i] = window->iconSlot[i];
                    window->iconSlot[i]++;
                    if (window->iconSlot[i] == cursor->unk8) {
                        window->iconSlot[i] = 0;
                    }
                }
            }
        }
        break;
    case 2:
        {
            s32 count = 0;
            for (i = 0; i < cursor->unk8; i++) {
                if (window->iconState[i] == 3) {
                    count++;
                }
            }
            if (count == cursor->unk8) {
                window->phase = 1;
            }
        }
        break;
    case 0xA:
        window->phase = 1;
        window->inputReady = 1;
        break;
    }

    window->ringMoving = 0;
    for (i = 0; i < cursor->unk8; i++) {
        switch (window->iconState[i]) {
        case 0:
            window->iconFrame[i]--;
            if (window->iconFrame[i] <= 0) {
                window->iconState[i] = 1;
                window->iconFrame[i] = 0;
            }
            break;
        case 1:
            window->iconFlags[i] |= 1;
            {
                cur = 0.2f * (f32)window->iconFrame[i] + float_0_8041c144;
            }
            angle = 3.1415927f * kakutable[cursor->unk8 - 3][window->iconSlot[i]] / 180.0f;
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
                    f32 r = 90.0f * (f32)window->iconFrame[i] / 10.0f + 270.0f;
                    if (r >= 360.0f) {
                        r = float_0_8041c144;
                    }
                    PSMTXRotRad(rotMtx, 'x', 0.017453292f * r);
                    PSMTXTrans(transMtx, 20.0f + vecB[i].x, vecB[i].y, vecB[i].z);
                    PSMTXConcat(transMtx, rotMtx, mtxA[i]);
                    window->iconFlags[i] |= 2;
                } else {
                    f32 r = 90.0f * (f32)window->iconFrame[i] / 10.0f + 270.0f;
                    if (r >= 360.0f) {
                        r = float_0_8041c144;
                    }
                    PSMTXRotRad(rotMtx, 'x', 0.017453292f * r);
                    PSMTXTrans(transMtx, vecB[i].x, vecB[i].y, vecB[i].z);
                    PSMTXConcat(transMtx, rotMtx, mtxA[i]);
                }
                scales[i] = float_0_8041c144;
                if (cur >= angle && window->iconFrame[i] >= 10) {
                    window->iconState[i] = 3;
                    window->iconFrame[i] = -1;
                }
                window->iconFrame[i]++;
            }
            break;
        case 3:
            angle = 3.1415927f * kakutable[cursor->unk8 - 3][window->iconSlot[i]] / 180.0f;
            GetRingCenter(&vecB[i]);
            GetRingOffset(&ringX, &ringY, angle);
            vecB[i].x += ringX;
            vecB[i].y += ringY;
            vecB[i].z = ringY;
            if (i == cursor->field_0) {
                PSMTXTrans(mtxA[i], 20.0f + vecB[i].x, vecB[i].y, vecB[i].z);
                window->iconFlags[i] |= 2;
            } else {
                PSMTXTrans(mtxA[i], vecB[i].x, vecB[i].y, vecB[i].z);
            }
            scales[i] = float_0_8041c144;
            break;
        case 4:
            {
                f32 v70 = kakutable[cursor->unk8 - 3][window->iconFromSlot[i]];
                f32 v84 = kakutable[cursor->unk8 - 3][window->iconSlot[i]];
                f32 cur;
                if (float_0_8041c144 == v84) {
                    v84 = 360.0f;
                }
                cur = (f32)window->iconFrame[i] * 0.2f + 3.1415927f * v70 / 180.0f;
                angle = 3.1415927f * v84 / 180.0f;
                if (cur >= angle) {
                    window->iconState[i] = 3;
                    cur = angle;
                    window->iconFrame[i] = -1;
                }
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, cur);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                PSMTXTrans(mtxA[i], vecB[i].x, vecB[i].y, vecB[i].z);
                scales[i] = float_0_8041c144;
                window->iconFlags[i] &= ~2;
                window->iconFrame[i]++;
            }
            break;
        case 5:
            {
                f32 v70;
                f32 v84;
                f32 cur;
                v70 = kakutable[cursor->unk8 - 3][window->iconFromSlot[i]];
                if (float_0_8041c144 == v70) {
                    v70 = 360.0f;
                }
                v84 = kakutable[cursor->unk8 - 3][window->iconSlot[i]];
                cur = 3.1415927f * v70 / 180.0f - (f32)window->iconFrame[i] * 0.2f;
                angle = 3.1415927f * v84 / 180.0f;
                if (cur <= angle) {
                    window->iconState[i] = 3;
                    cur = angle;
                    window->iconFrame[i] = -1;
                }
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, cur);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                PSMTXTrans(mtxA[i], vecB[i].x, vecB[i].y, vecB[i].z);
                scales[i] = float_0_8041c144;
                window->iconFlags[i] &= ~2;
                window->iconFrame[i]++;
            }
            break;
        case 0xA:
            window->iconFlags[i] |= 1;
            window->iconFlags[i] |= 2;
            GetRingCenter(&vecB[i]);
            GetRingOffset(&ringX, &ringY, float_0_8041c144);
            vecB[i].x += ringX;
            vecB[i].y += ringY;
            vecB[i].z = ringY;
            vecB[i].x = intplGetValue(4, window->iconFrame[i], 12, (-60.0f), vecB[i].x);
            vecB[i].y = intplGetValue(4, window->iconFrame[i], 12, 55.0f, vecB[i].y);
            PSMTXTrans(mtxA[i], 20.0f + vecB[i].x, vecB[i].y, vecB[i].z);
            scales[i] = intplGetValue(0, window->iconFrame[i], 12, 360.0f, float_0_8041c144);
            window->iconFrame[i]++;
            if (window->iconFrame[i] > 12) {
                window->iconState[i] = 3;
            }
            break;
        case 0xB:
            window->iconFlags[i] |= 1;
            window->iconFlags[i] &= ~2;
            angle = 3.1415927f * kakutable[cursor->unk8 - 3][window->iconSlot[i]] / 180.0f;
            {
                f32 s;
                GetRingCenter(&vecB[i]);
                GetRingOffset(&ringX, &ringY, angle);
                vecB[i].x += ringX;
                vecB[i].y += ringY;
                vecB[i].z = ringY;
                s = 0.1f * (f32)window->iconFrame[i];
                if (s >= 1.0f) {
                    window->iconState[i] = 3;
                }
                PSMTXScale(scaleMtx, s, s, 1.0f);
                PSMTXTrans(transMtx, vecB[i].x, vecB[i].y, vecB[i].z);
                PSMTXConcat(transMtx, scaleMtx, mtxA[i]);
                scales[i] = float_0_8041c144;
                window->iconFrame[i]++;
            }
            break;
        }
    }

    DrawMainIcon(window, window->mainIconTable, mtxA, vecB, scales);
    DrawMenuPartyChangeButton();
    DrawMenuPinchMark();
}

void BattleMenuDisp_WeaponSelect_Init(BattleWork* wp, BattleCursor* menu, void* arg, u32 flags) {
    BattleMenuWork* window = 0;
    BattleCursor* cursor = 0;
    InitSubMenuCommonProcess(&window, &cursor, menu);
    window->weaponEntries = arg;
    if (flags & 1) {
        InitSubMenuCommonProcess2(window, cursor);
    } else {
        InitSubMenuCommonProcess3(window, cursor);
    }
}

void BattleMenuDisp_WeaponSelect_Main(BattleWork* wp) {}
void BattleMenuDisp_WeaponSelect_Disp(BattleWork* wp) {
    DrawWeaponWin();
}

void BattleMenuDisp_WeaponSelect_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
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
    sel = ((BattleMenuWeaponEntry*)window->weaponEntries)[cursor->field_0].weapon;
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->scrollPos + i;
        if (idx < cursor->unk8) {
            arr88[i] = ((BattleMenuWeaponEntry*)window->weaponEntries)[idx].enabled;
            arr24[i] = cmd->weaponTable[idx].mIcon;
            arr6C[i] = (s32)((BattleMenuWeaponEntry*)window->weaponEntries)[idx].label;
            if (((BattleMenuWeaponEntry*)window->weaponEntries)[idx].weapon != 0) {
                arr50[i] = BtlUnit_GetWeaponCost(unit, ((BattleMenuWeaponEntry*)window->weaponEntries)[idx].weapon);
            } else {
                arr50[i] = 0;
            }
            if (((BattleMenuWeaponEntry*)window->weaponEntries)[idx].weapon != 0) {
                arr34[i] = ((BattleMenuWeaponEntry*)window->weaponEntries)[idx].weapon->baseSpCost;
            } else {
                arr34[i] = 0;
            }
        }
    }
    DrawMainIcon(window, wp->commandMenu.actionTable, mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, 0, 0);
    DrawMenuCursorAndScrollArrow((Vec){ -60.0f, 35.0f, 0.0f });
    if (sel != 0) {
        s32 id = sel->item;
        if (id != 0) {
            msg = itemDataTable[id].description;
        } else if (sel == &ItemWeaponData_CookingItem) {
            BattleMenuWeaponEntry* entry = (BattleMenuWeaponEntry*)window->weaponEntries;
            msg = itemDataTable[entry[cursor->field_0].itemId].description;
        } else {
            msg = sel->description;
        }
    } else {
        BattleMenuWeaponEntry* entry = (BattleMenuWeaponEntry*)window->weaponEntries;
        msg = itemDataTable[entry[cursor->field_0].itemId].description;
    }
    if (msg != 0) {
        DrawMenuHelpWin(msg, outX, outY);
    }
}

void BattleMenuDisp_Operation_Init(BattleWork* wp, u32 flags) {
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    BattleMenuWork* window;
    BattleCursor* cursor;
    BattleCursor* menu;

    if (unit->currentKind == UNIT_MARIO) {
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

void BattleMenuDisp_Operation_Main(BattleWork* wp) {}
void BattleMenuDisp_Operation_Disp(BattleWork* wp) {
    DrawOperationWin();
}

void BattleMenuDisp_Operation_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

// This declaration order, and the function-scope `entry`, are what make MWCC
// pack the frame at the original offsets (outX/outY at 0x14/0x10, arr88 at 0x88,
// ...) and hand r10/r9 to i/entry; the natural order does not.
#pragma dont_inline on
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
    f32 outY;

    BattleMenuDispEntryOp* entry;
    s32 i;
    BattleGetUnitPtr(wp, wp->activeUnitId);
    entries = (BattleMenuDispEntryOp*)wp->commandMenu.operationTable;
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->scrollPos + i;
        if (idx < cursor->unk8) {
            entry = &entries[idx];
            arr88[i] = entry->enabled;
            if (entries[i].iconId != 0) {
                arr24[i] = entry->iconId;
            } else {
                arr24[i] = 0x18B;
            }
            arr6C[i] = (s32)entry->label;
            arr50[i] = entry->fpCost;
            arr34[i] = 0;
        }
    }
    DrawMainIcon(window, wp->commandMenu.actionTable, mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, 0, 0);
    DrawMenuCursorAndScrollArrow((Vec){ -60.0f, 35.0f, 0.0f });
    if (entries[cursor->field_0].helpMsg != 0) {
        DrawMenuHelpWin(entries[cursor->field_0].helpMsg, outX, outY);
    }
}
#pragma dont_inline reset

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

void BattleMenuDisp_ChangePartySelect_Main(BattleWork* wp) {}
void BattleMenuDisp_ChangePartySelect_Disp(BattleWork* wp) {
    DrawChangePartyWin();
}

void BattleMenuDisp_ChangePartySelect_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

// This declaration order (and the split window/cursor assignment below) is what
// makes MWCC place the locals at the original frame offsets and hand out
// r30/r29/r28 to window/cursor/entries; the natural order does not match.
#pragma dont_inline on
void DrawChangePartyWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window;
    BattleCursor* cursor;
    BattleCommandParty* entries;
    f32 outX;
    f32 outY;
    u16 arr24[7];
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
    entries = wp->commandMenu.partyTable;
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->scrollPos + i;
        if (idx < cursor->unk8) {
            BattleCommandParty* entry = &entries[idx];
            arr88[i] = entry->mEnabled;
            if (entry->mIcon != 0) {
                arr24[i] = entry->mIcon;
            } else {
                arr24[i] = 0x18B;
            }
            arr6C[i] = (s32)entry->mName;
            arr50[i] = 0;
            arr34[i] = 0;
        }
    }
    DrawMainIcon(window, wp->commandMenu.actionTable, mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, entries, 1);
    DrawMenuCursorAndScrollArrow((Vec){ -60.0f, 35.0f, 0.0f });
    if (entries[cursor->field_0].mDescription != 0) {
        DrawMenuHelpWin(entries[cursor->field_0].mDescription, outX, outY);
    }
}
#pragma dont_inline reset

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

void BattleMenuDisp_MultiItem_Main(BattleWork* wp) {}
void BattleMenuDisp_MultiItem_Disp(BattleWork* wp) {
    DrawMultiItemWin();
}

void BattleMenuDisp_MultiItem_End(BattleWork* wp) {
    if (wp->commandMenu.field_55C) {
        BattleFree(wp->commandMenu.field_55C);
        wp->commandMenu.field_55C = 0;
    }
}

// The Disp wrappers must emit a real call, not an inline expansion of these.
#pragma dont_inline on
void DrawMultiItemWin(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleMenuWork* window = (BattleMenuWork*)wp->commandMenu.field_55C;
    BattleCursor* cursor = window->cursor;
    BattleMenuMultiEntry* entries;
    f32 outX;
    f32 outY;
    u16 arr24[7];
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
    entries = (BattleMenuMultiEntry*)wp->commandMenu.multiItemTable;
    DrawSubMenuCommonProcess(&outX, &outY, mats, centers, scales, rot);
    for (i = 0; i < 7; i++) {
        s32 idx = (s32)window->scrollPos + i;
        if (idx < cursor->unk8) {
            arr88[i] = entries[idx].enabled;
            if (entries[idx].iconId != 0) {
                arr24[i] = entries[idx].iconId;
            } else {
                arr24[i] = 0x18B;
            }
            arr6C[i] = (s32)entries[idx].label;
            arr50[i] = entries[idx].fpCost;
            arr34[i] = 0;
        }
    }
    DrawMainIcon(window, wp->commandMenu.actionTable, mats, centers, scales);
    DrawSubIcon(window, rot, arr88, arr24, arr6C, arr50, arr34, 0, 0);
    DrawMenuCursorAndScrollArrow((Vec){ -60.0f, 35.0f, 0.0f });
    if (entries[cursor->field_0].helpMsg != 0) {
        DrawMenuHelpWin(entries[cursor->field_0].helpMsg, outX, outY);
    }
}
#pragma dont_inline reset

#pragma explicit_zero_data on
const f32 float_0_8041c144 = 0.0f;
#pragma explicit_zero_data reset
