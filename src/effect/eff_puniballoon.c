#include "eff/eff_puniballoon.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"
#include <stdlib.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_queen.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effPuniBalloonEntry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	EffPuniBalloonData* data;
	s32 i;
	s32 count;

	entry = effEntry();
	count = 1;

	switch (type) {
	case 0:
		count = 1;
		break;
	case 1:
		count = 11;
		break;
	}

	entry->type = "PuniBalloon";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffPuniBalloonData) * entry->count);
	entry->userdata = data;
	entry->callback = effPuniBalloonMain;
	entry->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x24 = 0.0f;
	data->lifetime = (type == 0) ? 1000 : 30;
	data->counter = 0;
	data->scale = 0.0f;
	data->field_0x1c = scale;

	data++;
	for (i = 1; i < entry->count; i++, data++) {
		data->position.x = 0.0f;
		data->position.y = 0.0f;
		data->position.z = 0.0f;
		data->scale = scale;
		data->velX = (f32)(rand() % 11 - 5);
		data->velY = (f32)(rand() % 11 - 5);
		data->velZ = (f32)(rand() % 11 - 5);
	}

	return entry;
}

void effPuniBalloonMain(EffectEntry* effect) {
	EffPuniBalloonData* data = effect->userdata;
	Vec position;
	s32 type;
	s32 i;
	f32 scale;

	position = initializeVec(&data->position);
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x10;
	}

	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->counter++;

	if (data->lifetime < 0) {
		if (type == 0) {
			effPuniBalloonEntry(1, data->position.x, data->position.y, data->position.z, 1.0f);
		}
		effDelete(effect);
		return;
	}

	scale = data->scale;
	data->scale = scale + (1.0f - scale) / 10.0f;
	data->field_0x24 = 20.0f * (f32)sin((f32)data->counter / 20.0f);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		data->position.x += data->velX;
		data->position.y += data->velY;
		data->position.z += data->velZ;
		data->velY += -0.1f;
		data->velX *= 0.98f;
		data->velY *= 0.98f;
		data->velZ *= 0.98f;
		data->scale *= 0.9f;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPuniBalloonDisp, effect);
}

void effPuniBalloonDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffPuniBalloonData* data = effect->userdata;

	if (data->type == 0) {
		balloon_0(camId, effect);
	} else {
		balloon_1(camId, effect);
	}
}

void balloon_0(CameraId camId, EffectEntry* effect) {
	EffPuniBalloonData* data = effect->userdata;
	CameraEntry* camera;
	Mtx mtx;
	Mtx rot;
	Mtx scl;
	Mtx view;
	GXTexObj tex;
	f32 scale;

	camera = camGetPtr(camId);
	scale = data->scale;
	{
		f32 s = (f32)sin((f32)data->counter / 10.0f);
		scale += s * 0.1f;
	}
	scale *= data->field_0x1c;
	PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rot, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
	PSMTXConcat(mtx, rot, view);
	PSMTXConcat(camera->view, view, mtx);
	PSMTXRotRad(rot, 'z', MTXDegToRad(data->field_0x24));
	PSMTXScale(scl, scale, scale, scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scl, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7d);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ONE, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_1_2);
	effGetTexObj(0x26, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-12, 12, 0);
	GXTexCoord2s16(0, 0);
	GXPosition3s16(-12, -12, 0);
	GXTexCoord2s16(0, 1);
	GXPosition3s16(12, -12, 0);
	GXTexCoord2s16(2, 1);
	GXPosition3s16(12, 12, 0);
	GXTexCoord2s16(2, 0);
	GXSetNumIndStages(0);
	GXSetTevDirect(GX_TEVSTAGE0);
}

void balloon_1(CameraId camId, EffectEntry* effect) {
	EffPuniBalloonData* data = effect->userdata;
	CameraEntry* camera;
	Mtx mtxB;
	Mtx mtxA;
	Mtx rot;
	Mtx scl;
	GXTexObj tex;
	s32 i;

	camera = camGetPtr(camId);
	PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rot, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
	PSMTXConcat(mtxA, rot, mtxB);
	PSMTXConcat(camera->view, mtxB, mtxB);

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7d);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ONE, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_1_2);
	effGetTexObj(0x26, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scl, data->scale, data->scale, data->scale);
		PSMTXConcat(mtxA, scl, mtxA);
		PSMTXConcat(mtxB, mtxA, mtxA);
		GXLoadPosMtxImm(mtxA, 0);
		GXSetCurrentMtx(0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3s16(-12, 12, 0);
		GXTexCoord2s16(0, 0);
		GXPosition3s16(-12, -12, 0);
		GXTexCoord2s16(0, 1);
		GXPosition3s16(12, -12, 0);
		GXTexCoord2s16(2, 1);
		GXPosition3s16(12, 12, 0);
		GXTexCoord2s16(2, 0);
	}
}
