#include "battle/sac/sac_bakugame.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "battle/battle_disp.h"
#include "battle/battle_database_common.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_starstone.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

extern BattleWork* _battleWorkPointer;

typedef struct BakuGameWorkEntry {
    s32 field_0x0;   // 0x0
    s32 field_0x4;   // 0x4
    s32 type;        // 0x8, 0x100/0x200/0x400
    s32 index;       // 0xC
    u8 field_0x10[0x14 - 0x10];
    Vec pos;         // 0x14
    f32 field_0x20;  // 0x20
} BakuGameWorkEntry;

// Bounce-arrow counters living inside BattleWork at 0x1F2C (bytes 0..4 unsigned, 5..6 signed).
typedef struct BakuGameBouncePos {
    u8 field_0x0;
    u8 field_0x1;
    u8 field_0x2;
    u8 field_0x3;
    u8 field_0x4;
    s8 field_0x5;
    s8 field_0x6;
} BakuGameBouncePos;

struct BakuGameWork {
    s32 flags;                        // 0x0
    u8 field_0x4[0xC - 0x4];          // 0x4
    BakuGameWorkEntry entries[3];     // 0xC
    u8 field_0x78[0x88 - 0x78];       // 0x78
    Vec position;                     // 0x88
    u8 field_0x94[0xC4 - 0x94];       // 0x94
    f32 field_0xC4;                   // 0xC4
    u8 field_0xC8[0xD0 - 0xC8];       // 0xC8
    f32 field_0xD0;                   // 0xD0
    u8 field_0xD4[0xDC - 0xD4];       // 0xD4
    f32 field_0xDC;                   // 0xDC
    f32 field_0xE0;                   // 0xE0
    u8 field_0xE4[0xE8 - 0xE4];       // 0xE4
    f32 field_0xE8;                   // 0xE8
    s32 field_0xEC;                   // 0xEC
    u8 field_0xF0[0xF4 - 0xF0];       // 0xF0
    s32 field_0xF4;                   // 0xF4
    u8 field_0xF8[0x80];              // 0xF8, weapon copy
    s8 field_0x178[8];                // 0x178, inside the copied weapon
    u8 field_0x180[0x1B8 - 0x180];    // 0x180
    u8 field_0x1B8[0x1C0 - 0x1B8];    // 0x1B8
    s32 field_0x1C0;                  // 0x1C0
    s32 field_0x1C4;                  // 0x1C4
    s32 field_0x1C8;                  // 0x1C8
    s8 field_0x1CC;                   // 0x1CC
    u8 field_0x1CD[0x1D0 - 0x1CD];    // 0x1CD
    Vec field_0x1D0;                  // 0x1D0
    Vec field_0x1DC;                  // 0x1DC
    Vec field_0x1E8;                  // 0x1E8
    f32 field_0x1F4;                  // 0x1F4
    f32 field_0x1F8;                  // 0x1F8
    u8 field_0x1FC[0x200 - 0x1FC];    // 0x1FC
    s32 field_0x200;                  // 0x200
    s32 state;                        // 0x204
    s32 frame;                        // 0x208
    Vec field_0x20C;                  // 0x20C
    Vec field_0x218;                  // 0x218
    Vec field_0x224;                  // 0x224
    u8 field_0x230[0xC];              // 0x230
    Vec field_0x23C;                  // 0x23C
    Vec field_0x248;                  // 0x248
    void* starStone;                  // 0x254
};

