#include "window/win_party.h"
#include "window/win_root.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"

extern WindowPartyDt winPartyDt[];

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
extern const char str_802F45D0[];
extern const char str_802F45DC[];
extern const char str_802F45F0[];
extern const char str_802F4600[];
extern const char str_802F4608[];
extern const char str_802F4614[];
extern const char str_802F4628[];
extern const char str_802F4634[];
extern const char str_802F4640[];
extern const char str_802F4658[];
extern const char str_802F4664[];
extern const char str_802F4670[];
extern const char str_802F4688[];
extern const char str_802F4694[];
extern const char str_802F46A0[];
extern const char str_802F46B4[];
extern const char str_802F46C0[];
extern const char str_802F46C8[];
extern const char str_802F46D4[];
extern const char str_802F46E8[];
extern const char str_802F46F4[];
extern const char str_802F4700[];
extern const char str_802F4714[];
extern const char str_8041D1F0[];
extern const char str_8041D1F4[];
extern const char str_8041D1F8[];
extern const char str_8041D1FC[];
extern const char str_8041D200[];
extern const char str_8041D204[];
extern const char str_8041D20C[];
extern const char str_8041D214[];
extern const char str_802F49D8[];
extern const char str_802F49E8[];
extern const char str_802F49F8[];
extern const char str_802F4A08[];
extern const char str_802F4A18[];
extern const char str_802F4A28[];
extern const char str_8041D2A0[];
extern const char str_8041D2A4[];
extern const char str_8041D2A8[];
extern const char str_8041D2AC[];
extern const char str_8041D2B0[];
extern const char str_8041D2B8[];
extern u32 chri_waza[4];
extern u32 noko_waza[4];
extern u32 cloud_waza[4];
extern u32 yoshi_waza[4];
extern u32 vivi_waza[4];
extern u32 sand_waza[4];
extern u32 chu_waza[4];


// Extended fields of WindowWork used by this unit. win/win_main.h only declares
// the first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

void winPartyMain2(WindowWork* work) {
    f32 factor = 0.25f;
    f32 diff = WW_F32(work, 0x1E8) - WW_F32(work, 0x1E4);

    WW_F32(work, 0x1E4) += diff * factor;
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

void winPartyInit2(WindowWork* work) {
    f32 time = 360.0f / (f32)WW_S32(work, 0x1E0) * (f32)WW_S32(work, 0x1DC);

    WW_F32(work, 0x1E8) = time;
    WW_F32(work, 0x1E4) = time;
    WW_S32(work, 0x200) = 0;
    WW_F32(work, 0x158) = -200.0f;
    WW_F32(work, 0x15C) = 20.0f;
    winMsgEntry(work, 0, winPartyDt[WW_S32(work, 0x1D8)].field_0xC, 0);
}

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
                case 0: anim = str_8041D200; break;
                case 1: anim = str_8041D2A0; break;
                case 2: anim = str_8041D2A4; break;
                case 3: anim = str_8041D2A8; break;
                case 4: anim = str_8041D2AC; break;
                case 5: anim = str_8041D2B0; break;
                case 6:
                default: anim = str_8041D2B8; break;
                }
            } else if (j == 4) {
                switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                case 0: model = str_802F4688; break;
                case 1: model = str_802F49D8; break;
                case 2: model = str_802F49E8; break;
                case 3: model = str_802F49F8; break;
                case 4: model = str_802F4A08; break;
                case 5: model = str_802F4A18; break;
                case 6:
                default: model = str_802F4A28; break;
                }
                anim = str_8041D1F0;
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


