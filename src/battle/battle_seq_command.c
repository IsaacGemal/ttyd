#include "battle/battle_seq_command.h"
#include "battle/battle.h"
#include "battle/battle_mario.h"
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
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "manager/fontmgr.h"
#include "driver/windowdrv.h"
#include "pmario_sound.h"
#include "data/item_data.h"
#include <string.h>
#include <stdio.h>
#include "battle/battle_database_common.h"
#include "battle/battle_actrecord.h"
#include "battle/battle_attack_audience.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_ac_help.h"
#include "system.h"
#include "battle/battle_message.h"
#include "evt/evt_cmd.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern s32 _jump_badge_table[8];
extern s32 _hammer_badge_table[9];
extern CheckPartyTable _checkPartyTable[8];

extern ItemData itemDataTable[];

// Battle weapon data used by _btlcmd_SetAttackEvent; not declared in any header.
extern BattleWeapon ItemWeaponData_Naniga_Okorukana;


//TODO: move into battle/seq/?

void BattleCommandInit(BattleWork* wp) {
	wp->commandMenu.commandState = 0;
	wp->flags |= 0x80;
}

// Source position matters: the compiler emits functions in reverse source order, so among
// the table builders this one is emitted last and its `.rodata` strings pool last; the
// shared int->float/SFX block from the later BattleCommandDisplay_* functions pools first.
// Keep it ahead of _btlcmd_MakeOperationTable/_btlcmd_MakeMultiItemTable in the source.
void _btlcmd_MakeActClassTable(BattleWork* wp) {
	BattleUnit* mario = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd = &wp->commandMenu;
	PouchData* pouch = pouchGetPtr();
	BattleCursor* cursor;
	u32 count;
	NpcBattleInfo* setup = wp->fbatInfo->setup;

	if (cmd == NULL) {
		return;
	}
	if (mario->currentKind == 0xDE) {
		cursor = &cmd->cursor[0];
	} else {
		cursor = &cmd->cursor[1];
	}
	count = 0;
	if (mario->currentKind == 0xDE) {
		cmd->actionTable[0].mType = 0;
		cmd->actionTable[0].mEnabled = 1;
		cmd->actionTable[0].mDisableReason = 0;
		if (mario->mAttributeFlags & 0x10) {
			cmd->actionTable[0].mEnabled = 0;
			cmd->actionTable[0].mDisableReason = 3;
		}
		if (pouch->jumpLevel < 1) {
			cmd->actionTable[0].mEnabled = 0;
		}
		if (mario->badgesEquipped.Hammerman != 0) {
			cmd->actionTable[0].mEnabled = 0;
			cmd->actionTable[0].mDisableReason = 3;
		}
		if (*(s32*)&wp->field_18FF0[0x48] /* 0x19038 */ > 0) {
			cmd->actionTable[0].mEnabled = 0;
			cmd->actionTable[0].mDisableReason = 3;
		}
		if (setup->flags & NPC_BATTLE_NO_JUMP) {
			cmd->actionTable[0].mEnabled = 0;
			cmd->actionTable[0].mDisableReason = 3;
		}
		if (wp->flags & 0x06800000) {
			cmd->actionTable[0].mEnabled = 0;
			cmd->actionTable[0].mDisableReason = 3;
		}
		cmd->actionTable[0].mName = (char*)msgSearch("btl_cmd_act_jump");
		switch (pouch->jumpLevel) {
		case 1:
			cmd->actionTable[0].mIcon = 0x146;
			break;
		case 2:
			cmd->actionTable[0].mIcon = 0x147;
			break;
		default:
			cmd->actionTable[0].mIcon = 0x148;
			break;
		}
		count = 1;
	}
	if (mario->currentKind >= 0xE0 && mario->currentKind < 0xE7) {
		cmd->actionTable[count].mType = 6;
		if (mario->flippedTurns) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		} else {
			cmd->actionTable[count].mEnabled = 1;
			cmd->actionTable[count].mDisableReason = 0;
		}
		if (*(s32*)&wp->field_18FF0[0x54] /* 0x19044 */ > 0) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		if (wp->flags & 0x04800000) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		cmd->actionTable[count].mName = (char*)msgSearch("btl_cmd_act_tech_party");
		cmd->actionTable[count].mIcon = 0x14F;
		count++;
	}
	if (mario->currentKind == 0xDE && pouch->hammerLevel >= 1) {
		cmd->actionTable[count].mType = 1;
		cmd->actionTable[count].mEnabled = 1;
		cmd->actionTable[count].mDisableReason = 0;
		if (mario->mAttributeFlags & 0x10) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		if (pouch->hammerLevel < 1) {
			cmd->actionTable[count].mEnabled = 0;
		}
		if (mario->badgesEquipped.Jumpman != 0) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		if (*(s32*)&wp->field_18FF0[0x4C] /* 0x1903C */ > 0) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		if (setup->flags & NPC_BATTLE_NO_HAMMER) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		if (wp->flags & 0x05800000) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		cmd->actionTable[count].mName = (char*)msgSearch("btl_cmd_act_hammer");
		switch (pouch->hammerLevel) {
		case 1:
			cmd->actionTable[count].mIcon = 0x149;
			break;
		case 2:
			cmd->actionTable[count].mIcon = 0x14A;
			break;
		default:
			cmd->actionTable[count].mIcon = 0x14B;
			break;
		}
		count++;
	}
	if (mario->currentKind == 0xDE || (mario->currentKind >= 0xE0 && mario->currentKind < 0xE7)) {
		cmd->actionTable[count].mType = 2;
		if (mario->flippedTurns) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		} else {
			cmd->actionTable[count].mEnabled = 1;
			cmd->actionTable[count].mDisableReason = 0;
		}
		if (mario->currentKind == 0xDE) {
			if (*(s32*)&wp->field_18FF0[0x50] /* 0x19040 */ > 0) {
				cmd->actionTable[count].mEnabled = 0;
				cmd->actionTable[count].mDisableReason = 3;
			}
		} else {
			if (*(s32*)&wp->field_18FF0[0x58] /* 0x19048 */ > 0) {
				cmd->actionTable[count].mEnabled = 0;
				cmd->actionTable[count].mDisableReason = 3;
			}
		}
		if (wp->flags & 0x1F800000) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		cmd->actionTable[count].mName = (char*)msgSearch("btl_cmd_act_item");
		cmd->actionTable[count].mIcon = 0x14C;
		count++;
	}
	if (mario->currentKind == 0xDE && pouch->mStarPowersObtained != 0) {
		cmd->actionTable[count].mType = 4;
		cmd->actionTable[count].mEnabled = 1;
		cmd->actionTable[count].mDisableReason = 0;
		if (mario->mAttributeFlags & 0x10) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		if (wp->flags & 0x1B800000) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		cmd->actionTable[count].mName = (char*)msgSearch("btl_cmd_act_super_action");
		cmd->actionTable[count].mIcon = 0x14D;
		count++;
	}
	if (!(wp->flags & 0x08000000)) {
		cmd->actionTable[count].mType = 5;
		cmd->actionTable[count].mEnabled = 1;
		cmd->actionTable[count].mDisableReason = 0;
		if (wp->flags & 0x1F800000) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		cmd->actionTable[count].mName = (char*)msgSearch("btl_cmd_act_operation");
		cmd->actionTable[count].mIcon = 0x14E;
		count++;
	}
	if (wp->flags & 0x08000000) {
		cmd->actionTable[count].mType = 0xD;
		cmd->actionTable[count].mEnabled = 1;
		cmd->actionTable[count].mDisableReason = 0;
		cmd->actionTable[count].mName = (char*)msgSearch("btl_cmd_act_lecture_end");
		cmd->actionTable[count].mIcon = 0x14E;
		if (wp->flags & 0x10800000) {
			cmd->actionTable[count].mEnabled = 0;
			cmd->actionTable[count].mDisableReason = 3;
		}
		count++;
	}
	cursor->unk8 = count;
	// The original compares these signed (cmpw), though the header declares them u32.
	if ((s32)cursor->field_0 >= (s32)cursor->unk8) {
		cursor->field_0 = cursor->unk8 - 1;
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

s32 _btlcmd_MakeSelectWeaponTable(BattleWork* wp, s32 arg) {
	static BattleWeapon defaultWeapon_Dummy_NoItem = {
		"btl_wn_dummy_noitem",
		0,
		(ItemType)0,
		"msg_dummy_noitem",
		0x64, 0, 0, 0,
		0.0f,
		0, 0, 0, 0,
		NULL,
		{0, 0, 0, 0, 0, 0, 0, 0},
		NULL,
		{0, 0, 0, 0, 0, 0, 0, 0},
		0x01000160,
		0x20000002,
		0, 0, 3, 2,
		NULL,
		0,
		0,
		0x80002000,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		NULL,
		{0, 0, 0, 0x64},
	};
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd = &wp->commandMenu;
	PouchData* pouch = pouchGetPtr();
	BattleCursor* cursor;
	u32 i;
	s32 badge;
	s32 count;
	s32 j;
	u16 mask;
	s16* items;
	s32 id;
	s32 field_4;
	ItemData* item;

	if (cmd == NULL) {
		return 0;
	}
	_btlcmd_GetCursorPtr(cmd, arg, &cursor);
	count = 0;
	switch (arg) {
	case 0:
		if (pouch->jumpLevel >= 1) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mWeaponParams = &marioDefaultWeapon_Jump;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = _getJumpIconId();
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			if ((wp->flags & 0x09000000) != 0 && wp->field_424 != -1) {
				cmd->weaponTable[count].mWeaponParams = BtlUnit_GetData(BattleGetUnitPtr(wp, wp->field_424), 0x44);
			}
			count++;
		}
		if (pouch->jumpLevel >= 2) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mWeaponParams = &marioWeapon_KururinJump;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = _getJumpIconId();
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		if (pouch->jumpLevel >= 3) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mWeaponParams = &marioWeapon_JyabaraJump;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = _getJumpIconId();
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		break;
	case 1:
		if (pouch->hammerLevel >= 1) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mWeaponParams = &marioDefaultWeapon_Hammer;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = _getHammerIconId();
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			if ((wp->flags & 0x0A000000) != 0 && wp->field_424 != -1) {
				cmd->weaponTable[count].mWeaponParams = BtlUnit_GetData(BattleGetUnitPtr(wp, wp->field_424), 0x45);
			}
			count++;
		}
		if (pouch->hammerLevel >= 2) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mWeaponParams = &marioWeapon_KaitenHammer;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = _getHammerIconId();
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		if (pouch->hammerLevel >= 3) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mWeaponParams = &marioWeapon_UltraHammer;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = _getHammerIconId();
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		break;
	}
	switch (arg) {
	case 0:
		for (i = 0; i < 8; i++) {
			badge = _jump_badge_table[i];
			if (pouchEquipCheckBadge(badge) > 0) {
				cmd->weaponTable[count].mIndex = i;
				cmd->weaponTable[count].mItem = badge;
				cmd->weaponTable[count].mWeaponParams = itemDataTable[badge].battle_params;
				cmd->weaponTable[count].mEnabled = 0;
				cmd->weaponTable[count].mDisableReason = 0;
				if (cmd->weaponTable[count].mWeaponParams->item != 0) {
					cmd->weaponTable[count].mIcon = *(u16*)&itemDataTable[badge].icon_id;
				} else {
					cmd->weaponTable[count].mIcon = *(u16*)&cmd->weaponTable[count].mWeaponParams->icon;
				}
				if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
					cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
				} else {
					cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
				}
				count++;
			}
		}
		break;
	case 1:
	{
		u32 i;
		s32 badge;
		for (i = 0; i < 9; i++) {
			badge = _hammer_badge_table[i];
			if (pouchEquipCheckBadge(badge) > 0) {
				cmd->weaponTable[count].mIndex = i;
				cmd->weaponTable[count].mItem = badge;
				cmd->weaponTable[count].mWeaponParams = itemDataTable[badge].battle_params;
				cmd->weaponTable[count].mEnabled = 0;
				cmd->weaponTable[count].mDisableReason = 0;
				if (cmd->weaponTable[count].mWeaponParams->item != 0) {
					cmd->weaponTable[count].mIcon = *(u16*)&itemDataTable[badge].icon_id;
				} else {
					cmd->weaponTable[count].mIcon = *(u16*)&cmd->weaponTable[count].mWeaponParams->icon;
				}
				if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
					cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
				} else {
					cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
				}
				count++;
			}
		}
		break;
	}
	}
	switch (arg) {
	case 6:
		{ void (*fn)(BattleCommand*, void*) = (void (*)(BattleCommand*, void*))BtlUnit_GetData(unit, 1); fn(cmd, &count); }
		break;
	case 2:
		items = pouch->heldItems;
		for (j = 0; j < 20; j++) {
			id = items[j];
			item = &itemDataTable[id];
			if (id != 0 && (item->usable_locations & 2) != 0) {
				cmd->weaponTable[count].mIndex = j;
				cmd->weaponTable[count].mItem = id;
				cmd->weaponTable[count].mWeaponParams = item->battle_params;
				cmd->weaponTable[count].mEnabled = 0;
				cmd->weaponTable[count].mDisableReason = 0;
				if (cmd->weaponTable[count].mWeaponParams == NULL) {
					cmd->weaponTable[count].mWeaponParams = &ItemWeaponData_CookingItem;
					cmd->weaponTable[count].mName = (char*)msgSearch(item->name);
					cmd->weaponTable[count].mIcon = *(u16*)&item->icon_id;
				} else {
					cmd->weaponTable[count].mIcon = *(u16*)&item->icon_id;
					if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
						cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
					} else {
						cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
					}
				}
				count++;
				if (count >= 0x15) {
					break;
				}
			}
		}
		if (count == 0) {
			cmd->weaponTable[count].mIndex = -1;
			cmd->weaponTable[count].mItem = 0;
			cmd->weaponTable[count].mIndex = 0;
			cmd->weaponTable[count].mWeaponParams = &defaultWeapon_Dummy_NoItem;
			cmd->weaponTable[count].mEnabled = 0;
			cmd->weaponTable[count].mDisableReason = 0;
			cmd->weaponTable[count].mIcon = 0x162;
			if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
				cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
			} else {
				cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		break;
	case 4:
	{
		s32 j;
		mask = pouch->mStarPowersObtained;
		for (j = 0; j < 16; j++) {
			if ((mask & (1 << j)) != 0 && ((BattleWeapon**)superActionTable)[j] != NULL) {
				cmd->weaponTable[count].mIndex = -1;
				cmd->weaponTable[count].mItem = 0;
				cmd->weaponTable[count].mWeaponParams = ((BattleWeapon**)superActionTable)[j];
				cmd->weaponTable[count].mEnabled = 0;
				cmd->weaponTable[count].mIcon = *(u16*)&((BattleWeapon**)superActionTable)[j]->icon;
				cmd->weaponTable[count].mDisableReason = 0;
				if (cmd->weaponTable[count].mWeaponParams->name != NULL) {
					cmd->weaponTable[count].mName = (char*)msgSearch(cmd->weaponTable[count].mWeaponParams->name);
				} else {
					cmd->weaponTable[count].mName = (char*)msgSearch(itemDataTable[cmd->weaponTable[count].mWeaponParams->item].name);
				}
				count++;
				if (count >= 0x15) {
					break;
				}
			}
		}
		break;
	}
	}
	if (count <= 0) {
		return 0;
	}
	cursor->unk8 = count;
	if (cursor->field_0 >= cursor->unk8) {
		cursor->field_0 = cursor->unk8 - 1;
	}
	while (cursor->unk8 < cursor->field_4 + 6) {
		field_4 = cursor->field_4;
		if (field_4 <= 0) {
			break;
		}
		cursor->field_4--;
	}
	_btlcmd_UpdateSelectWeaponTable(wp, arg);
	return count;
}

