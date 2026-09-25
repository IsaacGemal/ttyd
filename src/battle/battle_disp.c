#include "battle/battle_disp.h"
#include "battle/battle.h"
#include "battle/battle_menu_disp.h"
#include "battle/battle_unit.h"
#include "sdk/texPalette.h"
#include "data/item_data.h"
#include <dolphin/gx.h>
#include <string.h>


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

//local prototypes
const char* searchPoseTbl(BattleDispPose* table, s32 poseId);
static void btlDispGXInit2DSub(void);
static void btlDispTexPlainGX(u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3);
static void btlDispGetTexSize(u32 id, u16* width, u16* height);
static void _pose_def(BattleUnitPart* part);
static void _pose_one_pattern(BattleUnitPart* part);
static void _pose_two_pattern(BattleUnitPart* part);
static void _partsBlurControl(BattleUnitPart* part, s32 flags, Mtx orientation, GXColor color, f32 rotation);

//.rodata
static Vec vec3_802eddd0 = { 0.0f, 0.0f, 0.0f };

extern s32 animPoseSetLocalTimeRate(s32 poseId, f32 localTimeRate);
extern void animPoseSetEffectAnim(s32 poseId, const char* animName, BOOL reset);
extern void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag);
extern s32 animPoseEntry(const char* animName, s32 type);
extern void animPoseSetMaterialLightFlagOn(s32 poseId, u32 mask);
extern s32 sysMsec2Frame(s32 msec);
extern s32 irand(s32 max);
extern f32 sinfd(f32 deg);
extern double double_to_int_802ede28;
extern volatile f32 float_4_8041bf3c;
extern volatile f32 float_2_8041bf68;
extern f32 float_0_8041bf58;
extern f32 float_1_8041bf64;
extern f32 float_0p5_8041bf4c;
extern f32 float_304_8041bf5c;
extern f32 float_240_8041bf60;
extern const char str_802EDE38[];
extern const char str_802EDE40[];
extern const char str_802EDE48[];
extern const char str_802EDE50[];
extern const char str_802EDE58[];
extern const char str_802EDE60[];
extern const char str_802EDE68[];
extern const char str_802EDE70[];

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
	GXPosition3f32(x, y, float_0_8041bf58);
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
	GXPosition3f32(x, y, float_0_8041bf58);
	GXColor4u8(r, g, b, a);
	GXTexCoord2f32(s, t);
}
#pragma dont_inline reset

void btlDispGXQuads2D(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	btlDispGXPoint2D(x0, y0, r, g, b, a, float_0_8041bf58, float_0_8041bf58);
	btlDispGXPoint2D(x1, y0, r, g, b, a, float_1_8041bf64, float_0_8041bf58);
	btlDispGXPoint2D(x1, y1, r, g, b, a, float_1_8041bf64, float_1_8041bf64);
	btlDispGXPoint2D(x0, y1, r, g, b, a, float_0_8041bf58, float_1_8041bf64);
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

// NONMATCHING (98.05%): target uses the named compiler-pool constants double_to_int_mask_802ede30/float_0_8041bf58 and the `beq then; b else` layout; ours are anonymous @N@ and `bne`.
void btlDispTexPlainGX(u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3) {
	GXTexObj texobj;
	f32 x, y;
	
	TEXGetGXTexObjFromPalette(*_battleWorkPointer->menuTex->data, &texobj, id);
	GXLoadTexObj(&texobj, GX_TEXMAP0);
	if (GXGetTexObjFmt(&texobj) == 0) {
		GXSetNumTevStages(1);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	}
	else {
		GXSetNumTevStages(1);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
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
void btlDispTexPlane2(Mtx mtx, u32 id, GXColor color) {
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

void btlDispGetTexSize(u32 id, u16* width, u16* height) {
	GXTexObj texobj;

	TEXGetGXTexObjFromPalette(*_battleWorkPointer->menuTex->data, &texobj, id);
	*width = GXGetTexObjWidth(&texobj);
	*height = GXGetTexObjHeight(&texobj);
}

void btlDispInit(void) {
	BattleWork* wp = _battleWorkPointer;
	
	battleCameraInit();
	battleMenuDispInit();
	wp->dispFlags |= 2;
}

// NONMATCHING (97.93%): target sign-extends the BtlUnit_CheckStatus arg (`extsb r4,r30`); `mr r3,r29` order also differs.
StatusEffectType _GetStatusPoseType(BattleUnit* unit) {
	s32* table = _status_pose_table;
	StatusEffectType type;

	if (unit->mAttributeFlags & 0x20000) {
		return STATUS_INSTANT_KILL;
	}
	while (1) {
		type = *table;
		if (*table == -1) break;
		if (BtlUnit_CheckStatus(unit, type)) {
			return type;
		}
		table++;
	}
	return STATUS_INVALID;
}

void btlDispMain(void) {
	BattleUnit* unit;
	int i;

	for (i = 0; i < 0x40; i++) {
		unit = BattleGetUnitPtr(_battleWorkPointer, i);
		if (unit) {
			if (unit->currentKind == UNIT_SHELL_SHIELD) {

			}
		}
	}
}

void btlUnitPartsDisp(CameraId camId, BattleUnitPart* parts) {

}








void btlDispPoseAnime(BattleUnitPart* part) {
	s32* flags = (s32*)((char*)part + 0x204);

	*flags &= ~0x12;
	*flags |= 1;
	*flags &= ~0x80;
	*flags &= ~0x4;
}

void btlDispChangePeraAnime(BattleUnitPart* part, const char* animName) {
	*(s32*)((char*)part + 0x204) |= 4;
	*(u8*)((char*)part + 0x215) = 0;
	animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), animName, 1);
}

void btlDispChangeAnime(BattleUnitPart* part, const char* animName, BOOL reset) {
	s32* flags;

	strcpy((char*)part + 0x1c4, animName);
	flags = (s32*)((char*)part + 0x204);
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

// NONMATCHING (95.93%): body matches; target emits `beq then; b else`, ours `bne else`.
#pragma dont_inline on
static void _pose_def(BattleUnitPart* part) {
	s32 f = *(u8*)((char*)part + 0x214);

	if (f) {
	} else {
		*(u8*)((char*)part + 0x214) = 1;
		animPoseSetEffect(*(s32*)((char*)part + 0x1c0), NULL, 0);
		if (part->mOwner->flags & 0x1000000) {
			animPoseSetMaterialLightFlagOn(*(s32*)((char*)part + 0x1c0), 8);
		}
	}
}

// NONMATCHING (76.30%): target keeps the string base in r31 and `part` in r30 and uses `beq then; b end`; ours folds each string into its own @ha/@l and uses `bne`.
static void _pose_one_pattern(BattleUnitPart* part) {
	s32 f = *(u8*)((char*)part + 0x214);

	if (f) return;
	*(u8*)((char*)part + 0x214) = 1;
	switch (*(u8*)((char*)part + 0x215)) {
	case 2:
		animPoseSetEffect(*(s32*)((char*)part + 0x1c0), str_802EDE38, 1);
		animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), str_802EDE48, 1);
		break;
	case 3:
		animPoseSetEffect(*(s32*)((char*)part + 0x1c0), str_802EDE38, 1);
		animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), str_802EDE68, 1);
		break;
	case 7:
		animPoseSetEffect(*(s32*)((char*)part + 0x1c0), str_802EDE38, 1);
		animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), str_802EDE70, 1);
		break;
	}
}

