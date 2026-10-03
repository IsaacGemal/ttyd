#include "window/win_party.h"
#include "matching.h"
#include "window/win_root.h"
#include "window/win_mario.h"
#include "driver/animdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "data/item_data.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>
#include <stdio.h>

extern WindowPartyDt winPartyDt[];

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

// NONMATCHING (99.97%): every opcode and register matches; the 23 differing arguments are
// all symbol addresses. winPartyInit's literals are pooled with the undecompiled winPartyDt /
// winPartyDisp data, which is emitted ahead of them in the same sections, so the referenced
// symbols land at different offsets from our object: .rodata (the seven EFF_m_yoshi* strings
// and the u32->double constant), .sdata2 (the R_5..R_11 and W_1 strings and the 360.0f
// constant) and .data (the three switch jump tables). Nothing inside winPartyInit can move
// them; declaring the strings as named locals instead only perturbs register allocation
// (99.57%), and referencing the original's named string symbols as externs is worse still
// (90-92%: the compiler can no longer use @sda21/one base register and reallocates r21).
// Confirmed: with winPartyDisp's data now defined, our object's .data is still short (the
// winPartyDt table at 0x0, 0xFC) and the compiler pools winPartyInit's EFF_m_yoshi* strings
// with a later base than the original's str_802F45D0, so the 8 offsets still differ. winPartyDt
// is the .data table at 0x0 (0xFC) that also emits the first ~0x30 .rodata strings; none of the
// remaining winPartyInit diffs are inside this function's control.
// NONMATCHING (99.97%): the 8 remaining !! are rodata base offsets; see above.
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

// ---------------------------------------------------------------------------
// Data used by winPartyDisp.  This unit's .rodata/.sdata2 objects are declared
// here (the data port is separate work); names match the original objects.
// ---------------------------------------------------------------------------