void _btlcmd_UpdateSelectWeaponTable(BattleWork* wp, s32 arg) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;
	s32 i;
	s32 count;

	_btlcmd_GetCursorPtr(&wp->commandMenu, arg, &cursor);
	count = cursor->unk8;
	for (i = 0; i < count; i++) {
		BattleWeapon* params;
		s32 check;

		params = cmd->weaponTable[i].mWeaponParams;
		cmd->weaponTable[i].mEnabled = 0;
		cmd->weaponTable[i].mDisableReason = 0;
		if (params == NULL) {
			continue;
		}
		check = _btlcmd_CheckWeaponTargetNum(wp, unit, params);
		if (check <= 0) {
			if (check == 0) {
				switch (arg) {
				case 0:
					cmd->weaponTable[i].mDisableReason = 4;
					break;
				case 1:
					cmd->weaponTable[i].mDisableReason = 5;
					break;
				default:
					cmd->weaponTable[i].mDisableReason = 3;
					break;
				}
			} else {
				if (arg != 4) {
					cmd->weaponTable[i].mDisableReason = 1;
				} else {
					cmd->weaponTable[i].mDisableReason = 2;
				}
			}
		} else {
			cmd->weaponTable[i].mEnabled = 1;
			cmd->weaponTable[i].mDisableReason = 0;
		}
	}
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
	BattleSamplingEnemy(work, weapon, unit->unitId, belong, weapon->targetClassFlags, weapon->targetPropertyFlags, -wp->alliances[belong].attackDirection);
	return *(s8*)&work[0xa6c];
}

#pragma dont_inline on
// Source position matters: emitted after _btlcmd_MakeMultiItemTable and before
// _btlcmd_MakeActClassTable, so the "btl_cmd_op_*" strings pool between their blocks.
void _btlcmd_MakeOperationTable(BattleWork* wp) {
	BattleWeapon* weapon;
	NpcBattleInfo* setup;
	BattleUnit* mario;
	BattleCommand* cmd;
	PouchData* pouch;
	BattleCursor* cursor;
	BattleUnit* party;
	u32 count;
	s32 escapeIdx;

	mario = BattleGetUnitPtr(wp, wp->activeUnitId);
	cmd = &wp->commandMenu;
	setup = wp->fbatInfo->setup;
	pouch = pouchGetPtr();

	if (cmd == NULL) {
		return;
	}
	_btlcmd_GetCursorPtr(cmd, 5, &cursor);
	count = 0;
	if (_btlcmd_MakePartyTable(wp) >= 2) {
		cmd->operationTable[0].mType = 0;
		if (mario->flippedTurns) {
			cmd->operationTable[0].mEnabled = 0;
			cmd->operationTable[0].mDisableReason = 3;
		} else {
			cmd->operationTable[0].mEnabled = 1;
			cmd->operationTable[0].mDisableReason = 0;
		}
		cmd->operationTable[0].mName = (char*)msgSearch("btl_cmd_op_chg_party");
		party = mario;
		if (!(mario->currentKind >= 0xE0 && mario->currentKind < 0xE7)) {
			party = BattleGetPartnerPtr(wp, mario);
		}
		BtlUnit_CanActStatus(party);
		cmd->operationTable[0].mIcon = _battleGetPartyIcon(party->currentKind);
		cmd->operationTable[0].mDescription = "btl_hlp_cmd_operation_changeparty";
		if (mario->mAttributeFlags & 0x10) {
			cmd->operationTable[0].mEnabled = 0;
			cmd->operationTable[0].mDisableReason = 3;
		}
		if (setup->flags & NPC_BATTLE_NO_PARTY_CHANGE) {
			cmd->operationTable[0].mEnabled = 0;
			cmd->operationTable[0].mDisableReason = 3;
		}
		cmd->operationTable[0].mWeaponCost = 0;
		count = 1;
	}
	if (pouch->mStarPowersObtained != 0) {
		cmd->operationTable[count].mType = 5;
		cmd->operationTable[count].mEnabled = 1;
		cmd->operationTable[count].mName = (char*)msgSearch("btl_cmd_op_appeal");
		cmd->operationTable[count].mIcon = 0x164;
		cmd->operationTable[count].mDisableReason = 0;
		cmd->operationTable[count].mWeaponCost = 0;
		cmd->operationTable[count].mDescription = "btl_hlp_cmd_operation_appeal";
		count++;
	}
	if (mario->badgesEquipped.Charge) {
		weapon = mario->currentKind == 0xDE ? &badgeWeapon_Charge : &badgeWeapon_ChargeP;
		cmd->operationTable[count].mType = 1;
		cmd->operationTable[count].mEnabled = BtlUnit_CheckWeaponCost(mario, weapon);
		cmd->operationTable[count].mWeaponCost = BtlUnit_GetWeaponCost(mario, weapon);
		if (cmd->operationTable[count].mEnabled != 0) {
			cmd->operationTable[count].mDisableReason = 0;
		} else {
			cmd->operationTable[count].mDisableReason = 1;
		}
		cmd->operationTable[count].mName = (char*)msgSearch(weapon->name);
		cmd->operationTable[count].mIcon = 0x166;
		cmd->operationTable[count].mDescription = (char*)weapon->description;
		if (mario->mAttributeFlags & 0x10) {
			cmd->operationTable[count].mEnabled = 0;
			cmd->operationTable[count].mDisableReason = 3;
		}
		count++;
	}
	if (mario->badgesEquipped.SuperCharge) {
		weapon = mario->currentKind == 0xDE ? &badgeWeapon_SuperCharge : &badgeWeapon_SuperChargeP;
		cmd->operationTable[count].mType = 2;
		cmd->operationTable[count].mEnabled = BtlUnit_CheckWeaponCost(mario, weapon);
		cmd->operationTable[count].mWeaponCost = BtlUnit_GetWeaponCost(mario, weapon);
		if (cmd->operationTable[count].mEnabled != 0) {
			cmd->operationTable[count].mDisableReason = 0;
		} else {
			cmd->operationTable[count].mDisableReason = 1;
		}
		cmd->operationTable[count].mName = (char*)msgSearch(weapon->name);
		cmd->operationTable[count].mIcon = 0x167;
		cmd->operationTable[count].mDescription = (char*)weapon->description;
		if (mario->mAttributeFlags & 0x10) {
			cmd->operationTable[count].mEnabled = 0;
			cmd->operationTable[count].mDisableReason = 3;
		}
		count++;
	}
	cmd->operationTable[count].mType = 6;
	cmd->operationTable[count].mEnabled = 1;
	cmd->operationTable[count].mName = (char*)msgSearch("btl_cmd_op_defend");
	cmd->operationTable[count].mIcon = 0x165;
	cmd->operationTable[count].mDisableReason = 0;
	cmd->operationTable[count].mWeaponCost = 0;
	cmd->operationTable[count].mDescription = "btl_hlp_cmd_operation_defend";
	if (mario->mAttributeFlags & 0x10) {
		cmd->operationTable[count].mEnabled = 0;
		cmd->operationTable[count].mDisableReason = 3;
	}
	escapeIdx = count + 1;
	cmd->operationTable[escapeIdx].mType = 3;
	cmd->operationTable[escapeIdx].mEnabled = BtlUnit_CanActStatus(mario);
	cmd->operationTable[escapeIdx].mName = (char*)msgSearch("btl_cmd_op_escape");
	cmd->operationTable[escapeIdx].mIcon = 0x163;
	cmd->operationTable[escapeIdx].mWeaponCost = 0;
	if (cmd->operationTable[escapeIdx].mEnabled != 0) {
		cmd->operationTable[escapeIdx].mDisableReason = 0;
	} else {
		cmd->operationTable[escapeIdx].mDisableReason = 1;
	}
	cmd->operationTable[escapeIdx].mDescription = "btl_hlp_cmd_operation_escape";
	if (mario->mAttributeFlags & 0x10) {
		cmd->operationTable[escapeIdx].mEnabled = 0;
		cmd->operationTable[escapeIdx].mDisableReason = 3;
	}
	if (wp->fbatInfo->setup->flags & NPC_BATTLE_NO_ESCAPE) {
		cmd->operationTable[escapeIdx].mEnabled = 0;
		cmd->operationTable[escapeIdx].mDisableReason = 3;
	}
	cursor->unk8 = count + 2;
	if (cursor->field_0 >= cursor->unk8) {
		cursor->field_0 = cursor->unk8 - 1;
	}
}

