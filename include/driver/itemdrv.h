#pragma once

#include "mario/mario_pouch.h"

#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>

typedef struct ItemEntry {
	u16 flags; //0x0
	u16 field_0x2; //0x2
	s32 itemId; //0x4
	s32 evtId; //0x8, TODO: better name?
	char name[0x10]; //0xC
	EffectEntry* effect; //0x1C
	u8 field_0x20[0x24 - 0x20]; //0x20
	u16 mode; //0x24
	u16 field_0x26; //0x26
	u8 field_0x28[0x3C - 0x28]; //0x28
	Vec position; //0x3C
	f32 scale; //0x48
	u8 alpha; //0x4C, TODO: "icon_alpha"?
	u8 field_0x4D[0x68 - 0x4D]; //0x4D
	OSTime field_0x68; //0x68
	u8 field_0x70[0x98 - 0x70]; //0x70
} ItemEntry;

typedef struct ItemWork {
	s32 count; //0x0
	ItemEntry* entries; //0x4
	u32 currId; //0x8, next available itemId
	u8 field_0xC[0x1C - 0xC]; //0xC
} ItemWork;

void itemInit(void);
void itemReInit(void);
void itemMain(void);
ItemEntry* itemNameToPtr(char*);
void itemSetPosition(ItemEntry* item, f32 x, f32 y, f32 z);
void itemDelete(char* name);
s32 N_itemPickUpFromFieldCheck(void);
s32 itemGetNokoCheck(ItemEntry* item);
ItemEntry* itemEntry(const char* name, ItemType type, u16 a3, s32 evtId, s32 a5, f32 a6, f32 a7, f32 a8);
void itemFlagOn(ItemEntry* item, u16 flag);
s32 itemStatus(ItemEntry* item, u32 mask);
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
// 2nd s32 arg (unused by the callee) is required by itemMain/evt_eff call sites.
EffectEntry* effHagetakaFlushN64Entry(s32 type, s32 unk, f32 x, f32 y, f32 z, f32 a5);
void itemCoinDrop(Vec* pos);