// Original .rodata layout for this translation unit (order matters for address matching).
const char str_802FEA98[20] = "btl_wn_sac_bakugame";
const char str_802FEAAC[16] = "msg_bakugame\000\000\000";
const char str_802FEABC[12] = "msg_sac_3\000\000";
const Vec vec3_802feac8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fead4 = { 0.0f, 0.0f, 0.0f };
const u32 dat_802feae0[] = { 0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007 };
const u32 dat_802feb00[] = { 0x00000100, 0x00000200, 0x00000400 };
const Vec vec3_802feb0c = { 0.0f, -1000.0f, 0.0f };
const Vec vec3_802feb18 = { 125.0f, 0.0f, -30.0f };
const Vec vec3_802feb24 = { 125.0f, 0.0f, 100.0f };
const Vec vec3_802feb30 = { 125.0f, 0.0f, 100.0f };
const Vec vec3_802feb3c = { 0.0f, -1000.0f, 0.0f };
const Vec vec3_802feb48 = { -125.0f, 500.0f, -30.0f };
const Vec vec3_802feb54 = { -125.0f, 0.0f, -30.0f };
const Vec vec3_802feb60 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802feb6c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802feb78 = { -125.0f, 0.0f, -30.0f };
const Vec vec3_802feb84 = { 125.0f, 0.0f, -30.0f };
const Vec vec3_802feb90 = { 0.0f, -50.0f, 150.0f };
const Vec vec3_802feb9c = { -125.0f, 0.0f, -30.0f };
const Vec vec3_802feba8 = { 125.0f, 0.0f, -30.0f };
const Vec vec3_802febb4 = { 125.0f, 0.0f, 100.0f };
const Vec vec3_802febc0 = { 0.0f, -50.0f, 150.0f };
const Vec vec3_802febcc = { 0.0f, -50.0f, 150.0f };
const Vec vec3_802febd8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802febe4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802febf0 = { 300.0f, 0.0f, 0.0f };
const u32 dat_802febfc[] = { 0xC2FA0000, 0x42480000, 0x00000000, 0x42FA0000, 0x42480000, 0x00000000, 0x00000000, 0x41200000, 0x43160000 };
const u32 dat_802fec20[] = { 0x00000000, 0x00000001, 0x00000002, 0x00000000, 0x00000002, 0x00000001, 0x00000001, 0x00000000, 0x00000002, 0x00000001, 0x00000002, 0x00000000, 0x00000002, 0x00000000, 0x00000001, 0x00000002, 0x00000001, 0x00000000 };
const f32 dat_802fec68[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
const char str_802FECA8[20] = "SFX_BTL_SAC_START1\000";
const char str_802FECBC[20] = "SFX_BTL_SAC_START2\000";
const char str_802FECD0[20] = "SFX_BTL_SAC_START3\000";
const char str_802FECE4[8] = "PKR_D_1";
const char str_802FECEC[8] = "PNK_T_2";
const char str_802FECF4[8] = "PWD_D_1";
const char str_802FECFC[8] = "PYS_D_1";
const char str_802FED04[8] = "PTR_H_1";
const char str_802FED0C[8] = "PBM_D_1";
const char str_802FED14[12] = "PCH_A2_3\000\000\000";
const char str_802FED20[20] = "SFX_BTL_SAC_TIME1\000\000";
const char str_802FED34[20] = "SFX_BTL_SAC_TIME4\000\000";
const char str_802FED48[20] = "SFX_BTL_SAC_TIME2\000\000";
const char str_802FED5C[28] = "SFX_VOICE_MARIO_SURPRISED1\000";
const char str_802FED78[20] = "SFX_BTL_SAC_TIME3\000\000";
const Vec vec3_802fed8c = { 0.0f, 0.0f, 0.0f };
const char str_802FED98[20] = "SFX_BTL_SAC_TIME5\000\000";
const f64 double_to_int_802fedb0 = 4503601774854144;
const char str_802FEDB8[12] = "c_star_bom\000";
const u32 gap_04_802FEDC4_rodata[] = { 0x00000000 };

// Original .sdata2 layout for this translation unit. The scalars are only declared here and defined at the
// bottom of the file, so MWCC references the named symbols instead of folding the values.
const u32 dat_80421600 = 0x000000FF;
extern const f32 float_neg1_80421604;
extern const f32 float_1_80421608;
extern const f32 float_180_8042160c;
#ifdef MUST_MATCH
// volatile forces MWCC to reload the GX quad position/UV constants instead of CSE-ing one load per function.
extern volatile const f32 float_neg20_80421610;
extern volatile const f32 float_40_80421614;
extern volatile const f32 float_0_80421618;
extern volatile const f32 float_20_8042161c;
#else
extern const f32 float_neg20_80421610;
extern const f32 float_40_80421614;
extern const f32 float_0_80421618;
extern const f32 float_20_8042161c;
#endif
extern const f32 float_100_80421620;
extern const f32 float_90_80421624;
extern const f32 float_270_80421628;
const char str_8042162C[8] = "M_N_7\000\000";
extern const f32 float_neg2_80421634;
extern const f32 float_5_80421638;
extern const f32 float_33_8042163c;
extern const f32 float_66_80421640;
extern const f32 float_300_80421644;
extern const f32 float_1p5_80421648;
extern const f32 float_neg0p3_8042164c;
extern const f32 float_110_80421650;
extern const f32 float_4096_80421654;
extern const f32 float_0p25_80421658;
extern const f32 float_2_8042165c;
extern const f32 float_3_80421660;
extern const f32 float_4_80421664;
extern const f32 float_6_80421668;
const char str_8042166C[4] = "S_1";
const char str_80421670[4] = "S_2";
const char str_80421674[4] = "S_3";
extern const f32 float_50_80421678;
extern const f32 float_neg1000_8042167c;
extern const f32 float_neg800_80421680;
extern const f32 float_80_80421684;
extern const f32 float_neg0p5_80421688;
extern const f32 float_70_8042168c;
extern const f32 float_neg200_80421690;
extern const f32 float_0p1_80421694;
extern const f32 float_7_80421698;
const char str_8042169C[8] = "M_W_1\000\000";
const char str_804216A4[8] = "M_U_2\000\000";
extern const f32 float_30_804216ac;
extern const f32 float_neg100_804216b0;
extern const f32 float_26_804216b4;
const char str_804216B8[8] = "M_C_1\000\000";
extern const f32 float_900_804216c0;
const char str_804216C4[4] = "Z_1";
extern const f32 float_37_804216c8;
extern const f32 float_2160_804216cc;

void bakuGameDispStar(void) {
}

void bakuGameDisp3D(void) {
    BakuGameWork* baku = GetBakuGamePtr();
    CameraEntry* cam = camGetPtr(4);
    Mtx scale;
    Mtx trans;
    Mtx mtx;
    GXTexObj texObj;
    s32 idx;

    PSMTXScale(scale, float_neg1_80421604, float_1_80421608, float_1_80421608);
    PSMTXTrans(trans, baku->position.x, baku->position.y, baku->position.z);
    PSMTXConcat(trans, scale, mtx);
    if (baku->field_0xEC != -1) {
        animPoseDrawMtx(baku->field_0xEC, mtx, 2, float_180_8042160c, baku->field_0xC4);
    }
    if (baku->flags & 1) {
        void* file;
        TEXPalette* pal;

        GXSetCullMode(0);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
        GXSetNumChans(1);
        GXSetChanCtrl(4, 0, 1, 0, 0, 2, 2);
        GXSetChanMatColor(4, *(GXColor*)&dat_80421600);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        GXSetNumTevStages(1);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 0xC, 8, 0xA);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, 5, 4, 7);
        GXSetTevOrder(0, 0, 0, 4);
        file = BattleAudienceBaseGetPtr()->normalTex;
        pal = *(TEXPalette**)(*(u8**)((u8*)file + 0xA0));
        TEXGetGXTexObjFromPalette(pal, &texObj, baku->field_0x200);
        GXLoadTexObj(&texObj, 0);
        PSMTXTrans(trans, baku->field_0x1D0.x, baku->field_0x1D0.y, baku->field_0x1D0.z);
        PSMTXConcat(cam->view, trans, mtx);
        GXLoadPosMtxImm(mtx, 0);
        {
            f32 texCoords[16] = {
                0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            };

            idx = (baku->field_0x1CC == 1) ? 4 : 0;
            GXBegin(0x80, 0, 4);
            GXPosition3f32(float_neg20_80421610, float_40_80421614, float_0_80421618);
            GXTexCoord2f32(texCoords[(idx + 0) * 2], texCoords[(idx + 0) * 2 + 1]);
            GXPosition3f32(float_20_8042161c, float_40_80421614, float_0_80421618);
            GXTexCoord2f32(texCoords[(idx + 1) * 2], texCoords[(idx + 1) * 2 + 1]);
            GXPosition3f32(float_20_8042161c, float_0_80421618, float_0_80421618);
            GXTexCoord2f32(texCoords[(idx + 2) * 2], texCoords[(idx + 2) * 2 + 1]);
            GXPosition3f32(float_neg20_80421610, float_0_80421618, float_0_80421618);
            GXTexCoord2f32(texCoords[(idx + 3) * 2], texCoords[(idx + 3) * 2 + 1]);
            GXEnd();
        }
    }
}

