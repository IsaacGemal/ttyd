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

extern ItemData itemDataTable[];

// Battle weapon data used by _btlcmd_SetAttackEvent; not declared in any header.
extern BattleWeapon ItemWeaponData_Naniga_Okorukana;


//TODO: move into battle/seq/?

//local prototypes

extern f32 float_5_8041c310;
extern f32 float_0_8041c2f0;
extern f32 float_1_8041c2f4;
extern f32 float_3_8041c2ec;
extern f32 float_6p25_8041c318;
extern f32 float_80_8041c31c;
extern f32 float_12p5_8041c320;
extern f32 float_77_8041c324;
extern f32 float_32_8041c30c;
extern f32 float_10_8041c2e4;
extern f32 float_19_8041c2e8;
extern f32 float_2_8041c2f8;
extern f32 float_34_8041c2fc;
extern f32 float_8_8041c300;
extern f32 float_20_8041c304;
extern f32 float_60_8041c314;
extern u32 dat_8041c2d8;
extern u32 dat_8041c2c0;
extern u32 dat_8041c2c4;
extern u32 dat_8041c2c8;
extern u32 dat_8041c2cc;
extern u32 dat_8041c2d0;
extern u32 dat_8041c2d4;
extern f32 float_neg240_8041c328;
extern f32 float_86_8041c32c;
extern f32 float_16_8041c330;
extern f32 float_neg232_8041c334;
extern f32 float_83_8041c338;
extern f32 float_118_8041c33c;
extern f32 float_115_8041c340;
extern f32 float_neg30_8041c344;
extern f32 float_30_8041c348;
extern f32 float_65_8041c34c;
extern f32 float_neg70_8041c350;
extern f32 float_neg10_8041c354;
extern f32 float_130_8041c358;
extern f32 float_27_8041c35c;
extern f32 float_neg20_8041c360;
extern f32 float_neg13_8041c364;
extern f32 float_neg38_8041c368;
extern f32 float_neg40_8041c36c;
extern f32 float_900_8041c370;

// Sampling work block lives inside BattleWork at offset 0x428 (same layout as the
// local BattleSampling struct in battle_detect.c).
typedef struct BattleTargetEntry {
    s16 unitId;   // 0x0
    s16 partId;   // 0x2
    s16 x;        // 0x4
    s16 y;        // 0x6
    s16 z;        // 0x8
    u8 pad_0xA[0x24 - 0xA];
} BattleTargetEntry;

typedef struct BattleTargetSampling {
    BattleWeapon* weapon;           // 0x0
    u8 entries[0xA6C - 0x4];        // 0x4, BattleTargetEntry[]
    s8 count;                       // 0xA6C
    s8 order[0xAB7 - 0xA6D];        // 0xA6D
    s8 selectIdx;                   // 0xAB7
} BattleTargetSampling;

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

// NONMATCHING (73.48%): 10 instructions differ, 12 missing, 10 extra, plus register differences.
void BattleDrawEnemyHPBar(Vec* pos, BattleUnit* unit, GXColor* color) {
	CameraEntry* camera = camGetPtr(CAMERA_2D);
	BattleWorkHpGauge* gauge = &unit->hpGauge;

	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(camera->view, 0);
	{
		f32 x = pos->x - float_10_8041c2e4;
		pos->x = x;
		btlDispTexPlane(0xA, *color, x + float_19_8041c2e8, pos->y - float_3_8041c2ec, float_0_8041c2f0,
		                float_1_8041c2f4, float_1_8041c2f4);
	}
	if ((s16)*(u16*)gauge->field_0x0 == 1) {
		if (gauge->field_0x4 > 0) {
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(camera->view, 0);
			btlDispGXQuads2DRasta(float_1_8041c2f4 + pos->x, float_2_8041c2f8 + pos->y,
			                     float_34_8041c2fc * gauge->field_0x18 + float_1_8041c2f4 + pos->x,
			                     float_2_8041c2f8 + pos->y - float_8_8041c300, 0, 0, 0, color->a);
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(camera->view, 0);
		}
		btlDispTexPlane(0xB, *color,
		                float_19_8041c2e8 + pos->x - float_20_8041c304 * (float_1_8041c2f4 - gauge->field_0x14),
		                pos->y - float_3_8041c2ec, float_0_8041c2f0, gauge->field_0x14, float_1_8041c2f4);
	} else {
		btlDispTexPlane(0xB, *color,
		                float_19_8041c2e8 + pos->x - float_20_8041c304 * (float_1_8041c2f4 - gauge->field_0x14),
		                pos->y - float_3_8041c2ec, float_0_8041c2f0, gauge->field_0x14, float_1_8041c2f4);
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

void BattleCommandDisplay_ProtectPartnerSelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleUnit* partner = BattleGetPartnerPtr(wp, unit);
	Vec iconPos;
	Vec pos;
	Vec screen;
	f32 len;
	f32 xoff;

	BtlUnit_GetPos(partner, &pos.x, &pos.y, &pos.z);
	pos.y += float_60_8041c314;
	btlGetScreenPoint(&pos, &screen);
	iconPos.x = screen.x;
	iconPos.y = screen.y;
	iconPos.z = float_0_8041c2f0;
	iconDispGx(&iconPos, 0x10, 0x1EF, float_1_8041c2f4);
	len = (f32)strlen(msgSearch((char*)partner->mKindParams->name));
	xoff = float_6p25_8041c318 * len;
	windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2d8, iconPos.x - xoff, float_80_8041c31c + iconPos.y,
	                      float_12p5_8041c320 * len, float_32_8041c30c, float_10_8041c2e4);
	FontDrawStart();
	FontDrawString(msgSearch((char*)partner->mKindParams->name),
	               float_3_8041c2ec + iconPos.x - xoff, float_77_8041c324 + iconPos.y);
}

void BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(BattleWork* wp) {
	wp->commandMenu.cursor[13].field_0 = 0;
	wp->commandMenu.cursor[13].field_4 = 0;
	wp->commandMenu.cursor[13].unk8 = 2;
}

// NONMATCHING (85.10%): pooled string base and fp register allocation of two (f32)len conversions.
void BattleCommandDisplay_WeaponMultiItemCancelCheckDisp(CameraId camId, BattleWork* wp) {
	char buf[0x20];
	Vec pos;
	u32 len;
	f32 xoff;

	pouchGetPtr();
	sprintf(buf, msgSearch("btl_msg_multi_item_cancel_check"));
	len = strlen(buf);
	xoff = float_6p25_8041c318 * (f32)len;
	windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2c0, float_neg30_8041c344 - xoff,
	                      float_30_8041c348, float_12p5_8041c320 * (f32)len + float_65_8041c34c,
	                      float_32_8041c30c, float_10_8041c2e4);
	windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2c4, float_neg70_8041c350, float_neg10_8041c354,
	                      float_130_8041c358, float_60_8041c314, float_10_8041c2e4);
	FontDrawStart();
	FontDrawString(buf, float_0_8041c2f0 - xoff, float_27_8041c35c);
	FontDrawString("\202\315\202\242\201@", float_neg20_8041c360, float_neg13_8041c364);
	FontDrawString("\202\242\202\242\202\246", float_neg20_8041c360, float_neg38_8041c368);
	pos.x = float_neg40_8041c36c;
	pos.y = (f32)(-42 - wp->commandMenu.cursor[13].field_0 * 25);
	pos.z = float_0_8041c2f0;
	iconDispGx(&pos, 0x14, 0x1F0, float_1_8041c2f4);
}

// NONMATCHING (94.16%): register allocation (cursor folded into 0x1c50(wp)) and cmplwi vs cmpwi.
s32 BattleCommandDisplay_WeaponMultiItemCancelCheckMain(BattleWork* wp) {
	BattleCursor* cursor = &wp->commandMenu.cursor[13];
	s32 done = 0;
	s32 cancel = 0;

	if (BattleCommandAttackAudienceCheck()) {
		wp->commandMenu.unk540[0x18] = 1;
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
			wp->commandMenu.unk540[0x11] = 1;
			wp->commandMenu.unk0 = 0x1D;
		} else {
			wp->commandMenu.unk0 = 6;
		}
	}
	dispEntry(CAMERA_2D, float_900_8041c370, 0, (DispCallback)BattleCommandDisplay_WeaponMultiItemCancelCheckDisp, wp);
	return done;
}

#pragma dont_inline on
void BattleCommandDisplay_ActMenuEnd(BattleWork* wp) {
	BattleMenuDisp_ActSelect_End(wp);
	wp->commandMenu.field_4 &= ~0x1;
}
#pragma dont_inline reset

