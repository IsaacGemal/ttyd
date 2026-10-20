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

// .sdata2 constants: declared const here, defined at the end of battle_break_slot.c so the compiler loads them
// by name instead of folding them into its own literal pool.
extern const f32 zero_80420520;
extern const u32 dat_80420524;
extern const u32 dat_80420528;
extern const u32 dat_8042052c;
extern const f32 float_226_80420530;
extern const f32 float_42_80420534;
extern const f32 float_122_80420538;
extern const f32 float_0_8042053c;
extern const f32 float_deg2rad_80420540;
extern const f32 float_neg32_80420544;
extern const f32 float_neg128_80420548;
extern const f32 float_490_8042054c;
extern const f32 float_neg1_80420550;
extern const f32 float_0p2_80420554;
extern const f32 float_0p15_80420558;
extern const f32 float_32767_8042055c;
