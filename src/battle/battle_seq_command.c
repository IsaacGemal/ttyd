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
#include "battle/battle_event_subset.h"
#include "battle/battle_ac_help.h"
#include "system.h"

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
extern f32 float_255_8041c308;
extern u32 dat_8041c2dc;
extern u32 dat_8041c2e0;

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

void BattleDrawEnemyHP(CameraId camId, void* param) {
	BattleWork* wp = param;
	int i;
	BattleUnit* unit;
	Vec position;
	Vec screen;
	Vec hpPos;
	GXColor color = *(GXColor*)&dat_8041c2dc;
	s32 icon;
	Mtx mtx;

	camGetPtr(CAMERA_2D); //unused, probably just stored

	for (i = 0; i < 64; i++) {
		unit = wp->units[i];
		if (unit) {
			if (!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
				if (battleCheckUnitMonosiriFlag(wp->units[i])) {
					if (!(unit->mAttributeFlags & 0x2000000)) {
						if ((wp->commandMenu.field_4 & 0x1000000) || (unit->flags & 1)) {
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
									color.a = (u8)(s32)intplGetValue(0, unit->hpGauge.field_0x8, 0x14, float_0_8041c2f0,
									                                  float_255_8041c308);
								}
							}
							icon = 20;
							if (wp->units[i]->currentHp >= 100) {
								icon = 12;
							} else if (wp->units[i]->currentHp >= 10) {
								icon = 16;
							}
							PSMTXTrans(mtx, screen.x + (f32)icon, screen.y - float_32_8041c30c, float_0_8041c2f0);
							iconNumberDispGx(mtx, wp->units[i]->currentHp, 1, *(GXColor*)&dat_8041c2e0);
							// MUST_MATCH: BattleDrawEnemyHPBar takes GXColor by value (target copies it to a temp);
							// the header declares GXColor*. The comma sequences the hpPos copy like the target.
							((void (*)(Vec*, BattleUnit*, GXColor))BattleDrawEnemyHPBar)((hpPos = screen, &hpPos), unit,
							                                                               color);
						}
					}
				}
			}
		}
	}
}

