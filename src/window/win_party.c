/* The original prototypes for this unit pass GXColor by value.  The shared
   header declares pointer forms, so keep the two sets of names apart while
   including, then take the by-value prototypes for this translation unit. */
#define WIN_PARTY_BYVALUE
#define animPoseVivianMain animPoseVivianMain_ptrproto
#define winFontSet winFontSet_ptrproto
#define winFontSetLabel winFontSetLabel_ptrproto
#define winFontSetEdge winFontSetEdge_ptrproto
#define winFontSetR winFontSetR_ptrproto
#define winIconSet winIconSet_ptrproto
#include "window/win_party.h"
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
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>

#undef winFontSet
#undef winFontSetLabel
#undef winFontSetEdge
#undef winFontSetR
#undef winIconSet
#undef animPoseVivianMain
#include "window/win_party_protos.h"

extern WindowPartyDt winPartyDt[];

// winPartyDisp reaches every .rodata object of the unit through one base register
// (str_802F45D0).  The same offsets in the natural spelling name the objects used
// in winPartyInit and the other window units; the base form is what the original
// used, so reproduce it here.
extern u8 str_802F45D0[];
#define VP(o) (rp + (o))
#define VEC_AT(o) (*(const Vec*)VP(o))
#define GXC_AT(o) (*(const GXColor*)VP(o))
#define PTR_AT(o) (*(const char* const*)VP(o))

// .sdata2 constants and animation-name strings winPartyDisp reads
// (compiler-named in the original: the names encode their dol address).
extern const GXColor dat_8041d21c;
extern const GXColor dat_8041d220;
extern const GXColor dat_8041d224;
extern const GXColor dat_8041d228;
extern const GXColor dat_8041d22c;
extern const f32 float_10_8041d230;
extern const f32 float_20_8041d234;
extern const f32 float_116_8041d238;
extern const f32 float_240_8041d23c;
extern const f32 float_76_8041d240;
extern const f32 float_45_8041d244;
extern const f32 float_2_8041d248;
extern const f32 float_110_8041d24c;
extern const f32 float_1_8041d250;
extern const f32 float_0p9_8041d254;
extern const f32 float_0p5_8041d258;
extern const f32 float_4_8041d25c;
extern const f32 float_12_8041d260;
extern const f32 float_21_8041d264;
extern const f32 float_60_8041d268;
extern const f32 float_32_8041d26c;
extern const f32 float_8_8041d270;
extern const f32 float_170_8041d274;
extern const char str_8041D278[4];
extern const f32 float_180_8041d27c;
extern const f32 float_0p5236_8041d280;
extern const f32 float_deg2rad_8041d284;
extern const f32 float_360_8041d288;
extern const f32 float_140_8041d28c;
extern const f32 float_0p8_8041d290;
extern const f32 float_0p6_8041d294;
extern const f32 float_neg1_8041d298;
extern const f32 float_3p1416_8041d29c;
extern const char str_8041D200[4];
extern const char str_8041D2A0[4];
extern const char str_8041D2A4[4];
extern const char str_8041D2A8[4];
extern const char str_8041D2AC[4];
extern __declspec(section ".sdata2") const char str_8041D2B0[8];
extern __declspec(section ".sdata2") const char str_8041D2B8[8];
extern const f32 float_neg0p8_8041d2c0;
extern const char str_8041D2C4[4];
extern const char str_8041D2C8[4];
extern const char str_8041D2CC[4];
extern const char str_8041D2D0[4];
extern const char str_8041D2D4[4];
extern __declspec(section ".sdata2") const char str_8041D2D8[8];
extern __declspec(section ".sdata2") const char str_8041D2E0[8];
extern const f32 float_0_8041d2e8;
extern const f32 float_5_8041d2ec;
extern const f32 float_230_8041d2f0;
extern const f32 float_50_8041d2f4;
extern const f32 float_200_8041d2f8;
extern const f32 float_neg230_8041d2fc;
extern const f32 float_90_8041d300;
extern const f32 float_30_8041d304;
extern const f32 float_100_8041d308;
extern const f32 float_150_8041d30c;
extern const f32 float_16_8041d310;
extern const f32 float_0p7_8041d314;
extern const f32 float_250_8041d318;
extern const f32 float_260_8041d31c;
extern const f32 lbl_8041D320;
extern __declspec(section ".sdata2") const char str_8041D324[8];
extern const f32 float_238_8041d32c;
extern const f32 float_17_8041d330;

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
// Confirmed: our object's .rodata/.sdata2/.data are 0x78/0x44/0x54 while the original's are
// 0x468/0x158/0x188. The missing entries all belong to undecompiled winPartyDisp (0x169C of
// code) and to winPartyDt: winPartyDt is the .data table at 0x0 (0xFC) that also emits the
// first ~0x30 .rodata strings and the .sdata waza strings, and winPartyDisp owns the two
// .data jump tables at 0xFC/0x118 plus most of .sdata2. Until those are decompiled the
// section-relative offsets of winPartyInit's literals cannot match; no source shape inside
// this function can change a section's size or another function's data.
// NONMATCHING (99.97%): symbol-address offsets only; see the note above.
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

