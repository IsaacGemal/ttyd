#pragma once

#include "driver/camdrv.h"

typedef struct BattleUnit BattleUnit;

typedef struct BattleWeapon BattleWeapon;
typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
#include "battle/battle_unit.h"
#include "mario/mario_pouch.h"

typedef struct BattleCommandCursor {
    s32 field_0; // 0x0
    s32 field_4; // 0x4
    s32 unk8;    // 0x8
} BattleCursor;

// Element layouts of the battle command menu tables inside BattleCommand. Each mirrors the matching
// BattleWorkCommand* entry in battle.h (same offsets and strides) and is named by the reader in
// battle_seq_command.c that uses it.
typedef struct BattleCommandAction {
    u32 mType;          // 0x0, 0 jump, 1 hammer, 2 item, 4 super action, 5 operation, 6 tech/party, 0xD lecture end
    BOOL mEnabled;      // 0x4
    char* mName;        // 0x8
    u16 mIcon;          // 0xC
    u8 pad_0xE[2];      // 0xE
    u32 mDisableReason; // 0x10, _get_msg error code: 0 none, 3 not now, 4/5 no Jump/Hammer target
} BattleCommandAction;  // 0x14

typedef struct BattleCommandWeapon {
    BattleWeapon* mWeaponParams; // 0x0
    u32 mEnabled;                // 0x4
    char* mName;                 // 0x8
    u16 mIcon;                   // 0xC, read at commandMenu + idx * 0x1C + 0x8C by DrawWeaponWin
    u8 pad_0xE[2];               // 0xE
    u32 mIndex;                  // 0x10, -1 for the built-in Jump and Hammer weapons
    ItemType mItem;              // 0x14
    u32 mDisableReason;          // 0x18, as BattleCommandAction.mDisableReason; also 1 no FP, 2 no AP
} BattleCommandWeapon;           // 0x1C

typedef struct BattleCommandOperation {
    u32 mType;          // 0x0
    BOOL mEnabled;      // 0x4
    u32 mDisableReason; // 0x8, as BattleCommandAction.mDisableReason; 1 no FP
    char* mName;        // 0xC
    u16 mIcon;          // 0x10
    u8 pad_0x12[2];     // 0x12
    char* mDescription; // 0x14
    u32 mWeaponCost;    // 0x18
} BattleCommandOperation; // 0x1C

typedef struct BattleCommandParty {
    BattleUnitType mPartyId; // 0x0
    BOOL mEnabled;           // 0x4
    char* mName;             // 0x8
    u16 mIcon;               // 0xC
    u8 pad_0xE[2];           // 0xE
    char* mDescription;      // 0x10
    u16 mCurrentHP;          // 0x14
    u16 mMaxHP;              // 0x16
} BattleCommandParty;        // 0x18

typedef struct BattleCommandMultiItem {
    BOOL mEnabled;      // 0x0
    char* mName;        // 0x4
    char* mDescription; // 0x8
    u16 mIcon;          // 0xC
    u8 mKind;           // 0xE, 1 Item, 2 Double Dip, 3 Triple Dip
    u8 pad_0xF;         // 0xF
    u32 mWeaponCost;    // 0x10
    u32 mDisableReason; // 0x14, 1 when mEnabled is 0
} BattleCommandMultiItem; // 0x18

typedef struct BattleCommand {
    s32 commandState;                         // 0x0
    s32 menuFlags;                            // 0x4, bit 0 tested in battle_seq.c
    BattleCommandAction actionTable[6];       // 0x8
    BattleCommandWeapon weaponTable[21];      // 0x80
    BattleCommandOperation operationTable[7]; // 0x2CC
    BattleCommandParty partyTable[8];         // 0x390
    BattleCommandMultiItem multiItemTable[3]; // 0x450
    BattleCursor cursor[14];                  // 0x498
    s32 actKind;                              // 0x540, action mType (get_actkind reads it); _btlcmd_GetCursorPtr switches on it
    s32 savedActKind;                         // 0x544, actKind saved before it is overridden with 0xB/0xC; restored in commandState 0
    s32 operationKind;                        // 0x548, operation mType, switched on in the operation menu
    s32 partyChangeId;                        // 0x54C, party mPartyId; BattleWork 0x1C68, passed to BattlePartyAnimeLoad
    u8 multiItemKind;                         // 0x550, multiItemTable[i].mKind of the confirmed entry
    u8 cancelled;                             // 0x551, set by the cancel path, read by the action setup
    u8 pad_0x552[0x554 - 0x552];              // 0x552
    s32 restoreId;                            // 0x554, commandState saved by the restore path, compared with it to clear restoreState
    u8 restoreState;                          // 0x558, set to 1 or 2 when a sub-menu opens; tested by BattleMenuKeyOKInACT
    u8 pad_0x559[0x55C - 0x559];              // 0x559
    void* field_55C;                          // 0x55C
    s32 idleFrames;                           // 0x560, reset while a button is held, counts up otherwise; 0x384 switches the idle camera
    s32 disableReason;                        // 0x564, mDisableReason of the rejected entry; _get_msg picks the error text
    s32 eventId;                              // 0x568, pending event checked with evtCheckID
    u8 pad_0x56C[0x574 - 0x56C];              // 0x56C
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
s32 _btlcmd_MakeSelectWeaponTable(BattleWork* wp, s32 arg);
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
void BattleDrawEnemyHPBar(Vec pos, BattleUnit* unit, GXColor color);
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
void BattleCommandDisplay_WeaponSelectMenuSetup(BattleWork* wp, s32 arg, u32 arg2);
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
typedef struct CheckPartyTable {
    s32 kind;    // 0x0
    char* name;  // 0x4
    u16 icon;    // 0x8
    u16 pad;     // 0xA
    char* help;  // 0xC
} CheckPartyTable;
void BattleCommandDisplay_AllEnd(BattleWork* wp);
void BattleCommandDisplay_WeaponMultiItemCancelCheckDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_WeaponSelectMenuEnd(BattleWork* wp);
void BattleCommandDisplay_OperationMenuEnd(BattleWork* wp);
void BattleCommandDisplay_MultiItemMenuEnd(BattleWork* wp);
void BattleCommandDisplay_ChangePartySelectMenuEnd(BattleWork* wp);
void BattleCommandDisplay_WeaponSelectMenuDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_OperationMenuDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_MultiItemMenuDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_ChangePartySelectMenuDisp(CameraId camId, BattleWork* wp);
void BattleCommandDisplay_ActMenuDisp(CameraId camId, void* param);
