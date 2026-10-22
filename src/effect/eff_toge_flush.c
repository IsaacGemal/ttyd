#include "eff/eff_toge_flush.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"


static u8 color_rotation_data[24] = {
	0xFF, 0xFF, 0x52, 0xFF, 0xFF, 0x52, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
	0xFF, 0xFF, 0xD6, 0xFF, 0xFF, 0xD6, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
};



EffectEntry* effTogeFlushEntry(s32 type, f32 x, f32 y, f32 z, s32 life) {
	EffectEntry* entry;
	effTogeFlushData* data;

	entry = effEntry();
	entry->type = "TogeFlush";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x34);
	entry->userdata = data;
	entry->callback = effTogeFlushMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = life;
	data->frame = 0;
	data->innerAlpha = 0;
	data->angle = 0.0f;
	if (type == 0 || type == 3) {
		data->angleSpeed = 30.0f;
	} else {
		data->angleSpeed = 0.0f;
	}
	data->innerScale = 0.0f;
	data->innerScaleSpeed = 0.8f;
	data->outerAlpha = 0;
	data->outerScale = 0.0f;
	return entry;
}

void effTogeFlushMain(EffectEntry* effect) {
	s32 life;
	effTogeFlushData* data = effect->userdata;
	s32 frame;
	f32 angle;
	f32 s;

	data->life--;
	data->frame++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	frame = data->frame;
	if (frame <= 6) {
		data->innerAlpha = (s32)(0.5f * (218 - data->innerAlpha) + data->innerAlpha);
	}
	if (frame > 6) {
		data->innerAlpha *= 0.78f;
	}

	data->innerScale += data->innerScaleSpeed;
	data->innerScaleSpeed += 0.29f;
	data->angle += data->angleSpeed;
	data->angleSpeed *= 0.93f;
	angle = 6.2831855f * (f32)(life * 50) / 360.0f;
	s = (f32)sin(angle);
	data->outerScale = 0.5f * s + 0.9f;
	angle = 6.2831855f * (f32)(life * 40) / 360.0f;
	s = (f32)sin(angle);
	data->outerAlpha = (s32)(64.0f * s + 144.0f);

	dispEntry(4, dispCalcZ(&(Vec){ data->x, data->y, data->z }), 2, effTogeFlushDisp, effect);
}


void effTogeFlushDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effTogeFlushData* data = effect->userdata;
	GXTexObj texObj;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxPos;
	Mtx mtxTmp;
	s32 type = data->type;
	s32 alpha;
	s32 idx;
	s32 colorAlpha;

	if (data->life > 10) {
		effGetTexObj(0x19, &texObj);
	} else if (data->life > 5) {
		effGetTexObj(0x1A, &texObj);
	} else {
		effGetTexObj(0x1B, &texObj);
	}
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(0);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	PSMTXTrans(mtxTrans, data->x, data->y, data->z);
	PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
	PSMTXConcat(mtxTrans, mtxRot, mtxTmp);
	colorAlpha = data->outerAlpha;
	idx = data->frame * 3;
	PSMTXScale(mtxScale, data->outerScale, data->outerScale, data->outerScale);
	PSMTXConcat(mtxTmp, mtxScale, mtxPos);
	PSMTXConcat(camGetPtr(cameraId)->view, mtxPos, mtxPos);
	GXLoadPosMtxImm(mtxPos, 0);
	GXSetCurrentMtx(0);

	switch (type) {
	case 0:
		idx %= 24;
		GXSetTevColor(1, (GXColor){ color_rotation_data[idx + 0], color_rotation_data[idx + 1],
		                            color_rotation_data[idx + 2], colorAlpha });
		break;
	case 1:
		GXSetTevColor(1, (GXColor){ 0xFF, 0x00, 0x00, colorAlpha });
		break;
	case 2:
		GXSetTevColor(1, (GXColor){ 0x00, 0x00, 0xFF, colorAlpha });
		break;
	case 3:
		GXSetTevColor(1, (GXColor){ 0xFF, 0x00, 0x00, colorAlpha });
		break;
	case 4:
		GXSetTevColor(1, (GXColor){ 0xFF, 0xFF, 0xFF, colorAlpha });
		break;
	}

	GXBegin(0x80, 0, 4);
	GXPosition3f32(-8.0f, 8.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(8.0f, 8.0f, 0.0f);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3f32(8.0f, -8.0f, 0.0f);
	GXTexCoord2f32(2.0f, 2.0f);
	GXPosition3f32(-8.0f, -8.0f, 0.0f);
	GXTexCoord2f32(0.0f, 2.0f);

	alpha = data->innerAlpha;
	if (alpha > 0xFF) {
		alpha = 0xFF;
	}
	if (alpha > 0) {
		PSMTXScale(mtxScale, data->innerScale, data->innerScale, data->innerScale);
		PSMTXConcat(mtxTmp, mtxScale, mtxPos);
		PSMTXRotRad(mtxRot, 'z', 0.017453292f * data->angle);
		PSMTXConcat(mtxPos, mtxRot, mtxPos);
		PSMTXConcat(camGetPtr(cameraId)->view, mtxPos, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		GXSetCurrentMtx(0);

		switch (type) {
		case 3:
			GXSetTevColor(1, (GXColor){ 0xF1, 0x4B, 0x41, alpha });
			break;
		case 4:
			GXSetTevColor(1, (GXColor){ 0xC9, 0xC0, 0xCF, alpha });
			break;
		default:
			GXSetTevColor(1, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
			break;
		}

		GXBegin(0x80, 0, 4);
		GXPosition3f32(-4.0f, 4.0f, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(4.0f, 4.0f, 0.0f);
		GXTexCoord2f32(2.0f, 0.0f);
		GXPosition3f32(4.0f, -4.0f, 0.0f);
		GXTexCoord2f32(2.0f, 2.0f);
		GXPosition3f32(-4.0f, -4.0f, 0.0f);
		GXTexCoord2f32(0.0f, 2.0f);
	}
}