// NONMATCHING (96.67%): 11 instructions differ, 4 missing, plus register differences.
s32 BattleCommandDisplay_ActMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCursor* cursor;
	s32 result = 0;

	_btlcmd_GetCursorPtr(cmd, 0xE, &cursor);
	if (BattlePadCheckTrigger(0x800) && BattleCommandCheckChangePositionEnable(wp)) {
		psndSFXOn((char*)0x9D8);
		BattleCommandDisplay_ActMenuEnd(wp);
		*(s32*)&cmd->unk540[0] = 9;
		cmd->unk0 = 0x1D;
		BattleMenuDisp_ActSelect_Main();
		return 1;
	}
	while (BattleMenuKeyOKInACT(wp)) {
		s32 i;
		s32 t;
		for (i = 0; i < cursor->unk8; i++) {
			t = wcmd->mActionTable[i].mType;
			if (t >= 2 || t < 0) {
				continue;
			}
			if (t == 0) {
				if (!(wp->flags & 0x200)) {
					continue;
				}
			} else {
				if (!(wp->flags & 0x400)) {
					continue;
				}
			}
			_btlcmd_MakeSelectWeaponTable(wp, (void*)t);
			if (_btlcmd_CheckWeaponTargetNum(wp, unit, wcmd->mWeaponTable[0].mWeaponParams) > 0) {
				continue;
			}
			wcmd->mActionTable[i].mEnabled = 0;
			if (wcmd->mActionTable[i].field_0x10 == 0) {
				if (t == 0) {
					wcmd->mActionTable[i].field_0x10 = 4;
				} else {
					wcmd->mActionTable[i].field_0x10 = 5;
				}
			}
		}
		if ((s32)BattlePadCheckNow(-1)) {
			*(s32*)&cmd->unk560[0] = 0;
			btl_camera_set_mode(0, 0);
		} else {
			s32 v = *(s32*)&cmd->unk560[0] + 1;
			*(s32*)&cmd->unk560[0] = v;
			if (v >= 0x384) {
				btl_camera_set_mode(0, 6);
				btl_camera_set_moveSpeedLv(0, 5);
			}
		}
		if (wp->commandMenu.unk540[0x18]) {
			s32 t2 = wcmd->mActionTable[cursor->field_0].mType;
			if ((t2 == 0 && (wp->flags & 0x200)) || (t2 == 1 && (wp->flags & 0x400))) {
				wp->commandMenu.unk540[0x18] = 0;
				*(s32*)&wp->commandMenu.unk540[0x14] = 0;
			}
		}
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.unk540[0x18] != 0) {
			if (wcmd->mActionTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				*(s32*)&cmd->unk540[0] = wcmd->mActionTable[cursor->field_0].mType;
				switch (*(s32*)&cmd->unk540[0]) {
				case 0:
				case 1:
				case 4:
				case 6:
				case 7:
				case 8:
					cmd->unk0 = 5;
					result = 1;
					break;
				case 2:
					cmd->unk0 = 5;
					if (unit->moveState == 0) {
						cmd->unk540[0x10] = 1;
						if (unit->badgesEquipped.DoubleDip || unit->badgesEquipped.TripleDip) {
							cmd->unk0 = 0xE;
							result = 1;
						}
					}
					break;
				case 5:
					cmd->unk0 = 0x13;
					result = 1;
					break;
				case 13:
					cmd->unk0 = 0x1D;
					result = 1;
					break;
				}
				BattleCommandDisplay_ActMenuEnd(wp);
				goto end;
			}
			if (wp->commandMenu.unk540[0x18] == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.unk540[0x18] = 0;
			if (wcmd->mActionTable[cursor->field_0].field_0x10 != 0) {
				BattleCommandDisplay_ActMenuEnd(wp);
				cmd->unk0 = 3;
				result = 1;
				*(s32*)&cmd->unk560[4] = wcmd->mActionTable[cursor->field_0].field_0x10;
				goto end;
			}
		}
		if ((s32)BattlePadCheckRepeat(0x80000)) {
			if (cursor->field_0 < cursor->unk8 - 1) {
				cursor->field_0++;
				psndSFXOn("SFX_BTL_CURSOR_MOVE1");
				goto end;
			}
			psndSFXOn("SFX_BTL_CURSOR_MOVE1");
			cursor->field_0 = 0;
			goto end;
		}
		if ((s32)BattlePadCheckRepeat(0x40000)) {
			if (cursor->field_0 > 0) {
				cursor->field_0--;
				psndSFXOn("SFX_BTL_CURSOR_MOVE1");
				goto end;
			}
			psndSFXOn("SFX_BTL_CURSOR_MOVE1");
			cursor->field_0 = cursor->unk8 - 1;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			goto end;
		}
		if (BattlePadCheckTrigger(0x800) && BattleCommandCheckChangePositionEnable(wp)) {
			psndSFXOn((char*)0x9D8);
			BattleCommandDisplay_ActMenuEnd(wp);
			*(s32*)&cmd->unk540[0] = 9;
			result = 1;
			cmd->unk0 = 0x1D;
		}
		break;
	}
end:
	BattleMenuDisp_ActSelect_Main();
	return result;
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_WeaponSelectMenuEnd(BattleWork* wp) {
	BattleMenuDisp_WeaponSelect_End(wp);
	wp->commandMenu.field_4 &= ~0x10;
}
#pragma dont_inline reset

// NONMATCHING (59.31%): 8 instructions differ, 1 missing, 79 extra, plus register differences.
s32 BattleCommandDisplay_WeaponSelectMenuMain(BattleWork* wp, s32 arg) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCursor* cursor;
	s32 result = 0;

	while (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, arg, &cursor);
		_btlcmd_UpdateSelectWeaponTable(wp, arg);
		if (wp->commandMenu.unk540[0x18]) {
			wp->commandMenu.unk540[0x18] = 0;
			*(s32*)&wp->commandMenu.unk540[0x14] = 0;
		}
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.unk540[0x18] != 0) {
			if (wcmd->mWeaponTable[cursor->field_0].field_0x4) {
				psndSFXOn((char*)0x9D8);
				BattleCommandDisplay_WeaponSelectMenuEnd(wp);
				_btlcmd_SelectWeaponDecide(wp, arg, cursor->field_0);
				BattleCommandDisplay_TargetSelectMenuSetup(wp);
				cmd->unk0 = 0xA;
				if (!(wcmd->mWeaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x200)) {
					BattleStageOnLightFriendInCommand();
				}
				if (!(wcmd->mWeaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x4000) &&
				    !(wcmd->mWeaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x100) &&
				    !(wcmd->mWeaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x10)) {
					BattleStageOnLightEnemyInCommand();
				}
				result = 1;
				goto end;
			}
			if (wp->commandMenu.unk540[0x18] == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.unk540[0x18] = 0;
			if (wcmd->mWeaponTable[cursor->field_0].field_0x18 != 0) {
				BattleCommandDisplay_WeaponSelectMenuEnd(wp);
				result = 1;
				cmd->unk0 = 8;
				*(s32*)&cmd->unk560[4] = wcmd->mWeaponTable[cursor->field_0].field_0x18;
				goto end;
			}
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
			if (arg == 2 && (unit->badgesEquipped.DoubleDip || unit->badgesEquipped.TripleDip)) {
				if (unit->moveState == 0) {
					psndSFXOn((char*)0x9D9);
					BattleCommandDisplay_WeaponSelectMenuEnd(wp);
					cmd->unk0 = 0xF;
				} else {
					BattleCommandDisplay_WeaponSelectMenuEnd(wp);
					cmd->unk0 = 0xC;
				}
			} else {
				psndSFXOn((char*)0x9D9);
				BattleCommandDisplay_WeaponSelectMenuEnd(wp);
				cmd->unk0 = 1;
			}
			result = 1;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			BattleCommandDisplay_WeaponSelectMenuEnd(wp);
			wp->commandMenu.unk540[0x18] = 1;
		}
		break;
	}