// NONMATCHING (98.33%): MWCC assigns baku to r31/wp to r30 where the original uses baku=r30/wp=r31, which also
// swaps the loop induction registers (r27/r28).
void bakuGameDisp2D(void) {
    BattleWork* wp;
    BakuGameWork* baku;
    s32 i;
    s32 flag;

    baku = GetBakuGamePtr();
    wp = _battleWorkPointer;
    for (i = 0; i < 3; i++) {
        BakuGameWorkEntry* entry = &baku->entries[i];
        s16 icon;

        switch (entry->type) {
            case 0x100:
                icon = 0x64;
                break;
            case 0x200:
                icon = 0x66;
                break;
            case 0x400:
                icon = 0x68;
                break;
            default:
                icon = 0;
                break;
        }
        if ((u16)icon != 0) {
            Vec screen;

            btlGetScreenPoint(&entry->pos, &screen);
            iconDispGx(&screen, 0x10, icon, entry->field_0x20);
        }
    }
    flag = 0;
    if (baku->field_0xD0 >= float_100_80421620) {
        BakuGameBouncePos* p = (BakuGameBouncePos*)((u8*)wp + 0x1F2C);

        flag = 1;
        p->field_0x0 = 0;
        p->field_0x3 = 0xFF;
        p->field_0x1 += p->field_0x5;
        p->field_0x2 += p->field_0x6;
        if (p->field_0x1 <= 0x80 && p->field_0x5 <= 0) {
            p->field_0x6 = 0x10;
            p->field_0x5 = 0x10;
        }
        if (p->field_0x1 >= 0xF5 && p->field_0x5 >= 0) {
            p->field_0x6 = -0x10;
            p->field_0x5 = -0x10;
        }
    }
    BattleAcDrawGauge((s32)baku->field_0xDC, (s32)baku->field_0xE0, 0xB1, 3, 0x21, 0x42, 0x64, flag, baku->field_0xE8);
}

