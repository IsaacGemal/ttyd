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


BattleIcon* BtlIconGetPtr(s32 index) {
	BattleIcon* icon;
	int i;

	for (i = 0; i < 16; i++) {
		icon = &_battleWorkPointer->icons[i];
		if (icon->flags & 1 && icon->index == index) {
			return icon; //TODO: redo bwp->iconWork?
		}
	}
	return NULL;
}

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
		if (icon->flags & 1) { //in use
			if (icon->base) {
				iconSetPos(icon->base->name, icon->position.x, icon->position.y, icon->position.z);
				iconSetScale(icon->base->name, icon->field_0x14 * icon->field_0x18);
				if (icon->flags & 2) {
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

// name[8] is what the original reserves for "_biu_%02x" (8 bytes with the null);
// a larger buffer grows the frame past the original's 0x50.
BattleIcon* BtlIcon_Entry(u16 iconId, f32 x, f32 y, f32 z) {
	BattleIcon* icon;
	int i;
	char name[8];

	for (i = 0; i < 16; i++) {
		icon = &_battleWorkPointer->icons[i];
		if (!(icon->flags & 1)) {
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
	iconEntry(name, icon->iconId);
	icon->base = iconNameToPtr(name);
	iconSetPos(icon->base->name, icon->position.x, icon->position.y, icon->position.z);
	return icon;
}

void BtlIcon_Delete(BattleIcon* icon) {
	if (icon->flags & 1) {
		iconDelete(icon->base->name);
		icon->flags &= ~1;
	}
}

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
	icon = BtlIcon_Entry(iconId, x, y, z);
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
USER_FUNC(btlevtcmd_BtlIconJumpPosition) {
	s32* args = event->args;
	BattleIcon* icon;
	s32 index;
	f32 vx, vy, vz;
	f32 x, y, z;
	f32 dy;
	s32 count;
	// Computing x - vx into a local makes MWCC put the fsubs result in f2 and the named
	// float_0 load in f0, as the original does; writing x - vx inline swaps those two registers.
	f32 d;

	index = evtGetValue(event, args[0]);
	x = (f32)evtGetValue(event, args[1]);
	y = (f32)evtGetValue(event, args[2]);
	z = (f32)evtGetValue(event, args[3]);
	count = evtGetValue(event, args[4]);
	icon = BtlIconGetPtr(index);
	if (isFirstCall) {
		event->blocked = 1;
		if (count != 0) {
			icon->moveFrames = count;
		}
		icon->moveTargetPos.x = x;
		icon->moveTargetPos.y = y;
		icon->moveTargetPos.z = z;
		vx = icon->position.x;
		vy = icon->position.y;
		vz = icon->position.z;
		icon->moveCurrentPos.x = vx;
		icon->moveStartPos.x = vx;
		icon->moveCurrentPos.y = vy;
		icon->moveStartPos.y = vy;
		icon->moveCurrentPos.z = vz;
		icon->moveStartPos.z = vz;
		icon->moveAngle = angleABf(vx, vz, x, z);
		icon->moveDist = distABf(vx, vz, x, z);
		d = x - vx;
		if (d > float_0_8041e7a8) {
			icon->moveDirection = 1;
		}
		else if (d < float_0_8041e7a8) {
			icon->moveDirection = -1;
		}
		dy = y - vy;
		if (icon->moveFrames == 0) {
			icon->moveFrames = (s32)(icon->moveDist / icon->moveSpeedXZ);
			icon->moveDist = icon->moveDist - (f32)icon->moveFrames * icon->moveSpeedXZ;
		}
		else {
			icon->moveSpeedXZ = icon->moveDist / (f32)icon->moveFrames;
			icon->moveDist = icon->moveDist - (f32)icon->moveFrames * icon->moveSpeedXZ;
		}
		if (icon->moveFrames == 0) {
			return EVT_RETURN_DONE;
		}
		icon->moveSpeedY = icon->fallAccel * (f32)icon->moveFrames / 2.0f + dy / (f32)icon->moveFrames;
		icon->moveSpeedXZ = icon->moveSpeedXZ + icon->moveDist / (f32)icon->moveFrames;
	}
	icon->moveCurrentPos.y = icon->moveCurrentPos.y + icon->moveSpeedY;
	icon->moveSpeedY = icon->moveSpeedY - icon->fallAccel;
	if (icon->moveSpeedY < float_0_8041e7a8 && icon->moveCurrentPos.y < icon->moveTargetPos.y) {
		icon->moveCurrentPos.y = icon->moveTargetPos.y;
	}
	btlMovePos(&icon->moveCurrentPos.x, &icon->moveCurrentPos.z, icon->moveSpeedXZ, icon->moveAngle);
	icon->position.x = icon->moveCurrentPos.x;
	icon->position.y = icon->moveCurrentPos.y;
	icon->position.z = icon->moveCurrentPos.z;
	icon->moveFrames = icon->moveFrames - 1;
	if (icon->moveFrames <= 0) {
		icon->position.x = icon->moveTargetPos.x;
		icon->position.y = icon->moveTargetPos.y;
		icon->position.z = icon->moveTargetPos.z;
		return EVT_RETURN_YIELD;
	}
	return EVT_RETURN_BLOCK;
}

// .sdata2 objects the code refers to by name. Defined after the code so the references above stay bound
// to these names; the compiler's own 0.5 pool entry (from `... / 2.0f`) lands after them.
const f32 float_0_8041e7a8 = 0.0f;
const f32 float_1_8041e7ac = 1.0f;