// NONMATCHING (66.92%): nested-if dispatch on byte 0x215 matches semantically; register shape differs.
void statusPoseControl(BattleUnitPart* part) {
	s32 s = *(u8*)((char*)part + 0x215);

	if (s < 4) {
		if (s == 1) {
			_pose_def(part);
		} else if (s >= 2) {
			_pose_one_pattern(part);
		} else if (s >= 0) {
		} else {
			_pose_def(part);
		}
	} else {
		if (s == 7) {
			_pose_one_pattern(part);
		} else if (s >= 7) {
			_pose_def(part);
		} else {
			_pose_two_pattern(part);
		}
	}
}
#pragma dont_inline reset

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

void btlDispAnimeSpeed(BattleUnit* unit, f32 speed) {
	((f32*)&unit->hpGauge.field_0xC)[1] = speed;
	animPoseSetLocalTimeRate(*(s32*)((char*)unit + 0x1c0), ((f32*)&unit->hpGauge.field_0xC)[1] * ((f32*)&unit->hpGauge.field_0xC)[2]);
}

// NONMATCHING (88.55%): target hoists both display-constant loads and products; our multiply/store schedule differs.
void btlGetScreenPoint(Vec* pos, Vec* out) {
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	Vec v;

	v.x = pos->x;
	v.y = pos->y;
	v.z = pos->z;
	PSMTXMultVec(cam->view, &v, &v);
	PSMTX44MultVec(cam->projection, &v, &v);
	out->x = float_304_8041bf5c * v.x;
	out->y = float_240_8041bf60 * v.y;
	out->z = float_0_8041bf58;
}

// NONMATCHING (93.20%): only the 2^52 constant reloc name differs (target: double_to_int_802ede28, ours: anonymous @N@).
f32 getGravityDispOffset(BattleUnit* unit) {
	return float_4_8041bf3c * sinfd((f32)((*(u16*)((char*)unit + 0x1a6) * 3 + 0x10e) % 360)) + float_4_8041bf3c;
}

// NONMATCHING (93.20%): only the 2^52 constant reloc name differs (target: double_to_int_802ede28, ours: anonymous @N@).
f32 getFloatDispOffset(BattleUnit* unit) {
	return float_2_8041bf68 * sinfd((f32)((*(u16*)((char*)unit + 0x1a8) * 4 + 0x10e) % 360)) + float_2_8041bf68;
}

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


void btlUnitPartsBlurControl(BattleUnitPart* part, s32 unused, Mtx orientation, GXColor color, f32 rotation) {

	if (part->attributes & 0x4000000) {
		_partsBlurControl(part, 1, orientation, color, rotation);
	}
	else {
		_partsBlurControl(part, 0, orientation, color, rotation);
	}
}

