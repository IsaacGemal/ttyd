#include "eff/eff_nice.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

extern GlobalWork* gp;

// acrobatMain mixes the two local Vecs: `disp` is zeroed then overwritten from the
// first entry, and a second `position` local is copied from it before dispCalcZ.
// `disp`'s zero Vec literal is this unit's first .rodata entry (retail vec3_802f3748, .rodata:0xb0); the
// compiler pools the entries behind it at vec3_802f3754 (0xbc), "SFX_SYSTEM_ACROBAT1" (0xc8) and
// double_to_int (0xe0).

// .sdata2 named float constants
extern const f32 float_0p017453_8041cba4;
extern f32 float_20_8041cba8;
extern f32 float_0p5_8041cbac;
extern const GXColor dat_8041cba0;
// effNiceMain's fade thresholds and per-type colours
extern const GXColor dat_8041cb90;
extern const GXColor dat_8041cb94;
extern const GXColor dat_8041cb98;
extern const GXColor dat_8041cb9c;

//.sdata
const char* anim_jp[] = {
	"EFF_nice",
	"EFF_good",
	"EFF_great",
	"EFF_wonderful",
	"EFF_excellent",
	NULL,
	"EFF_lucky"
};
const char* anim_us[] = {
	"EFF_nice_us",
	"EFF_good_us",
	"EFF_great_us",
	"EFF_wonderful_us",
	"EFF_excellent_us",
	NULL,
	"EFF_lucky_us"
};
static const char** anim_tbl[2] = { anim_jp, anim_us };
s32 acrobat_jp[] = {179, 180, 181, 182, 183, 184, 185};
s32 acrobat_us[] = {183, 184, 185, 186, 187, 188}; //includes nonexistant textures
s32* acrobat_tbl[2] = { acrobat_jp, acrobat_us };
// 4-byte zero word that ends the unit's .sdata; needs explicit_zero_data to stay in the section.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80411CDC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

//forward declarations
static void rendermodeFunc(s32 wXluStage);

// The declaration order matters: `valid` must be declared after `i` so the
// compiler gives valid r28 and i r29, matching the original.
BOOL effNiceAsync(s32 group) {
	const char* name;
	int i;
	BOOL valid;

	valid = TRUE;
	for (i = 0; i < 7; i++) {
		name = anim_tbl[gp->language][i];
		if (name && !animGroupBaseAsync(name, group, NULL)) {
			valid = FALSE;
		}
	}
	return valid;
}

static void rendermodeFunc(s32 wXluStage) {
	AnimationWork* wp = animGetPtr();

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	wp->currDispMode = wXluStage;
}

// 100% match: MWCC emits the `beq body; b else` shape for a single-case switch (not the if/else
// `bne else`), so both type==7 blocks are switches; the language test is unsigned (`(u32)`), and
// the animPoseEntry boolean is a ternary (`? 1 : 0`), which lands the gp/anim_tbl temporaries in
// the original registers (a `!= 0` there gives the same bool with them swapped).
EffectEntry* effNiceEntry(s32 type, f32 x, f32 y, f32 z) {
	NiceEffectWork *userdata, *ptr;
	EffectEntry* effect;
	u32 size, count;
	int i;

	effect = effEntry();
	if (type == 5) { //guard for unused slot
		type = 0;
	}
	switch (type) {
	case 7:
		if ((u32)gp->language == 0) {
			count = 8;
		}
		else {
			count = 7;
		}
		break;
	default:
		count = 1;
		break;
	}
	effect->type = "Nice";
	size = sizeof(NiceEffectWork) * count;
	effect->count = count;
	userdata = __memAlloc(HEAP_EFFECT, size);
	effect->userdata = userdata;
	memset(userdata, 0, size);
	effect->callback = effNiceMain;
	effect->flags |= 2;
	userdata->type = type;
	userdata->position.x = x;
	userdata->position.y = y;
	userdata->position.z = z;
	userdata->scaleX = 1.0f;
	userdata->scaleY = 1.0f;
	userdata->scaleZ = 1.0f;
	userdata->scale = 1.0f;
	userdata->poseId = -1;
	switch (type) { //initialize other entries
	case 7:
		ptr = userdata + 1;
		for (i = 1; i < effect->count; i++, ptr++) {
			ptr->position.x = 1000.0f;
			ptr->position.y = 0.0f;
			ptr->position.z = 0.0f;
			ptr->scaleX = 1.0f;
			ptr->scaleY = 1.0f;
			ptr->scaleZ = 1.0f;
			ptr->state = 0;
			ptr->stateTimer = 0;
		}
		break;
	default:
		userdata->poseId = animPoseEntry(anim_tbl[gp->language][type], gp->inBattle ? 1 : 0);
		animPoseSetAnim(userdata->poseId, "A_1", TRUE);
		animPoseSetMaterialFlagOn(userdata->poseId, 0x1800);
		animPoseSetGXFunc(userdata->poseId, rendermodeFunc, FALSE);
		break;
	}
	return effect;
}