// ---- data -------------------------------------------------------------------
// Defined after the code, in the original's order: named objects land ahead of the compiler's
// own pool. The 7 waza tables live in .sdata.
u32 chri_waza[4] = { (u32)partyWeapon_ChristineNormalAttack, (u32)partyWeapon_ChristineMonosiri, (u32)partyWeapon_ChristineRenzokuAttack, (u32)partyWeapon_ChristineKiss };
u32 noko_waza[4] = { (u32)partyWeapon_NokotarouKouraAttack, (u32)partyWeapon_NokotarouSyubibinKoura, (u32)partyWeapon_NokotarouKouraGuard, (u32)partyWeapon_NokotarouTsuranukiKoura };
u32 cloud_waza[4] = { (u32)partyWeapon_ClaudaNormalAttack, (u32)partyWeapon_ClaudaBreathAttack, (u32)partyWeapon_ClaudaPredationAttack, (u32)partyWeapon_ClaudaKumogakureAttack };
u32 yoshi_waza[4] = { (u32)partyWeapon_YoshiNormalAttack, (u32)partyWeapon_YoshiNomikomi_Shot, (u32)partyWeapon_YoshiEggAttack_Minimini, (u32)partyWeapon_YoshiCallGuard };
u32 vivi_waza[4] = { (u32)partyWeapon_VivianNormalAttack, (u32)partyWeapon_VivianShadowGuard, (u32)partyWeapon_VivianMagicalPowder, (u32)partyWeapon_VivianCharmKissAttack };
u32 sand_waza[4] = { (u32)partyWeapon_SandersNormalAttack, (u32)partyWeapon_SandersTimeBombSet, (u32)partyWeapon_SandersCounterSet, (u32)partyWeapon_SandersSuperBombAttack };
u32 chu_waza[4] = { (u32)partyWeapon_ChuchurinaNormalAttackLeft, (u32)partyWeapon_ChuchurinaItemSteal, (u32)partyWeapon_ChuchurinaMadowaseAttack, (u32)partyWeapon_ChuchurinaKiss };

// .data winPartyDt (0xFC = 7 entries of 0x24).
WindowPartyDt winPartyDt[7] = {
    { 1, 0x155, str_802F45D0, str_802F45DC, str_802F45F0, str_8041D1F0, str_802F4600, str_8041D1F4, (const char*)chri_waza },
    { 2, 0x156, str_802F4608, str_802F4614, str_802F4628, str_8041D1F0, str_802F4600, str_8041D1F8, (const char*)noko_waza },
    { 5, 0x15F, str_802F4634, str_802F4640, str_802F4658, str_8041D1F0, str_802F4600, str_8041D1FC, (const char*)cloud_waza },
    { 4, 0x158, str_802F4664, str_802F4670, str_802F4688, str_8041D1F0, str_802F4600, str_8041D200, (const char*)yoshi_waza },
    { 6, 0x160, str_802F4694, str_802F46A0, str_802F46B4, str_802F46C0, str_802F4600, str_8041D204, (const char*)vivi_waza },
    { 3, 0x157, str_802F46C8, str_802F46D4, str_802F46E8, str_8041D1F0, str_802F4600, str_8041D20C, (const char*)sand_waza },
    { 7, 0x161, str_802F46F4, str_802F4700, str_802F4714, str_8041D1F0, str_802F4600, str_8041D214, (const char*)chu_waza },
};

// ---- .rodata / .sdata2 named objects, in the original's order ----------------
// Nothing references most of these yet (winPartyDisp is not written), so force_active
// keeps them; the compiler pools its own copies of the literals the code uses.
#pragma force_active on

// rodata str_802F45D0 size 0xc
const char str_802F45D0[] = "name_party0";

// rodata str_802F45DC size 0x14
const char str_802F45DC[] = "msg_menu_party_kuri";

// rodata str_802F45F0 size 0x10
const char str_802F45F0[] = "EFF_m_kuriko";

// rodata str_802F4600 size 0x8
const char str_802F4600[] = "d_mario";

// rodata str_802F4608 size 0xc
const char str_802F4608[] = "name_party1";

// rodata str_802F4614 size 0x14
const char str_802F4614[] = "msg_menu_party_noko";

// rodata str_802F4628 size 0xc
const char str_802F4628[] = "EFF_m_noko";

// rodata str_802F4634 size 0xc
const char str_802F4634[] = "name_party4";

// rodata str_802F4640 size 0x18
const char str_802F4640[] = "msg_menu_party_cloud";

// rodata str_802F4658 size 0xc
const char str_802F4658[] = "EFF_m_cloud";

