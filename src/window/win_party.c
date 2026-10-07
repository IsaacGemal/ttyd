#include "window/win_party.h"
#include "window/win_root.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "battle/battle_database_common.h"
#include "data/item_data.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "win/win_main.h"
#include "window/win_mario.h"
#include <stdio.h>
#include <stdarg.h>


// The waza tables below relocate against these units' weapon data.
#include "unit/party/unit_party_christine.h"
#include "unit/party/unit_party_nokotarou.h"
#include "unit/party/unit_party_clauda.h"
#include "unit/party/unit_party_yoshi.h"
#include "unit/party/unit_party_vivian.h"
#include "unit/party/unit_party_sanders.h"
#include "unit/party/unit_party_chuchurina.h"

// Data objects defined later in this file or in the party units, declared here (their headers don't list data).
extern u32 partyWeapon_ChuchurinaNormalAttackLeft[48];
extern u32 partyWeapon_NokotarouKouraAttack[48];
extern u32 partyWeapon_NokotarouSyubibinKoura[48];
extern u32 partyWeapon_NokotarouKouraGuard[48];
extern u32 partyWeapon_NokotarouTsuranukiKoura[48];
extern u32 partyWeapon_VivianNormalAttack[48];
extern u32 partyWeapon_VivianShadowGuard[48];
extern u32 partyWeapon_VivianMagicalPowder[48];
extern u32 partyWeapon_VivianCharmKissAttack[48];
extern u32 partyWeapon_YoshiNormalAttack[48];
extern u32 partyWeapon_YoshiNomikomi_Shot[48];
extern u32 partyWeapon_YoshiEggAttack_Minimini[48];
extern u32 partyWeapon_YoshiCallGuard[48];


// ---- data ----
// .sdata: each partner's four moves; .data: the partner table. Defined before the code so their strings
// pool first, as in the original.
u32 chri_waza[4] = { (u32)partyWeapon_ChristineNormalAttack, (u32)partyWeapon_ChristineMonosiri, (u32)partyWeapon_ChristineRenzokuAttack, (u32)partyWeapon_ChristineKiss };
u32 noko_waza[4] = { (u32)partyWeapon_NokotarouKouraAttack, (u32)partyWeapon_NokotarouSyubibinKoura, (u32)partyWeapon_NokotarouKouraGuard, (u32)partyWeapon_NokotarouTsuranukiKoura };
u32 cloud_waza[4] = { (u32)partyWeapon_ClaudaNormalAttack, (u32)partyWeapon_ClaudaBreathAttack, (u32)partyWeapon_ClaudaPredationAttack, (u32)partyWeapon_ClaudaKumogakureAttack };
u32 yoshi_waza[4] = { (u32)partyWeapon_YoshiNormalAttack, (u32)partyWeapon_YoshiNomikomi_Shot, (u32)partyWeapon_YoshiEggAttack_Minimini, (u32)partyWeapon_YoshiCallGuard };
u32 vivi_waza[4] = { (u32)partyWeapon_VivianNormalAttack, (u32)partyWeapon_VivianShadowGuard, (u32)partyWeapon_VivianMagicalPowder, (u32)partyWeapon_VivianCharmKissAttack };
u32 sand_waza[4] = { (u32)partyWeapon_SandersNormalAttack, (u32)partyWeapon_SandersTimeBombSet, (u32)partyWeapon_SandersCounterSet, (u32)partyWeapon_SandersSuperBombAttack };
u32 chu_waza[4] = { (u32)partyWeapon_ChuchurinaNormalAttackLeft, (u32)partyWeapon_ChuchurinaItemSteal, (u32)partyWeapon_ChuchurinaMadowaseAttack, (u32)partyWeapon_ChuchurinaKiss };

