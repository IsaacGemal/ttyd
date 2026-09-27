#include "battle/battle_seq_command.h"
#include "battle/battle.h"
#include "battle/battle_monosiri.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_detect.h"
#include "battle/battle_menu_disp.h"
#include "battle/battle_stage.h"
#include "eff/eff_recovery.h"
#include "battle/battle_camera.h"
#include "battle/battle_audience.h"


//TODO: move into battle/seq/?

//local prototypes

extern f32 float_5_8041c310;

// NONMATCHING (19.37%): 116 missing, plus register differences.
void BattleDrawEnemyHP(CameraId camId, void* param) {
	BattleWork* wp = param;
	BattleUnit* unit;
	Vec position;
	int i;

	camGetPtr(CAMERA_2D); //unused, probably just stored

	for (i = 0; i < 64; i++) {
		unit = wp->units[i];
		if (unit) {
			if (!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
				if (battleCheckUnitMonosiriFlag(unit)) {
					if (!unit->mAttributeFlags & 0x2000000) {
						if (wp->commandMenu.field_4 & 0x1000000) {
							if (unit->flags & 1) {
								position.x = unit->mPosition.x;
								position.y = unit->mPosition.y;
								position.z = unit->mPosition.z;
								if (wp->flags & 0x80) {
									position.x = unit->mHomePosition.x;
									position.y = unit->mHomePosition.y;
									position.z = unit->mHomePosition.z;
								}
								position.x += (f32)unit->mHpGaugeOffset[0];
								position.y += (f32)unit->mHpGaugeOffset[1];
							}
						}
					}
				}
			}
		}
	}
}

void BattleCommandInit(BattleWork* wp) {
	wp->commandMenu.unk0 = 0;
	wp->flags |= 0x80;
}

#pragma dont_inline on
void BattleCommandDisplay_AllEnd(BattleWork* wp) {
	wp->commandMenu.field_4 &= ~0x111111;
}
#pragma dont_inline reset

void BattleCommandDisplay_ProtectPartnerSelectMenuEnd(BattleWork* wp) {
	wp->commandMenu.field_4 &= ~0x100000;
}

void BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(BattleWork* wp) {
	wp->commandMenu.cursor[13].field_0 = 0;
	wp->commandMenu.cursor[13].field_4 = 0;
	wp->commandMenu.cursor[13].unk8 = 2;
}

#pragma dont_inline on
void BattleCommandDisplay_ActMenuEnd(BattleWork* wp) {
	BattleMenuDisp_ActSelect_End(wp);
	wp->commandMenu.field_4 &= ~0x1;
}
#pragma dont_inline reset

void BattleCommandDisplay_WeaponSelectMenuEnd(BattleWork* wp) {
	BattleMenuDisp_WeaponSelect_End(wp);
	wp->commandMenu.field_4 &= ~0x10;
}

void BattleCommandDisplay_OperationMenuEnd(BattleWork* wp) {
	BattleMenuDisp_Operation_End(wp);
	wp->commandMenu.field_4 &= ~0x100;
}

void BattleCommandDisplay_MultiItemMenuEnd(BattleWork* wp) {
	BattleMenuDisp_MultiItem_End(wp);
	wp->commandMenu.field_4 &= ~0x20;
}

void BattleCommandDisplay_ChangePartySelectMenuEnd(BattleWork* wp) {
	BattleMenuDisp_ChangePartySelect_End(wp);
	wp->commandMenu.field_4 &= ~0x1000;
}

void BattleCommandDisplay_ChangePartySelectMenuSetup(BattleWork* wp, void* arg) {
	BattleMenuDisp_ChangePartySelect_Init(wp, (u32)arg);
	wp->commandMenu.field_4 |= 0x01001000;
}

void BattleCommandDisplay_WeaponSelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_WeaponSelect_Disp(wp);
}

void BattleCommandDisplay_OperationMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_Operation_Disp(wp);
}

void BattleCommandDisplay_MultiItemMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_MultiItem_Disp(wp);
}