// rodata str_802F4664 size 0xc
const char str_802F4664[] = "name_party3";

// rodata str_802F4670 size 0x18
const char str_802F4670[] = "msg_menu_party_yoshi";

// rodata str_802F4688 size 0xc
const char str_802F4688[] = "EFF_m_yoshi";

// rodata str_802F4694 size 0xc
const char str_802F4694[] = "name_party5";

// rodata str_802F46A0 size 0x14
const char str_802F46A0[] = "msg_menu_party_vivi";

// rodata str_802F46B4 size 0xc
const char str_802F46B4[] = "c_vivian";

// rodata str_802F46C0 size 0x8
const char str_802F46C0[] = "PTR_S_1";

// rodata str_802F46C8 size 0xc
const char str_802F46C8[] = "name_party2";

// rodata str_802F46D4 size 0x14
const char str_802F46D4[] = "msg_menu_party_bom";

// rodata str_802F46E8 size 0xc
const char str_802F46E8[] = "EFF_m_bomb";

// rodata str_802F46F4 size 0xc
const char str_802F46F4[] = "name_party6";

// rodata str_802F4700 size 0x14
const char str_802F4700[] = "msg_menu_party_chu";

// rodata str_802F4714 size 0xc
const char str_802F4714[] = "EFF_m_chu";

// rodata dat_802f4720 size 0x8
const u32 dat_802f4720[2] = { 0xCB8CA2FE, 0x9CC381FE };

// rodata str_802F4728 size 0x14
const char str_802F4728[] = "msg_menu_party_hp";

// rodata str_802F473C size 0x14
const char str_802F473C[] = "msg_menu_party_rank";

// rodata jumptable_802F4750 size 0x8
__declspec(section ".rodata") const char* jumptable_802F4750[2] = { (const char*)str_802F4728, (const char*)str_802F473C };

// rodata str_802F4758 size 0x18
const char str_802F4758[] = "msg_menu_mario_ruby_hp";

// rodata jumptable_802F4770 size 0x8
__declspec(section ".rodata") const char* jumptable_802F4770[2] = { (const char*)str_802F4758, (const char*)str_802F4758 };

// rodata vec3_802f4778 size 0xc
const Vec vec3_802f4778 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4784 size 0xc
const Vec vec3_802f4784 = {1.0f, 1.0f, 1.0f};

// rodata vec3_802f4790 size 0xc
const Vec vec3_802f4790 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f479c size 0xc
const Vec vec3_802f479c = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f47a8 size 0xc
const Vec vec3_802f47a8 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f47b4 size 0xc
const Vec vec3_802f47b4 = {0.5f, 0.5f, 0.5f};

// rodata vec3_802f47c0 size 0xc
const Vec vec3_802f47c0 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f47cc size 0xc
const Vec vec3_802f47cc = {0.82f, 0.82f, 0.82f};

// rodata vec3_802f47d8 size 0xc
const Vec vec3_802f47d8 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f47e4 size 0xc
const Vec vec3_802f47e4 = {0.8f, 0.8f, 0.8f};

// rodata vec3_802f47f0 size 0xc
const Vec vec3_802f47f0 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f47fc size 0xc
const Vec vec3_802f47fc = {1.0f, 1.0f, 1.0f};

// rodata vec3_802f4808 size 0xc
const Vec vec3_802f4808 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4814 size 0xc
const Vec vec3_802f4814 = {0.9f, 0.9f, 0.9f};

// rodata vec3_802f4820 size 0xc
const Vec vec3_802f4820 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f482c size 0xc
const Vec vec3_802f482c = {0.9f, 0.9f, 0.9f};

// rodata vec3_802f4838 size 0xc
const Vec vec3_802f4838 = {0.0f, 0.0f, 80.0f};

// rodata vec3_802f4844 size 0xc
const Vec vec3_802f4844 = {0.0f, 0.0f, 80.0f};

// rodata vec3_802f4850 size 0xc
const Vec vec3_802f4850 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f485c size 0xc
const Vec vec3_802f485c = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4868 size 0xc
const Vec vec3_802f4868 = {1.0f, 1.0f, 1.0f};

