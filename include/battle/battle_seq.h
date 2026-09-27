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
