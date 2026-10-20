#include "battle/battle_disp.h"
#include "battle/battle.h"
#include "battle/battle_menu_disp.h"
#include "battle/battle_unit.h"
#include "sdk/texPalette.h"
#include "data/item_data.h"
#include <dolphin/gx.h>
#include <string.h>
#include "driver/animdrv.h"
#include "system.h"
#include "battle/battle_sub.h"
#include "math_ext.h"
#include "driver/shadowdrv.h"
#include "battle/battle_break_slot.h"
#include "battle/battle_audience.h"
#include "battle/battle_stage.h"
#include "battle/battle_seq_command.h"

// LOCAL_PROTO(iconDispGx): this unit's original passes the icon id zero-extended (ItemData.icon_id read with `lhz`
// and handed over with no `extsh`), so it saw the id as u16.
void iconDispGx(f32 scale, Vec* position, s16 flags, u16 iconId);

extern BattleWork* _battleWorkPointer;

//.sdata
s32 _status_pose_table[] = {
	STATUS_INSTANT_KILL,
	STATUS_STOP,
	STATUS_FREEZE,
	STATUS_SLEEP,
	STATUS_POISON,
	STATUS_CONFUSE,
	STATUS_DIZZY,
	-1
};



extern ItemData itemDataTable[];

static void btlDispAnimeSpeedReset(BattleUnit* unit);

// Keep searchPoseTbl out of line: the original calls it rather than inlining.
#pragma dont_inline on
const char* searchPoseTbl(BattleDispPose* table, s32 poseId) { //TODO: fix mtctr
	int i;

	if (!table) return NULL;
	for (i = 0x1C; i != 0; i--) {
		if ((s32)table->poseId == poseId || (s32)table->poseId == 0x1C) {
			return table->poseName;
		}
		table++;
	}
	return NULL;
}
#pragma dont_inline reset

// Keep btlDispGXInit2DSub out of line: the original calls it rather than inlining.
#pragma dont_inline on
void btlDispGXInit2DSub(void) {
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
	GXClearVtxDesc();
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_NONE);
}
#pragma dont_inline reset

void btlDispGXInit2DRasta(void) {
	btlDispGXInit2DSub();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
}

void btlDispGXInit2D(void) {
	btlDispGXInit2DSub();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GXSetNumTevStages(1);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
}

// Keep btlDispGXPoint2DRasta out of line: the original calls it rather than inlining.
#pragma dont_inline on
void btlDispGXPoint2DRasta(f32 x, f32 y, u8 r, u8 g, u8 b, u8 a) {
	GXPosition3f32(x, y, 0.0f);
	GXColor4u8(r, g, b, a);
}
#pragma dont_inline reset

void btlDispGXQuads2DRasta(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	btlDispGXPoint2DRasta(x0, y0, r, g, b, a);
	btlDispGXPoint2DRasta(x1, y0, r, g, b, a);
	btlDispGXPoint2DRasta(x1, y1, r, g, b, a);
	btlDispGXPoint2DRasta(x0, y1, r, g, b, a);
}

// Keep btlDispGXPoint2D out of line: the original calls it rather than inlining.
#pragma dont_inline on
void btlDispGXPoint2D(f32 x, f32 y, u8 r, u8 g, u8 b, u8 a, f32 s, f32 t) {
	GXPosition3f32(x, y, 0.0f);
	GXColor4u8(r, g, b, a);
	GXTexCoord2f32(s, t);
}
#pragma dont_inline reset

void btlDispGXQuads2D(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	btlDispGXPoint2D(x0, y0, r, g, b, a, 0.0f, 0.0f);
	btlDispGXPoint2D(x1, y0, r, g, b, a, 1.0f, 0.0f);
	btlDispGXPoint2D(x1, y1, r, g, b, a, 1.0f, 1.0f);
	btlDispGXPoint2D(x0, y1, r, g, b, a, 0.0f, 1.0f);
}

// Keep btlDispTexPlaneInit out of line: the original calls it rather than inlining.
#pragma dont_inline on
void btlDispTexPlaneInit(void) {
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}
#pragma dont_inline reset

void btlDispTexPlainGX(u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3) {
	GXTexObj texobj;
	f32 x, y;
	
	TEXGetGXTexObjFromPalette(*_battleWorkPointer->menuTex->data, &texobj, id);
	GXLoadTexObj(&texobj, GX_TEXMAP0);
	// A switch, not if/else: it keeps the original `beq then; b else` layout (if/else folds to `bne`).
	switch (GXGetTexObjFmt(&texobj)) {
	case 0:
		GXSetNumTevStages(1);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		break;
	default:
		GXSetNumTevStages(1);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		break;
	}

	x = 0.5f * (f32)GXGetTexObjWidth(&texobj);
	y = 0.5f * (f32)GXGetTexObjHeight(&texobj);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-x, y, 0.0f);
	GXColor4u8(color0.r, color0.g, color0.b, color0.a);
	GXTexCoord2f32(0.0f, 0.0f);

	GXPosition3f32(x, y, 0.0f);
	GXColor4u8(color1.r, color1.g, color1.b, color1.a);
	GXTexCoord2f32(1.0f, 0.0f);

	GXPosition3f32(x, -y, 0.0f);
	GXColor4u8(color2.r, color2.g, color2.b, color2.a);
	GXTexCoord2f32(1.0f, 1.0f);

	GXPosition3f32(-x, -y, 0.0f);
	GXColor4u8(color3.r, color3.g, color3.b, color3.a);
	GXTexCoord2f32(0.0f, 1.0f);
}

void btlDispTexPlane(u32 id, GXColor color, f32 tX, f32 tY, f32 tZ, f32 sX, f32 sY) {
	CameraEntry* camera;
	Mtx scale, trans, mtx;

	btlDispTexPlaneInit();
	camera = camGetCurPtr();
	PSMTXScale(scale, sX, sY, 1.0f);
	PSMTXTrans(trans, tX, tY, tZ);
	PSMTXConcat(trans, scale, mtx);
	PSMTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	btlDispTexPlainGX(id, color, color, color, color);
}

// Keep btlDispTexPlane2 out of line: the original calls it rather than inlining.
#pragma dont_inline on
void btlDispTexPlane2(Mtx mtx, u32 id, GXColor color, s32 unused) {
	CameraEntry* camera;
	Mtx mtx2;

	btlDispTexPlaneInit();
	MTXCopy(mtx, mtx2);
	camera = camGetCurPtr();
	MTXConcat(camera->view, mtx2, mtx2);
	GXLoadPosMtxImm(mtx2, 0);
	btlDispTexPlainGX(id, color, color, color, color);
}
#pragma dont_inline reset

void btlDispTexPlane3(Mtx mtx, u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3) {
	CameraEntry* camera;
	Mtx mtx2;

	btlDispTexPlaneInit();
	PSMTXCopy(mtx, mtx2);
	camera = camGetCurPtr();
	PSMTXConcat(camera->view, mtx2, mtx2);
	GXLoadPosMtxImm(mtx2, 0);
	btlDispTexPlainGX(id, color0, color1, color2, color3);
}