WindowPartyDt winPartyDt[7] = {
    { 1, 0x155, "name_party0", "msg_menu_party_kuri", "EFF_m_kuriko", "W_1", "d_mario", "R_2", (const char*)chri_waza },
    { 2, 0x156, "name_party1", "msg_menu_party_noko", "EFF_m_noko", "W_1", "d_mario", "R_3", (const char*)noko_waza },
    { 5, 0x15F, "name_party4", "msg_menu_party_cloud", "EFF_m_cloud", "W_1", "d_mario", "R_4", (const char*)cloud_waza },
    { 4, 0x158, "name_party3", "msg_menu_party_yoshi", "EFF_m_yoshi", "W_1", "d_mario", "R_5", (const char*)yoshi_waza },
    { 6, 0x160, "name_party5", "msg_menu_party_vivi", "c_vivian", "PTR_S_1", "d_mario", "R_12", (const char*)vivi_waza },
    { 3, 0x157, "name_party2", "msg_menu_party_bom", "EFF_m_bomb", "W_1", "d_mario", "R_13", (const char*)sand_waza },
    { 7, 0x161, "name_party6", "msg_menu_party_chu", "EFF_m_chu", "W_1", "d_mario", "R_14", (const char*)chu_waza },
};

static const GXColor winPartyWhite = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor winPartyBlack = {0x00, 0x00, 0x00, 0xFF};

// Extended fields of WindowWork used by this unit. win/win_main.h only declares
// the first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

// NONMATCHING (99.99%): one instruction differs, `lfd f2, 0x400(r31)` vs 0x468. The compiler
// pools its own int->float magic double after the named .rodata objects because the function that
// first used it (winPartyDisp) has no source; the named copy is already at the original 0x400.
void winPartyInit(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
#ifndef MUST_MATCH
    u8* p;
#endif
    s32 i;
    s32 j;
    s32 k;
    s32 pose;
    const char* model;
    const char* anim;

    WW_S32(work, 0x1E0) = 0;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 8; j++) {
            if (j != winPartyDt[i].field_0x0)
                continue;
            if (!(pouch->partyData[j].flags & 1))
                continue;
            if (pouch->partyData[j].flags & 2)
                continue;

            if (WW_U16(work, 0x0) & 0x2000) {
                model = winPartyDt[i].field_0x18;
                switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                case 0: anim = "R_5"; break;
                case 1: anim = "R_6"; break;
                case 2: anim = "R_7"; break;
                case 3: anim = "R_8"; break;
                case 4: anim = "R_9"; break;
                case 5: anim = "R_10"; break;
                case 6:
                default: anim = "R_11"; break;
                }
            } else if (j == 4) {
                switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                case 0: model = "EFF_m_yoshi"; break;
                case 1: model = "EFF_m_yoshi2"; break;
                case 2: model = "EFF_m_yoshi3"; break;
                case 3: model = "EFF_m_yoshi4"; break;
                case 4: model = "EFF_m_yoshi5"; break;
                case 5: model = "EFF_m_yoshi6"; break;
                case 6:
                default: model = "EFF_m_yoshi7"; break;
                }
                anim = "W_1";
            } else {
                model = winPartyDt[i].field_0x10;
                anim = winPartyDt[i].field_0x14;
            }

            pose = animPoseEntry(model, 0);
            ((s32*)work)[WW_S32(work, 0x1E0) + 0x68] = pose;
            animPoseSetAnim(((s32*)work)[WW_S32(work, 0x1E0) + 0x68], anim, 1);
            animPoseSetMaterialFlagOn(((s32*)work)[WW_S32(work, 0x1E0) + 0x68], 0x1800);
            ((s32*)work)[WW_S32(work, 0x1E0) + 0x6F] = j;
            WW_S32(work, 0x1E0)++;
        }
    }

    i = 0;
    WW_S32(work, 0x1DC) = i;
#ifdef MUST_MATCH
    for (; i < WW_S32(work, 0x1E0); i++) {
#else
    for (p = (u8*)work; i < WW_S32(work, 0x1E0); i++, p += 4) {
#endif
        s32 party;

        party = !marioBgmodeChk() ? marioGetParty() : marioGetPtr()->prevPartyMemberId[0];
#ifdef MUST_MATCH
        if (party == ((s32*)work)[i + 0x6F]) {
#else
        if (party == *(s32*)(p + 0x1BC)) {
#endif
            WW_S32(work, 0x1DC) = i;
            break;
        }
    }

    for (k = 0; k < 8; k++) {
        if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
            WW_S32(work, 0x1D8) = k;
            break;
        }
    }

    WW_S32(work, 0x1FC) = WW_S32(work, 0x1DC);
    {
        f32 time = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);

        WW_F32(work, 0x1E8) = time;
        WW_F32(work, 0x1E4) = time;
    }
    WW_S32(work, 0x200) = 0;
    WW_S32(work, 0x204) = pouchGetPartyAttackLv(winPartyDt[WW_S32(work, 0x1D8)].field_0x0) + 2;

    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
    case 0: winPartyDt[3].field_0x4 = 0x158; break;
    case 1: winPartyDt[3].field_0x4 = 0x159; break;
    case 2: winPartyDt[3].field_0x4 = 0x15A; break;
    case 3: winPartyDt[3].field_0x4 = 0x15B; break;
    case 4: winPartyDt[3].field_0x4 = 0x15C; break;
    case 5: winPartyDt[3].field_0x4 = 0x15D; break;
    case 6:
    default: winPartyDt[3].field_0x4 = 0x15E; break;
    }
}