end:
	BattleMenuDisp_WeaponSelect_Main();
	return result;
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_OperationMenuEnd(BattleWork* wp) {
	BattleMenuDisp_Operation_End(wp);
	wp->commandMenu.field_4 &= ~0x100;
}
#pragma dont_inline reset

// NONMATCHING (94.09%): switch body layout, cmplwi vs cmpwi, pooled labels.
s32 BattleCommandDisplay_OperationMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	BattleCursor* cursor;
	s32 result = 0;

	if (wp->commandMenu.unk540[0x18]) {
		wp->commandMenu.unk540[0x18] = 0;
		*(s32*)&wp->commandMenu.unk540[0x14] = 0;
	}
	if (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, 5, &cursor);
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.unk540[0x18] != 0) {
			if (wcmd->mOperationTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				*(s32*)&cmd->unk540[8] = wcmd->mOperationTable[cursor->field_0].mType;
				switch (*(s32*)&cmd->unk540[8]) {
				case 0:
					cmd->unk0 = 0x18;
					break;
				case 4:
					cmd->unk0 = 0x1B;
					break;
				case 1:
				case 2:
				case 3:
				case 5:
				case 6:
				case 7:
				case 8:
					BattleStageOffLightInCommand(0, 1);
					cmd->unk0 = 0x1D;
					break;
				}
				BattleCommandDisplay_OperationMenuEnd(wp);
				result = 1;
				goto end;
			}
			if (wp->commandMenu.unk540[0x18] == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.unk540[0x18] = 0;
			if (wcmd->mOperationTable[cursor->field_0].field_0x8 != 0) {
				BattleCommandDisplay_OperationMenuEnd(wp);
				cmd->unk0 = 0x16;
				result = 1;
				*(s32*)&cmd->unk560[4] = wcmd->mOperationTable[cursor->field_0].field_0x8;
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
			cmd->unk0 = 1;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			wp->commandMenu.unk540[0x18] = 1;
			BattleCommandDisplay_OperationMenuEnd(wp);
		}
	}
end:
	BattleMenuDisp_Operation_Main();
	return result;
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_MultiItemMenuEnd(BattleWork* wp) {
	BattleMenuDisp_MultiItem_End(wp);
	wp->commandMenu.field_4 &= ~0x20;
}
#pragma dont_inline reset

// NONMATCHING (96.60%): cmplwi vs cmpwi, pooled labels, missing mr r3 before MultiItem_Main.
s32 BattleCommandDisplay_MultiItemMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	BattleCursor* cursor;
	s32 result = 0;

	if (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, 3, &cursor);
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.unk540[0x18] != 0) {
			if (wcmd->mMultiItemTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				cmd->unk540[0x10] = wcmd->mMultiItemTable[cursor->field_0].pad_0xE[0];
				cmd->unk0 = 5;
				BattleCommandDisplay_MultiItemMenuEnd(wp);
				result = 1;
				goto end;
			}
			if (wp->commandMenu.unk540[0x18] == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.unk540[0x18] = 0;
			if (wcmd->mMultiItemTable[cursor->field_0].field_0x14 != 0) {
				BattleCommandDisplay_MultiItemMenuEnd(wp);
				cmd->unk0 = 0x11;
				result = 1;
				*(s32*)&cmd->unk560[4] = wcmd->mMultiItemTable[cursor->field_0].field_0x14;
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
			cmd->unk0 = 1;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			wp->commandMenu.unk540[0x18] = 1;
			BattleCommandDisplay_MultiItemMenuEnd(wp);
		}
	}
end:
	BattleMenuDisp_MultiItem_Main();
	return result;
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_ChangePartySelectMenuEnd(BattleWork* wp) {
	BattleMenuDisp_ChangePartySelect_End(wp);
	wp->commandMenu.field_4 &= ~0x1000;
}
#pragma dont_inline reset

void BattleCommandDisplay_ChangePartySelectMenuSetup(BattleWork* wp, void* arg) {
	BattleMenuDisp_ChangePartySelect_Init(wp, (u32)arg);
	wp->commandMenu.field_4 |= 0x01001000;
}

// NONMATCHING (96.64%): cmplwi vs cmpwi, pooled labels, missing mr r3 before ChangePartySelect_Main.
s32 BattleCommandDisplay_ChangePartySelectMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	BattleCursor* cursor;
	s32 result = 0;

	if (BattleMenuKeyOKInACT(wp)) {
		_btlcmd_GetCursorPtr(cmd, 0xA, &cursor);
		if (BattlePadCheckTrigger(0x100) || wp->commandMenu.unk540[0x18] != 0) {
			if (wcmd->mPartyTable[cursor->field_0].mEnabled) {
				psndSFXOn((char*)0x9D8);
				BattleStageOffLightInCommand(0, 1);
				*(s32*)&cmd->unk540[0xC] = wcmd->mPartyTable[cursor->field_0].mPartyId;
				cmd->unk0 = 0x1D;
				BattleCommandDisplay_ChangePartySelectMenuEnd(wp);
				result = 1;
				goto end;
			}
			if (wp->commandMenu.unk540[0x18] == 0) {
				psndSFXOn("SFX_NOT_CHOICE1");
			}
			wp->commandMenu.unk540[0x18] = 0;
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
			cmd->unk0 = 0x14;
			goto end;
		}
		if (BattleCommandAttackAudienceCheck()) {
			wp->commandMenu.unk540[0x18] = 1;
			BattleCommandDisplay_ChangePartySelectMenuEnd(wp);
		}
	}
end:
	BattleMenuDisp_ChangePartySelect_Main();
	return result;
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

// NONMATCHING (99.09%): the single-target path matches, but the compiler keeps `wp` in r26 where
// the original keeps it in r29; the multi-target loop's sampling/i/yoff then shift from r28/r27/r26
// to r29/r28/r27. The `double_to_int_*` reloc names are the known pool artifact.
void BattleCommandDisplay_TargetSelectMenuDisp(CameraId camId, BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	BattleCursor* cursor;
	BattleWeapon* weapon;
	Vec iconPos;

	_btlcmd_GetCursorPtr(cmd, *(s32*)&cmd->unk540[0], &cursor);
	weapon = wcmd->mWeaponTable[cursor->field_0].mWeaponParams;
	if (weapon->targetClassFlags & 0x01000000) {
		BattleTargetSampling* sampling = (BattleTargetSampling*)wp->field_428;
		BattleUnit* unit;
		BattleUnitPart* part;
		BattleTargetEntry* entry;
		char* name;
		f32 width;
		Vec pos;
		Vec screen;

		entry = (BattleTargetEntry*)((u8*)sampling + 4 + sampling->order[sampling->selectIdx] * 0x24);
		unit = BattleGetUnitPtr(wp, entry->unitId);
		part = BtlUnit_GetPartsPtr(unit, entry->partId);
		pos.x = entry->x;
		pos.y = entry->y;
		pos.z = entry->z;
		if ((unit->mAttributeFlags & 2) == 0) {
			btlGetScreenPoint(&pos, &screen);
			iconPos.x = screen.x;
			iconPos.y = screen.y;
			iconPos.z = float_0_8041c2f0;
			iconDispGx(&iconPos, 0x10, 0x1EF, float_1_8041c2f4);
			if (unit->currentKind == 0xE2) {
				name = pouchGetYoshiName();
			} else {
				name = (char*)msgSearch(part->partName);
			}
			width = (f32)FontGetMessageWidth(name);
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2c8, float_neg240_8041c328, float_86_8041c32c,
			                      float_16_8041c330 + width, float_32_8041c30c, float_10_8041c2e4);
			FontDrawStart();
			FontDrawString(name, float_neg232_8041c334, float_83_8041c338);
		} else {
			btlGetScreenPoint(&pos, &screen);
			iconPos.x = screen.x;
			iconPos.y = screen.y;
			iconPos.z = float_0_8041c2f0;
			iconDispGx(&iconPos, 0x18, 0x1EF, float_1_8041c2f4);
			if (unit->currentKind == 0xE2) {
				name = pouchGetYoshiName();
			} else {
				name = (char*)msgSearch(part->partName);
			}
			width = (f32)FontGetMessageWidth(name);
			windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2cc, float_neg240_8041c328, float_86_8041c32c,
			                      float_16_8041c330 + width, float_32_8041c30c, float_10_8041c2e4);
			FontDrawStart();
			FontDrawString(name, float_neg232_8041c334, float_83_8041c338);
		}
	} else {
		BattleTargetSampling* sampling = (BattleTargetSampling*)wp->field_428;
		BattleUnit* unit;
		BattleUnitPart* part;
		BattleTargetEntry* entry;
		char* name;
		f32 width;
		Vec pos;
		Vec screen;
		s32 i;
		s32 yoff;

		yoff = 0;
		for (i = 0; i < sampling->count; i++, yoff += 0x20) {
			entry = (BattleTargetEntry*)((u8*)sampling + 4 + sampling->order[i] * 0x24);
			unit = BattleGetUnitPtr(wp, entry->unitId);
			part = BtlUnit_GetPartsPtr(unit, entry->partId);
			pos.x = entry->x;
			pos.y = entry->y;
			pos.z = entry->z;
			if ((unit->mAttributeFlags & 2) == 0) {
				btlGetScreenPoint(&pos, &screen);
				iconPos.x = screen.x;
				iconPos.y = screen.y;
				iconPos.z = float_0_8041c2f0;
				iconDispGx(&iconPos, 0x10, 0x1EF, float_1_8041c2f4);
				if (unit->currentKind == 0xE2) {
					name = pouchGetYoshiName();
				} else {
					name = (char*)msgSearch(part->partName);
				}
				width = (f32)FontGetMessageWidth(name);
				windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2d0, float_neg240_8041c328,
				                      float_118_8041c33c - (f32)yoff, float_16_8041c330 + width,
				                      float_32_8041c30c, float_10_8041c2e4);
				FontDrawStart();
				FontDrawString(name, float_neg232_8041c334, float_115_8041c340 - (f32)yoff);
			} else {
				btlGetScreenPoint(&pos, &screen);
				iconPos.x = screen.x;
				iconPos.y = screen.y;
				iconPos.z = float_0_8041c2f0;
				iconDispGx(&iconPos, 0x18, 0x1EF, float_1_8041c2f4);
				if (unit->currentKind == 0xE2) {
					name = pouchGetYoshiName();
				} else {
					name = (char*)msgSearch(part->partName);
				}
				width = (f32)FontGetMessageWidth(name);
				windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2d4, float_neg240_8041c328,
				                      float_118_8041c33c - (f32)yoff, float_16_8041c330 + width,
				                      float_32_8041c30c, float_10_8041c2e4);
				FontDrawStart();
				FontDrawString(name, float_neg232_8041c334, float_115_8041c340 - (f32)yoff);
			}
		}
	}
	btl_camera_set_mode(0, 0);
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

