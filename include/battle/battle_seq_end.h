#pragma once

typedef struct EventEntry EventEntry;

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>

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
void L__LvupParamHelpMsgDisp(CameraId camId, void* param);
void _lvup_select_object_disp(CameraId camId, void* param);
void btlseqEnd_DispMain(CameraId camId, BattleWork* wp);
void btlseqEnd_DispInit(BattleWork* wp);
void _audience_Whistle_control(void);
void _check_audience_battle_end_exec(void);
void _ExecAllUnitBattleEndEvent(void);
void _BattleMarioKouraDelete(void);
const char* BattleGetRankNameLabel(s32 level);
