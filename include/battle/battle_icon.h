#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>
#include "driver/icondrv.h"

typedef struct BattleWorkIcon {
	s16 flags; //0x0
	s16 iconId; //0x2
	s32 index; //0x4, TODO: rename?
	Vec position; //0x8
	f32 field_0x14; //0x14, prob X, TODO
	f32 field_0x18; //0x18
	IconEntry* base; //0x1C
	Vec moveStartPos; //0x20, btlevtcmd_BtlIconJumpPosition's move state (laid out like BattleUnit's)
	Vec moveCurrentPos; //0x2C
	Vec moveTargetPos; //0x38
	s32 moveFrames; //0x44
	f32 moveSpeedXZ; //0x48
	f32 fallAccel; //0x4C
	f32 moveSpeedY; //0x50
	f32 moveAngle; //0x54
	f32 moveDist; //0x58
	f32 field_0x5C; //0x5C
	s8 moveDirection; //0x60
	u8 field_0x61[0x9C - 0x61]; //0x61
} BattleIcon;

void BattleIconInit(void);
void BattleIconMain(void);
void BattleIconEnd(void);
BattleIcon* BtlIcon_Entry(u16 iconId, f32 x, f32 y, f32 z);
void BtlIcon_Delete(BattleIcon* icon);

USER_FUNC(btlevtcmd_BtlIconDelete);
USER_FUNC(btlevtcmd_BtlIconEntry);
USER_FUNC(btlevtcmd_BtlIconEntryItemId);
USER_FUNC(btlevtcmd_BtlIconJumpPosition);
USER_FUNC(btlevtcmd_BtlIconSetFallAccel);
USER_FUNC(btlevtcmd_BtlIconSetPosition);