// NONMATCHING (81.06%): full behavioural reconstruction from the original
// assembly.  Every drawing stage is present and in the original order: the two
// stat rows, the partner/badge icons, HP, the rotating shadows, every party
// pose (with the selected/non-selected anim strings, the yoshi-colour switch
// and the retrace-count local time), the name, the henkou/waza labels and the
// move list.  The call sequence and control flow match.  What still differs is
// register allocation, the stack frame (the original keeps more distinct Vec
// temporaries, so its frame is 0x630 vs our 0x530), and the compiler pool
// constants for the (f32)(u16) conversions, which the original addresses as
// 0x3f8/0x400(r31) but this compiler emits as `lis @615@ha; lfd @615@l`.
// NONMATCHING (81.06%): register allocation, stack frame and pool-constant addressing only
void winPartyDisp(CameraId camId, WindowWork* work, s32 idx) {
    PouchData* pouch;
    PouchPartyData* pd;
    GXColor white = dat_8041d21c;
    GXColor black = dat_8041d220;
    f32 x = WW_F32(work, 0xC4 + idx * 0x14);
    f32 y = WW_F32(work, 0xC8 + idx * 0x14);
    f32 f18, f19, f20, f21, f22, f23, f24, f25, f26;
    s32 i, j, k;
    Mtx mtxRotX, mtxRotY, mtxTrans, mtxScale, mtxS, mtxS2;
    char buf[0x40];
    const u8* rp = str_802F45D0;

    (void)camId;

    winBgGX(work, 1, x, y);
    if (WW_S32(work, 0x1E0) == 0) {
        return;
    }

    pouch = pouchGetPtr();
    pd = &pouch->partyData[winPartyDt[WW_S32(work, 0x1D8)].field_0x0];
    winKirinukiGX(work, 0, x - float_10_8041d230,
                  float_20_8041d234 + (float_116_8041d238 + y),
                  float_240_8041d23c, float_76_8041d240);

    f22 = float_110_8041d24c + y;
    f24 = float_45_8041d244 + (x - float_10_8041d230);
    f25 = f24 - float_2_8041d248;

    {
        Vec tr, sc, pos, sc2, pos2, sc3;
        GXColor red = dat_8041d224;

        for (i = 0, j = 0; i < 2; i++, j += 0x20) {
            GXColor colors[2];
            const char* labels[2];
            const char* sublabels[2];

            colors[0] = GXC_AT(0x150);
            colors[1] = GXC_AT(0x154);
            labels[0] = PTR_AT(0x180);
            labels[1] = PTR_AT(0x184);
            sublabels[0] = PTR_AT(0x1a0);
            sublabels[1] = PTR_AT(0x1a4);

            f26 = (i == 0) ? float_1_8041d250 : float_0p9_8041d254;
            f19 = f26 * (f32)(u16)FontGetMessageWidth(msgSearch(labels[i]));
            f18 = float_0p5_8041d258 * (f32)(u16)FontGetMessageWidth(msgSearch(sublabels[i]));
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            f23 = f22 - (f32)j;
            tr = VEC_AT(0x1a8);
            sc = VEC_AT(0x1b4);
            tr.x = f25;
            tr.y = f23 - float_4_8041d25c;
            winTexSet(0xAF, tr, sc, colors[i]);
            winFontInit();
            pos = VEC_AT(0x1c0);
            sc2 = VEC_AT(0x1cc);
            pos.x = f24 - f19 * float_0p5_8041d258;
            pos.y = float_12_8041d260 + f23;
            sc2.x = f26;
            sc2.y = f26;
            sc2.z = f26;
            winFontSet(pos, sc2, black, msgSearch(labels[i]));
            if (i < 1) {
                pos2 = VEC_AT(0x1d8);
                sc3 = VEC_AT(0x1e4);
                pos2.x = f24 - f18 * float_0p5_8041d258;
                pos2.y = float_21_8041d264 + f23;
                winFontSet(pos2, sc3, red, msgSearch(sublabels[i]));
            }
        }
    }

    winIconInit();
    f24 = float_60_8041d268 + f24;
    f23 = float_4_8041d25c + f22;
    {
        Vec tr, sc;

        tr = VEC_AT(0x1f0);
        sc = VEC_AT(0x1fc);
        tr.x = f24;
        tr.y = f23;
        winIconSet(0x19F, &tr, &sc, white);
    }

    f19 = float_4_8041d25c + (f22 - float_32_8041d26c);
    {
        Vec tr, sc;

        for (i = 0, j = 0; i < WW_S32(work, 0x204) - 2; i++, j += 0x22) {
            tr = VEC_AT(0x208);
            sc = VEC_AT(0x214);
            tr.x = f24 + (f32)j;
            tr.y = f19;
            winIconSet(*(u16*)&itemDataTable[87].icon_id, &tr, &sc, white);
        }
    }

    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    {
        Vec tr, sc;

        tr = VEC_AT(0x220);
        sc = VEC_AT(0x22c);
        tr.x = float_8_8041d270 + (float_170_8041d274 + (x - float_10_8041d230));
        tr.y = f23;
        winTexSet(0x10, tr, sc, white);
    }
    winFontInit();
    {
        Vec pos, sc;

        pos = VEC_AT(0x238);
        sc = VEC_AT(0x244);
        pos.x = float_10_8041d230 + (float_110_8041d24c + (x - float_10_8041d230));
        f18 = float_4_8041d25c + (float_12_8041d260 + f22);
        pos.y = f18;
        winFontSetR(pos, sc, black, str_8041D278, winZenkakuStr(pd->currentHP));
    }
    {
        Vec pos, sc;

        pos = VEC_AT(0x250);
        sc = VEC_AT(0x25c);
        pos.x = float_8_8041d270 + (float_180_8041d27c + (x - float_10_8041d230));
        pos.y = f18;
        winFontSet(pos, sc, black, str_8041D278, winZenkakuStr(pd->maximumHP));
    }

    winZClear();
    f19 = x - float_140_8041d28c;
    {
        Vec pS;

        for (i = 0; i < WW_S32(work, 0x1E0); i++) {
            GXColor col = dat_8041d228;
            f32 d;

            pS = VEC_AT(0x268);
            PSMTXRotRad(mtxS, 'x', float_0p5236_8041d280);
            d = float_360_8041d288 / (f32)WW_S32(work, 0x1E0);
            d = d * (f32)i - WW_F32(work, 0x1E4);
            PSMTXRotRad(mtxS2, 'y', float_deg2rad_8041d284 * d);
            PSMTXMultVec(mtxS2, &pS, &pS);
            PSMTXMultVec(mtxS, &pS, &pS);
            winKageGX(work, &col, f19 + pS.x, float_20_8041d234 + (y + pS.y), pS.z,
                      (i == WW_S32(work, 0x1DC)) ? float_0p8_8041d290 : float_0p6_8041d294);
        }
    }

    {
        Vec p, pos;

        for (i = 0; i < WW_S32(work, 0x1E0); i++) {
            s32 pose;
            AnimData* animData;
            f32 d;
            const char* anim;

            p = VEC_AT(0x274);
            pose = ((s32*)work)[i + 0x68];
            if (pose == -1) {
                continue;
            }
            animData = animPoseGetAnimDataPtr(pose);
            PSMTXRotRad(mtxRotX, 'x', float_0p5236_8041d280);
            d = float_360_8041d288 / (f32)WW_S32(work, 0x1E0);
            d = d * (f32)i - WW_F32(work, 0x1E4);
            PSMTXRotRad(mtxRotY, 'y', float_deg2rad_8041d284 * d);
            PSMTXMultVec(mtxRotY, &p, &p);
            PSMTXMultVec(mtxRotX, &p, &p);

            if (i == WW_S32(work, 0x1DC)) {
                PSMTXTrans(mtxTrans, (x - float_140_8041d28c) + p.x, float_20_8041d234 + (y + p.y), p.z);
                PSMTXScale(mtxScale, float_1_8041d250, float_1_8041d250, float_neg1_8041d298);
                PSMTXRotRad(mtxRotY, 'y', float_3p1416_8041d29c);
                PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
                PSMTXConcat(mtxTrans, mtxRotY, mtxTrans);
                animPoseSetMaterialFlagOff(pose, 0x40);
                for (k = 0; k < 7; k++) {
                    if (winPartyDt[k].field_0x0 == ((s32*)work)[i + 0x6F]) {
                        break;
                    }
                }
                if (WW_U16(work, 0x0) & 0x2000) {
                    anim = winPartyDt[k].field_0x1C;
                    if (winPartyDt[k].field_0x0 == PARTNER_YOSHI) {
                        switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                        case 0: anim = str_8041D200; break;
                        case 1: anim = str_8041D2A0; break;
                        case 2: anim = str_8041D2A4; break;
                        case 3: anim = str_8041D2A8; break;
                        case 4: anim = str_8041D2AC; break;
                        case 5: anim = str_8041D2B0; break;
                        case 6: anim = str_8041D2B8; break;
                        }
                    }
                } else {
                    anim = winPartyDt[k].field_0x14;
                }
                animPoseSetAnim(pose, anim, 0);
                animPoseSetLocalTime(pose, (f32)(gp->retraceCount %
                    (s32)*(const f32*)(*(const u8**)((const u8*)animData + 0x24) + 0x8)));
            } else {
                PSMTXTrans(mtxTrans, (x - float_140_8041d28c) + p.x, float_20_8041d234 + (y + p.y), p.z);
                PSMTXScale(mtxScale, float_0p8_8041d290, float_0p8_8041d290, float_neg0p8_8041d2c0);
                PSMTXRotRad(mtxRotY, 'y', float_3p1416_8041d29c);
                PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
                PSMTXConcat(mtxTrans, mtxRotY, mtxTrans);
                animPoseSetMaterialFlagOn(pose, 0x40);
                animPoseSetMaterialEvtColor(pose, dat_8041d22c);
                for (k = 0; k < 7; k++) {
                    if (winPartyDt[k].field_0x0 == ((s32*)work)[i + 0x6F]) {
                        break;
                    }
                }
                if (WW_U16(work, 0x0) & 0x2000) {
                    anim = winPartyDt[k].field_0x1C;
                    if (winPartyDt[k].field_0x0 == PARTNER_YOSHI) {
                        switch (pouchGetPartyColor(PARTNER_YOSHI)) {
                        case 0: anim = str_8041D2C4; break;
                        case 1: anim = str_8041D2C8; break;
                        case 2: anim = str_8041D2CC; break;
                        case 3: anim = str_8041D2D0; break;
                        case 4: anim = str_8041D2D4; break;
                        case 5: anim = str_8041D2D8; break;
                        case 6: anim = str_8041D2E0; break;
                        }
                    }
                } else {
                    anim = winPartyDt[k].field_0x14;
                }
                animPoseSetAnim(pose, anim, 0);
                animPoseSetLocalTime(pose, float_0_8041d2e8);
            }

            animPoseMain(pose);
            if (((s32*)work)[i + 0x6F] == PARTNER_VIVIAN) {
                pos = VEC_AT(0x280);
                pos.x = (x - float_140_8041d28c) + p.x;
                pos.y = float_20_8041d234 + (y + p.y);
                pos.z = p.z;
                animPoseVivianMain(pose, pos);
            }
            animPoseDrawMtx(pose, mtxTrans, 2, float_0_8041d2e8, float_5_8041d2ec);
        }
    }

    if (WW_S32(work, 0x1E0) != 0) {
        winNameGX(work, 0, x - float_230_8041d2f0,
                  float_20_8041d234 + (y - float_50_8041d2f4) - float_10_8041d230,
                  float_200_8041d2f8, float_32_8041d26c);
        winFontInit();
        f18 = float_20_8041d234 + ((y - float_50_8041d2f4) - float_4_8041d25c) - float_10_8041d230;
        {
            Vec pos, sc;

            for (i = 0; i < 7; i++) {
                if (((s32*)work)[WW_S32(work, 0x1DC) + 0x6F] == winPartyDt[i].field_0x0) {
                    const char* name = (PARTNER_YOSHI == winPartyDt[i].field_0x0)
                        ? pouchGetYoshiName() : msgSearch(winPartyDt[i].field_0x8);
                    u16 w = FontGetMessageWidth(name);

                    pos = VEC_AT(0x28c);
                    sc = VEC_AT(0x298);
                    pos.x = (x - float_230_8041d2f0)
                          + (float_200_8041d2f8 - (f32)w) * float_0p5_8041d258;
                    pos.y = f18;
                    winFontSetEdge(pos, sc, white, name);
                }
            }
        }
    }

    if (WW_S32(work, 0x1E0) > 1) {
        f32 w;

        winFontInit();
        w = (f32)(u16)FontGetMessageWidth(msgSearch((const char*)VP(0x3ac)));
        f18 = x + (float_neg230_8041d2fc - float_0p8_8041d290 * w) * float_0p5_8041d258;
        {
            Vec pos, sc;

            pos = VEC_AT(0x2a4);
            sc = VEC_AT(0x2b0);
            pos.x = f18;
            pos.y = y - float_90_8041d300;
            winFontSet(pos, sc, black, msgSearch((const char*)VP(0x3ac)));
        }

        winIconInit();
        {
            Vec tr, sc;

            tr = VEC_AT(0x2bc);
            sc = VEC_AT(0x2c8);
            tr.x = f18 - float_30_8041d304;
            tr.y = y - float_100_8041d308;
            winIconSet(0x64, &tr, &sc, white);
        }

        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        {
            Vec tr, sc;

            tr = VEC_AT(0x2d4);
            sc = VEC_AT(0x2e0);
            tr.x = (x - float_150_8041d30c) - float_50_8041d2f4;
            tr.y = y - float_16_8041d310;
            winTexSet(0xC0, tr, sc, white);
        }
        {
            Vec tr, sc;

            tr = VEC_AT(0x2ec);
            sc = VEC_AT(0x2f8);
            tr.x = (x - float_150_8041d30c) + float_50_8041d2f4;
            tr.y = y - float_16_8041d310;
            winTexSet(0xC0, tr, sc, white);
        }
    }

    {
        f32 w;

        winFontInit();
        w = (f32)(u16)FontGetMessageWidth(msgSearch((const char*)VP(0x3c4)));
        {
            f32 t = float_0p7_8041d314 * w;
            f18 = x + (float_250_8041d318 - t);
        }
        {
            Vec pos, sc;

            pos = VEC_AT(0x304);
            sc = VEC_AT(0x310);
            pos.x = f18;
            pos.y = y - float_100_8041d308;
            winFontSet(pos, sc, black, msgSearch((const char*)VP(0x3c4)));
        }

        winIconInit();
        {
            Vec tr, sc;

            tr = VEC_AT(0x31c);
            sc = VEC_AT(0x328);
            tr.x = f18 - float_20_8041d234;
            tr.y = y - float_110_8041d24c;
            winIconSet(0x1F5, &tr, &sc, white);
        }
    }

    {
        f32 w;
        Vec pos, sc, pos2, sc2;

        f22 = float_50_8041d2f4 + y;
        winWazaGX(work, 0, x - float_5_8041d2ec, f22, float_260_8041d31c, lbl_8041D320);
        winFontInit();
        w = (f32)(u16)FontGetMessageWidth(msgSearch((const char*)VP(0x3e0)));
        f18 = float_45_8041d244 + y;
        pos = VEC_AT(0x334);
        sc = VEC_AT(0x340);
        pos.x = x + (float_200_8041d2f8 - float_0p8_8041d290 * w) * float_0p5_8041d258;
        pos.y = f18;
        winFontSetEdge(pos, sc, white, msgSearch((const char*)VP(0x3e0)));

        f19 = float_230_8041d2f0 + x;
        pos2 = VEC_AT(0x34c);
        sc2 = VEC_AT(0x358);
        pos2.x = f19 - float_20_8041d234;
        pos2.y = f18;
        winFontSetEdge(pos2, sc2, white, str_8041D324);

        f23 = (float_238_8041d32c + x) - float_20_8041d234;
        f21 = float_10_8041d230 + x;
        f24 = float_2_8041d248 + (float_2_8041d248
              + ((float_17_8041d330 + f19) - float_20_8041d234));
        f20 = float_2_8041d248;
        for (i = 0, j = 0; i < WW_S32(work, 0x204); i++, j += 0x1a) {
            f25 = f22 - (f32)j - float_30_8041d304;
            {
                Vec pos3, sc3;

                pos3 = VEC_AT(0x364);
                sc3 = VEC_AT(0x370);
                pos3.x = f21;
                pos3.y = f25;
                winFontSetLabel(&pos3, &sc3, black,
                    *(const char**)((void**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20)[i]);
            }
            sprintf(buf, "%s", winZenkakuStr(
                ((const u8*)((void**)winPartyDt[WW_S32(work, 0x1D8)].field_0x20)[i])[0x11]));
            {
                Vec pos4, sc4;

                pos4 = VEC_AT(0x37c);
                sc4 = VEC_AT(0x388);
                pos4.x = f23 - (f32)((u16)FontGetMessageWidth(buf) >> 1) - f20;
                pos4.y = f25;
                winFontSet(pos4, sc4, black, buf);
            }
            {
                Vec pos5, sc5;

                pos5 = VEC_AT(0x394);
                sc5 = VEC_AT(0x3a0);
                pos5.x = f24;
                pos5.y = f25 - float_10_8041d230;
                winFontSet(pos5, sc5, black, str_8041D324);
            }
        }
    }
}
