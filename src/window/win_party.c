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
#include "manager/filemgr.h"
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
extern BattleWeapon partyWeapon_ChuchurinaNormalAttackLeft;
extern BattleWeapon partyWeapon_NokotarouKouraAttack;
extern BattleWeapon partyWeapon_NokotarouSyubibinKoura;
extern BattleWeapon partyWeapon_NokotarouKouraGuard;
extern BattleWeapon partyWeapon_NokotarouTsuranukiKoura;
extern BattleWeapon partyWeapon_VivianNormalAttack;
extern BattleWeapon partyWeapon_VivianShadowGuard;
extern BattleWeapon partyWeapon_VivianMagicalPowder;
extern BattleWeapon partyWeapon_VivianCharmKissAttack;
extern BattleWeapon partyWeapon_YoshiNormalAttack;
extern BattleWeapon partyWeapon_YoshiNomikomi_Shot;
extern BattleWeapon partyWeapon_YoshiEggAttack_Minimini;
extern BattleWeapon partyWeapon_YoshiCallGuard;


// ---- data ----
// .sdata: each partner's four moves; .data: the partner table. Defined before the code so their strings
// pool first, as in the original.
BattleWeapon* chri_waza[4] = { &partyWeapon_ChristineNormalAttack, &partyWeapon_ChristineMonosiri, &partyWeapon_ChristineRenzokuAttack, &partyWeapon_ChristineKiss };
BattleWeapon* noko_waza[4] = { &partyWeapon_NokotarouKouraAttack, &partyWeapon_NokotarouSyubibinKoura, &partyWeapon_NokotarouKouraGuard, &partyWeapon_NokotarouTsuranukiKoura };
BattleWeapon* cloud_waza[4] = { &partyWeapon_ClaudaNormalAttack, &partyWeapon_ClaudaBreathAttack, &partyWeapon_ClaudaPredationAttack, &partyWeapon_ClaudaKumogakureAttack };
BattleWeapon* yoshi_waza[4] = { &partyWeapon_YoshiNormalAttack, &partyWeapon_YoshiNomikomi_Shot, &partyWeapon_YoshiEggAttack_Minimini, &partyWeapon_YoshiCallGuard };
BattleWeapon* vivi_waza[4] = { &partyWeapon_VivianNormalAttack, &partyWeapon_VivianShadowGuard, &partyWeapon_VivianMagicalPowder, &partyWeapon_VivianCharmKissAttack };
BattleWeapon* sand_waza[4] = { &partyWeapon_SandersNormalAttack, &partyWeapon_SandersTimeBombSet, &partyWeapon_SandersCounterSet, &partyWeapon_SandersSuperBombAttack };
BattleWeapon* chu_waza[4] = { &partyWeapon_ChuchurinaNormalAttackLeft, &partyWeapon_ChuchurinaItemSteal, &partyWeapon_ChuchurinaMadowaseAttack, &partyWeapon_ChuchurinaKiss };

WindowPartyDt winPartyDt[7] = {
    { 1, 0x155, "name_party0", "msg_menu_party_kuri", "EFF_m_kuriko", "W_1", "d_mario", "R_2", chri_waza },
    { 2, 0x156, "name_party1", "msg_menu_party_noko", "EFF_m_noko", "W_1", "d_mario", "R_3", noko_waza },
    { 5, 0x15F, "name_party4", "msg_menu_party_cloud", "EFF_m_cloud", "W_1", "d_mario", "R_4", cloud_waza },
    { 4, 0x158, "name_party3", "msg_menu_party_yoshi", "EFF_m_yoshi", "W_1", "d_mario", "R_5", yoshi_waza },
    { 6, 0x160, "name_party5", "msg_menu_party_vivi", "c_vivian", "PTR_S_1", "d_mario", "R_12", vivi_waza },
    { 3, 0x157, "name_party2", "msg_menu_party_bom", "EFF_m_bomb", "W_1", "d_mario", "R_13", sand_waza },
    { 7, 0x161, "name_party6", "msg_menu_party_chu", "EFF_m_chu", "W_1", "d_mario", "R_14", chu_waza },
};

