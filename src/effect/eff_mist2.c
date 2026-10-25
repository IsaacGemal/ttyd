#include "matching.h"
#include "eff/eff_mist2.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"

extern const f32 float_deg2rad_80421ca8;
extern const f32 float_16_80421cac;
extern const f32 float_255_80421cb0;
extern const f32 float_0p5_80421cb4;

EffectEntry* effMist2Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 range, f32 yOffset) {
	EffectEntry* entry;
	EffMist2Data* data;
	EffMist2Data* elem;
	s32 num;
	s32 count;
	s32 i;
	f32 fAngle;
	f32 fRange;

	entry = effEntry();
	num = (type == 0) ? 30 : 10;
	count = num + 1;
	entry->type = "Mist2";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(EffMist2Data));
	entry->userdata = data;
	entry->callback = effMist2Main;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = lifetime;
	}
	data->field_0x30 = 0;
	data->alpha = float_255_80421cb0;
	data->scale = 1.0f;
	data->colorR = 0;
	data->colorG = 0;
	data->colorB = 0;

	elem = &data[1];
	for (i = 1; i < entry->count; i++, elem++) {
		elem->range = range;
		elem->yOffset = yOffset;
		fRange = (f32)irand((s32)elem->range);
		fAngle = 6.2831855f * (f32)irand(360) / 360.f;
		elem->x = fRange * (f32)sin(fAngle);
		elem->y = 0.0f;
		elem->z = fRange * (f32)cos(fAngle);
		elem->scale = 0.0f;
		elem->targetScale = float_0p5_80421cb4 + (f32)irand(100) / 100.f;
		elem->alpha = float_255_80421cb0;
		elem->lifetime = irand(60) + 60;
		elem->field_0x30 = 0;
		elem->rotZVel = (f32)(5 - irand(10));
		elem->rotZ = 0.0f;
		elem->delay = irand(30);
		elem->gravity = 0.2f;
		elem->velY = 0.0f;
		if (type == 1) {
			f32 fA = 6.2831855f * (f32)irand(360) / 360.f;
			elem->x = elem->range * (f32)sin(fA);
			elem->y = elem->yOffset;
			elem->z = elem->range * (f32)cos(fA);
		}
	}
	return entry;
}

void effMist2Main(EffectEntry* effect) {
	EffMist2Data* data = effect->userdata;
	Vec position;
	Vec tmp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 i;
	s32 type;
	f32 fAngle;
	f32 fRange;

	tmp.x = data->x;
	tmp.y = data->y;
	tmp.z = data->z;
	position = tmp;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x20;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->lifetime < 0x20) {
		data->alpha = (f32)(data->lifetime << 3);
	}

	data->field_0x30++;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->delay != 0) {
			data->delay--;
			continue;
		}
		data->lifetime--;
		if (data->lifetime < 0x20) {
			data->alpha = (f32)(data->lifetime << 3);
		}
		if (data->alpha <= 0.0f) {
			fRange = (f32)irand((s32)data->range);
			fAngle = 6.2831855f * (f32)irand(360) / 360.f;
			data->x = fRange * (f32)sin(fAngle);
			data->y = 0.0f;
			data->z = fRange * (f32)cos(fAngle);
			data->scale = 0.0f;
			data->targetScale = float_0p5_80421cb4 + (f32)irand(100) / 100.f;
			data->alpha = float_255_80421cb0;
			data->lifetime = irand(60) + 60;
			data->field_0x30 = 0;
			data->rotZVel = (f32)(5 - irand(10));
			data->rotZ = 0.0f;
			data->delay = irand(30);
			data->gravity = 0.2f;
			data->velY = 0.0f;
		} else {
			data->rotZ += data->rotZVel;
			data->scale += (data->targetScale - data->scale) / 10.f;
			if (type == 1) {
				data->y += data->velY;
				data->velY -= data->gravity;
				if (data->y < 0.0f) {
					data->y = 0.0f;
				}
			}
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMist2Disp, effect);
}

void effMist2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	EffMist2Data* work = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxView;
	s32 i;
	s32 alpha;
	u8 cr;
	u8 cg;
	u8 cb;
	f32 f28;
	f32 f27;

	cam = camGetPtr(cameraId);
	alpha = (s32)work->alpha;
	cr = work->colorR;
	cg = work->colorG;
	cb = work->colorB;
	effGetTexObj(0x5E, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	PSMTXTrans(mtxTrans, work->x, work->y, work->z);
	PSMTXRotRad(mtxRot, 'y', float_deg2rad_80421ca8 * -camGetPtr(cameraId)->viewYaw);
	PSMTXScale(mtxScale, work->scale, work->scale, work->scale);
	PSMTXConcat(mtxTrans, mtxRot, mtxView);
	PSMTXConcat(mtxView, mtxScale, mtxView);
	PSMTXConcat(cam->view, mtxView, mtxView);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	f28 = -float_16_80421cac * float_0p5_80421cb4;
	work = &work[1];
	for (i = 1; i < effect->count; i++, work++) {
		PSMTXTrans(mtxTrans, work->x, work->y, work->z);
		PSMTXScale(mtxScale, work->scale, work->scale, work->scale);
		PSMTXRotRad(mtxRot, 'z', float_deg2rad_80421ca8 * work->rotZ);
		PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
		PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
		PSMTXConcat(mtxView, mtxTrans, mtxTrans);
		GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		f27 = 8.0f;
		GXSetChanMatColor(GX_COLOR0A0, (GXColor){ cr, cg, cb, (u8)(work->alpha * (f32)alpha / float_255_80421cb0) });
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(f28, f27, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f27, f27, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(f27, f28, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(f28, f28, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_80421ca8 = 0.017453292f;
const f32 float_16_80421cac = 16.0f;
const f32 float_255_80421cb0 = 255.0f;
const f32 float_0p5_80421cb4 = 0.5f;
