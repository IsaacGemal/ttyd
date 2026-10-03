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
// vec3_802f3748 is this unit's data (retail .rodata:0xb0); the compiler pools the zero
// for `disp` and the int->float 2^52 double right behind it, so define it to keep those
// pool entries at vec3_802f3754 (0xbc), str_802F3760 (0xc8) and double_to_int (0xe0).
extern const f32 float_500_8041cbbc;
extern const f32 float_0p4_8041cbc0;
extern const f32 float_1p5_8041cbc4;
extern const f32 float_0p2_8041cbc8;
extern const f32 float_0p1_8041cbcc;
extern const f32 float_2_8041cbd0;

// .sdata2 named float constants
extern const f32 float_0p017453_8041cba4;
extern f32 float_20_8041cba8;
extern f32 float_0p5_8041cbac;
extern f32 float_0_8041cbb0;
extern f32 float_10_8041cbb4;
extern f32 float_1_8041cbb8;
extern const f32 float_8_8041cbd4;
extern const GXColor dat_8041cba0;
// effNiceMain's fade thresholds and per-type colours
extern const f32 float_32_8041cbd8;
extern const f32 float_34_8041cbdc;
extern const f32 float_255_8041cbe0;
extern const f32 float_35_8041cbe4;
extern const f32 float_37_8041cbe8;
extern const f32 float_45_8041cbec;
extern const f32 float_40_8041cbf0;
extern const f32 float_48_8041cbf4;
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
const char** anim_tbl[2] = { anim_jp, anim_us };
s32 acrobat_jp[] = {179, 180, 181, 182, 183, 184, 185};
s32 acrobat_us[] = {183, 184, 185, 186, 187, 188}; //includes nonexistant textures
s32* acrobal_tbl[2] = { acrobat_jp, acrobat_us };