// Source position matters: emitted before _btlcmd_MakeOperationTable and after the later
// BattleCommandDisplay_* menu functions, so "btl_cmd_act_item"/"in_3kai_item_p"/
// "msg_3kai_item_p" pool after the shared int->float/SFX block and before the
// "btl_cmd_op_*" strings. `count = 0; ...; count++` keeps the `mulli` alive.
void _btlcmd_MakeMultiItemTable(BattleWork* wp) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd;
	u32 count;

	if ((cmd = &wp->commandMenu)) {
		BattleCursor* cursor;
		_btlcmd_GetCursorPtr(cmd, 3, &cursor);
		count = 0;
		cmd->multiItemTable[count].mKind = 1;
		cmd->multiItemTable[count].mEnabled = 1;
		cmd->multiItemTable[count].mName = (char*)msgSearch("btl_cmd_act_item");
		cmd->multiItemTable[count].mIcon = 0x14C;
		cmd->multiItemTable[count].mDescription = NULL;
		cmd->multiItemTable[count].mWeaponCost = 0;
		count++;
		if (unit->badgesEquipped.DoubleDip) {
			cmd->multiItemTable[count].mKind = 2;
			if (unit->currentKind == UNIT_MARIO) {
				cmd->multiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_2KaiItem);
				cmd->multiItemTable[count].mName = (char*)msgSearch(itemDataTable[254].name);
				cmd->multiItemTable[count].mIcon = *(u16*)&itemDataTable[254].icon_id;
				cmd->multiItemTable[count].mDescription = (char*)itemDataTable[254].description;
				cmd->multiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_2KaiItem);
			} else {
				cmd->multiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_2KaiItem);
				cmd->multiItemTable[count].mName = (char*)msgSearch(itemDataTable[255].name);
				cmd->multiItemTable[count].mIcon = *(u16*)&itemDataTable[255].icon_id;
				cmd->multiItemTable[count].mDescription = (char*)itemDataTable[255].description;
				cmd->multiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_2KaiItem);
			}
			if (cmd->multiItemTable[count].mEnabled == 0) {
				cmd->multiItemTable[count].mDisableReason = 1;
			} else {
				cmd->multiItemTable[count].mDisableReason = 0;
			}
			count = 2;
		}
		if (unit->badgesEquipped.TripleDip) {
			cmd->multiItemTable[count].mKind = 3;
			if (unit->currentKind == UNIT_MARIO) {
				cmd->multiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_3KaiItem);
				cmd->multiItemTable[count].mName = (char*)msgSearch(itemDataTable[329].name);
				cmd->multiItemTable[count].mIcon = *(u16*)&itemDataTable[329].icon_id;
				cmd->multiItemTable[count].mDescription = (char*)itemDataTable[329].description;
				cmd->multiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_3KaiItem);
			} else {
				cmd->multiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_3KaiItem);
				cmd->multiItemTable[count].mName = (char*)msgSearch("in_3kai_item_p");
				cmd->multiItemTable[count].mIcon = 0x62;
				cmd->multiItemTable[count].mDescription = "msg_3kai_item_p";
				cmd->multiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_3KaiItem);
			}
			if (cmd->multiItemTable[count].mEnabled == 0) {
				cmd->multiItemTable[count].mDisableReason = 1;
			} else {
				cmd->multiItemTable[count].mDisableReason = 0;
			}
			count++;
		}
		cursor->unk8 = count;
		if (cursor->field_0 >= cursor->unk8) {
			cursor->field_0 = cursor->unk8 - 1;
		}
	}
}

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

// Declaration order is significant here: it fixes the callee-saved register
// allocation (entry must be declared before the others to land in r26).
s32 _btlcmd_MakePartyTable(BattleWork* wp) {
	CheckPartyTable* entry;
	BattleUnit* mario = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleUnit* partner = BattleGetPartnerPtr(wp, mario);
	BattleCommand* cmd = &wp->commandMenu;
	s32 count = 0;
	s32 select = 0;
	PouchData* pouch = pouchGetPtr();
	BattleCursor* cursor;
	PouchPartyData* party;
	s32 i = 0;
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
		cmd->partyTable[count].mPartyId = kind;
		cmd->partyTable[count].mEnabled = 1;
		if (kind == 0xE2) {
			name = pouchGetYoshiName();
		} else {
			name = (char*)msgSearch(entry->name);
		}
		cmd->partyTable[count].mName = name;
		cmd->partyTable[count].mIcon = _battleGetPartyIcon(kind);
		cmd->partyTable[count].mDescription = entry->help;
		cmd->partyTable[count].mCurrentHP = party->currentHP;
		cmd->partyTable[count].mMaxHP = party->maximumHP;
		if (mario->currentKind == kind || BattleGetPartnerPtr(wp, mario)->currentKind == kind) {
			cmd->partyTable[count].mEnabled = 0;
			select = count;
			if (mario->currentKind == kind) {
				cmd->partyTable[count].mCurrentHP = mario->currentHp;
			} else {
				cmd->partyTable[count].mCurrentHP = partner->currentHp;
			}
		} else if (party->currentHP <= 0) {
			cmd->partyTable[count].mEnabled = 0;
		}
		if (party->flags & 2) {
			cmd->partyTable[count].mEnabled = 0;
		}
		count++;
	}
	cursor->unk8 = count;
	cursor->field_0 = select;
	return count;
}

