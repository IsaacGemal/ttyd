#include "eff/eff_itemget.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_rankup.c).
// Its anonymous {0,0,0} template is the unit's first .rodata object and stands in for the
// original's named vec3_802fdda8, so the function must be defined before the named vectors.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

static u8 scale_data[12] = {
	0x00, 0x12, 0x26, 0x3B, 0x4E, 0x60, 0x6E, 0x78, 0x79, 0x72, 0x69, 0x64
};
GXColor white = { 0xE1, 0xE1, 0xE1, 0xFF };
static GXColor color_tbl[10] = {
	{ 0xC8, 0xB2, 0x25, 0xFF },
	{ 0x26, 0xA3, 0x52, 0xFF },
	{ 0x5C, 0xCA, 0x10, 0xFF },
	{ 0xCB, 0x4C, 0x4D, 0xFF },
	{ 0x7E, 0xC2, 0x1E, 0xFF },
	{ 0x30, 0x9C, 0xBF, 0xFF },
	{ 0xEB, 0x7D, 0x06, 0xFF },
	{ 0xE8, 0x5B, 0xD0, 0xFF },
	{ 0x51, 0x75, 0x9D, 0xFF },
	{ 0xC8, 0xB2, 0x25, 0xFF },
};
u8 size_tbl[12] = {
	0x32, 0x32, 0x32, 0x28, 0x0C, 0x0C, 0x0C, 0x0A, 0x0A, 0x0F, 0x00, 0x00
};

EffectEntry* effItemGetEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effItemGetData* data;
	effItemGetData* ptr;
	s32 count;
	s32 i;

	entry = effEntry();
	count = 1;
	if (type == 0 || type == 3 || type == 9) {
		count = 8;
	} else if (type == 10) {
		count = 6;
	}
	entry->type = "ItemGet";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(effItemGetData));
	entry->userdata = data;
	entry->callback = effItemGetMain;
	entry->flags |= 2;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->animScale = 0.0f;
	data->baseSize = (f32)size_tbl[data->type] / 10.0f * 0.75f;
	data->frame = 0;
	data->scaleMode = 0;
	data->camId = CAMERA_3D;
	data->alpha = 0xFF;
	data->life = 1000;
	ptr = data + 1;
	for (i = 1; i < entry->count; i++, ptr++) {
		ptr->position.x = 0.0f;
		ptr->position.y = 0.0f;
		ptr->position.z = 0.0f;
		if (type == 10) {
			f32 c;
			ptr->angle = 1.0471976f + 6.2831855f * (i - 1) / 6.0f;
			ptr->velX = 5.0f * (f32)sin(ptr->angle);
			c = (f32)cos(ptr->angle);
			ptr->velY = -c * 5.0f;
			ptr->velZ = 0.0f;
		} else {
			ptr->angle = 6.2831855f * (i - 1) / (entry->count - 1);
			ptr->velX = (f32)sin(ptr->angle);
			ptr->velY = -(f32)cos(ptr->angle);
			ptr->velZ = 0.0f;
		}
		ptr->alpha = 0;
		ptr->frame = 0;
	}
	return entry;
}

void effItemGetMain(EffectEntry* effect) {
	effItemGetData* data = effect->userdata;
	Vec disp;
	s32 zeroCount = 0;
	s32 type;
	s32 camera;
	s32 i;

	disp = initializeVec(&data->position);

	camera = data->camId;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}
	if (data->life < 1000) {
		data->life--;
	}
	if (data->life < 16) {
		data->alpha = data->life << 4;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->frame++;
	if ((u32)data->frame < 12) {
		data->animScale = (f32)scale_data[data->frame] / 100.0f;
	}
	if (data->frame > 1000) {
		data->frame = 1000;
	}
	switch (data->scaleMode) {
	case 0:
		break;
	case 1:
		if (data->animScale < 100.0f) {
			data->animScale *= 1.1f;
		}
		break;
	case 2:
		data->animScale *= 0.9f;
		break;
	}
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		data->frame++;
		if (data->frame < 16) {
			data->alpha += 16;
			if (data->alpha > 0xFF) {
				data->alpha = 0xFF;
			}
		} else {
			data->alpha -= 8;
			if (data->alpha < 0) {
				data->alpha = 0;
			}
		}
		if (data->frame > 1000) {
			data->frame = 1000;
		}
		if (data->alpha == 0) {
			zeroCount++;
		} else {
			data->position.x += data->velX;
			data->position.y += data->velY;
			data->position.z += data->velZ;
			if (type == 10) {
				data->velX *= 0.9f;
				data->velY *= 0.9f;
				data->velZ *= 0.9f;
			}
		}
	}
	if (type == 10) {
		if (zeroCount >= effect->count - 1) {
			effDelete(effect);
			return;
		}
		dispEntry(camera, dispCalcZ(&disp), 2, effItemUseDisp, effect);
	} else {
		dispEntry(camera, dispCalcZ(&disp), 2, effItemGetDisp, effect);
	}
}

void effItemGetDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effItemGetData* data = effect->userdata;
	CameraEntry* cam;
	Mtx mtxTrans;
	Mtx mtxScale;
	Mtx mtxRot;
	Mtx mtxTex;
	Mtx mtxLight;
	Mtx mtxLook;
	Mtx mtxBase;
	GXTexObj texObj;
	Vec vecA;
	Vec vecB;
	Vec vecC;
	s32 alphaB;
	s32 alphaA;
	s32 headAlpha;
	s32 i;
	s32 j;
	f32 f31;
	f32 f30;
	f32 f30b;
	f32 f29;
	f32 scale;

	cam = camGetPtr(camId);
	vecA = (Vec){ 0.0f, 0.0f, 1000.0f };
	vecB = (Vec){ 0.0f, 1.0f, 0.0f };
	vecC = (Vec){ 0.0f, 0.0f, 0.0f };
	scale = data->animScale * data->baseSize;
	headAlpha = data->alpha;

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXScale(mtxScale, scale, scale, scale);
	PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
	PSMTXConcat(mtxTrans, mtxScale, mtxBase);
	f31 = (f32)cos(6.2831855f * camGetPtr(camId)->viewYaw / 360.0f);
	PSMTXTrans(mtxTrans, 2.0f * (f32)sin(6.2831855f * camGetPtr(camId)->viewYaw / 360.0f), 0.0f, 2.0f * -f31);
	PSMTXConcat(mtxTrans, mtxBase, mtxBase);

	GXSetTevColor(GX_TEVREG0, white);
	GXSetTevColor(GX_TEVREG1, color_tbl[data->type]);
	C_MTXLightOrtho(mtxLight, 0.0f, 6.0f, 0.0f, 6.0f, 0.5f, 0.5f, 0.5f, 0.5f);
	PSMTXMultVec(mtxRot, &vecA, &vecA);
	C_MTXLookAt(mtxLook, &vecA, &vecB, &vecC);
	PSMTXConcat(mtxLight, mtxLook, mtxLight);
	PSMTXConcat(mtxLight, mtxBase, mtxTex);

	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXLoadTexMtxImm(mtxTex, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	effGetTexObj(0x2C, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObj(0x2A, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	alphaA = headAlpha * 128 / 255;
	alphaB = headAlpha * 255 / 255;
	f31 = 32.0f;
	{
		f32 f = 64.0f;
		f32 half = 0.5f;
		f30 = -f * half;
	}
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			PSMTXConcat(cam->view, mtxBase, mtxTrans);
			PSMTXTrans(mtxRot, 2.0f, -2.0f, 0.0f);
			PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
			GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0, 0, 0, alphaA });
		} else {
			PSMTXConcat(cam->view, mtxBase, mtxTrans);
			GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, alphaB });
		}
		GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(f30, f31, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f31, f31, 0.0f);
		GXTexCoord2f32(2.0f, 0.0f);
		GXPosition3f32(f31, f30, 0.0f);
		GXTexCoord2f32(2.0f, 2.0f);
		GXPosition3f32(f30, f30, 0.0f);
		GXTexCoord2f32(0.0f, 2.0f);
	}

	GXSetTevColor(GX_TEVREG0, color_tbl[data->type]);
	GXSetTevColor(GX_TEVREG1, white);
	effGetTexObj(0x2B, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);

	{
		f32 f42 = 42.666668f;
		f30b = -f42;
	}
	f31 = 5.3333335f;
	// The half-size is computed inside the loop; MWCC hoists the negate back to the preheader but
	// colours its scratch in f2 (as the original) instead of fusing it into the product destination's
	// register.  `f30b` gives the direct `-42.666668f` its own variable (reused as machine f30), which
	// puts the first-loop cos result in f31 as the original.  The values are idempotent, so recomputing
	// them per iteration is harmless.
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		{
			f32 f = 10.666667f;
			f29 = -f;
		}
		f29 = f29 / 2.0f;
		for (j = 0; j < 2; j++) {
			if (j == 0) {
				alphaA = data->alpha * 128 / 255;
				if (data->alpha != 0xFF) {
					alphaA -= 20;
					if (alphaA < 0) {
						alphaA = 0;
					}
				}
				PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(mtxBase, mtxTrans, mtxTex);
				PSMTXTrans(mtxTrans, 2.0f, -2.0f, 0.0f);
				PSMTXConcat(mtxTex, mtxTrans, mtxTex);
				PSMTXRotRad(mtxRot, 'z', data->angle);
				PSMTXConcat(mtxTex, mtxRot, mtxTex);
				GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0, 0, 0, alphaA });
			} else {
				PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
				PSMTXConcat(mtxBase, mtxTrans, mtxTex);
				PSMTXRotRad(mtxRot, 'z', data->angle);
				PSMTXConcat(mtxTex, mtxRot, mtxTex);
				GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, data->alpha * 255 / 255 });
			}
			PSMTXConcat(cam->view, mtxTex, mtxTrans);
			GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			PSMTXConcat(mtxLight, mtxTex, mtxTrans);
			GXLoadTexMtxImm(mtxTrans, GX_TEXMTX0, GX_MTX3x4);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(f29, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(f31, 0.0f, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(f31, f30b, 0.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(f29, f30b, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

void effItemUseDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effItemGetData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx1;
	Mtx mtx3;
	Mtx mtx2;
	Mtx mtx4;
	s32 i;
	s32 j;
	f32 f30;
	f32 f31;
	f32 f29;

	cam = camGetPtr(camId);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtx2, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXScale(mtx3, 0.4f, 0.4f, 0.4f);
	PSMTXConcat(mtx1, mtx3, mtx4);
	PSMTXConcat(mtx4, mtx2, mtx4);
	PSMTXConcat(cam->view, mtx4, mtx4);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	effGetTexObj(0x2B, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);

	// The half-size is computed inside the loop after the j==0 branch's 2.0f call; MWCC hoists it to
	// the preheader, colours the product's negate scratch in f2 (as the original) and pools 2.0 before
	// 0.5.  The values are idempotent, so recomputing them per iteration is harmless.
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		for (j = 0; j < 2; j++) {
			if (j == 0) {
				PSMTXTrans(mtx1, data->position.x + 2.0f, data->position.y - 2.0f, data->position.z);
				PSMTXRotRad(mtx2, 'z', data->angle);
				PSMTXConcat(mtx1, mtx2, mtx1);
				PSMTXConcat(mtx4, mtx1, mtx1);
				GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0, 0, 0, (data->alpha * 64) / 255 });
			} else {
				PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
				PSMTXRotRad(mtx2, 'z', data->angle);
				PSMTXConcat(mtx1, mtx2, mtx1);
				PSMTXConcat(mtx4, mtx1, mtx1);
				GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, (data->alpha * 128) / 255 });
			}
			{
				f32 f = 21.333334f;
				f31 = -f;
			}
			f31 = f31 / 2.0f;
			{
				f32 f = 42.666668f;
				f30 = -f;
			}
			f29 = 10.666667f;
			GXLoadPosMtxImm(mtx1, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(f31, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(f29, 0.0f, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(f29, f30, 0.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(f31, f30, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

// the 4 zero bytes after the unit's last .sdata object (gap_07_804159FC_sdata)
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") static u32 gap_end_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
