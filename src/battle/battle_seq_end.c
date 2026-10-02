#include "battle/battle_seq_end.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "battle/battle_audience.h"
#include "battle/battle_stage.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
#include "battle/battle_sub.h"
#include "battle/battle_actrecord.h"
#include "battle/battle_break_slot.h"
#include "battle/battle_seq.h"
#include "driver/mapdrv.h"
#include "driver/fadedrv.h"
#include "driver/seqdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_recovery.h"
#include "eff/eff_pointget.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"
#include "pmario_sound.h"
#include "eff/n64/eff_pokopi_pcharge_n64.h"
#include <stdio.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803ACB10[2] = { 0xBF800000, 0x3F800000 };
static s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall);  // local to this file in the original

// Prototypes for functions that are not declared in a shared header.


extern s32 btlevtcmd_RunDataEventChild[];

// .sdata2 constants this file loads by name.
extern const f32 float_3_80420c6c;
extern const f32 float_4_80420c78;
extern const f32 float_0_80420c7c;
extern const f32 float_255_80420c88;
extern const f32 float_0p5_80420c8c;
extern const f32 float_neg20_80420c90;
extern const f32 float_50_80420c94;
extern const f32 float_1p1_80420c98;
extern const f32 float_25_80420c9c;
extern const f32 float_90_80420ca0;
extern const f32 float_2p5_80420ca4;
extern const f32 float_65_80420ca8;
extern const f32 float_180_80420cac;
extern const f32 float_7p5_80420cb0;
extern const f32 float_270_80420cb4;
extern const f32 float_4p5_80420cb8;
extern const f32 float_neg25_80420cbc;
extern const f32 float_128_80420cc0;
// sdata2 constants used by the display helpers below.
extern const f32 float_20_80420c48;
extern const f32 float_neg145_80420cf8;
extern const f32 float_10_80420cd8;
extern const f32 float_neg148_80420cfc;
extern const GXColor dat_80420c14;
// sdata2/rodata constants used by _lvup_select_object_disp.
extern const f32 float_deg2rad_80420c40;
extern const f32 float_16_80420c44;
extern const f32 float_2_80420c4c;
extern const f32 float_8_80420c50;
extern const f32 float_6_80420c54;
extern const f32 float_96_80420c58;
extern const f32 float_5_80420c5c;
extern const f32 float_0p8_80420c60;
extern const f32 float_1_80420c64;
extern const f32 float_1p2_80420c68;
extern const f32 float_neg0p6_80420c70;
extern const f32 float_0p6_80420c74;
extern const f64 double_to_int_802fe050;

//.data
RankUpData _rank_up_data[] = {
    {1, 0, "msg_menu_mario_name_0", "butai_01"},
    {10, 1, "msg_menu_mario_name_1", "butai_01"},
    {20, 2, "msg_menu_mario_name_2", "butai_02"},
    {30, 3, "msg_menu_mario_name_3", "butai_03"},
    {0xFFFF, 0, NULL, NULL},
};

s32 _koura_delete_event[] = {
    0x0003005B, (s32)_check_guard_koura, 0xFFFFFFFD, 0xFE363C8A,
    0x00020019, 0xFE363C8A, 0xFFFFFFFF, 0x0003005B,
    (s32)btlevtcmd_RunDataEventChild, 0xFE363C8A, 0x0000003E,
    0x00000021, 0x00000002, 0x00000001,
};

#pragma dont_inline on
RankUpData* _get_rank_data(s32 level) {
    RankUpData* rank;

    for (rank = _rank_up_data; level != rank->startLevel; rank++) {
        if (!rank->rankUpMessage) {
            return NULL;
        }
    }
    return rank;
}
#pragma dont_inline reset

const char* BattleGetRankNameLabel(s32 level) {
    RankUpData* rank = _rank_up_data;
    RankUpData* found = NULL;

    while (level >= rank->startLevel) {
        if (rank->startLevel < 0) {
            break;
        }
        found = rank;
        rank++;
    }
    if (found == NULL) {
        return NULL;
    }
    return found->rankUpName;
}

// ---- btlseqEnd ------------------------------------------------------------
// Data tables in this unit's .data, reached as offsets from its first object,
// and the string bank in .rodata reached from its first string.
extern const char str_802FDDF0[];
extern const GXColor dat_80420c00;
extern const GXColor dat_80420c04;
extern s32 _event_MajinaiEnd[];
extern s32 _rank_up_event[];
extern s32 _curtain_close_event[];
extern s32 _move_to_heart_event[];
extern s32 _move_to_flower_event[];
extern s32 _move_to_badge_point_event[];
extern s32 _jump_event[];
extern s32 marioAttackEvent_MajinaiExpUp[];
extern BattleDispPose pose_table_nokotarou_stay[];

