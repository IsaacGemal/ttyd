#include "battle/battle_icon.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "data/item_data.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <string.h>
#include <stdio.h>

extern BattleWork* _battleWorkPointer;
extern ItemData itemDataTable[];

// dont_inline keeps BtlIconGetPtr a real `bl` from the evt commands as in retail.
#pragma dont_inline on
// The original keeps this 16-iteration loop rolled; at -O4,p the compiler unrolls it 4x.
#pragma optimize_for_size on

BattleIcon* BtlIconGetPtr(s32 index) {
	BattleIcon* icon;
	int i;

	for (i = 0; i < 16; i++) {
		icon = &_battleWorkPointer->icons[i];
#ifdef MUST_MATCH
		// BattleWorkIcon.flags is u16 in the original (lhz); the header declares it s16, so reinterpret
		// to emit the unsigned load without editing the existing header line.
		if (*(u16*)&icon->flags & 1 && icon->index == index) {
#else
		if (icon->flags & 1 && icon->index == index) {
#endif
			return icon; //TODO: redo bwp->iconWork?
		}
	}
	return NULL;
}
#pragma optimize_for_size reset
#pragma dont_inline reset

void BattleIconInit(void) {
	memset(_battleWorkPointer->icons, 0, sizeof(_battleWorkPointer->icons));
}

// The icon's base must be reloaded each use; caching it makes the compiler keep an
// extra register. flags is u16 (lhz).
void BattleIconMain(void) {
	BattleWork* wp;
	BattleIcon* icon;
	int i;

	wp = _battleWorkPointer;
	for (i = 0; i < 16; i++) {
		icon = &wp->icons[i];
#ifdef MUST_MATCH
		// flags is u16 in the original (lhz); the header declares it s16, so reinterpret
		// to emit the unsigned load without editing the existing header line.
		if (*(u16*)&icon->flags & 1) { //in use
#else
		if (icon->flags & 1) { //in use
#endif
			if (icon->base) {
				iconSetPos(icon->base->name, icon->position.x, icon->position.y, icon->position.z);
				iconSetScale(icon->base->name, icon->field_0x14 * icon->field_0x18);
#ifdef MUST_MATCH
				if (*(u16*)&icon->flags & 2) {
#else
				if (icon->flags & 2) {
#endif
					iconFlagOn(icon->base->name, 2);
				}
				else {
					iconFlagOff(icon->base->name, 2);
				}
			}
		}
	}
}

void BattleIconEnd(void) {
	BattleWork* wp = _battleWorkPointer;
	int i;

	for (i = 0; i < 16; i++) {
		BtlIcon_Delete(&wp->icons[i]);
	}
}

// dont_inline keeps BtlIcon_Entry a real `bl` from the evt commands as in retail.
#pragma dont_inline on

// name[8] is what the original reserves for "_biu_%02x" (8 bytes with the null);
// a larger buffer grows the frame past the original's 0x50.
BattleIcon* BtlIcon_Entry(s16 iconId, f32 x, f32 y, f32 z) {
	BattleIcon* icon;
	int i;
	char name[8];

	for (i = 0; i < 16; i++) {
		icon = &_battleWorkPointer->icons[i];
#ifdef MUST_MATCH
		// flags is u16 in the original (lhz); the header declares it s16, so reinterpret
		// to emit the unsigned load without editing the existing header line.
		if (!(*(u16*)&icon->flags & 1)) {
#else
		if (!(icon->flags & 1)) {
#endif
			break; //found one not in use
		}
	}
	if (i >= 16) {
		return NULL;
	}
	memset(icon, 0, sizeof(BattleIcon));
	icon->index = i;
	icon->flags = 1;
	icon->field_0x14 = 1.0f;
	icon->field_0x18 = 1.0f;
	icon->position.x = x;
	icon->position.y = y;
	icon->position.z = z;
	icon->iconId = iconId;
	sprintf(name, "_biu_%02x", icon->index);
#ifdef MUST_MATCH
	// Same s16/u16 issue: the original loads iconId with lhz.
	iconEntry(name, *(u16*)&icon->iconId);
#else
	iconEntry(name, icon->iconId);
#endif
	icon->base = iconNameToPtr(name);
	iconSetPos(icon->base->name, icon->position.x, icon->position.y, icon->position.z);
	return icon;
}
#pragma dont_inline reset

// dont_inline keeps BtlIcon_Delete a real `bl` from the evt commands as in retail.
#pragma dont_inline on

// flags is u16 in the original (lhz); the header declares it s16, so `& 1` and `&= ~1`
// need the sign-agnostic forms.
void BtlIcon_Delete(BattleIcon* icon) {
#ifdef MUST_MATCH
	// BattleIcon.flags is u16 in the original (lhz); the header declares it s16, so reinterpret
	// to emit the unsigned load/mask without editing the existing header line.
	if (*(u16*)&icon->flags & 1) {
		iconDelete(icon->base->name);
		*(u16*)&icon->flags &= ~1;
	}
#else
	if (icon->flags & 1) {
		iconDelete(icon->base->name);
		icon->flags &= ~1;
	}
#endif
}
#pragma dont_inline reset

// This unit's original is built for size: at -O4,p mwcc emits individual stw in the
// prologue where retail has stmw, so use the size pragma for this function.
#pragma optimize_for_size on

//s32 iconId, f32 x, f32 y, f32 z, s32 evtIndex
USER_FUNC(btlevtcmd_BtlIconEntry) {
	s32* args = event->args;
	BattleIcon* icon;
	s32 iconId, index;
	f32 x, y, z;

	iconId = evtGetValue(event, args[0]);
	x = (f32)evtGetValue(event, args[1]);
	y = (f32)evtGetValue(event, args[2]);
	z = (f32)evtGetValue(event, args[3]);
	index = args[4];
#ifdef MUST_MATCH
	// The header declares BtlIcon_Entry's id as s16, but retail narrows it with clrlwi (zero-extend,
	// not extsh), so the original parameter is u16. Call through a cast pointer to emit the unsigned
	// narrowing without editing the existing header line; see REPORT.md.
	icon = ((BattleIcon* (*)(u16, f32, f32, f32))BtlIcon_Entry)(iconId, x, y, z);
#else
	icon = BtlIcon_Entry(iconId, x, y, z);
#endif
	evtSetValue(event, index, icon->index);
	return EVT_RETURN_DONE;
}
#pragma optimize_for_size reset

//s32 itemId (index for itemDataTable) , f32 x, f32 y, f32 z, s32 evtIndex
// NONMATCHING (99.17%): ItemData.icon_id is u16 in the original (lhz); the header declares
// it s16, so the direct read emits lha (same bits for the passed value). The header line
// can't be changed from here; see REPORT.md.
USER_FUNC(btlevtcmd_BtlIconEntryItemId) {
	s32* args = event->args;
	BattleIcon* icon;
	s32 itemId, index;
	f32 x, y, z;
	s16 iconId;

	itemId = evtGetValue(event, args[0]);
	x = (f32)evtGetValue(event, args[1]);
	y = (f32)evtGetValue(event, args[2]);
	z = (f32)evtGetValue(event, args[3]);
	index = args[4];
#ifdef MUST_MATCH
	iconId = itemDataTable[itemId].icon_id;
	icon = BtlIcon_Entry(iconId, x, y, z);
#else
	icon = BtlIcon_Entry(itemDataTable[itemId].icon_id, x, y, z);
#endif
	evtSetValue(event, index, icon->index);
	return EVT_RETURN_DONE;
}

//s32 index for BtlIconGetPtr
USER_FUNC(btlevtcmd_BtlIconDelete) {
	BattleIcon* icon;
	s32 value;

	value = evtGetValue(event, *event->args);
	icon = BtlIconGetPtr(value);
	BtlIcon_Delete(icon);
	return EVT_RETURN_DONE;
}

//s32 index for BtlIconGetPtr, f32 x, f32 y, f32 z
// Individual component stores avoid a (Vec){...} stack temp; the size form is needed
// for the prologue's stmw r29.
#pragma optimize_for_size on
USER_FUNC(btlevtcmd_BtlIconSetPosition) {
	s32* args = event->args;
	BattleIcon* icon;
	f32 x, y, z;
	s32 index;

	index = evtGetValue(event, args[0]);
	icon = BtlIconGetPtr(index);
	x = evtGetFloat(event, args[1]);
	y = evtGetFloat(event, args[2]);
	z = evtGetFloat(event, args[3]);
	icon->position.x = x;
	icon->position.y = y;
	icon->position.z = z;
	return EVT_RETURN_DONE;
}
#pragma optimize_for_size reset

//s32 index for BtlIconGetPtr, f32 fallAccel
// The size form is needed for the prologue's stmw r30 / epilogue's lmw.
#pragma optimize_for_size on
USER_FUNC(btlevtcmd_BtlIconSetFallAccel) {
	s32* args = event->args;
	s32 index = evtGetValue(event, args[0]);
	BattleIcon* icon = BtlIconGetPtr(index);
	icon->fallAccel = evtGetFloat(event, args[1]);
	return EVT_RETURN_DONE;
}
#pragma optimize_for_size reset

// All logic and almost every instruction match. Remaining: the second `&&` comparison loads
// field_0x30/field_0x3C into f1/f0 instead of f0/f1, and the 0.5f pool entry keeps a local
// symbol name (@279) instead of the original's float_0p5_8041e7b0.
// NONMATCHING (98.65%): see above.
// `opt_common_subs off`: with CSE on MWCC hoists icon+0x2C/0x34 across btlMovePos into r29/r30.
// Using `icon->fallAccel * (f32)count / 2.0f` (not `(f32)count * icon->fallAccel * 0.5f`)
// is what makes MWCC pick the original's fmuls/fmadds register allocation.
USER_FUNC(btlevtcmd_BtlIconJumpPosition) {
	s32* args = event->args;
	BattleIcon* icon;
	s32 index;
	f32 vx, vy, vz;
	f32 x, y, z;
	f32 dy;
	s32 count;

	index = evtGetValue(event, args[0]);
	x = (f32)evtGetValue(event, args[1]);
	y = (f32)evtGetValue(event, args[2]);
	z = (f32)evtGetValue(event, args[3]);
	count = evtGetValue(event, args[4]);
	icon = BtlIconGetPtr(index);
	if (isFirstCall) {
		event->blocked = 1;
		if (count != 0) {
			*(s32*)((u8*)icon + 0x44) = count;
		}
		*(f32*)((u8*)icon + 0x38) = x;
		*(f32*)((u8*)icon + 0x3C) = y;
		*(f32*)((u8*)icon + 0x40) = z;
		vx = icon->position.x;
		vy = icon->position.y;
		vz = icon->position.z;
		*(f32*)((u8*)icon + 0x2C) = vx;
		*(f32*)((u8*)icon + 0x20) = vx;
		*(f32*)((u8*)icon + 0x30) = vy;
		*(f32*)((u8*)icon + 0x24) = vy;
		*(f32*)((u8*)icon + 0x34) = vz;
		*(f32*)((u8*)icon + 0x28) = vz;
		*(f32*)((u8*)icon + 0x54) = angleABf(vx, vz, x, z);
		*(f32*)((u8*)icon + 0x58) = distABf(vx, vz, x, z);
		if (x - vx > 0.0f) {
			*(s8*)((u8*)icon + 0x60) = 1;
		}
		else if (x - vx < 0.0f) {
			*(s8*)((u8*)icon + 0x60) = -1;
		}
		dy = y - vy;
		if (*(s32*)((u8*)icon + 0x44) == 0) {
			*(s32*)((u8*)icon + 0x44) = (s32)(*(f32*)((u8*)icon + 0x58) / *(f32*)((u8*)icon + 0x48));
			*(f32*)((u8*)icon + 0x58) = *(f32*)((u8*)icon + 0x58) - (f32)*(s32*)((u8*)icon + 0x44) * *(f32*)((u8*)icon + 0x48);
		}
		else {
			*(f32*)((u8*)icon + 0x48) = *(f32*)((u8*)icon + 0x58) / (f32)*(s32*)((u8*)icon + 0x44);
			*(f32*)((u8*)icon + 0x58) = *(f32*)((u8*)icon + 0x58) - (f32)*(s32*)((u8*)icon + 0x44) * *(f32*)((u8*)icon + 0x48);
		}
		if (*(s32*)((u8*)icon + 0x44) == 0) {
			return EVT_RETURN_DONE;
		}
		*(f32*)((u8*)icon + 0x50) = icon->fallAccel * (f32)*(s32*)((u8*)icon + 0x44) / 2.0f + dy / (f32)*(s32*)((u8*)icon + 0x44);
		*(f32*)((u8*)icon + 0x48) = *(f32*)((u8*)icon + 0x48) + *(f32*)((u8*)icon + 0x58) / (f32)*(s32*)((u8*)icon + 0x44);
	}
	*(f32*)((u8*)icon + 0x30) = *(f32*)((u8*)icon + 0x30) + *(f32*)((u8*)icon + 0x50);
	*(f32*)((u8*)icon + 0x50) = *(f32*)((u8*)icon + 0x50) - icon->fallAccel;
	if (*(f32*)((u8*)icon + 0x50) < 0.0f && *(f32*)((u8*)icon + 0x30) < *(f32*)((u8*)icon + 0x3C)) {
		*(f32*)((u8*)icon + 0x30) = *(f32*)((u8*)icon + 0x3C);
	}
	btlMovePos((f32*)((u8*)icon + 0x2C), (f32*)((u8*)icon + 0x34), *(f32*)((u8*)icon + 0x48), *(f32*)((u8*)icon + 0x54));
	icon->position.x = *(f32*)((u8*)icon + 0x2C);
	icon->position.y = *(f32*)((u8*)icon + 0x30);
	icon->position.z = *(f32*)((u8*)icon + 0x34);
	*(s32*)((u8*)icon + 0x44) = *(s32*)((u8*)icon + 0x44) - 1;
	if (*(s32*)((u8*)icon + 0x44) <= 0) {
		icon->position.x = *(f32*)((u8*)icon + 0x38);
		icon->position.y = *(f32*)((u8*)icon + 0x3C);
		icon->position.z = *(f32*)((u8*)icon + 0x40);
		return EVT_RETURN_YIELD;
	}
	return EVT_RETURN_BLOCK;
}
