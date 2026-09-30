#include "eff/eff_sandars.h"
#include "eff/n64/eff_bomb_n64.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// .sdata2 constants (definitions live in the retail object, referenced by name).
extern const u32 dat_80421b98;
// MUST_MATCH: volatile forces the per-vertex reloads the original has; the objects aren't volatile.
#ifdef MUST_MATCH
#define MATCH_VOLATILE volatile
#else
#define MATCH_VOLATILE
#endif
extern const MATCH_VOLATILE u32 dat_80421b9c;
extern const f32 float_deg2rad_80421ba0;
extern const f32 float_32_80421ba4;
extern const f32 float_0p5_80421ba8;
extern const f32 float_16_80421bac;
extern const MATCH_VOLATILE f32 float_0_80421bb0;
extern const MATCH_VOLATILE f32 float_1_80421bb4;
extern const f32 float_4_80421bb8;
extern const f32 float_100_80421bbc;
const char str_80421BC0[] = "Z_1";
extern const f32 float_6p2832_80421bc4;
extern const f32 float_360_80421bc8;
extern const f32 float_0p2_80421bcc;
extern const f32 float_51_80421bd0;
extern const f32 float_1p1_80421bd4;
extern const f32 float_15_80421bd8;

// .rodata
const Vec vec3_80300138 = { 0.0f, 0.0f, 0.0f };
const char str_80300144[16] = "EFF_bakuhatu";
const f64 double_to_int_mask_80300158 = 4503599627370496.0;
const char str_80300160[8] = "Sandars";
const f64 double_to_int_80300168 = 4503601774854144.0;

// .sdata
static u8 scale_data[16] = {
	0x00, 0x00, 0x00, 0x0D,
	0x1C, 0x2C, 0x3C, 0x4B,
	0x59, 0x65, 0x6F, 0x75,
	0x78, 0x4B, 0x00, 0x00,
};

#ifdef MUST_MATCH
/* MUST_MATCH: returning the colour through an inline aggregate forces MWCC to build it in the return
   temporary (r1+8) and copy it to the by-value argument slot (r1+0xC), as retail does. */
static inline GXColor effSandarsColorAnim(u8 alpha) {
	GXColor color = *(GXColor*)&dat_80421b98;
	color.a = alpha;
	return color;
}

static inline GXColor effSandarsColorDisp2(u8 alpha) {
	GXColor color = *(GXColor*)&dat_80421b9c;
	color.a = alpha;
	return color;
}
#endif

// Under MUST_MATCH the zero literal stays `0.0f` rather than `float_0_80421bb0` on purpose: `zero` is volatile for
// effSandarsDisp2, and the literal lets MWCC treat it as rematerializable, which lands the loop-invariant
// loads in retail's f27/f28/f29 (f27=0, f28=float_0p2, f29=float_51). It pools as an anonymous @NNN@ whose
// bytes equal float_0_80421bb0; the int->float magic likewise pools anonymously instead of
// double_to_int_80300168. Both relocations score the same in objdiff.
EffectEntry* effSandarsEntry(s32 type, f32 x, f32 y, f32 z, f32 a) {
	EffectEntry* entry;
	effSandarsData* data;
	BOOL inBattle;
	s32 count;
	s32 i;
	f32 angle;
	f32 zero;

	entry = effEntry();
	inBattle = gp->inBattle != 0;
	count = 1;
	if (type == 5) {
		count = 6;
	}
	entry->type = str_80300160;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effSandarsData));
	entry->userdata = data;
	entry->callback = effSandarsMain;
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = float_1_80421bb4;
	data->state = 0;
	data->timer = 0;
	data->poseId = -1;
	data->camId = 4;

	switch (type) {
	case 0:
	case 4:
		data->bombType = 1;
		data->animScale = float_1_80421bb4;
		break;
	case 1:
	case 2:
	case 3:
		data->bombType = 2;
		data->animScale = float_4_80421bb8;
		break;
	case 5:
		data->timer = 0x64;
		data->field_0x30 = a;
		break;
	}

	if (data->type == 3 || data->type == 4) {
		animGroupBaseAsync(str_80300144, inBattle, NULL);
		data->poseId = -1;
		data->alpha = 0xff;
	}

	angle = (float_6p2832_80421bc4 * a) / float_360_80421bc8;
