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

// NONMATCHING (98.91%): same instructions; only register numbers or operands differ.
BOOL effNiceAsync(s32 group) {
	const char* name;
	BOOL valid;
	int i;

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

// NONMATCHING (96.93%): 4 instructions differ, 2 missing, plus register differences.
EffectEntry* effNiceEntry(s32 type, f32 x, f32 y, f32 z) {
	NiceEffectWork *userdata, *ptr;
	EffectEntry* effect;
	u32 size, count;
	int i;

	effect = effEntry();
	if (type == 5) { //guard for unused slot
		type = 0;
	}
	if (type == 7) {
		if (gp->language) { //TODO: ternary?
			count = 7;
		}
		else {
			count = 8;
		}
	}
	else {
		count = 1;
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
	if (type == 7) { //initialize other entries
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
	}
	else {
		userdata->poseId = animPoseEntry(anim_tbl[gp->language][type], gp->inBattle != 0);
		animPoseSetAnim(userdata->poseId, "A_1", TRUE);
		animPoseSetMaterialFlagOn(userdata->poseId, 0x1800);
		animPoseSetGXFunc(userdata->poseId, rendermodeFunc, FALSE);
	}
	return effect;
}

// NONMATCHING (0.41%): empty placeholder, not decompiled yet.
void effNiceMain(EffectEntry* effect) {

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
	PSMTXScale(mtxScale, data->field_0x1C, data->field_0x1C, data->field_0x1C);
	PSMTXConcat(mtxTrans, mtxRot, mtxFinal);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
	PSMTXScale(mtxScale, float_8_8041cbd4, float_8_8041cbd4, float_8_8041cbd4);
	PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
	animPoseDrawMtx(data->poseId, mtxFinal, 1, float_0_8041cbb0, float_1_8041cbb8);
	animPoseDrawMtx(data->poseId, mtxFinal, 2, float_0_8041cbb0, float_1_8041cbb8);
	animPoseDrawMtx(data->poseId, mtxFinal, 3, float_0_8041cbb0, float_1_8041cbb8);
}

// The control flow, values and jump table all match: (1) the string address is materialised as
// `addi r0,...; mr r29,r0` instead of a direct `addi r29`; (2) the int->float magic in case 2
// lands in f1 (original f2) and some lfs loads are scheduled in the opposite order; (3) the
// prologue initialises x/y before i. Needs the rest of the unit's .sdata2/.rodata before the
// anonymous pool offsets all resolve.
// NONMATCHING (87.05%): string address rematerialised through r0, case-2 int->float in f1 not f2, prologue x/y before i.
void acrobatMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it into a second local
	   before dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 x;
	s32 y;
	const char* sfx = "SFX_SYSTEM_ACROBAT1";
	NiceEffectWork* data = effect->userdata;
	s32 i;
	s32 done = 0;

	x = 4;
	y = 0xC;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data++;
	for (i = 1; i < effect->count; i++, data++, x += 4, y += 0xC) {
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
			data->position.x = intplGetValue(0xC, ++data->field_0x20, 0x1E, float_500_8041cbbc, (f32)x);
			if (data->field_0x20 >= 0x1E) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 3:
			data->field_0x10 = intplGetValue(0xC, ++data->field_0x20, 8, float_1_8041cbb8, float_0p4_8041cbc0);
			data->field_0x14 = intplGetValue(0xC, data->field_0x20, 8, float_1_8041cbb8, float_1p5_8041cbc4);
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
			data->position.x = intplGetValue(8, ++data->field_0x20, 0x1E, (f32)x, (f32)y);
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
			data->field_0x10 = intplGetValue(0xC, ++data->field_0x20, 0xF, float_1_8041cbb8, float_1p5_8041cbc4);
			data->field_0x14 = intplGetValue(0xC, data->field_0x20, 0xF, float_1_8041cbb8, float_0p1_8041cbcc);
			if (data->field_0x20 >= 0xF) {
				data->field_0x20 = 0;
				data->field_0x28++;
			}
			break;
		case 8:
			data->field_0x10 = intplGetValue(0xC, ++data->field_0x20, 0xF, float_1p5_8041cbc4, float_0p1_8041cbcc);
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