void winPartyInit2(WindowWork* work) {
    f32 time = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);

    WW_F32(work, 0x1E8) = time;
    WW_F32(work, 0x1E4) = time;
    WW_S32(work, 0x200) = 0;
    WW_F32(work, 0x158) = -200.0f;
    WW_F32(work, 0x15C) = 20.0f;
    winMsgEntry(work, 0, winPartyDt[WW_S32(work, 0x1D8)].field_0xC, 0);
}

void winPartyExit(WindowWork* work) {
    u8* p = (u8*)work;
    s32 i;

    for (i = 0; i < WW_S32(work, 0x1E0); i++) {
        if (*(s32*)(p + 0x1A0) != -1) {
            animPoseRelease(*(s32*)(p + 0x1A0));
        }
        p += 4;
    }
}

s32 winPartyMain(WindowWork* work) {
    s32 k;

    if (WW_S32(work, 0x1E0) == 0) {
        return -1;
    }

    switch (WW_S32(work, 0x200)) {
    case 0: {
        u32 button = WW_U32(work, 0x4);
        u32 dir;

        if (button & 0x100) {
            WW_S32(work, 0x19C) = 1;
            return -2;
        }
        if (button & 0x400) {
            WW_S32(work, 0x200) = 1;
            WW_S32(work, 0x208) = 0;
            psndSFXOn((const char*)0x20012);
            goto apply;
        }
        if (button & 0x200) {
            psndSFXOn((const char*)0x20013);
            WW_S32(work, 0x19C) = 0;
            return -1;
        }
        if (button & 0x1000) {
            WW_S32(work, 0x19C) = 0;
            return -2;
        }

        dir = WW_U32(work, 0x10);
        if (dir & 0x4000) {
            s32 prev = WW_S32(work, 0x1DC);

            WW_S32(work, 0x1DC)--;
            while (WW_S32(work, 0x1DC) < 0) {
                WW_S32(work, 0x1DC) += WW_S32(work, 0x1E0);
            }
            WW_F32(work, 0x1E8) = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);
            while (WW_F32(work, 0x1E4) < WW_F32(work, 0x1E8)) {
                WW_F32(work, 0x1E4) += 360.0f;
            }
            if (prev != WW_S32(work, 0x1DC)) {
                psndSFXOn((const char*)0x209D6);
            }
            for (k = 0; k < 8; k++) {
                if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
                    WW_S32(work, 0x1D8) = k;
                    break;
                }
            }
            goto apply;
        }
        if (dir & 0x8000) {
            s32 prev = WW_S32(work, 0x1DC);

            WW_S32(work, 0x1DC)++;
            while (WW_S32(work, 0x1DC) > WW_S32(work, 0x1E0) - 1) {
                WW_S32(work, 0x1DC) -= WW_S32(work, 0x1E0);
            }
            WW_F32(work, 0x1E8) = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);
            while (WW_F32(work, 0x1E4) > WW_F32(work, 0x1E8)) {
                WW_F32(work, 0x1E4) -= 360.0f;
            }
            if (prev != WW_S32(work, 0x1DC)) {
                psndSFXOn((const char*)0x209D6);
            }
            for (k = 0; k < 8; k++) {
                if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
                    WW_S32(work, 0x1D8) = k;
                    break;
                }
            }
            goto apply;
        }
    apply:
        WW_F32(work, 0x158) = -200.0f;
        WW_F32(work, 0x15C) = 20.0f;
        WW_S32(work, 0x204) = pouchGetPartyAttackLv(winPartyDt[WW_S32(work, 0x1D8)].field_0x0) + 2;
        winMsgEntry(work, 0, winPartyDt[WW_S32(work, 0x1D8)].field_0xC, 0);
        break;
    }
    case 1: {
        u32 dir = WW_U32(work, 0x10);

        if (dir & 0x1000) {
            WW_S32(work, 0x208)--;
            if (WW_S32(work, 0x208) < 0) {
                WW_S32(work, 0x208) = WW_S32(work, 0x204) - 1;
            }
            psndSFXOn((const char*)0x20005);
        } else if (dir & 0x2000) {
            WW_S32(work, 0x208)++;
            if (WW_S32(work, 0x208) >= WW_S32(work, 0x204)) {
                WW_S32(work, 0x208) = 0;
            }
            psndSFXOn((const char*)0x20005);
        } else {
            u32 button = WW_U32(work, 0x4);

            if (button & 0x200) {
                WW_S32(work, 0x200) = 0;
                psndSFXOn((const char*)0x20013);
            } else if (button & 0x1000) {
                WW_S32(work, 0x19C) = 0;
                return -2;
            }
        }

        WW_F32(work, 0x158) = -10.0f;
        WW_F32(work, 0x15C) = -26.0f * (f32)WW_S32(work, 0x208) + 5.0f;
        winMsgEntry(work, 0, *(const char**)((u8*)((void**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20)[WW_S32(work, 0x208)] + 0xC), 0);
        break;
    }
    default:
        break;
    }

    return 0;
}