// Keep btlDispGetTexSize out of line: the original calls it rather than inlining.
#pragma dont_inline on
void btlDispGetTexSize(u32 id, u16* width, u16* height) {
	GXTexObj texobj;

	TEXGetGXTexObjFromPalette(*_battleWorkPointer->menuTex->data, &texobj, id);
	*width = GXGetTexObjWidth(&texobj);
	*height = GXGetTexObjHeight(&texobj);
}
#pragma dont_inline reset

void btlDispInit(void) {
	BattleWork* wp = _battleWorkPointer;
	
	battleCameraInit();
	battleMenuDispInit();
	wp->dispFlags |= 2;
}

// The header's BtlUnit_CheckStatus second parameter is StatusEffectType (int), but the callee
// sign-extends it and battle_status_effect.h says it "should be an s8". Calling through a matching
// function pointer makes MWCC coerce the argument like the original (`extsb r4,r30` after `mr r3,r29`).
// Keep _GetStatusPoseType out of line: the original calls it rather than inlining it into btlDispMain.
#pragma dont_inline on
StatusEffectType _GetStatusPoseType(BattleUnit* unit) {
	s32* table = _status_pose_table;
	StatusEffectType type;

	if (unit->mAttributeFlags & kInactive) {
		return STATUS_INSTANT_KILL;
	}
	while (1) {
		type = *table;
		if (*table == -1) break;
#ifdef MUST_MATCH
		if (((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
		if (BtlUnit_CheckStatus(unit, type)) {
#endif
			return type;
		}
		table++;
	}
	return STATUS_INVALID;
}
#pragma dont_inline reset

// `_GetStatusPoseType` and `BattleStockExpDisp` are kept out of line so the original's `bl`s are
// emitted. Case 2 matches only with the `double fabsx` local below: it keeps the __fabs result in f4
// while the 10.0 double stays in f0, which the bare `10.0 < __fabs(...)` form gets backwards.
// The second status gain writes the sum as `(f32)strength + 1.0f`, so the 1.0 constant is
// materialised before the int->float conversion. That orders the temp webs as retail does: 1.0 in
// f3 and the 2.0 limit in f0.
void btlDispMain(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit;
	BattleUnit* other;
	BattleUnitPart* part;
	const char* animName;
	s8 turns;
	s8 strength;
	s32 i;
	s32 statusPose;
	s32 unk;
	Vec pos2;
	Vec pos1;
	Vec v2;
	Vec sp;
	Vec sp2;
	Vec partnerPos;
	Vec vivian;
	Vec v;
	f32 wx, wy, wz;
	f32 floor;

	for (i = 0; i < 0x40; i++) {
		unit = BattleGetUnitPtr(_battleWorkPointer, i);
		if (unit == NULL) {
			continue;
		}

		if (unit->currentKind == UNIT_SHELL_SHIELD && unit->work[2] != 0) {
			other = BattleGetUnitPtr(wp, unit->work[0]);
			if (other != NULL) {
				if (!((other->attackEventId != 0 && evtCheckID(other->attackEventId) != 0 &&
				       !(*(s32*)other->field_0x2A8 & 4)) ||
				      (other->damageEventId != 0 && evtCheckID(other->damageEventId) != 0))) {
					BtlUnit_GetPos(unit, &pos1.x, &pos1.y, &pos1.z);
					BtlUnit_GetPos(other, &pos2.x, &pos2.y, &pos2.z);
					pos2.z += 7.0f;
					floor = BattleGetFloorHeight(wp, pos2.x, pos2.y, pos2.z);
					switch (unit->work[2]) {
					case 2: {
						double fabsx = __fabs(pos1.x - pos2.x);
						if (fabsx > 10.0) {
							if (pos2.x > pos1.x) {
								pos1.x = pos1.x + ((pos2.x - pos1.x) - 10.0f);
							} else {
								pos1.x = pos1.x - ((pos1.x - pos2.x) - 10.0f);
							}
						}
						BtlUnit_SetPos(unit, pos1.x, pos2.y, pos2.z);
						break;
					}
					case 3:
						if (__fabs(pos1.x - pos2.x) < 1.0) {
							BtlUnit_SetPos(unit, pos2.x, floor, pos2.z);
							unit->work[2] = 1;
						} else {
							if (pos2.x > pos1.x) {
								pos1.x++;
								BtlUnit_SetPos(unit, pos1.x, floor, pos2.z);
							} else {
								pos1.x--;
								BtlUnit_SetPos(unit, pos1.x, floor, pos2.z);
							}
						}
						break;
					default:
						BtlUnit_SetPos(unit, pos2.x, floor, pos2.z);
						break;
					}
				}
			}
		}

		unk = unit->mMoveColorCur;
		BtlUnit_OffUnitFlag(unit, 2);
		if (unk != unit->mMoveColorLv) {
			BtlUnit_OnUnitFlag(unit, 2);
			if (unit->mMoveColorLv > unk) {
				unk += 0xC;
				if (unk > unit->mMoveColorLv) {
					unk = unit->mMoveColorLv;
				}
			} else {
				unk -= 2;
				if (unk < unit->mMoveColorLv) {
					unk = unit->mMoveColorLv;
				}
			}
		}
		unit->mMoveColorCur = unk;

		BtlUnit_HpGaugeMain(unit);
		unit->mSizeMultiplier = 1.0f;
		BtlUnit_GetStatus(unit, STATUS_TINY, &turns, &strength);
		if (turns > 0 && strength < 0) {
			f32 f = 0.5f * (f32)strength + 1.0f;
			if (f < 0.5f) {
				f = 0.5f;
			}
			unit->mSizeMultiplier = unit->mSizeMultiplier * f;
		}
		BtlUnit_GetStatus(unit, STATUS_HUGE, &turns, &strength);
		if (turns > 0 && strength > 0) {
			f32 f = (f32)strength + 1.0f;
			if (f > 2.0f) {
				f = 2.0f;
			}
			unit->mSizeMultiplier = unit->mSizeMultiplier * f;
		}
		BattleStatusEffectMain(unit);
		BattleStatusIconMain(unit);

		part = unit->mParts;
		while (part != NULL) {
			if (part->poseId != -1) {
			animName = part->poseName;
			if (!(part->partFlags & 2)) {
				statusPose = _GetStatusPoseType(unit);
				if (unit->currentKind >= UNIT_GOOMBA && unit->currentKind < UNIT_UNUSED_TREE) {
					animName = searchPoseTbl((BattleDispPose*)part->poseTable, statusPose);
				} else {
					if (statusPose == STATUS_INVALID && !BtlUnit_CheckStatus(unit, STATUS_HOLD_FAST) &&
					    (unit->mStatusFlags & 0x20000000)) {
						animName = BtlUnit_GetPoseNameFromType(part, 0x39);
						if (animName == NULL) {
							animName = searchPoseTbl((BattleDispPose*)part->poseTable, 0x1C);
						}
					} else if (statusPose == STATUS_INVALID && !BtlUnit_CheckStatus(unit, STATUS_HOLD_FAST) &&
					           (unit->mStatusFlags & 0x10000000)) {
						animName = BtlUnit_GetPoseNameFromType(part, 0x38);
						if (animName == NULL) {
							animName = searchPoseTbl((BattleDispPose*)part->poseTable, 0x1C);
						}
					} else {
						animName = searchPoseTbl((BattleDispPose*)part->poseTable, statusPose);
					}
					if (BtlUnit_CheckStatusFlag(unit, 0x1000000) && BtlUnit_CanActStatus(unit)) {
						animName = BtlUnit_GetPoseNameFromType(part, 0x2D);
					}
				}
				if (BtlUnit_CheckStatus(unit, STATUS_FAST)) {
					btlDispAnimeSpeed((BattleUnit*)part, 2.0f);
				} else if (BtlUnit_CheckStatus(unit, STATUS_SLOW)) {
					btlDispAnimeSpeed((BattleUnit*)part, 0.5f);
				} else {
					btlDispAnimeSpeedReset((BattleUnit*)part);
				}
			} else {
				btlDispAnimeSpeedReset((BattleUnit*)part);
			}

			if (part->partFlags & 1) {
				s32 changed = 0;

				part->partFlags &= ~1;
				if ((part->partFlags &= ~0x200) != 0) {
					animPoseSetAnim(part->poseId, animName, 1);
				} else {
					animPoseSetAnim(part->poseId, animName, 0);
				}
				part->statusPoseStep = 0;
				if (part->partFlags & 4) {
					part->statusPoseKind = 0;
				} else {
					part->statusPoseKind = 1;
					if (BtlUnit_CheckStatus(unit, 0x12) ||
					    part->color.a != 0xFF) {
						changed = 1;
						part->statusPoseKind = 7;
					}
				}
				if (!((part->partFlags & 2) && !(part->partFlags & 8) &&
				      !(part->partFlags & 0x80))) {
					part->statusPoseKind = 1;
					if (BtlUnit_CheckStatus(unit, 0x12) ||
					    part->color.a != 0xFF) {
						changed = 1;
						part->statusPoseKind = 7;
					}
					if (BtlUnit_CheckStatus(unit, 7)) {
						changed = 1;
						part->statusPoseKind = 4;
					}
				}
				if (changed && (part->attributes & 0x8000)) {
					changed = 0;
					part->statusPoseKind = 0;
				}
				if (!(part->partFlags & 4)) {
					if (changed && !(part->partFlags & 0x40)) {
						part->partFlags |= 0x40;
					} else if (!changed && (part->partFlags & 0x40)) {
						part->partFlags &= ~0x40;
					}
				}
			}

			gravityOffsetControl(part);
			floatOffsetControl(part);
			statusPoseControl(part);
			BtlUnit_GetPartsWorldPos(part, &wx, &wy, &wz);
			animPoseMain(part->poseId);
			v = (Vec){ 0.0f, 0.0f, 0.0f };
			v.x = wx;
			v.y = wy;
			v.z = wz + *(f32*)(part->field_0x4F4 + 4);
			v2 = v;

			part->field_0x4F4[3] = (u8)part->mKindPartParams->baseAlpha;
			if (BtlUnit_CheckStatus(unit, 0x12) || (unit->mAttributeFlags & 0x8)) {
				part->field_0x4F4[3] =
					(u8)(part->field_0x4F4[3] * 0x80 / 0xFF);
			}
			part->field_0x4F4[3] =
				(u8)(part->field_0x4F4[3] * part->color.a / 0xFF);
			part->field_0x4F4[0] = part->color.r;
			part->field_0x4F4[1] = part->color.g;
			part->field_0x4F4[2] = part->color.b;

			if (!(unit->mAttributeFlags & 0x1000000) && !(part->attributes & 0x1000000)) {
				if (!(part->attributes & kMainPart) || !(unit->mAttributeFlags & kVeiled)) {
					if (part->field_0x4F4[3] < 0xFF ||
					    animPoseTestXLU(part->poseId)) {
						if (!(part->partFlags & 0x100)) {
							dispEntry(4, dispCalcZ(&v2), 2, btlUnitPartsDisp, part);
						}
					} else {
						if (!(part->partFlags & 0x100)) {
							dispEntry(4, dispCalcZ(&v2), 1, btlUnitPartsDisp, part);
						}
					}
				}
				if (part->attributes & 0x4000000) {
					dispEntry(4, wz, 2, (DispCallback)btlUnitPartsBlurDisp, part);
				}
			}

			if (!(unit->mAttributeFlags & 0x20000000) && !(part->attributes & 0x2000000)) {
				BtlUnit_GetPartsWorldPos(part, &sp.x, &sp.y, &sp.z);
				sp2 = sp;
				if (unit->flags & 0x10000) {
					other = BattleGetPartnerPtr(wp, unit);
					if (other != NULL) {
						BtlUnit_GetPos(other, &partnerPos.x, &partnerPos.y, &partnerPos.z);
						sp2.x += partnerPos.x;
						sp2.x *= 0.5f;
						sp2.y += partnerPos.y;
						sp2.y *= 0.5f;
						sp2.z += partnerPos.z;
						sp2.z *= 0.5f;
					}
				}
				{
					f32 f;
					f32 floorY;

					sp2.y = BattleGetFloorHeight(wp, sp2.x, sp2.y, sp2.z);
					floorY = sp2.y;
					if (sp.y <= floorY) {
						f = 1.0f;
					} else {
						f = (100.0f - (sp.y - floorY)) / 100.0f;
					}
					if (f > 1.0) {
						f = 1.0f;
					}
					if (f < 0.1f) {
						f = 0.1f;
					}
					f *= (f32)*(s16*)part->field_0x1A2 * unit->mScale.x *
					     part->mScale.x * unit->mSizeMultiplier;
					f *= part->field_0x78;
					if (unit->flags & 0x10000) {
						if (f < 16.0f) {
							f = 16.0f;
						}
					}
					if (f < 10.0f) {
						f = 8.0f;
					}
					shadowEntry(sp2.x, floorY + 10.0f, sp2.z, f);
				}
			}

			// Shadow Sirens (and Doopliss as Vivian) run the Vivian model animation.
			if (unit->currentKind == UNIT_VIVIAN || unit->currentKind == UNIT_DOOPLISS_CHAPTER8_VIVIAN ||
			    unit->currentKind == UNIT_MARILYN_CHAPTER8 ||
			    (u32)(unit->currentKind - UNIT_BELDAM_CHAPTER2) <= 2 ||
			    (u32)(unit->currentKind - UNIT_UNUSED_BELDAM2) <= 4 ||
			    unit->currentKind == UNIT_BELDAM_CHAPTER8) {
				vivian.x = (f32)unit->work[0];
				vivian.y = (f32)unit->work[1];
				vivian.z = (f32)unit->work[2];
				animPoseVivianMain(part->poseId, &vivian);
			}
			BtlUnit_ControlPoseSoundMain(part);
			}
			part = part->mNextPart;
		}

		if (unit->heldItem != 0) {
			if ((unit->flags & 0x40000000) && !(unit->flags & 0x08000000)) {
				dispEntry(4, 0.0f, 2, btlUnitStolenItemDisp, unit);
			} else if (!(unit->flags & 0x04000000) || (unit->flags & 0x02000000)) {
				if (!(unit->flags & 0x08000000)) {
					dispEntry(4, 0.0f, 2, (DispCallback)btlUnitItemDisp, unit);
				}
			}
		} else {
			if (wp->flags & 0x20000) {
				dispEntry(4, 0.0f, 2, (DispCallback)btlUnitItemDisp2, unit);
			}
		}
	}

	BattleStockExpDisp();
	BattleStageDisp();
	BattleCommandDisplay(wp);
	BattleAudience_Disp();
	BattleBreakSlot_Disp();
	if (wp->flags & 0x10000) {
		wp->flags &= ~0x10000;
	}
	if (wp->flags & 0x20000) {
		wp->flags &= ~0x20000;
	}
}

// The f26-f31 locals are reused across both branches; their declaration order (and wp before parts) sets
// MWCC's callee-saved register assignment. The three callees are kept as real `bl`s with #pragma dont_inline
// because the target does not inline them.
void btlUnitPartsDisp(CameraId camId, void* param) {
	BattleWork* wp;
	BattleUnitPart* parts = param;
	BattleUnit* owner;
	CameraEntry* cam;
	// Order matters for MWCC's callee-saved FPR allocation (do not sort).
	f32 angle;
	f32 f31;
	f32 f29;
	f32 f30;
	f32 f28;
	f32 f27;
	f32 flip;
	f32 f26;
	Mtx m28;
	Mtx m27;
	Mtx m26;
	Mtx m25;
	Mtx m24;
	Mtx m23;
	Mtx m22;
	Mtx m21;
	Mtx m20;
	Mtx m19;
	Mtx m18;
	Mtx m17;
	Mtx m16;
	Mtx m15;
	Mtx m14;
	Mtx m13;
	Mtx m12;
	Mtx m11;
	Mtx m10;
	Mtx m9;
	Mtx m8;
	Mtx m7;
	Mtx m6;
	Mtx m5;
	Mtx m4;
	Mtx m3;
	Mtx m2;
	Mtx m1;
	Mtx m0;

	(void)camId;
	wp = _battleWorkPointer;
	flip = 1.0f;
	owner = parts->mOwner;
	if (parts->poseId < 0) {
		return;
	}

	PSMTXTrans(m11, -owner->mRotationOffset.x, -owner->mRotationOffset.y, -owner->mRotationOffset.z);
	PSMTXTrans(m10, owner->mRotationOffset.x, owner->mRotationOffset.y, owner->mRotationOffset.z);
	PSMTXTrans(m15, owner->mDisplayOffset.x, owner->mDisplayOffset.y, owner->mDisplayOffset.z);
	PSMTXTrans(m16, owner->mPosition.x, owner->mPosition.y, owner->mPosition.z);
	PSMTXRotRad(m14, 'x', 0.017453292f * (owner->mRotation.x + owner->mBaseRotation.x));
	PSMTXRotRad(m13, 'y', 0.017453292f * (owner->mRotation.y + owner->mBaseRotation.y));
	PSMTXRotRad(m12, 'z', 0.017453292f * (owner->mRotation.z + owner->mBaseRotation.z));
	PSMTXScale(m9, owner->mSizeMultiplier * (owner->mBaseScale.x * owner->mScale.x),
	                   owner->mSizeMultiplier * (owner->mBaseScale.y * owner->mScale.y),
	                   owner->mBaseScale.z * owner->mScale.z);
	PSMTXIdentity(m24);
	PSMTXConcat(m9, m24, m24);
	PSMTXConcat(m11, m24, m24);
	PSMTXConcat(m13, m24, m24);
	PSMTXConcat(m12, m24, m24);
	PSMTXConcat(m14, m24, m24);
	PSMTXConcat(m10, m24, m28);
	PSMTXConcat(m15, m28, m28);


	f27 = 270.0f;
	if (parts->attributes & kAbsolutePosition) {
		if (parts->faceDirection >= 0) {
			f27 = 90.0f;
		}
		f29 = parts->mPosition.x;
// dont_inline: retail keeps these real bls from btlUnitPartsDisp.
#pragma dont_inline on
		f30 = parts->mPosition.y + getGravityDispOffset((BattleUnit*)parts) + getFloatDispOffset((BattleUnit*)parts);
		f28 = parts->mPosition.z;
		cam = camGetCurPtr();
		f31 = angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z);
		f31 = reviseAngle(f31 - angleABf(cam->cameraPos.x, cam->cameraPos.z, f29, f28));

		PSMTXTrans(m22, -parts->mRotationOffset.x, -parts->mRotationOffset.y, -parts->mRotationOffset.z);
		PSMTXTrans(m21, parts->mRotationOffset.x, parts->mRotationOffset.y, parts->mRotationOffset.z);
		PSMTXRotRad(m20, 'x', 0.017453292f * (parts->mRotation.x + parts->mBaseRotation.x));
		PSMTXRotRad(m19, 'y', 0.017453292f * (parts->mRotation.y + parts->mBaseRotation.y + f31));
		PSMTXRotRad(m18, 'z', 0.017453292f * (parts->mRotation.z + parts->mBaseRotation.z));
		PSMTXTrans(m23, f29, f30, f28);

		f30 = reviseAngle(-1.0f * f27 - 90.0f);
		f28 = reviseAngle(parts->mBaseRotation.y + (f30 + parts->mRotation.y));
		cam = camGetCurPtr();
		angle = angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z);
		{
			f32 f0 = reviseAngle(f28 + angle);
			if (f0 >= 90.0f && f0 <= 270.0f) {
				flip *= -1.0f;
				PSMTXRotRad(m19, 'y', 0.017453292f *
					(f31 + (parts->mRotation.y + parts->mBaseRotation.y) -
					 2.0f * (180.0f - (f0 - angle))));
			}
		}

		if (parts->attributes & 0x08000000) {
			PSMTXScale(m26,
				parts->mBaseScale.x * parts->mScale.x,
				parts->mBaseScale.y * parts->mScale.y,
				flip * (parts->mBaseScale.z * parts->mScale.z));
		} else {
			PSMTXScale(m26,
				owner->mSizeMultiplier * (parts->mBaseScale.x * parts->mScale.x),
				owner->mSizeMultiplier * (parts->mBaseScale.y * parts->mScale.y),
				flip * (parts->mBaseScale.z * parts->mScale.z));
		}

		PSMTXIdentity(m17);
		PSMTXConcat(m26, m17, m17);
		PSMTXConcat(m22, m17, m17);
		PSMTXConcat(m18, m17, m17);
		PSMTXConcat(m20, m17, m17);
		PSMTXConcat(m19, m17, m17);
		PSMTXConcat(m21, m17, m17);
		PSMTXConcat(m23, m17, m27);
		PSMTXCopy(m27, m25);
		PSMTXTrans(m0, 0.0f, 0.0f, -5.0f);
		PSMTXConcat(m0, m25, m25);
	} else {
		f32 dir;
		f32 sign;

		if (owner->faceDirection >= 0) {
			f27 = 90.0f;
		}
		dir = (f32)parts->faceDirection;
		if (dir > 0.0f) {
			sign = 1.0f;
		} else if (dir < 0.0f) {
			sign = -1.0f;
		} else {
			sign = (f32)wp->alliances[owner->mAlliance].attackDirection;
		}
		f30 = parts->mPositionOffset.x * sign + owner->mPosition.x;
		f29 = owner->mPosition.y + parts->mPositionOffset.y + getGravityDispOffset((BattleUnit*)parts) +
		      getFloatDispOffset((BattleUnit*)parts);
		f31 = owner->mPosition.z + parts->mPositionOffset.z;
		f28 = 0.0f;
		if (parts->mOwner->mAttributeFlags & 0x04000000) {
			cam = camGetCurPtr();
			f26 = angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z);
			f28 = reviseAngle(f26 - angleABf(cam->cameraPos.x, cam->cameraPos.z, f30, f31));
		}

		PSMTXTrans(m3, -parts->mRotationOffset.x, -parts->mRotationOffset.y, -parts->mRotationOffset.z);
		PSMTXTrans(m2, parts->mRotationOffset.x, parts->mRotationOffset.y, parts->mRotationOffset.z);
		PSMTXRotRad(m6, 'x', 0.017453292f * (parts->mRotation.x + parts->mBaseRotation.x));
		PSMTXRotRad(m5, 'y', 0.017453292f * (parts->mRotation.y + parts->mBaseRotation.y + f28));
		PSMTXRotRad(m4, 'z', 0.017453292f * (parts->mRotation.z + parts->mBaseRotation.z));
		PSMTXTrans(m8, f30, f29, f31);
		PSMTXTrans(m7, parts->mDisplayOffset.x, parts->mDisplayOffset.y, parts->mDisplayOffset.z);

		f30 = reviseAngle(-1.0f * f27 - 90.0f);
		if (!(parts->mOwner->flags & 0x1000000)) {
			f26 = reviseAngle(parts->mBaseRotation.y + (owner->mBaseRotation.y +
				(parts->mRotation.y + (f30 + owner->mRotation.y))));
			cam = camGetCurPtr();
			{
				f32 f0 = reviseAngle(f26 + angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z));
				if (f0 >= 90.0f && f0 <= 270.0f) {
					PSMTXScale(m9,
						owner->mSizeMultiplier * (owner->mBaseScale.x * owner->mScale.x),
						owner->mSizeMultiplier * (owner->mBaseScale.y * owner->mScale.y),
						-1.0f * (owner->mBaseScale.z * owner->mScale.z));
					PSMTXIdentity(m24);
					PSMTXConcat(m9, m24, m24);
					PSMTXConcat(m11, m24, m24);
					switch (*(s8*)((char*)parts + 0x4FC)) {
					case 0:
						PSMTXConcat(m13, m24, m24);
						PSMTXConcat(m12, m24, m24);
						PSMTXConcat(m14, m24, m24);
						break;
					case 1:
						PSMTXConcat(m13, m24, m24);
						PSMTXConcat(m14, m24, m24);
						PSMTXConcat(m12, m24, m24);
						break;
					case 2:
						PSMTXConcat(m14, m24, m24);
						PSMTXConcat(m13, m24, m24);
						PSMTXConcat(m12, m24, m24);
						break;
					case 3:
						PSMTXConcat(m14, m24, m24);
						PSMTXConcat(m12, m24, m24);
						PSMTXConcat(m13, m24, m24);
						break;
					case 4:
						PSMTXConcat(m12, m24, m24);
						PSMTXConcat(m14, m24, m24);
						PSMTXConcat(m13, m24, m24);
						break;
					case 5:
						PSMTXConcat(m12, m24, m24);
						PSMTXConcat(m13, m24, m24);
						PSMTXConcat(m14, m24, m24);
						break;
					}
					PSMTXConcat(m10, m24, m28);
					PSMTXConcat(m15, m28, m28);
				}
			}
		}

		PSMTXScale(m1,
			parts->mBaseScale.x * parts->mScale.x,
			parts->mBaseScale.y * parts->mScale.y,
			flip * (parts->mBaseScale.z * parts->mScale.z));
		PSMTXIdentity(m24);
		PSMTXConcat(m28, m24, m24);
		PSMTXConcat(m1, m24, m24);
		PSMTXConcat(m3, m24, m24);
		switch (*(s8*)((char*)parts + 0x4FC)) {
		case 0:
			PSMTXConcat(m5, m24, m24);
			PSMTXConcat(m4, m24, m24);
			PSMTXConcat(m6, m24, m24);
			break;
		case 1:
			PSMTXConcat(m5, m24, m24);
			PSMTXConcat(m6, m24, m24);
			PSMTXConcat(m4, m24, m24);
			break;
		case 2:
			PSMTXConcat(m6, m24, m24);
			PSMTXConcat(m5, m24, m24);
			PSMTXConcat(m4, m24, m24);
			break;
		case 3:
			PSMTXConcat(m6, m24, m24);
			PSMTXConcat(m4, m24, m24);
			PSMTXConcat(m5, m24, m24);
			break;
		case 4:
			PSMTXConcat(m4, m24, m24);
			PSMTXConcat(m6, m24, m24);
			PSMTXConcat(m5, m24, m24);
			break;
		case 5:
			PSMTXConcat(m4, m24, m24);
			PSMTXConcat(m5, m24, m24);
			PSMTXConcat(m6, m24, m24);
			break;
		}
		PSMTXConcat(m2, m24, m24);
		PSMTXConcat(m8, m24, m27);
		PSMTXConcat(m7, m27, m27);
		PSMTXCopy(m27, m25);
		PSMTXTrans(m0, 0.0f, 0.0f, -5.0f);
		PSMTXConcat(m0, m25, m25);
	}

	*(u32*)((char*)parts + 0x218) = *(u32*)((char*)parts + 0x4F4);
	if (parts->partFlags & 0x10) {
		parts->evtColor.r = 0;
		parts->evtColor.g = 0;
		parts->evtColor.b = 0;
	}
	if (parts->partFlags & 0x20) {
		parts->evtColor.r = 0;
		parts->evtColor.g = 0;
		parts->evtColor.b = 0;
	}
	if (parts->evtColor.r == 0xFF && parts->evtColor.g == 0xFF &&
	    parts->evtColor.b == 0xFF) {
		if (BtlUnit_CheckStatus(owner, 4)) {
			parts->evtColor.r = parts->evtColor.r * 0xA0 / 0xFF;
			parts->evtColor.b = parts->evtColor.r * 0xA0 / 0xFF;
		}
		parts->evtColor.r = parts->evtColor.r * owner->mMoveColorCur / 0xFF;
		parts->evtColor.g = parts->evtColor.g * owner->mMoveColorCur / 0xFF;
		parts->evtColor.b = parts->evtColor.b * owner->mMoveColorCur / 0xFF;
	}
	animPoseSetMaterialEvtColor(parts->poseId, parts->evtColor);
	animPoseSetMaterialFlagOn(parts->poseId, 0x40);
	if (parts->dispCallback) {
		parts->dispCallback(parts, 0);
	}
	if (BtlUnit_CheckStatus(owner, 0x10)) {
		animPoseSetMaterialFlagOn(parts->poseId, 0x800000);
	} else {
		animPoseSetMaterialFlagOff(parts->poseId, 0x800000);
	}
	animPoseDrawMtx(parts->poseId, m27, 1, f30, 2.0f);
	animPoseDrawMtx(parts->poseId, m27, 2, f30, 2.0f);
	animPoseDrawMtx(parts->poseId, m27, 3, f30, 2.0f);
	if (parts->dispCallback) {
		parts->dispCallback(parts, 1);
	}
