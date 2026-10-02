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


//.rodata
const Vec vec3_802eddd0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802edddc = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802edde8 = { -1.0f, 1.0f, 1.0f };
const Vec vec3_802eddf4 = { -1.0f, -1.0f, 1.0f };
const Vec vec3_802ede00 = { 1.0f, -1.0f, 1.0f };

extern double double_to_int_802ede28;
extern f32 float_4_8041bf3c;
extern f32 float_2_8041bf68;
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
extern f32 float_deg2rad_8041bf54;
extern f32 float_270_8041bf70;
extern f32 float_90_8041bf74;
extern f32 float_neg1_8041bf78;
extern f32 float_180_8041bf7c;
extern f32 float_neg5_8041bf80;
typedef void (*BattleUnitPartDispCallback)(BattleUnitPart* part, s32 mode);

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

// Writing the quad through a local `volatile PPCWGPipe*` puts the GX FIFO base in r12 as the target does (and
// `ny` keeps -y in f2); the remaining diffs are only which caller-saved register holds each colour byte
// (target: r0/r3/r6/r7, r4/r5/r10/r11, r6/r7/r8/r9, r0/r3/r4/r5). The `switch` keeps the target's
// `beq then; b else` layout.
// NONMATCHING (99.22%): only GX colour-byte temp registers differ; opcode sequence is identical.
// Tried without a win: a GXColor4u8-style 4-scalar-argument inline helper (and all 24 argument permutations)
// reproduces the target's colour0 registers exactly but forces the FIFO base into r5 instead of r12; a color
// pointer helper, local GXColor copies, per-byte helpers, a 16-parameter whole-quad helper, explicit
// `u8`/array temporaries, direct `GXWGFifo` accesses, and every relevant opt_* pragma (opt_lifetimes,
// opt_common_subs, opt_propagation, opt_pointer_analysis, ...) all leave one of {base r12, colour registers}
// wrong. The 1.0f/double pool-symbol names differ but do not affect the score.
void btlDispTexPlainGX(u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3) {
	GXTexObj texobj;
	f32 x, y;
	
	TEXGetGXTexObjFromPalette(*_battleWorkPointer->menuTex->data, &texobj, id);
	GXLoadTexObj(&texobj, GX_TEXMAP0);
#ifdef MUST_MATCH
	/* MUST_MATCH: a switch reproduces the original `beq then; b else` layout; if/else folds to `bne`. */
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
#else
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
#endif

	x = 0.5f * (f32)GXGetTexObjWidth(&texobj);
	y = 0.5f * (f32)GXGetTexObjHeight(&texobj);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
	/* MUST_MATCH: writing through this local pointer reproduces the target's r12 GX FIFO base. */
	{
		volatile PPCWGPipe* fifo = &GXWGFifo;
		f32 nx = -x, ny = -y;

		fifo->f32 = nx;
		fifo->f32 = y;
		fifo->f32 = 0.0f;
		fifo->u8 = color0.r;
		fifo->u8 = color0.g;
		fifo->u8 = color0.b;
		fifo->u8 = color0.a;
		fifo->f32 = 0.0f;
		fifo->f32 = 0.0f;

		fifo->f32 = x;
		fifo->f32 = y;
		fifo->f32 = 0.0f;
		fifo->u8 = color1.r;
		fifo->u8 = color1.g;
		fifo->u8 = color1.b;
		fifo->u8 = color1.a;
		fifo->f32 = 1.0f;
		fifo->f32 = 0.0f;

		fifo->f32 = x;
		fifo->f32 = ny;
		fifo->f32 = 0.0f;
		fifo->u8 = color2.r;
		fifo->u8 = color2.g;
		fifo->u8 = color2.b;
		fifo->u8 = color2.a;
		fifo->f32 = 1.0f;
		fifo->f32 = 1.0f;

		fifo->f32 = nx;
		fifo->f32 = ny;
		fifo->f32 = 0.0f;
		fifo->u8 = color3.r;
		fifo->u8 = color3.g;
		fifo->u8 = color3.b;
		fifo->u8 = color3.a;
		fifo->f32 = 0.0f;
		fifo->f32 = 1.0f;
	}
#else
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
#endif
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
StatusEffectType _GetStatusPoseType(BattleUnit* unit) {
	s32* table = _status_pose_table;
	StatusEffectType type;

	if (unit->mAttributeFlags & 0x20000) {
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

// NONMATCHING (1.93%): 3 instructions differ, 725 missing, plus register differences.
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

#pragma opt_common_subs off // keeps the faceDirection int->float conversion in retail's f1/f2/f0 registers
// All code, calls and matrix/switch structure match; remaining diffs are register allocation only. The
// target keeps owner in r29 / wp in r31 (we have them swapped) and maps the six callee-saved floats to
// f26-f31 in the "natural" order (f26=1.0, f27=270/90, f28=pos.z, f29=pos.x, f30=height, f31=angle),
// while MWCC assigns ours in a different permutation. The three callees are kept as real `bl`s with
// #pragma dont_inline because the target does not inline them. MWCC gives owner r31 / wp r29 and puts
// height in f27 and the 270/90 constant in f30; every x/z/1.0/angle and stack slot already matches. The
// f32 declaration order and the prologue load order below are chosen so the allocator lands as close as
// it can - reordering them moved several dozen instructions into the target's registers.
// NONMATCHING (98.82%): only the callee-saved register assignment (owner/wp GPR swap and the height/270
// FPR swap) plus the prologue schedule differ. parts->mOwner reloads at the flag tests, the rotation-sum
// grouping, FP operand order, the float_270 initializer and opt_common_subs off moved the rest into place.
void btlUnitPartsDisp(CameraId camId, BattleUnitPart* parts) {
	BattleWork* wp;
	BattleUnit* owner;
	CameraEntry* cam;
	// Order matters for MWCC's callee-saved FPR allocation (do not sort).
	f32 f27 = float_270_8041bf70;
	f32 f28;
	f32 f29;
	f32 f30;
	f32 f26;
	f32 f31;
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
	owner = parts->mOwner;
	f26 = float_1_8041bf64;
	wp = _battleWorkPointer;
	if (*(s32*)((char*)parts + 0x1C0) < 0) {
		return;
	}

	PSMTXTrans(m11, -owner->mRotationOffset.x, -owner->mRotationOffset.y, -owner->mRotationOffset.z);
	PSMTXTrans(m10, owner->mRotationOffset.x, owner->mRotationOffset.y, owner->mRotationOffset.z);
	PSMTXTrans(m15, owner->mDisplayOffset.x, owner->mDisplayOffset.y, owner->mDisplayOffset.z);
	PSMTXTrans(m16, owner->mPosition.x, owner->mPosition.y, owner->mPosition.z);
	PSMTXRotRad(m14, 'x', float_deg2rad_8041bf54 * (owner->mRotation.x + owner->mBaseRotation.x));
	PSMTXRotRad(m13, 'y', float_deg2rad_8041bf54 * (owner->mRotation.y + owner->mBaseRotation.y));
	PSMTXRotRad(m12, 'z', float_deg2rad_8041bf54 * (owner->mRotation.z + owner->mBaseRotation.z));
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


	if (parts->attributes & 0x10000000) {
		if (parts->faceDirection >= 0) {
			f27 = float_90_8041bf74;
		}
		f29 = parts->mPosition.x;
		f30 = getFloatDispOffset((BattleUnit*)parts);
// dont_inline: retail keeps these real bls from btlUnitPartsDisp.
#pragma dont_inline on
		f30 = (parts->mPosition.y + getGravityDispOffset((BattleUnit*)parts)) + f30;
		f28 = parts->mPosition.z;
		cam = camGetCurPtr();
		f31 = angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z);
		f31 = reviseAngle(f31 - angleABf(cam->cameraPos.x, cam->cameraPos.z, f29, f28));

		PSMTXTrans(m22, -parts->mRotationOffset.x, -parts->mRotationOffset.y, -parts->mRotationOffset.z);
		PSMTXTrans(m21, parts->mRotationOffset.x, parts->mRotationOffset.y, parts->mRotationOffset.z);
		PSMTXRotRad(m20, 'x', float_deg2rad_8041bf54 * (parts->mRotation.x + parts->mBaseRotation.x));
		PSMTXRotRad(m19, 'y', float_deg2rad_8041bf54 * (parts->mRotation.y + parts->mBaseRotation.y + f31));
		PSMTXRotRad(m18, 'z', float_deg2rad_8041bf54 * (parts->mRotation.z + parts->mBaseRotation.z));
		PSMTXTrans(m23, f29, f30, f28);

		f30 = reviseAngle(float_neg1_8041bf78 * f27 - float_90_8041bf74);
		f28 = reviseAngle(parts->mBaseRotation.y + (f30 + parts->mRotation.y));
		cam = camGetCurPtr();
		f27 = angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z);
		{
			f32 f0 = reviseAngle(f28 + f27);
			if (f0 >= float_90_8041bf74 && f0 <= float_270_8041bf70) {
				f26 = f26 * float_neg1_8041bf78;
				PSMTXRotRad(m19, 'y', float_deg2rad_8041bf54 *
					(f31 + (parts->mRotation.y + parts->mBaseRotation.y) -
					 float_2_8041bf68 * (float_180_8041bf7c - (f0 - f27))));
			}
		}

		if (parts->attributes & 0x08000000) {
			PSMTXScale(m26,
				parts->mBaseScale.x * parts->mScale.x,
				parts->mBaseScale.y * parts->mScale.y,
				f26 * (parts->mBaseScale.z * parts->mScale.z));
		} else {
			PSMTXScale(m26,
				owner->mSizeMultiplier * (parts->mBaseScale.x * parts->mScale.x),
				owner->mSizeMultiplier * (parts->mBaseScale.y * parts->mScale.y),
				f26 * (parts->mBaseScale.z * parts->mScale.z));
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
		PSMTXTrans(m0, float_0_8041bf58, float_0_8041bf58, float_neg5_8041bf80);
		PSMTXConcat(m0, m25, m25);
	} else {
		f32 dir;
		f32 sign;

		if (owner->faceDirection >= 0) {
			f27 = float_90_8041bf74;
		}
		dir = (f32)parts->faceDirection;
		if (dir > float_0_8041bf58) {
			sign = float_1_8041bf64;
		} else if (dir < float_0_8041bf58) {
			sign = float_neg1_8041bf78;
		} else {
			sign = (f32)*(s8*)((char*)wp + owner->mAlliance * 8 + 0xA);
		}
		f30 = sign * parts->mPositionOffset.x + owner->mPosition.x;
		f26 = getFloatDispOffset((BattleUnit*)parts);
		f31 = (owner->mPosition.y + parts->mPositionOffset.y + getGravityDispOffset((BattleUnit*)parts)) + f26;
		f29 = owner->mPosition.z + parts->mPositionOffset.z;
		f28 = float_0_8041bf58;
		if (parts->mOwner->mAttributeFlags & 0x04000000) {
			cam = camGetCurPtr();
			f26 = angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z);
			f28 = reviseAngle(f26 - angleABf(cam->cameraPos.x, cam->cameraPos.z, f30, f29));
		}

		PSMTXTrans(m3, -parts->mRotationOffset.x, -parts->mRotationOffset.y, -parts->mRotationOffset.z);
		PSMTXTrans(m2, parts->mRotationOffset.x, parts->mRotationOffset.y, parts->mRotationOffset.z);
		PSMTXRotRad(m6, 'x', float_deg2rad_8041bf54 * (parts->mRotation.x + parts->mBaseRotation.x));
		PSMTXRotRad(m5, 'y', float_deg2rad_8041bf54 * (parts->mRotation.y + parts->mBaseRotation.y + f28));
		PSMTXRotRad(m4, 'z', float_deg2rad_8041bf54 * (parts->mRotation.z + parts->mBaseRotation.z));
		PSMTXTrans(m8, f30, f31, f29);
		PSMTXTrans(m7, parts->mDisplayOffset.x, parts->mDisplayOffset.y, parts->mDisplayOffset.z);

		f30 = reviseAngle(float_neg1_8041bf78 * f27 - float_90_8041bf74);
		if (!(parts->mOwner->flags & 0x1000000)) {
			f26 = reviseAngle(parts->mBaseRotation.y + (owner->mBaseRotation.y +
				(parts->mRotation.y + (f30 + owner->mRotation.y))));
			cam = camGetCurPtr();
			{
				f32 f0 = reviseAngle(f26 + angleABf(cam->cameraPos.x, cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.z));
				if (f0 >= float_90_8041bf74 && f0 <= float_270_8041bf70) {
					PSMTXScale(m9,
						owner->mSizeMultiplier * (owner->mBaseScale.x * owner->mScale.x),
						owner->mSizeMultiplier * (owner->mBaseScale.y * owner->mScale.y),
						float_neg1_8041bf78 * (owner->mBaseScale.z * owner->mScale.z));
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
			float_1_8041bf64 * (parts->mBaseScale.z * parts->mScale.z));
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
		PSMTXTrans(m0, float_0_8041bf58, float_0_8041bf58, float_neg5_8041bf80);
		PSMTXConcat(m0, m25, m25);
	}

	*(u32*)((char*)parts + 0x218) = *(u32*)((char*)parts + 0x4F4);
	if (*(s32*)((char*)parts + 0x204) & 0x10) {
		*(u8*)((char*)parts + 0x218) = 0;
		*(u8*)((char*)parts + 0x219) = 0;
		*(u8*)((char*)parts + 0x21A) = 0;
	}
	if (*(s32*)((char*)parts + 0x204) & 0x20) {
		*(u8*)((char*)parts + 0x218) = 0;
		*(u8*)((char*)parts + 0x219) = 0;
		*(u8*)((char*)parts + 0x21A) = 0;
	}
	if (*(u8*)((char*)parts + 0x218) == 0xFF && *(u8*)((char*)parts + 0x219) == 0xFF &&
	    *(u8*)((char*)parts + 0x21A) == 0xFF) {
		if (BtlUnit_CheckStatus(owner, 4)) {
			*(u8*)((char*)parts + 0x218) = *(u8*)((char*)parts + 0x218) * 0xA0 / 0xFF;
			*(u8*)((char*)parts + 0x21A) = *(u8*)((char*)parts + 0x218) * 0xA0 / 0xFF;
		}
		*(u8*)((char*)parts + 0x218) = *(u8*)((char*)parts + 0x218) * *(u8*)((char*)owner + 0x30E) / 0xFF;
		*(u8*)((char*)parts + 0x219) = *(u8*)((char*)parts + 0x219) * *(u8*)((char*)owner + 0x30E) / 0xFF;
		*(u8*)((char*)parts + 0x21A) = *(u8*)((char*)parts + 0x21A) * *(u8*)((char*)owner + 0x30E) / 0xFF;
	}
	animPoseSetMaterialEvtColor(*(s32*)((char*)parts + 0x1C0), *(GXColor*)((char*)parts + 0x218));
	animPoseSetMaterialFlagOn(*(s32*)((char*)parts + 0x1C0), 0x40);
	if (*(BattleUnitPartDispCallback*)((char*)parts + 0x210)) {
		(*(BattleUnitPartDispCallback*)((char*)parts + 0x210))(parts, 0);
	}
	if (BtlUnit_CheckStatus(owner, 0x10)) {
		animPoseSetMaterialFlagOn(*(s32*)((char*)parts + 0x1C0), 0x800000);
	} else {
		animPoseSetMaterialFlagOff(*(s32*)((char*)parts + 0x1C0), 0x800000);
	}
	animPoseDrawMtx(*(s32*)((char*)parts + 0x1C0), m27, 1, f30, float_2_8041bf68);
	animPoseDrawMtx(*(s32*)((char*)parts + 0x1C0), m27, 2, f30, float_2_8041bf68);
	animPoseDrawMtx(*(s32*)((char*)parts + 0x1C0), m27, 3, f30, float_2_8041bf68);
	if (*(BattleUnitPartDispCallback*)((char*)parts + 0x210)) {
		(*(BattleUnitPartDispCallback*)((char*)parts + 0x210))(parts, 1);
	}
// dont_inline: retail keeps this a real bl from btlUnitPartsDisp.
#pragma dont_inline on
	btlUnitPartsBlurControl(parts, 0, m25, *(GXColor*)((char*)parts + 0x218), f30);
}

#pragma opt_common_subs reset








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

#pragma dont_inline on
void _pose_def(BattleUnitPart* part) {
#ifdef MUST_MATCH
	/* MUST_MATCH: a one-case switch makes MWCC emit the original `beq body; b end` guard;
	   the natural early-return folds to `bne end`. */
	switch (*(u8*)((char*)part + 0x214)) {
	case 0:
		*(u8*)((char*)part + 0x214) = 1;
		animPoseSetEffect(*(s32*)((char*)part + 0x1c0), NULL, 0);
		if (part->mOwner->flags & 0x1000000) {
			animPoseSetMaterialLightFlagOn(*(s32*)((char*)part + 0x1c0), 8);
		}
		break;
	}
#else
	if (*(u8*)((char*)part + 0x214)) return;
	*(u8*)((char*)part + 0x214) = 1;
	animPoseSetEffect(*(s32*)((char*)part + 0x1c0), NULL, 0);
	if (part->mOwner->flags & 0x1000000) {
		animPoseSetMaterialLightFlagOn(*(s32*)((char*)part + 0x1c0), 8);
	}
#endif
}

// _pose_one_pattern matches 100% once the strings are referenced as literals: MWCC pools
// them and common-bases the .rodata references off vec3_802eddd0.
void _pose_one_pattern(BattleUnitPart* part) {
#ifdef MUST_MATCH
	/* MUST_MATCH: one-case switch reproduces the original `beq body; b end` guard. */
	switch (*(u8*)((char*)part + 0x214)) {
	case 0:
		*(u8*)((char*)part + 0x214) = 1;
		switch (*(u8*)((char*)part + 0x215)) {
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
#else
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
#endif
}

// _pose_two_pattern matches 100% once the strings are referenced as literals (see
// _pose_one_pattern). The three separate state-2 cases and the scoped `n` that fixes the
// mullw operand order are also required.
#pragma dont_inline on
void _pose_two_pattern(BattleUnitPart* part) {
	s32 t;

	switch (*(u8*)((char*)part + 0x214)) {
	case 0:
		*(u8*)((char*)part + 0x214) = 1;
		switch (*(u8*)((char*)part + 0x215)) {
		case 4:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_Y_1B", 1);
			*(u16*)((char*)part + 0x216) = sysMsec2Frame(0x898);
			break;
		case 5:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_C_1A", 1);
			t = sysMsec2Frame(0x898);
			{
				s32 n = irand(4) + 6;
				*(u16*)((char*)part + 0x216) = n * t;
			}
			break;
		case 6:
			animPoseSetEffect(*(s32*)((char*)part + 0x1c0), "p_b_st", 1);
			animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_F_2", 1);
			*(u16*)((char*)part + 0x216) = (irand(4) + 7) * 0x16;
			break;
		}
		break;
	case 1:
		switch (*(u8*)((char*)part + 0x215)) {
		case 4:
			*(u16*)((char*)part + 0x216) -= 1;
			if (*(u16*)((char*)part + 0x216) == 0) {
				animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_Y_1B", 1);
				*(u16*)((char*)part + 0x216) = sysMsec2Frame(0x898);
			}
			break;
		case 5:
			*(u16*)((char*)part + 0x216) -= 1;
			if (*(u16*)((char*)part + 0x216) == 0) {
				*(u8*)((char*)part + 0x214) = 2;
				animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_C_1B", 1);
				*(u16*)((char*)part + 0x216) = 0x16;
			}
			break;
		case 6:
			*(u16*)((char*)part + 0x216) -= 1;
			if (*(u16*)((char*)part + 0x216) == 0) {
				*(u8*)((char*)part + 0x214) = 2;
				animPoseSetEffectAnim(*(s32*)((char*)part + 0x1c0), "PM_F_1B", 1);
				*(u16*)((char*)part + 0x216) = 0x16;
			}
			break;
		}
		break;
	case 2:
		switch (*(u8*)((char*)part + 0x215)) {
		case 4:
			*(u16*)((char*)part + 0x216) -= 1;
			if (*(u16*)((char*)part + 0x216) == 0) {
				*(u8*)((char*)part + 0x214) = 0;
				_pose_two_pattern(part);
			}
			break;
		case 5:
			*(u16*)((char*)part + 0x216) -= 1;
			if (*(u16*)((char*)part + 0x216) == 0) {
				*(u8*)((char*)part + 0x214) = 0;
				_pose_two_pattern(part);
			}
			break;
		case 6:
			*(u16*)((char*)part + 0x216) -= 1;
			if (*(u16*)((char*)part + 0x216) == 0) {
				*(u8*)((char*)part + 0x214) = 0;
				_pose_two_pattern(part);
			}
			break;
		}
		break;
	}
}
#pragma dont_inline reset

void statusPoseControl(BattleUnitPart* part) {
	switch (*(u8*)((char*)part + 0x215)) {
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

f32 getGravityDispOffset(BattleUnit* unit) {
	return sinfd((f32)((*(u16*)((char*)unit + 0x1a6) * 3 + 0x10e) % 360)) * 4.0f + 4.0f;
}

f32 getFloatDispOffset(BattleUnit* unit) {
	return sinfd((f32)((*(u16*)((char*)unit + 0x1a8) * 4 + 0x10e) % 360)) * 2.0f + 2.0f;
}
#pragma dont_inline reset

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
			animPoseDrawMtx(*(s32*)((char*)parts + 0x1c0), work->orientation, 1, work->rotation, float_2_8041bf68);
			animPoseDrawMtx(*(s32*)((char*)parts + 0x1c0), work->orientation, 2, work->rotation, float_2_8041bf68);
			animPoseDrawMtx(*(s32*)((char*)parts + 0x1c0), work->orientation, 3, work->rotation, float_2_8041bf68);
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
#pragma dont_inline reset

#pragma dont_inline on
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

extern f32 float_901_8041bf50;

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

// .rodata
static volatile const Vec vec3_802ede0c = { 0.0f, 0.0f, -100.0f };
static volatile const Vec vec3_802ede18 = { 0.0f, 0.0f, -100.0f };

// .sbss
static f32 y_1185;
static f32 x_1184;

// Declaring these explicitly made the compiler emit a second copy of its u32->f32 conversion
// doubles in .rodata, which shifted the pooled strings by 8 bytes and broke every function that
// common-bases .rodata. The compiler already emits both doubles
#if 0
double double_to_int_802ede28 = 4503601774854144.0;
#endif

// NONMATCHING (91.05%): instructions differ; see check.py
void _btlStockExpDisp(CameraId cameraId, void* param) {
	BattleWork* wp = _battleWorkPointer;
	s32 exp;
	s32 value;
	s32 tens;
	s32 ones;
	s32 i;
	Vec pos1;
	Vec pos2;

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
		pos1 = vec3_802ede0c;
		pos1.x = (308.0f + x_1184) - 3.5f - 13.0f * (f32)i;
		pos1.y = y_1185 - 220.0f;
		iconDispGx(&pos1, 0x10, 0x18c, 0.35f);
	}
	for (i = 0; i < tens; i++) {
		pos2 = vec3_802ede18;
		pos2.x = (308.0f + x_1184) - 10.0f - 20.0f * (f32)i + 4.0f;
		pos2.y = (y_1185 - 220.0f) + 16.0f;
		iconDispGx(&pos2, 0x10, 0x18c, 0.5f);
	}
}

extern ItemData itemDataTable[];
extern f32 float_5_8041bf6c;

// NONMATCHING (91.46%): 2 instructions differ, 2 missing, 2 extra, plus register differences.
void btlUnitStolenItemDisp(CameraId cameraId, void* param) {
	BattleUnit* unit = param;
	Vec v;
	s32 itemId = unit->heldItem;

	BtlUnit_GetPos(unit, &v.x, &v.y, &v.z);
	v.y += unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit) + float_5_8041bf6c;
	iconDispGx(&v, 0, itemDataTable[itemId].icon_id, float_1_8041bf64);
}

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

// The original's dead int->double conversion of BtlUnit_GetHeight survives as the two `stw`s of a
// constant double pair (frame 0x30). A bare `(f32)`/`(f64)` statement is DCE'd away; assigning
// the height to an otherwise-unused double local keeps the conversion stores.
#pragma dont_inline on
// NONMATCHING (91.57%): only the iconDispGx arg setup differs -- target computes &itemDataTable[item] in
// r4 and reads icon_id `lhz` late; we use r3 / `lha` early.
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
		iconDispGx(&v, 0, itemDataTable[item].icon_id, float_1_8041bf64);
	}
}
#pragma dont_inline reset

// See the note on double_to_int_802ede28 above.
#if 0
const double double_to_int_mask_802ede30 = 4503599627370496.0;
#endif

// The original passes the vectors and colour by value to an inlined wrapper, which copies
// them into the _btlDispTex4 argument slots. Keep the wrapper's body a plain pass-through.
// NONMATCHING (86.14%): the target holds every argument value in registers before storing it
// into the argument block (r14-r25), but the compiler here interleaves the loads and stores
// unless the match-only copies below force the same register pressure. The extra copies add
// 0x30 of frame (0x1d0 vs the target's 0x1a0) and the parameter registers come out r15/r22-r26
// instead of the target's r26-r30; the conversion double is also at .rodata+0x58 instead of +0x60.
static inline void _btlDispTex4Sub(s32 id, Vec trans, Vec scale, Vec rot, Vec pos, Vec scale2, GXColor color) {
	_btlDispTex4(id, &trans, &scale, &rot, &pos, &scale2, &color);
}

// NONMATCHING (86.14%): see the note on the helper above. Everything under MUST_MATCH is a
// match-only copy sequence; the natural form is the #else branch.
void btlDispTex4(s32 id, Vec* trans, Vec* scale, Vec* rot, GXColor* color) {
#ifdef MUST_MATCH
	Vec new_var3;
	u16 width;
	u16 height;
	Vec new_var7;
	int new_var6;
	Vec new_var4;
	Vec new_var;
	f32 halfW;
	f32 halfH;
	Vec new_var5;
	Vec pos;
	Vec new_var2;

	pos.z = float_0_8041bf58;
	btlDispGetTexSize(id, &width, &height);
	new_var6 = color != NULL;
	halfW = float_0p5_8041bf4c * ((f32)width * scale->x);
	halfH = (f32)height * scale->y;
	halfH = float_0p5_8041bf4c * halfH;

	pos.x = -halfW;
	new_var3 = *scale;
	new_var7 = *trans;
	// match-only: the repeated test keeps `color` live in a callee-saved register like the target
	if ((new_var6 && new_var6) && new_var6) {
	}
	new_var4 = new_var7;
	pos.y = halfH;
	new_var5 = *rot;
	new_var = new_var5;
	_btlDispTex4Sub(id, new_var7, new_var3, *rot, pos, vec3_802edddc, *color);

	pos.x = halfW;
	new_var2 = new_var4;
	pos.y = halfH;
	_btlDispTex4Sub(id, new_var2, new_var3, new_var, pos, vec3_802edde8, *color);

	pos.x = halfW;
	pos.y = -halfH;
	_btlDispTex4Sub(id, *trans, new_var3, new_var, pos, vec3_802eddf4, *color);

	pos.x = -halfW;
	pos.y = -halfH;
	_btlDispTex4Sub(id, new_var4, new_var3, new_var5, pos, vec3_802ede00, *color);
#else
	u16 width;
	u16 height;
	f32 halfW;
	f32 halfH;
	Vec pos;

	pos.z = float_0_8041bf58;
	btlDispGetTexSize(id, &width, &height);
	halfW = float_0p5_8041bf4c * ((f32)width * scale->x);
	halfH = float_0p5_8041bf4c * ((f32)height * scale->y);

	pos.x = -halfW;
	pos.y = halfH;
	_btlDispTex4Sub(id, *trans, *scale, *rot, pos, vec3_802edddc, *color);

	pos.x = halfW;
	pos.y = halfH;
	_btlDispTex4Sub(id, *trans, *scale, *rot, pos, vec3_802edde8, *color);

	pos.x = halfW;
	pos.y = -halfH;
	_btlDispTex4Sub(id, *trans, *scale, *rot, pos, vec3_802eddf4, *color);

	pos.x = -halfW;
	pos.y = -halfH;
	_btlDispTex4Sub(id, *trans, *scale, *rot, pos, vec3_802ede00, *color);
#endif
}

// Keep _btlDispTex4 out of line: the original calls it rather than inlining.
#pragma dont_inline on
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
	btlDispTexPlane2(mtx, id, *color, 0);
}
#pragma dont_inline reset