// NONMATCHING (86.95%): the `switch` reproduces the target's `beq then; b else`; the default arm's
// call matches after spilling its args to locals (`w`/`tx`/`ty`). Only the 0xA call's schedule differs
// (target keeps the `pos->x` store and colour copy late; our scheduler hoists them).
void BattleDrawEnemyHPBar(Vec* pos, BattleUnit* unit, GXColor* color) {
	CameraEntry* camera = camGetPtr(CAMERA_2D);
	BattleWorkHpGauge* gauge = &unit->hpGauge;

	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(camera->view, 0);
	{
		f32 x = pos->x - float_10_8041c2e4;
		pos->x = x;
		// MUST_MATCH: btlDispTexPlane's original signature has an unused s32 3rd parameter (target passes r5=0);
		// the header omits it. Cast calls it with it.
		((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)(
		    0xA, *color, 0, float_19_8041c2e8 + x, pos->y - float_3_8041c2ec, float_0_8041c2f0,
		    float_1_8041c2f4, float_1_8041c2f4);
	}
	switch ((s32)*(u16*)gauge->field_0x0) {
	case 1:
		if (gauge->field_0x4 > 0) {
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(camera->view, 0);
			btlDispGXQuads2DRasta(float_1_8041c2f4 + pos->x, float_2_8041c2f8 + pos->y,
			                     float_34_8041c2fc * gauge->field_0x18 + (float_1_8041c2f4 + pos->x),
			                     (float_2_8041c2f8 + pos->y) - float_8_8041c300, 0, 0, 0, color->a);
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(camera->view, 0);
		}
		((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)(
		    0xB, *color, 0,
		    float_19_8041c2e8 + pos->x - float_20_8041c304 * (float_1_8041c2f4 - gauge->field_0x14),
		    pos->y - float_3_8041c2ec, float_0_8041c2f0, gauge->field_0x14, float_1_8041c2f4);
		break;
	default:
		{
		f32 w = gauge->field_0x14;
		f32 tx = float_19_8041c2e8 + pos->x - float_20_8041c304 * (float_1_8041c2f4 - w);
		f32 ty = pos->y - float_3_8041c2ec;
		((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)(
		    0xB, *color, 0, tx, ty, float_0_8041c2f0, w, float_1_8041c2f4);
		}
		break;
	}}

void BattleCommandInit(BattleWork* wp) {
	wp->commandMenu.unk0 = 0;
	wp->flags |= 0x80;
}


// NONMATCHING (99.98%): the code matches instruction for instruction. Only the .rodata
// pool base differs, so all seven string addends fall short of the target: six by 0x24
// (e.g. "btl_cmd_act_jump" at r29+0x3b8 vs +0x3dc) and "btl_cmd_act_item" by 0xa8 (it
// dedupes into _btlcmd_MakeMultiItemTable's earlier block, +0x228 vs +0x2d0). The pool is
// missing content owned by other functions in this TU: "btl_wn_dummy_noitem" (its first
// user, _get_msg, currently drops the dead initial assignment and so never emits it),
// "msg_dummy_noitem" (the static defaultWeapon_Dummy_NoItem data in the 0%
// _btlcmd_MakeSelectWeaponTable), the double/SFX run, and _btlcmd_MakeMultiItemTable's
// dedupe anchor for "btl_cmd_act_item". None of it can be fixed from this function; the
// offsets will land once those functions are written/ordered.
void _btlcmd_MakeActClassTable(BattleWork* wp) {
	BattleUnit* mario = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	PouchData* pouch = pouchGetPtr();
	BattleWorkCommandCursor* cursor;
	u32 count;
	NpcBattleInfo* setup = wp->fbatInfo->setup;

	if (cmd == NULL) {
		return;
	}
	if (mario->currentKind == 0xDE) {
		cursor = &wcmd->mCursors[0];
	} else {
		cursor = &wcmd->mCursors[1];
	}
	count = 0;
	if (mario->currentKind == 0xDE) {
		wcmd->mActionTable[0].mType = 0;
		wcmd->mActionTable[0].mEnabled = 1;
		wcmd->mActionTable[0].field_0x10 = 0;
		if (mario->mAttributeFlags & 0x10) {
			wcmd->mActionTable[0].mEnabled = 0;
			wcmd->mActionTable[0].field_0x10 = 3;
		}
		if (pouch->jumpLevel < 1) {
			wcmd->mActionTable[0].mEnabled = 0;
		}
		if (mario->badgesEquipped.Hammerman != 0) {
			wcmd->mActionTable[0].mEnabled = 0;
			wcmd->mActionTable[0].field_0x10 = 3;
		}
		if (*(s32*)&wp->field_18FF0[0x48] /* 0x19038 */ > 0) {
			wcmd->mActionTable[0].mEnabled = 0;
			wcmd->mActionTable[0].field_0x10 = 3;
		}
		if (setup->field_4 & 0x01000000) {
			wcmd->mActionTable[0].mEnabled = 0;
			wcmd->mActionTable[0].field_0x10 = 3;
		}
		if (wp->flags & 0x06800000) {
			wcmd->mActionTable[0].mEnabled = 0;
			wcmd->mActionTable[0].field_0x10 = 3;
		}
		wcmd->mActionTable[0].mName = (char*)msgSearch("btl_cmd_act_jump");
		switch (pouch->jumpLevel) {
		case 1:
			wcmd->mActionTable[0].mIcon = 0x146;
			break;
		case 2:
			wcmd->mActionTable[0].mIcon = 0x147;
			break;
		default:
			wcmd->mActionTable[0].mIcon = 0x148;
			break;
		}
		count = 1;
	}
	if (mario->currentKind >= 0xE0 && mario->currentKind < 0xE7) {
		wcmd->mActionTable[count].mType = 6;
		if (mario->flippedTurns) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		} else {
			wcmd->mActionTable[count].mEnabled = 1;
			wcmd->mActionTable[count].field_0x10 = 0;
		}
		if (*(s32*)&wp->field_18FF0[0x54] /* 0x19044 */ > 0) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		if (wp->flags & 0x04800000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		wcmd->mActionTable[count].mName = (char*)msgSearch("btl_cmd_act_tech_party");
		wcmd->mActionTable[count].mIcon = 0x14F;
		count++;
	}
	if (mario->currentKind == 0xDE && pouch->hammerLevel >= 1) {
		wcmd->mActionTable[count].mType = 1;
		wcmd->mActionTable[count].mEnabled = 1;
		wcmd->mActionTable[count].field_0x10 = 0;
		if (mario->mAttributeFlags & 0x10) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		if (pouch->hammerLevel < 1) {
			wcmd->mActionTable[count].mEnabled = 0;
		}
		if (mario->badgesEquipped.Jumpman != 0) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		if (*(s32*)&wp->field_18FF0[0x4C] /* 0x1903C */ > 0) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		if (setup->field_4 & 0x02000000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		if (wp->flags & 0x05800000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		wcmd->mActionTable[count].mName = (char*)msgSearch("btl_cmd_act_hammer");
		switch (pouch->hammerLevel) {
		case 1:
			wcmd->mActionTable[count].mIcon = 0x149;
			break;
		case 2:
			wcmd->mActionTable[count].mIcon = 0x14A;
			break;
		default:
			wcmd->mActionTable[count].mIcon = 0x14B;
			break;
		}
		count++;
	}
	if (mario->currentKind == 0xDE || (mario->currentKind >= 0xE0 && mario->currentKind < 0xE7)) {
		wcmd->mActionTable[count].mType = 2;
		if (mario->flippedTurns) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		} else {
			wcmd->mActionTable[count].mEnabled = 1;
			wcmd->mActionTable[count].field_0x10 = 0;
		}
		if (mario->currentKind == 0xDE) {
			if (*(s32*)&wp->field_18FF0[0x50] /* 0x19040 */ > 0) {
				wcmd->mActionTable[count].mEnabled = 0;
				wcmd->mActionTable[count].field_0x10 = 3;
			}
		} else {
			if (*(s32*)&wp->field_18FF0[0x58] /* 0x19048 */ > 0) {
				wcmd->mActionTable[count].mEnabled = 0;
				wcmd->mActionTable[count].field_0x10 = 3;
			}
		}
		if (wp->flags & 0x1F800000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		wcmd->mActionTable[count].mName = (char*)msgSearch("btl_cmd_act_item");
		wcmd->mActionTable[count].mIcon = 0x14C;
		count++;
	}
	if (mario->currentKind == 0xDE && pouch->mStarPowersObtained != 0) {
		wcmd->mActionTable[count].mType = 4;
		wcmd->mActionTable[count].mEnabled = 1;
		wcmd->mActionTable[count].field_0x10 = 0;
		if (mario->mAttributeFlags & 0x10) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		if (wp->flags & 0x1B800000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		wcmd->mActionTable[count].mName = (char*)msgSearch("btl_cmd_act_super_action");
		wcmd->mActionTable[count].mIcon = 0x14D;
		count++;
	}
	if (!(wp->flags & 0x08000000)) {
		wcmd->mActionTable[count].mType = 5;
		wcmd->mActionTable[count].mEnabled = 1;
		wcmd->mActionTable[count].field_0x10 = 0;
		if (wp->flags & 0x1F800000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		wcmd->mActionTable[count].mName = (char*)msgSearch("btl_cmd_act_operation");
		wcmd->mActionTable[count].mIcon = 0x14E;
		count++;
	}
	if (wp->flags & 0x08000000) {
		wcmd->mActionTable[count].mType = 0xD;
		wcmd->mActionTable[count].mEnabled = 1;
		wcmd->mActionTable[count].field_0x10 = 0;
		wcmd->mActionTable[count].mName = (char*)msgSearch("btl_cmd_act_lecture_end");
		wcmd->mActionTable[count].mIcon = 0x14E;
		if (wp->flags & 0x10800000) {
			wcmd->mActionTable[count].mEnabled = 0;
			wcmd->mActionTable[count].field_0x10 = 3;
		}
		count++;
	}
	cursor->mNumOptions = count;
	// The original compares these signed (cmpw), though the header declares them u32.
	if ((s32)cursor->mAbsolutePos >= (s32)cursor->mNumOptions) {
		cursor->mAbsolutePos = cursor->mNumOptions - 1;
	}
}


#pragma dont_inline on
void BattleCommandDisplay_AllEnd(BattleWork* wp) {
	wp->commandMenu.field_4 &= ~0x111111;
}

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
	{
	f32 yoff = float_12p5_8041c320 * (f32)len + float_65_8041c34c;
	windowDispGX_Waku_col(0, *(GXColor*)&dat_8041c2c0, float_neg30_8041c344 - xoff,
	                      float_30_8041c348, yoff,
	                      float_32_8041c30c, float_10_8041c2e4);
	}
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

// NONMATCHING (97.59%): register allocation (cursor folded into 0x1c50(wp)) and cmplwi vs cmpwi.
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

// NONMATCHING (97.45%): 11 instructions differ, 4 missing, plus register differences.
s32 BattleCommandDisplay_ActMenuMain(BattleWork* wp) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
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

// NONMATCHING (98.88%): callee-saved register shift only (target unit=r27/cmd=r31, ours r31/r30).
s32 BattleCommandDisplay_WeaponSelectMenuMain(BattleWork* wp, s32 arg) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
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
			if ((s32)wcmd->mWeaponTable[cursor->field_0].field_0x4 != 0) {
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
			if ((s32)wcmd->mWeaponTable[cursor->field_0].field_0x18 != 0) {
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
#ifdef MUST_MATCH
	// MUST_MATCH: header declares BattleMenuDisp_WeaponSelect_Main(void), but the original passes wp in r3.
	((void (*)(BattleWork*))BattleMenuDisp_WeaponSelect_Main)(wp);
#else
	BattleMenuDisp_WeaponSelect_Main();
#endif
	return result;
}

#pragma dont_inline on // the original keeps these menu End calls out of line
void BattleCommandDisplay_OperationMenuEnd(BattleWork* wp) {
	BattleMenuDisp_Operation_End(wp);
	wp->commandMenu.field_4 &= ~0x100;
}

// NONMATCHING (96.10%): switch body layout, cmplwi vs cmpwi, pooled labels.
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

// NONMATCHING (98.95%): cmplwi vs cmpwi, pooled labels, missing mr r3 before MultiItem_Main.
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

void BattleCommandDisplay_ChangePartySelectMenuSetup(BattleWork* wp, void* arg) {
	BattleMenuDisp_ChangePartySelect_Init(wp, (u32)arg);
	wp->commandMenu.field_4 |= 0x01001000;
}

// NONMATCHING (99.27%): cmplwi vs cmpwi, pooled labels, missing mr r3 before ChangePartySelect_Main.
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

__declspec(section ".sdata") s32 _jump_badge_table[8] = {0xF0, 0xF1, 0xF4, 0xF5, 0xF6, 0xF3, 0xF2, 0x14C};
__declspec(section ".sdata") s32 _hammer_badge_table[9] = {0xF7, 0xFA, 0xF9, 0xFB, 0xFD, 0xFC, 0xF8, 0x14D, 0x14E};
// NONMATCHING (99.52%): the byte stream matches except for the prologue's `arg`/`wp`
// save pair (retail arg=r31/wp=r30; ours wp=r31/arg=r30 plus one extra `mr r0,r4`).
// Typing the parameter `s32` to try to fix the colouring hangs MWCC on the whole file.
s32 _btlcmd_MakeSelectWeaponTable(BattleWork* wp, void* arg_) {
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
	s16 arg = (s16)arg_;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleWorkCommand* wcmd = (BattleWorkCommand*)&wp->commandMenu;
	PouchData* pouch = pouchGetPtr();
	BattleCursor* cursor;
	s32 count;
	u32 i;
	s32 j;
	s32 badge;
	s16* items;
	s32 id;
	ItemData* item;
	s32 mask;
	s32 field_4;

	if (wcmd == NULL) {
		return 0;
	}
	_btlcmd_GetCursorPtr((BattleCommand*)wcmd, arg, &cursor);
	count = 0;
	switch (arg) {
	case 0:
		if (pouch->jumpLevel >= 1) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &marioDefaultWeapon_Jump;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = _getJumpIconId();
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
			}
			if ((wp->flags & 0x09000000) != 0 && wp->field_424 != -1) {
				wcmd->mWeaponTable[count].mWeaponParams = BtlUnit_GetData(BattleGetUnitPtr(wp, wp->field_424), 0x44);
			}
			count++;
		}
		if (pouch->jumpLevel >= 2) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &marioWeapon_KururinJump;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = _getJumpIconId();
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		if (pouch->jumpLevel >= 3) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &marioWeapon_JyabaraJump;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = _getJumpIconId();
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		break;
	case 1:
		if (pouch->hammerLevel >= 1) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &marioDefaultWeapon_Hammer;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = _getHammerIconId();
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
			}
			if ((wp->flags & 0x0A000000) != 0 && wp->field_424 != -1) {
				wcmd->mWeaponTable[count].mWeaponParams = BtlUnit_GetData(BattleGetUnitPtr(wp, wp->field_424), 0x45);
			}
			count++;
		}
		if (pouch->hammerLevel >= 2) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &marioWeapon_KaitenHammer;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = _getHammerIconId();
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		if (pouch->hammerLevel >= 3) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &marioWeapon_UltraHammer;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = _getHammerIconId();
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
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
				wcmd->mWeaponTable[count].mIndex = i;
				wcmd->mWeaponTable[count].mItem = badge;
				wcmd->mWeaponTable[count].mWeaponParams = itemDataTable[badge].battle_params;
				wcmd->mWeaponTable[count].field_0x4 = 0;
				wcmd->mWeaponTable[count].field_0x18 = 0;
				if (wcmd->mWeaponTable[count].mWeaponParams->item != 0) {
					wcmd->mWeaponTable[count].mIcon = *(u16*)&itemDataTable[badge].icon_id;
				} else {
					wcmd->mWeaponTable[count].mIcon = *(u16*)&wcmd->mWeaponTable[count].mWeaponParams->icon;
				}
				if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
				} else {
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
				}
				count++;
			}
		}
		break;
	case 1:
		for (i = 0; i < 9; i++) {
			badge = _hammer_badge_table[i];
			if (pouchEquipCheckBadge(badge) > 0) {
				wcmd->mWeaponTable[count].mIndex = i;
				wcmd->mWeaponTable[count].mItem = badge;
				wcmd->mWeaponTable[count].mWeaponParams = itemDataTable[badge].battle_params;
				wcmd->mWeaponTable[count].field_0x4 = 0;
				wcmd->mWeaponTable[count].field_0x18 = 0;
				if (wcmd->mWeaponTable[count].mWeaponParams->item != 0) {
					wcmd->mWeaponTable[count].mIcon = *(u16*)&itemDataTable[badge].icon_id;
				} else {
					wcmd->mWeaponTable[count].mIcon = *(u16*)&wcmd->mWeaponTable[count].mWeaponParams->icon;
				}
				if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
				} else {
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
				}
				count++;
			}
		}
		break;
	}
	switch (arg) {
	case 6:
		{ void (*fn)(BattleWorkCommand*, void*) = (void (*)(BattleWorkCommand*, void*))BtlUnit_GetData(unit, 1); fn(wcmd, &count); }
		break;
	case 2:
		items = pouch->heldItems;
		for (j = 0; j < 20; j++) {
			id = items[j];
			item = &itemDataTable[id];
			if (id != 0 && (item->usable_locations & 2) != 0) {
				wcmd->mWeaponTable[count].mIndex = j;
				wcmd->mWeaponTable[count].mItem = id;
				wcmd->mWeaponTable[count].mWeaponParams = item->battle_params;
				wcmd->mWeaponTable[count].field_0x4 = 0;
				wcmd->mWeaponTable[count].field_0x18 = 0;
				if (wcmd->mWeaponTable[count].mWeaponParams == NULL) {
					wcmd->mWeaponTable[count].mWeaponParams = &ItemWeaponData_CookingItem;
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(item->name);
					wcmd->mWeaponTable[count].mIcon = *(u16*)&item->icon_id;
				} else {
					wcmd->mWeaponTable[count].mIcon = *(u16*)&item->icon_id;
					if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
						wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
					} else {
						wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
					}
				}
				count++;
				if (count >= 0x15) {
					break;
				}
			}
		}
		if (count == 0) {
			wcmd->mWeaponTable[count].mIndex = -1;
			wcmd->mWeaponTable[count].mItem = 0;
			wcmd->mWeaponTable[count].mIndex = 0;
			wcmd->mWeaponTable[count].mWeaponParams = &defaultWeapon_Dummy_NoItem;
			wcmd->mWeaponTable[count].field_0x4 = 0;
			wcmd->mWeaponTable[count].field_0x18 = 0;
			wcmd->mWeaponTable[count].mIcon = 0x162;
			if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
			} else {
				wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
			}
			count++;
		}
		break;
	case 4:
		mask = pouch->mStarPowersObtained;
		for (j = 0; j < 16; j++) {
			if ((mask & (1 << j)) != 0 && ((BattleWeapon**)superActionTable)[j] != NULL) {
				wcmd->mWeaponTable[count].mIndex = -1;
				wcmd->mWeaponTable[count].mItem = 0;
				wcmd->mWeaponTable[count].mWeaponParams = ((BattleWeapon**)superActionTable)[j];
				wcmd->mWeaponTable[count].field_0x4 = 0;
				wcmd->mWeaponTable[count].mIcon = *(u16*)&((BattleWeapon**)superActionTable)[j]->icon;
				wcmd->mWeaponTable[count].field_0x18 = 0;
				if (wcmd->mWeaponTable[count].mWeaponParams->name != NULL) {
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(wcmd->mWeaponTable[count].mWeaponParams->name);
				} else {
					wcmd->mWeaponTable[count].mName = (char*)msgSearch(itemDataTable[wcmd->mWeaponTable[count].mWeaponParams->item].name);
				}
				count++;
				if (count >= 0x15) {
					break;
				}
			}
		}
		break;
	}
	if (count <= 0) {
		return 0;
	}
	cursor->unk8 = count;
	if (cursor->field_0 >= cursor->unk8) {
		cursor->field_0 = cursor->unk8 - 1;
	}
	while (cursor->unk8 < (field_4 = cursor->field_4) + 6) {
		if (field_4 <= 0) {
			break;
		}
		cursor->field_4--;
	}
	_btlcmd_UpdateSelectWeaponTable(wp, arg);
	return count;
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