// rodata vec3_802f4874 size 0xc
const Vec vec3_802f4874 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4880 size 0xc
const Vec vec3_802f4880 = {0.8f, 0.8f, 0.8f};

// rodata vec3_802f488c size 0xc
const Vec vec3_802f488c = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4898 size 0xc
const Vec vec3_802f4898 = {0.8f, 0.8f, 0.8f};

// rodata vec3_802f48a4 size 0xc
const Vec vec3_802f48a4 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f48b0 size 0xc
const Vec vec3_802f48b0 = {-0.6f, 0.6f, 0.6f};

// rodata vec3_802f48bc size 0xc
const Vec vec3_802f48bc = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f48c8 size 0xc
const Vec vec3_802f48c8 = {0.6f, 0.6f, 0.6f};

// rodata vec3_802f48d4 size 0xc
const Vec vec3_802f48d4 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f48e0 size 0xc
const Vec vec3_802f48e0 = {0.7f, 0.7f, 0.7f};

// rodata vec3_802f48ec size 0xc
const Vec vec3_802f48ec = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f48f8 size 0xc
const Vec vec3_802f48f8 = {0.7f, 0.7f, 0.7f};

// rodata vec3_802f4904 size 0xc
const Vec vec3_802f4904 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4910 size 0xc
const Vec vec3_802f4910 = {0.8f, 0.8f, 0.8f};

// rodata vec3_802f491c size 0xc
const Vec vec3_802f491c = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4928 size 0xc
const Vec vec3_802f4928 = {0.8f, 0.8f, 0.8f};

// rodata vec3_802f4934 size 0xc
const Vec vec3_802f4934 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4940 size 0xc
const Vec vec3_802f4940 = {1.0f, 1.0f, 1.0f};

// rodata vec3_802f494c size 0xc
const Vec vec3_802f494c = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4958 size 0xc
const Vec vec3_802f4958 = {1.0f, 1.0f, 1.0f};

// rodata vec3_802f4964 size 0xc
const Vec vec3_802f4964 = {0.0f, 0.0f, 0.0f};

// rodata vec3_802f4970 size 0xc
const Vec vec3_802f4970 = {0.5f, 0.5f, 0.5f};

// rodata str_802F497C size 0x18
const char str_802F497C[] = "msg_menu_party_henkou";

// rodata str_802F4994 size 0x1c
const char str_802F4994[] = "msg_menu_party_waza_kakunin";

// rodata str_802F49B0 size 0x14
const char str_802F49B0[] = "msg_menu_party_waza";

// rodata gap_04_802F49C4_rodata size 0x4
__declspec(section ".rodata") const u32 gap_04_802F49C4_rodata = 0;

// rodata double_to_int_mask_802f49c8 size 0x8
const f64 double_to_int_mask_802f49c8 = 4503599627370496.0;

// rodata double_to_int_802f49d0 size 0x8
const f64 double_to_int_802f49d0 = 4503601774854144.0;

// rodata str_802F49D8 size 0x10
const char str_802F49D8[] = "EFF_m_yoshi2";

// rodata str_802F49E8 size 0x10
const char str_802F49E8[] = "EFF_m_yoshi3";

// rodata str_802F49F8 size 0x10
const char str_802F49F8[] = "EFF_m_yoshi4";

// rodata str_802F4A08 size 0x10
const char str_802F4A08[] = "EFF_m_yoshi5";

// rodata str_802F4A18 size 0x10
const char str_802F4A18[] = "EFF_m_yoshi6";

// rodata str_802F4A28 size 0x10
const char str_802F4A28[] = "EFF_m_yoshi7";

// sdata2 str_8041D1F0 size 0x4
const char str_8041D1F0[] = "W_1";

// sdata2 str_8041D1F4 size 0x4
const char str_8041D1F4[] = "R_2";

// sdata2 str_8041D1F8 size 0x4
const char str_8041D1F8[] = "R_3";

// sdata2 str_8041D1FC size 0x4
const char str_8041D1FC[] = "R_4";

