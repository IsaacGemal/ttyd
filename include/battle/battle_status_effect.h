#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

typedef struct BattleUnit BattleUnit;

#include <dolphin/types.h>

//battle_status_effect
//should be an s8, typecasting for now
typedef enum StatusEffectType {
	STATUS_ALLERGIC, //0x0
	STATUS_SLEEP, //0x1
	STATUS_STOP, //0x2
	STATUS_DIZZY, //0x3
	STATUS_POISON, //0x4
	STATUS_CONFUSE, //0x5
	STATUS_ELECTRIC, //0x6
	STATUS_DODGY, //0x7
	STATUS_BURN, //0x8
	STATUS_FREEZE, //0x9
	STATUS_HUGE, //0xA
	STATUS_TINY, //0xB
	STATUS_ATTACK_UP, //0xC
	STATUS_ATTACK_DOWN, //0xD
	STATUS_DEFENSE_UP, //0xE
	STATUS_DEFENSE_DOWN, //0xF
	STATUS_CHARGE, //0x10
	STATUS_FLIPPED, //0x11
	STATUS_INVISIBLE, //0x12
	STATUS_FAST, //0x13
	STATUS_SLOW, //0x14
	STATUS_PAYBACK, //0x15
	STATUS_HOLD_FAST, //0x16
	STATUS_HP_REGEN, //0x17
	STATUS_FP_REGEN, //0x18
	STATUS_FRIGHT, //0x19
	STATUS_GALE_FORCE, //0x1A
	STATUS_INSTANT_KILL, //0x1B
	STATUS_INVALID //0x1C
} StatusEffectType;

void BattleStatusEffectInit(struct BattleUnit* unit);
void BattleStatusEffectMain(struct BattleUnit* unit);
void BSE_TurnFirstProcessEffectEntry(struct BattleUnit* unit, s16 active);
BOOL BSE_TurnFirstProcessEffectMain(struct BattleUnit* unit);
void BattleStatusChangeInfoWorkInit(struct BattleUnit* unit);








void BattleStatusChangeMsgWorkInit(void);
s32 _get_pri(s8 type);
void BSE_Sleep(BattleUnit* unit);
void BSE_SleepDelete(BattleUnit* unit);
void BSE_Biribiri(BattleUnit* unit);
void BSE_BiribiriDelete(BattleUnit* unit);
void BSE_Fire(BattleUnit* unit);
void BSE_FireDelete(BattleUnit* unit);
void BSE_Freeze(BattleUnit* unit);
void BSE_FreezeDelete(BattleUnit* unit);
void BSE_Kagegakure(BattleUnit* unit);
void BSE_KagegakureDelete(BattleUnit* unit);
void BattleStatusChangeInfoSetAnnouce(BattleUnit* unit, s8 type, s8 p1, s32 p2);
void BattleStatusChangeMsgSetAnnouce(BattleUnit* unit, s32 type, s32 msg);
void BattleStatusChangeMsgAdjust(BattleWork* wp);
BOOL BattleStatusChangeMsgMain(BattleWork* wp);
BOOL BattleStatusChangeAnnouceMain(BattleWork* wp);
BOOL BattleStatusChangeAnnouceMain_Unit(BattleUnit* unit);
void _st_chg_msg_disp(CameraId camera, void* param);
void BattleStatusEffectDelete(BattleUnit* unit);
