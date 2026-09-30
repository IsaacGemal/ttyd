#include "battle/battle_icon.h"
#include "battle/battle.h"
#include "data/item_data.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
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

// NONMATCHING (59.35%): 8 instructions differ, 7 missing, 7 extra, plus register differences.
void BattleIconMain(void) {
	BattleWork* wp;
	BattleIcon* icon;
	IconEntry* base;
	int i;

	wp = _battleWorkPointer;
	for (i = 0; i < 16; i++) {
		icon = &wp->icons[i];
		if (icon->flags & 1) { //in use
			base = icon->base;
			if (base) {
				iconSetPos(base->name, icon->position.x, icon->position.y, icon->position.z);
				iconSetScale(base->name, icon->field_0x14 * icon->field_0x18);
				if (icon->flags & 2) {
					iconFlagOn(base->name, 2);
				}
				else {
					iconFlagOff(base->name, 2);
				}
			}
		}
	}
}

// NONMATCHING (32.25%): 3 instructions differ, 2 missing, 10 extra, plus register differences.
void BattleIconEnd(void) {
	BattleWork* wp = _battleWorkPointer;
	int i;

	for (i = 0; i < 16; i++) {
		BtlIcon_Delete(&wp->icons[i]);
	}
}

// dont_inline keeps BtlIcon_Entry a real `bl` from the evt commands as in retail.
#pragma dont_inline on

// NONMATCHING (7.59%): 7 instructions differ, 4 missing, 62 extra, plus register differences.
BattleIcon* BtlIcon_Entry(s16 iconId, f32 x, f32 y, f32 z) {
	BattleIcon* icon;
	int i;
	char name[32];

	for (i = 0; i < 16; i++) {
		icon = &_battleWorkPointer->icons[i];
		if (!(icon->flags & 1)) {
			break; //found one not in use
		}
	}
	if (i < 16) {
		memset(icon, 0, sizeof(BattleIcon));
		icon->index = i;
		icon->flags = 1;
		icon->field_0x14 = 1.0f;
		icon->field_0x18 = 1.0f;
		icon->position = (Vec){x, y, z};
		icon->iconId = iconId;
		sprintf(name, "_biu_%02x", icon->index);
		iconEntry(name, icon->iconId);
		icon->base = iconNameToPtr(name);
		iconSetPos(icon->base->name, icon->position.x, icon->position.y, icon->position.z);
		return icon;
	}
	else {
		return NULL;
	}
}
#pragma dont_inline reset

// dont_inline keeps BtlIcon_Delete a real `bl` from the evt commands as in retail.
#pragma dont_inline on

// NONMATCHING (90.53%): 3 instructions differ.
void BtlIcon_Delete(BattleIcon* icon) {
	if (icon->flags & 1) {
		iconDelete(icon->base->name);
		icon->flags &= ~1;
	}
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
#pragma optimize_for_size off

//s32 itemId (index for itemDataTable) , f32 x, f32 y, f32 z, s32 evtIndex
// NONMATCHING (89.92%): the prologue needs the file's size form (stmw/lmw), itemDataTable[].icon_id
// is u16 (lhz, not lha), and the pooled int->float double stays anonymous.
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
	icon = BtlIcon_Entry(itemDataTable[itemId].icon_id, x, y, z);
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
// NONMATCHING (0.00%): 5 instructions differ, 1 missing, 60 extra, plus register differences.
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
	icon->position = (Vec){x, y, z};
	return EVT_RETURN_DONE;
}

//s32 index for BtlIconGetPtr, f32 fallAccel
// NONMATCHING (0.00%): 3 instructions differ, 1 missing, 46 extra, plus register differences.
USER_FUNC(btlevtcmd_BtlIconSetFallAccel) {
	s32* args = event->args;
	s32 index = evtGetValue(event, args[0]);
	BattleIcon* icon = BtlIconGetPtr(index);
	icon->fallAccel = evtGetFloat(event, args[1]);
	return EVT_RETURN_DONE;
}

// NONMATCHING (0.57%): 1 instruction differs, 244 missing.
USER_FUNC(btlevtcmd_BtlIconJumpPosition) {
	//TODO: revisit when more of BattleWorkIcon is filled out
	return 2;
}