// dont_inline: retail keeps this a real bl from btlUnitPartsDisp.
#pragma dont_inline on
	btlUnitPartsBlurControl(parts, 0, m25, parts->evtColor, f30);
}
#pragma dont_inline reset
#pragma dont_inline reset

// The original's dead int->double conversion of BtlUnit_GetHeight survives as the two `stw`s of a
// constant double pair (frame 0x30). A bare `(f32)`/`(f64)` statement is DCE'd away; `opt_dead_assignments
// off` plus assigning the height to an otherwise-unused double local keeps the conversion stores.
#pragma opt_dead_assignments off
#pragma dont_inline on
void btlUnitItemDisp(CameraId cameraId, BattleUnit* unit) {
	BattleWork* wp = _battleWorkPointer;
	Vec v;
	s32 item;
	double dd;

	dd = BtlUnit_GetHeight(unit);
	item = BtlUnit_EnemyItemCanUseCheck(unit->heldItem);
	if ((wp->flags & 0x20000) && item == 0) {
		item = 0x94;
	}
	if (item != 0) {
		BtlUnit_GetPos(unit, &v.x, &v.y, &v.z);
		v.x += *(f32*)((char*)unit + 0xb4);
		v.y += *(f32*)((char*)unit + 0xb8);
		v.z += *(f32*)((char*)unit + 0xbc);
		v.x += unit->heldItemOffset.x;
		v.y += unit->heldItemOffset.y;
		v.z += unit->heldItemOffset.z;
		iconDispGx(1.0f, &v, 0, itemDataTable[item].icon_id);
	}
}
#pragma dont_inline reset
#pragma opt_dead_assignments reset