// This unit's original build inlines nothing: every callee is a real `bl` (cf. battle.c's `-inline off`).
// Until configure.py gets that flag for this file, keep btlseqEnd and the helpers it calls out of line.
// (Without this mwcc inlines _audience_Whistle_control, btlseqEnd_DispMain, ... into btlseqEnd.)
#pragma dont_inline on
// NONMATCHING (95.47%): logic matches; remaining diffs are register allocation (mwcc picks r31 for wp here
// instead of r26) and a few scheduling/switch-tree differences. See REPORT.md.
void btlseqEnd(BattleWork* wp) {
    BattleSeqEndWork* work = wp->seqEndWork;
    FieldBattleInfo* info = wp->fbatInfo;
#ifdef MUST_MATCH
    u8* data = (u8*)lbl_803ACB10;
#endif
    const char* str = str_802FDDF0;
    PouchData* pouch = pouchGetPtr();
    BattleSeqEndWork* newWork;
    BattleUnit* unit;
    BattleUnit* party;
    EventEntry* event;
    s32 flag;
    s8 i;

    _audience_Whistle_control();

    switch (BattleGetSeq(wp, SEQ_END)) {
    case 0x07000000:
        btlsubResetMoveColorLvAll(wp);
        unit = BattleGetMarioPtr(wp);
        {
            s32 marioId = unit->mAlliance;
            s32 belong = BtlUnit_GetEnemyBelong(unit);
            u32 flags = wp->flags;

            if (flags & 0x40) {
                BattleInformationSetResult(info, 4);
            } else if (flags & 0x20) {
                BattleInformationSetResult(info, 5);
            } else if ((s32)wp->alliances[marioId].clearConditionMet != 0) {
                BattleInformationSetResult(info, 3);
            } else if ((s32)wp->alliances[belong].clearConditionMet != 0) {
                BattleInformationSetResult(info, 1);
            } else if (flags & 0x10) {
                BattleInformationSetResult(info, 4);
            }
        }
        switch ((s32)BattleInformationGetResult(info)) {
        case 1:
            if (*(u32*)((u8*)fbatGetPointer() + 0x3C) != 0) {
                BattleSetSeq(wp, SEQ_END, 0x07000003);
                return;
            }
            _check_audience_battle_end_exec();
            BattleSetSeq(wp, SEQ_END, 0x07000005);
            _BattleMarioKouraDelete();
            break;
        case 3:
            BattleSetSeq(wp, SEQ_END, 0x07000001);
            break;
        default:
            _ExecAllUnitBattleEndEvent();
            BattleSetSeq(wp, SEQ_END, 0x0700001D);
            BattleAudienceSoundSetVolAll(0, 0x1E);
            break;
        }
        newWork = BattleAlloc(0x2AC);
        wp->seqEndWork = newWork;
        btlseqEnd_DispInit(wp);
        *(s32*)((u8*)newWork + 0x0) = 0;
        *(s32*)((u8*)newWork + 0x29C) = 0;
        *(s32*)((u8*)newWork + 0x2A0) = irand(0x78) + 0x3C;
        *(s32*)((u8*)newWork + 0x2A4) = 0;
        break;

    case 0x07000003:
        {
            FieldBattleData* fbat = fbatGetPointer();

            event = evtEntry(*(void**)((u8*)fbat + 0x3C), 0xA, 0);
            *(s32*)((u8*)fbat + 0x40) = event->eventId;
        }
        BattleSetSeq(wp, SEQ_END, 0x07000004);
        return;

    case 0x07000004:
        {
            FieldBattleData* fbat = fbatGetPointer();

            if (evtCheckID(*(s32*)((u8*)fbat + 0x40))) {
                break;
            }
            if (!BattleWaitAllActiveEvtEnd(wp)) {
                break;
            }
            if (*(s32*)((u8*)fbat + 0x44) != 0) {
                BattleSetSeq(wp, SEQ_INIT, 0x03000001);
                BattleSetSeq(wp, SEQ_UNKNOWN, 2);
                return;
            }
        }
        BattleSetSeq(wp, SEQ_END, 0x07000005);
        _BattleMarioKouraDelete();
        newWork = BattleAlloc(0x2AC);
        wp->seqEndWork = newWork;
        btlseqEnd_DispInit(wp);
        *(s32*)((u8*)newWork + 0x0) = 0;
        return;

    case 0x07000001:
        _ExecAllUnitBattleEndEvent();
        btl_camera_set_moveSpeedLv(0, 2);
        btl_camera_set_mode(0, 0x13);
        btl_camera_set_posoffset(0, 0.0f, 0.0f, 0.0f);
        fadeEntry(0xA, 0xC8, dat_80420c00);
        psndBGMOff(0x201);
        BattleSetSeq(wp, SEQ_END, 0x07000002);
        break;

    case 0x07000002:
        if (!fadeIsFinish()) {
            break;
        }
        if (!(wp->fbatInfo->setup->field_4 & 0x40)) {
            seqSetSeq(SEQ_MOVE, (char*)1, NULL);
        } else {
            BattleSetSeq(wp, SEQ_END, 0x0700001F);
        }
        break;

    case 0x07000005:
        unit = BattleGetMarioPtr(wp);
        party = BattleGetPartyPtr(wp);
        if (party != NULL && BtlUnit_CheckStatusFlag(party, 1)) {
            party->flippedTurns = 0;
            BtlUnit_OffStatusFlag(party, 1);
            *(void**)((u8*)BtlUnit_GetPartsPtr(party, BtlUnit_GetBodyPartsId(party)) + 0x1BC) =
                pose_table_nokotarou_stay;
        }
        BattleConsumeReserveItem();
        for (i = 0; i < 0x1C; i++) {
            if (i >= 0x1C || i < 0x19) {
                if (party != NULL) {
                    BtlUnit_SetStatus(party, i, 0, 0);
                }
                BtlUnit_SetStatus(unit, i, 0, 0);
            }
        }
        if (unit->mAttributeFlags & 0x10) {
            party = BattleGetPartyPtr(wp);
            if (party == NULL) {
                break;
            }
            event = evtEntry(BtlUnit_GetData(party, 3), 0xA, 0);
            event->unitId = party->unitId;
            party->attackEventId = event->eventId;
        } else {
            BattleSetSeq(wp, SEQ_END, 0x07000006);
        }
        /* fallthrough */

    case 0x07000006:
        if (!BattleWaitAllActiveEvtEnd(wp)) {
            break;
        }
        BattleAudienceSoundClap(-1, 0);
        _ExecAllUnitBattleEndEvent();
        BattleInformationSetDropMaterial(info);
        pouch = pouchGetPtr();
        unit = BattleGetMarioPtr(wp);
        if (pouch->mMerleeNextCurseType == 3) {
            wp->field_18FE1[0] = 3;
            BattleMajinaiDone();
            BattleMajinaiEndCheck();
            event = evtEntry(marioAttackEvent_MajinaiExpUp, 0xA, 0);
            event->unitId = unit->unitId;
            unit->attackEventId = event->eventId;
            BattleSetSeq(wp, SEQ_END, 0x07000007);
        } else {
            BattleSetSeq(wp, SEQ_END, 0x07000008);
        }
        break;

    case 0x07000007:
        if (BattleWaitAllActiveEvtEnd(wp)) {
            BattleSetSeq(wp, SEQ_END, 0x07000008);
        }
        break;

    case 0x07000008:
        BattleFogForceStop();
        *(s32*)((u8*)work + 0xC) = 0;
        *(s16*)((u8*)work + 0x8) = BattleGetStockExp(wp);
        if (wp->field_18FE1[0] == 3) {
            *(s16*)((u8*)work + 0x8) *= 2;
        }
        if (pouch->level >= 0x63) {
            *(s16*)((u8*)work + 0x8) = 0;
        }
        if (*(s16*)((u8*)work + 0x8) > 0x64) {
            *(s16*)((u8*)work + 0x8) = 0x64;
        }
        *(s16*)((u8*)work + 0xA) = *(s16*)((u8*)work + 0x8);
        wp->dispFlags &= ~2;
        if (*(s16*)((u8*)work + 0x8) > 0) {
            _GetExpIcon_Init(work);
        }
        *(s32*)((u8*)work + 0x4) = 0x78;
        btl_camera_set_moveSpeedLv(0, 2);
        if (!(wp->flags & 0x100)) {
            btl_camera_set_mode(0, 0x11);
        } else {
            btl_camera_set_mode(0, 0x12);
        }
        btl_camera_set_posoffset(0, 0.0f, 0.0f, 0.0f);
        BattleSetSeq(wp, SEQ_END, 0x07000009);
        BattleAudienceSoundCheer(0x3C, 0x3C);
        BattleAudienceSoundClap(-1, 0);
        BattleAudienceSoundWhistle();
        flag = 0;
        party = BattleGetPartyPtr(wp);
        if (party != NULL && BtlUnit_CheckStatus(party, 0x1B)) {
            flag = 1;
        }
        if (flag == 0) {
            psndBGMOff(0x400);
            psndBGMOn(1, (char*)(str + 0x340));
        } else {
            psndBGMOff(0x400);
            psndBGMOn(1, (char*)(str + 0x350));
        }
        /* fallthrough */
    case 0x07000009:
        (*(s32*)((u8*)work + 0x4))--;
        if (*(s32*)((u8*)work + 0x4) > 0) {
            break;
        }
        BattleSetSeq(wp, SEQ_END, 0x0700000A);
        break;

    case 0x0700000A:
        if (*(s16*)((u8*)work + 0x8) > 0) {
            unit = BattleGetMarioPtr(wp);
            party = BattleGetPartyPtr(wp);
            event = evtEntry(BtlUnit_GetData(unit, 0x40), 0xA, 0);
            if (event != NULL) {
                event->unitId = unit->unitId;
                unit->attackEventId = event->eventId;
            }
            if (party != NULL) {
                event = evtEntry(BtlUnit_GetData(party, 0x40), 0xA, 0);
                if (event != NULL) {
                    event->unitId = party->unitId;
                    party->attackEventId = event->eventId;
                }
            }
            if (*(void**)((u8*)work + 0x29C) == NULL) {
                *(void**)((u8*)work + 0x29C) =
                    effPointGetEntry(0, *(s16*)((u8*)work + 0x8), 0, 0.0f, 90.0f, 0.0f);
            }
            BattleSetSeq(wp, SEQ_END, 0x0700000B);
            break;
        }
        if (!(wp->flags & 0x400000)) {
            BattleSetSeq(wp, SEQ_END, 0x07000019);
            break;
        }
        BattleSetSeq(wp, SEQ_END, 0x0700000E);
        break;

    case 0x0700000B:
        if (*(s16*)((u8*)work + 0xA) > 0) {
            (*(s32*)((u8*)work + 0x4))++;
            if (BattlePadCheckNow(0x300) && *(s32*)((u8*)work + 0x4) < 0x3E8) {
                *(s32*)((u8*)work + 0x4) += 0x3E8;
            }
            if ((*(s32*)((u8*)work + 0x4) & 3) == 0 || *(s32*)((u8*)work + 0x4) >= 0x3E8) {
                (*(s16*)((u8*)work + 0xA))--;
                pouch->mStarPoints++;
                if ((*(s32*)((u8*)work + 0x4) & 7) == 0) {
                    psndSFXOn(str + 0x360);
                }
            }
            if (pouch->mStarPoints >= 0x64) {
                pouch->mStarPoints = 0;
                *(s32*)((u8*)work + 0xC) = 1;
                if (pouch->level + 1 >= 0x63) {
                    *(s16*)((u8*)work + 0xA) = 0;
                }
            }
            break;
        }
        if (*(s32*)((u8*)work + 0x4) >= 0x3E8) {
            *(s32*)((u8*)work + 0x4) = 0xA;
        } else if (*(s32*)((u8*)work + 0x4) < 0x78) {
            *(s32*)((u8*)work + 0x4) = 0x8C - *(s32*)((u8*)work + 0x4);
        } else {
            *(s32*)((u8*)work + 0x4) = 0x14;
        }
        BattleSetSeq(wp, SEQ_END, 0x0700000C);
        break;

    case 0x0700000C:
        (*(s32*)((u8*)work + 0x4))--;
        if (*(s32*)((u8*)work + 0x4) > 0) {
            break;
        }
        *(s32*)((u8*)work + 0x4) = 0x3C;
        _GetExpIcon_End(work);
        BattleSetSeq(wp, SEQ_END, 0x0700000D);
        effSoftDelete(*(EffectEntry**)((u8*)work + 0x29C));
        if (*(s32*)((u8*)work + 0xC) != 0) {
            *(s32*)((u8*)work + 0x4) = 0x3C;
            break;
        }
        *(s32*)((u8*)work + 0x4) = 5;
        break;

    case 0x0700000D:
        (*(s32*)((u8*)work + 0x4))--;
        if (*(s32*)((u8*)work + 0x4) > 0) {
            break;
        }
        *(s32*)((u8*)work + 0x29C) = 0;
        BattleSetSeq(wp, SEQ_END, 0x0700000E);
        break;

    case 0x0700000E:
        if (wp->flags & 0x400000) {
            unit = BattleGetMarioPtr(wp);
            event = evtEntry(_event_MajinaiEnd, 0xA, 0);
            event->unitId = unit->unitId;
            unit->attackEventId = event->eventId;
            BattleSetSeq(wp, SEQ_END, 0x0700000F);
            break;
        }
        BattleSetSeq(wp, SEQ_END, 0x07000010);
        break;

    case 0x0700000F:
        if (!BattleWaitAllActiveEvtEnd(wp)) {
            break;
        }
        if (*(s16*)((u8*)work + 0x8) > 0) {
            BattleSetSeq(wp, SEQ_END, 0x07000010);
            break;
        }
        BattleSetSeq(wp, SEQ_END, 0x07000019);
        break;

    case 0x07000010:
        *(u32*)((u8*)work + 0x30) &= ~1;
        if (*(s32*)((u8*)work + 0xC) != 0) {
            *(s32*)((u8*)work + 0x10) = 1;
            *(s32*)((u8*)work + 0x14) = 0;
            *(s32*)((u8*)work + 0x18) = 3;
            *(s32*)((u8*)work + 0x4) = 0;
            BattleAudience_WinSetActive(0);
            BattleBreakSlot_HideReel();
            BattleSetSeq(wp, SEQ_END, 0x07000011);
            break;
        }
        BattleSetSeq(wp, SEQ_END, 0x07000019);
        break;

    case 0x07000011:
        if (*(s32*)((u8*)work + 0x4) == 0) {
#ifdef MUST_MATCH
            event = evtEntry(data + 0x7C, 0xA, 0);
#else
            event = evtEntry(_curtain_close_event, 0xA, 0);
#endif
            *(s32*)((u8*)work + 0x1FC) = event->eventId;
            BattleStatusWindowSystemOff();
            psndBGMOff(0x400);
            psndBGMOn(1, (char*)(str + 0x374));
        }
        (*(s32*)((u8*)work + 0x4))++;
        if (evtCheckID(*(s32*)((u8*)work + 0x1FC))) {
            break;
        }
        *(s32*)((u8*)work + 0x1FC) = 0;
        BattleStatusWindowSystemOn();
        BattleStatusWindowEventOn();
        BattleSetSeq(wp, SEQ_END, 0x07000012);
        break;

    case 0x07000012:
        flag = *(s32*)((u8*)work + 0x10);
        if (*(s32*)((u8*)work + 0x1FC) != 0) {
            if (evtCheckID(*(s32*)((u8*)work + 0x1FC))) {
                break;
            }
            *(s32*)((u8*)work + 0x1FC) = 0;
        }
        if (BattlePadCheckTrigger(8)) {
            if (*(s32*)((u8*)work + 0x10) < *(s32*)((u8*)work + 0x18) - 1) {
                (*(s32*)((u8*)work + 0x10))++;
            }
        }
        if (BattlePadCheckTrigger(4)) {
            if (*(s32*)((u8*)work + 0x10) > 0) {
                (*(s32*)((u8*)work + 0x10))--;
            }
        }
        if (*(s32*)((u8*)work + 0x10) != flag) {
            event = NULL;
            switch (*(s32*)((u8*)work + 0x10)) {
#ifdef MUST_MATCH
            case 0: event = evtEntry(data + 0xAEC, 0xA, 0); break;
#else
            case 0: event = evtEntry(_move_to_heart_event, 0xA, 0); break;
#endif
#ifdef MUST_MATCH
            case 1: event = evtEntry(data + 0xC04, 0xA, 0); break;
#else
            case 1: event = evtEntry(_move_to_flower_event, 0xA, 0); break;
#endif
#ifdef MUST_MATCH
            case 2: event = evtEntry(data + 0xD34, 0xA, 0); break;
#else
            case 2: event = evtEntry(_move_to_badge_point_event, 0xA, 0); break;
#endif
            }
            if (event != NULL) {
                *(s32*)((u8*)work + 0x1FC) = event->eventId;
                break;
            }
        }
        if (BattlePadCheckTrigger(0x100)) {
            flag = 0;
            switch (*(s32*)((u8*)work + 0x10)) {
            case 0: if (pouch->baseMaxHP < 0xC8) flag = 1; break;
            case 1: if (pouch->baseMaxFP < 0xC8) flag = 1; break;
            case 2: if (pouch->maximumBP < 0x63) flag = 1; break;
            }
            if (flag != 0) {
                *(s32*)((u8*)work + 0x24) = 0;
                *(s32*)((u8*)work + 0x28) = 0;
                *(s32*)((u8*)work + 0x2C) = 2;
                *(u32*)((u8*)work + 0x30) &= ~0x20;
                BattleSetSeq(wp, SEQ_END, 0x07000014);
#ifdef MUST_MATCH
                event = evtEntry(data + 0x9C4, 0xA, 0);
#else
                event = evtEntry(_jump_event, 0xA, 0);
#endif
                *(s32*)((u8*)work + 0x1FC) = event->eventId;
            }
        }
        break;

    case 0x07000013:
        if (BattlePadCheckTrigger(2)) {
            (*(s32*)((u8*)work + 0x24))++;
            if (*(s32*)((u8*)work + 0x24) >= *(s32*)((u8*)work + 0x2C)) {
                *(s32*)((u8*)work + 0x24) = 0;
            }
        }
        if (BattlePadCheckTrigger(1)) {
            (*(s32*)((u8*)work + 0x24))--;
            if (*(s32*)((u8*)work + 0x24) < 0) {
                *(s32*)((u8*)work + 0x24) = *(s32*)((u8*)work + 0x2C) - 1;
            }
        }
        if (BattlePadCheckTrigger(0x200)) {
            *(u32*)((u8*)work + 0x30) &= ~0x10;
            BattleSetSeq(wp, SEQ_END, 0x07000012);
        }
        if (BattlePadCheckTrigger(0x100)) {
            if (*(s32*)((u8*)work + 0x24) == 0) {
                *(u32*)((u8*)work + 0x30) &= ~0x18;
                BattleSetSeq(wp, SEQ_END, 0x07000015);
                break;
            }
            *(u32*)((u8*)work + 0x30) &= ~0x10;
            BattleSetSeq(wp, SEQ_END, 0x07000012);
        }
        break;

    case 0x07000014:
        if (!evtCheckID(*(s32*)((u8*)work + 0x1FC))) {
            *(s32*)((u8*)work + 0x1FC) = 0;
            BattleSetSeq(wp, SEQ_END, 0x07000015);
        }
        break;

    case 0x07000015:
        pouch->level++;
        switch (*(s32*)((u8*)work + 0x10)) {
        case 0: pouch->baseMaxHP += 5; break;
        case 1: pouch->baseMaxFP += 5; break;
        case 2: pouch->maximumBP += 3; break;
        }
        pouchReviseMarioParam();
        pouch->currentHP = pouch->maximumHP;
        pouch->currentFP = pouch->maximumFP;
        unit = BattleGetMarioPtr(wp);
        BtlUnit_SetParamFromPouch(unit);
        party = BattleGetPartyPtr(wp);
        if (party != NULL) {
            s32 id = BattleTransPartyId(party->currentKind);

            if (id != 0 && id < 8) {
                pouch->partyData[id].currentHP = pouch->partyData[id].maximumHP;
                party->currentHp = party->maxHp;
            }
        }
        flag = 1;
        unit = BattleGetMarioPtr(wp);
        party = BattleGetPartyPtr(wp);
        {
            u32 sf = (u32)unit->mStatusFlags;

            if ((sf & 0x10000000) || (sf & 0x20000000)) {
                flag = 0;
            } else if (party != NULL) {
                if ((sf & 0x10000000) || (sf & 0x20000000)) {
                    flag = 0;
                } else if (BtlUnit_CheckStatus(party, 0x1B)) {
                    flag = 0;
                }
            }
        }
        if (flag != 0) {
            if (info->setup->field_4 & 0x20000000) {
                BtlUnit_snd_se(unit, str + 0x384, 0xF1194D80, 0);
            } else {
                BtlUnit_snd_se(unit, str + 0x39C, 0xF1194D80, 0);
            }
        }
        BtlUnit_SetBodyAnim(BattleGetMarioPtr(wp), "M_V_2");
        BattleAudienceSoundCheer(0xB4, 0x28);
        *(s32*)((u8*)work + 0x4) = 0xB4;
        BattleSetSeq(wp, SEQ_END, 0x07000016);
        unit = BattleGetMarioPtr(wp);
        party = BattleGetPartyPtr(wp);
        if (unit != NULL) {
            effRecoveryEntry(4, 0, unit->mPosition.x, 40.0f + unit->mPosition.y, unit->mPosition.z);
        }
        if (party != NULL) {
            effRecoveryEntry(4, 0, party->mPosition.x, 40.0f + party->mPosition.y, party->mPosition.z);
        }
        break;

    case 0x07000016:
        (*(s32*)((u8*)work + 0x4))--;
        if (*(s32*)((u8*)work + 0x4) == 0x78) {
            unit = BattleGetMarioPtr(wp);
            if (unit != NULL) {
                effRecoveryEntry(5, 0, unit->mPosition.x, 40.0f + unit->mPosition.y, unit->mPosition.z);
            }
        }
        if (*(s32*)((u8*)work + 0x4) > 0) {
            break;
        }
        if (*(s32*)((u8*)work + 0x2A4) != 0) {
            evtDeleteID(*(s32*)((u8*)work + 0x2A4));
        }
        BattleSetSeq(wp, SEQ_END, 0x07000017);
        break;

    case 0x07000017:
        if (!_get_rank_data(pouch->level)) {
            BattleSetSeq(wp, SEQ_END, 0x0700001C);
            break;
        }
        pouch->rank = _get_rank_data(pouch->level)->rankId;
        event = evtEntry(_rank_up_event, 0xA, 0);
        *(s32*)((u8*)work + 0x2A8) = event->eventId;
        BattleSetSeq(wp, SEQ_END, 0x07000018);
        /* fallthrough */
    case 0x07000018:
        if (!evtCheckID(*(s32*)((u8*)work + 0x2A8))) {
            BattleSetSeq(wp, SEQ_END, 0x0700001C);
        }
        break;

    case 0x07000019:
        unit = BattleGetMarioPtr(wp);
        party = BattleGetPartyPtr(wp);
        flag = 1;
        {
            u32 sf = (u32)unit->mStatusFlags;

            if ((sf & 0x10000000) || (sf & 0x20000000)) {
                flag = 0;
            } else if (party != NULL) {
                if ((sf & 0x10000000) || (sf & 0x20000000)) {
                    flag = 0;
                } else if (BtlUnit_CheckStatus(party, 0x1B)) {
                    flag = 0;
                }
            }
        }
        if (flag == 0) {
            if (info->setup->field_4 & 0x20000000) {
                BtlUnit_snd_se(unit, str + 0x3B4, 0xF1194D80, 0);
            } else {
                BtlUnit_snd_se(unit, str + 0x3CC, 0xF1194D80, 0);
            }
        }
        BtlUnit_SetBodyAnim(BattleGetMarioPtr(wp), "M_V_1");
        *(s32*)((u8*)work + 0x4) = 0x3C;
        BattleSetSeq(wp, SEQ_END, 0x0700001A);
        break;

    case 0x0700001A:
        if (--(*(s32*)((u8*)work + 0x4)) > 0) {
            break;
        }
        BtlUnit_SetBodyAnimType(BattleGetMarioPtr(wp), 0x2A);
        party = BattleGetPartyPtr(wp);
        if (party != NULL) {
            if (!(party->currentKind == 0xE1 && party->flippedTurns >= 1)) {
                BtlUnit_SetBodyAnimType(party, 0x2A);
            }
        }
        BattleSetSeq(wp, SEQ_END, 0x0700001B);
        break;

    case 0x0700001B:
        BattleSetSeq(wp, SEQ_END, 0x0700001C);
        break;

    case 0x0700001C:
        BattleSetSeq(wp, SEQ_END, 0x0700001D);
        BattleAudienceSoundSetVolAll(0, 0x1E);
        {
            NpcEntry* target = fbatGetPointer()->target;

            if (target != NULL) {
                *(u32*)((u8*)target + 0x140) |= 0x100;
            }
        }
        break;

    case 0x0700001D:
        fadeEntry(0xA, 0xC8, dat_80420c04);
        BattleSetSeq(wp, SEQ_END, 0x0700001E);
        psndBGMOff(0x201);
        /* fallthrough */
    case 0x0700001E:
        if (fadeIsFinish()) {
            BattleSetSeq(wp, SEQ_END, 0x0700001F);
        }
        break;

    case 0x0700001F:
        party = BattleGetPartyPtr(wp);
        flag = (party != NULL) ? party->currentKind : 0;
        i = BattleTransPartyId((BattleUnitType)flag);
        unit = BattleGetMarioPtr(wp);
        if ((info->setup->field_4 & 0x40) && unit->currentHp <= 0) {
            unit->currentHp = 1;
        }
        BattleSetMarioParamToFieldBattle(wp);
        BattleFree(wp->seqEndWork);
        wp->seqEndWork = NULL;
        BattleSetSeq(wp, SEQ_UNKNOWN, 4);
        *(u32*)((u8*)marioGetPtr() + 0x4) &= ~1;
        if (flag != 0) {
            marioPartyEntry(i);
        } else {
            marioPartyKill();
        }
        {
            void* p = partyGetPtr(marioGetPartyId());

            if (p != NULL) {
                *(u32*)((u8*)p + 0x4) &= ~1;
            }
        }
        BtlActRec_JudgeRuleKeep();
        BattleAudienceSoundStop(1);
        break;
    }

    btlseqEnd_DispMain(8, wp);
}