void bakuGameAudienceSurprise(void) {
    s32 i;

    for (i = 0; i < 200; i++) {
        BattleAudienceMember* member = BattleAudienceGetPtr(i);

        if ((u8)BattleAudience_GetWaiting(i)) {
            member->flags |= 0x10;
            memset(&member->interpFramesLeft, 0, 0x34);
            if (!bakuGameAudienceCanThrowPos(i)) {
                BattleAudience_SetAnim(i, 7, 0);
                if (irand(2) == 0) {
                    member->field_0xE8 = float_90_80421624;
                } else {
                    member->field_0xE8 = float_270_80421628;
                }
            } else {
                BattleAudience_SetAnim(i, 3, 0);
            }
        }
    }
}

void bakuGameEnemySurprise(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL && unit->mAlliance == 1 && !(unit->mAttributeFlags & 0x20000) &&
            !BtlUnit_CheckStatus(unit, 0x1B)) {
            BtlUnit_SetBodyAnimType(unit, 0x27);
        }
    }
}

// NONMATCHING (95.27%): MWCC uses the .rodata section base for the PKR_D_1..PCH_A2_3 switch strings where the
// original uses str_802FEA98 + offsets, and an anonymous jump table where the original names jumptable_803B0E94.
void bakuGameMarioSurprise(BattleUnit* unit, BattleUnit* party) {
    BtlUnit_SetBodyAnim(unit, str_8042162C);
    if (bakuGamePartyExist(party)) {
        switch (party->initialKind) {
            case 0xE0:
                BtlUnit_SetBodyAnim(party, str_802FECE4);
                break;
            case 0xE1:
                BtlUnit_SetBodyAnim(party, str_802FECEC);
                break;
            case 0xE2:
                BtlUnit_SetBodyAnim(party, str_802FECF4);
                break;
            case 0xE3:
                BtlUnit_SetBodyAnim(party, str_802FECFC);
                break;
            case 0xE4:
                BtlUnit_SetBodyAnim(party, str_802FED04);
                break;
            case 0xE5:
                BtlUnit_SetBodyAnim(party, str_802FED0C);
                break;
            case 0xE6:
                BtlUnit_SetBodyAnim(party, str_802FED14);
                break;
        }
    }
}