void BattleCommandDisplay_ChangePartySelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_ChangePartySelect_Disp(wp);
}

s32 _getHammerIconId(void) {
	switch (pouchGetPtr()->hammerLevel) {
	case 3:
		return 0x9E;
	case 2:
		return 0x9D;
	default:
		return 0x9C;
	}
}

s32 _getJumpIconId(void) {
	switch (pouchGetPtr()->jumpLevel) {
	case 3:
		return 0xA1;
	case 2:
		return 0xA0;
	default:
		return 0x9F;
	}
}

void BattleCommandDisplay_MultiItemMenuSetup(BattleWork* wp, void* arg) {
	_btlcmd_MakeMultiItemTable(wp);
	BattleMenuDisp_MultiItem_Init(wp, (u32)arg);
	wp->commandMenu.field_4 |= 0x20;
}

void BattleCommandDisplay_OperationMenuSetup(BattleWork* wp, void* arg) {
	_btlcmd_MakeOperationTable(wp);
	BattleMenuDisp_Operation_Init(wp, (u32)arg);
	wp->commandMenu.field_4 |= 0x01000100;
}

void BattleCommandDisplay_WeaponSelectMenuSetup(BattleWork* wp, void* arg, void* arg2) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_MakeSelectWeaponTable(wp, arg);
	_btlcmd_GetCursorPtr(cmd, (s32)arg, &cursor);
	BattleMenuDisp_WeaponSelect_Init(wp, cursor, &cmd->unk8[0x80 - 0x8], (u32)arg2);
	cmd->field_4 |= 0x01000010;
}

void BattleCommandDisplay_ActMenuSetup(BattleWork* wp, void* arg) {
	BattleWork* work = wp;
	void* a = arg;
	BattleCursor* cursor;
	BattleCommand* cmd = &work->commandMenu;

	_btlcmd_MakeActClassTable(work);
	_btlcmd_GetCursorPtr(cmd, 14, &cursor);
	BattleMenuDisp_ActSelect_Init(work, cursor, &cmd->unk8[0], (u32)a);
	*(s32*)&cmd->unk560[0] = 0;
	cmd->field_4 |= 0x01000001;
	if (a == NULL && cmd->unk540[0x18] == 0) {
		_cursor_init(&cmd->cursor[2]);
		_cursor_init(&cmd->cursor[3]);
		_cursor_init(&cmd->cursor[4]);
		_cursor_init(&cmd->cursor[5]);
		_cursor_init(&cmd->cursor[6]);
		_cursor_init(&cmd->cursor[7]);
		_cursor_init(&cmd->cursor[8]);
		_cursor_init(&cmd->cursor[9]);
		_cursor_init(&cmd->cursor[11]);
	}
}

// NONMATCHING (92.50%): 2 instructions differ.
void BattleCommandDisplay_ActMenuDisp(CameraId camId, BattleWork* wp) {
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr(&wp->commandMenu, 0xE, &cursor);
	BattleMenuDisp_ActSelect_Disp(wp);
}

void BattleCommandDisplay_TargetSelectMenuSetup(BattleWork* wp) {
	BattleUnit* unit;
	s32 id = wp->activeUnitId;

	unit = BattleGetUnitPtr(wp, id);
	wp->commandMenu.field_4 |= 0x00010000;
	BtlUnit_SetAnimType(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), 0x28);
}

void BattleCommandDisplay_ProtectPartnerSelectMenuSetup(BattleWork* wp) {
	BattleUnit* unit;

	unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleGetPartnerPtr(wp, unit);
	wp->commandMenu.field_4 |= 0x00100000;
	wp->commandMenu.field_4 &= ~0x01000000;
}

void BattleCommandDisplay_TargetSelectMenuEnd(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr(cmd, *(s32*)&cmd->unk540[0], &cursor);
	btl_camera_set_mode(0, 0);
	cmd->field_4 &= ~0x10000;
	cmd->field_4 &= ~0x01000000;
}

