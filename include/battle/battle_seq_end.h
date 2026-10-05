#pragma once

typedef struct EventEntry EventEntry;

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>

// .sdata2 white used by _lvup_spot_on's entry->field_28.
extern const u32 dat_80420c28;

typedef struct RankUpData {
    s16 startLevel; //0x0, the level you need to reach to be a certain rank
    s16 rankId; //0x2, the index of this rank for lookups
    const char* rankUpName; //0x4, name of the rank for pause menu
    const char* rankUpMessage; //0x8, text box when you rank up
} RankUpData;

typedef struct BattleSeqEndWork {
    u8 unk0[0x2AC - 0x0]; //0x0
} BattleSeqEndWork;
void _GetExpIcon_Main(BattleWork* wp);
void _GetExpIcon_End(BattleSeqEndWork* work);
void _GetExpIcon_Init(BattleSeqEndWork* arg);
void _LvupParamConfirmDisp(CameraId camId, void* param);
void _MarioExpDisp(CameraId camId, void* param);
void _GetExpIcon_Disp(CameraId camId, void* param);
void L__LvupParamHelpMsgDisp(CameraId camId, void* param);
void _lvup_select_object_disp(CameraId camId, void* param);
void btlseqEnd_DispMain(CameraId camId, BattleWork* wp);
void btlseqEnd_DispInit(BattleWork* wp);
void _audience_Whistle_control(void);
void _check_audience_battle_end_exec(void);
void _ExecAllUnitBattleEndEvent(void);
void _BattleMarioKouraDelete(void);
const char* BattleGetRankNameLabel(s32 level);
s32 _lvup_disp_flag_onoff(EventEntry* event, BOOL isFirstCall);
s32 _lvup_select_obj_disp_on(EventEntry* event, BOOL isFirstCall);
s32 _lvup_select_obj_lvup_param_disp(EventEntry* event, BOOL isFirstCall);
s32 _lvup_object_noselect_delete(EventEntry* event, BOOL isFirstCall);
s32 _lvup_object_select_move(EventEntry* event, BOOL isFirstCall);
s32 _lvup_object_decide_bound(EventEntry* event, BOOL isFirstCall);
s32 _lvup_obj_set_color(EventEntry* event, BOOL isFirstCall);
s32 _get_rank_up_msg(EventEntry* event, BOOL isFirstCall);
s32 _nozzle_set_offset(EventEntry* event, BOOL isFirstCall);
s32 _set_confetti_control_evtid(EventEntry* event, BOOL isFirstCall);
s32 _lvup_spot_on(EventEntry* event, BOOL isFirstCall);
s32 _lvup_spot_move(EventEntry* event, BOOL isFirstCall);
s32 _disable_enemy_unit_disp(EventEntry* event, BOOL isFirstCall);
s32 _lvup_select_object_init(EventEntry* event, BOOL isFirstCall);
s32 _lvup_spot_off(EventEntry* event, BOOL isFirstCall);
s32 _lvup_ap_recover_full(void);