// The original keeps _keta_get out of line: _lvup_select_object_disp calls it with bl, not inlined.
s32 _keta_get(s32 value) {
    s32 digits = 0;

    if (value < 0) {
        value *= -1;
    }
    if (value == 0) {
        return 1;
    }
    while (value > 0) {
        value /= 10;
        digits++;
    }
    return digits;
}

s32 _get_rank_up_msg(EventEntry* event, BOOL isFirstCall) {
    s32 idx = event->args[0];
    PouchData* pouch = pouchGetPtr();
    RankUpData* rank = _get_rank_data(pouch->level);

    evtSetValue(event, idx, (s32)rank->rankUpMessage);
    return 2;
}

static s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall) {
    EventEntry* e = event;
    BattleWork* wp = _battleWorkPointer;
    s32* args = e->args;
    s32 type = evtGetValue(e, args[0]);
    s32 outIdx = args[1];
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(e, type));

    if (!(unit->mAttributeFlags & 0x20)) {
        evtSetValue(e, outIdx, -1);
        return 2;
    }
    {
        BattleUnit* koura = BtlUnit_GetGuardKouraPtr(unit);

        if (koura == NULL) {
            evtSetValue(e, outIdx, -1);
            return 2;
        }
        evtSetValue(e, outIdx, koura->unitId);
    }
    return 2;
}