// Raw offsets into the enemy-sampling buffer `wp->sampling` (battle_detect owns the layout):
// 0xA6C sample count, 0xA6D.. unit-index codes, entry size 0x24 with s16 unitId at +4, 0xAB7 result.
#pragma dont_inline on // the original calls _btlcmd_SelectWeaponDecide out of line from BattleSetConfuseAct
s32 _btlcmd_SelectWeaponDecide(BattleWork* wp, s32 arg, s32 weaponIdx) {
	u8* work = (u8*)&wp->sampling;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleWeapon* weapon;
	s32 selected;
	s32 i;
	s8 belong;

	_btlcmd_MakeSelectWeaponTable(wp, arg);
	weapon = wp->commandMenu.weaponTable[weaponIdx].mWeaponParams;
	belong = (s8)BtlUnit_GetEnemyBelong(unit);
#ifdef MUST_MATCH
	// the field is already s8; the no-op cast changes the scheduling (98.75% without it)
	BattleSamplingEnemy(work, weapon, wp->activeUnitId, belong, weapon->targetClassFlags,
	                    weapon->targetPropertyFlags, (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);
#else
	BattleSamplingEnemy(work, weapon, wp->activeUnitId, belong, weapon->targetClassFlags,
	                    weapon->targetPropertyFlags, wp->alliances[unit->mAlliance].attackDirection);
#endif

	work[0xAB7] = 0;
	if (weapon->targetPropertyFlags & 0x30000000) {
		s8 ownBelong;

		selected = -1;
		ownBelong = (s8)BtlUnit_GetBelong(unit);
		for (i = 0; i < *(s8*)&work[0xA6C]; i++) {
			s32 offset = ((s8)work[0xA6D + i]) * 0x24 + 4;

			if (weapon->targetPropertyFlags & 0x10000000) {
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

s32 BattleCommandInput(BattleWork* wp) {
	s32 unitId;
	s32 partId;
	const char* sfx;
	const char* sfx2;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd = &wp->commandMenu;
	s32 result;
	s32 action;
	BattleSampling* sampling;
	BattleCursor* cursor;
	EventEntry* event;
	s32 state;
	s32 i;

	if (cmd == NULL) {
		return 1;
	}
	_btlcmd_GetCursorPtr(cmd, 0xE, &cursor);
	if (_check_present_item()) {
		BattleAcHelpSetDispType(0);
	}
	_commandRestoreRec();
	result = 1;
	sfx = (0, "SFX_BTL_CURSOR_MOVE2");
	sfx2 = (0, "SFX_BTL_CURSOR_MOVE2");

	while (result) {
		result = 0;
		for (;;) {
			state = cmd->commandState;
			switch (state) {
		case 0:
			if (wp->commandMenu.restoreState != 0) {
				switch (cmd->actKind) {
				case 0xB:
				case 0xC:
					cmd->actKind = cmd->savedActKind;
					break;
				}
				if (wp->commandMenu.restoreState == 2) {
					wp->commandMenu.restoreState = 0;
				}
			}
			cmd->cancelled = 0;
			wp->dispFlags |= 1;
			BtlUnit_SetCommandAnimPose(unit);
			if (unit->moveState >= 5 && unit->moveState <= 9) {
				BattleStageOnLightInCommand();
				BattleStageOffLightFriendInCommand();
				BattleStageOffLightEnemyInCommand();
				cmd->commandState = 5;
				break;
			}
			BattleCommandDisplay_ActMenuSetup(wp, 0);
			cmd->commandState = 2;
			BattleStageOnLightInCommand();
			BattleStageOffLightFriendInCommand();
			BattleStageOffLightEnemyInCommand();
			break;
		case 1:
			BattleCommandDisplay_ActMenuSetup(wp, (void*)1);
			cmd->commandState = 2;
			break;
		case 2:
			if (BattleCommandDisplay_ActMenuMain(wp)) {
				result = 1;
			}
			break;
		case 3:
			cmd->commandState = 4;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			cmd->eventId = event->eventId;
			// fallthrough
		case 4:
			if (evtCheckID(cmd->eventId) != 0) {
				break;
			}
			cmd->eventId = 0;
			result = 1;
			cmd->commandState = 1;
			break;
		case 5:
		case 6:
			{
			s32 a = cmd->actKind;
			if ((a == 0 && (wp->flags & 0x200)) ||
			    (a == 1 && (wp->flags & 0x400))) {
				_btlcmd_SelectWeaponDecide(wp, a, 0);
				BattleCommandDisplay_TargetSelectMenuSetup(wp);
				cmd->commandState = 0xA;
				BattleStageOnLightEnemyInCommand();
				break;
			}
			if (state == 5) {
				if (a == 2 && (unit->badgesEquipped.DoubleDip || unit->badgesEquipped.TripleDip)) {
					BattleCommandDisplay_WeaponSelectMenuSetup(wp, a, 0);
				} else {
					BattleCommandDisplay_WeaponSelectMenuSetup(wp, a, 1);
				}
			} else {
				BattleCommandDisplay_WeaponSelectMenuSetup(wp, a, 0);
			}
			}
			BattleStageOffLightFriendInCommand();
			BattleStageOffLightEnemyInCommand();
			cmd->commandState = 7;
			_commandRestoreRec();
			// fallthrough
		case 7:
			if (BattleCommandDisplay_WeaponSelectMenuMain(wp, cmd->actKind)) {
				result = 1;
			}
			break;
		case 8:
			cmd->commandState = 9;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			cmd->eventId = event->eventId;
			// fallthrough
		case 9:
			if (evtCheckID(cmd->eventId) != 0) {
				break;
			}
			cmd->eventId = 0;
			result = 1;
			cmd->commandState = 6;
			break;
		case 12:
			cmd->commandState = 0xD;
			BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(wp);
			_commandRestoreRec();
			break;
		case 13:
			if (BattleCommandDisplay_WeaponMultiItemCancelCheckMain(wp)) {
				result = 1;
			}
			break;
		case 10:
			_btlcmd_GetCursorPtr(cmd, cmd->actKind, &cursor);
			BattleAcHelpSetHelp(cmd->weaponTable[cursor->field_0].mWeaponParams->acHelpMsg);
			BattleAcHelpSetDispType(1);
			cmd->commandState = 0xB;
			break;
		case 11:
			action = cmd->actKind;
			sampling = &wp->sampling;
			_btlcmd_GetCursorPtr(cmd, action, &cursor);
			{
				BattleSamplingEntry* entry = &sampling->entries[sampling->order[sampling->selectIdx]];

				unitId = entry->unitId;
				partId = entry->partId;
			}
			BattleSamplingEnemyUpdate((u8*)sampling);
			for (i = 0; i < sampling->count; i++) {
				BattleSamplingEntry* entry = &sampling->entries[sampling->order[i]];

				if (entry->unitId == unitId && entry->partId == partId) {
					break;
				}
			}
			if (i >= sampling->count) {
				i = 0;
			}
			sampling->selectIdx = i;
			if (cmd->weaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x01000000) {
				if (BattlePadCheckRepeat(0x80000)) {
					sampling->selectIdx++;
					if (sampling->selectIdx >= sampling->count) {
						sampling->selectIdx = 0;
					}
					if (sampling->count >= 2) {
						psndSFXOn(sfx);
					}
				}
				if (BattlePadCheckRepeat(0x40000)) {
					sampling->selectIdx--;
					if (sampling->selectIdx < 0) {
						sampling->selectIdx = sampling->count - 1;
					}
					if (sampling->count >= 2) {
						psndSFXOn(sfx2);
					}
				}
			}
			if (BattlePadCheckTrigger(0x200)) {
				psndSFXOn((char*)0x9D9);
				BattleCommandDisplay_TargetSelectMenuEnd(wp);
				cmd->commandState = 6;
				wp->flags &= ~0x4;
				BattleAcHelpSetDispType(0);
				BtlUnit_SetCommandAnimPose(unit);
				if ((cmd->actKind == 0 && (wp->flags & 0x200)) ||
				    (cmd->actKind == 1 && (wp->flags & 0x400))) {
					cmd->commandState = 1;
					BattleStageOffLightEnemyInCommand();
				}
				break;
			}
			if (BattlePadCheckTrigger(0x100)) {
				psndSFXOn((char*)0x9D8);
				BattleStageOffLightInCommand(1, 1);
				cmd->commandState = 0x1D;
				BattleCommandDisplay_TargetSelectMenuEnd(wp);
				wp->flags |= 4;
				if (action == 2 && unit->moveState == 0) {
					switch (cmd->multiItemKind) {
					case 2:
						unit->moveState = 8;
						BtlUnit_PayWeaponCost(unit, &badgeWeapon_2KaiItem);
						break;
					case 3:
						unit->moveState = 5;
						BtlUnit_PayWeaponCost(unit, &badgeWeapon_3KaiItem);
						break;
					}
				}
				BattleAcHelpSetDispType(2);
			}
			if (BattleCommandAttackAudienceCheck()) {
				wp->commandMenu.restoreState = 1;
				BattleCommandDisplay_TargetSelectMenuEnd(wp);
				BattleAcHelpSetDispType(0);
			}
			break;
		case 14:
		case 15:
			if (state == 0xE) {
				BattleCommandDisplay_MultiItemMenuSetup(wp, (void*)1);
			} else {
				BattleCommandDisplay_MultiItemMenuSetup(wp, 0);
			}
			cmd->commandState = 0x10;
			_commandRestoreRec();
			// fallthrough
		case 16:
			if (BattleCommandDisplay_MultiItemMenuMain(wp)) {
				result = 1;
			}
			break;
		case 17:
			cmd->commandState = 0x12;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			cmd->eventId = event->eventId;
			// fallthrough
		case 18:
			if (evtCheckID(cmd->eventId) != 0) {
				break;
			}
			cmd->eventId = 0;
			result = 1;
			cmd->commandState = 0xF;
			break;
		case 19:
		case 20:
			if (state == 0x13) {
				BattleCommandDisplay_OperationMenuSetup(wp, (void*)1);
			} else {
				BattleCommandDisplay_OperationMenuSetup(wp, 0);
			}
			cmd->commandState = 0x15;
			_commandRestoreRec();
			// fallthrough
		case 21:
			if (BattleCommandDisplay_OperationMenuMain(wp)) {
				result = 1;
			}
			break;
		case 22:
			cmd->commandState = 0x17;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			cmd->eventId = event->eventId;
			// fallthrough
		case 23:
			if (evtCheckID(cmd->eventId) != 0) {
				break;
			}
			cmd->eventId = 0;
			result = 1;
			cmd->commandState = 0x14;
			break;
		case 24:
		case 25:
			if (state == 0x18) {
				BattleCommandDisplay_ChangePartySelectMenuSetup(wp, (void*)1);
			} else {
				BattleCommandDisplay_ChangePartySelectMenuSetup(wp, 0);
			}
			cmd->commandState = 0x1A;
			_commandRestoreRec();
			// fallthrough
		case 26:
			if (BattleCommandDisplay_ChangePartySelectMenuMain(wp)) {
				result = 1;
			}
			break;
		case 27:
			BattleCommandDisplay_ProtectPartnerSelectMenuSetup(wp);
			cmd->commandState = 0x1C;
			_commandRestoreRec();
			// fallthrough
		case 28:
			if (BattlePadCheckTrigger(0x200)) {
				psndSFXOn((char*)0x9D9);
				BattleCommandDisplay_ProtectPartnerSelectMenuEnd(wp);
				cmd->commandState = 0x14;
			} else if (BattlePadCheckTrigger(0x100)) {
				cmd->commandState = 0x1D;
				BattleCommandDisplay_ProtectPartnerSelectMenuEnd(wp);
			}
			break;
		case 29:
			_btlcmd_SetAttackEvent(unit, cmd);
			cmd->menuFlags &= ~0x01000000;
			wp->dispFlags &= ~1;
			BattleStageOffLightInCommand(0, 0);
			return 1;
		default:
			break;
		}
		if (wp->commandMenu.restoreState == 0) {
			break;
		}
		_commandRestoreRec();
		}
	}
	return 0;
}

void BattleCommandDisplay(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;

	if (cmd) {
		SelectedItemCoordinateColorUpDate();
		dispEntry(CAMERA_2D, 902.0f, 0, BattleDrawEnemyHP, wp);
		if (cmd->menuFlags & 0x1) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_ActMenuDisp, wp);
		}
		if (cmd->menuFlags & 0x10) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_WeaponSelectMenuDisp, wp);
		}
		if (cmd->menuFlags & 0x100) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_OperationMenuDisp, wp);
		}
		if (cmd->menuFlags & 0x1000) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_ChangePartySelectMenuDisp, wp);
		}
		if (cmd->menuFlags & 0x20) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_MultiItemMenuDisp, wp);
		}
		if (cmd->menuFlags & 0x10000) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_TargetSelectMenuDisp, wp);
		}
		if (cmd->menuFlags & 0x100000) {
			dispEntry(CAMERA_2D, 500.0f, 0, (DispCallback)BattleCommandDisplay_ProtectPartnerSelectMenuDisp, wp);
		}
	}
}

#pragma dont_inline on
void _cursor_init(BattleCursor* cursor) {
	cursor->field_0 = 0;
	cursor->field_4 = 0;
}