void _btlcmd_SetAttackEvent(BattleUnit* unit, BattleCommand* cmd) {
	BattleWork* wp = _battleWorkPointer;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	s32 command = *(s32*)&cmd->unk540[0];

	if (cmd->unk540[0x11]) {
		unit->moveState = 0;
		*(s32*)unit->field_0x29C = (s32)btldefaultevt_Wait;
		*(s32*)unit->field_0x2A8 = 1;
		return;
	}

	switch (command) {
	case 5:
		switch (*(s32*)&cmd->unk540[8]) {
		case 6:
			*(s32*)unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				*(s32*)unit->field_0x29C = (s32)btldefaultevt_Defend;
			} else {
				*(s32*)unit->field_0x29C = (s32)btldefaultevt_DefendParty;
			}
			break;
		case 5:
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_Appeal;
			*(s32*)unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F2D);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F2E);
			}
			break;
		case 7:
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_Wait;
			*(s32*)unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F2D);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F2E);
			}
			break;
		case 3:
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_Escape;
			*(s32*)unit->field_0x2A8 = 1;
			if (unit->currentKind == 0xDE) {
				BtlActRec_AddCount((u8*)wp + 0x16F37);
			} else {
				BtlActRec_AddCount((u8*)wp + 0x16F38);
			}
			break;
		case 1:
			*(s32*)unit->field_0x2A8 = 0;
			if (unit->currentKind == 0xDE) {
				*(s32*)unit->field_0x29C = (s32)badgeWeapon_Charge.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F33);
			} else {
				*(s32*)unit->field_0x29C = (s32)badgeWeapon_ChargeP.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F34);
			}
			break;
		case 2:
			*(s32*)unit->field_0x2A8 = 0;
			if (unit->currentKind == 0xDE) {
				*(s32*)unit->field_0x29C = (s32)badgeWeapon_SuperCharge.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F35);
			} else {
				*(s32*)unit->field_0x29C = (s32)badgeWeapon_SuperChargeP.mAttackEvtScript;
				BtlActRec_AddCount((u8*)wp + 0x16F36);
			}
			break;
		case 0:
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_ChangeParty;
			*(s32*)unit->field_0x2A8 = 1;
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
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_ProtectPartner;
			*(s32*)unit->field_0x2A8 = 1;
			break;
		case 8:
			*(s32*)unit->field_0x29C = (s32)BtlUnit_GetData(unit, 2);
			*(s32*)unit->field_0x2A8 = 3;
			break;
		default:
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_Wait;
			*(s32*)unit->field_0x2A8 = 1;
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
		weapon = wcmd->mWeaponTable[cursor->field_0].mWeaponParams;
		*(s32*)unit->field_0x29C = (s32)weapon->mAttackEvtScript;
		*(s32*)unit->field_0x2A8 = 0;
		if (*(u32*)unit->field_0x29C == 0) {
			*(s32*)unit->field_0x29C = (s32)btldefaultevt_Wait;
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
		*(s32*)unit->field_0x29C = (s32)btldefaultevt_ChangePosition;
		*(s32*)unit->field_0x2A8 = 7;
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
		*(s32*)unit->field_0x29C = (s32)btlataudGetAttackEventPtr(unit->unitId);
		*(s32*)unit->field_0x2A8 = 3;
		if (unit->currentKind == 0xDE) {
			BtlActRec_AddCount((u8*)wp + 0x16F2B);
		} else {
			BtlActRec_AddCount((u8*)wp + 0x16F2C);
		}
		if (*(u32*)&wp->field_19064[0x1C] & 1) {
			msgWindow_Continue(*(s32*)&wp->field_19064[0x20]);
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
		*(s32*)unit->field_0x29C = (s32)btlataudGetMoveEventPtr(unit->unitId);
		*(s32*)unit->field_0x2A8 = 3;
		break;
	case 13:
		*(s32*)unit->field_0x29C = (s32)btldefaultevt_LectureEnd;
		*(s32*)unit->field_0x2A8 = 3;
		break;
	default:
		*(s32*)unit->field_0x29C = (s32)btldefaultevt_Wait;
		*(s32*)unit->field_0x2A8 = 0;
		break;
	}
}