// NONMATCHING (96.45%): 1 instruction differs, 1 missing.
void _audience_Whistle_control(void) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    if (work == NULL) {
        return;
    }
    if (*(s32*)((u8*)work + 0x0) < 0x0700001C) {
        if (*(s32*)((u8*)work + 0x0) >= 0x07000007) {
            if (--*(s32*)((u8*)work + 0x2A0) < 0) {
                *(s32*)((u8*)work + 0x2A0) = irand(0x78) + 0x3C;
                BattleAudienceSoundWhistle();
            }
        }
    }
}

// NONMATCHING (98.45%): only the intplGetValue 0.0f float-constant load is scheduled after lwz r4/r5;
// the original loads it first.
s32 _lvup_spot_off(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    s32 i;
    s32* args = event->args;

    if (isFirstCall) {
        event->userdata[0] = evtGetValue(event, args[0]);
        event->userdata[1] = event->userdata[0];
        event->userdata[2] = *((u8*)spot->entries + 0x2B);
    }
    event->userdata[1]--;
    for (i = 0; i < 5; i++) {
        BattleSpotEntry* entry = &spot->entries[i];
        GXColor color = {0xFF, 0xFF, 0xFF, 0x00};

        ((u8*)&color)[3] = (s32)intplGetValue(2, event->userdata[1], event->userdata[0], 0.0f, (f32)event->userdata[2]);
        entry->field_28 = *(u32*)&color;
        if (((u8*)&entry->field_28)[3] == 0) {
            entry->flags &= ~2;
        }
    }
    return (event->userdata[1] > 0) ? 0 : 2;
}

// NONMATCHING (98.19%): rebuilt from the asm. Body matches; only the .rodata constant base/offsets differ.
s32 _lvup_spot_on(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    BattleSpotEntry* entry;
    s32 code;

    s32* args = event->args;
    spot->skipRender = TRUE;
    code = evtGetValue(event, args[0]);
    entry = &spot->entries[code];

    switch (code) {
    case 0:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, 25.0f, -50.0f};
        break;
    case 1:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, -10.0f, -50.0f};
        break;
    case 2:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, 25.0f, -50.0f};
        break;
    case 3:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, -10.0f, -50.0f};
        break;
    case 4:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, 25.0f, -50.0f};
        break;
    }
    entry->field_10 = (Vec){0.0f, 0.0f, 0.0f};
    entry->field_1C = (Vec){15.0f, 15.0f, 150.0f};
    entry->field_28 = 0xFFFFFFFF;
    entry->flags |= 2;
    return 2;
}

