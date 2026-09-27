#pragma once

#include <dolphin/types.h>
#include "battle/battle.h"
#include "battle/battle_seq_command.h"

void SelectedItemCoordinateColorUpDate(void);
BOOL BattleMenuKeyOKInACT(BattleWork* wp);
void battleMenuDispInit(void);

void battleMenuDispEnd(void);
void BattleMenuDisp_ActSelect_Disp(BattleWork* wp);
void BattleMenuDisp_WeaponSelect_Disp(BattleWork* wp);
void BattleMenuDisp_Operation_Disp(BattleWork* wp);
void BattleMenuDisp_MultiItem_Disp(BattleWork* wp);
void BattleMenuDisp_ChangePartySelect_Disp(BattleWork* wp);
void BattleMenuDisp_ActSelect_Init(BattleWork* wp, BattleCursor* cursor, void* table, u32 flags);
void BattleMenuDisp_WeaponSelect_Init(BattleWork* wp, BattleCursor* cursor, void* table, u32 flags);
void BattleMenuDisp_Operation_Init(BattleWork* wp, u32 flags);
void BattleMenuDisp_MultiItem_Init(BattleWork* wp, u32 flags);
void BattleMenuDisp_ChangePartySelect_Init(BattleWork* wp, u32 flags);