// sdata2 str_8041D200 size 0x4
const char str_8041D200[] = "R_5";

// sdata2 str_8041D204 size 0x8
const char str_8041D204[] = "R_12";

// sdata2 str_8041D20C size 0x8
const char str_8041D20C[] = "R_13";

// sdata2 str_8041D214 size 0x8
const char str_8041D214[] = "R_14";

// sdata2 dat_8041d21c size 0x4
const u32 dat_8041d21c[1] = { 0xFFFFFFFF };

// sdata2 dat_8041d220 size 0x4
const u32 dat_8041d220[1] = { 0x000000FF };

// sdata2 dat_8041d224 size 0x4
const u32 dat_8041d224[1] = { 0xA73C3CFF };

// sdata2 dat_8041d228 size 0x4
const u32 dat_8041d228[1] = { 0x00000080 };

// sdata2 dat_8041d22c size 0x4
const u32 dat_8041d22c[1] = { 0x808080FF };

// sdata2 float_10_8041d230 size 0x4
const f32 float_10_8041d230 = 10.0f;

// sdata2 float_20_8041d234 size 0x4
const f32 float_20_8041d234 = 20.0f;

// sdata2 float_116_8041d238 size 0x4
const f32 float_116_8041d238 = 116.0f;

// sdata2 float_240_8041d23c size 0x4
const f32 float_240_8041d23c = 240.0f;

// sdata2 float_76_8041d240 size 0x4
const f32 float_76_8041d240 = 76.0f;

// sdata2 float_45_8041d244 size 0x4
const f32 float_45_8041d244 = 45.0f;

// sdata2 float_2_8041d248 size 0x4
const f32 float_2_8041d248 = 2.0f;

// sdata2 float_110_8041d24c size 0x4
const f32 float_110_8041d24c = 110.0f;

// sdata2 float_1_8041d250 size 0x4
const f32 float_1_8041d250 = 1.0f;

// sdata2 float_0p9_8041d254 size 0x4
const f32 float_0p9_8041d254 = 0.9f;

// sdata2 float_0p5_8041d258 size 0x4
const f32 float_0p5_8041d258 = 0.5f;

// sdata2 float_4_8041d25c size 0x4
const f32 float_4_8041d25c = 4.0f;

// sdata2 float_12_8041d260 size 0x4
const f32 float_12_8041d260 = 12.0f;

// sdata2 float_21_8041d264 size 0x4
const f32 float_21_8041d264 = 21.0f;

// sdata2 float_60_8041d268 size 0x4
const f32 float_60_8041d268 = 60.0f;

// sdata2 float_32_8041d26c size 0x4
const f32 float_32_8041d26c = 32.0f;

// sdata2 float_8_8041d270 size 0x4
const f32 float_8_8041d270 = 8.0f;

// sdata2 float_170_8041d274 size 0x4
const f32 float_170_8041d274 = 170.0f;

// sdata2 str_8041D278 size 0x4
const char str_8041D278[] = "%s";

// sdata2 float_180_8041d27c size 0x4
const f32 float_180_8041d27c = 180.0f;

// sdata2 float_0p5236_8041d280 size 0x4
const f32 float_0p5236_8041d280 = 0.5235988f;

// sdata2 float_deg2rad_8041d284 size 0x4
const f32 float_deg2rad_8041d284 = 0.017453292f;

// sdata2 float_360_8041d288 size 0x4
const f32 float_360_8041d288 = 360.0f;

// sdata2 float_140_8041d28c size 0x4
const f32 float_140_8041d28c = 140.0f;

// sdata2 float_0p8_8041d290 size 0x4
const f32 float_0p8_8041d290 = 0.8f;

// sdata2 float_0p6_8041d294 size 0x4
const f32 float_0p6_8041d294 = 0.6f;

// sdata2 float_neg1_8041d298 size 0x4
const f32 float_neg1_8041d298 = -1.0f;

// sdata2 float_3p1416_8041d29c size 0x4
const f32 float_3p1416_8041d29c = 3.1415927f;

