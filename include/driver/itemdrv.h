#pragma once

#include "mario/mario_pouch.h"

#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>

typedef struct ItemEntry {
	u16 flags; //0x0
	u16 spawnType; //0x2, the a3 argument of itemEntry; its switch sets the starting mode
	s32 itemId; //0x4
	s32 evtId; //0x8, TODO: better name?
	char name[0x10]; //0xC
	EffectEntry* effect; //0x1C
	void* getScript; //0x20, the a5 argument of itemEntry; evtEntry'd when the item is picked up (itemseq_tryGet)
	u16 mode; //0x24
	u16 subMode; //0x26, step inside the current mode (itemseq_GetItem's switch)
	s32 winMainId; //0x28, WinMgr entry of the item-name window (winNameDisp, winNameDisp2)
	s32 winHelpId; //0x2C, WinMgr entry of the help window (winHelpDisp)
	s32 winFullId; //0x30, WinMgr entry of the pouch-full window (winFullDisp)
	u8 pad34[4]; //0x34
	u32 status; //0x38, status bits (itemStatus, itemStatusOn)
	Vec position; //0x3C
	f32 scale; //0x48
	u8 alpha; //0x4C, TODO: "icon_alpha"?
	u8 pad4D[3]; //0x4D
	f32 moveSpeed; //0x50, horizontal speed of a bounce; evt_item_set_move_dir_speed
	s32 moveDir; //0x54, heading in degrees; evt_item_set_move_dir_speed
	f32 jumpPower; //0x58, initial vertical speed; evt_item_set_jump_power
	u8 pad5C[4]; //0x5C
	s64 bounceTime; //0x60, time since the current arc started (reset on each landing and ceiling hit)
	OSTime lastTime; //0x68, render time at the previous update (itemMain and itemseq_Bound)
	s64 curTime; //0x70, render time of this bounce step; curTime - lastTime is the step length
	s64 lifeTime; //0x78, remaining life in ms, 0 for no timer; blinks at or below 3000 (itemMain)
	f32 boundRate; //0x80, factor applied to the vertical speed on each landing; evt_item_set_bound_rate
	f32 gravity; //0x84, gravity multiplier (-980 * gravity); evt_item_set_gravity
	s32 boundLimit; //0x88, landings allowed before the item stops (0 = no limit); evt_item_set_bound_limit
	s32 boundCount; //0x8C, landings so far
	s32 boundNextDir; //0x90, moveDir taken over while boundCount <= 1 (flag 0x200); evt_item_set_bound_next_dir
	f32 boundNextSpeed; //0x94, moveSpeed taken over while boundCount <= 1 (flag 0x200); evt_item_set_bound_next_speed
} ItemEntry;

typedef struct ItemWork {
	s32 count; //0x0
	ItemEntry* entries; //0x4
	u32 currId; //0x8, next available itemId
	EffectEntry* getEffect; //0xC, effect shown while an item is being picked up (itemseq_GetItem)
	struct WinMgrSelect* getSelect; //0x10, replace-an-item choice window (itemseq_GetItem subMode 5, pouchRemoveItem)
	struct EventEntry* getEvent; //0x14, lecture event for a juyo item (itemseq_GetItem subMode 6)
	s32 getMsgId; //0x18, first-hit message window (itemseq_GetItem subMode 3 and 4)
} ItemWork;

void itemInit(void);
void itemReInit(void);
void itemMain(void);
ItemEntry* itemNameToPtr(char*);
void itemSetPosition(ItemEntry* item, f32 x, f32 y, f32 z);
void itemDelete(char* name);
s32 N_itemPickUpFromFieldCheck(void);
s32 itemGetNokoCheck(ItemEntry* item);
ItemEntry* itemEntry(const char* name, ItemType type, u16 a3, f32 a6, f32 a7, f32 a8, s32 evtId, s32 a5);
void itemFlagOn(ItemEntry* item, u16 flag);
u32 itemStatus(ItemEntry* item, u32 mask);
void itemStatusOn(ItemEntry* item, u32 mask);
void itemFlagOff(ItemEntry* item, u16 flag);
u16 itemFlag(ItemEntry* item, u16 flag);
void itemModeChange(ItemEntry* item, u16 mode);
void itemForceGet(ItemEntry* item);
ItemEntry* itemNearDistCheck(f32 x, f32 y, f32 z, f32 maxDist);
ItemEntry* itemHitCheck(f32 x, f32 y, f32 z, f32 maxDist);
void itemNokoForceGet(ItemEntry* item);
void itemPickUp(ItemEntry* item);
#include "driver/hitdrv.h"
HitEntry* itemHitCheckSide(ItemEntry* item, f32* x, f32* z, s32* angle, f32 dx, f32 dz);
s32 itemGetCheck(ItemEntry* entry);
void itemseq_Bound(ItemEntry* item);
void itemseq_sfxGet(ItemEntry* item);
void itemseq_sfxBound(ItemEntry* item, Vec* pos);
HitEntry* itemBoundHitFloor(ItemEntry* item, f32 vy);
HitEntry* itemBoundHitCeiling(ItemEntry* item, f32 vy);
void itemseq_GetItem(ItemEntry* item);
void itemCoinDrop(Vec* pos);