void btlUnitItemDisp2(CameraId cameraId, BattleUnit* unit) {
	BattleWork* wp = _battleWorkPointer;
	s32 kind = unit->currentKind;

	if (!((kind >= 1 && kind < 0xd8) || (kind >= 0xde && kind < 0xe0) || (kind >= 0xe0 && kind < 0xe7))) {
		return;
	}
	if (wp->flags & 0x20000) {
		btlUnitItemDisp(cameraId, unit);
	} else {
		wp->flags |= 0x20000;
		btlUnitItemDisp(cameraId, unit);
		wp->flags &= ~0x20000;
	}
}

void btlUnitStolenItemDisp(CameraId cameraId, void* param) {
	BattleUnit* unit = param;
	Vec v;
	s32 itemId = unit->heldItem;

	BtlUnit_GetPos(unit, &v.x, &v.y, &v.z);
	v.y += unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit) + 5.0f;
	iconDispGx(1.0f, &v, 0, itemDataTable[itemId].icon_id);
}

#pragma dont_inline on // the original keeps calls to this function out of line
void btlUnitPartsBlurControl(BattleUnitPart* part, s32 unused, Mtx orientation, GXColor color, f32 rotation) {

	if (part->attributes & 0x4000000) {
		_partsBlurControl(part, 1, orientation, color, rotation);
	}
	else {
		_partsBlurControl(part, 0, orientation, color, rotation);
	}
}
#pragma dont_inline reset