void winPartyMain2(WindowWork* work) {
    f32 factor = 0.25f;
    f32 diff = WW_F32(work, 0x1E8) - WW_F32(work, 0x1E4);

    WW_F32(work, 0x1E4) += diff * factor;
}


// The window draw functions take their Vec and GXColor arguments by value: every caller here copies them into
// fresh stack slots. win_main.h declares them with pointers, which compiles to the same callee.
#ifdef MUST_MATCH
// Match-only: called through by-value casts (caller copies plus a pointer, ABI-identical to the real prototypes).
#define winFontSetV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSet)
#define winFontSetRV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSetR)
#define winFontSetEdgeV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSetEdge)
#define winFontSetLabelV ((void (*)(Vec, Vec, GXColor, const char*))winFontSetLabel)
#define winIconSetV ((void (*)(s32, Vec, Vec, GXColor))winIconSet)
#else
static void winPartyFontV(void (*fn)(Vec*, Vec*, GXColor*, const char*, ...), Vec pos, Vec scale, GXColor color,
                          const char* format, va_list args) {
    char text[0x200];

    vsprintf(text, format, args);
    fn(&pos, &scale, &color, "%s", text);
}
static void winFontSetV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winPartyFontV(winFontSet, pos, scale, color, format, args);
    va_end(args);
}
static void winFontSetRV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winPartyFontV(winFontSetR, pos, scale, color, format, args);
    va_end(args);
}
static void winFontSetEdgeV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winPartyFontV(winFontSetEdge, pos, scale, color, format, args);
    va_end(args);
}
static inline void winFontSetLabelV(Vec pos, Vec scale, GXColor color, const char* tag) {
    winFontSetLabel(&pos, &scale, &color, tag);
}
static inline void winIconSetV(s32 id, Vec pos, Vec scale, GXColor color) {
    winIconSet(id, &pos, &scale, &color);
}
#endif

#define WIN_PARTY_TEX(w) (**(TEXPalette***)(*(u8**)((u8*)(w) + 0x28) + 0xA0))
// the party member shown in pose slot n, and its animation pose
#define WIN_PARTY_ID(w, n) (((s32*)(w))[0x6F + (n)])
#define WIN_PARTY_POSE(w, n) (((s32*)(w))[0x68 + (n)])