s32 _lvup_spot_move(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    BattleSpotEntry* entry;
    s32 code;

    code = evtGetValue(event, event->args[0]);
    entry = &spot->entries[code];

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        event->userdata[2] = (s32)entry->field_4.x;
        event->field_84 = (s32)entry->field_4.y;
        *(s32*)&event->wInterpRelatedTime = 0;
    }

    event->userdata[0]++;
    switch (event->userdata[1]) {
    case 0:
        event->userdata[0] = 1;
        event->userdata[1]++;
    case 1:
        switch (code) {
        case 0:
            entry->field_4.x = float_0p5_80420c8c * BattleGetMarioPtr(_battleWorkPointer)->mPosition.x;
            entry->field_4.y = float_neg20_80420c90;
            break;
        case 1:
            entry->field_4.x = float_50_80420c94 * sinfd(reviseAngle(float_1p1_80420c98 * (float_3_80420c6c * (f32)event->userdata[0]))) + (f32)event->userdata[2];
            entry->field_4.y = (f32)((s32)event->field_84 + 25) - float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_2p5_80420ca4 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        case 2:
            entry->field_4.x = float_65_80420ca8 * sinfd(reviseAngle(float_1p1_80420c98 * (float_2p5_80420ca4 * (f32)event->userdata[0]) + float_180_80420cac)) + (f32)event->userdata[2];
            entry->field_4.y = (f32)((s32)event->field_84 - 25) + float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_7p5_80420cb0 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        case 3:
            entry->field_4.x = (f32)event->userdata[2] - float_50_80420c94 * sinfd(reviseAngle(float_1p1_80420c98 * (float_4_80420c78 * (f32)event->userdata[0])));
            entry->field_4.y = (f32)((s32)event->field_84 + 25) - float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_2p5_80420ca4 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        case 4:
            entry->field_4.x = (f32)(event->userdata[2] - 50) - float_50_80420c94 * sinfd(reviseAngle(float_1p1_80420c98 * (float_3_80420c6c * (f32)event->userdata[0]) + float_270_80420cb4));
            entry->field_4.y = (f32)((s32)event->field_84 - 25) + float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_4p5_80420cb8 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        }
        break;
    case 2:
        *(s32*)&event->wInterpRelatedTime = 20;
        event->userdata[2] = (s32)entry->field_4.x;
        event->field_84 = (s32)entry->field_4.y;
        event->userdata[1]++;
    case 3:
        --*(s32*)&event->wInterpRelatedTime;
        entry->field_4.x = intplGetValue(2, *(s32*)&event->wInterpRelatedTime, 21, float_0_80420c7c, (f32)event->userdata[2]);
        entry->field_4.y = intplGetValue(2, *(s32*)&event->wInterpRelatedTime, 21, float_neg25_80420cbc, (f32)(s32)event->field_84);
        {
            GXColor color = {0xFF, 0xFF, 0xFF, 0x00};

            ((u8*)&color)[3] = (s32)intplGetValue(0, *(s32*)&event->wInterpRelatedTime, 21, float_255_80420c88, float_128_80420cc0);
            entry->field_28 = *(u32*)&color;
        }
        if (*(s32*)&event->wInterpRelatedTime <= 0) {
            event->userdata[1]++;
        }
        break;
    }
    return (event->userdata[1] < 4) ? 0 : 2;
}

// NONMATCHING (99.76%): every instruction matches; all 30 objdiff differences are DIFF_ARG_MISMATCH, not
// opcode/register/scheduling. 27 are the six .rodata Vec pairs + three zero Vecs: ours are at 0xD8+, the
// original's at 0x1F0+, a constant 0x118 displacement because this unit's .rodata still lacks the data-table
// strings (nozzle/confetti/levelup/SFX/spotlight) that precede them in the original. The other 3 are the
// .sdata2 colour words (anonymous @292/@299/@306 here, dat_80420c34/38/3c in the original; same value). No edit
// to this function can move either pool: both depend on the rest of the unit's functions/data. Verified that
// naming the colours `extern const GXColor dat_80420c34` regresses to 92.50% (mwcc hoists the load and shifts
// the registers), and moving the function to its reverse-asm source position only changes 0xD8 -> 0xE4.
s32 _lvup_select_object_init(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    PouchData* pouch = pouchGetPtr();

    *(Vec*)((u8*)work + 0x200) = (Vec){-80.0f, 105.0f, 0.0f};
    *(Vec*)((u8*)work + 0x20C) = (Vec){0.5f, 0.5f, 1.0f};
    *(Vec*)((u8*)work + 0x234) = (Vec){0.0f, 105.0f, 0.0f};
    *(Vec*)((u8*)work + 0x240) = (Vec){0.5f, 0.5f, 1.0f};
    *(Vec*)((u8*)work + 0x268) = (Vec){80.0f, 105.0f, 0.0f};
    *(Vec*)((u8*)work + 0x274) = (Vec){0.5f, 0.5f, 1.0f};

    ((u8*)work)[0x228] = 0;
    *(GXColor*)((u8*)work + 0x230) = (GXColor){0xC8, 0xFF, 0xFF, 0xFF};
    *(s16*)((u8*)work + 0x224) = pouch->baseMaxHP;
    *(Vec*)((u8*)work + 0x218) = (Vec){0.0f, 0.0f, 0.0f};
    if (*(u16*)((u8*)work + 0x224) >= 0xC8) {
        *(u16*)((u8*)work + 0x226) = *(u16*)((u8*)work + 0x224);
    } else {
        *(u16*)((u8*)work + 0x226) = *(u16*)((u8*)work + 0x224) + 5;
    }

    ((u8*)work)[0x25C] = 0;
    *(GXColor*)((u8*)work + 0x264) = (GXColor){0xC8, 0xFF, 0xFF, 0xFF};
    *(s16*)((u8*)work + 0x258) = pouch->baseMaxFP;
    *(Vec*)((u8*)work + 0x24C) = (Vec){0.0f, 0.0f, 0.0f};
    if (*(u16*)((u8*)work + 0x258) >= 0xC8) {
        *(u16*)((u8*)work + 0x25A) = *(u16*)((u8*)work + 0x258);
    } else {
        *(u16*)((u8*)work + 0x25A) = *(u16*)((u8*)work + 0x258) + 5;
    }

    ((u8*)work)[0x290] = 0;
    *(GXColor*)((u8*)work + 0x298) = (GXColor){0xC8, 0xFF, 0xFF, 0xFF};
    *(s16*)((u8*)work + 0x28C) = pouch->maximumBP;
    *(Vec*)((u8*)work + 0x280) = (Vec){0.0f, 0.0f, 0.0f};
    if (*(u16*)((u8*)work + 0x28C) >= 99) {
        *(u16*)((u8*)work + 0x28E) = *(u16*)((u8*)work + 0x28C);
    } else {
        *(u16*)((u8*)work + 0x28E) = *(u16*)((u8*)work + 0x28C) + 3;
    }

    *(u32*)((u8*)work + 0x30) |= 8;
    return 2;
}

s32 _lvup_object_select_move(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 selected = *(s32*)((u8*)work + 0x10);
    BattleUnit* mario = BattleGetMarioPtr(wp);
    u8* entry = (u8*)work + 0x200 + selected * 0x34;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0x3C;
        event->userdata[2] = (s32)*(f32*)(entry + 0x0);
        event->field_84 = (s32)*(f32*)(entry + 0x8);
        *(s32*)((u8*)event + 0x88) = (s32)mario->mPosition.x;
        *(s32*)((u8*)event + 0x8C) = (s32)mario->mPosition.z;
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[1]) {
        *(f32*)(entry + 0x0) = intplGetValue(5, event->userdata[0], event->userdata[1], (f32)(s32)event->userdata[2], (f32)*(s32*)((u8*)event + 0x88));
        *(f32*)(entry + 0x8) = intplGetValue(5, event->userdata[0], event->userdata[1], (f32)(s32)event->field_84, (f32)*(s32*)((u8*)event + 0x8C));
        *(f32*)(entry + 0x1C) = reviseAngle(intplGetValue(5, event->userdata[0], event->userdata[1], 0.0f, 2160.0f));
        return 0;
    }
    *(f32*)(entry + 0x0) = (f32)*(s32*)((u8*)event + 0x88);
    *(f32*)(entry + 0x8) = (f32)*(s32*)((u8*)event + 0x8C);
    return 2;
}

s32 _lvup_object_decide_bound(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 selected = *(s32*)((u8*)work + 0x10);
    u8* entry = (u8*)work + 0x200 + selected * 0x34;
    s32 half;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = (s32)*(f32*)(entry + 0x4);
        event->userdata[2] = 0x10;
        event->field_84 = 0x1E;
        entry[0x28] = 2;
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[2]) {
        half = event->userdata[2] / 2;
        if (event->userdata[0] < half) {
            *(f32*)(entry + 0x4) = (f32)event->userdata[1] + intplGetValue(5, event->userdata[0], half, 0.0f, (f32)(s32)event->field_84);
        } else {
            *(f32*)(entry + 0x4) = (f32)event->userdata[1] + intplGetValue(5, event->userdata[0] - half, half, (f32)(s32)event->field_84, 0.0f);
        }
        return 0;
    }
    *(f32*)(entry + 0x4) = (f32)event->userdata[1];
    entry[0x28] = 4;
    effPokopiPchargeN64Entry(*(f32*)(entry + 0x0), *(f32*)(entry + 0x4), *(f32*)(entry + 0x8), 1.5f, 3, 0x78);
    return 2;
}