#pragma dont_inline on // the original keeps calls to this function out of line
void _partsBlurControl(BattleUnitPart* part, s32 flags, Mtx orientation, GXColor color, f32 rotation) {
	BattleWorkUnitPartBlur* blur;
	int i;

	blur = &part->blurWork;
	for (i = 9; i > 0; i--) { //shift all slots forward one
		blur->work[i] = blur->work[i - 1];
	}
	blur->work[0].flags = flags;
	MTXCopy(orientation, blur->work[0].orientation);
	blur->work[0].rotation = rotation;
	blur->work[0].base = color;
	*(u32*)&blur->work[0].blurColor[0] = *(u32*)&blur->color[0];
	*(u32*)&blur->work[0].blurColor[4] = *(u32*)&blur->color[4];
	if (blur->flags & 1 && blur->work[1].flags & 1) {
		blur->work[0].flags &= ~1;
	}
}
#pragma dont_inline reset

void btlUnitPartsBlurDisp(CameraId camId, BattleUnitPart* parts) {
	BattleWorkUnitPartBlur* blur = &parts->blurWork;
	BattleWorkUnitPartBlurEntry* work;
	GXColor color;
	int i;

	for (i = 0; i < 10; i++) {
		work = &blur->work[i];
		color = work->base;
		if (work->flags & 1) {
			// blurColor is loaded unsigned in the original (lbz); the header's s8 is wrong.
			color.r = color.r * (u8)(((u8*)work->blurColor)[0] * (10 - i) / 10 + ((u8*)work->blurColor)[4] * i / 10) / 0xff;
			color.g = color.g * (u8)(((u8*)work->blurColor)[1] * (10 - i) / 10 + ((u8*)work->blurColor)[5] * i / 10) / 0xff;
			color.b = color.b * (u8)(((u8*)work->blurColor)[2] * (10 - i) / 10 + ((u8*)work->blurColor)[6] * i / 10) / 0xff;
			color.a = color.a * (u8)(((u8*)work->blurColor)[3] * (10 - i) / 10 + ((u8*)work->blurColor)[7] * i / 10) / 0xff;
			if (color.a == 0) continue;
			animPoseSetMaterialEvtColor(*(s32*)((char*)parts + 0x1c0), color);
			animPoseSetMaterialFlagOn(*(s32*)((char*)parts + 0x1c0), 0x40);
			animPoseDrawMtx(parts->poseId, work->orientation, 1, work->rotation, 2.0f);
			animPoseDrawMtx(parts->poseId, work->orientation, 2, work->rotation, 2.0f);
			animPoseDrawMtx(parts->poseId, work->orientation, 3, work->rotation, 2.0f);
		}
	}
}