void BattleCommandDisplay_ActMenuSetup(BattleWork* wp, void* arg) {
	BattleWork* work = wp;
	void* a = arg;
	BattleCursor* cursor;
	BattleCommand* cmd = &work->commandMenu;

	_btlcmd_MakeActClassTable(work);
	_btlcmd_GetCursorPtr(cmd, 14, &cursor);
	BattleMenuDisp_ActSelect_Init(work, cursor, cmd->actionTable, (u32)a);
	cmd->idleFrames = 0;
	cmd->menuFlags |= 0x01000001;
	if (a == NULL && cmd->restoreState == 0) {
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

// Matches. The range guard is a `switch (t)` with cases 0 and 1 (the original's `default:
// continue;`), not `if (t >= 2 || t < 0) continue;`: MWCC emits the switch range check as
// `cmpwi 2; bge default; cmpwi 0; bge body; b default`, whose `bge body` is the redundant
// branch to the next instruction the `if` form can't produce. The 12 remaining diffs are
// the unit's pooled string base.
s32 BattleCommandDisplay_ActMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCursor* cursor;
	s32 result = 0;

	_btlcmd_GetCursorPtr(cmd, 0xE, &cursor);
	if (BattlePadCheckTrigger(0x800) && BattleCommandCheckChangePositionEnable(wp)) {
		psndSFXOn((char*)0x9D8);
		BattleCommandDisplay_ActMenuEnd(wp);
		cmd->actKind = 9;
		cmd->commandState = 0x1D;
		BattleMenuDisp_ActSelect_Main(wp);
		return 1;
	}
	while (BattleMenuKeyOKInACT(wp)) {
		s32 i;
		s32 t;
		for (i = 0; i < cursor->unk8; i++) {
			t = cmd->actionTable[i].mType;
			switch (t) {
			case 0:
			case 1:
				if ((t == 0 && (wp->flags & 0x200)) || (t == 1 && (wp->flags & 0x400))) {
					_btlcmd_MakeSelectWeaponTable(wp, t);
					if (_btlcmd_CheckWeaponTargetNum(wp, unit, cmd->weaponTable[0].mWeaponParams) > 0) {
						continue;
					}
					cmd->actionTable[i].mEnabled = 0;
					if ((s32)cmd->actionTable[i].mDisableReason == 0) {
						if (t == 0) {
							cmd->actionTable[i].mDisableReason = 4;
						} else {
							cmd->actionTable[i].mDisableReason = 5;
						}
					}
				}
				break;
			default:
				continue;
			}

		}
		if ((s32)BattlePadCheckNow(-1)) {
			cmd->idleFrames = 0;
			btl_camera_set_mode(0, 0);
		} else {
			s32 v = cmd->idleFrames + 1;
			cmd->idleFrames = v;
			if (v >= 0x384) {
				btl_camera_set_mode(0, 6);
				btl_camera_set_moveSpeedLv(0, 5);
			}
		}
		if (wp->commandMenu.restoreState) {
			s32 t2 = cmd->actionTable[cursor->field_0].mType;
			if ((t2 == 0 && (wp->flags & 0x200)) || (t2 == 1 && (wp->flags & 0x400))) {
				wp->commandMenu.restoreState = 0;
				wp->commandMenu.restoreId = 0;
			}
		}
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.restoreState != 0) {
			if (cmd->actionTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				cmd->actKind = cmd->actionTable[cursor->field_0].mType;
				switch (cmd->actKind) {
				case 0:
				case 1:
				case 4:
				case 6:
				case 7:
				case 8:
					cmd->commandState = 5;
					result = 1;
					break;
				case 2:
					cmd->commandState = 5;
					if (unit->moveState == 0) {
						cmd->multiItemKind = 1;
						if (unit->badgesEquipped.DoubleDip || unit->badgesEquipped.TripleDip) {
							cmd->commandState = 0xE;
							result = 1;
						}
					}
					break;
				case 5:
					cmd->commandState = 0x13;
					result = 1;
					break;
				case 13:
					cmd->commandState = 0x1D;
					result = 1;
					break;
				}
				BattleCommandDisplay_ActMenuEnd(wp);
				break;
			}
			if (wp->commandMenu.restoreState == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.restoreState = 0;
			if ((s32)cmd->actionTable[cursor->field_0].mDisableReason != 0) {
				BattleCommandDisplay_ActMenuEnd(wp);
				cmd->commandState = 3;
				result = 1;
				cmd->disableReason = cmd->actionTable[cursor->field_0].mDisableReason;
				break;
			}
		}
		if ((s32)BattlePadCheckRepeat(0x80000)) {
			if (cursor->field_0 < cursor->unk8 - 1) {
				cursor->field_0++;
				psndSFXOn("SFX_BTL_CURSOR_MOVE1");
				break;
			}
			psndSFXOn("SFX_BTL_CURSOR_MOVE1");
			cursor->field_0 = 0;
			break;
		}
		if ((s32)BattlePadCheckRepeat(0x40000)) {
			if (cursor->field_0 > 0) {
				cursor->field_0--;
				psndSFXOn("SFX_BTL_CURSOR_MOVE1");
				break;
			}
			psndSFXOn("SFX_BTL_CURSOR_MOVE1");
			cursor->field_0 = cursor->unk8 - 1;
			break;
		}
		if (BattleCommandAttackAudienceCheck()) {
			break;
		}
		if (BattlePadCheckTrigger(0x800) && BattleCommandCheckChangePositionEnable(wp)) {
			psndSFXOn((char*)0x9D8);
			BattleCommandDisplay_ActMenuEnd(wp);
			cmd->actKind = 9;
			result = 1;
			cmd->commandState = 0x1D;
		}
		break;
	}
	BattleMenuDisp_ActSelect_Main(wp);
	return result;
}

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

void BattleCommandDisplay_ActMenuDisp(CameraId camId, void* param) {
	BattleWork* wp = param;
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr(&wp->commandMenu, 0xE, &cursor);
	BattleMenuDisp_ActSelect_Disp(wp);
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_ActMenuEnd(BattleWork* wp) {
	BattleMenuDisp_ActSelect_End(wp);
	wp->commandMenu.menuFlags &= ~0x1;
}

void BattleCommandDisplay_WeaponSelectMenuSetup(BattleWork* wp, s32 arg, u32 arg2) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_MakeSelectWeaponTable(wp, arg);
	_btlcmd_GetCursorPtr(cmd, arg, &cursor);
	BattleMenuDisp_WeaponSelect_Init(wp, cursor, cmd->weaponTable, arg2);
	cmd->menuFlags |= 0x01000010;
}

s32 BattleCommandDisplay_WeaponSelectMenuMain(BattleWork* wp, s32 arg) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCursor* cursor;
	s32 result = 0;

	while (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, arg, &cursor);
		_btlcmd_UpdateSelectWeaponTable(wp, arg);
		if (wp->commandMenu.restoreState) {
			wp->commandMenu.restoreState = 0;
			wp->commandMenu.restoreId = 0;
		}
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.restoreState != 0) {
			if ((s32)cmd->weaponTable[cursor->field_0].mEnabled != 0) {
				psndSFXOn((char*)0x9D8);
				BattleCommandDisplay_WeaponSelectMenuEnd(wp);
				_btlcmd_SelectWeaponDecide(wp, arg, cursor->field_0);
				BattleCommandDisplay_TargetSelectMenuSetup(wp);
				cmd->commandState = 0xA;
				if (!(cmd->weaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x200)) {
					BattleStageOnLightFriendInCommand();
				}
				if (!(cmd->weaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x4000) &&
				    !(cmd->weaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x100) &&
				    !(cmd->weaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x10)) {
					BattleStageOnLightEnemyInCommand();
				}
				result = 1;
				break;
			}
			if (wp->commandMenu.restoreState == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.restoreState = 0;
			if ((s32)cmd->weaponTable[cursor->field_0].mDisableReason != 0) {
				BattleCommandDisplay_WeaponSelectMenuEnd(wp);
				result = 1;
				cmd->commandState = 8;
				cmd->disableReason = cmd->weaponTable[cursor->field_0].mDisableReason;
			}
			break;
		}
		if (cursor->field_0 < cursor->unk8 - 1) {
			if (BattlePadCheckRepeat(0x20000)) {
				cursor->field_0++;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				break;
			}
		} else {
			if (BattlePadCheckTrigger(0x20000)) {
				cursor->field_0 = 0;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				break;
			}
		}
		if (cursor->field_0 > 0) {
			if (BattlePadCheckRepeat(0x10000)) {
				cursor->field_0--;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				break;
			}
		} else {
			if (BattlePadCheckTrigger(0x10000)) {
				cursor->field_0 = cursor->unk8 - 1;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				break;
			}
		}
		if (BattlePadCheckTrigger(0x200)) {
			if (arg == 2 && (unit->badgesEquipped.DoubleDip || unit->badgesEquipped.TripleDip)) {
				if (unit->moveState == 0) {
					psndSFXOn((char*)0x9D9);
					BattleCommandDisplay_WeaponSelectMenuEnd(wp);
					cmd->commandState = 0xF;
				} else {
					BattleCommandDisplay_WeaponSelectMenuEnd(wp);
					cmd->commandState = 0xC;
				}
			} else {
				psndSFXOn((char*)0x9D9);
				BattleCommandDisplay_WeaponSelectMenuEnd(wp);
				cmd->commandState = 1;
			}
			result = 1;
			break;
		}
		if (BattleCommandAttackAudienceCheck()) {
			BattleCommandDisplay_WeaponSelectMenuEnd(wp);
			wp->commandMenu.restoreState = 1;
		}
		break;
	}
	BattleMenuDisp_WeaponSelect_Main(wp);
	return result;
}

void BattleCommandDisplay_WeaponSelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_WeaponSelect_Disp(wp);
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_WeaponSelectMenuEnd(BattleWork* wp) {
	BattleMenuDisp_WeaponSelect_End(wp);
	wp->commandMenu.menuFlags &= ~0x10;
}

void BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(BattleWork* wp) {
	wp->commandMenu.cursor[13].field_0 = 0;
	wp->commandMenu.cursor[13].field_4 = 0;
	wp->commandMenu.cursor[13].unk8 = 2;
}

__declspec(section ".sdata") s32 _jump_badge_table[8] = {0xF0, 0xF1, 0xF4, 0xF5, 0xF6, 0xF3, 0xF2, 0x14C};
__declspec(section ".sdata") s32 _hammer_badge_table[9] = {0xF7, 0xFA, 0xF9, 0xFB, 0xFD, 0xFC, 0xF8, 0x14D, 0x14E};
// opt_propagation off: with propagation on MWCC folds the `cursor` base into `wp`; the original keeps it
// in r31 (`addi r31, r28, 0x1c50`). The `cmd` intermediate is needed so the cursor is derived from a local
// rather than straight from the parameter.
#pragma opt_propagation off
s32 BattleCommandDisplay_WeaponMultiItemCancelCheckMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor = &cmd->cursor[13];
	s32 done = 0;
	s32 cancel = 0;

	if (BattleCommandAttackAudienceCheck()) {
		wp->commandMenu.restoreState = 1;
		return 0;
	}
	if (cursor->field_0 < cursor->unk8 - 1) {
		if ((s32)BattlePadCheckRepeat(0x20000)) {
			cursor->field_0++;
			psndSFXOn("SFX_BTL_CURSOR_MOVE2");
			goto after;
		}
	} else {
		if (BattlePadCheckTrigger(0x20000)) {
			cursor->field_0 = 0;
			psndSFXOn("SFX_BTL_CURSOR_MOVE2");
			goto after;
		}
	}
	if (cursor->field_0 > 0) {
		if ((s32)BattlePadCheckRepeat(0x10000)) {
			cursor->field_0--;
			psndSFXOn("SFX_BTL_CURSOR_MOVE2");
			goto after;
		}
	} else {
		if (BattlePadCheckTrigger(0x10000)) {
			cursor->field_0 = cursor->unk8 - 1;
			psndSFXOn("SFX_BTL_CURSOR_MOVE2");
		}
	}
after:
	if (BattlePadCheckTrigger(0x200)) {
		done = 1;
		cancel = 0;
	} else if (BattlePadCheckTrigger(0x100)) {
		done = 1;
		if (cursor->field_0 == 0) {
			cancel = 1;
		}
	}
	if (done) {
		if (cancel) {
			wp->commandMenu.cancelled = 1;
			wp->commandMenu.commandState = 0x1D;
		} else {
			wp->commandMenu.commandState = 6;
		}
	}
	dispEntry(CAMERA_2D, 900.0f, 0, (DispCallback)BattleCommandDisplay_WeaponMultiItemCancelCheckDisp, wp);
	return done;
}
#pragma opt_propagation reset