void winPartyInit(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
    s32 i;
    s32 j;
    s32 k;
    s32 pose;
    const char* model;
    const char* anim;

    work->partyCount = 0;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 8; j++) {
            if (j != winPartyDt[i].partnerId)
                continue;
            if (!(pouch->partyData[j].flags & 1))
                continue;
            if (pouch->partyData[j].flags & 2)
                continue;

            if (work->flags & 0x2000) {
                model = winPartyDt[i].famicomModel;
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
                model = winPartyDt[i].model;
                anim = winPartyDt[i].anim;
            }

            pose = animPoseEntry(model, 0);
            work->partyPose[work->partyCount] = pose;
            animPoseSetAnim(work->partyPose[work->partyCount], anim, 1);
            animPoseSetMaterialFlagOn(work->partyPose[work->partyCount], 0x1800);
            work->partyId[work->partyCount] = j;
            work->partyCount++;
        }
    }

    i = 0;
    work->partySel = i;
    for (; i < work->partyCount; i++) {
        s32 party;

        party = !marioBgmodeChk() ? marioGetParty() : marioGetPtr()->prevPartyMemberId[0];
        if (party == work->partyId[i]) {
            work->partySel = i;
            break;
        }
    }

    for (k = 0; k < 8; k++) {
        if (work->partyId[work->partySel] == winPartyDt[k].partnerId) {
            work->selPartyDt = k;
            break;
        }
    }

    work->lastPartySel = work->partySel;
    {
        f32 time = 360.0f / (f32)work->partyCount * (f32)work->partySel;

        work->partyAngleTarget = time;
        work->partyAngle = time;
    }
    work->partyState = 0;
    work->wazaCount = pouchGetPartyAttackLv(winPartyDt[work->selPartyDt].partnerId) + 2;

    switch (pouchGetPartyColor(PARTNER_YOSHI)) {
    case 0: winPartyDt[3].iconId = 0x158; break;
    case 1: winPartyDt[3].iconId = 0x159; break;
    case 2: winPartyDt[3].iconId = 0x15A; break;
    case 3: winPartyDt[3].iconId = 0x15B; break;
    case 4: winPartyDt[3].iconId = 0x15C; break;
    case 5: winPartyDt[3].iconId = 0x15D; break;
    case 6:
    default: winPartyDt[3].iconId = 0x15E; break;
    }
}

void winPartyInit2(WindowWork* work) {
    f32 time = 360.0f / (f32)work->partyCount * (f32)work->partySel;

    work->partyAngleTarget = time;
    work->partyAngle = time;
    work->partyState = 0;
    work->targetX = -200.0f;
    work->targetY = 20.0f;
    winMsgEntry(work, 0, winPartyDt[work->selPartyDt].descMsg, NULL);
}

void winPartyExit(WindowWork* work) {
    s32 i;

    for (i = 0; i < work->partyCount; i++) {
        if (work->partyPose[i] != -1) {
            animPoseRelease(work->partyPose[i]);
        }
    }
}

