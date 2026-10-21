#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>

void BattleBreakSlot_Init(void);
void BattleBreakSlot_Main(void);
void BattleBreakSlot_End(void);
s32 BattleBreakSlot_GetBreakTurn(void);
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
typedef struct BattleBreakSlot BattleBreakSlot;
typedef struct BattleBreakSlotReel BattleBreakSlotReel;

struct BattleBreakSlotReel {
    s32 type;       // 0x00
    u32 flags;      // 0x04
    s32 state;      // 0x08
    s32 stateTimer; // 0x0C
    s32 value;      // 0x10
    s32* cards;     // 0x14
    s32 count;      // 0x18
    s32 index;      // 0x1C
    Vec pos;        // 0x20
    Vec rot;        // 0x2C
    Vec scale;      // 0x38
    Vec pivot;      // 0x44
    f32 scrollStep; // 0x50
    f32 scrollDelta; // 0x54
    u8 alpha;      // 0x58
    u8 field_59[3]; // 0x59
};

struct BattleBreakSlot {
    u32 flags;      // 0x00
    s32 state;      // 0x04
    s32 timer;      // 0x08
    s32 counter;    // 0x0C
    s32 breakTurn;  // 0x10
    f32 field_14;   // 0x14
    BattleBreakSlotReel reels[3]; // 0x18
    s32 sound;      // 0x12C
    EventEntry* event; // 0x130
};

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