#pragma dont_inline on
void _cursor_init(BattleCursor* cursor) {
	cursor->field_0 = 0;
	cursor->field_4 = 0;
}
#pragma dont_inline reset

void _commandRestoreRec(void) {
	BattleWork* wp = _battleWorkPointer;

	if (wp->commandMenu.unk540[0x18] == 0) {
		*(s32*)&wp->commandMenu.unk540[0x14] = wp->commandMenu.unk0;
	} else if (*(s32*)&wp->commandMenu.unk540[0x14] == wp->commandMenu.unk0) {
		*(s32*)&wp->commandMenu.unk540[0x14] = 0;
		wp->commandMenu.unk540[0x18] = 0;
	}
}

BattleWeapon* BattleGetSelectWeapon(BattleWork* wp) {
	BattleWorkCommand* cmd = (BattleWorkCommand*)&wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr((BattleCommand*)cmd, *(s32*)&cmd->field_0x540[0], &cursor);
	return cmd->mWeaponTable[cursor->field_0].mWeaponParams;
}

BOOL BattleCommandAttackAudienceCheck(void) {
	BattleWork* wp = _battleWorkPointer;
	s32 pad = BattlePadCheckTrigger(0x400);

	if (pad) {
		if (!(wp->flags & 0x40000000)) {
			BattleUnit* mario = BattleGetMarioPtr(wp);

			if (mario->mAttributeFlags & kVeiled) {
				return FALSE;
			}
			{
				s32 item;

				BattleAudience_GetItemOn(&item, NULL, NULL, NULL, NULL);
				if (item != -1) {
					BattleAudience_SetTarget(item);
					BattleCommandDisplay_ActMenuEnd(wp);
					*(s32*)&wp->commandMenu.unk540[4] = *(s32*)&wp->commandMenu.unk540[0];
					*(s32*)&wp->commandMenu.unk540[0] = 0xB;
					wp->commandMenu.unk0 = 0x1D;
					BattleStageOffLightInCommand(0, 1);
					wp->flags |= 0x4;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void BattleCommandDisplay(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;

	if (cmd) {
		SelectedItemCoordinateColorUpDate();
		dispEntry(CAMERA_2D, 902.0f, 0, BattleDrawEnemyHP, wp);
		if (cmd->field_4 & 0x1) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_ActMenuDisp, wp);
		}
		if (cmd->field_4 & 0x10) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_WeaponSelectMenuDisp, wp);
		}
		if (cmd->field_4 & 0x100) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_OperationMenuDisp, wp);
		}
		if (cmd->field_4 & 0x1000) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_ChangePartySelectMenuDisp, wp);
		}
		if (cmd->field_4 & 0x20) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_MultiItemMenuDisp, wp);
		}
		if (cmd->field_4 & 0x10000) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_TargetSelectMenuDisp, wp);
		}
		if (cmd->field_4 & 0x100000) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_ProtectPartnerSelectMenuDisp, wp);
		}
	}
}

void _btlcmd_GetCursorPtr(BattleCommand* cmd, s32 index, BattleCursor** out) {
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);

	switch (index) {
	case 14:
		if (unit->currentKind == 0xDE) {
			*out = &cmd->cursor[0];
		} else {
			*out = &cmd->cursor[1];
		}
		break;
	case 0:
		*out = &cmd->cursor[2];
		break;
	case 1:
		*out = &cmd->cursor[3];
		break;
	case 2:
		*out = &cmd->cursor[11];
		break;
	case 3:
		*out = &cmd->cursor[12];
		break;
	case 4:
		*out = &cmd->cursor[7];
		break;
	case 5:
		if (unit->currentKind == 0xDE) {
			*out = &cmd->cursor[8];
		} else {
			*out = &cmd->cursor[9];
		}
		break;
	case 6:
		*out = &cmd->cursor[4];
		break;
	case 7:
		*out = &cmd->cursor[5];
		break;
	case 8:
		*out = &cmd->cursor[6];
		break;
	case 10:
		*out = &cmd->cursor[10];
		break;
	}
}

