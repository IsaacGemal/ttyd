#pragma once

#include "dolphin/types.h"
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"
#include "tou_03.h"

typedef void (*voidfunctionptr)(void);

typedef struct RuleEntry {
    const char* field_00; // 0x0
    const char* msg;      // 0x4
    s32 field_08;         // 0x8
    s32 field_0C;         // 0xC
    s32 field_10;         // 0x10
} RuleEntry;

typedef struct DispWork {
    f32 field_00; // 0x0
    f32 field_04; // 0x4
    s32 field_08; // 0x8
} DispWork;

typedef struct BeltData {
    u8 pad_00[8]; // 0x0
    Vec pos;      // 0x8
    u8 pad_14[8]; // 0x14
    f32 field_1C; // 0x1C
} BeltData;

typedef struct PoseUser {
    Vec* locators; // 0x0
} PoseUser;

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

extern BattleSetupData btlsetup_tou_tbl[26];
extern u32 btl_tou_setup_no_tbl[52];

USER_FUNC(tou_champion_belt_onoff_28_text_29DC);
USER_FUNC(tou_champion_belt_28_text_2A10);
USER_FUNC(evt_tou_set_rule_result_28_text_355C);
USER_FUNC(evt_tou_set_ranking_win_lose_28_text_35C0);
USER_FUNC(evt_tou_get_fighter_tribe);
USER_FUNC(evt_tou_get_fighter_num);
USER_FUNC(evt_tou_get_fighter_battle_msg_28_text_3CB4);
USER_FUNC(evt_tou_get_fighter_name2_28_text_3DD4);
USER_FUNC(evt_tou_get_fighter_info_28_text_3E58);
USER_FUNC(evt_tou_get_ranking_28_text_4048);
USER_FUNC(unk_tou2_00004b50);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
