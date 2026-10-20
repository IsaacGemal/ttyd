#pragma once

#include <dolphin/types.h>

typedef struct BattleWork BattleWork;

void BattleSequenceManager(void);
BOOL BattleCheckConcluded(BattleWork* wp);
void battleSortPhaseMoveTable(BattleWork* wp, s32 mode);
void BattleCheckAllPinchStatus(BattleWork* wp, BOOL a2);
void btlseqInit(BattleWork* wp);
void btlseqFirstAct(BattleWork* wp);
void btlseqTurn(BattleWork* wp);
void btlseqEnd(BattleWork* wp);
s32 BattleWaitAllActiveEvtEnd_NoBgSetEndWait(BattleWork* wp);
struct BattleWeapon;
BOOL BattleWaitAllActiveEvtEnd(BattleWork* wp);
s32 _set_haikei_entry_scale(struct EventEntry* event, BOOL isFirstCall);
void _mapobj_data_touch_scale(s32 kind);
void battleMakePhaseEvtTable(BattleWork* wp);
void btlseqPhaseFirstProcess(BattleWork* wp);
void btlseqTurnFirstProcess(BattleWork* wp);
void btlseqAct(BattleWork* wp);
BOOL BattlePhaseEndCheck(void);
void btlseqMove(BattleWork* wp);
void btlseqPhase(BattleWork* wp);
void _rule_disp(void);
s32 _set_effect_luck(void);
struct BattleWeapon* _GetFirstAttackWeapon(s32 id);
extern const char str_SFX_REST_CONDITION1[];
extern const char str_SFX_CONDITION_BREAK_ICE1[];
