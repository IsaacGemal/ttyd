#pragma once

#include <dolphin/types.h>

void BattleBreakSlot_Init(void);
void BattleBreakSlot_Main(void);
void BattleBreakSlot_End(void);
s32 BattleBreakSlot_GetBreakTurn(void);
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
typedef struct BattleBreakSlot BattleBreakSlot;
typedef struct BattleBreakSlotReel BattleBreakSlotReel;
void BattleBreakSlot_HideReel(void);
void BattleBreakSlot_DecBreakTurn(void);
s32 BattleBreakSlot_CheckReaction(void);
void BattleBreakSlot_Start(void);
void BattleBreakSlot_PointInc(void);
void BattleBreakSlotDispReel(CameraId camId, void* param);
void BattleBreakSlot_Disp(void);
BattleBreakSlot* BattleBreakSlotGetPtr(void);
BattleBreakSlotReel* BattleBreakSlotReelGetPtr(s32 index);
void item_appear_force(BattleBreakSlotReel* reel, s32 id);
