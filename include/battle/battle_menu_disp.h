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
void DrawMultiItemWin(void);
void DrawChangePartyWin(void);
void DrawOperationWin(void);
void DrawWeaponWin(void);
void DrawMainMenu(void);
void BattleMenuDisp_ActSelect_End(BattleWork* wp);
void BattleMenuDisp_WeaponSelect_End(BattleWork* wp);
void BattleMenuDisp_Operation_End(BattleWork* wp);
void BattleMenuDisp_MultiItem_End(BattleWork* wp);
void BattleMenuDisp_ChangePartySelect_End(BattleWork* wp);

typedef struct BattleMenuWork BattleMenuWork;
void InitSubMenuCommonProcess(BattleMenuWork** window, BattleCursor** cursor, BattleCursor* menu);
void InitSubMenuCommonProcess2(BattleMenuWork* window, BattleCursor* cursor);
void InitSubMenuCommonProcess3(BattleMenuWork* window, BattleCursor* cursor);
void DrawMenuPartyChangeButton_Sub(u32 tex, f32 x, f32 y, f32 angle);
void DrawMenuPartyChangeButton(void);
void DrawMenuCursorAndScrollArrow(Vec* pos);