__declspec(section ".sdata2") const char str_8041D1F0[4] = "W_1";
__declspec(section ".sdata2") const char str_8041D1F4[4] = "R_2";
__declspec(section ".sdata2") const char str_8041D1F8[4] = "R_3";
__declspec(section ".sdata2") const char str_8041D1FC[4] = "R_4";
__declspec(section ".sdata2") const char str_8041D200[4] = "R_5";
__declspec(section ".sdata2") const char str_8041D204[8] = "R_12";
__declspec(section ".sdata2") const char str_8041D20C[8] = "R_13";
__declspec(section ".sdata2") const char str_8041D214[8] = "R_14";
__declspec(section ".sdata2") const GXColor dat_8041d21c = {0xFF, 0xFF, 0xFF, 0xFF};
__declspec(section ".sdata2") const GXColor dat_8041d220 = {0xFF, 0x00, 0x00, 0x00};
__declspec(section ".sdata2") const GXColor dat_8041d224 = {0xFF, 0x3C, 0x3C, 0xA7};
__declspec(section ".sdata2") const GXColor dat_8041d228 = {0x80, 0x00, 0x00, 0x00};
__declspec(section ".sdata2") const GXColor dat_8041d22c = {0xFF, 0x80, 0x80, 0x80};
__declspec(section ".sdata2") const f32 float_10_8041d230 = 10.0f;
__declspec(section ".sdata2") const f32 float_20_8041d234 = 20.0f;
__declspec(section ".sdata2") const f32 float_116_8041d238 = 116.0f;
__declspec(section ".sdata2") const f32 float_240_8041d23c = 240.0f;
__declspec(section ".sdata2") const f32 float_76_8041d240 = 76.0f;
__declspec(section ".sdata2") const f32 float_45_8041d244 = 45.0f;
__declspec(section ".sdata2") const f32 float_2_8041d248 = 2.0f;
__declspec(section ".sdata2") const f32 float_110_8041d24c = 110.0f;
__declspec(section ".sdata2") const f32 float_1_8041d250 = 1.0f;
__declspec(section ".sdata2") const f32 float_0p9_8041d254 = 0.9f;
__declspec(section ".sdata2") const f32 float_0p5_8041d258 = 0.5f;
__declspec(section ".sdata2") const f32 float_4_8041d25c = 4.0f;
__declspec(section ".sdata2") const f32 float_12_8041d260 = 12.0f;
__declspec(section ".sdata2") const f32 float_21_8041d264 = 21.0f;
__declspec(section ".sdata2") const f32 float_60_8041d268 = 60.0f;
__declspec(section ".sdata2") const f32 float_32_8041d26c = 32.0f;
__declspec(section ".sdata2") const f32 float_8_8041d270 = 8.0f;
__declspec(section ".sdata2") const f32 float_170_8041d274 = 170.0f;
__declspec(section ".sdata2") const char str_8041D278[4] = "%s";
__declspec(section ".sdata2") const f32 float_180_8041d27c = 180.0f;
__declspec(section ".sdata2") const f32 float_0p5236_8041d280 = 0.5235988f;
__declspec(section ".sdata2") const f32 float_deg2rad_8041d284 = 0.017453292f;
__declspec(section ".sdata2") const f32 float_360_8041d288 = 360.0f;
__declspec(section ".sdata2") const f32 float_140_8041d28c = 140.0f;
__declspec(section ".sdata2") const f32 float_0p8_8041d290 = 0.8f;
__declspec(section ".sdata2") const f32 float_0p6_8041d294 = 0.6f;
__declspec(section ".sdata2") const f32 float_neg1_8041d298 = -1.0f;
__declspec(section ".sdata2") const f32 float_3p1416_8041d29c = 3.1415927f;
__declspec(section ".sdata2") const char str_8041D2A0[4] = "R_6";
__declspec(section ".sdata2") const char str_8041D2A4[4] = "R_7";
__declspec(section ".sdata2") const char str_8041D2A8[4] = "R_8";
__declspec(section ".sdata2") const char str_8041D2AC[4] = "R_9";
__declspec(section ".sdata2") const char str_8041D2B0[8] = "R_10";
__declspec(section ".sdata2") const char str_8041D2B8[8] = "R_11";
__declspec(section ".sdata2") const f32 float_neg0p8_8041d2c0 = -0.8f;
__declspec(section ".sdata2") const char str_8041D2C4[4] = "Z_5";
__declspec(section ".sdata2") const char str_8041D2C8[4] = "Z_6";
__declspec(section ".sdata2") const char str_8041D2CC[4] = "Z_7";
__declspec(section ".sdata2") const char str_8041D2D0[4] = "Z_8";
__declspec(section ".sdata2") const char str_8041D2D4[4] = "Z_9";
__declspec(section ".sdata2") const char str_8041D2D8[8] = "Z_10";
__declspec(section ".sdata2") const char str_8041D2E0[8] = "Z_11";
__declspec(section ".sdata2") const f32 float_0_8041d2e8 = 0.0f;
__declspec(section ".sdata2") const f32 float_5_8041d2ec = 5.0f;
__declspec(section ".sdata2") const f32 float_230_8041d2f0 = 230.0f;
__declspec(section ".sdata2") const f32 float_50_8041d2f4 = 50.0f;
__declspec(section ".sdata2") const f32 float_200_8041d2f8 = 200.0f;
__declspec(section ".sdata2") const f32 float_neg230_8041d2fc = -230.0f;
__declspec(section ".sdata2") const f32 float_90_8041d300 = 90.0f;
__declspec(section ".sdata2") const f32 float_30_8041d304 = 30.0f;
__declspec(section ".sdata2") const f32 float_100_8041d308 = 100.0f;
__declspec(section ".sdata2") const f32 float_150_8041d30c = 150.0f;
__declspec(section ".sdata2") const f32 float_16_8041d310 = 16.0f;
__declspec(section ".sdata2") const f32 float_0p7_8041d314 = 0.7f;
__declspec(section ".sdata2") const f32 float_250_8041d318 = 250.0f;
__declspec(section ".sdata2") const f32 float_260_8041d31c = 260.0f;
__declspec(section ".sdata2") const f32 lbl_8041D320 = 144.0f;
__declspec(section ".sdata2") const char str_8041D324[8] = "\202e\202o";
__declspec(section ".sdata2") const f32 float_238_8041d32c = 238.0f;
__declspec(section ".sdata2") const f32 float_17_8041d330 = 17.0f;
__declspec(section ".sdata2") const f32 float_0p25_8041d334 = 0.25f;
__declspec(section ".sdata2") const f32 float_neg200_8041d338 = -200.0f;
__declspec(section ".sdata2") const f32 float_neg10_8041d33c = -10.0f;
__declspec(section ".sdata2") const f32 float_neg26_8041d340 = -26.0f;
const char str_802F45D0[12] = "name_party0";
const char str_802F45DC[20] = "msg_menu_party_kuri";
const char str_802F45F0[16] = "EFF_m_kuriko";
const char str_802F4600[8] = "d_mario";
const char str_802F4608[12] = "name_party1";
const char str_802F4614[20] = "msg_menu_party_noko";
const char str_802F4628[12] = "EFF_m_noko";
const char str_802F4634[12] = "name_party4";
const char str_802F4640[24] = "msg_menu_party_cloud";
const char str_802F4658[12] = "EFF_m_cloud";
const char str_802F4664[12] = "name_party3";
const char str_802F4670[24] = "msg_menu_party_yoshi";
const char str_802F4688[12] = "EFF_m_yoshi";
const char str_802F4694[12] = "name_party5";
const char str_802F46A0[20] = "msg_menu_party_vivi";
const char str_802F46B4[12] = "c_vivian";
const char str_802F46C0[8] = "PTR_S_1";
const char str_802F46C8[12] = "name_party2";
const char str_802F46D4[20] = "msg_menu_party_bom";
const char str_802F46E8[12] = "EFF_m_bomb";
const char str_802F46F4[12] = "name_party6";
const char str_802F4700[20] = "msg_menu_party_chu";
const char str_802F4714[12] = "EFF_m_chu";
__declspec(section ".rodata") const GXColor dat_802f4720[2] = {{0xFE, 0xA2, 0x8C, 0xCB}, {0xFE, 0x81, 0xC3, 0x9C}};
const char str_802F4728[20] = "msg_menu_party_hp";
const char str_802F473C[20] = "msg_menu_party_rank";
static __declspec(section ".rodata") const char* const jumptable_802F4750[2] = {str_802F4728, str_802F473C};
const char str_802F4758[24] = "msg_menu_mario_ruby_hp";
static __declspec(section ".rodata") const char* const jumptable_802F4770[2] = {str_802F4758, str_802F4758};
const Vec vec3_802f4778 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4784 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4790 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f479c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f47a8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f47b4 = {0.5f, 0.5f, 0.5f};
const Vec vec3_802f47c0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f47cc = {0.82f, 0.82f, 0.82f};
const Vec vec3_802f47d8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f47e4 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f47f0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f47fc = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4808 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4814 = {0.9f, 0.9f, 0.9f};
const Vec vec3_802f4820 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f482c = {0.9f, 0.9f, 0.9f};
const Vec vec3_802f4838 = {0.0f, 0.0f, 80.0f};
const Vec vec3_802f4844 = {0.0f, 0.0f, 80.0f};
const Vec vec3_802f4850 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f485c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4868 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4874 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4880 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f488c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4898 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f48a4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f48b0 = {-0.6f, 0.6f, 0.6f};
const Vec vec3_802f48bc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f48c8 = {0.6f, 0.6f, 0.6f};
const Vec vec3_802f48d4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f48e0 = {0.7f, 0.7f, 0.7f};
const Vec vec3_802f48ec = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f48f8 = {0.7f, 0.7f, 0.7f};
const Vec vec3_802f4904 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4910 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f491c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4928 = {0.8f, 0.8f, 0.8f};
const Vec vec3_802f4934 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4940 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f494c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4958 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802f4964 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f4970 = {0.5f, 0.5f, 0.5f};
const char str_802F497C[24] = "msg_menu_party_henkou";
const char str_802F4994[28] = "msg_menu_party_waza_kakunin";
const char str_802F49B0[20] = "msg_menu_party_waza";
const char str_802F49D8[16] = "EFF_m_yoshi2";
const char str_802F49E8[16] = "EFF_m_yoshi3";
const char str_802F49F8[16] = "EFF_m_yoshi4";
const char str_802F4A08[16] = "EFF_m_yoshi5";
const char str_802F4A18[16] = "EFF_m_yoshi6";
const char str_802F4A28[16] = "EFF_m_yoshi7";