void _btlcmd_UpdateSelectWeaponTable(BattleWork* wp, s32 arg) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleWorkCommand* wcmd = (BattleWorkCommand*)&wp->commandMenu;
	BattleCursor* cursor;
	s32 i;
	s32 count;

	_btlcmd_GetCursorPtr(&wp->commandMenu, arg, &cursor);
	count = cursor->unk8;
	for (i = 0; i < count; i++) {
		BattleWeapon* params;
		s32 check;

		params = wcmd->mWeaponTable[i].mWeaponParams;
		wcmd->mWeaponTable[i].field_0x4 = 0;
		wcmd->mWeaponTable[i].field_0x18 = 0;
		if (params == NULL) {
			continue;
		}
		check = _btlcmd_CheckWeaponTargetNum(wp, unit, params);
		if (check <= 0) {
			if (check == 0) {
				switch (arg) {
				case 0:
					wcmd->mWeaponTable[i].field_0x18 = 4;
					break;
				case 1:
					wcmd->mWeaponTable[i].field_0x18 = 5;
					break;
				default:
					wcmd->mWeaponTable[i].field_0x18 = 3;
					break;
				}
			} else {
				if (arg != 4) {
					wcmd->mWeaponTable[i].field_0x18 = 1;
				} else {
					wcmd->mWeaponTable[i].field_0x18 = 2;
				}
			}
		} else {
			wcmd->mWeaponTable[i].field_0x4 = 1;
			wcmd->mWeaponTable[i].field_0x18 = 0;
		}
	}
}

s32 _check_weapon_type_attack(BattleWeapon* weapon) {
	s8* p = (s8*)weapon;

	if (weapon == NULL) {
		return 0;
	}
	if (weapon->hpDamageFn != NULL) {
		return 1;
	}
	if (p[0x80] > 0 && p[0x81] > 0) {
		return 1;
	}
	if (p[0x82] > 0 && p[0x83] > 0) {
		return 1;
	}
	if (p[0x84] > 0 && p[0x85] > 0) {
		return 1;
	}
	if (p[0x86] > 0 && p[0x87] > 0) {
		return 1;
	}
	if (p[0x89] > 0 && p[0x8A] > 0) {
		return 1;
	}
	if (p[0x8F] > 0 && p[0x90] > 0) {
		return 1;
	}
	if (p[0x91] > 0 && p[0x92] > 0) {
		return 1;
	}
	if (p[0x93] > 0 && p[0x94] > 0 && p[0x95] < 0) {
		return 1;
	}
	if (p[0x96] > 0 && p[0x97] > 0 && p[0x98] < 0) {
		return 1;
	}
	if (p[0x99] > 0 && p[0x9A] > 0 && p[0x9B] < 0) {
		return 1;
	}
	if (p[0x9E] > 0) {
		return 1;
	}
	if (p[0xA4] > 0) {
		return 1;
	}
	if (p[0xA5] > 0) {
		return 1;
	}
	if (p[0xA2] > 0 && p[0xA3] > 0) {
		return 1;
	}
	return 0;
}