#ifdef MUST_MATCH
	zero = 0.0f;
#else
	zero = float_0_80421bb0;
#endif
	data = &data[1];
	for (i = 1; i < entry->count; i++, data++) {
		data->x = (f32)(i * 10) * (f32)cos(angle);
		data->y = (f32)(i * 10) * (f32)sin(angle);
		data->z = zero;
#ifdef MUST_MATCH
		data->scaleRate = float_0p2_80421bcc * (f32)i;
		data->scale = zero;
#else
		data->scale = zero;
		data->scaleRate = float_0p2_80421bcc * (f32)i;
#endif
		data->alpha = (s32)(float_51_80421bd0 * (f32)i);
		data->field_0x34 = (i - 1) * 5;
		data->timer = 0;
		data->rot = zero;
		data->rotSpeed = zero;
		if (i == entry->count - 1) {
			data->scaleRate = float_1p1_80421bd4;
			data->alpha = 0xCC;
			data->rotSpeed = float_15_80421bd8;
		}
	}

	return entry;
}

// NONMATCHING (95.44%): the `type == 5` test emits `bne` where retail emits `beq; b`, the vec3 copy is
// scheduled before the `userdata` load, one `mr` arg move is reversed, and the unsigned int->float magic is
// pooled as `@300` rather than `double_to_int_mask_80300158`.
void effSandarsMain(EffectEntry* effect) {
	effSandarsData* data = effect->userdata;
	Vec position;
	Vec disp;
	s32 camId;
	BOOL inBattle;
	s32 type;
	s32 i;

	disp = vec3_80300138;
	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	inBattle = gp->inBattle != 0;
	type = data->type;
	camId = data->camId;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->poseId != -1) {
			animPoseRelease(data->poseId);
		}
		effDelete(effect);
		return;
	}

	if (type == 5) {
		data->timer--;
		if (data->timer < 0) {
			effDelete(effect);
			return;
		}
		data = &data[1];
		for (i = 1; i < effect->count; i++, data++) {
			if (data->field_0x34 != 0) {
				data->field_0x34--;
			} else {
				if ((u32)data->timer < 0xF) {
					data->scale = (f32)scale_data[data->timer] / float_100_80421bbc;
				}
				data->timer++;
				data->rot += data->rotSpeed;
			}
		}
		dispEntry(camId, dispCalcZ(&position), 2, effSandarsDisp2, effect);
	} else {
		switch (data->state) {
		case 0:
			effBombN64SetCamId(effBombN64Entry(data->bombType, data->x, data->y, data->z), camId);
			data->timer = 0;
			data->state++;
			break;
		case 1:
			if (type < 3) {
				effDelete(effect);
				return;
			}
			if (data->timer != 0) {
				data->timer--;
				break;
			}
			if (animGroupBaseAsync(str_80300144, inBattle, NULL)) {
				data->poseId = animPoseEntry(str_80300144, inBattle);
				animPoseSetAnim(data->poseId, str_80421BC0, 1);
				data->state++;
			}
			break;
		case 2:
			data->alpha -= 8;
			if (data->alpha < 0) {
				animPoseRelease(data->poseId);
				data->poseId = -1;
				effDelete(effect);
				return;
			}
			break;
		}

		dispEntry(camId, dispCalcZ(&position), 2, effSandarsDisp, effect);
	}
}

void effSandarsDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSandarsData* data = effect->userdata;
#ifndef MUST_MATCH
	GXColor color;
