#pragma once

#include "driver/camdrv.h"

typedef struct BattleUnit BattleUnit;

typedef struct BattleWeapon BattleWeapon;
typedef struct BattleWork BattleWork;

#include <dolphin/types.h>

typedef struct BattleCommandCursor {
    s32 field_0; // 0x0
    s32 field_4; // 0x4
    s32 unk8;    // 0x8
} BattleCursor;

typedef struct BattleCommand {
    s32 unk0;                 // 0x0
    s32 field_4;              // 0x4
    u8 unk8[0x498 - 0x8];     // 0x8
    BattleCursor cursor[14];  // 0x498
    u8 unk540[0x55C - 0x540]; // 0x540
    void* field_55C;          // 0x55C
    u8 unk560[0x574 - 0x560]; // 0x560
} BattleCommand;
BattleWeapon* BattleGetSelectWeapon(BattleWork* wp);
void* BattleSetConfuseAct(BattleWork* wp, BattleUnit* unit);
void BattleDrawEnemyHP(CameraId camId, void* param);
void _btlcmd_MakeActClassTable(BattleWork* wp);
void _btlcmd_MakeMultiItemTable(BattleWork* wp);
void _btlcmd_MakeOperationTable(BattleWork* wp);
void _btlcmd_MakeSelectWeaponTable(BattleWork* wp, void* arg);
void _btlcmd_GetCursorPtr(BattleCommand* cmd, s32 index, BattleCursor** out);
s32 _btlcmd_CheckWeaponTargetNum(BattleWork* wp, BattleUnit* unit, BattleWeapon* weapon);
void _cursor_init(BattleCursor* cursor);
void BattleCommandDisplay_ActMenuEnd(BattleWork* wp);
void BattleCommandDisplay_TargetSelectMenuDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_ProtectPartnerSelectMenuDisp(CameraId camId, BattleWork* wp);
BOOL BattleCommandCheckChangePositionEnable(BattleWork* wp);