// NONMATCHING (88.31%): color copy fixed; remaining diffs are loop address arithmetic / register allocation.
#pragma dont_inline on
void _partsBlurControl(BattleUnitPart* part, s32 flags, Mtx orientation, GXColor color, f32 rotation) {
	BattleWorkUnitPartBlur* blur;
	int i;

	i = 9;
	blur = &part->blurWork;
	do { //shift all slots forward one
		blur->work[i] = blur->work[i - 1];
	} while (--i > 0);
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

extern f32 float_901_8041bf50;
void _btlStockExpDisp(CameraId cameraId, void* param);

void BattleStockExpDisp(void) {
	BattleWork* wp = _battleWorkPointer;

	if (*(s32*)((char*)wp + 0xf00) > *(s32*)((char*)wp + 0xf04)) {
		*(s32*)((char*)wp + 0xf08) += 1;
		if (*(s32*)((char*)wp + 0xf08) >= 4) {
			*(s32*)((char*)wp + 0xf04) += 1;
		}
	}
	if (wp->dispFlags & 2) {
		dispEntry(CAMERA_2D, float_901_8041bf50, 1, _btlStockExpDisp, NULL);
	}
}

extern ItemData itemDataTable[];
extern f32 float_5_8041bf6c;
extern s32 BtlUnit_EnemyItemCanUseCheck(s32 itemId);
void btlUnitItemDisp(CameraId cameraId, BattleUnit* unit);

// NONMATCHING (46.00%): MWCC copies the Vec into the iconDispGx arg slot; target reuses the BtlUnit_GetPos slot. Height conversion also emits extsh vs target xoris.
void btlUnitStolenItemDisp(CameraId cameraId, BattleUnit* unit) {
	Vec v;
	s32 itemId = unit->heldItem;

	BtlUnit_GetPos(unit, &v.x, &v.y, &v.z);
	v.y += unit->mSizeMultiplier * (f32)(u32)BtlUnit_GetHeight(unit) + float_5_8041bf6c;
	iconDispGx(v, 0, itemDataTable[itemId].icon_id, float_1_8041bf64);
}

// NONMATCHING (96.86%): one branch-layout instruction (target `blt body; b end`, ours `bge end`).
void btlUnitItemDisp2(CameraId cameraId, BattleUnit* unit) {
	BattleWork* wp = _battleWorkPointer;
	s32 kind = unit->currentKind;

	if ((kind >= 1 && kind < 0xd8) || (kind >= 0xde && kind < 0xe0) || (kind >= 0xe0 && kind < 0xe7)) {
		if (wp->flags & 0x20000) {
			btlUnitItemDisp(cameraId, unit);
		} else {
			wp->flags |= 0x20000;
			btlUnitItemDisp(cameraId, unit);
			wp->flags &= ~0x20000;
		}
	}
}

// NONMATCHING (63.40%): target allocates the Vec in the iconDispGx arg slot; ours copies it. The dead (f32)(u32)GetHeight conversion is also dropped by our optimizer.
#pragma dont_inline on
void btlUnitItemDisp(CameraId cameraId, BattleUnit* unit) {
	BattleWork* wp = _battleWorkPointer;
	Vec v;
	s32 item;

	(f32)(u32)BtlUnit_GetHeight(unit);
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
		iconDispGx(v, 0, itemDataTable[item].icon_id, float_1_8041bf64);
	}
}
#pragma dont_inline reset

extern f32 float_deg2rad_8041bf54;

// NONMATCHING (98.90%): exact except the caller does not emit the trailing `li r6,0` for the by-value GXColor argument to btlDispTexPlane2.
void _btlDispTex4(s32 id, Vec* trans, Vec* scale, Vec* rot, Vec* pos2, Vec* scale2, GXColor* color) {
	Mtx mtx;
	Mtx transM;
	Mtx scaleM;
	Mtx rotXM;
	Mtx rotYM;
	Mtx rotZM;
	Mtx posM;
	Mtx scale2M;

	PSMTXIdentity(mtx);
	PSMTXTrans(transM, trans->x, trans->y, trans->z);
	PSMTXTrans(posM, pos2->x, pos2->y, pos2->z);
	PSMTXScale(scaleM, scale->x, scale->y, scale->z);
	PSMTXScale(scale2M, scale2->x, scale2->y, scale2->z);
	PSMTXRotRad(rotXM, 0x78, float_deg2rad_8041bf54 * rot->x);
	PSMTXRotRad(rotYM, 0x79, float_deg2rad_8041bf54 * rot->y);
	PSMTXRotRad(rotZM, 0x7a, float_deg2rad_8041bf54 * rot->z);
	PSMTXConcat(scaleM, mtx, mtx);
	PSMTXConcat(scale2M, mtx, mtx);
	PSMTXConcat(posM, mtx, mtx);
	PSMTXConcat(rotZM, mtx, mtx);
	PSMTXConcat(rotXM, mtx, mtx);
	PSMTXConcat(rotYM, mtx, mtx);
	PSMTXConcat(transM, mtx, mtx);
	btlDispTexPlane2(mtx, id, *color);
}