BattleWeapon* BattleGetSelectWeapon(BattleWork* wp) {
	BattleWorkCommand* cmd = (BattleWorkCommand*)&wp->commandMenu;
	BattleCursor* cursor;

	_btlcmd_GetCursorPtr((BattleCommand*)cmd, *(s32*)&cmd->field_0x540[0], &cursor);
	return cmd->mWeaponTable[cursor->field_0].mWeaponParams;
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
#pragma dont_inline reset

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
	BattleSamplingEnemy(work, weapon, unit->unitId, belong, weapon->targetClassFlags, weapon->targetPropertyFlags, -wp->alliances[belong].attackDirection);
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

// NONMATCHING (99.97%): the code is byte-for-byte right; all eight pooled string offsets
// are 0xa8 short of the target (this function's literals land at .rodata+0x25c instead of
// +0x304, e.g. "btl_cmd_op_chg_party"). 0x28 of that is "btl_wn_dummy_noitem"/
// "msg_dummy_noitem", allocated by the local static defaultWeapon_Dummy_NoItem$743 in the
// not-yet-written _btlcmd_MakeSelectWeaponTable; the other 0x80 is the double/SFX pool
// entries the functions around this one (BattleDrawEnemyHP, the display functions) allocate
// first, which currently land after this function instead of before it. Both depend on
// functions outside this one, so they can't be fixed here.
void _btlcmd_MakeOperationTable(BattleWork* wp) {
	BattleWeapon* weapon;
	NpcBattleInfo* setup;
	BattleUnit* mario;
	BattleCommand* cmd;
	PouchData* pouch;
	BattleWorkCommand* wcmd;
	BattleCursor* cursor;
	BattleUnit* party;
	s32 count;
	s32 escapeIdx;

	mario = BattleGetUnitPtr(wp, wp->activeUnitId);
	cmd = &wp->commandMenu;
	setup = wp->fbatInfo->setup;
	pouch = pouchGetPtr();
	wcmd = (BattleWorkCommand*)cmd;

	if (cmd == NULL) {
		return;
	}
	_btlcmd_GetCursorPtr(cmd, 5, &cursor);
	count = 0;
	if (_btlcmd_MakePartyTable(wp) >= 2) {
		wcmd->mOperationTable[0].mType = 0;
		if (mario->flippedTurns) {
			wcmd->mOperationTable[0].mEnabled = 0;
			wcmd->mOperationTable[0].field_0x8 = 3;
		} else {
			wcmd->mOperationTable[0].mEnabled = 1;
			wcmd->mOperationTable[0].field_0x8 = 0;
		}
		wcmd->mOperationTable[0].mName = (char*)msgSearch("btl_cmd_op_chg_party");
		party = mario;
		if (!(mario->currentKind >= 0xE0 && mario->currentKind < 0xE7)) {
			party = BattleGetPartnerPtr(wp, mario);
		}
		BtlUnit_CanActStatus(party);
		wcmd->mOperationTable[0].mIcon = _battleGetPartyIcon(party->currentKind);
		wcmd->mOperationTable[0].mDescription = "btl_hlp_cmd_operation_changeparty";
		if (mario->mAttributeFlags & 0x10) {
			wcmd->mOperationTable[0].mEnabled = 0;
			wcmd->mOperationTable[0].field_0x8 = 3;
		}
		if (setup->field_4 & 0x04000000) {
			wcmd->mOperationTable[0].mEnabled = 0;
			wcmd->mOperationTable[0].field_0x8 = 3;
		}
		wcmd->mOperationTable[0].mWeaponCost = 0;
		count = 1;
	}
	if (pouch->mStarPowersObtained != 0) {
		wcmd->mOperationTable[count].mType = 5;
		wcmd->mOperationTable[count].mEnabled = 1;
		wcmd->mOperationTable[count].mName = (char*)msgSearch("btl_cmd_op_appeal");
		wcmd->mOperationTable[count].mIcon = 0x164;
		wcmd->mOperationTable[count].field_0x8 = 0;
		wcmd->mOperationTable[count].mWeaponCost = 0;
		wcmd->mOperationTable[count].mDescription = "btl_hlp_cmd_operation_appeal";
		count++;
	}
	if (mario->badgesEquipped.Charge) {
		weapon = mario->currentKind == 0xDE ? &badgeWeapon_Charge : &badgeWeapon_ChargeP;
		wcmd->mOperationTable[count].mType = 1;
		wcmd->mOperationTable[count].mEnabled = BtlUnit_CheckWeaponCost(mario, weapon);
		wcmd->mOperationTable[count].mWeaponCost = BtlUnit_GetWeaponCost(mario, weapon);
		if (wcmd->mOperationTable[count].mEnabled != 0) {
			wcmd->mOperationTable[count].field_0x8 = 0;
		} else {
			wcmd->mOperationTable[count].field_0x8 = 1;
		}
		wcmd->mOperationTable[count].mName = (char*)msgSearch(weapon->name);
		wcmd->mOperationTable[count].mIcon = 0x166;
		wcmd->mOperationTable[count].mDescription = (char*)weapon->description;
		if (mario->mAttributeFlags & 0x10) {
			wcmd->mOperationTable[count].mEnabled = 0;
			wcmd->mOperationTable[count].field_0x8 = 3;
		}
		count++;
	}
	if (mario->badgesEquipped.SuperCharge) {
		weapon = mario->currentKind == 0xDE ? &badgeWeapon_SuperCharge : &badgeWeapon_SuperChargeP;
		wcmd->mOperationTable[count].mType = 2;
		wcmd->mOperationTable[count].mEnabled = BtlUnit_CheckWeaponCost(mario, weapon);
		wcmd->mOperationTable[count].mWeaponCost = BtlUnit_GetWeaponCost(mario, weapon);
		if (wcmd->mOperationTable[count].mEnabled != 0) {
			wcmd->mOperationTable[count].field_0x8 = 0;
		} else {
			wcmd->mOperationTable[count].field_0x8 = 1;
		}
		wcmd->mOperationTable[count].mName = (char*)msgSearch(weapon->name);
		wcmd->mOperationTable[count].mIcon = 0x167;
		wcmd->mOperationTable[count].mDescription = (char*)weapon->description;
		if (mario->mAttributeFlags & 0x10) {
			wcmd->mOperationTable[count].mEnabled = 0;
			wcmd->mOperationTable[count].field_0x8 = 3;
		}
		count++;
	}
	wcmd->mOperationTable[count].mType = 6;
	wcmd->mOperationTable[count].mEnabled = 1;
	wcmd->mOperationTable[count].mName = (char*)msgSearch("btl_cmd_op_defend");
	wcmd->mOperationTable[count].mIcon = 0x165;
	wcmd->mOperationTable[count].field_0x8 = 0;
	wcmd->mOperationTable[count].mWeaponCost = 0;
	wcmd->mOperationTable[count].mDescription = "btl_hlp_cmd_operation_defend";
	if (mario->mAttributeFlags & 0x10) {
		wcmd->mOperationTable[count].mEnabled = 0;
		wcmd->mOperationTable[count].field_0x8 = 3;
	}
	escapeIdx = count + 1;
	wcmd->mOperationTable[escapeIdx].mType = 3;
	wcmd->mOperationTable[escapeIdx].mEnabled = BtlUnit_CanActStatus(mario);
	wcmd->mOperationTable[escapeIdx].mName = (char*)msgSearch("btl_cmd_op_escape");
	wcmd->mOperationTable[escapeIdx].mIcon = 0x163;
	wcmd->mOperationTable[escapeIdx].mWeaponCost = 0;
	if (wcmd->mOperationTable[escapeIdx].mEnabled != 0) {
		wcmd->mOperationTable[escapeIdx].field_0x8 = 0;
	} else {
		wcmd->mOperationTable[escapeIdx].field_0x8 = 1;
	}
	wcmd->mOperationTable[escapeIdx].mDescription = "btl_hlp_cmd_operation_escape";
	if (mario->mAttributeFlags & 0x10) {
		wcmd->mOperationTable[escapeIdx].mEnabled = 0;
		wcmd->mOperationTable[escapeIdx].field_0x8 = 3;
	}
	if (wp->fbatInfo->setup->field_4 & 0x10) {
		wcmd->mOperationTable[escapeIdx].mEnabled = 0;
		wcmd->mOperationTable[escapeIdx].field_0x8 = 3;
	}
	cursor->unk8 = count + 2;
	if (cursor->field_0 >= cursor->unk8) {
		cursor->field_0 = cursor->unk8 - 1;
	}
}

// Keeping the original `bl _battleGetPartyIcon` needs the dont_inline pragma on that callee.
// NONMATCHING (78.48%): instruction stream now matches; only callee-saved register
// allocation differs (target keeps mario/cmd in r25/r24 and the loop counter in r20; ours
// use r26/r25 and r24).
// NONMATCHING (78.48%): _btlcmd_MakeMultiItemTable builds the multi-item menu table (Double/Triple Dip);
// the compiler constant-folds `count` and the field offset.
void _btlcmd_MakeMultiItemTable(BattleWork* wp) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd;
	s32 count = 1;

	if ((cmd = &wp->commandMenu)) {
		BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
		BattleCursor* cursor;
		_btlcmd_GetCursorPtr(cmd, 3, &cursor);
		wcmd->mMultiItemTable[0].pad_0xE[0] = 1;
		wcmd->mMultiItemTable[0].mEnabled = 1;
		wcmd->mMultiItemTable[0].mName = (char*)msgSearch("btl_cmd_act_item");
		wcmd->mMultiItemTable[0].mIcon = 0x14C;
		wcmd->mMultiItemTable[0].mDescription = NULL;
		wcmd->mMultiItemTable[0].mWeaponCost = 0;
		if (unit->badgesEquipped.DoubleDip) {
			wcmd->mMultiItemTable[count].pad_0xE[0] = 2;
			if (unit->currentKind == UNIT_MARIO) {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_2KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch(itemDataTable[254].name);
				wcmd->mMultiItemTable[count].mIcon = (u16)itemDataTable[254].icon_id;
				wcmd->mMultiItemTable[count].mDescription = (char*)itemDataTable[254].description;
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_2KaiItem);
			} else {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_2KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch(itemDataTable[255].name);
				wcmd->mMultiItemTable[count].mIcon = (u16)itemDataTable[255].icon_id;
				wcmd->mMultiItemTable[count].mDescription = (char*)itemDataTable[255].description;
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_2KaiItem);
			}
			wcmd->mMultiItemTable[count].field_0x14 = (wcmd->mMultiItemTable[count].mEnabled == 0) ? 1 : 0;
			count = 2;
		}
		if (unit->badgesEquipped.TripleDip) {
			wcmd->mMultiItemTable[count].pad_0xE[0] = 3;
			if (unit->currentKind == UNIT_MARIO) {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_3KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch(itemDataTable[329].name);
				wcmd->mMultiItemTable[count].mIcon = (u16)itemDataTable[329].icon_id;
				wcmd->mMultiItemTable[count].mDescription = (char*)itemDataTable[329].description;
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_3KaiItem);
			} else {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_3KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch("in_3kai_item_p");
				wcmd->mMultiItemTable[count].mIcon = 0x62;
				wcmd->mMultiItemTable[count].mDescription = "msg_3kai_item_p";
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_3KaiItem);
			}
			wcmd->mMultiItemTable[count].field_0x14 = (wcmd->mMultiItemTable[count].mEnabled == 0) ? 1 : 0;
			count++;
		}
		cursor->unk8 = count;
		if (cursor->field_0 >= cursor->unk8) {
			cursor->field_0 = cursor->unk8 - 1;
		}
	}
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