void btlDispEntAnime(BattleUnit* unit) {
	BattleUnitPart* part = unit->mParts;

	while (part) {
		if (part->poseTable == NULL) {
			*(s32*)((char*)part + 0x1c0) = -1;
		} else {
			const char* modelName = part->mKindPartParams->modelName;
			if (modelName == NULL) {
				*(s32*)((char*)part + 0x1c0) = -1;
			} else {
				strcpy((char*)part + 0x1c4, searchPoseTbl((BattleDispPose*)part->poseTable, 0x1c));
				*(s32*)((char*)part + 0x1c0) = animPoseEntry(modelName, 1);
			}
		}
		part = part->mNextPart;
	}
}

void btlDispChangeAnime(BattleUnitPart* part, const char* animName, BOOL reset) {
	u32* flags;

	strcpy((char*)part + 0x1c4, animName);
	flags = &part->partFlags;
	*flags |= 2;
	*flags |= 1;
	*flags &= ~0x80;
	*flags &= ~0x200;
	*flags &= ~0x10;
	if (reset) {
		*flags |= 8;
	} else {
		*flags &= ~8;
	}
}

void btlDispChangePeraAnime(BattleUnitPart* part, const char* animName) {
	part->partFlags |= 4;
	part->statusPoseKind = 0;
	animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), animName, 1);
}