// NONMATCHING (96.54%): 2 schedule instructions (BtlUnit_GetPos arg setup) and the pooled
// `double_to_int_802ef890` reloc name (`@667`, resolves once the pool is complete) differ.
s32 _check_present_item(void) {
	BattleWork* wp = _battleWorkPointer;
	s32 item = BattleAudience_GetPresentItemNo();
	s32 unitId;
	BattleUnit* unit;
	f32 z, y, x;

	if (item >= 0x153) {
		return 0;
	}
	unitId = BattleAudience_GetPresentTargetUnitId();
	if (unitId < -1 || unitId >= 0x40) {
		return 0;
	}
	unit = BattleGetUnitPtr(wp, unitId);
	switch (item) {
	case 0:
		return 0;
	case 0x7B:
		BtlUnit_RecoverHp(unit, 1);
		BtlUnit_GetPos(unit, &x, &y, &z);
		y = (f32)BtlUnit_GetHeight(unit) * unit->mSizeMultiplier + float_5_8041c310 + y;
		effRecoveryEntry(0, 1, x, y, z);
		BattleAudience_SetPresentItemNo(0);
		return 0;
	case 0x7C:
		BtlUnit_RecoverFp(unit, 1);
		BtlUnit_GetPos(unit, &x, &y, &z);
		y = (f32)BtlUnit_GetHeight(unit) * unit->mSizeMultiplier + float_5_8041c310 + y;
		effRecoveryEntry(1, 1, x, y, z);
		BattleAudience_SetPresentItemNo(0);
		return 0;
	default:
		break;
	}
	if (item < 0 || item >= 0x153) {
		return 0;
	}
	wp->commandMenu.unk540[0x18] = 1;
	if (wp->commandMenu.unk0 == 0xC || wp->commandMenu.unk0 == 0xD) {
		wp->commandMenu.unk540[0x18] = 2;
	}
	BattleCommandDisplay_AllEnd(wp);
	*(s32*)&wp->commandMenu.unk540[4] = *(s32*)&wp->commandMenu.unk540[0];
	*(s32*)&wp->commandMenu.unk540[0] = 0xC;
	wp->commandMenu.unk0 = 0x1D;
	return 1;
}


s32 _btlcmd_CheckWeaponTargetNum(BattleWork* wp, BattleUnit* unit, BattleWeapon* weapon) {
	u8 work[0xacc];
	s32 belong;

	if (weapon == NULL) {
		return 0;
	}
	if (weapon->mAttackEvtScript == NULL) {
		return 0;
	}
	if (!BtlUnit_CheckWeaponCost(unit, weapon)) {
		return -1;
	}
	belong = BtlUnit_GetEnemyBelong(unit);
	BattleSamplingEnemy(work, weapon, unit->unitId, belong, *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4], -wp->alliances[belong].attackDirection);
	return *(s8*)&work[0xa6c];
}

typedef struct CheckPartyTable {
	s32 kind;    // 0x0
	char* name;  // 0x4
	u16 icon;    // 0x8
	u16 pad;     // 0xA
	char* help;  // 0xC
} CheckPartyTable;

CheckPartyTable _checkPartyTable[8] = {
	{ 0x0000, NULL, 0x0000, 0x0000, NULL },
	{ 0x00E0, "btl_cmd_pt_christine", 0x0155, 0x0000, "btl_hlp_cmd_operation_party1" },
	{ 0x00E1, "btl_cmd_pt_nokotarou", 0x0156, 0x0000, "btl_hlp_cmd_operation_party2" },
	{ 0x00E3, "btl_cmd_pt_clauda", 0x015F, 0x0000, "btl_hlp_cmd_operation_party3" },
	{ 0x00E2, "btl_cmd_pt_yoshi", 0x0158, 0x0000, "btl_hlp_cmd_operation_party4" },
	{ 0x00E4, "btl_cmd_pt_vivian", 0x0160, 0x0000, "btl_hlp_cmd_operation_party5" },
	{ 0x00E5, "btl_cmd_pt_sanders", 0x0157, 0x0000, "btl_hlp_cmd_operation_party6" },
	{ 0x00E6, "btl_cmd_pt_chuchurina", 0x0161, 0x0000, "btl_hlp_cmd_operation_party7" },
};

#pragma dont_inline on // prevents _battleGetPartyIcon from being inlined into _btlcmd_MakePartyTable
s32 _battleGetPartyIcon(s32 kind) {
	CheckPartyTable* entry;
	s32 i;

	for (i = 0; i < 8; i++) {
		entry = &_checkPartyTable[i + 1];
		if (entry->kind == kind) {
			if (entry->kind == 0xE2) {
				switch (pouchGetPartyColor(4)) {
				case 1:
					return 0x159;
				case 2:
					return 0x15A;
				case 3:
					return 0x15B;
				case 4:
					return 0x15C;
				case 5:
					return 0x15D;
				case 6:
					return 0x15E;
				case 0:
				default:
					return 0x158;
				}
			}
			return entry->icon;
		}
	}
	return 0;
}
#pragma dont_inline reset