void BattleCommandDisplay_ActMenuDisp(CameraId camId, void* param) {
	BattleWork* wp = param;
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

		for (i = 0, yoff = 0; i < sampling->count; i++, yoff += 0x20) {
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

// The three later candidate loops declare their counter in a local block. With the
// function-scope `i`, MWCC folds `i = 0` into `mr r7, r6` and schedules the hoisted
// `addi r3, r1, 0xc` (candidates) after the inits; the local counter makes it emit
// `li r7, 0` and puts the address first, matching the original.
void* BattleSetConfuseAct(BattleWork* wp, BattleUnit* unit) {
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
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
		if (wcmd->mActionTable[i].mEnabled == 0) {
			continue;
		}
		if ((s32)wcmd->mActionTable[i].mType == 4) {
			continue;
		}
		if ((s32)wcmd->mActionTable[i].mType == 2) {
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
	*(s32*)&cmd->unk540[0] = wcmd->mActionTable[cursor->field_0].mType;
	if (wcmd->mActionTable[cursor->field_0].mEnabled == 0) {
		return subsetevt_confuse_flustered;
	}
	switch (*(s32*)&cmd->unk540[0]) {
	case 0:
	case 1:
	case 2:
	case 4:
	case 6:
	case 7:
	case 8:
		_btlcmd_MakeSelectWeaponTable(wp, (void*)*(s32*)&cmd->unk540[0]);
		_btlcmd_GetCursorPtr(cmd, *(s32*)&cmd->unk540[0], &cursor);
		count = 0;
		{
			u8 n;

			for (n = 0; n < cursor->unk8; n++) {
				weapon = wcmd->mWeaponTable[n].mWeaponParams;
				if ((s32)wcmd->mWeaponTable[n].field_0x4 == 0) {
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
		if ((s32)wcmd->mWeaponTable[cursor->field_0].field_0x4 == 0) {
			return subsetevt_confuse_flustered;
		}
		_btlcmd_SelectWeaponDecide(wp, *(s32*)&cmd->unk540[0], cursor->field_0);
		weapon = wcmd->mWeaponTable[cursor->field_0].mWeaponParams;
		if (!(weapon->special & 0x10000)) {
			return subsetevt_confuse_flustered;
		}
		if ((weapon->targetClassFlags & 0x02000000) || !(weapon->targetClassFlags & 0x01000000)) {
			return subsetevt_confuse_flustered;
		}
		{
			s8 numConfused = *(s8*)&wp->field_428[0xA6C]; // 0xE94

			if (numConfused <= 0) {
				return subsetevt_confuse_flustered;
			}
			*(u8*)&wp->field_428[0xAB7] = irand(numConfused); // 0xEDF
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
				if ((s32)wcmd->mOperationTable[n].mEnabled == 0) {
					continue;
				}
				if ((s32)wcmd->mOperationTable[n].mType != 0 && (s32)wcmd->mOperationTable[n].mType != 6 &&
				    (s32)wcmd->mOperationTable[n].mType != 3) {
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
		if ((s32)wcmd->mOperationTable[cursor->field_0].mEnabled == 0) {
			return subsetevt_confuse_flustered;
		}
		*(s32*)&cmd->unk540[8] = wcmd->mOperationTable[cursor->field_0].mType;
		switch (*(s32*)&cmd->unk540[8]) {
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
					if ((s32)wcmd->mPartyTable[n].mEnabled == 0) {
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
			if ((s32)wcmd->mPartyTable[cursor->field_0].mEnabled == 0) {
				return subsetevt_confuse_flustered;
			}
			*(s32*)&cmd->unk540[0xC] = wcmd->mPartyTable[cursor->field_0].mPartyId;
			wcmd->mCursors[1].mAbsolutePos = 0;
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

s32 BattleCommandInput(BattleWork* wp) {
	s32 unitId;
	s32 partId;
	const char* sfx;
	const char* sfx2;
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd = &wp->commandMenu;
	BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
	s32 result;
	s32 action;
	BattleTargetSampling* sampling;
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
			state = cmd->unk0;
			switch (state) {
		case 0:
			if (wp->commandMenu.unk540[0x18] != 0) {
				switch (*(s32*)&cmd->unk540[0]) {
				case 0xB:
				case 0xC:
					*(s32*)&cmd->unk540[0] = *(s32*)&cmd->unk540[4];
					break;
				}
				if (wp->commandMenu.unk540[0x18] == 2) {
					wp->commandMenu.unk540[0x18] = 0;
				}
			}
			cmd->unk540[0x11] = 0;
			wp->dispFlags |= 1;
			BtlUnit_SetCommandAnimPose(unit);
			if (unit->moveState >= 5 && unit->moveState <= 9) {
				BattleStageOnLightInCommand();
				BattleStageOffLightFriendInCommand();
				BattleStageOffLightEnemyInCommand();
				cmd->unk0 = 5;
				break;
			}
			BattleCommandDisplay_ActMenuSetup(wp, 0);
			cmd->unk0 = 2;
			BattleStageOnLightInCommand();
			BattleStageOffLightFriendInCommand();
			BattleStageOffLightEnemyInCommand();
			break;
		case 1:
			BattleCommandDisplay_ActMenuSetup(wp, (void*)1);
			cmd->unk0 = 2;
			break;
		case 2:
			if (BattleCommandDisplay_ActMenuMain(wp)) {
				result = 1;
			}
			break;
		case 3:
			cmd->unk0 = 4;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			*(s32*)&cmd->unk560[8] = event->eventId;
			// fallthrough
		case 4:
			if (evtCheckID(*(s32*)&cmd->unk560[8]) != 0) {
				break;
			}
			*(s32*)&cmd->unk560[8] = 0;
			result = 1;
			cmd->unk0 = 1;
			break;
		case 5:
		case 6:
			{
			s32 a = *(s32*)&cmd->unk540[0];
			if ((a == 0 && (wp->flags & 0x200)) ||
			    (a == 1 && (wp->flags & 0x400))) {
				_btlcmd_SelectWeaponDecide(wp, a, 0);
				BattleCommandDisplay_TargetSelectMenuSetup(wp);
				cmd->unk0 = 0xA;
				BattleStageOnLightEnemyInCommand();
				break;
			}
			if (state == 5) {
				if (a == 2 && (unit->badgesEquipped.DoubleDip || unit->badgesEquipped.TripleDip)) {
					BattleCommandDisplay_WeaponSelectMenuSetup(wp, (void*)a, 0);
				} else {
					BattleCommandDisplay_WeaponSelectMenuSetup(wp, (void*)a, (void*)1);
				}
			} else {
				BattleCommandDisplay_WeaponSelectMenuSetup(wp, (void*)a, 0);
			}
			}
			BattleStageOffLightFriendInCommand();
			BattleStageOffLightEnemyInCommand();
			cmd->unk0 = 7;
			_commandRestoreRec();
			// fallthrough
		case 7:
			if (BattleCommandDisplay_WeaponSelectMenuMain(wp, *(s32*)&cmd->unk540[0])) {
				result = 1;
			}
			break;
		case 8:
			cmd->unk0 = 9;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			*(s32*)&cmd->unk560[8] = event->eventId;
			// fallthrough
		case 9:
			if (evtCheckID(*(s32*)&cmd->unk560[8]) != 0) {
				break;
			}
			*(s32*)&cmd->unk560[8] = 0;
			result = 1;
			cmd->unk0 = 6;
			break;
		case 12:
			cmd->unk0 = 0xD;
			BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(wp);
			_commandRestoreRec();
			break;
		case 13:
			if (BattleCommandDisplay_WeaponMultiItemCancelCheckMain(wp)) {
				result = 1;
			}
			break;
		case 10:
			_btlcmd_GetCursorPtr(cmd, *(s32*)&cmd->unk540[0], &cursor);
			BattleAcHelpSetHelp(wcmd->mWeaponTable[cursor->field_0].mWeaponParams->acHelpMsg);
			BattleAcHelpSetDispType(1);
			cmd->unk0 = 0xB;
			break;
		case 11:
			action = *(s32*)&cmd->unk540[0];
			sampling = (BattleTargetSampling*)wp->field_428;
			_btlcmd_GetCursorPtr(cmd, action, &cursor);
			{
				BattleTargetEntry* entry = (BattleTargetEntry*)(sampling->entries + sampling->order[sampling->selectIdx] * 0x24);

				unitId = entry->unitId;
				partId = entry->partId;
			}
			BattleSamplingEnemyUpdate((u8*)sampling);
			for (i = 0; i < sampling->count; i++) {
				BattleTargetEntry* entry = (BattleTargetEntry*)(sampling->entries + sampling->order[i] * 0x24);

				if (entry->unitId == unitId && entry->partId == partId) {
					break;
				}
			}
			if (i >= sampling->count) {
				i = 0;
			}
			sampling->selectIdx = i;
			if (wcmd->mWeaponTable[cursor->field_0].mWeaponParams->targetClassFlags & 0x01000000) {
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
				cmd->unk0 = 6;
				wp->flags &= ~0x4;
				BattleAcHelpSetDispType(0);
				BtlUnit_SetCommandAnimPose(unit);
				if ((*(s32*)&cmd->unk540[0] == 0 && (wp->flags & 0x200)) ||
				    (*(s32*)&cmd->unk540[0] == 1 && (wp->flags & 0x400))) {
					cmd->unk0 = 1;
					BattleStageOffLightEnemyInCommand();
				}
				break;
			}
			if (BattlePadCheckTrigger(0x100)) {
				psndSFXOn((char*)0x9D8);
				BattleStageOffLightInCommand(1, 1);
				cmd->unk0 = 0x1D;
				BattleCommandDisplay_TargetSelectMenuEnd(wp);
				wp->flags |= 4;
				if (action == 2 && unit->moveState == 0) {
					switch (cmd->unk540[0x10]) {
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
				wp->commandMenu.unk540[0x18] = 1;
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
			cmd->unk0 = 0x10;
			_commandRestoreRec();
			// fallthrough
		case 16:
			if (BattleCommandDisplay_MultiItemMenuMain(wp)) {
				result = 1;
			}
			break;
		case 17:
			cmd->unk0 = 0x12;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			*(s32*)&cmd->unk560[8] = event->eventId;
			// fallthrough
		case 18:
			if (evtCheckID(*(s32*)&cmd->unk560[8]) != 0) {
				break;
			}
			*(s32*)&cmd->unk560[8] = 0;
			result = 1;
			cmd->unk0 = 0xF;
			break;
		case 19:
		case 20:
			if (state == 0x13) {
				BattleCommandDisplay_OperationMenuSetup(wp, (void*)1);
			} else {
				BattleCommandDisplay_OperationMenuSetup(wp, 0);
			}
			cmd->unk0 = 0x15;
			_commandRestoreRec();
			// fallthrough
		case 21:
			if (BattleCommandDisplay_OperationMenuMain(wp)) {
				result = 1;
			}
			break;
		case 22:
			cmd->unk0 = 0x17;
			event = evtEntry(_btl_cmd_error_msg_event, 0, 0x20);
			*(s32*)&cmd->unk560[8] = event->eventId;
			// fallthrough
		case 23:
			if (evtCheckID(*(s32*)&cmd->unk560[8]) != 0) {
				break;
			}
			*(s32*)&cmd->unk560[8] = 0;
			result = 1;
			cmd->unk0 = 0x14;
			break;
		case 24:
		case 25:
			if (state == 0x18) {
				BattleCommandDisplay_ChangePartySelectMenuSetup(wp, (void*)1);
			} else {
				BattleCommandDisplay_ChangePartySelectMenuSetup(wp, 0);
			}
			cmd->unk0 = 0x1A;
			_commandRestoreRec();
			// fallthrough
		case 26:
			if (BattleCommandDisplay_ChangePartySelectMenuMain(wp)) {
				result = 1;
			}
			break;
		case 27:
			BattleCommandDisplay_ProtectPartnerSelectMenuSetup(wp);
			cmd->unk0 = 0x1C;
			_commandRestoreRec();
			// fallthrough
		case 28:
			if (BattlePadCheckTrigger(0x200)) {
				psndSFXOn((char*)0x9D9);
				BattleCommandDisplay_ProtectPartnerSelectMenuEnd(wp);
				cmd->unk0 = 0x14;
			} else if (BattlePadCheckTrigger(0x100)) {
				cmd->unk0 = 0x1D;
				BattleCommandDisplay_ProtectPartnerSelectMenuEnd(wp);
			}
			break;
		case 29:
			_btlcmd_SetAttackEvent(unit, cmd);
			cmd->field_4 &= ~0x01000000;
			wp->dispFlags &= ~1;
			BattleStageOffLightInCommand(0, 0);
			return 1;
		default:
			break;
		}
		if (wp->commandMenu.unk540[0x18] == 0) {
			break;
		}
		_commandRestoreRec();
		}
	}
	return 0;
}


// Raw offsets into the enemy-sampling buffer `wp->field_428` (battle_detect owns the layout):
// 0xA6C sample count, 0xA6D.. unit-index codes, entry size 0x24 with s16 unitId at +4, 0xAB7 result.
#pragma dont_inline on // the original calls _btlcmd_SelectWeaponDecide out of line from BattleSetConfuseAct
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
	u32 count;
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

// NONMATCHING (99.98%): the instruction stream matches; only the pooled string base differs:
// "btl_cmd_act_item"/"in_3kai_item_p"/"msg_3kai_item_p" are +0x228/+0x23c/+0x24c here vs +0x2d0/+0x2e4/+0x2f4
// in the target (0xa8 short), which lands when the unit links. `count = 0; ...; count++` keeps the `mulli` alive.
void _btlcmd_MakeMultiItemTable(BattleWork* wp) {
	BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
	BattleCommand* cmd;
	u32 count;

	if ((cmd = &wp->commandMenu)) {
		BattleWorkCommand* wcmd = (BattleWorkCommand*)cmd;
		BattleCursor* cursor;
		_btlcmd_GetCursorPtr(cmd, 3, &cursor);
		count = 0;
		wcmd->mMultiItemTable[count].pad_0xE[0] = 1;
		wcmd->mMultiItemTable[count].mEnabled = 1;
		wcmd->mMultiItemTable[count].mName = (char*)msgSearch("btl_cmd_act_item");
		wcmd->mMultiItemTable[count].mIcon = 0x14C;
		wcmd->mMultiItemTable[count].mDescription = NULL;
		wcmd->mMultiItemTable[count].mWeaponCost = 0;
		count++;
		if (unit->badgesEquipped.DoubleDip) {
			wcmd->mMultiItemTable[count].pad_0xE[0] = 2;
			if (unit->currentKind == UNIT_MARIO) {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_2KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch(itemDataTable[254].name);
				wcmd->mMultiItemTable[count].mIcon = *(u16*)&itemDataTable[254].icon_id;
				wcmd->mMultiItemTable[count].mDescription = (char*)itemDataTable[254].description;
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_2KaiItem);
			} else {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_2KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch(itemDataTable[255].name);
				wcmd->mMultiItemTable[count].mIcon = *(u16*)&itemDataTable[255].icon_id;
				wcmd->mMultiItemTable[count].mDescription = (char*)itemDataTable[255].description;
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_2KaiItem);
			}
			if (wcmd->mMultiItemTable[count].mEnabled == 0) {
				wcmd->mMultiItemTable[count].field_0x14 = 1;
			} else {
				wcmd->mMultiItemTable[count].field_0x14 = 0;
			}
			count = 2;
		}
		if (unit->badgesEquipped.TripleDip) {
			wcmd->mMultiItemTable[count].pad_0xE[0] = 3;
			if (unit->currentKind == UNIT_MARIO) {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_3KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch(itemDataTable[329].name);
				wcmd->mMultiItemTable[count].mIcon = *(u16*)&itemDataTable[329].icon_id;
				wcmd->mMultiItemTable[count].mDescription = (char*)itemDataTable[329].description;
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_3KaiItem);
			} else {
				wcmd->mMultiItemTable[count].mEnabled = BtlUnit_CheckWeaponCost(unit, &badgeWeapon_3KaiItem);
				wcmd->mMultiItemTable[count].mName = (char*)msgSearch("in_3kai_item_p");
				wcmd->mMultiItemTable[count].mIcon = 0x62;
				wcmd->mMultiItemTable[count].mDescription = "msg_3kai_item_p";
				wcmd->mMultiItemTable[count].mWeaponCost = (u8)BtlUnit_GetWeaponCost(unit, &badgeWeapon_3KaiItem);
			}
			if (wcmd->mMultiItemTable[count].mEnabled == 0) {
				wcmd->mMultiItemTable[count].field_0x14 = 1;
			} else {
				wcmd->mMultiItemTable[count].field_0x14 = 0;
			}
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

// NONMATCHING (98.59%): instruction sequence now matches; only the pooled string base
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
