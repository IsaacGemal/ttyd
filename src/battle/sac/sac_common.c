#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "battle/battle_event_cmd.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include "battle/battle_camera.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "battle/battle_ac_help.h"

// ---- sbss globals ----
static s32 count;
s32 count2;
s32 handBeatEnable;
s32 cheerEnable;
u8 volPer;

// ---- .data script tables ----
// clang-format off
EVT_BEGIN(sac_start_camera_evt)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_btl_camera_set_zoom, 0, 100)
    ELSE
        CALL(evt_btl_camera_set_zoom, 0, 300)
    END_IF
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoomSpeedLv, 0, 2)
    RETURN
EVT_END()
// clang-format on

// clang-format off
EVT_BEGIN(sac_mario_item_on_evt)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 5, -1, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_HAPPY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_audience_sound_cheer, 180, 60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_P_2"))
    CALL(btlevtcmd_GetPartsPos, -2, 1, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(1), LW(5))
    ADD(LW(1), 35)
    ADD(LW(2), -5)
    CALL(evt_eff, STRING("sac_treasure"), STRING("treasure_map"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    ADD(LW(1), 2)
    ADD(LW(2), -7)
    CALL(evt_eff, STRING("sac_itemget"), STRING("itemget"), 9, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()
// clang-format on

// clang-format off
EVT_BEGIN(sac_start_stardust)
    LOOP(6)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 50)
        CALL(evt_eff, STRING(""), STRING("stardust"), 5, LW(0), LW(1), LW(2), 30, 30, 10, 30, 0, 0, 0, 0)
        WAIT_FRAMES(10)
    END_LOOP
    RETURN
EVT_END()
// clang-format on

// clang-format off
EVT_BEGIN(sac_mario_back_home)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 5, -1, 0)
    RETURN
EVT_END()
// clang-format on

// clang-format off
EVT_BEGIN(enemy_slide_go_evt)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 480)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 1)
    RETURN
EVT_END()
// clang-format on

// clang-format off
EVT_BEGIN(enemy_slide_return_evt)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(7.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 1)
    RETURN
EVT_END()
// clang-format on

// ---- .sdata script table ----
// clang-format off
EVT_BEGIN(sac_mario_item_on_end_evt)
    CALL(evt_eff_delete, STRING("sac_itemget"))
    CALL(evt_eff_delete, STRING("sac_treasure"))
    RETURN
EVT_END()
// clang-format on

// .text:0x3F8 | 0x80242F7C | size: 0x7C
USER_FUNC(sac_wao) {
    s32 num = BattleAudience_GetAudienceNum();

    BattleAudienceSoundClap(0x78, 0x78);
    if (num >= 1 && num <= 0x95) {
        BattleAudienceSoundCheerKind(1, 0x78, 0x78);
    }
    if (num >= 0x96) {
        BattleAudienceSoundCheerKind(4, 0x78, 0x78);
        BattleAudienceSoundWhistleKind(1);
    }
    return 2;
}

// .text:0x2BC | 0x80242E40 | size: 0x13C
s32 sac_handbeat(EventEntry* event, BOOL isFirstCall) {
    if (isFirstCall) {
        count = 0;
        count2 = 0x3C;
        handBeatEnable = 1;
        volPer = 0;
        if (BattleAudienceSoundCheck(4) == 1) {
            BattleAudienceSoundStop(4);
        }
        if (BattleAudienceSoundCheck(5) == 1) {
            BattleAudienceSoundStop(5);
        }
        BattleAudienceSoundCheer(-1, 0);
        BattleAudienceSoundSetVol(4, 0, 1);
    }
    BattleAudienceSoundSetVol(4, volPer, 1);
    if (handBeatEnable == 0) {
        BattleAudienceSound* ptr = BattleAudienceSoundGetPtr(4);
        *(s32*)((u8*)ptr + 0xC) = 0xB4;
        ptr->mFadeoutTime = 0xB4;
        return 2;
    }
    if (count < 0) {
        BattleAudienceSoundHandBeat();
        count = count2;
        count += irand(2);
        count2 -= 4;
        if (count2 < 0x14) {
            count2 = 0x14;
        }
        volPer += 5;
        if ((u8)volPer > 0x64) {
            volPer = 0x64;
        }
    }
    count--;
    return 0;
}

// .text:0x2AC | 0x80242E30 | size: 0x10
USER_FUNC(sac_handbeat_end) {
    handBeatEnable = 0;
    return 2;
}

// .text:0x150 | 0x80242CD4 | size: 0x15C
s32 sac_cheer(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    if (isFirstCall) {
        count = 0;
        count2 = 0x3C;
        cheerEnable = 1;
        volPer = 0;
        if (BattleAudienceSoundCheck(4) == 1) {
            BattleAudienceSoundStop(4);
        }
        if (BattleAudienceSoundCheck(5) == 1) {
            BattleAudienceSoundStop(5);
        }
        BattleAudienceSoundCheer(-1, 0);
        BattleAudienceSoundSetVol(4, 0, 1);
    }
    if (value == 0) {
        BattleAudienceSoundSetVol(4, volPer, 1);
    }
    if (cheerEnable == 0) {
        BattleAudienceSound* ptr = BattleAudienceSoundGetPtr(4);
        *(s32*)((u8*)ptr + 0xC) = 0xB4;
        ptr->mFadeoutTime = 0xB4;
        return 2;
    }
    if (count < 0) {
        count = count2;
        count += irand(2);
        count2 -= 4;
        if (count2 < 0x14) {
            count2 = 0x14;
        }
        volPer += 5;
        if ((u8)volPer > 0x64) {
            volPer = 0x64;
        }
    }
    count--;
    return 0;
}

// .text:0x140 | 0x80242CC4 | size: 0x10
USER_FUNC(sac_cheer_end) {
    cheerEnable = 0;
    return 2;
}

// .text:0xC8 | 0x80242C4C | size: 0x78
USER_FUNC(sac_enemy_slide_go) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        if (unit != NULL && unit->mAlliance == 1) {
            EventEntry* event = evtEntry(enemy_slide_go_evt, 0, 0x20);
            event->unitId = i;
        }
    }
    return 2;
}

// .text:0x50 | 0x80242BD4 | size: 0x78
USER_FUNC(sac_enemy_slide_return) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        if (unit != NULL && unit->mAlliance == 1) {
            EventEntry* event = evtEntry(enemy_slide_return_evt, 0, 0x20);
            event->unitId = i;
        }
    }
    return 2;
}

// .text:0x28 | 0x80242BAC | size: 0x28
USER_FUNC(sac_ac_help_on) {
    BattleAcHelpSetDispType(2);
    return 2;
}

// .text:0x0 | 0x80242B84 | size: 0x28
USER_FUNC(sac_ac_help_off) {
    BattleAcHelpSetDispType(0);
    return 2;
}