BOOL BattleCommandCheckChangePositionEnable(BattleWork* wp) {
	BattleUnit* unit;

	if (wp->flags & 0x800000) {
		return FALSE;
	}
	if (wp->flags & 0x40000000) {
		return FALSE;
	}
	unit = BattleGetMarioPtr(wp);
	if (unit == NULL) {
		return FALSE;
	}
	if (unit->moveState != 0) {
		return FALSE;
	}
	if (unit->maxMovesThisTurn <= 0 || unit->maxMovesThisTurn != unit->movesRemaining) {
		return FALSE;
	}
	if (BtlUnit_CheckStatus(unit, STATUS_SLEEP) || BtlUnit_CheckStatus(unit, STATUS_STOP) ||
	    BtlUnit_CheckStatus(unit, STATUS_FREEZE) || BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
		return FALSE;
	}
	if (unit->mAttributeFlags & 0x10) {
		return FALSE;
	}
	unit = BattleGetPartyPtr(wp);
	if (unit == NULL) {
		return FALSE;
	}
	if (unit->moveState != 0) {
		return FALSE;
	}
	if (unit->maxMovesThisTurn <= 0 || unit->maxMovesThisTurn != unit->movesRemaining) {
		return FALSE;
	}
	if (BtlUnit_CheckStatus(unit, STATUS_SLEEP) || BtlUnit_CheckStatus(unit, STATUS_STOP) ||
	    BtlUnit_CheckStatus(unit, STATUS_FREEZE) || BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
		return FALSE;
	}
	return unit->flippedTurns <= 0;
}

