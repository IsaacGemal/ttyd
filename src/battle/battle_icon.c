#include "battle/battle_icon.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "data/item_icon_view.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <string.h>
#include <stdio.h>

extern BattleWork* _battleWorkPointer;

// .sdata2: float_1 is a real (extern) object defined at the bottom of the file, so MWCC emits it before
// the 0.5 it pools for the `/ 2.0f` below; that puts .sdata2 in the original's 0, 1, 0.5 order. float_0 is
// read by name at the x-vx/fallAccel comparisons; as a real object it lands before float_1.
extern const f32 float_0_8041e7a8;
extern const f32 float_1_8041e7ac;


// dont_inline keeps BtlIconGetPtr a real `bl` from the evt commands as in retail.
#pragma dont_inline on
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
	icon->field_0x14 = float_1_8041e7ac;
	icon->field_0x18 = float_1_8041e7ac;
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
	// narrowing without editing the existing header line
	icon = ((BattleIcon* (*)(u16, f32, f32, f32))BtlIcon_Entry)(iconId, x, y, z);
#else
	icon = BtlIcon_Entry(iconId, x, y, z);
#endif
	evtSetValue(event, index, icon->index);
	return EVT_RETURN_DONE;
}

//s32 itemId (index for itemDataTable) , f32 x, f32 y, f32 z, s32 evtIndex
USER_FUNC(btlevtcmd_BtlIconEntryItemId) {
	s32* args = event->args;
	BattleIcon* icon;
	s32 itemId, index;
	f32 x, y, z;

	itemId = evtGetValue(event, args[0]);
	x = (f32)evtGetValue(event, args[1]);
	y = (f32)evtGetValue(event, args[2]);
	z = (f32)evtGetValue(event, args[3]);
	index = args[4];
#ifdef MUST_MATCH
	// The header declares BtlIcon_Entry's id as s16, but the original passes the u16 field straight
	// into r3 (lhz, no extsh); call through a u16-function pointer to avoid the sign extension.
	// Remove when the header's first parameter becomes u16.
	icon = ((BattleIcon* (*)(u16, f32, f32, f32))BtlIcon_Entry)(itemDataTable[itemId].icon_id, x, y, z);
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
// Individual component stores avoid a (Vec){...} stack temp.
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

//s32 index for BtlIconGetPtr, f32 fallAccel
USER_FUNC(btlevtcmd_BtlIconSetFallAccel) {
	s32* args = event->args;
	s32 index = evtGetValue(event, args[0]);
	BattleIcon* icon = BtlIconGetPtr(index);
	icon->fallAccel = evtGetFloat(event, args[1]);
	return EVT_RETURN_DONE;
}

// `icon->fallAccel * (f32)count / 2.0f` (not `(f32)count * icon->fallAccel * 0.5f`)
// is what makes MWCC pick the original's fmuls/fmadds register allocation.
// The raw 0x20..0x60 fields are undeclared in battle_icon.h; the 0x2C/0x34 pre-move position
// reads after btlMovePos use a fresh base pointer to keep the size optimiser from caching
// the two argument addresses (see the MUST_MATCH note on p).
USER_FUNC(btlevtcmd_BtlIconJumpPosition) {
	s32* args = event->args;
	BattleIcon* icon;
	s32 index;
	f32 vx, vy, vz;
	f32 x, y, z;
	f32 dy;
	s32 count;
#ifdef MUST_MATCH
	// Match-only: computing x - vx into a local makes MWCC put the fsubs result in f2 and the named
	// float_0 load in f0, as the original does; writing x - vx inline swaps those two registers.
	f32 d;
	// Match-only: a second, separately-created base pointer for the post-btlMovePos reads. Reading
	// through the same `(u8*)icon + off` expression used by btlMovePos' arguments makes the -O4,s
	// size optimiser cache both argument addresses in r29/r30 across the call; the fresh pointer
	// keeps the original's `lfs f0, 0x2c(r31)` displacement form.
	u8* p;
#endif

	index = evtGetValue(event, args[0]);
	x = (f32)evtGetValue(event, args[1]);
	y = (f32)evtGetValue(event, args[2]);
	z = (f32)evtGetValue(event, args[3]);
	count = evtGetValue(event, args[4]);
	icon = BtlIconGetPtr(index);
#ifdef MUST_MATCH
	p = (u8*)icon;
#endif
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
#ifdef MUST_MATCH
		d = x - vx;
		if (d > float_0_8041e7a8) {
#else
		if (x - vx > float_0_8041e7a8) {
#endif
			*(s8*)((u8*)icon + 0x60) = 1;
		}
#ifdef MUST_MATCH
		else if (d < float_0_8041e7a8) {
#else
		else if (x - vx < float_0_8041e7a8) {
#endif
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
	if (*(f32*)((u8*)icon + 0x50) < float_0_8041e7a8 && *(f32*)((u8*)icon + 0x30) < *(f32*)((u8*)icon + 0x3C)) {
		*(f32*)((u8*)icon + 0x30) = *(f32*)((u8*)icon + 0x3C);
	}
	btlMovePos((f32*)((u8*)icon + 0x2C), (f32*)((u8*)icon + 0x34), *(f32*)((u8*)icon + 0x48), *(f32*)((u8*)icon + 0x54));
#ifdef MUST_MATCH
	// Match-only: see the fresh-base-pointer comment above p's declaration.
	icon->position.x = *(f32*)(p + 0x2C);
	icon->position.y = *(f32*)(p + 0x30);
	icon->position.z = *(f32*)(p + 0x34);
#else
	icon->position.x = *(f32*)((u8*)icon + 0x2C);
	icon->position.y = *(f32*)((u8*)icon + 0x30);
	icon->position.z = *(f32*)((u8*)icon + 0x34);
#endif
	*(s32*)((u8*)icon + 0x44) = *(s32*)((u8*)icon + 0x44) - 1;
	if (*(s32*)((u8*)icon + 0x44) <= 0) {
		icon->position.x = *(f32*)((u8*)icon + 0x38);
		icon->position.y = *(f32*)((u8*)icon + 0x3C);
		icon->position.z = *(f32*)((u8*)icon + 0x40);
		return EVT_RETURN_YIELD;
	}
	return EVT_RETURN_BLOCK;
}

// .sdata2 objects the code refers to by name. Defined after the code so the references above stay bound
// to these names; the compiler's own 0.5 pool entry (from `... / 2.0f`) lands after them.
const f32 float_0_8041e7a8 = 0.0f;
const f32 float_1_8041e7ac = 1.0f;
