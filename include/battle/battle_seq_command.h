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
// The battle-command error event script. It is a .sdata object in battle_seq_command.c; the declspec
// lets BattleCommandInput emit the target's `li r3, _btl_cmd_error_msg_event@sda21` address form.
extern __declspec(section ".sdata") s32 _btl_cmd_error_msg_event[];
BattleWeapon* BattleGetSelectWeapon(BattleWork* wp);
void* BattleSetConfuseAct(BattleWork* wp, BattleUnit* unit);
void BattleDrawEnemyHP(CameraId camId, void* param);
void _btlcmd_MakeActClassTable(BattleWork* wp);
void _btlcmd_MakeMultiItemTable(BattleWork* wp);
void _btlcmd_MakeOperationTable(BattleWork* wp);
s32 _btlcmd_MakeSelectWeaponTable(BattleWork* wp, void* arg);
s32 _getJumpIconId(void);
s32 _getHammerIconId(void);
void _btlcmd_GetCursorPtr(BattleCommand* cmd, s32 index, BattleCursor** out);
s32 _btlcmd_CheckWeaponTargetNum(BattleWork* wp, BattleUnit* unit, BattleWeapon* weapon);
void _cursor_init(BattleCursor* cursor);
void BattleCommandDisplay(BattleWork* wp);
void BattleCommandDisplay_ActMenuEnd(BattleWork* wp);
void BattleCommandDisplay_TargetSelectMenuDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_ProtectPartnerSelectMenuDisp(CameraId camId, BattleWork* wp);
BOOL BattleCommandCheckChangePositionEnable(BattleWork* wp);
void BattleDrawEnemyHPBar(Vec* pos, BattleUnit* unit, GXColor* color);
void _btlcmd_UpdateSelectWeaponTable(BattleWork* wp, s32 arg);
s32 _btlcmd_SelectWeaponDecide(BattleWork* wp, s32 arg, s32 weaponIdx);
void BattleCommandDisplay_TargetSelectMenuSetup(BattleWork* wp);
BOOL BattleCommandAttackAudienceCheck(void);
s32 BattleCommandDisplay_WeaponMultiItemCancelCheckMain(BattleWork* wp);
s32 BattleCommandDisplay_WeaponSelectMenuMain(BattleWork* wp, s32 arg);
s32 _btlcmd_MakePartyTable(BattleWork* wp);
s32 _check_weapon_type_attack(BattleWeapon* weapon);
void _btlcmd_SetAttackEvent(BattleUnit* unit, BattleCommand* cmd);
s32 _battleGetPartyIcon(s32 kind);
s32 BattleCommandInput(BattleWork* wp);
void BattleCommandInit(BattleWork* wp);
void BattleCommandDisplay_ActMenuSetup(BattleWork* wp, void* arg);
s32 BattleCommandDisplay_ActMenuMain(BattleWork* wp);
void BattleCommandDisplay_WeaponSelectMenuSetup(BattleWork* wp, void* arg, void* arg2);
void BattleCommandDisplay_TargetSelectMenuEnd(BattleWork* wp);
void BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(BattleWork* wp);
void BattleCommandDisplay_MultiItemMenuSetup(BattleWork* wp, void* arg);
s32 BattleCommandDisplay_MultiItemMenuMain(BattleWork* wp);
void BattleCommandDisplay_OperationMenuSetup(BattleWork* wp, void* arg);
s32 BattleCommandDisplay_OperationMenuMain(BattleWork* wp);
void BattleCommandDisplay_ChangePartySelectMenuSetup(BattleWork* wp, void* arg);
s32 BattleCommandDisplay_ChangePartySelectMenuMain(BattleWork* wp);
void BattleCommandDisplay_ProtectPartnerSelectMenuSetup(BattleWork* wp);
void BattleCommandDisplay_ProtectPartnerSelectMenuEnd(BattleWork* wp);
void _commandRestoreRec(void);
s32 _check_present_item(void);