// effNiceMain fades the material colour of the pose as the animation nears its end.
// The animation end time is read through a pointer at AnimData+0x24 (offset 8) and the
// current time from AnimationPose+0x24; both offsets fall in header padding, so raw
// offsets are used. The four fade cases use distinct locals (time/end/base and time2...)
// because a single shared set makes MWCC put `end` in f3 instead of the original f4.
#ifdef MUST_MATCH
// MUST_MATCH: the plain `color = ...; color.a = ...; call(color);` puts the colour scratch
// and its outgoing argument copy in the opposite stack slots; returning the colour by value
// from this helper reproduces retail's slots.
static inline GXColor niceEvtColor(GXColor color, u8 alpha) {
	color.a = alpha;
	return color;
}
#endif
void effNiceMain(EffectEntry* effect) {
	NiceEffectWork* data = effect->userdata;
	AnimationPose* pose;
	AnimData* animData;
	Vec position;
	Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 type;
	f32 time, end, base, alpha;
	f32 time2, end2, base2;
	f32 time3, end3, base3;
	f32 time4, end4, base4;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->poseId != -1) {
			animPoseRelease(data->poseId);
		}
		effDelete(effect);
		return;
	}
	else if (data->poseId != -1) {
		pose = animPoseGetAnimPosePtr(data->poseId);
		animData = animPoseGetAnimDataPtr(data->poseId);
		animPoseMain(data->poseId);
		switch (type) {
		case 0:
		case 1:
			end = *(f32*)((u8*)(void*)animData->clip + 8);
			base = (u32)gp->language == 0 ? 32.0f : 34.0f;
			time = pose->clipTime;
			if (time > base) {
				alpha = 1.0f - (time - base) / (end - base);
				if (alpha < 0.0f) {
					alpha = 0.0f;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb90, (u8)(s32)(255.0f * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb90;
					color.a = (u8)(s32)(255.0f * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 2:
			end2 = *(f32*)((u8*)(void*)animData->clip + 8);
			base2 = (u32)gp->language == 0 ? 35.0f : 37.0f;
			time2 = pose->clipTime;
			if (time2 > base2) {
				alpha = 1.0f - (time2 - base2) / (end2 - base2);
				if (alpha < 0.0f) {
					alpha = 0.0f;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb94, (u8)(s32)(255.0f * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb94;
					color.a = (u8)(s32)(255.0f * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 3:
			end3 = *(f32*)((u8*)(void*)animData->clip + 8);
			base3 = (u32)gp->language == 0 ? 45.0f : 40.f;
			time3 = pose->clipTime;
			if (time3 > base3) {
				alpha = 1.0f - (time3 - base3) / (end3 - base3);
				if (alpha < 0.0f) {
					alpha = 0.0f;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb98, (u8)(s32)(255.0f * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb98;
					color.a = (u8)(s32)(255.0f * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 4:
			end4 = *(f32*)((u8*)(void*)animData->clip + 8);
			base4 = (u32)gp->language == 0 ? 48.0f : 40.f;
			time4 = pose->clipTime;
			if (time4 > base4) {
				alpha = 1.0f - (time4 - base4) / (end4 - base4);
				if (alpha < 0.0f) {
					alpha = 0.0f;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb9c, (u8)(s32)(255.0f * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb9c;
					color.a = (u8)(s32)(255.0f * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 6:
			break;
		}
		if (animPoseGetLoopTimes(data->poseId) >= 1.0f) {
			animPoseRelease(data->poseId);
			effDelete(effect);
			return;
		}
	}
	if (type == 7) {
		acrobatMain(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 8, effNiceDisp, effect);
	}
}

void effNiceDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	NiceEffectWork* data = effect->userdata;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxFinal;

	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_0p017453_8041cba4);
	PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtxTrans, mtxRot, mtxFinal);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
	PSMTXScale(mtxScale, 8.0f, 8.0f, 8.0f);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
	animPoseDrawMtx(data->poseId, mtxFinal, 1, 0.0f, 1.0f);
	animPoseDrawMtx(data->poseId, mtxFinal, 2, 0.0f, 1.0f);
	animPoseDrawMtx(data->poseId, mtxFinal, 3, 0.0f, 1.0f);
}

// The control flow, values and jump table all match. The int->float magic in case 2 lands in f2 and
// the lfs loads are in the original order.
// The two ramp values are written as the loop counter's own `i * 4` / `i * 0xC` (4 at i == 1, then
// +4 / +0xC per step: the same progression as the named `x`/`y` locals the disassembly suggests).
// As expressions MWCC keeps them in strength-reduced compiler temps, which is what colours them
// r31/r30. The `s32` round trip on the string at its only use pins the address materialisation into
// the string's own callee-saved register (`addi r29, r3, str_802F3760@l`); a plain use leaves an
// `addi r0`/`mr r29` scratch copy, and the comma form that removes that copy colours the string r31.
void acrobatMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it into a second local
	   before dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0 };
	const char* sfx = "SFX_SYSTEM_ACROBAT1";
	NiceEffectWork* data = effect->userdata;
	s32 i;
	s32 done = 0;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		switch (data->state) {
		case 0:
			data->stateTimer = i * 4;
			data->state++;
			if (i == 1) {
#ifdef MUST_MATCH
				// MUST_MATCH: the s32 round trip pins the direct address materialisation (see above).
				psndSFXOn((const char*)(s32)sfx);
#else
				psndSFXOn(sfx);
#endif
			}
			break;
		case 1:
			data->stateTimer--;
			if (data->stateTimer < 0) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 2:
			data->stateTimer++;
			data->position.x = intplGetValue(0xC, data->stateTimer, 0x1E, 500.f, (f32)(i * 4));
			if (data->stateTimer >= 0x1E) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 3:
			data->stateTimer++;
			data->scaleX = intplGetValue(0xC, data->stateTimer, 8, 1.0f, 0.4f);
			data->scaleY = intplGetValue(0xC, data->stateTimer, 8, 1.0f, 1.5f);
			if (data->stateTimer >= 8) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 4:
			data->stateTimer++;
			if (data->stateTimer > (effect->count - i - 1) * 4) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 5:
			data->stateTimer++;
			data->position.x = intplGetValue(8, data->stateTimer, 0x1E, (f32)(i * 4), (f32)(i * 0xC));
			data->scaleX = intplGetValue(0xC, data->stateTimer, 0x1E, 0.2f, 1.0f);
			data->scaleY = intplGetValue(0xC, data->stateTimer, 0x1E, 1.5f, 1.0f);
			if (data->stateTimer >= 0x1E) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 6:
			data->stateTimer++;
			if (data->stateTimer > 0x10) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 7:
			data->stateTimer++;
			data->scaleX = intplGetValue(0xC, data->stateTimer, 0xF, 1.0f, 1.5f);
			data->scaleY = intplGetValue(0xC, data->stateTimer, 0xF, 1.0f, 0.1f);
			if (data->stateTimer >= 0xF) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 8:
			data->stateTimer++;
			data->scaleX = intplGetValue(0xC, data->stateTimer, 0xF, 1.5f, 0.1f);
			data->scaleY = intplGetValue(0xC, data->stateTimer, 0xF, 0.1f, 2.0f);
			if (data->stateTimer >= 0xF) {
				data->stateTimer = 0;
				data->state++;
			}
			break;
		case 9:
			data->scaleZ = 0.0f;
			data->scaleY = 0.0f;
			data->scaleX = 0.0f;
			done++;
			break;
		}
	}
	if (done >= effect->count - 1) {
		effDelete(effect);
	}
	else {
#ifdef MUST_MATCH
		dispEntry(CAMERA_3D, dispCalcZ(&position), 8, effAcrobatDisp, effect);
#else
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 8, effAcrobatDisp, effect);
#endif
	}
}

// The `i - 1` texid index makes the compiler synthesise a second induction (r29). With the natural
// `[i - 1]` its latch increment is scheduled after data++ (99.91%); an explicit texIdx gives the
// original's order.
void effAcrobatDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	NiceEffectWork* data = effect->userdata;
	CameraEntry* camera = camGetPtr(camId);
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxFinal;
	GXTexObj texObj;
	s32 i;
#ifdef MUST_MATCH
	s32 texIdx;
#endif
	f32 max, negSize, min;
	f32 offsetX, offsetY;

	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_0p017453_8041cba4);
	PSMTXConcat(mtxTrans, mtxRot, mtxFinal);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetChanMatColor(GX_COLOR0A0, dat_8041cba0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	negSize = -float_20_8041cba8;
	min = negSize * float_0p5_8041cbac;
#ifndef MUST_MATCH
	max = 10.0f;
#endif
	i = 1;
#ifdef MUST_MATCH
	texIdx = 1;
#endif
	data = data + 1;
#ifdef MUST_MATCH
	for (; i < effect->count; i++, texIdx++, data++) {
#else
	for (; i < effect->count; i++, data++) {
#endif
		if (data->state < 6) {
			offsetX = negSize * float_0p5_8041cbac;
			offsetY = 0.0f;
		}
		else {
			offsetX = 0.0f;
			offsetY = negSize * float_0p5_8041cbac;
		}
#ifdef MUST_MATCH
		// The original pools 10.0f after the loop's own 0.0f, so assign it here; the loop-invariant
		// load is still hoisted into the preheader. The plain form is assigned before the loop.
		max = 10.f;
#endif
		PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
		PSMTXTrans(mtxRot, offsetX, offsetY, 0.0f);
		PSMTXScale(mtxScale, data->scaleX, data->scaleY, data->scaleZ);
		PSMTXConcat(mtxRot, mtxScale, mtxScale);
		PSMTXTrans(mtxRot, -offsetX, -offsetY, 0.0f);
		PSMTXConcat(mtxScale, mtxRot, mtxScale);
		PSMTXConcat(mtxTrans, mtxScale, mtxScale);
		PSMTXConcat(mtxFinal, mtxScale, mtxScale);
		PSMTXConcat(camera->view, mtxScale, mtxScale);
		GXLoadPosMtxImm(mtxScale, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

#ifdef MUST_MATCH
		effGetTexObj(acrobat_tbl[gp->language][texIdx - 1], &texObj);
#else
		effGetTexObj(acrobat_tbl[gp->language][i - 1], &texObj);
#endif
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(min, max, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(max, max, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(max, min, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(min, min, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

// The unit's named .sdata2 objects, in the original's order, defined after the code so a
// const object is not folded into a literal at its uses.
const GXColor dat_8041cb90 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041cb94 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041cb98 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041cb9c = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041cba0 = { 0xFF, 0xFF, 0xFF, 0xFF };

const f32 float_0p017453_8041cba4 = 0.017453292f;
__declspec(section ".sdata2") f32 float_20_8041cba8 = 20.0f;
__declspec(section ".sdata2") f32 float_0p5_8041cbac = 0.5f;