void btlDispPoseAnime(BattleUnitPart* part) {
	u32* flags = &part->partFlags;

	*flags &= ~0x12;
	*flags |= 1;
	*flags &= ~0x80;
	*flags &= ~0x4;
}

#pragma dont_inline on // the original keeps calls to this function out of line
void gravityOffsetControl(BattleUnitPart* part) {
	BattleWork* wp = _battleWorkPointer;
	BOOL active = TRUE;
	BattleUnit* owner = part->mOwner;

	if (!(wp->flags & 0x40000000)) {
		active = FALSE;
	} else if (owner->mAttributeFlags & 0x1004) {
		active = FALSE;
	}
	if (active) {
		*(u16*)((char*)part + 0x1a6) += 1;
		return;
	}
	if (*(u16*)((char*)part + 0x1a6) != 0) {
		*(u16*)((char*)part + 0x1a6) %= 120;
		if (*(u16*)((char*)part + 0x1a6) >= 0x3c) {
			*(u16*)((char*)part + 0x1a6) += 1;
		} else if (*(u16*)((char*)part + 0x1a6) != 0) {
			*(u16*)((char*)part + 0x1a6) -= 1;
		}
	}
}
#pragma dont_inline reset

#pragma dont_inline on // the original keeps calls to this function out of line
f32 getGravityDispOffset(BattleUnit* unit) {
	return sinfd((f32)((*(u16*)((char*)unit + 0x1a6) * 3 + 0x10e) % 360)) * 4.0f + 4.0f;
}
#pragma dont_inline reset

#pragma dont_inline on // the original keeps calls to this function out of line
void floatOffsetControl(BattleUnitPart* part) {
	BattleUnit* owner = part->mOwner;
	BOOL active = TRUE;

	if (!(owner->flags & 0x100000)) {
		active = FALSE;
	} else if (!(owner->flags & 0x200000)) {
		active = FALSE;
	} else if (BtlUnit_CheckStatus(owner, 0x1b) != 0) {
		active = FALSE;
	}
	if (active) {
		*(u16*)((char*)part + 0x1a8) += 1;
	} else if (*(u16*)((char*)part + 0x1a8) != 0) {
		*(u16*)((char*)part + 0x1a8) %= 90;
		if (*(u16*)((char*)part + 0x1a8) >= 0x2d) {
			*(u16*)((char*)part + 0x1a8) += 1;
		} else if (*(u16*)((char*)part + 0x1a8) != 0) {
			*(u16*)((char*)part + 0x1a8) -= 1;
		}
	}
}
#pragma dont_inline reset


#pragma dont_inline on // the original keeps calls to this function out of line
f32 getFloatDispOffset(BattleUnit* unit) {
	return sinfd((f32)((*(u16*)((char*)unit + 0x1a8) * 4 + 0x10e) % 360)) * 2.0f + 2.0f;
}
#pragma dont_inline reset

// Inlined into btlDispMain, but its body is still generated here, which pools 1.0f ahead of getFloatDispOffset's
// 2.0f as retail's .sdata2 has it.
static void btlDispAnimeSpeedReset(BattleUnit* unit) {
	btlDispAnimeSpeed(unit, 1.0f);
}

#pragma dont_inline on // the original keeps calls to this function out of line
void statusPoseControl(BattleUnitPart* part) {
	switch (part->statusPoseKind) {
	case 0:
		break;
	case 2:
	case 3:
	case 7:
		_pose_one_pattern(part);
		break;
	case 4:
	case 5:
	case 6:
		_pose_two_pattern(part);
		break;
	default:
		_pose_def(part);
		break;
	}
}
#pragma dont_inline reset

#pragma dont_inline on
void _pose_def(BattleUnitPart* part) {
	// A one-case switch makes MWCC emit the original `beq body; b end` guard;
	// an early return folds to `bne end`.
	switch (part->statusPoseStep) {
	case 0:
		part->statusPoseStep = 1;
		animPoseSetEffect(*(s32*)((char*)part + 0x1c0), NULL, 0);
		if (part->mOwner->flags & 0x1000000) {
			animPoseSetMaterialLightFlagOn(*(s32*)((char*)part + 0x1c0), 8);
		}
		break;
	}
}
#pragma dont_inline reset

// _pose_one_pattern matches 100% once the strings are referenced as literals: MWCC pools
// them and common-bases the .rodata references off its first zero Vec (retail vec3_802eddd0).
#pragma dont_inline on
void _pose_one_pattern(BattleUnitPart* part) {
	// A one-case switch reproduces the original `beq body; b end` guard.
	switch (part->statusPoseStep) {
	case 0:
		part->statusPoseStep = 1;
		switch (part->statusPoseKind) {
		case 2:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_C_1A", 1);
			break;
		case 3:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_F_1A", 1);
			break;
		case 7:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_Y_1A", 1);
			break;
		}
		break;
	}
}
#pragma dont_inline reset