// sdata2 str_8041D2A0 size 0x4
const char str_8041D2A0[] = "R_6";

// sdata2 str_8041D2A4 size 0x4
const char str_8041D2A4[] = "R_7";

// sdata2 str_8041D2A8 size 0x4
const char str_8041D2A8[] = "R_8";

// sdata2 str_8041D2AC size 0x4
const char str_8041D2AC[] = "R_9";

// sdata2 str_8041D2B0 size 0x8
const char str_8041D2B0[] = "R_10";

// sdata2 str_8041D2B8 size 0x8
const char str_8041D2B8[] = "R_11";

// sdata2 float_neg0p8_8041d2c0 size 0x4
const f32 float_neg0p8_8041d2c0 = -0.8f;

// sdata2 str_8041D2C4 size 0x4
const char str_8041D2C4[] = "Z_5";

// sdata2 str_8041D2C8 size 0x4
const char str_8041D2C8[] = "Z_6";

// sdata2 str_8041D2CC size 0x4
const char str_8041D2CC[] = "Z_7";

// sdata2 str_8041D2D0 size 0x4
const char str_8041D2D0[] = "Z_8";

// sdata2 str_8041D2D4 size 0x4
const char str_8041D2D4[] = "Z_9";

// sdata2 str_8041D2D8 size 0x8
const char str_8041D2D8[] = "Z_10";

// sdata2 str_8041D2E0 size 0x8
const char str_8041D2E0[] = "Z_11";

// sdata2 float_0_8041d2e8 size 0x4
const f32 float_0_8041d2e8 = 0.0f;

// sdata2 float_5_8041d2ec size 0x4
const f32 float_5_8041d2ec = 5.0f;

// sdata2 float_230_8041d2f0 size 0x4
const f32 float_230_8041d2f0 = 230.0f;

// sdata2 float_50_8041d2f4 size 0x4
const f32 float_50_8041d2f4 = 50.0f;

// sdata2 float_200_8041d2f8 size 0x4
const f32 float_200_8041d2f8 = 200.0f;

// sdata2 float_neg230_8041d2fc size 0x4
const f32 float_neg230_8041d2fc = -230.0f;

// sdata2 float_90_8041d300 size 0x4
const f32 float_90_8041d300 = 90.0f;

// sdata2 float_30_8041d304 size 0x4
const f32 float_30_8041d304 = 30.0f;

// sdata2 float_100_8041d308 size 0x4
const f32 float_100_8041d308 = 100.0f;

// sdata2 float_150_8041d30c size 0x4
const f32 float_150_8041d30c = 150.0f;

// sdata2 float_16_8041d310 size 0x4
const f32 float_16_8041d310 = 16.0f;

// sdata2 float_0p7_8041d314 size 0x4
const f32 float_0p7_8041d314 = 0.7f;

// sdata2 float_250_8041d318 size 0x4
const f32 float_250_8041d318 = 250.0f;

// sdata2 float_260_8041d31c size 0x4
const f32 float_260_8041d31c = 260.0f;

// sdata2 lbl_8041D320 size 0x4
const f32 lbl_8041D320 = 144.0f;

// sdata2 str_8041D324 size 0x8
const char str_8041D324[] = "\202e\202o";

// sdata2 float_238_8041d32c size 0x4
const f32 float_238_8041d32c = 238.0f;

// sdata2 float_17_8041d330 size 0x4
const f32 float_17_8041d330 = 17.0f;

// sdata2 float_0p25_8041d334 size 0x4
const f32 float_0p25_8041d334 = 0.25f;

// sdata2 float_neg200_8041d338 size 0x4
const f32 float_neg200_8041d338 = -200.0f;

// sdata2 float_neg10_8041d33c size 0x4
const f32 float_neg10_8041d33c = -10.0f;

// sdata2 float_neg26_8041d340 size 0x4
const f32 float_neg26_8041d340 = -26.0f;

// sdata2 gap_09_8041D344_sdata2 size 0x4
const u32 gap_09_8041D344_sdata2[1] = { 0x00000000 };
#pragma force_active reset