// retail effNiceMain's zero Vec (it is not decompiled yet); defined here for the .rodata layout.
const Vec vec3_802f3748 = { 0.0f, 0.0f, 0.0f };

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
	userdata->field_0x10 = 1.0f;
	userdata->field_0x14 = 1.0f;
	userdata->field_0x18 = 1.0f;
	userdata->field_0x1C = 1.0f;
	userdata->poseId = -1;
	switch (type) { //initialize other entries
	case 7:
		ptr = userdata + 1;
		for (i = 1; i < effect->count; i++, ptr++) {
			ptr->position.x = 1000.0f;
			ptr->position.y = 0.0f;
			ptr->position.z = 0.0f;
			ptr->field_0x10 = 1.0f;
			ptr->field_0x14 = 1.0f;
			ptr->field_0x18 = 1.0f;
			ptr->field_0x28 = 0;
			ptr->field_0x20 = 0;
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
#pragma opt_common_subs off // without it MWCC CSEs the per-case `end` loads and puts `end` in f3 instead of f4
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
	Vec disp = vec3_802f3748;
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
			end = *(f32*)((u8*)*(void**)((u8*)animData + 0x24) + 8);
			base = (u32)gp->language == 0 ? float_32_8041cbd8 : float_34_8041cbdc;
			time = *(f32*)((u8*)pose + 0x24);
			if (time > base) {
				alpha = float_1_8041cbb8 - (time - base) / (end - base);
				if (alpha < float_0_8041cbb0) {
					alpha = float_0_8041cbb0;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb90, (u8)(s32)(float_255_8041cbe0 * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb90;
					color.a = (u8)(s32)(float_255_8041cbe0 * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 2:
			end2 = *(f32*)((u8*)*(void**)((u8*)animData + 0x24) + 8);
			base2 = (u32)gp->language == 0 ? float_35_8041cbe4 : float_37_8041cbe8;
			time2 = *(f32*)((u8*)pose + 0x24);
			if (time2 > base2) {
				alpha = float_1_8041cbb8 - (time2 - base2) / (end2 - base2);
				if (alpha < float_0_8041cbb0) {
					alpha = float_0_8041cbb0;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb94, (u8)(s32)(float_255_8041cbe0 * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb94;
					color.a = (u8)(s32)(float_255_8041cbe0 * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 3:
			end3 = *(f32*)((u8*)*(void**)((u8*)animData + 0x24) + 8);
			base3 = (u32)gp->language == 0 ? float_45_8041cbec : float_40_8041cbf0;
			time3 = *(f32*)((u8*)pose + 0x24);
			if (time3 > base3) {
				alpha = float_1_8041cbb8 - (time3 - base3) / (end3 - base3);
				if (alpha < float_0_8041cbb0) {
					alpha = float_0_8041cbb0;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb98, (u8)(s32)(float_255_8041cbe0 * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb98;
					color.a = (u8)(s32)(float_255_8041cbe0 * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 4:
			end4 = *(f32*)((u8*)*(void**)((u8*)animData + 0x24) + 8);
			base4 = (u32)gp->language == 0 ? float_48_8041cbf4 : float_40_8041cbf0;
			time4 = *(f32*)((u8*)pose + 0x24);
			if (time4 > base4) {
				alpha = float_1_8041cbb8 - (time4 - base4) / (end4 - base4);
				if (alpha < float_0_8041cbb0) {
					alpha = float_0_8041cbb0;
				}
				animPoseSetMaterialFlagOn(data->poseId, 0x40);
#ifdef MUST_MATCH
				animPoseSetMaterialEvtColor(data->poseId, niceEvtColor(*(GXColor*)&dat_8041cb9c, (u8)(s32)(float_255_8041cbe0 * alpha)));
#else
				{
					GXColor color = *(GXColor*)&dat_8041cb9c;
					color.a = (u8)(s32)(float_255_8041cbe0 * alpha);
					animPoseSetMaterialEvtColor(data->poseId, color);
				}
#endif
			}
			break;
		case 6:
			break;
		}
		if (animPoseGetLoopTimes(data->poseId) >= float_1_8041cbb8) {
			animPoseRelease(data->poseId);
			effDelete(effect);
			return;
		}
	}
	if (type == 7) {
		acrobatMain(effect);
	}
	else {
		dispEntry(4, dispCalcZ(&position), 8, effNiceDisp, effect);
	}
}
#pragma opt_common_subs reset

void effNiceDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	NiceEffectWork* data = effect->userdata;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxFinal;

	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_0p017453_8041cba4);
	PSMTXScale(mtxScale, data->field_0x1C, data->field_0x1C, data->field_0x1C);
	PSMTXConcat(mtxTrans, mtxRot, mtxFinal);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
	PSMTXScale(mtxScale, float_8_8041cbd4, float_8_8041cbd4, float_8_8041cbd4);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
	animPoseDrawMtx(data->poseId, mtxFinal, 1, float_0_8041cbb0, float_1_8041cbb8);
	animPoseDrawMtx(data->poseId, mtxFinal, 2, float_0_8041cbb0, float_1_8041cbb8);
	animPoseDrawMtx(data->poseId, mtxFinal, 3, float_0_8041cbb0, float_1_8041cbb8);
}

// The control flow, values and jump table all match. The int->float magic in case 2 now lands in
// f2, the lfs loads are in the original order (the 1.0f start constant is a literal, as in the
// original, instead of the non-const `float_1_8041cbb8` extern — MWCC schedules a non-const load
// after the const ones), and the prologue li order (i, x, y) is reproduced by initialising them in
// the for-clause.
// The comma in the `sfx` initialiser is the `ac_button_down` allocator nudge: it makes MWCC
// materialise the string address directly (`addi rN, r3, @l`) instead of through a scratch
// (`addi r0, r3, @l; mr rN, r0`). It also makes the address temp take r31, so retail's x=r31/y=r30
// become x=r30/y=r29 and sfx=r31 instead of r29; all 10 remaining `!!` are that one
// register-colouring shift. Without the comma the address is copied through r0 and the third word of
// the zero Vec loads into r3 (5 `!!`).
// NONMATCHING (99.78%): one register-colouring shift (x/y/sfx and their uses).
void acrobatMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it into a second local
	   before dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 x;
	s32 y;
#ifdef MUST_MATCH
	// MUST_MATCH: allocator nudge (see above); the natural form is under #else.
	const char* sfx = (0, "SFX_SYSTEM_ACROBAT1");
#else
	const char* sfx = "SFX_SYSTEM_ACROBAT1";
#endif
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
	for (i = 1, x = 4, y = 0xC; i < effect->count; x += 4, y += 0xC, i++, data++) {
		switch (data->field_0x28) {
		case 0:
			data->field_0x20 = x;
			data->field_0x28++;
			if (i == 1) {
				psndSFXOn(sfx);
			}
			break;
		case 1:
			data->field_0x20--;
			if (data->field_0x20 < 0) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 2:
			data->field_0x20++;
			data->position.x = intplGetValue(0xC, data->field_0x20, 0x1E, float_500_8041cbbc, (f32)x);
			if (data->field_0x20 >= 0x1E) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 3:
			data->field_0x20++;
			data->field_0x10 = intplGetValue(0xC, data->field_0x20, 8, 1.0f, float_0p4_8041cbc0);
			data->field_0x14 = intplGetValue(0xC, data->field_0x20, 8, 1.0f, float_1p5_8041cbc4);
			if (data->field_0x20 >= 8) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 4:
			data->field_0x20++;
			if (data->field_0x20 > (effect->count - i - 1) * 4) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 5:
			data->field_0x20++;
			data->position.x = intplGetValue(8, data->field_0x20, 0x1E, (f32)x, (f32)y);
			data->field_0x10 = intplGetValue(0xC, data->field_0x20, 0x1E, float_0p2_8041cbc8, float_1_8041cbb8);
			data->field_0x14 = intplGetValue(0xC, data->field_0x20, 0x1E, float_1p5_8041cbc4, float_1_8041cbb8);
			if (data->field_0x20 >= 0x1E) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 6:
			data->field_0x20++;
			if (data->field_0x20 > 0x10) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 7:
			data->field_0x20++;
			data->field_0x10 = intplGetValue(0xC, data->field_0x20, 0xF, 1.0f, float_1p5_8041cbc4);
			data->field_0x14 = intplGetValue(0xC, data->field_0x20, 0xF, 1.0f, float_0p1_8041cbcc);
			if (data->field_0x20 >= 0xF) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 8:
			data->field_0x20++;
			data->field_0x10 = intplGetValue(0xC, data->field_0x20, 0xF, float_1p5_8041cbc4, float_0p1_8041cbcc);
			data->field_0x14 = intplGetValue(0xC, data->field_0x20, 0xF, float_0p1_8041cbcc, float_2_8041cbd0);
			if (data->field_0x20 >= 0xF) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 9:
			data->field_0x18 = 0.0f;
			data->field_0x14 = 0.0f;
			data->field_0x10 = 0.0f;
			done++;
			break;
		}
	}
	if (done >= effect->count - 1) {
		effDelete(effect);
	}
	else {
#ifdef MUST_MATCH
		dispEntry(4, dispCalcZ(&position), 8, effAcrobatDisp, effect);
#else
		dispEntry(4, dispCalcZ(&disp), 8, effAcrobatDisp, effect);
#endif
	}
}

#pragma opt_common_subs off // without it MWCC CSEs the repeated float_0/float_1 pool loads in the GX vertex block
#ifdef MUST_MATCH
#pragma opt_dead_assignments off // needed with texIdx below: without it MWCC folds the redundant index into i's induction and the loop latch order flips (data++ before the i-1 texid increment)
#endif
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
	PSMTXScale(mtxScale, data->field_0x1C, data->field_0x1C, data->field_0x1C);
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

	max = float_10_8041cbb4;
	negSize = -float_20_8041cba8;
	min = negSize * float_0p5_8041cbac;
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
		if (data->field_0x28 < 6) {
			offsetX = negSize * float_0p5_8041cbac;
			offsetY = float_0_8041cbb0;
		}
		else {
			offsetX = float_0_8041cbb0;
			offsetY = negSize * float_0p5_8041cbac;
		}
		PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
		PSMTXTrans(mtxRot, offsetX, offsetY, float_0_8041cbb0);
		PSMTXScale(mtxScale, data->field_0x10, data->field_0x14, data->field_0x18);
		PSMTXConcat(mtxRot, mtxScale, mtxScale);
		PSMTXTrans(mtxRot, -offsetX, -offsetY, float_0_8041cbb0);
		PSMTXConcat(mtxScale, mtxRot, mtxScale);
		PSMTXConcat(mtxTrans, mtxScale, mtxScale);
		PSMTXConcat(mtxFinal, mtxScale, mtxScale);
		PSMTXConcat(camera->view, mtxScale, mtxScale);
		GXLoadPosMtxImm(mtxScale, 0);
		GXSetCurrentMtx(0);

#ifdef MUST_MATCH
		effGetTexObj(acrobal_tbl[gp->language][texIdx - 1], &texObj);
#else
		effGetTexObj(acrobal_tbl[gp->language][i - 1], &texObj);
#endif
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(min, max, float_0_8041cbb0);
		GXTexCoord2f32(float_0_8041cbb0, float_0_8041cbb0);
		GXPosition3f32(max, max, float_0_8041cbb0);
		GXTexCoord2f32(float_1_8041cbb8, float_0_8041cbb0);
		GXPosition3f32(max, min, float_0_8041cbb0);
		GXTexCoord2f32(float_1_8041cbb8, float_1_8041cbb8);
		GXPosition3f32(min, min, float_0_8041cbb0);
		GXTexCoord2f32(float_0_8041cbb0, float_1_8041cbb8);
	}
}
#pragma opt_common_subs on
#ifdef MUST_MATCH
#pragma opt_dead_assignments reset
#endif