// _pose_two_pattern matches 100% once the strings are referenced as literals (see
// _pose_one_pattern). The three separate state-2 cases and the scoped `n` that fixes the
// mullw operand order are also required.
#pragma dont_inline on
void _pose_two_pattern(BattleUnitPart* part) {
	s32 t;

	switch (part->statusPoseStep) {
	case 0:
		part->statusPoseStep = 1;
		switch (part->statusPoseKind) {
		case 4:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_Y_1B", 1);
			part->statusPoseTimer = sysMsec2Frame(0x898);
			break;
		case 5:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_C_1A", 1);
			t = sysMsec2Frame(0x898);
			{
				s32 n = irand(4) + 6;
				part->statusPoseTimer = n * t;
			}
			break;
		case 6:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_F_2", 1);
			part->statusPoseTimer = (irand(4) + 7) * 0x16;
			break;
		}
		break;
	case 1:
		switch (part->statusPoseKind) {
		case 4:
			part->statusPoseTimer -= 1;
			if (part->statusPoseTimer == 0) {
				animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_Y_1B", 1);
				part->statusPoseTimer = sysMsec2Frame(0x898);
			}
			break;
		case 5:
			part->statusPoseTimer -= 1;
			if (part->statusPoseTimer == 0) {
				part->statusPoseStep = 2;
				animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_C_1B", 1);
				part->statusPoseTimer = 0x16;
			}
			break;
		case 6:
			part->statusPoseTimer -= 1;
			if (part->statusPoseTimer == 0) {
				part->statusPoseStep = 2;
				animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_F_1B", 1);
				part->statusPoseTimer = 0x16;
			}
			break;
		}
		break;
	case 2:
		switch (part->statusPoseKind) {
		case 4:
			part->statusPoseTimer -= 1;
			if (part->statusPoseTimer == 0) {
				part->statusPoseStep = 0;
				_pose_two_pattern(part);
			}
			break;
		case 5:
			part->statusPoseTimer -= 1;
			if (part->statusPoseTimer == 0) {
				part->statusPoseStep = 0;
				_pose_two_pattern(part);
			}
			break;
		case 6:
			part->statusPoseTimer -= 1;
			if (part->statusPoseTimer == 0) {
				part->statusPoseStep = 0;
				_pose_two_pattern(part);
			}
			break;
		}
		break;
	}
}
#pragma dont_inline reset

#pragma dont_inline on // the original keeps calls to this function out of line
void btlDispAnimeSpeed(BattleUnit* unit, f32 speed) {
	((f32*)&unit->hpGauge.field_0xC)[1] = speed;
	animPoseSetLocalTimeRate(*(s32*)((char*)unit + 0x1c0), ((f32*)&unit->hpGauge.field_0xC)[1] * ((f32*)&unit->hpGauge.field_0xC)[2]);
}
#pragma dont_inline reset

// The original pools 304.0f/240.0f/0.0f as literals; with the named externs MWCC keeps the
// multiply results in f0 and reorders the constant loads. The literals reproduce the target
// schedule exactly (the 3 remaining diffs are symbol-only and disappear when the unit links).
void btlGetScreenPoint(Vec* pos, Vec* out) {
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	Vec v;

	v.x = pos->x;
	v.y = pos->y;
	v.z = pos->z;
	PSMTXMultVec(cam->view, &v, &v);
	PSMTX44MultVec(cam->projection, &v, &v);
	out->x = 304.0f * v.x;
	out->y = 240.0f * v.y;
	out->z = 0.0f;
}

void btlDispTex4(s32 id, Vec trans, Vec scale, Vec rot, GXColor color) {
	u16 width;
	u16 height;
	f32 halfW;
	f32 halfH;
	Vec pos;

	pos.z = 0.0f;
	btlDispGetTexSize(id, &width, &height);
	halfW = 0.5f * ((f32)width * scale.x);
	halfH = 0.5f * ((f32)height * scale.y);

	pos.x = -halfW;
	pos.y = halfH;
	_btlDispTex4(id, trans, scale, rot, pos, (Vec){ 1.0f, 1.0f, 1.0f }, color);

	pos.x = halfW;
	pos.y = halfH;
	_btlDispTex4(id, trans, scale, rot, pos, (Vec){ -1.0f, 1.0f, 1.0f }, color);

	pos.x = halfW;
	pos.y = -halfH;
	_btlDispTex4(id, trans, scale, rot, pos, (Vec){ -1.0f, -1.0f, 1.0f }, color);

	pos.x = -halfW;
	pos.y = -halfH;
	_btlDispTex4(id, trans, scale, rot, pos, (Vec){ 1.0f, -1.0f, 1.0f }, color);
}

// Keep _btlDispTex4 out of line: the original calls it rather than inlining.
#pragma dont_inline on
void _btlDispTex4(s32 id, Vec trans, Vec scale, Vec rot, Vec pos2, Vec scale2, GXColor color) {
	Mtx mtx;
	Mtx transM;
	Mtx scaleM;
	Mtx rotXM;
	Mtx rotYM;
	Mtx rotZM;
	Mtx posM;
	Mtx scale2M;

	PSMTXIdentity(mtx);
	PSMTXTrans(transM, trans.x, trans.y, trans.z);
	PSMTXTrans(posM, pos2.x, pos2.y, pos2.z);
	PSMTXScale(scaleM, scale.x, scale.y, scale.z);
	PSMTXScale(scale2M, scale2.x, scale2.y, scale2.z);
	PSMTXRotRad(rotXM, 0x78, 0.017453292f * rot.x);
	PSMTXRotRad(rotYM, 0x79, 0.017453292f * rot.y);
	PSMTXRotRad(rotZM, 0x7a, 0.017453292f * rot.z);
	PSMTXConcat(scaleM, mtx, mtx);
	PSMTXConcat(scale2M, mtx, mtx);
	PSMTXConcat(posM, mtx, mtx);
	PSMTXConcat(rotZM, mtx, mtx);
	PSMTXConcat(rotXM, mtx, mtx);
	PSMTXConcat(rotYM, mtx, mtx);
	PSMTXConcat(transM, mtx, mtx);
	btlDispTexPlane2(mtx, id, color, 0);
}
#pragma dont_inline reset

// Keep BattleStockExpDisp out of line: the original calls it rather than inlining it into btlDispMain.
#pragma dont_inline on
void BattleStockExpDisp(void) {
	BattleWork* wp = _battleWorkPointer;

	if (*(s32*)((char*)wp + 0xf00) > *(s32*)((char*)wp + 0xf04)) {
		*(s32*)((char*)wp + 0xf08) += 1;
		if (*(s32*)((char*)wp + 0xf08) >= 4) {
			*(s32*)((char*)wp + 0xf04) += 1;
		}
	}
	if (wp->dispFlags & 2) {
		dispEntry(CAMERA_2D, 901.0f, 1, _btlStockExpDisp, NULL);
	}
}
#pragma dont_inline reset

void _btlStockExpDisp(CameraId cameraId, void* param) {
	static f32 y;
	static f32 x;
	BattleWork* wp = _battleWorkPointer;
	s32 exp;
	s32 value;
	s32 tens;
	s32 ones;
	s32 i;

	exp = *(s32*)((char*)wp + 0xf04);
	if (exp <= 0) {
		return;
	}
	value = exp;
	if (value > 100) {
		value = 100;
	}
	tens = value / 10;
	ones = value - tens * 10;
	for (i = 0; i < ones; i++) {
		iconDispGx(0.35f, &(Vec){(308.0f + x) - 3.5f - 13.0f * (f32)i, y - 220.0f, -100.0f}, 0x10, 0x18c);
	}
	for (i = 0; i < tens; i++) {
		iconDispGx(0.5f, &(Vec){(308.0f + x) - 10.0f - 20.0f * (f32)i + 4.0f, (y - 220.0f) + 16.0f, -100.0f}, 0x10, 0x18c);
	}
}