// NONMATCHING (97.76%): calls, branches, immediates, the frame and both literal pools match; register colouring
// differs (x/y rank above cx/barX/top in the first loop; the original reuses x's register for the move list's
// name column) and the stack slots of some block locals.
extern const f32 float_0p5_8041d258;
void winPartyDisp(CameraId camId, WindowWork* work, s32 partyIdx) {
    GXColor unusedWhite = winPartyWhite;
    GXColor unusedBlack = winPartyBlack;
    GXColor white = winPartyWhite;
    GXColor black = winPartyBlack;
    GXColor loopWhite = winPartyWhite;
    GXColor loopBlack = winPartyBlack;
    f32 x = WW_F32(work, 0xC4 + partyIdx * 0x14);
    f32 y = WW_F32(work, 0xC8 + partyIdx * 0x14);
    PouchPartyData* party;
    f32 cx, cy, barX, top, rowY, scale, w, w2, x2, y2;
    s32 i, n;

    winBgGX(work, 1, x, y);
    if (WW_S32(work, 0x1E0) == 0) {
        return;
    }
    party = &pouchGetPtr()->partyData[winPartyDt[WW_S32(work, 0x1D8)].field_0x0];

    // HP and rank bars
    winKirinukiGX(work, 0, x - 10.0f, 20.0f + (116.0f + y), 240.0f, 76.0f);
    cx = 45.0f + (x - 10.0f);
    barX = cx - 2.0f;
    top = 110.0f + y;
    for (i = 0; i < 2; i++) {
        GXColor barColors[2] = {{0xCB, 0x8C, 0xA2, 0xFE}, {0x9C, 0xC3, 0x81, 0xFE}};
        const char* labels[2] = {"msg_menu_party_hp", "msg_menu_party_rank"};
        const char* subLabels[2] = {"msg_menu_mario_ruby_hp", "msg_menu_mario_ruby_hp"};
        GXColor subColors[1] = {{0xA7, 0x3C, 0x3C, 0xFF}};

        scale = i == 0 ? 1.0f : 0.9f;
        w = scale * FontGetMessageWidth(msgSearch(labels[i]));
        w2 = 0.5f * FontGetMessageWidth(msgSearch(subLabels[i]));
        winTexInit(WIN_PARTY_TEX(work));
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        rowY = top - (f32)(i * 32);
        winTexSet(0xAF, (Vec){barX, rowY - 4.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, barColors[i]);
        winFontInit();
        winFontSetV((Vec){cx - w * 0.5f, 12.0f + rowY, 0.0f}, (Vec){scale, scale, scale}, loopBlack,
                    msgSearch(labels[i]));
        if (i < 1) {
            winFontSetV((Vec){cx - w2 * float_0p5_8041d258, 21.0f + rowY, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, subColors[i],
                        msgSearch(subLabels[i]));
        }
    }

    // rank stars: the rank's icon, then one per rank above 2
    winIconInit();
    cx = 60.0f + cx;
    rowY = 4.0f + top;
    winIconSetV(0x19F, (Vec){cx, rowY, 0.0f}, (Vec){0.82f, 0.82f, 0.82f}, white);
    y2 = 4.0f + (top - 32.0f);
    for (i = 0; i < WW_S32(work, 0x204) - 2; i++) {
        winIconSetV(((ItemDataView*)itemDataTable)[ITEM_SHINE_SPRITE].icon_id,
                    (Vec){cx + (f32)(i * 34), y2, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, loopWhite);
    }

    // HP: current / max
    winTexInit(WIN_PARTY_TEX(work));
    winTexSet(0x10, (Vec){8.0f + (170.0f + (x - 10.0f)), rowY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
    winFontInit();
    y2 = 4.0f + (12.0f + top);
    winFontSetRV((Vec){10.0f + (110.0f + (x - 10.0f)), y2, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s",
                 winZenkakuStr(party->currentHP));
    winFontSetV((Vec){8.0f + (180.0f + (x - 10.0f)), y2, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s",
                winZenkakuStr(party->maximumHP));
    winZClear();

    // the party on its turntable: shadows first, then the models
    for (i = 0; i < WW_S32(work, 0x1E0); i++) {
        Vec pos = {0.0f, 0.0f, 80.0f};
        Mtx tilt, turn;
        GXColor shadow = {0x00, 0x00, 0x00, 0x80};

        PSMTXRotRad(tilt, 'x', 0.5235988f);
        PSMTXRotRad(turn, 'y',
                    0.017453292f * (360.0f / (f32)WW_S32(work, 0x1E0) * (f32)i - WW_F32(work, 0x1E4)));
        PSMTXMultVec(turn, &pos, &pos);
        PSMTXMultVec(tilt, &pos, &pos);
        winKageGX(work, &shadow, (x - 140.0f) + pos.x, 20.0f + (y + pos.y), pos.z,
                  i == WW_S32(work, 0x1DC) ? 0.8f : 0.6f);
    }

    for (i = 0; i < WW_S32(work, 0x1E0); i++) {
        Vec pos = {0.0f, 0.0f, 80.0f};
        Mtx tilt, turn, scaleMtx, mtx;
        s32 pose = WIN_PARTY_POSE(work, i);
        AnimData* anim;
        const char* animName;

        if (pose == -1) {
            continue;
        }
        anim = animPoseGetAnimDataPtr(pose);
        PSMTXRotRad(tilt, 'x', 0.5235988f);
        PSMTXRotRad(turn, 'y',
                    0.017453292f * (360.0f / (f32)WW_S32(work, 0x1E0) * (f32)i - WW_F32(work, 0x1E4)));
        PSMTXMultVec(turn, &pos, &pos);
        PSMTXMultVec(tilt, &pos, &pos);

        if (i == WW_S32(work, 0x1DC)) {
            // the selected member: full size, lit, animating
            PSMTXTrans(mtx, (x - 140.0f) + pos.x, 20.0f + (y + pos.y), pos.z);
            PSMTXScale(scaleMtx, 1.0f, 1.0f, -1.0f);
            PSMTXRotRad(turn, 'y', 3.1415927f);
            PSMTXConcat(mtx, scaleMtx, mtx);
            PSMTXConcat(mtx, turn, mtx);
            animPoseSetMaterialFlagOff(pose, 0x40);
            for (n = 0; n < 7; n++) {
                if (WIN_PARTY_ID(work, i) == winPartyDt[n].field_0x0) {
                    break;
                }
            }
            if (work->flags & 0x2000) {
                animName = winPartyDt[n].field_0x1C;
                if (winPartyDt[n].field_0x0 == 4) {
                    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                    case 0: animName = "R_5"; break;
                    case 1: animName = "R_6"; break;
                    case 2: animName = "R_7"; break;
                    case 3: animName = "R_8"; break;
                    case 4: animName = "R_9"; break;
                    case 5: animName = "R_10"; break;
                    case 6:
                    default: animName = "R_11"; break;
                    }
                }
            } else {
                animName = winPartyDt[n].field_0x14;
            }
            animPoseSetAnim(pose, animName, 0);
            animPoseSetLocalTime(pose,
                                 gp->retraceCount % (s32)*(f32*)(*(u8**)((u8*)anim + 0x24) + 0x8));
        } else {
            // the others: smaller, greyed out, still
            PSMTXTrans(mtx, (x - 140.0f) + pos.x, 20.0f + (y + pos.y), pos.z);
            PSMTXScale(scaleMtx, 0.8f, 0.8f, -0.8f);
            PSMTXRotRad(turn, 'y', 3.1415927f);
            PSMTXConcat(mtx, scaleMtx, mtx);
            PSMTXConcat(mtx, turn, mtx);
            animPoseSetMaterialFlagOn(pose, 0x40);
            animPoseSetMaterialEvtColor(pose, (GXColor){0x80, 0x80, 0x80, 0xFF});
            for (n = 0; n < 7; n++) {
                if (WIN_PARTY_ID(work, i) == winPartyDt[n].field_0x0) {
                    break;
                }
            }
            if (work->flags & 0x2000) {
                animName = winPartyDt[n].field_0x1C;
                if (winPartyDt[n].field_0x0 == 4) {
                    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                    case 0: animName = "Z_5"; break;
                    case 1: animName = "Z_6"; break;
                    case 2: animName = "Z_7"; break;
                    case 3: animName = "Z_8"; break;
                    case 4: animName = "Z_9"; break;
                    case 5: animName = "Z_10"; break;
                    case 6:
                    default: animName = "Z_11"; break;
                    }
                }
            } else {
                animName = winPartyDt[n].field_0x14;
            }
            animPoseSetAnim(pose, animName, 0);
            animPoseSetLocalTime(pose, 0.0f);
        }
        animPoseMain(pose);
        if (WIN_PARTY_ID(work, i) == 6) {
            Vec vivianPos = (Vec){(x - 140.0f) + pos.x, 20.0f + (y + pos.y), pos.z};

            animPoseVivianMain(pose, &vivianPos);
        }
        animPoseDrawMtx(pose, mtx, 2, 0.0f, 5.0f);
    }

    // the selected member's name
    if (WW_S32(work, 0x1E0) != 0) {
        winNameGX(work, 0, x - 230.0f, (20.0f + (y - 50.0f)) - 10.0f, 200.0f, 32.0f);
        winFontInit();
        y2 = (20.0f + ((y - 50.0f) - 4.0f)) - 10.0f;
        for (n = 0; n < 7; n++) {
            if (WIN_PARTY_ID(work, WW_S32(work, 0x1DC)) == winPartyDt[n].field_0x0) {
                const char* name;

                if (winPartyDt[n].field_0x0 != 4) {
                    name = msgSearch(winPartyDt[n].field_0x8);
                } else {
                    name = pouchGetYoshiName();
                }
                w = FontGetMessageWidth(name);
                winFontSetEdgeV((Vec){(200.0f - w) * float_0p5_8041d258 + (x - 230.0f), y2, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                                loopWhite, name);
            }
        }
    }

    // "change partner" prompt and the turntable arrows
    if (WW_S32(work, 0x1E0) > 1) {
        winFontInit();
        w = 0.8f * FontGetMessageWidth(msgSearch("msg_menu_party_henkou"));
        x2 = (-230.0f - w) * float_0p5_8041d258 + x;
        winFontSetV((Vec){x2, y - 90.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, black, msgSearch("msg_menu_party_henkou"));
        winIconInit();
        winIconSetV(0x64, (Vec){x2 - 30.0f, y - 100.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white);
        winTexInit(WIN_PARTY_TEX(work));
        winTexSet(0xC0, (Vec){(x - 150.0f) - 50.0f, y - 16.0f, 0.0f}, (Vec){-0.6f, 0.6f, 0.6f}, white);
        winTexSet(0xC0, (Vec){50.0f + (x - 150.0f), y - 16.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }

    // "check moves" prompt, then the move list with each move's FP cost
    winFontInit();
    w = 0.7f * FontGetMessageWidth(msgSearch("msg_menu_party_waza_kakunin"));
    x2 = x + (250.0f - w);
    winFontSetV((Vec){x2, y - 100.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, black,
                msgSearch("msg_menu_party_waza_kakunin"));
    winIconInit();
    winIconSetV(0x1F5, (Vec){x2 - 20.0f, y - 110.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, white);
    top = 50.0f + y;
    winWazaGX(work, 0, x - 5.0f, top, 260.0f, 144.0f);
    winFontInit();
    w = 0.8f * FontGetMessageWidth(msgSearch("msg_menu_party_waza"));
    y2 = 45.0f + y;
    winFontSetEdgeV((Vec){(200.0f - w) * float_0p5_8041d258 + x, y2, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white,
                    msgSearch("msg_menu_party_waza"));
    x2 = 230.0f + x;
    winFontSetEdgeV((Vec){x2 - 20.0f, y2, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, "\202e\202o");
    {
        f32 costX = (238.0f + x) - 20.0f;
        f32 fpX = 2.0f + (2.0f + ((17.0f + x2) - 20.0f));
        f32 nameX = 10.0f + x;

        for (i = 0; i < WW_S32(work, 0x204); i++) {
            BattleWeapon* weapon;
            char buf[64];

            rowY = (top - (f32)(i * 26)) - 30.0f;
            winFontSetLabelV((Vec){nameX, rowY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, loopBlack,
                             ((BattleWeapon**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20)[i]->name);
            weapon = ((BattleWeapon**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20)[i];
            sprintf(buf, "%s", winZenkakuStr(weapon->baseFpCost));
            winFontSetV((Vec){(costX - (f32)(FontGetMessageWidth(buf) >> 1)) - 2.0f, rowY, 0.0f},
                        (Vec){1.0f, 1.0f, 1.0f}, loopBlack, buf);
            winFontSetV((Vec){fpX, rowY - 10.0f, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, loopBlack, "\202e\202o");
        }
    }
}