// Raw offsets into the enemy-sampling buffer `wp->field_428` (battle_detect owns the layout):
// 0xA6C sample count, 0xA6D.. unit-index codes, entry size 0x24 with s16 unitId at +4, 0xAB7 result.
s32 _btlcmd_SelectWeaponDecide(BattleWork* wp, s32 arg, s32 weaponIdx) {
	u8* work = wp->field_428;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleWeapon* weapon;
	s32 selected;
	s32 i;
	s8 belong;

	_btlcmd_MakeSelectWeaponTable(wp, (void*)arg);
	weapon = ((BattleWorkCommand*)&wp->commandMenu)->mWeaponTable[weaponIdx].mWeaponParams;
	belong = (s8)BtlUnit_GetEnemyBelong(unit);
	BattleSamplingEnemy(work, weapon, wp->activeUnitId, belong, *(s32*)&weapon->field_64[0],
	                    *(s32*)&weapon->field_64[4], (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

	work[0xAB7] = 0;
	if (*(s32*)&weapon->field_64[4] & 0x30000000) {
		s8 ownBelong;

		selected = -1;
		ownBelong = (s8)BtlUnit_GetBelong(unit);
		for (i = 0; i < *(s8*)&work[0xA6C]; i++) {
			s32 offset = ((s8)work[0xA6D + i]) * 0x24 + 4;

			if (*(s32*)&weapon->field_64[4] & 0x10000000) {
				s32 uid = *(s16*)(work + offset);

				if (ownBelong == BtlUnit_GetBelong(BattleGetUnitPtr(wp, uid))) {
					selected = i;
				}
			} else {
				s32 uid = *(s16*)(work + offset);

				if (ownBelong != BtlUnit_GetBelong(BattleGetUnitPtr(wp, uid))) {
					selected = i;
					break;
				}
			}
		}
		if (selected >= 0) {
			work[0xAB7] = selected;
		}
	}
	return 0;
}

// Keeping the original `bl _battleGetPartyIcon` needs the dont_inline pragma on that callee.
// NONMATCHING (98.94%): instruction stream now matches; only callee-saved register
// allocation differs (target keeps mario/cmd in r25/r24 and the loop counter in r20; ours
// use r26/r25 and r24).
s32 _btlcmd_MakePartyTable(BattleWork* wp) {
	BattleUnit* mario = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleUnit* partner = BattleGetPartnerPtr(wp, mario);
	BattleCommand* cmd = &wp->commandMenu;
	s32 i = 0;
	s32 count = 0;
	s32 select = 0;
	PouchData* pouch = pouchGetPtr();
	BattleCursor* cursor;
	CheckPartyTable* entry;
	PouchPartyData* party;
	s32 kind;
	char* name;

	if (cmd == NULL) {
		return 0;
	}
	if (partner == NULL) {
		return 0;
	}
	_btlcmd_GetCursorPtr(cmd, 0xA, &cursor);
	for (i = 0; i < 8; i++) {
		entry = &_checkPartyTable[i];
		kind = entry->kind;
		if (kind == 0) {
			continue;
		}
		party = &pouch->partyData[BattleTransPartyId(kind)];
		if (!(party->flags & 1)) {
			continue;
		}
		((BattleWorkCommand*)cmd)->mPartyTable[count].mPartyId = kind;
		((BattleWorkCommand*)cmd)->mPartyTable[count].mEnabled = 1;
		if (kind == 0xE2) {
			name = pouchGetYoshiName();
		} else {
			name = (char*)msgSearch(entry->name);
		}
		((BattleWorkCommand*)cmd)->mPartyTable[count].mName = name;
		((BattleWorkCommand*)cmd)->mPartyTable[count].mIcon = _battleGetPartyIcon(kind);
		((BattleWorkCommand*)cmd)->mPartyTable[count].mDescription = entry->help;
		((BattleWorkCommand*)cmd)->mPartyTable[count].mCurrentHP = party->currentHP;
		((BattleWorkCommand*)cmd)->mPartyTable[count].mMaxHP = party->maximumHP;
		if (mario->currentKind == kind || BattleGetPartnerPtr(wp, mario)->currentKind == kind) {
			((BattleWorkCommand*)cmd)->mPartyTable[count].mEnabled = 0;
			select = count;
			if (mario->currentKind == kind) {
				((BattleWorkCommand*)cmd)->mPartyTable[count].mCurrentHP = mario->currentHp;
			} else {
				((BattleWorkCommand*)cmd)->mPartyTable[count].mCurrentHP = partner->currentHp;
			}
		} else if (party->currentHP <= 0) {
			((BattleWorkCommand*)cmd)->mPartyTable[count].mEnabled = 0;
		}
		if (party->flags & 2) {
			((BattleWorkCommand*)cmd)->mPartyTable[count].mEnabled = 0;
		}
		count++;
	}
	cursor->unk8 = count;
	cursor->field_0 = select;
	return count;
}

// NONMATCHING (98.44%): instruction sequence now matches; only the pooled string base
// (`str_802EF640` vs local `...rodata.0`, offsets shifted 0x28) and the msg-base/args
// register swap (target r5/r6, ours r6/r5) differ, both expected to resolve once the
// file's rodata pool is complete.
s32 _get_msg(EventEntry* event) {
	char* msg = "btl_wn_dummy_noitem";
	BattleWork* wp = _battleWorkPointer;
	s32 index = event->args[0];

	switch (*(s32*)&wp->commandMenu.unk560[4]) {
	case 1:
		msg = "btl_msg_cmderror_no_fp";
		break;
	case 2:
		msg = "btl_msg_cmderror_no_ap";
		break;
	case 4:
		msg = "btl_msg_cmderror_no_jump_target";
		break;
	case 5:
		msg = "btl_msg_cmderror_no_hammer_target";
		break;
	case 6:
		msg = "btl_msg_cmderror_no_escape";
		break;
	default:
		msg = "btl_msg_cmderror_no_select_now";
		break;
	}
	evtSetValue(event, index, (s32)msg);
	return 2;
}
