#include "eff/eff_toge_flush.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

static u8 color_rotation_data[24] = {
	0xFF, 0xFF, 0x52, 0xFF, 0xFF, 0x52, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
	0xFF, 0xFF, 0xD6, 0xFF, 0xFF, 0xD6, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
};

const GXColor dat_8041e328 = { 0xFF, 0x00, 0x00, 0x00 };
const GXColor dat_8041e32c = { 0x00, 0x00, 0xFF, 0x00 };
const GXColor dat_8041e330 = { 0xFF, 0x00, 0x00, 0x00 };
const GXColor dat_8041e334 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041e338 = { 0xF1, 0x4B, 0x41, 0x00 };
const GXColor dat_8041e33c = { 0xC9, 0xC0, 0xCF, 0x00 };
const GXColor dat_8041e340 = { 0xFF, 0xFF, 0xFF, 0x00 };
GXColor unk_80423070;

const Vec vec3_802f8e80 = { 0.0f, 0.0f, 0.0f };
const char str_802F8E98[] = "TogeFlush";

EffectEntry* effTogeFlushEntry(s32 type, f32 x, f32 y, f32 z, s32 life) {
	EffectEntry* entry;
	effTogeFlushData* data;

	entry = effEntry();
	entry->type = str_802F8E98;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x34);
	entry->userdata = data;
	entry->callback = effTogeFlushMain;
	data->field_0x0 = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x2C = life;
	data->field_0x30 = 0;
	data->field_0x24 = 0;
	data->field_0x10 = 0.0f;
	if (type == 0 || type == 3) {
		data->field_0x14 = 30.0f;
	} else {
		data->field_0x14 = 0.0f;
	}
	data->field_0x18 = 0.0f;
	data->field_0x1C = 0.8f;
	data->field_0x28 = 0;
	data->field_0x20 = 0.0f;
	return entry;
}

// NONMATCHING (95.82%): register allocation, both int->float alpha conversions and the float integrator
// (0x10/0x14/0x18/0x1C) match. The compiler hoists the three `vec3_802f8e80` loads above the second
// `fctiwz`; the original schedules them after the `stw r0, 0x28(r30)` that stores the colour index.
void effTogeFlushMain(EffectEntry* effect) {
	s32 life;
	effTogeFlushData* data = effect->userdata;
	s32 frame;
	Vec pos;
	f32 angle;

	data->field_0x2C--;
	data->field_0x30++;
	life = data->field_0x2C;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	frame = data->field_0x30;
	if (frame <= 6) {
		data->field_0x24 = (s32)(0.5f * (218 - data->field_0x24) + data->field_0x24);
	}
	if (frame > 6) {
		data->field_0x24 *= 0.78f;
	}

	data->field_0x18 += data->field_0x1C;
	data->field_0x1C += 0.29f;
	data->field_0x10 += data->field_0x14;
	data->field_0x14 *= 0.93f;
	angle = 6.2831855f * (f32)(life * 50) / 360.0f;
	data->field_0x20 = 0.5f * (f32)sin(angle) + 0.9f;
	angle = 6.2831855f * (f32)(life * 40) / 360.0f;
	data->field_0x28 = (s32)(64.0f * (f32)sin(angle) + 144.0f);

	{
		pos = vec3_802f8e80;
		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		dispEntry(4, dispCalcZ(&pos), 2, effTogeFlushDisp, effect);
	}
}

// NONMATCHING (99.90%): every opcode matches; the only diffs are the compiler colouring the per-case
// `GXColor color` temp and the by-value argument copy to different stack slots (ours: temp 0x44/0x08..0x14,
// arg 0x40/0x38.. ; original: an 8-byte block per case at 0x40/0x38/0x30/0x28/0x20/0x18/0x10/0x08).
void effTogeFlushDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effTogeFlushData* data = effect->userdata;
	GXColor color;
	GXTexObj texObj;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxPos;
	Mtx mtxTmp;
	s32 type = data->field_0x0;
	s32 alpha;
	s32 idx;
	s32 colorAlpha;

	if (data->field_0x2C > 10) {
		effGetTexObj(0x19, &texObj);
	} else if (data->field_0x2C > 5) {
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
	colorAlpha = data->field_0x28;
	idx = data->field_0x30 * 3;
	PSMTXScale(mtxScale, data->field_0x20, data->field_0x20, data->field_0x20);
	PSMTXConcat(mtxTmp, mtxScale, mtxPos);
	PSMTXConcat(camGetPtr(cameraId)->view, mtxPos, mtxPos);
	GXLoadPosMtxImm(mtxPos, 0);
	GXSetCurrentMtx(0);

	switch (type) {
	case 0:
		idx %= 24;
		color = *(GXColor*)&unk_80423070;
		color.a = colorAlpha;
		color.r = color_rotation_data[idx + 0];
		color.g = color_rotation_data[idx + 1];
		color.b = color_rotation_data[idx + 2];
		GXSetTevColor(1, color);
		break;
	case 1:
		color = dat_8041e328;
		color.a = colorAlpha;
		GXSetTevColor(1, color);
		break;
	case 2:
		color = dat_8041e32c;
		color.a = colorAlpha;
		GXSetTevColor(1, color);
		break;
	case 3:
		color = dat_8041e330;
		color.a = colorAlpha;
		GXSetTevColor(1, color);
		break;
	case 4:
		color = dat_8041e334;
		color.a = colorAlpha;
		GXSetTevColor(1, color);
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

	alpha = data->field_0x24;
	if (alpha > 0xFF) {
		alpha = 0xFF;
	}
	if (alpha > 0) {
		PSMTXScale(mtxScale, data->field_0x18, data->field_0x18, data->field_0x18);
		PSMTXConcat(mtxTmp, mtxScale, mtxPos);
		PSMTXRotRad(mtxRot, 'z', 0.017453292f * data->field_0x10);
		PSMTXConcat(mtxPos, mtxRot, mtxPos);
		PSMTXConcat(camGetPtr(cameraId)->view, mtxPos, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		GXSetCurrentMtx(0);

		switch (type) {
		case 3:
			color = dat_8041e338;
			color.a = alpha;
			GXSetTevColor(1, color);
			break;
		case 4:
			color = dat_8041e33c;
			color.a = alpha;
			GXSetTevColor(1, color);
			break;
		default:
			color = dat_8041e340;
			color.a = alpha;
			GXSetTevColor(1, color);
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