void bakuGameAudienceSurpriseReset(void) {
    s32 i;

    for (i = 0; i < 200; i++) {
        if ((u8)BattleAudience_GetExist(i)) {
            BattleAudienceMember* member = BattleAudienceGetPtr(i);

            member->field_0xE8 = float_90_80421624;
            if (!bakuGameAudienceCanThrowPos(i)) {
                member->flags &= ~0x10;
            }
        }
    }
}

void bakuGameEnemySurpriseReset(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL && unit->mAlliance == 1 && !(unit->mAttributeFlags & 0x20000) &&
            !BtlUnit_CheckStatus(unit, 0x1B)) {
            btlDispPoseAnime(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)));
        }
    }
}

// Keep bakuGameMarioSurpriseReset out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameMarioSurpriseReset(BattleUnit* unit, BattleUnit* party) {
    btlDispPoseAnime(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)));
    if (bakuGamePartyExist(party)) {
        btlDispPoseAnime(BtlUnit_GetPartsPtr(party, BtlUnit_GetBodyPartsId(party)));
    }
}
#pragma dont_inline reset

// Keep bakuGameAudienceCanThrowPos out of line: callers emit bl instead of inlining the loop.
#pragma dont_inline on
s32 bakuGameAudienceCanThrowPos(s32 id) {
    s32 i;
    s32 ofs = 0;

    for (i = 0; i < 3; i++) {
        if (id >= ofs + 8 && id <= ofs + 0xB) {
            return 1;
        }
        ofs += 0x14;
    }
    return 0;
}
#pragma dont_inline reset

// Keep bakuGamePartyExist out of line: callers emit bl instead of inlining the status checks.
#pragma dont_inline on
BOOL bakuGamePartyExist(BattleUnit* unit) {
    if (unit == NULL) {
        return 0;
    }
    if (BtlUnit_CheckStatus(unit, 0x1B)) {
        return 0;
    }
    return BtlUnit_CanActStatus(unit) != 0;
}
#pragma dont_inline reset

// NONMATCHING (97.82%): local initializer blobs use anonymous symbols; loop index registers r3/r4 are swapped.
void bakuGameDecideButton(s32 arg) {
    BakuGameWork* baku = GetBakuGamePtr();
    Vec positions[3] = {
        { -125.0f, 50.0f, 0.0f },
        { 125.0f, 50.0f, 0.0f },
        { 0.0f, 10.0f, 150.0f },
    };
    s32 perms[6][3] = {
        { 0, 1, 2 },
        { 0, 2, 1 },
        { 1, 0, 2 },
        { 1, 2, 0 },
        { 2, 0, 1 },
        { 2, 1, 0 },
    };
    s32 rnd = irand(6);
    s32 i;

    for (i = 0; i < 3; i++) {
        baku->entries[i].index = perms[rnd][i];
        baku->entries[i].pos = positions[perms[rnd][i]];
    }
    if (arg == 0) {
        baku->field_0xF4 = 0x3C;
    }
}

// NONMATCHING (86.90%): case-5 Vec copy order and case-0 FPR allocation differ.
s32 bakuGameHeihoReturn(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();

    if (!(baku->flags & 1)) {
        return 2;
    }
    if (isFirstCall) {
        baku->field_0x1C0 = 0;
    }
    switch (baku->field_0x1C0) {
        case 0:
            if (float_0_80421618 != baku->field_0x1D0.y) {
                baku->field_0x1D0.y += baku->field_0x1F8;
                baku->field_0x1F8 += float_neg2_80421634;
            }
            if (baku->field_0x1D0.y <= float_0_80421618) {
                baku->field_0x1D0.y = float_0_80421618;
                baku->field_0x1C0 = 5;
            }
            break;
        case 5: {
            Vec v;

            baku->field_0x1C0 = 10;
            baku->field_0x1CC = 1;
            baku->field_0x1DC = baku->field_0x1D0;
            v = vec3_802febf0;
            v.z = baku->field_0x1D0.z;
            baku->field_0x1E8 = v;
            baku->field_0x1C4 = 0;
            baku->field_0x1C8 = 0x1E;
            break;
        }
        case 10: {
            s32 m;

            baku->field_0x1D0.x = intplGetValue(0, baku->field_0x1C4, baku->field_0x1C8, baku->field_0x1DC.x,
                                                baku->field_0x1E8.x);
            m = baku->field_0x1C4 % 12;
            if (m >= 0 && m <= 3) {
                baku->field_0x200 = 0x5A;
            }
            if (m >= 4 && m <= 7) {
                baku->field_0x200 = 0x5B;
            }
            if (m >= 8 && m <= 0xB) {
                baku->field_0x200 = 0x5C;
            }
            baku->field_0x1C4++;
            if (baku->field_0x1C4 > baku->field_0x1C8) {
                return 2;
            }
            break;
        }
    }
    dispEntry(4, float_0_80421618, 1, (DispCallback)bakuGameDisp3D, NULL);
    return 0;
}