s32 _lvup_object_noselect_delete(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 selected = *(s32*)((u8*)work + 0x10);
    s32 i;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[2] = 0x1E;
        for (i = 0; i < *(s32*)((u8*)work + 0x18); i++) {
            if (i != selected) {
                ((u8*)work)[i * 0x34 + 0x228] = 3;
            }
        }
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[2]) {
        for (i = 0; i < *(s32*)((u8*)work + 0x18); i++) {
            if (i != selected) {
                ((u8*)work)[i * 0x34 + 0x233] = (u8)(s32)intplGetValue(5, event->userdata[0], event->userdata[2], 255.0f, 0.0f);
            }
        }
        return 0;
    }
    for (i = 0; i < *(s32*)((u8*)work + 0x18); i++) {
        if (i != selected) {
            ((u8*)work)[i * 0x34 + 0x233] = 0;
        }
    }
    return 2;
}

s32 _disable_enemy_unit_disp(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        BattleUnitType type;

        if (unit == NULL) {
            continue;
        }
        if ((u32)unit->flags & 0x80000000) {
            continue;
        }
        type = unit->currentKind;
        if (type == 0xDC) {
            continue;
        }
        if (type == 0xDE) {
            continue;
        }
        if (type >= 0xE0 && type < 0xE7 && unit->mAlliance == 0 && type != 0xE5) {
            continue;
        }
        {
            void* data = BtlUnit_GetData(unit, 0x42);

            if (data != NULL) {
                EventEntry* e = evtEntry(data, 0xA, 0);
                e->unitId = unit->unitId;
            } else if (unit->currentKind != 0xE5) {
                unit->mAttributeFlags |= 0x21000000;
            }
        }
    }
    {
        BattleStageObject* obj = BattleSearchObjectPtr("if_body");

        if (obj != NULL) {
            *(u8*)((u8*)wp + 0x180DC) |= 0x20;
            obj->mFlags |= 1;
            mapGrpFlagOn("if_body", 1);
            mapGrpFlagOn("if_wire", 1);
        }
    }
    return 2;
}

void _check_audience_battle_end_exec(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleGroupSetup* group = wp->fbatInfo->setup->groupSetup;
    s32 firstLevel = group->enemyData[0].mUnitKindParams->mLevel;
    s32 turn;
    s32 sum;
    s32 i;
    s32 level;
    BattleUnitSetup* entry;
    PouchData* pouch;

    if (firstLevel > (pouch = pouchGetPtr())->level + 1) {
        BattleAudience_Case_GreatVictory();
    }
    turn = wp->turnCount;
    pouch = pouchGetPtr();
    sum = 0;
    level = pouch->level + 1;
    entry = group->enemyData;
    for (i = 0; i < group->numEnemies; i++, entry++) {
        sum += entry->mUnitKindParams->mLevel;
    }
    level = sum * 2 / level / 3;
    if (turn < level) {
        BattleAudience_Case_FastVictory((f32)(level - turn));
    }
}

void _ExecAllUnitBattleEndEvent(void) {
    BattleWork* wp;
    s32 i;

    wp = _battleWorkPointer;
    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL) {
            void* data = BtlUnit_GetData(unit, 0x3F);

            if (data != NULL) {
                EventEntry* event = evtEntry(data, 0xA, 0);
                event->unitId = unit->unitId;
            }
        }
    }
}

s32 _lvup_ap_recover_full(void) {
    BattleAudience* audience = BattleAudienceBaseGetPtr();

    audience->excited = TRUE;
    pouchSetAP(pouchGetMaxAP());
    return 2;
}

s32 _nozzle_set_offset(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    f32 offset = evtGetFloat(event, args[1]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)id);

    if (obj != NULL) {
        obj->field_0x10.y = offset;
    }
    return 2;
}

void _BattleMarioKouraDelete(void) {
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    EventEntry* event = evtEntry(_koura_delete_event, 0xA, 0);

    event->unitId = mario->unitId;
}

void _GetExpIcon_End(BattleSeqEndWork* work) {
    s32 i;

    *(u32*)((u8*)work + 0x30) &= ~2;
    for (i = 0; i < 0x13; i++) {
        iconDelete((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18));
    }
}

// NONMATCHING (97.48%): 2 missing, 1 extra, plus register differences.
void _GetExpIcon_Main(BattleWork* wp) {
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 exp = *(s16*)((u8*)work + 0xA);
    s32 i;

    for (i = 0; i < exp / 10; i++) {
        iconFlagOff((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18), 2);
        if (*(s32*)((u8*)work + 0x80 + i * 4) <= 0) {
            *(s32*)((u8*)work + 0x80 + i * 4) = 0x40;
        } else {
            *(s32*)((u8*)work + 0x80 + i * 4) += 0x40;
            if (*(s32*)((u8*)work + 0x80 + i * 4) > 0xFF) {
                *(s32*)((u8*)work + 0x80 + i * 4) = 0xFF;
            }
        }
        if (*(s32*)((u8*)work + 0xCC + i * 4) < 0x10) {
            *(s32*)((u8*)work + 0xCC + i * 4) += 1;
        }
        *(f32*)((u8*)work + 0x118 + i * 0xC) =
            36.0f * (f32)i + (24.0f - 18.0f * (f32)(*(s16*)((u8*)work + 0xA) / 10));
    }
    for (; i < 10; i++) {
        iconFlagOn((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18), 2);
        *(s32*)((u8*)work + 0x80 + i * 4) = 0;
        *(s32*)((u8*)work + 0xCC + i * 4) = 0;
    }
    {
        s32 rem = exp % 10;
        s32 j;

        for (j = 10; j - 10 < rem; j++) {
            iconFlagOff((const char*)(*(u32*)((u8*)work + 0x34 + j * 4) + 0x18), 2);
            if (*(s32*)((u8*)work + 0x80 + j * 4) <= 0) {
                *(s32*)((u8*)work + 0x80 + j * 4) = 0x40;
            } else {
                *(s32*)((u8*)work + 0x80 + j * 4) += 0x40;
                if (*(s32*)((u8*)work + 0x80 + j * 4) > 0xFF) {
                    *(s32*)((u8*)work + 0x80 + j * 4) = 0xFF;
                }
            }
        }
        for (; j < 0x13; j++) {
            iconFlagOn((const char*)(*(u32*)((u8*)work + 0x34 + j * 4) + 0x18), 2);
            *(s32*)((u8*)work + 0x80 + j * 4) = 0;
        }
    }
    for (i = 0; i < 0x13; i++) {
        iconSetAlpha((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18),
                     (u8)*(s32*)((u8*)work + 0x80 + i * 4));
        iconSetPos((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18),
                   *(f32*)((u8*)work + 0x118 + i * 0xC), 0.0f + *(f32*)((u8*)work + 0x11C + i * 0xC),
                   *(f32*)((u8*)work + 0x120 + i * 0xC));
    }
}

// NONMATCHING (93.80%): second-loop offsets (orig preloads li 0x28 / li 0x78 and increments, ours
// computes i*4 / i*0xc) and some register naming differ.
void _GetExpIcon_Init(BattleSeqEndWork* arg) {
    BattleSeqEndWork* work = arg;
    s32 i;
    char name[0x10];

    for (i = 0; i < 10; i++) {
        sprintf(name, "_Bgsp%d", i);
        iconEntry2D(name, 0x18C);
        *(u32*)((u8*)work + 0x34 + i * 4) = (u32)iconNameToPtr(name);
        iconSetScale(name, 0.9f);
        iconFlagOn(name, 2);
        *(s32*)((u8*)work + 0xCC + i * 4) = 0;
        *(f32*)((u8*)work + 0x118 + i * 0xC) =
            36.0f * (f32)i + (24.0f - 18.0f * (f32)(*(s16*)((u8*)work + 0xA) / 10));
        *(f32*)((u8*)work + 0x11C + i * 0xC) = 25.0f;
        *(f32*)((u8*)work + 0x120 + i * 0xC) = 0.0f;
        iconSetPos(name, *(f32*)((u8*)work + 0x118 + i * 0xC),
                   *(f32*)((u8*)work + 0x11C + i * 0xC), *(f32*)((u8*)work + 0x120 + i * 0xC));
    }
    for (; i < 0x13; i++) {
        sprintf(name, "_Bgsp%d", i);
        iconEntry2D(name, 0x18C);
        *(u32*)((u8*)work + 0x34 + i * 4) = (u32)iconNameToPtr(name);
        iconSetScale(name, 0.45f);
        iconFlagOn(name, 2);
        *(s32*)((u8*)work + 0xCC + i * 4) = 0;
        *(f32*)((u8*)work + 0x118 + i * 0xC) = 86.899994f - 16.199999f * (f32)(i - 9);
        *(f32*)((u8*)work + 0x11C + i * 0xC) = 0.0f;
        *(f32*)((u8*)work + 0x120 + i * 0xC) = 0.0f;
        iconSetPos(name, *(f32*)((u8*)work + 0x118 + i * 0xC),
                   *(f32*)((u8*)work + 0x11C + i * 0xC), *(f32*)((u8*)work + 0x120 + i * 0xC));
    }
    *(u32*)((u8*)work + 0x30) |= 2;
}