s32 winPartyMain(WindowWork* work) {
    s32 k;

    if (work->partyCount == 0) {
        return -1;
    }

    switch (work->partyState) {
    case 0: {
        u32 button = work->buttonTrg;
        u32 dir;

        if (button & 0x100) {
            work->partyChange = 1;
            return -2;
        }
        if (button & 0x400) {
            work->partyState = 1;
            work->wazaSel = 0;
            psndSFXOn((const char*)0x20012);
        } else {
            if (button & 0x200) {
                psndSFXOn((const char*)0x20013);
                work->partyChange = 0;
                return -1;
            }
            if (button & 0x1000) {
                work->partyChange = 0;
                return -2;
            }

            dir = work->dirRep;
            if (dir & 0x4000) {
                s32 prev = work->partySel;

                work->partySel--;
                while (work->partySel < 0) {
                    work->partySel += work->partyCount;
                }
                work->partyAngleTarget = 360.0f / (f32)work->partyCount * (f32)work->partySel;
                while (work->partyAngle < work->partyAngleTarget) {
                    work->partyAngle += 360.0f;
                }
                if (prev != work->partySel) {
                    psndSFXOn((const char*)0x209D6);
                }
                for (k = 0; k < 8; k++) {
                    if (work->partyId[work->partySel] == winPartyDt[k].partnerId) {
                        work->selPartyDt = k;
                        break;
                    }
                }
            } else if (dir & 0x8000) {
                s32 prev = work->partySel;

                work->partySel++;
                while (work->partySel > work->partyCount - 1) {
                    work->partySel -= work->partyCount;
                }
                work->partyAngleTarget = 360.0f / (f32)work->partyCount * (f32)work->partySel;
                while (work->partyAngle > work->partyAngleTarget) {
                    work->partyAngle -= 360.0f;
                }
                if (prev != work->partySel) {
                    psndSFXOn((const char*)0x209D6);
                }
                for (k = 0; k < 8; k++) {
                    if (work->partyId[work->partySel] == winPartyDt[k].partnerId) {
                        work->selPartyDt = k;
                        break;
                    }
                }
            }
        }
        work->targetX = -200.0f;
        work->targetY = 20.0f;
        work->wazaCount = pouchGetPartyAttackLv(winPartyDt[work->selPartyDt].partnerId) + 2;
        winMsgEntry(work, 0, winPartyDt[work->selPartyDt].descMsg, NULL);
        break;
    }
    case 1: {
        u32 dir = work->dirRep;

        if (dir & 0x1000) {
            work->wazaSel--;
            if (work->wazaSel < 0) {
                work->wazaSel = work->wazaCount - 1;
            }
            psndSFXOn((const char*)0x20005);
        } else if (dir & 0x2000) {
            work->wazaSel++;
            if (work->wazaSel >= work->wazaCount) {
                work->wazaSel = 0;
            }
            psndSFXOn((const char*)0x20005);
        } else {
            u32 button = work->buttonTrg;

            if (button & 0x200) {
                work->partyState = 0;
                psndSFXOn((const char*)0x20013);
            } else if (button & 0x1000) {
                work->partyChange = 0;
                return -2;
            }
        }

        work->targetX = -10.0f;
        work->targetY = -26.0f * (f32)work->wazaSel + 5.0f;
        winMsgEntry(work, 0, winPartyDt[work->selPartyDt].waza[work->wazaSel]->description, NULL);
        break;
    }
    default:
        break;
    }

    return 0;
}

void winPartyMain2(WindowWork* work) {
    f32 factor = 0.25f;
    f32 diff = work->partyAngleTarget - work->partyAngle;

    work->partyAngle += diff * factor;
}



#define WIN_PARTY_TEX(w) (*(w)->winTpl->data)
// the party member shown in pose slot n, and its animation pose
#define WIN_PARTY_ID(w, n) ((w)->partyId[(n)])
#define WIN_PARTY_POSE(w, n) ((w)->partyPose[(n)])