static Vec winPartyMkVec(Vec base, f32 x, f32 y) {
    base.x = x;
    base.y = y;
    return base;
}

// winPartyDisp reconstruction. Every call, branch, immediate, switch jump table and
// .rodata/.sdata2/.data reference matches; the remaining diffs are stack layout and register
// allocation (compiler scheduling). The argument/scale Vecs are copied to stack locals and the
// constants are defined in this file so they are addressed with one base register + displacement
// like the original; a MUST_MATCH stack pad brings the frame to the original's 0x630.
// NONMATCHING (80.42%): register allocation/scheduling differ; all opcodes/immediates/calls/
// branches match (the item icon is read via ItemDataView, the waza cost is an unsigned byte, and
// the width scaling is split so it avoids FP contraction into fnmsubs).
void winPartyDisp(CameraId camId, WindowWork* work, s32 partyIdx) {
    f32 f18, f19, f20, f21, f22, f23, f24, f25, f26, f31;
    f32 t1, t2, t3, t4;
    s32 i, j, k;
    s32 idx;
    s32 pose;
    s32 count;
    PouchData* pouch;
    PouchPartyData* party;
    GXColor c21c, c220, c224, c228, c22c;
    Mtx m4D0, m4A0, m470, m440, m410, m3E0;
    Vec v, v2, p, s;
    Vec sv1, sv2, sv3, sv4, sv5, sv6, sv7, sv8, sv9, sv10, sv11, sv12, sv13, sv14;
    const char* name;
    char numbuf[16];

#ifdef MUST_MATCH
    // The original frames 0x630 bytes; reserving the extra 0x120 makes the offsets of the
    // Vec/Mtx temporaries line up with the target (docs/AGENT_BRIEF.md rule 7).
    volatile u8 frame_pad[0x120];

    frame_pad[0] = 0;
#endif
    (void)camId;
    f21 = WW_F32(work, 0xC4 + partyIdx * 0x14);
    f20 = WW_F32(work, 0xC8 + partyIdx * 0x14);
    winBgGX(work, 1, f21, f20);
    count = WW_S32(work, 0x1E0);
    if (count == 0) {
        return;
    }

    pouch = pouchGetPtr();
    party = &pouch->partyData[winPartyDt[WW_S32(work, 0x1D8)].field_0x0];

    winKirinukiGX(work, 0, f21 - float_10_8041d230,
                  float_20_8041d234 + (float_116_8041d238 + f20),
                  float_240_8041d23c, float_76_8041d240);

    f22 = float_110_8041d24c + f20;
    f24 = float_45_8041d244 + (f21 - float_10_8041d230);
    f25 = f24 - float_2_8041d248;

    c21c = dat_8041d21c;
    c220 = dat_8041d220;
    c224 = dat_8041d224;

    for (i = 0; i < 2; i++) {
        f26 = (i == 0) ? float_1_8041d250 : float_0p9_8041d254;
        f19 = f26 * (f32)FontGetMessageWidth(msgSearch(jumptable_802F4750[i]));
        f18 = float_0p5_8041d258 * (f32)FontGetMessageWidth(msgSearch(jumptable_802F4770[i]));

        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
        GXSetTevAlphaIn(0, 7, 1, 4, 7);

        f23 = f22 - (f32)(i * 0x20);
        winTexSet(0xAF, winPartyMkVec(vec3_802f4778, f25, f23 - float_4_8041d25c),
                  vec3_802f4784, dat_802f4720[i]);
        winFontInit();

        t1 = -((f19 * float_0p5_8041d258) - f24);
        p = winPartyMkVec(vec3_802f4790, t1, float_12_8041d260 + f23);
        s = winPartyMkVec(vec3_802f479c, f26, f26);
        winFontSet(&p, &s, &c220, msgSearch(jumptable_802F4750[i]));
        if (i < 1) {
            t2 = -((f18 * float_0p5_8041d258) - f24);
            p = winPartyMkVec(vec3_802f47a8, t2, float_21_8041d264 + f23);
            s = winPartyMkVec(vec3_802f47b4, float_0p5_8041d258, float_0p5_8041d258);
            winFontSet(&p, &s, &c224, msgSearch(jumptable_802F4770[i]));
        }
    }

    winIconInit();
    f24 = float_60_8041d268 + f24;
    f23 = float_4_8041d25c + f22;
    p = winPartyMkVec(vec3_802f47c0, f24, f23);
    sv1 = vec3_802f47cc;
    winIconSet(0x19F, &p, &sv1, &c21c);

    f19 = float_4_8041d25c + (f22 - float_32_8041d26c);
    for (k = 0; k < WW_S32(work, 0x204) - 2; k++) {
        p = winPartyMkVec(vec3_802f47d8, f19, f24 + (f32)(k * 0x22));
        sv2 = vec3_802f47e4;
        winIconSet(((ItemDataView*)itemDataTable)[87].icon_id, &p, &sv2, &c21c);
    }

    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    winTexSet(0x10,
              winPartyMkVec(vec3_802f47f0, f23,
                            float_8_8041d270 + (float_170_8041d274 + (f21 - float_10_8041d230))),
              vec3_802f47fc, c21c);
    winFontInit();

    t1 = float_10_8041d230 + (float_110_8041d24c + (f21 - float_10_8041d230));
    t2 = float_4_8041d25c + (float_12_8041d260 + f22);
    p = winPartyMkVec(vec3_802f4808, t1, t2);
    sv3 = vec3_802f4814;
    winFontSetR(&p, &sv3, &c220, str_8041D278, winZenkakuStr(party->currentHP));
    p = winPartyMkVec(vec3_802f4820,
                      float_8_8041d270 + (float_180_8041d27c + (f21 - float_10_8041d230)), t2);
    sv4 = vec3_802f482c;
    winFontSet(&p, &sv4, &c220, str_8041D278, winZenkakuStr(party->maximumHP));
    winZClear();

    f19 = f21 - float_140_8041d28c;
    for (k = 0; k < count; k++) {
        v = vec3_802f4838;
        PSMTXRotRad(m4D0, 0x78, float_0p5236_8041d280);
        t1 = float_360_8041d288 / (f32)count;
        PSMTXRotRad(m4A0, 0x79, float_deg2rad_8041d284 * ((t1 * (f32)k) - WW_F32(work, 0x1E4)));
        PSMTXMultVec(m4A0, &v, &v);
        PSMTXMultVec(m4D0, &v, &v);
        c228 = dat_8041d228;
        winKageGX(work, &c228, f19 + v.x, float_20_8041d234 + (f20 + v.y), v.z,
                  (k == WW_S32(work, 0x1DC)) ? float_0p8_8041d290 : float_0p6_8041d294);
    }

    for (j = 0; j < count; j++) {
        AnimData* anim;

        pose = ((s32*)work)[j + 0x68];
        v = vec3_802f4844;
        if (pose == -1) {
            continue;
        }
        anim = animPoseGetAnimDataPtr(pose);

        PSMTXRotRad(m410, 0x78, float_0p5236_8041d280);
        t1 = float_360_8041d288 / (f32)count;
        PSMTXRotRad(m3E0, 0x79, float_deg2rad_8041d284 * ((t1 * (f32)j) - WW_F32(work, 0x1E4)));
        PSMTXMultVec(m3E0, &v, &v);
        PSMTXMultVec(m410, &v, &v);

        if (j == WW_S32(work, 0x1DC)) {
            PSMTXTrans(m470, (f21 - float_140_8041d28c) + v.x,
                       float_20_8041d234 + (f20 + v.y), v.z);
            PSMTXScale(m440, float_1_8041d250, float_1_8041d250, float_neg1_8041d298);
            PSMTXRotRad(m3E0, 0x79, float_3p1416_8041d29c);
            PSMTXConcat(m470, m440, m470);
            PSMTXConcat(m470, m3E0, m470);
            animPoseSetMaterialFlagOff(pose, 0x40);
            for (idx = 0; idx < 7; idx++) {
                if (winPartyDt[idx].field_0x0 == ((s32*)work)[j + 0x6F]) {
                    break;
                }
            }

            if (WW_U16(work, 0x0) & 0x2000) {
                name = winPartyDt[idx].field_0x1C;
                if (winPartyDt[idx].field_0x0 == 4) {
                    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                    case 0: name = str_8041D200; break;
                    case 1: name = str_8041D2A0; break;
                    case 2: name = str_8041D2A4; break;
                    case 3: name = str_8041D2A8; break;
                    case 4: name = str_8041D2AC; break;
                    case 5: name = str_8041D2B0; break;
                    case 6: name = str_8041D2B8; break;
                    default: name = str_8041D2B8; break;
                    }
                }
            } else {
                name = winPartyDt[idx].field_0x14;
            }
            animPoseSetAnim(pose, name, 0);
            {
                s32 frames = (s32)*(f32*)(*(u8**)((u8*)anim + 0x24) + 0x8);

                animPoseSetLocalTime(pose, (f32)(gp->retraceCount % (u32)frames));
            }
        } else {
            PSMTXTrans(m470, (f21 - float_140_8041d28c) + v.x,
                       float_20_8041d234 + (f20 + v.y), v.z);
            PSMTXScale(m440, float_0p8_8041d290, float_0p8_8041d290, float_neg0p8_8041d2c0);
            PSMTXRotRad(m3E0, 0x79, float_3p1416_8041d29c);
            PSMTXConcat(m470, m440, m470);
            PSMTXConcat(m470, m3E0, m470);
            animPoseSetMaterialFlagOn(pose, 0x40);
            c22c = dat_8041d22c;
            animPoseSetMaterialEvtColor(pose, c22c);
            for (idx = 0; idx < 7; idx++) {
                if (winPartyDt[idx].field_0x0 == ((s32*)work)[j + 0x6F]) {
                    break;
                }
            }

            if (WW_U16(work, 0x0) & 0x2000) {
                name = winPartyDt[idx].field_0x1C;
                if (winPartyDt[idx].field_0x0 == 4) {
                    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                    case 0: name = str_8041D2C4; break;
                    case 1: name = str_8041D2C8; break;
                    case 2: name = str_8041D2CC; break;
                    case 3: name = str_8041D2D0; break;
                    case 4: name = str_8041D2D4; break;
                    case 5: name = str_8041D2D8; break;
                    case 6: name = str_8041D2E0; break;
                    default: name = str_8041D2E0; break;
                    }
                }
            } else {
                name = winPartyDt[idx].field_0x14;
            }
            animPoseSetAnim(pose, name, 0);
            animPoseSetLocalTime(pose, float_0_8041d2e8);
        }

        animPoseMain(pose);
        if (((s32*)work)[j + 0x6F] == 6) {
            v2 = vec3_802f4850;
            v2.x = (f21 - float_140_8041d28c) + v.x;
            v2.y = float_20_8041d234 + (f20 + v.y);
            v2.z = v.z;
            animPoseVivianMain(pose, &v2);
        }
        animPoseDrawMtx(pose, m470, 2, float_0_8041d2e8, float_5_8041d2ec);
    }

    if (count != 0) {
        winNameGX(work, 0, f21 - float_230_8041d2f0,
                  (float_20_8041d234 + (f20 - float_50_8041d2f4)) - float_10_8041d230,
                  float_200_8041d2f8, float_32_8041d26c);
        winFontInit();
        t2 = (float_20_8041d234 + ((f20 - float_50_8041d2f4) - float_4_8041d25c)) - float_10_8041d230;
        for (k = 0; k < 7; k++) {
            if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[k].field_0x0) {
                if (winPartyDt[k].field_0x0 != 4) {
                    name = msgSearch(winPartyDt[k].field_0x8);
                } else {
                    name = pouchGetYoshiName();
                }
                t3 = (f32)FontGetMessageWidth(name);
                p = winPartyMkVec(vec3_802f485c,
                                  ((float_200_8041d2f8 - t3) * float_0p5_8041d258) +
                                      (f21 - float_230_8041d2f0), t2);
                sv5 = vec3_802f4868;
                winFontSetEdge(&p, &sv5, &c21c, name);
            }
        }
    }

    if (count > 1) {
        winFontInit();
        t3 = float_0p8_8041d290 * (f32)FontGetMessageWidth(msgSearch(str_802F497C));
        t4 = float_neg230_8041d2fc - t3;
        f18 = (t4 * float_0p5_8041d258) + f21;
        p = winPartyMkVec(vec3_802f4874, f18, f20 - float_90_8041d300);
        sv6 = vec3_802f4880;
        winFontSet(&p, &sv6, &c220, msgSearch(str_802F497C));
        winIconInit();
        p = winPartyMkVec(vec3_802f488c, f18 - float_30_8041d304, f20 - float_100_8041d308);
        sv7 = vec3_802f4898;
        winIconSet(0x64, &p, &sv7, &c21c);
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        winTexSet(0xC0,
                  winPartyMkVec(vec3_802f48a4, (f21 - float_150_8041d30c) - float_50_8041d2f4,
                                f20 - float_16_8041d310),
                  vec3_802f48b0, c21c);
        winTexSet(0xC0,
                  winPartyMkVec(vec3_802f48bc, float_50_8041d2f4 + (f21 - float_150_8041d30c),
                                f20 - float_16_8041d310),
                  vec3_802f48b0, c21c);
    }

    winFontInit();
    t3 = (f32)FontGetMessageWidth(msgSearch(str_802F4994));
    t3 = float_0p7_8041d314 * t3;
    t4 = float_250_8041d318 - t3;
    f18 = f21 + t4;
    p = winPartyMkVec(vec3_802f48c8, f18, f20 - float_100_8041d308);
    sv8 = vec3_802f48d4;
    winFontSet(&p, &sv8, &c220, msgSearch(str_802F4994));
    winIconInit();
    p = winPartyMkVec(vec3_802f48e0, f18 - float_20_8041d234, f20 - float_110_8041d24c);
    sv9 = vec3_802f48ec;
    winIconSet(0x1F5, &p, &sv9, &c21c);
    f22 = float_50_8041d2f4 + f20;
    winWazaGX(work, 0, f21 - float_5_8041d2ec, f22, float_260_8041d31c, lbl_8041D320);
    winFontInit();
    t3 = (f32)FontGetMessageWidth(msgSearch(str_802F49B0));
    f18 = float_45_8041d244 + f20;
    t3 = float_0p8_8041d290 * t3;
    t4 = float_200_8041d2f8 - t3;
    p = winPartyMkVec(vec3_802f48f8, (t4 * float_0p5_8041d258) + f21, f18);
    sv10 = vec3_802f4904;
    winFontSetEdge(&p, &sv10, &c21c, msgSearch(str_802F49B0));
    p = winPartyMkVec(vec3_802f4910, (float_230_8041d2f0 + f21) - float_20_8041d234, f18);
    sv11 = vec3_802f491c;
    winFontSetEdge(&p, &sv11, &c21c, str_8041D324);

    t1 = float_10_8041d230 + f21;
    t2 = (float_238_8041d32c + f21) - float_20_8041d234;
    t3 = float_2_8041d248 + (float_2_8041d248 + ((float_17_8041d330 + float_230_8041d2f0 + f21) - float_20_8041d234));
    for (i = 0; i < WW_S32(work, 0x204); i++) {
        const char** waza = (const char**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20;
        f25 = (f22 - (f32)(i * 0x1A)) - float_30_8041d304;
        p = winPartyMkVec(vec3_802f4934, t1, f25);
        sv12 = vec3_802f4940;
        winFontSetLabel(&p, &sv12, &c220, *(const char**)waza[i]);
        sprintf(numbuf, str_8041D278, winZenkakuStr(*(u8*)(waza[i] + 0x11)));
        f31 = (f32)(FontGetMessageWidth(numbuf) >> 1);
        p = winPartyMkVec(vec3_802f494c, (t2 - f31) - float_2_8041d248, f25);
        sv13 = vec3_802f4958;
        winFontSet(&p, &sv13, &c220, numbuf);
        p = winPartyMkVec(vec3_802f4964, t3, f25 - float_10_8041d230);
        sv14 = vec3_802f4970;
        winFontSet(&p, &sv14, &c220, str_8041D324);
    }
}
