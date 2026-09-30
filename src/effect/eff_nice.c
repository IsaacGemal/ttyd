#include "eff/eff_nice.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

extern GlobalWork* gp;

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

#pragma opt_common_subs off // without it MWCC CSEs the repeated float_0/float_1 pool loads in the GX vertex block
// NONMATCHING (99.91%): only the two loop-increment stores are scheduled in the opposite order
// (`addi r29,r29,4` texidx vs `addi r28,r28,0x64` data++); every other instruction is identical.
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
	data = data + 1;
	for (; i < effect->count; i++, data++) {
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

		effGetTexObj(acrobal_tbl[gp->language][i - 1], &texObj);
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