s32 _lvup_disp_flag_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 flags = evtGetValue(event, args[0]);
    u32 mask = args[1];

    if (flags != 0) {
        *(u32*)((u8*)work + 0x30) = *(u32*)((u8*)work + 0x30) | mask;
    } else {
        *(u32*)((u8*)work + 0x30) = *(u32*)((u8*)work + 0x30) & ~mask;
    }
    return 2;
}

s32 _lvup_obj_set_color(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    u8* obj = (u8*)work + 0x200 + evtGetValue(event, args[0]) * 0x34;

    obj[0x30] = evtGetValue(event, args[1]);
    obj[0x31] = evtGetValue(event, args[2]);
    obj[0x32] = evtGetValue(event, args[3]);
    obj[0x33] = evtGetValue(event, args[4]);
    return 2;
}

s32 _lvup_select_obj_lvup_param_disp(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 i;

    for (i = 0; i < 3; i++) {
        ((u8*)work)[i * 0x34 + 0x228] = 1;
    }
    return 2;
}

s32 _lvup_select_obj_disp_on(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    *(u32*)((u8*)work + 0x30) |= 8;
    return 2;
}

// NONMATCHING (91.12%): 2 instructions differ, plus register differences.
s32 _set_confetti_control_evtid(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 val = args[0];
    BattleSeqEndWork* work = wp->seqEndWork;

    *(s32*)((u8*)work + 0x2A4) = evtGetValue(event, val);
    return 2;
}

void _GetExpIcon_Disp(CameraId camId, void* param) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0x80};

    windowDispGX_Waku_col(0, color, -76.899994f, 21.0f, 165.79999f, 24.199999f, 10.0f);
}

// NONMATCHING (68.47%): rebuilt from the asm; logic is complete. The switch, pouch checks and message
// selection match. Remaining differences are int->float conversion codegen (the original divides the u16
// width with srawi/addze) and instruction scheduling inside the window computation.
void L__LvupParamHelpMsgDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    PouchData* pouch;
    const char* msg;
    u16 lines;
    u16 width;
    f32 x;

    pouch = pouchGetPtr();
    switch (*(s32*)((u8*)work + 0x10)) {
    case 0:
        msg = msgSearch("help_btl_levelup_hp");
        if (pouch->baseMaxHP >= 0xC8) {
            msg = msgSearch("helperr_btl_levelup_hp");
        }
        break;
    case 1:
        msg = msgSearch("help_btl_levelup_fp");
        if (pouch->baseMaxFP >= 0xC8) {
            msg = msgSearch("helperr_btl_levelup_fp");
        }
        break;
    case 2:
        msg = msgSearch("help_btl_levelup_bp");
        if (pouch->maximumBP >= 0x63) {
            msg = msgSearch("helperr_btl_levelup_bp");
        }
        break;
    }
    width = FontGetMessageWidthLine(msg, &lines);
    x = (f32)(-(s32)(width / 2));
    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, x - 20.0f, -145.0f + (f32)(lines * 29),
                          (f32)(width + 40), (f32)((lines + 1) * 29 + 1), 10.0f);
    lines++;
    FontDrawStart_alpha(0xFF);
    FontDrawMessage((s32)x, -148 + (lines - 1) * 29, msg);
}

// NONMATCHING (82.23%): 8 missing, 8 extra, plus register differences.
void _LvupParamConfirmDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    Vec pos;
    char buf[0x14];
    u32 len;

    pouchGetPtr();
    sprintf(buf, msgSearch("btl_msg_lvup_ok"));
    len = strlen(buf);
    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -30.0f - 6.25f * (f32)len, 30.0f,
                          12.5f * (f32)len + 65.0f, 32.0f, 10.0f);
    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -70.0f, -10.0f, 130.0f, 60.0f, 10.0f);
    FontDrawStart();
    FontDrawString(buf, 0.0f - 6.25f * (f32)len, 27.0f);
    FontDrawString(msgSearch("btl_msg_lvup_yes"), -20.0f, -13.0f);
    FontDrawString(msgSearch("btl_msg_lvup_no"), -20.0f, -38.0f);
    pos.x = -40.0f;
    pos.y = (f32)(-50 - *(s32*)((u8*)work + 0x24) * 25);
    pos.z = 0.0f;
    iconDispGx(&pos, 0x10, 0x18C, 1.0f);
}

// NONMATCHING (94.62%): 2 instructions differ, 2 extra, plus register differences. Known issue: strlen's result is converted unsigned in the original (no xoris); u32 len alone scores lower because the frame shifts
void _MarioExpDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    Vec pos;
    char buf[8];
    s32 len;

    pouchGetPtr();
    pos.x = 200.0f;
    pos.y = -200.0f;
    pos.z = 0.0f;
    if (work != NULL) {
        sprintf(buf, "%2d", *(s16*)((u8*)work + 0xA));
        len = strlen(buf);
        windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, 180.0f, -160.0f, 12.5f * (f32)len + 65.0f, 32.0f, 10.0f);
        iconDispGx(&pos, 0x10, 0x18C, 1.0f);
        FontDrawStart();
        FontDrawString(buf, 225.0f, -163.0f);
    }
}