static BattleWeapon weapon;

s32 bakuGameBombEntry(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();

    if (animGroupBaseAsync(str_802FEDB8, 2, 0) == 0) {
        return 0;
    }
    baku->field_0xEC = animPoseEntry(str_802FEDB8, 2);
    animPoseSetAnim(baku->field_0xEC, str_804216C4, 1);
    return 2;
}

// NONMATCHING (93.00%): MWCC folds baku+0x178 into stb offsets instead of materializing the base pointer in a register.
s32 bakuGameDecideWeapon(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 v;
    BakuGameWork* baku;
    s8* p;

    evtGetFloat(event, args[0]);
    v = evtGetValue(event, args[1]);
    baku = GetBakuGamePtr();
    p = baku->field_0x178;
    memcpy(&baku->field_0xF8, &weapon, 0xC0);
    p[2] = 100;
    if (v != -1) {
        if (baku->field_0xD0 >= float_100_80421620) {
            p[2] = 0x7F;
        } else {
            p[2] = 100;
        }
        p[3] = v - 1;
    } else {
        p[2] = 0;
        p[3] = 0;
    }
    evtSetValue(event, args[2], (s32)((u8*)baku + 0xF8));
    return EVT_RETURN_DONE;
}

s32 end_bakugame(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();
    void* p = baku->starStone;
    void* q = *(void**)((u8*)p + 0x0C);
    effDelete(*(EffectEntry**)((u8*)q + 0x34));
    effDelete((EffectEntry*)baku->starStone);
    return EVT_RETURN_DONE;
}

s32 init_bakugame(EventEntry* event, BOOL isFirstCall) {
    memset(GetBakuGamePtr(), 0, 0x2B8);
    return EVT_RETURN_DONE;
}

// NONMATCHING (75.09%): GX/float register scheduling and several sdata2 constant relocations differ.
void main_star(void) {
    BakuGameWork* baku = GetBakuGamePtr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);

    switch (baku->state) {
        case 0:
        case 4:
            break;
        case 1:
            baku->state = 2;
            baku->frame = 0;
            baku->starStone = effStarStoneEntry(1, float_0_80421618, float_neg1000_8042167c, float_0_80421618,
                                                float_1_80421608);
            BtlUnit_GetPos(mario, &baku->field_0x20C.x, &baku->field_0x20C.y, &baku->field_0x20C.z);
            baku->field_0x20C.y += mario->mSizeMultiplier * (f32)mario->mHeight + float_37_804216c8;
            baku->field_0x218 = baku->field_0x20C;
            baku->field_0x224 = baku->field_0x20C;
            baku->field_0x224.y += float_50_80421678;
            baku->field_0x224.z += float_neg1_80421604;
            baku->field_0x23C = vec3_802feac8;
            baku->field_0x248 = vec3_802fead4;
            break;
        case 2:
            baku->frame++;
            if (baku->frame <= 100) {
                baku->field_0x20C.x = intplGetValue(0, baku->frame, 100, baku->field_0x218.x, baku->field_0x224.x);
                baku->field_0x20C.y = intplGetValue(0, baku->frame, 100, baku->field_0x218.y, baku->field_0x224.y);
                baku->field_0x20C.z = intplGetValue(0, baku->frame, 100, baku->field_0x218.z, baku->field_0x224.z);
                baku->field_0x23C.z = intplGetValue(0, baku->frame, 100, float_0_80421618, float_2_8042165c);
                baku->field_0x23C.y = baku->field_0x23C.z;
                baku->field_0x23C.x = baku->field_0x23C.z;
            } else {
                baku->field_0x20C = baku->field_0x224;
                baku->field_0x23C.z = float_2_8042165c;
                baku->field_0x23C.y = baku->field_0x23C.z;
                baku->field_0x23C.x = baku->field_0x23C.z;
            }
            baku->field_0x248.y = intplGetValue(4, baku->frame, 0x78, float_0_80421618, float_2160_804216cc);
            if (baku->frame >= 0x78) {
                baku->state = 3;
                baku->frame = 0;
                baku->field_0x218 = baku->field_0x20C;
                baku->field_0x218.y = float_300_80421644;
            }
            break;
        case 3:
            baku->frame++;
            baku->field_0x20C.y = intplGetValue(1, baku->frame, 0x3C, baku->field_0x218.y, baku->field_0x224.y);
            if (baku->frame >= 0x3C) {
                baku->state = 4;
                baku->frame = 0;
            }
            break;
    }
    if (baku->starStone != NULL) {
        f32* e = *(f32**)((u8*)baku->starStone + 0x0C);

        e[2] = baku->field_0x20C.x;
        e[3] = baku->field_0x20C.y;
        e[4] = baku->field_0x20C.z;
        e[6] = baku->field_0x248.x;
        e[7] = baku->field_0x248.y;
        e[8] = baku->field_0x248.z;
        e[5] = float_1p5_80421648 * baku->field_0x23C.x;
    }
    dispEntry(4, float_0_80421618, 2, (DispCallback)bakuGameDispStar, NULL);
}