void BattleCommandDisplay_WeaponMultiItemCancelCheckDisp(CameraId camId, BattleWork* wp) {
	char buf[0x20];
	Vec pos;
	u32 len;
	f32 xoff;
	f32 cx = 0.0f;

	pouchGetPtr();
	sprintf(buf, msgSearch("btl_msg_multi_item_cancel_check"));
	len = strlen(buf);
#ifdef MUST_MATCH
	// Retail evaluates the float arguments before the texture id and the colour (as in battle_seq_end's
	// _MarioExpDisp), so the call goes through a float-first cast; the colour copy precedes the conversions.
	((void (*)(f32, f32, f32, f32, f32, u16, GXColor))windowDispGX_Waku_col)(
	    -30.0f - (xoff = 6.25f * (f32)len), 30.0f, 12.5f * (f32)len + 65.0f, 32.0f, 10.0f, 0,
	    (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
#else
	windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF }, -30.0f - (xoff = 6.25f * (f32)len), 30.0f,
	                      12.5f * (f32)len + 65.0f, 32.0f, 10.0f);
#endif
	windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF }, -70.0f, -10.0f, 130.0f, 60.0f, 10.0f);
	FontDrawStart();
	FontDrawString(buf, cx - xoff, 27.0f);
	FontDrawString("\202\315\202\242\201@", -20.0f, -13.0f);
	FontDrawString("\202\242\202\242\202\246", -20.0f, -38.0f);
	pos.x = -40.0f;
	pos.y = (f32)(-42 - wp->commandMenu.cursor[13].field_0 * 25);
	pos.z = 0.0f;
	iconDispGx(1.0f, &pos, 0x14, 0x1F0);
}

void BattleCommandDisplay_OperationMenuSetup(BattleWork* wp, void* arg) {
	_btlcmd_MakeOperationTable(wp);
	BattleMenuDisp_Operation_Init(wp, (u32)arg);
	wp->commandMenu.menuFlags |= 0x01000100;
}

// Matches. The target tests mDisableReason signed and passes wp to the *void header declaration; see the
// (s32) cast and the MUST_MATCH call below.
s32 BattleCommandDisplay_OperationMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;
	s32 result = 0;

	if (wp->commandMenu.restoreState) {
		wp->commandMenu.restoreState = 0;
		wp->commandMenu.restoreId = 0;
	}
	if (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, 5, &cursor);
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.restoreState != 0) {
			if (cmd->operationTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				cmd->operationKind = cmd->operationTable[cursor->field_0].mType;
				switch (cmd->operationKind) {
				case 1:
				case 2:
				case 3:
				case 5:
				case 6:
				case 7:
				case 8:
					BattleStageOffLightInCommand(0, 1);
					cmd->commandState = 0x1D;
					break;
				case 0:
					cmd->commandState = 0x18;
					break;
				case 4:
					cmd->commandState = 0x1B;
					break;
				}
				BattleCommandDisplay_OperationMenuEnd(wp);
				result = 1;
				goto end;
			}
			if (wp->commandMenu.restoreState == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.restoreState = 0;
			if ((s32)cmd->operationTable[cursor->field_0].mDisableReason != 0) {
				BattleCommandDisplay_OperationMenuEnd(wp);
				cmd->commandState = 0x16;
				result = 1;
				cmd->disableReason = cmd->operationTable[cursor->field_0].mDisableReason;
			}
			goto end;
		}
		if (cursor->field_0 < cursor->unk8 - 1) {
			if (BattlePadCheckRepeat(0x20000)) {
				cursor->field_0++;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		} else {
			if (BattlePadCheckTrigger(0x20000)) {
				cursor->field_0 = 0;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		}
		if (cursor->field_0 > 0) {
			if (BattlePadCheckRepeat(0x10000)) {
				cursor->field_0--;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		} else {
			if (BattlePadCheckTrigger(0x10000)) {
				cursor->field_0 = cursor->unk8 - 1;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		}
		if (BattlePadCheckTrigger(0x200)) {
			psndSFXOn((char*)0x9D9);
			BattleCommandDisplay_OperationMenuEnd(wp);
			result = 1;
			cmd->commandState = 1;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			wp->commandMenu.restoreState = 1;
			BattleCommandDisplay_OperationMenuEnd(wp);
		}
	}
end:
	BattleMenuDisp_Operation_Main(wp);
	return result;
}

void BattleCommandDisplay_OperationMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_Operation_Disp(wp);
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_OperationMenuEnd(BattleWork* wp) {
	BattleMenuDisp_Operation_End(wp);
	wp->commandMenu.menuFlags &= ~0x100;
}

void BattleCommandDisplay_MultiItemMenuSetup(BattleWork* wp, void* arg) {
	_btlcmd_MakeMultiItemTable(wp);
	BattleMenuDisp_MultiItem_Init(wp, (u32)arg);
	wp->commandMenu.menuFlags |= 0x20;
}

// Matches. The target tests mDisableReason signed and passes wp to the *void header declaration; see the
// (s32) cast and the MUST_MATCH call below.
s32 BattleCommandDisplay_MultiItemMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;
	s32 result = 0;

	if (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, 3, &cursor);
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.restoreState != 0) {
			if (cmd->multiItemTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				cmd->multiItemKind = cmd->multiItemTable[cursor->field_0].mKind;
				cmd->commandState = 5;
				BattleCommandDisplay_MultiItemMenuEnd(wp);
				result = 1;
				goto end;
			}
			if (wp->commandMenu.restoreState == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.restoreState = 0;
			if ((s32)cmd->multiItemTable[cursor->field_0].mDisableReason != 0) {
				BattleCommandDisplay_MultiItemMenuEnd(wp);
				cmd->commandState = 0x11;
				result = 1;
				cmd->disableReason = cmd->multiItemTable[cursor->field_0].mDisableReason;
			}
			goto end;
		}
		if (cursor->field_0 < cursor->unk8 - 1) {
			if (BattlePadCheckRepeat(0x20000)) {
				cursor->field_0++;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		} else {
			if (BattlePadCheckTrigger(0x20000)) {
				cursor->field_0 = 0;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		}
		if (cursor->field_0 > 0) {
			if (BattlePadCheckRepeat(0x10000)) {
				cursor->field_0--;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		} else {
			if (BattlePadCheckTrigger(0x10000)) {
				cursor->field_0 = cursor->unk8 - 1;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		}
		if (BattlePadCheckTrigger(0x200)) {
			psndSFXOn((char*)0x9D9);
			BattleCommandDisplay_MultiItemMenuEnd(wp);
			result = 1;
			cmd->commandState = 1;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			wp->commandMenu.restoreState = 1;
			BattleCommandDisplay_MultiItemMenuEnd(wp);
		}
	}
end:
	BattleMenuDisp_MultiItem_Main(wp);
	return result;
}

void BattleCommandDisplay_MultiItemMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_MultiItem_Disp(wp);
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_MultiItemMenuEnd(BattleWork* wp) {
	BattleMenuDisp_MultiItem_End(wp);
	wp->commandMenu.menuFlags &= ~0x20;
}

void BattleCommandDisplay_ChangePartySelectMenuSetup(BattleWork* wp, void* arg) {
	BattleMenuDisp_ChangePartySelect_Init(wp, (u32)arg);
	wp->commandMenu.menuFlags |= 0x01001000;
}

s32 BattleCommandDisplay_ChangePartySelectMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;
	s32 result = 0;

	if (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, 0xA, &cursor);
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.restoreState != 0) {
			if (cmd->partyTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				BattleStageOffLightInCommand(0, 1);
				cmd->partyChangeId = cmd->partyTable[cursor->field_0].mPartyId;
				cmd->commandState = 0x1D;
				BattleCommandDisplay_ChangePartySelectMenuEnd(wp);
				result = 1;
				goto end;
			}
			if (wp->commandMenu.restoreState == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.restoreState = 0;
		}
		if (cursor->field_0 < cursor->unk8 - 1) {
			if (BattlePadCheckRepeat(0x20000)) {
				cursor->field_0++;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		} else {
			if (BattlePadCheckTrigger(0x20000)) {
				cursor->field_0 = 0;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		}
		if (cursor->field_0 > 0) {
			if (BattlePadCheckRepeat(0x10000)) {
				cursor->field_0--;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		} else {
			if (BattlePadCheckTrigger(0x10000)) {
				cursor->field_0 = cursor->unk8 - 1;
				psndSFXOn("SFX_BTL_CURSOR_MOVE2");
				goto end;
			}
		}
		if (BattlePadCheckTrigger(0x200)) {
			psndSFXOn((char*)0x9D9);
			BattleCommandDisplay_ChangePartySelectMenuEnd(wp);
			result = 1;
			cmd->commandState = 0x14;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			wp->commandMenu.restoreState = 1;
			BattleCommandDisplay_ChangePartySelectMenuEnd(wp);
		}
	}
end:
	BattleMenuDisp_ChangePartySelect_Main(wp);
	return result;
}

void BattleCommandDisplay_ChangePartySelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleMenuDisp_ChangePartySelect_Disp(wp);
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_ChangePartySelectMenuEnd(BattleWork* wp) {
	BattleMenuDisp_ChangePartySelect_End(wp);
	wp->commandMenu.menuFlags &= ~0x1000;
}

void BattleCommandDisplay_TargetSelectMenuSetup(BattleWork* wp) {
	BattleUnit* unit;
	s32 id = wp->activeUnitId;

	unit = BattleGetUnitPtr(wp, id);
	wp->commandMenu.menuFlags |= 0x00010000;
	BtlUnit_SetAnimType(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), 0x28);
}

void BattleCommandDisplay_TargetSelectMenuDisp(CameraId camId, BattleWork* param) {
#ifdef MUST_MATCH
	// MUST_MATCH: the original's parameter is `void*`, so its typed `BattleWork*` local is not
	// coalesced with the parameter; the cast keeps them apart and puts wp in r29 (with
	// `BattleWork* wp = param;` MWCC uses r26 and the whole tail rotates, 99.09%).
	BattleWork* wp = (BattleWork*)(void*)param;
#else
	BattleWork* wp = param;
#endif
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;
	BattleWeapon* weapon;
	Vec iconPos;

	_btlcmd_GetCursorPtr(cmd, cmd->actKind, &cursor);
	weapon = cmd->weaponTable[cursor->field_0].mWeaponParams;
	if (weapon->targetClassFlags & 0x01000000) {
		BattleSampling* sampling = &wp->sampling;
		BattleUnit* unit;
		BattleUnitPart* part;
		BattleSamplingEntry* entry;
		char* name;
		f32 width;
		Vec pos;
		Vec screen;

		entry = &sampling->entries[sampling->order[sampling->selectIdx]];
		unit = BattleGetUnitPtr(wp, entry->unitId);
		part = BtlUnit_GetPartsPtr(unit, entry->partId);
		pos.x = entry->x;
		pos.y = entry->y;
		pos.z = entry->z;
		if ((unit->mAttributeFlags & 2) == 0) {
			btlGetScreenPoint(&pos, &screen);
			iconPos.x = screen.x;
			iconPos.y = screen.y;
			iconPos.z = 0.0f;
			iconDispGx(1.0f, &iconPos, 0x10, 0x1EF);
			if (unit->currentKind == 0xE2) {
				name = pouchGetYoshiName();
			} else {
				name = (char*)msgSearch(part->partName);
			}
			width = (f32)FontGetMessageWidth(name);
			windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xCF }, -240.0f, 86.0f, 16.0f + width, 32.0f,
			                      10.0f);
			FontDrawStart();
			FontDrawString(name, -232.0f, 83.0f);
		} else {
			btlGetScreenPoint(&pos, &screen);
			iconPos.x = screen.x;
			iconPos.y = screen.y;
			iconPos.z = 0.0f;
			iconDispGx(1.0f, &iconPos, 0x18, 0x1EF);
			if (unit->currentKind == 0xE2) {
				name = pouchGetYoshiName();
			} else {
				name = (char*)msgSearch(part->partName);
			}
			width = (f32)FontGetMessageWidth(name);
			windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xCF }, -240.0f, 86.0f, 16.0f + width, 32.0f,
			                      10.0f);
			FontDrawStart();
			FontDrawString(name, -232.0f, 83.0f);
		}
	} else {
		BattleSampling* sampling = &wp->sampling;
		BattleUnit* unit;
		BattleUnitPart* part;
		BattleSamplingEntry* entry;
		char* name;
		f32 width;
		Vec pos;
		Vec screen;
		s32 i;
		s32 yoff;

		for (i = 0, yoff = 0; i < sampling->count; i++, yoff += 0x20) {
			entry = &sampling->entries[sampling->order[i]];
			unit = BattleGetUnitPtr(wp, entry->unitId);
			part = BtlUnit_GetPartsPtr(unit, entry->partId);
			pos.x = entry->x;
			pos.y = entry->y;
			pos.z = entry->z;
			if ((unit->mAttributeFlags & 2) == 0) {
				btlGetScreenPoint(&pos, &screen);
				iconPos.x = screen.x;
				iconPos.y = screen.y;
				iconPos.z = 0.0f;
				iconDispGx(1.0f, &iconPos, 0x10, 0x1EF);
				if (unit->currentKind == 0xE2) {
					name = pouchGetYoshiName();
				} else {
					name = (char*)msgSearch(part->partName);
				}
				width = (f32)FontGetMessageWidth(name);
				windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xCF }, -240.0f,
				                      118.0f - (f32)yoff, 16.0f + width, 32.0f, 10.0f);
				FontDrawStart();
				FontDrawString(name, -232.0f, 115.0f - (f32)yoff);
			} else {
				btlGetScreenPoint(&pos, &screen);
				iconPos.x = screen.x;
				iconPos.y = screen.y;
				iconPos.z = 0.0f;
				iconDispGx(1.0f, &iconPos, 0x18, 0x1EF);
				if (unit->currentKind == 0xE2) {
					name = pouchGetYoshiName();
				} else {
					name = (char*)msgSearch(part->partName);
				}
				width = (f32)FontGetMessageWidth(name);
				windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xCF }, -240.0f,
				                      118.0f - (f32)yoff, 16.0f + width, 32.0f, 10.0f);
				FontDrawStart();
				FontDrawString(name, -232.0f, 115.0f - (f32)yoff);
			}
		}
	}
	btl_camera_set_mode(0, 0);
}