// NONMATCHING (98.87%): callee-saved allocation is swapped: orig camId=r29, param=r31, work=r30; ours
// param=r30, work=r31.
void btlseqEnd_DispMain(CameraId camId, BattleWork* param) {
    BattleSeqEndWork* work = param->seqEndWork;

    if (work == NULL) {
        return;
    }
    if (*(u32*)((u8*)work + 0x30) & 2) {
        _GetExpIcon_Main(param);
        dispEntry(camId, 900.0f, 1, _GetExpIcon_Disp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 4) {
        dispEntry(camId, 900.0f, 1, _MarioExpDisp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 8) {
        Vec pos = {0.0f, 0.0f, 0.0f};
        dispEntry(4, dispCalcZ(&pos), 2, _lvup_select_object_disp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 0x10) {
        dispEntry(camId, 900.0f, 1, _LvupParamConfirmDisp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 0x20) {
        dispEntry(camId, 900.0f, 1, L__LvupParamHelpMsgDisp, param);
    }
}

void btlseqEnd_DispInit(BattleWork* wp) {
    *(u32*)((u8*)wp->seqEndWork + 0x30) = 0;
}

void _ac_help_disp(CameraId camId, void* param) {

}

// The three level-up select objects live in the sequence-end work at 0x200, stride 0x34.
typedef struct LvupSelectObj {
    Vec pos;      // 0x00
    Vec scale;    // 0x0C
    Vec rot;      // 0x18
    u16 value;    // 0x24
    u16 max;      // 0x26
    u8 state;     // 0x28
    u8 counter;   // 0x29
    u8 pad[2];    // 0x2A
    s32 flag;     // 0x2C
    GXColor color;// 0x30
} LvupSelectObj;

// The 0x200 area of the sequence-end work: three select objects.
typedef struct BattleSeqEndWorkLvup {
    u8 unk0[0x200];
    LvupSelectObj objs[3]; // 0x200..0x29C
    u8 unk1[0x10];         // 0x29C..0x2AC
} BattleSeqEndWorkLvup;

// Logic and instruction sequence match, apart from _keta_get, which ours inlines where the original calls it, the
// stack/matrix layout, the pooled float order and the three btlDispTexPlane calls (the extra trailing zero
// comes from btlDispTexPlane's missing 8th parameter -- see the report). Remaining: the register/scheduling
// of the cur!=max position computation and the pooled int->float double is @N instead of
// double_to_int_802fe050.
// NONMATCHING (97.84%): see the note above.
void _lvup_select_object_disp(CameraId camId, void* param) {
    BattleSeqEndWorkLvup* work = (BattleSeqEndWorkLvup*)_battleWorkPointer->seqEndWork;
    Mtx mtxMain;
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRotX;
    Mtx mtxRotY;
    Mtx mtxRotZ;
    Mtx mtxT2;
    Mtx mtxT3;
    Mtx mtxT4;
    Mtx mtxS2;
    Vec pos;
    Vec pos2;
    Vec pos3;
    LvupSelectObj* obj;
    s32 i;

    for (i = 0; i < 3; i++) {
        obj = &work->objs[i];

        switch (obj->state) {
        case 4:
            obj->state = 5;
            obj->counter = 1;
            /* fallthrough */
        case 5:
            obj->counter--;
            if (obj->counter == 0) {
                obj->counter = 8;
                if (obj->flag != 0) {
                    obj->flag = 0;
                } else {
                    obj->flag = 1;
                }
            }
            break;
        default:
            obj->flag = 1;
            break;
        }
    }

    if (work->objs[0].flag != 0) {
        PSMTXIdentity(mtxMain);
        PSMTXTrans(mtxTrans, work->objs[0].pos.x, work->objs[0].pos.y, work->objs[0].pos.z);
        PSMTXScale(mtxScale, work->objs[0].scale.x, work->objs[0].scale.y, work->objs[0].scale.z);
        PSMTXRotRad(mtxRotX, 0x78, float_deg2rad_80420c40 * work->objs[0].rot.x);
        PSMTXRotRad(mtxRotY, 0x79, float_deg2rad_80420c40 * work->objs[0].rot.y);
        PSMTXRotRad(mtxRotZ, 0x7a, float_deg2rad_80420c40 * work->objs[0].rot.z);
        PSMTXConcat(mtxRotZ, mtxScale, mtxMain);
        PSMTXConcat(mtxRotZ, mtxMain, mtxMain);
        PSMTXConcat(mtxRotX, mtxMain, mtxMain);
        PSMTXConcat(mtxRotY, mtxMain, mtxMain);
        PSMTXConcat(mtxTrans, mtxMain, mtxMain);
        btlDispTexPlane2(mtxMain, 0x24, work->objs[0].color, 0);
#ifdef MUST_MATCH
        ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
            work->objs[0].pos.x - float_16_80420c44, float_20_80420c48 + work->objs[0].pos.y,
            float_2_80420c4c + work->objs[0].pos.z, work->objs[0].scale.x, work->objs[0].scale.y,
            0x27, work->objs[0].color, 0);
#else
        btlDispTexPlane(0x27, work->objs[0].color, work->objs[0].pos.x - float_16_80420c44,
                        float_20_80420c48 + work->objs[0].pos.y, float_2_80420c4c + work->objs[0].pos.z,
                        work->objs[0].scale.x, work->objs[0].scale.y);
#endif
    }

    if (work->objs[1].flag != 0) {
        PSMTXIdentity(mtxMain);
        PSMTXTrans(mtxTrans, work->objs[1].pos.x, work->objs[1].pos.y, work->objs[1].pos.z);
        PSMTXScale(mtxScale, work->objs[1].scale.x, work->objs[1].scale.y, work->objs[1].scale.z);
        PSMTXRotRad(mtxRotX, 0x78, float_deg2rad_80420c40 * work->objs[1].rot.x);
        PSMTXRotRad(mtxRotY, 0x79, float_deg2rad_80420c40 * work->objs[1].rot.y);
        PSMTXRotRad(mtxRotZ, 0x7a, float_deg2rad_80420c40 * work->objs[1].rot.z);
        PSMTXConcat(mtxRotZ, mtxScale, mtxMain);
        PSMTXConcat(mtxRotZ, mtxMain, mtxMain);
        PSMTXConcat(mtxRotX, mtxMain, mtxMain);
        PSMTXConcat(mtxRotY, mtxMain, mtxMain);
        PSMTXConcat(mtxTrans, mtxMain, mtxMain);
        btlDispTexPlane2(mtxMain, 0x25, work->objs[1].color, 0);
#ifdef MUST_MATCH
        ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
            work->objs[1].pos.x - float_16_80420c44, float_20_80420c48 + work->objs[1].pos.y,
            float_2_80420c4c + work->objs[1].pos.z, work->objs[1].scale.x, work->objs[1].scale.y,
            0x28, work->objs[1].color, 0);
#else
        btlDispTexPlane(0x28, work->objs[1].color, work->objs[1].pos.x - float_16_80420c44,
                        float_20_80420c48 + work->objs[1].pos.y, float_2_80420c4c + work->objs[1].pos.z,
                        work->objs[1].scale.x, work->objs[1].scale.y);
#endif
    }

    if (work->objs[2].flag != 0) {
        PSMTXIdentity(mtxMain);
        PSMTXTrans(mtxTrans, work->objs[2].pos.x, work->objs[2].pos.y, work->objs[2].pos.z);
        PSMTXScale(mtxScale, work->objs[2].scale.x, work->objs[2].scale.y, work->objs[2].scale.z);
        PSMTXRotRad(mtxRotX, 0x78, float_deg2rad_80420c40 * work->objs[2].rot.x);
        PSMTXRotRad(mtxRotY, 0x79, float_deg2rad_80420c40 * work->objs[2].rot.y);
        PSMTXRotRad(mtxRotZ, 0x7a, float_deg2rad_80420c40 * work->objs[2].rot.z);
        PSMTXConcat(mtxRotZ, mtxScale, mtxMain);
        PSMTXConcat(mtxRotZ, mtxMain, mtxMain);
        PSMTXConcat(mtxRotX, mtxMain, mtxMain);
        PSMTXConcat(mtxRotY, mtxMain, mtxMain);
        PSMTXConcat(mtxTrans, mtxMain, mtxMain);
        btlDispTexPlane2(mtxMain, 0x26, work->objs[2].color, 0);
#ifdef MUST_MATCH
        ((void (*)(f32, f32, f32, f32, f32, s32, GXColor, s32))btlDispTexPlane)(
            work->objs[2].pos.x - float_16_80420c44, float_20_80420c48 + work->objs[2].pos.y,
            float_2_80420c4c + work->objs[2].pos.z, work->objs[2].scale.x, work->objs[2].scale.y,
            0x29, work->objs[2].color, 0);
#else
        btlDispTexPlane(0x29, work->objs[2].color, work->objs[2].pos.x - float_16_80420c44,
                        float_20_80420c48 + work->objs[2].pos.y, float_2_80420c4c + work->objs[2].pos.z,
                        work->objs[2].scale.x, work->objs[2].scale.y);
#endif
    }

    for (i = 0; i < 3; i++) {
        obj = &work->objs[i];

        if (obj->flag == 0) {
            continue;
        }
        if (obj->state == 1) {
            if (obj->value == obj->max) {
                continue;
            }
            pos2 = obj->pos;
            pos2.x -= float_8_80420c50; pos2.x -= float_6_80420c54;
            pos2.y = pos2.y - (float_96_80420c58 * obj->scale.y + float_2_80420c4c);
            pos2.z = pos2.z + float_5_80420c5c;
            PSMTXScale(mtxS2, float_0p8_80420c60, float_0p8_80420c60, float_1_80420c64);
            PSMTXTrans(mtxT2, pos2.x, pos2.y, pos2.z);
            PSMTXConcat(mtxT2, mtxS2, mtxT2);
            iconNumberDispGx3D(mtxT2, obj->value, 0, obj->color);

            pos3 = obj->pos;
            pos3.x = pos3.x + float_1p2_80420c68 * (f32)(_keta_get(obj->max) * 8 + 6);
            pos3.y = pos3.y - (float_96_80420c58 * obj->scale.y + float_3_80420c6c);
            pos3.z = pos3.z + float_5_80420c5c;
            PSMTXTrans(mtxT3, pos3.x, pos3.y, pos3.z);
            PSMTXScale(mtxS2, float_1p2_80420c68, float_1p2_80420c68, float_1_80420c64);
            PSMTXConcat(mtxT3, mtxS2, mtxT3);
            iconNumberDispGx3D(mtxT3, obj->max, 0, obj->color);

            pos = obj->pos;
            pos.y = pos.y - float_96_80420c58 * obj->scale.y;
            pos.z = pos.z + float_5_80420c5c;
            PSMTXTrans(mtxT4, pos.x, pos.y, pos.z);
            PSMTXScale(mtxS2, float_neg0p6_80420c70, float_0p6_80420c74, float_1_80420c64);
            PSMTXConcat(mtxT4, mtxS2, mtxT4);
            iconDispGxCol(mtxT4, 0, 0x163, obj->color);
        } else if (obj->state == 2 || obj->state == 5) {
            pos3 = obj->pos;
            pos3.x = pos3.x + ((f32)(_keta_get(obj->max) * 8 / 2) - float_4_80420c78);
            pos3.y = pos3.y - (float_96_80420c58 * obj->scale.y + float_2_80420c4c);
            pos3.z = pos3.z + float_5_80420c5c;
            PSMTXTrans(mtxT3, pos3.x, pos3.y, pos3.z);
            iconNumberDispGx3D(mtxT3, obj->max, 0, obj->color);
        }
    }
}
// Reset at the end of the file: with -inline deferred the setting in force at end of file decides.
#pragma dont_inline reset