void winPartyDisp(CameraId camId, void* param, s32 partyIdx) {
    WindowWork* work = param;
    GXColor loopWhite, loopBlack, white, black;
    f32 barX;
    f32 cx;
    f32 cy;
    f32 rowY;
    f32 y2;
    f32 costX;
    f32 top;
    f32 x2;
    f32 fpX;
    f32 listTop;
    f32 x;
    f32 y;
    f32 w;
    f32 w2;
    f32 iconY;
    f32 scale;
    s32 i;
    s32 n;
    PouchPartyData* party;

    white = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
    loopWhite = white;
    black = (GXColor){0x00, 0x00, 0x00, 0xFF};
    loopBlack = black;
    x = work->recs[partyIdx].x;
    y = work->recs[partyIdx].y;

    winBgGX(work, 1, x, y);
    if (work->partyCount == 0) {
        return;
    }
    party = &pouchGetPtr()->partyData[winPartyDt[work->selPartyDt].partnerId];

    // HP and rank bars
    winKirinukiGX(work, 0, x - 10.0f, 20.0f + (116.0f + y), 240.0f, 76.0f);
    cx = 45.0f + (x - 10.0f);
    barX = cx - 2.0f;
    top = 110.0f + y;
    for (i = 0; i < 2; i++) {
        GXColor barColors[2] = {{0xCB, 0x8C, 0xA2, 0xFE}, {0x9C, 0xC3, 0x81, 0xFE}};
        GXColor subColors[1] = {{0xA7, 0x3C, 0x3C, 0xFF}};
        const char* labels[2] = {"msg_menu_party_hp", "msg_menu_party_rank"};
        const char* subLabels[2] = {"msg_menu_mario_ruby_hp", "msg_menu_mario_ruby_hp"};

        scale = i == 0 ? 1.0f : 0.9f;
        w = scale * FontGetMessageWidth(msgSearch(labels[i]));
        w2 = 0.5f * FontGetMessageWidth(msgSearch(subLabels[i]));
        winTexInit(WIN_PARTY_TEX(work));
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        winTexSet(0xAF, (Vec){barX, (rowY = top - (f32)(i * 32)) - 4.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, barColors[i]);
        winFontInit();
        winFontSet((Vec){cx - w / 2.0f, 12.0f + rowY, 0.0f}, (Vec){scale, scale, scale}, loopBlack,
                   msgSearch(labels[i]));
        if (i < 1) {
            winFontSet((Vec){cx - w2 / 2.0f, 21.0f + rowY, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, subColors[i],
                       msgSearch(subLabels[i]));
        }
    }

    // rank stars: the rank's icon, then one per rank above 2
    winIconInit();
    {
        f32 starX;

        starX = 60.0f + cx;
        iconY = 4.0f + top;
        winIconSet(0x19F, (Vec){starX, iconY, 0.0f}, (Vec){0.82f, 0.82f, 0.82f}, white);
        cy = 4.0f + (top - 32.0f);
        for (i = 0; i < work->wazaCount - 2; i++) {
            winIconSet(itemDataTable[ITEM_SHINE_SPRITE].icon_id,
                       (Vec){starX + (f32)(i * 34), cy, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, loopWhite);
        }
    }

    // HP: current / max
    winTexInit(WIN_PARTY_TEX(work));
    winTexSet(0x10, (Vec){8.0f + (170.0f + (x - 10.0f)), iconY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
    winFontInit();
    winFontSetR((Vec){10.0f + (110.0f + (x - 10.0f)), y2 = 4.0f + (12.0f + top), 0.0f}, (Vec){0.9f, 0.9f, 0.9f},
                black, "%s", winZenkakuStr(party->currentHP));
    winFontSet((Vec){8.0f + (180.0f + (x - 10.0f)), y2, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s",
               winZenkakuStr(party->maximumHP));
    winZClear();

    // the party on its turntable: shadows first, then the models
    for (i = 0; i < work->partyCount; i++) {
        Vec pos = {0.0f, 0.0f, 80.0f};
        Mtx tilt, turn;

        PSMTXRotRad(tilt, 'x', 0.5235988f);
        PSMTXRotRad(turn, 'y',
                    0.017453292f * (360.0f / (f32)work->partyCount * (f32)i - work->partyAngle));
        PSMTXMultVec(turn, &pos, &pos);
        PSMTXMultVec(tilt, &pos, &pos);
        winKageGX(work, (x - 140.0f) + pos.x, 20.0f + (y + pos.y), pos.z, i == work->partySel ? 0.8f : 0.6f,
                  &(GXColor){0x00, 0x00, 0x00, 0x80});
    }

    for (i = 0; i < work->partyCount; i++) {
        Vec pos = {0.0f, 0.0f, 80.0f};
        Mtx mtx, scaleMtx, tilt, turn;
        s32 pose = WIN_PARTY_POSE(work, i);
        AnimData* anim;
        const char* animName;

        if (pose == -1) {
            continue;
        }
        anim = animPoseGetAnimDataPtr(pose);
        PSMTXRotRad(tilt, 'x', 0.5235988f);
        PSMTXRotRad(turn, 'y',
                    0.017453292f * (360.0f / (f32)work->partyCount * (f32)i - work->partyAngle));
        PSMTXMultVec(turn, &pos, &pos);
        PSMTXMultVec(tilt, &pos, &pos);

        if (i == work->partySel) {
            // the selected member: full size, lit, animating
            PSMTXTrans(mtx, (x - 140.0f) + pos.x, 20.0f + (y + pos.y), pos.z);
            PSMTXScale(scaleMtx, 1.0f, 1.0f, -1.0f);
            PSMTXRotRad(turn, 'y', 3.1415927f);
            PSMTXConcat(mtx, scaleMtx, mtx);
            PSMTXConcat(mtx, turn, mtx);
            animPoseSetMaterialFlagOff(pose, 0x40);
            for (n = 0; n < 7; n++) {
                if (WIN_PARTY_ID(work, i) == winPartyDt[n].partnerId) {
                    break;
                }
            }
            if (work->flags & 0x2000) {
                animName = winPartyDt[n].famicomAnim;
                if (winPartyDt[n].partnerId == 4) {
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
                animName = winPartyDt[n].anim;
            }
            animPoseSetAnim(pose, animName, 0);
            animPoseSetLocalTime(pose,
                                 gp->retraceCount % (s32)anim->clip->end);
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
                if (WIN_PARTY_ID(work, i) == winPartyDt[n].partnerId) {
                    break;
                }
            }
            if (work->flags & 0x2000) {
                animName = winPartyDt[n].famicomAnim;
                if (winPartyDt[n].partnerId == 4) {
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
                animName = winPartyDt[n].anim;
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
    if (work->partyCount != 0) {
        winNameGX(work, 0, x - 230.0f, (20.0f + (y - 50.0f)) - 10.0f, 200.0f, 32.0f);
        winFontInit();
        y2 = (20.0f + ((y - 50.0f) - 4.0f)) - 10.0f;
        for (n = 0; n < 7; n++) {
            if (WIN_PARTY_ID(work, work->partySel) == winPartyDt[n].partnerId) {
                const char* name;

                if (4 != winPartyDt[n].partnerId) {
                    name = msgSearch(winPartyDt[n].nameMsg);
                } else {
                    name = pouchGetYoshiName();
                }
                w = FontGetMessageWidth(name);
                winFontSetEdge((Vec){(200.0f - w) / 2.0f + (x - 230.0f), y2, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                               loopWhite, name);
            }
        }
    }

    // "change partner" prompt and the turntable arrows
    if (work->partyCount > 1) {
        winFontInit();
        w = 0.8f * FontGetMessageWidth(msgSearch("msg_menu_party_henkou"));
        x2 = (-230.0f - w) / 2.0f + x;
        winFontSet((Vec){x2, y - 90.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, black, msgSearch("msg_menu_party_henkou"));
        winIconInit();
        winIconSet(0x64, (Vec){x2 - 30.0f, y - 100.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white);
        winTexInit(WIN_PARTY_TEX(work));
        winTexSet(0xC0, (Vec){(x - 150.0f) - 50.0f, y - 16.0f, 0.0f}, (Vec){-0.6f, 0.6f, 0.6f}, white);
        winTexSet(0xC0, (Vec){50.0f + (x - 150.0f), y - 16.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }

    // "check moves" prompt, then the move list with each move's FP cost
    winFontInit();
    w = 0.7f * FontGetMessageWidth(msgSearch("msg_menu_party_waza_kakunin"));
    x2 = x + (250.0f - w);
    winFontSet((Vec){x2, y - 100.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, black,
               msgSearch("msg_menu_party_waza_kakunin"));
    winIconInit();
    winIconSet(0x1F5, (Vec){x2 - 20.0f, y - 110.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, white);
    winWazaGX(work, 0, x - 5.0f, listTop = 50.0f + y, 260.0f, 144.0f);
    winFontInit();
    w = 0.8f * FontGetMessageWidth(msgSearch("msg_menu_party_waza"));
    y2 = 45.0f + y;
    winFontSetEdge((Vec){(200.0f - w) / 2.0f + x, y2, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white,
                   msgSearch("msg_menu_party_waza"));
    x2 = 230.0f + x;
    winFontSetEdge((Vec){x2 - 20.0f, y2, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, "\202e\202o");
    {
        f32 listY;

        y = 10.0f + x;
        costX = (238.0f + x) - 20.0f;
        fpX = 2.0f + (2.0f + ((17.0f + x2) - 20.0f));

        for (i = 0; i < work->wazaCount; i++) {
            BattleWeapon* weapon;
            char buf[64];

            winFontSetLabel((Vec){y, listY = (listTop - (f32)(i * 26)) - 30.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                            loopBlack, winPartyDt[work->selPartyDt].waza[i]->name);
            weapon = winPartyDt[work->selPartyDt].waza[i];
            sprintf(buf, "%s", winZenkakuStr(weapon->baseFpCost));
            winFontSet((Vec){(costX - (f32)(FontGetMessageWidth(buf) >> 1)) - 2.0f, listY, 0.0f},
                       (Vec){1.0f, 1.0f, 1.0f}, loopBlack, buf);
            winFontSet((Vec){fpX, listY - 10.0f, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, loopBlack, "\202e\202o");
        }
    }
}
