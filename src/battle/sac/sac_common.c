#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "battle/battle_event_cmd.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"

// ---- local prototypes (no header declares these yet) ----
void BattleAcHelpSetDispType(s32 type);
BOOL BattleAudienceSoundCheck(s32 index);
void BattleAudienceSoundStop(s32 index);
void BattleAudienceSoundCheer(s32 a, s32 b);
void BattleAudienceSoundSetVol(s32 index, u8 a2, s32 a3);
void BattleAudienceSoundHandBeat(void);
void BattleAudienceSoundClap(s32 a, s32 b);
void BattleAudienceSoundCheerKind(s32 kind, s32 a, s32 b);
void BattleAudienceSoundWhistleKind(s32 kind);

// Event command functions referenced by the script tables below. The ones not
// already in battle_event_cmd.h have no header yet.
USER_FUNC(evt_btl_camera_set_mode);
USER_FUNC(evt_btl_camera_set_homing_unit);
USER_FUNC(evt_btl_camera_set_zoom);
USER_FUNC(evt_btl_camera_set_moveSpeedLv);
USER_FUNC(evt_btl_camera_set_zoomSpeedLv);
USER_FUNC(btlevtcmd_CheckStatus);
USER_FUNC(btlevtcmd_AnimeChangePoseType);
USER_FUNC(btlevtcmd_FaceDirectionAdd);
USER_FUNC(btlevtcmd_MovePosition);
USER_FUNC(btlevtcmd_snd_se);
USER_FUNC(btlevtcmd_AnimeChangePose);
USER_FUNC(btlevtcmd_SetMoveSpeed);
USER_FUNC(evt_audience_sound_cheer);
USER_FUNC(evt_eff);
USER_FUNC(evt_eff_delete);

// ---- sbss globals ----
static s32 count;
s32 count2;
s32 handBeatEnable;
s32 cheerEnable;
u8 volPer;

// ---- .rodata strings ----
const char str_803004A0[0x18] = "SFX_VOICE_MARIO_HAPPY1";
const char str_803004B8[0x10] = "sac_treasure";
const char str_803004C8[0x10] = "treasure_map";
const char str_803004D8[0x0C] = "sac_itemget";
const char str_803004E4[0x08] = "itemget";
const char str_803004EC[0x0C] = "stardust";

// ---- .sdata2 constants ----
const char str_80421E30[] = "M_P_2";
const u32 zero_80421e38 = 0;

// ---- .data script tables ----
s32 sac_start_camera_evt[] = {
    0x0003005B, (s32)evt_btl_camera_set_mode, 0x00000000, 0x00000007,
    0x0004005B, (s32)evt_btl_camera_set_homing_unit, 0x00000000, 0xFFFFFFFE, 0xFFFFFFFF,
    0x0004005B, (s32)btlevtcmd_CheckStatus, 0xFFFFFFFE, 0x0000000A, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x00000001,
    0x0003005B, (s32)evt_btl_camera_set_zoom, 0x00000000, 0x00000064, 0x00000020,
    0x0003005B, (s32)evt_btl_camera_set_zoom, 0x00000000, 0x0000012C, 0x00000021,
    0x0003005B, (s32)evt_btl_camera_set_moveSpeedLv, 0x00000000, 0x00000002,
    0x0003005B, (s32)evt_btl_camera_set_zoomSpeedLv, 0x00000000, 0x00000002,
    0x00000002, 0x00000001,
};

s32 sac_mario_item_on_evt[] = {
    0x0004005B, (s32)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x00000028,
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (s32)btlevtcmd_FaceDirectionAdd, 0xFFFFFFFE, 0xFE363C80, 0x0000000F,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x00000005, 0xFFFFFFFF, 0x00000000,
    0x0006005B, (s32)btlevtcmd_snd_se, 0xFFFFFFFE, (s32)(char*)str_803004A0,
        0xF1194D80, 0x00000000, 0xF1194D80,
    0x0003005B, (s32)evt_audience_sound_cheer, 0x000000B4, 0x0000003C,
    0x0004005B, (s32)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (s32)(char*)str_80421E30,
    0x0006005B, (s32)btlevtcmd_GetPartsPos, 0xFFFFFFFE, 0x00000001,
        0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)btlevtcmd_GetHeight, 0xFFFFFFFE, 0xFE363C85,
    0x0003005B, (s32)btlevtcmd_GetStatusMg, 0xFFFFFFFE, 0xFE363C86,
    0x0002003C, 0xFE363C85, 0xFE363C86,
    0x00020035, 0xFE363C81, 0xFE363C85,
    0x00020035, 0xFE363C81, 0x00000023,
    0x00020035, 0xFE363C82, 0xFFFFFFFB,
    0x000F005B, (s32)evt_eff, (s32)(char*)str_803004B8, (s32)(char*)str_803004C8,
        0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00020035, 0xFE363C81, 0x00000002,
    0x00020035, 0xFE363C82, 0xFFFFFFF9,
    0x000F005B, (s32)evt_eff, (s32)(char*)str_803004D8, (s32)(char*)str_803004E4,
        0x00000009, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000002, 0x00000001,
};

s32 sac_start_stardust[] = {
    0x00010005, 0x00000006,
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020035, 0xFE363C81, 0x00000032,
    0x000F005B, (s32)evt_eff, (s32)(char*)&zero_80421e38, (s32)(char*)str_803004EC,
        0x00000005, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x0000001E, 0x0000001E, 0x0000000A, 0x0000001E,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00010009, 0x0000000A, 0x00000006,
    0x00000002, 0x00000001,
};

s32 sac_mario_back_home[] = {
    0x0004005B, (s32)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x00000028,
    0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x00000005, 0xFFFFFFFF, 0x00000000,
    0x00000002, 0x00000001,
};

s32 enemy_slide_go_evt[] = {
    0x0005005B, (s32)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020035, 0xFE363C80, 0x000001E0,
    0x0003005B, (s32)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A8E80,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x00000000, 0x00000000, 0x00000001,
    0x00000002, 0x00000001,
};

s32 enemy_slide_return_evt[] = {
    0x0005005B, (s32)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)btlevtcmd_SetMoveSpeed, 0xFFFFFFFE, 0xF24A9680,
    0x0008005B, (s32)btlevtcmd_MovePosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
        0x00000000, 0x00000000, 0x00000001,
    0x00000002, 0x00000001,
};

// ---- .sdata script table ----
s32 sac_mario_item_on_end_evt[] = {
    0x0002005B, (s32)evt_eff_delete, (s32)(char*)str_803004D8,
    0x0002005B, (s32)evt_eff_delete, (s32)(char*)str_803004B8,
    0x00000002, 0x00000001,
};
// .text:0x0 | 0x80242B84 | size: 0x28
USER_FUNC(sac_ac_help_off) {
    BattleAcHelpSetDispType(0);
    return 2;
}

// .text:0x28 | 0x80242BAC | size: 0x28
USER_FUNC(sac_ac_help_on) {
    BattleAcHelpSetDispType(2);
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

// .text:0x140 | 0x80242CC4 | size: 0x10
USER_FUNC(sac_cheer_end) {
    cheerEnable = 0;
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

// .text:0x2AC | 0x80242E30 | size: 0x10
USER_FUNC(sac_handbeat_end) {
    handBeatEnable = 0;
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