void BattleCommandDisplay_TargetSelectMenuEnd(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr(cmd, cmd->actKind, &cursor);
	btl_camera_set_mode(0, 0);
	cmd->menuFlags &= ~0x10000;
	cmd->menuFlags &= ~0x01000000;
}

void BattleCommandDisplay_ProtectPartnerSelectMenuSetup(BattleWork* wp) {
	BattleUnit* unit;

	unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleGetPartnerPtr(wp, unit);
	wp->commandMenu.menuFlags |= 0x00100000;
	wp->commandMenu.menuFlags &= ~0x01000000;
}

void BattleCommandDisplay_ProtectPartnerSelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleUnit* partner = BattleGetPartnerPtr(wp, unit);
	Vec iconPos;
	Vec pos;
	Vec screen;
	f32 len;
	f32 xoff;

	BtlUnit_GetPos(partner, &pos.x, &pos.y, &pos.z);
	pos.y += 60.0f;
	btlGetScreenPoint(&pos, &screen);
	iconPos.x = screen.x;
	iconPos.y = screen.y;
	iconPos.z = 0.0f;
	iconDispGx(1.0f, &iconPos, 0x10, 0x1EF);
	len = (f32)strlen(msgSearch((char*)partner->mKindParams->name));
	xoff = 6.25f * len;
	windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xCF }, iconPos.x - xoff, 80.0f + iconPos.y,
	                      12.5f * len, 32.0f, 10.0f);
	FontDrawStart();
	FontDrawString(msgSearch((char*)partner->mKindParams->name),
	               3.0f + iconPos.x - xoff, 77.0f + iconPos.y);
}

void BattleCommandDisplay_ProtectPartnerSelectMenuEnd(BattleWork* wp) {
	wp->commandMenu.menuFlags &= ~0x100000;
}

void BattleCommandDisplay_AllEnd(BattleWork* wp) {
	wp->commandMenu.menuFlags &= ~0x111111;
}

void _btlcmd_SetAttackEvent(BattleUnit* unit, BattleCommand* cmd) {
	BattleWork* wp = _battleWorkPointer;
	s32 command = cmd->actKind;

	if (cmd->cancelled) {
		unit->moveState = 0;
		unit->attackEventCode = btldefaultevt_Wait;
		unit->field_0x2A8 = 1;
		return;
	}

	switch (command) {
	case 5:
		switch (cmd->operationKind) {
		case 6:
			unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				unit->attackEventCode = btldefaultevt_Defend;
			} else {
				unit->attackEventCode = btldefaultevt_DefendParty;
			}
			break;
		case 5:
			unit->attackEventCode = btldefaultevt_Appeal;
			unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F2D);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F2E);
			}
			break;
		case 7:
			unit->attackEventCode = btldefaultevt_Wait;
			unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F2D);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F2E);
			}
			break;
		case 3:
			unit->attackEventCode = btldefaultevt_Escape;
			unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F37);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F38);
			}
			break;
		case 1:
			unit->field_0x2A8 = 0;
			if (unit->currentKind == 0xDE) {
				unit->attackEventCode = badgeWeapon_Charge.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F33);
			} else {
				unit->attackEventCode = badgeWeapon_ChargeP.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F34);
			}
			break;
		case 2:
			unit->field_0x2A8 = 0;
			if (unit->currentKind == 0xDE) {
				unit->attackEventCode = badgeWeapon_SuperCharge.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F35);
			} else {
				unit->attackEventCode = badgeWeapon_SuperChargeP.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F36);
			}
			break;
		case 0:
			unit->attackEventCode = btldefaultevt_ChangeParty;
			unit->field_0x2A8 = 1;
			cmd->cursor[1].field_0 = 0;
			if (wp->badgeEquippedFlags & 1) {
				unit->moveState = 3;
			}
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F29);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F2A);
			}
			break;
		case 4:
			unit->attackEventCode = btldefaultevt_ProtectPartner;
			unit->field_0x2A8 = 1;
			break;
		case 8:
			unit->attackEventCode = BtlUnit_GetData(unit, 2);
			unit->field_0x2A8 = 3;
			break;
		default:
			unit->attackEventCode = btldefaultevt_Wait;
			unit->field_0x2A8 = 1;
			break;
		}
		break;
	case 0:
	case 1:
	case 2:
	case 4:
	case 6: {
		BattleCursor* cursor;
		BattleWeapon* weapon;

		_btlcmd_GetCursorPtr(cmd, command, &cursor);
		weapon = cmd->weaponTable[cursor->field_0].mWeaponParams;
		unit->attackEventCode = weapon->mAttackEvtScript;
		unit->field_0x2A8 = 0;
		if (unit->attackEventCode == NULL) {
			unit->attackEventCode = btldefaultevt_Wait;
		}

		switch (command) {
		case 0:
			BtlActRec_AddCount((u8*)wp + 0x16F1C);
			break;
		case 1:
			BtlActRec_AddCount((u8*)wp + 0x16F1D);
			break;
		case 4:
			if (_check_weapon_type_attack(weapon)) {
				BtlActRec_AddCount((u8*)wp + 0x16F1E);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F1F);
			}
			break;
		case 2:
			if ((s32)weapon != (s32)&ItemWeaponData_Naniga_Okorukana) {
				if (unit->currentKind == 0xDE) {
					if (_check_weapon_type_attack(weapon)) {
						BtlActRec_AddCount((u8*)wp + 0x16F25);
					} else {
						BtlActRec_AddCount((u8*)wp + 0x16F26);
					}
				} else {
					if (_check_weapon_type_attack(weapon)) {
						BtlActRec_AddCount((u8*)wp + 0x16F27);
					} else {
						BtlActRec_AddCount((u8*)wp + 0x16F28);
					}
				}
			}
			break;
		case 6:
			if (_check_weapon_type_attack(weapon)) {
				BtlActRec_AddCount((u8*)wp + 0x16F39);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F3A);
			}
			break;
		}
		break;
	}
	case 9:
		unit->moveState = 1;
		unit->attackEventCode = btldefaultevt_ChangePosition;
		unit->field_0x2A8 = 7;
		break;
	case 11:
		switch ((s8)unit->moveState) {
		case 6:
			unit->moveState = 5;
			break;
		case 7:
			unit->moveState = 6;
			break;
		case 9:
			unit->moveState = 8;
			break;
		default:
			unit->moveState = 4;
			break;
		}
		unit->attackEventCode = btlataudGetAttackEventPtr(unit->unitId);
		unit->field_0x2A8 = 3;
		if (unit->currentKind == 0xDE) {
			BtlActRec_AddCount((u8*)wp + 0x16F2B);
		} else {
			BtlActRec_AddCount((u8*)wp + 0x16F2C);
		}
		if (*(u32*)&wp->field_19080[0] & 1) {
			msgWindow_Continue(*(s32*)&wp->field_19080[4]);
		}
		break;
	case 12:
		switch ((s8)unit->moveState) {
		case 6:
			unit->moveState = 5;
			break;
		case 7:
			unit->moveState = 6;
			break;
		case 9:
			unit->moveState = 8;
			break;
		default:
			unit->moveState = 4;
			break;
		}
		unit->attackEventCode = btlataudGetMoveEventPtr(unit->unitId);
		unit->field_0x2A8 = 3;
		break;
	case 13:
		unit->attackEventCode = btldefaultevt_LectureEnd;
		unit->field_0x2A8 = 3;
		break;
	default:
		unit->attackEventCode = btldefaultevt_Wait;
		unit->field_0x2A8 = 0;
		break;
	}
}


BattleWeapon* BattleGetSelectWeapon(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr(cmd, cmd->actKind, &cursor);
	return cmd->weaponTable[cursor->field_0].mWeaponParams;
}

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

// Matches. A single `Vec pos` (not three separate f32 locals) makes MWCC emit the BtlUnit_GetPos
// argument setup in the target's order (&pos.y, &pos.z, unit, &pos.x).
s32 _check_present_item(void) {
	BattleWork* wp = _battleWorkPointer;
	s32 item = BattleAudience_GetPresentItemNo();
	s32 unitId;
	BattleUnit* unit;
	Vec pos;

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
		BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
		pos.y = (f32)BtlUnit_GetHeight(unit) * unit->mSizeMultiplier + 5.0f + pos.y;
		effRecoveryEntry(0, 1, pos.x, pos.y, pos.z);
		BattleAudience_SetPresentItemNo(0);
		return 0;
	case 0x7C:
		BtlUnit_RecoverFp(unit, 1);
		BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
		pos.y = (f32)BtlUnit_GetHeight(unit) * unit->mSizeMultiplier + 5.0f + pos.y;
		effRecoveryEntry(1, 1, pos.x, pos.y, pos.z);
		BattleAudience_SetPresentItemNo(0);
		return 0;
	default:
		break;
	}
	if (item < 0 || item >= 0x153) {
		return 0;
	}
	wp->commandMenu.restoreState = 1;
	if (wp->commandMenu.commandState == 0xC || wp->commandMenu.commandState == 0xD) {
		wp->commandMenu.restoreState = 2;
	}
	BattleCommandDisplay_AllEnd(wp);
	wp->commandMenu.savedActKind = wp->commandMenu.actKind;
	wp->commandMenu.actKind = 0xC;
	wp->commandMenu.commandState = 0x1D;
	return 1;
}