static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();

    baku->state = 1;
    return EVT_RETURN_DONE;
}

// Keep GetBakuGamePtr out of line: every caller emits bl GetBakuGamePtr as in the original.
#pragma dont_inline on
BakuGameWork* GetBakuGamePtr(void) {
    return (BakuGameWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

// Original .sdata2 scalar definitions for this translation unit (declared extern at the top).
const f32 float_neg1_80421604 = -1.0f;
const f32 float_1_80421608 = 1.0f;
const f32 float_180_8042160c = 180.0f;
#ifdef MUST_MATCH
volatile const f32 float_neg20_80421610 = -20.0f;
volatile const f32 float_40_80421614 = 40.0f;
volatile const f32 float_0_80421618 = 0.0f;
volatile const f32 float_20_8042161c = 20.0f;
#else
const f32 float_neg20_80421610 = -20.0f;
const f32 float_40_80421614 = 40.0f;
const f32 float_0_80421618 = 0.0f;
const f32 float_20_8042161c = 20.0f;
#endif
const f32 float_100_80421620 = 100.0f;
const f32 float_90_80421624 = 90.0f;
const f32 float_270_80421628 = 270.0f;
const f32 float_neg2_80421634 = -2.0f;
const f32 float_5_80421638 = 5.0f;
const f32 float_33_8042163c = 33.0f;
const f32 float_66_80421640 = 66.0f;
const f32 float_300_80421644 = 300.0f;
const f32 float_1p5_80421648 = 1.5f;
const f32 float_neg0p3_8042164c = -0.3f;
const f32 float_110_80421650 = 110.0f;
const f32 float_4096_80421654 = 4096.0f;
const f32 float_0p25_80421658 = 0.25f;
const f32 float_2_8042165c = 2.0f;
const f32 float_3_80421660 = 3.0f;
const f32 float_4_80421664 = 4.0f;
const f32 float_6_80421668 = 6.0f;
const f32 float_50_80421678 = 50.0f;
const f32 float_neg1000_8042167c = -1000.0f;
const f32 float_neg800_80421680 = -800.0f;
const f32 float_80_80421684 = 80.0f;
const f32 float_neg0p5_80421688 = -0.5f;
const f32 float_70_8042168c = 70.0f;
const f32 float_neg200_80421690 = -200.0f;
const f32 float_0p1_80421694 = 0.1f;
const f32 float_7_80421698 = 7.0f;
const f32 float_30_804216ac = 30.0f;
const f32 float_neg100_804216b0 = -100.0f;
const f32 float_26_804216b4 = 26.0f;
const f32 float_900_804216c0 = 900.0f;
const f32 float_37_804216c8 = 37.0f;
const f32 float_2160_804216cc = 2160.0f;