#endif
	Mtx mtxA0;
	Mtx mtx70;
	Mtx mtx40;
	Mtx mtx10;

	camGetPtr(camId);
	if (data->poseId == -1) {
		return;
	}

	PSMTXTrans(mtxA0, data->x, data->y, data->z);
	PSMTXRotRad(mtx70, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80421ba0);
	PSMTXScale(mtx40, data->scale, data->scale, data->scale);
	PSMTXConcat(mtxA0, mtx70, mtxA0);
	PSMTXConcat(mtxA0, mtx40, mtx10);
	PSMTXScale(mtx40, data->animScale, data->animScale, data->animScale);
	PSMTXConcat(mtx10, mtx40, mtx10);
	animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
	animPoseSetMaterialEvtColor(data->poseId, effSandarsColorAnim(data->alpha));
#else
	color = *(GXColor*)&dat_80421b98;
	color.a = data->alpha;
	animPoseSetMaterialEvtColor(data->poseId, color);
#endif
	animPoseMain(data->poseId);
	animPoseDrawMtx(data->poseId, mtx10, 1, float_0_80421bb0, float_4_80421bb8);
	animPoseDrawMtx(data->poseId, mtx10, 2, float_0_80421bb0, float_4_80421bb8);
	animPoseDrawMtx(data->poseId, mtx10, 3, float_0_80421bb0, float_4_80421bb8);
}

void effSandarsDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSandarsData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
#ifndef MUST_MATCH
	GXColor color;
#endif
	Mtx mtxC0;
	Mtx mtx90;
	Mtx mtx60;
	Mtx mtx30;
	s32 i;
	f32 deg;
	f32 negSize;
	f32 posSize;

	cam = camGetPtr(camId);
	PSMTXTrans(mtxC0, data->x, data->y, data->z);
	PSMTXRotRad(mtx90, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80421ba0);
	PSMTXScale(mtx60, data->scale, data->scale, data->scale);
	PSMTXConcat(mtxC0, mtx90, mtxC0);
	PSMTXConcat(mtxC0, mtx60, mtxC0);
	PSMTXConcat(cam->view, mtxC0, mtx30);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	effGetTexObj(0x44, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

#ifdef MUST_MATCH
	/* MUST_MATCH: retail materialises the quad half-size through the 32 and 0.5 pool entries. */
	deg = float_deg2rad_80421ba0;
	posSize = float_16_80421bac;
	negSize = -float_32_80421ba4 * float_0p5_80421ba8;
#else
	deg = float_deg2rad_80421ba0;
	posSize = 16.0f;
	negSize = -16.0f;
#endif

	data = &data[1];
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(mtxC0, data->x, data->y, data->z);
		PSMTXRotRad(mtx90, 'z', deg * data->rot);
		PSMTXScale(mtx60, data->scale * data->scaleRate, data->scale * data->scaleRate, data->scale * data->scaleRate);
		PSMTXConcat(mtxC0, mtx90, mtxC0);
		PSMTXConcat(mtxC0, mtx60, mtxC0);
		PSMTXConcat(mtx30, mtxC0, mtxC0);
		GXLoadPosMtxImm(mtxC0, 0);
		GXSetCurrentMtx(0);
#ifdef MUST_MATCH
		GXSetChanMatColor(GX_COLOR0A0, effSandarsColorDisp2(data->alpha));
#else
		color = *(GXColor*)&dat_80421b9c;
		color.a = data->alpha;
		GXSetChanMatColor(GX_COLOR0A0, color);
#endif
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(negSize, posSize, float_0_80421bb0);
		GXTexCoord2f32(float_0_80421bb0, float_0_80421bb0);
		GXPosition3f32(posSize, posSize, float_0_80421bb0);
		GXTexCoord2f32(float_1_80421bb4, float_0_80421bb0);
		GXPosition3f32(posSize, negSize, float_0_80421bb0);
		GXTexCoord2f32(float_1_80421bb4, float_1_80421bb4);
		GXPosition3f32(negSize, negSize, float_0_80421bb0);
		GXTexCoord2f32(float_0_80421bb0, float_1_80421bb4);
	}
}