void BattleDrawEnemyHP(CameraId camId, void* param) {
	BattleWork* wp = param;
	int i;
	BattleUnit* unit;
	Vec position;
	Vec screen;
	GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
	s32 icon;
	Mtx mtx;

	camGetPtr(CAMERA_2D); //unused, probably just stored

	for (i = 0; i < 64; i++) {
		unit = wp->units[i];
		if (unit) {
			if (!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
				if (battleCheckUnitMonosiriFlag(wp->units[i])) {
					if (!(unit->mAttributeFlags & 0x2000000)) {
						if ((wp->commandMenu.menuFlags & 0x1000000) || (unit->flags & 1)) {
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
							btlGetScreenPoint(&position, &screen);
							if (unit->flags & 1) {
								if ((unit->hpGauge.field_0x2 & 1) && unit->hpGauge.field_0x8 < 0x14) {
									color.a = (u8)(s32)intplGetValue(0, unit->hpGauge.field_0x8, 0x14, 0.0f, 255.0f);
								}
							}
							icon = 20;
							if (wp->units[i]->currentHp >= 100) {
								icon = 12;
							} else if (wp->units[i]->currentHp >= 10) {
								icon = 16;
							}
							PSMTXTrans(mtx, screen.x + (f32)icon, screen.y - 32.0f, 0.0f);
							iconNumberDispGx(mtx, wp->units[i]->currentHp, 1, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
							BattleDrawEnemyHPBar(screen, unit, color);
						}
					}
				}
			}
		}
	}
}

void BattleDrawEnemyHPBar(Vec pos, BattleUnit* unit, GXColor color) {
	CameraEntry* camera = camGetPtr(CAMERA_2D);
	BattleWorkHpGauge* gauge = &unit->hpGauge;

	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(camera->view, 0);
	pos.x -= 10.0f;
	((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)(
	    0xA, color, 0, 19.0f + pos.x, pos.y - 3.0f, 0.0f, 1.0f, 1.0f);
	switch ((s32)*(u16*)gauge->field_0x0) {
	case 1:
		if (gauge->field_0x4 > 0) {
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(camera->view, 0);
			btlDispGXQuads2DRasta(1.0f + pos.x, 2.0f + pos.y, 34.0f * gauge->field_0x18 + (1.0f + pos.x),
			                     (2.0f + pos.y) - 8.0f, 0, 0, 0, color.a);
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(camera->view, 0);
		}
		((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)(
		    0xB, color, 0, 19.0f + pos.x - 20.0f * (1.0f - gauge->field_0x14), pos.y - 3.0f, 0.0f,
		    gauge->field_0x14, 1.0f);
		break;
	default: {
		f32 w = gauge->field_0x14;
		((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)(
		    0xB, color, 0, 19.0f + pos.x - 20.0f * (1.0f - w), pos.y - 3.0f, 0.0f, w, 1.0f);
		break;
	}
	}
}

// The three later candidate loops declare their counter in a local block. With the
// function-scope `i`, MWCC folds `i = 0` into `mr r7, r6` and schedules the hoisted
// `addi r3, r1, 0xc` (candidates) after the inits; the local counter makes it emit
// `li r7, 0` and puts the address first, matching the original.
void* BattleSetConfuseAct(BattleWork* wp, BattleUnit* unit) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleCursor* cursor;
	s16 candidates[24];
	u8 count;
	u8 i;
	BattleWeapon* weapon;

	if (unit->moveState == 3) {
		unit->moveState = 0;
	}
	BattleAcHelpSetDispType(0);
	if (unit->mAlliance != 0) {
		return subsetevt_confuse_flustered;
	}
	if (!(unit->currentKind == 0xDE || (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7))) {
		return subsetevt_confuse_flustered;
	}
	_btlcmd_MakeActClassTable(wp);
	_btlcmd_GetCursorPtr(cmd, 0xE, &cursor);
	count = 0;
	for (i = 0; i < cursor->unk8; i++) {
		if (cmd->actionTable[i].mEnabled == 0) {
			continue;
		}
		if ((s32)cmd->actionTable[i].mType == 4) {
			continue;
		}
		if ((s32)cmd->actionTable[i].mType == 2) {
			continue;
		}
		candidates[count++] = i;
	}
	candidates[count] = -1;
	if (count != 0) {
		cursor->field_0 = candidates[irand(count)];
	} else {
		return subsetevt_confuse_flustered;
	}
	cmd->actKind = cmd->actionTable[cursor->field_0].mType;
	if (cmd->actionTable[cursor->field_0].mEnabled == 0) {
		return subsetevt_confuse_flustered;
	}
	switch (cmd->actKind) {
	case 0:
	case 1:
	case 2:
	case 4:
	case 6:
	case 7:
	case 8:
		_btlcmd_MakeSelectWeaponTable(wp, cmd->actKind);
		_btlcmd_GetCursorPtr(cmd, cmd->actKind, &cursor);
		count = 0;
		{
			u8 n;

			for (n = 0; n < cursor->unk8; n++) {
				weapon = cmd->weaponTable[n].mWeaponParams;
				if ((s32)cmd->weaponTable[n].mEnabled == 0) {
					continue;
				}
				if (!(weapon->special & 0x10000)) {
					continue;
				}
				if (weapon->targetClassFlags & 0x02000000) {
					continue;
				}
				if (!(weapon->targetClassFlags & 0x01000000)) {
					continue;
				}
				candidates[count++] = n;
			}
		}
		candidates[count] = -1;
		if (count != 0) {
			cursor->field_0 = candidates[irand(count)];
		} else {
			return NULL;
		}
		if ((s32)cmd->weaponTable[cursor->field_0].mEnabled == 0) {
			return subsetevt_confuse_flustered;
		}
		_btlcmd_SelectWeaponDecide(wp, cmd->actKind, cursor->field_0);
		weapon = cmd->weaponTable[cursor->field_0].mWeaponParams;
		if (!(weapon->special & 0x10000)) {
			return subsetevt_confuse_flustered;
		}
		if ((weapon->targetClassFlags & 0x02000000) || !(weapon->targetClassFlags & 0x01000000)) {
			return subsetevt_confuse_flustered;
		}
		{
			s8 numConfused = wp->sampling.count; // 0xE94

			if (numConfused <= 0) {
				return subsetevt_confuse_flustered;
			}
			wp->sampling.selectIdx = irand(numConfused); // 0xEDF
		}
		wp->flags |= 4;
		return weapon->mAttackEvtScript;
	case 5:
		_btlcmd_MakeOperationTable(wp);
		_btlcmd_GetCursorPtr(cmd, 5, &cursor);
		count = 0;
		{
			u8 n;

			for (n = 0; n < cursor->unk8; n++) {
				if ((s32)cmd->operationTable[n].mEnabled == 0) {
					continue;
				}
				if ((s32)cmd->operationTable[n].mType != 0 && (s32)cmd->operationTable[n].mType != 6 &&
				    (s32)cmd->operationTable[n].mType != 3) {
					continue;
				}
				candidates[count++] = n;
			}
		}
		candidates[count] = -1;
		if (count != 0) {
			cursor->field_0 = candidates[irand(count)];
		} else {
			return subsetevt_confuse_flustered;
		}
		if ((s32)cmd->operationTable[cursor->field_0].mEnabled == 0) {
			return subsetevt_confuse_flustered;
		}
		cmd->operationKind = cmd->operationTable[cursor->field_0].mType;
		switch (cmd->operationKind) {
		case 6:
			if (unit->currentKind == 0xDE) {
				return btldefaultevt_Defend;
			}
			return btldefaultevt_DefendParty;
		case 5:
			return btldefaultevt_Appeal;
		case 7:
			return btldefaultevt_Wait;
		case 3:
			return btldefaultevt_Escape;
		case 1:
			if (unit->currentKind == 0xDE) {
				return badgeWeapon_Charge.mAttackEvtScript;
			}
			return badgeWeapon_ChargeP.mAttackEvtScript;
		case 2:
			if (unit->currentKind == 0xDE) {
				return badgeWeapon_SuperCharge.mAttackEvtScript;
			}
			return badgeWeapon_SuperChargeP.mAttackEvtScript;
		case 8:
			return BtlUnit_GetData(unit, 2);
		case 0:
			_btlcmd_GetCursorPtr(cmd, 0xA, &cursor);
			count = 0;
			{
				u8 n;

				for (n = 0; n < cursor->unk8; n++) {
					if ((s32)cmd->partyTable[n].mEnabled == 0) {
						continue;
					}
					candidates[count++] = n;
				}
			}
			candidates[count] = -1;
			if (count != 0) {
				cursor->field_0 = candidates[irand(count)];
			} else {
				return NULL;
			}
			if ((s32)cmd->partyTable[cursor->field_0].mEnabled == 0) {
				return subsetevt_confuse_flustered;
			}
			cmd->partyChangeId = cmd->partyTable[cursor->field_0].mPartyId;
			cmd->cursor[1].field_0 = 0;
			return btldefaultevt_ChangeParty;
		case 4:
			return subsetevt_confuse_flustered;
		default:
			return subsetevt_confuse_flustered;
		}
	default:
		return subsetevt_confuse_flustered;
	}
}

void _commandRestoreRec(void) {
	BattleWork* wp = _battleWorkPointer;

	if (wp->commandMenu.restoreState == 0) {
		wp->commandMenu.restoreId = wp->commandMenu.commandState;
	} else if (wp->commandMenu.restoreId == wp->commandMenu.commandState) {
		wp->commandMenu.restoreId = 0;
		wp->commandMenu.restoreState = 0;
	}
}


#pragma dont_inline on // the original calls BattleCommandAttackAudienceCheck instead of inlining it into WeaponMultiItemCancelCheckMain
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
					wp->commandMenu.savedActKind = wp->commandMenu.actKind;
					wp->commandMenu.actKind = 0xB;
					wp->commandMenu.commandState = 0x1D;
					BattleStageOffLightInCommand(0, 1);
					wp->flags |= 0x4;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}





#pragma opt_dead_assignments off // needed: without it _get_msg 98.59%
// `opt_dead_assignments off` keeps the overwritten first `msg` assignment so the pooled string
// base stays in r5. The `args` local (instead of a `wp` local) is what colours wp r4 / args r6.
s32 _get_msg(EventEntry* event) {
	char* msg = "btl_wn_dummy_noitem";
	s32* args = event->args;
	s32 index = args[0];

	switch (_battleWorkPointer->commandMenu.disableReason) {
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

__declspec(section ".sdata") EVT_BEGIN(_btl_cmd_error_msg_event)
	CALL(_get_msg, LW(0))
	CALL(btlevtcmd_AnnounceMessage, 0, 0, -30, LW(0), 60)
	RETURN
EVT_END()

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
#pragma opt_dead_assignments reset
