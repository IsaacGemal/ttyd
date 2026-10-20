#include "eff/eff_toge_flush.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

typedef struct TogeFlushData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	f32 angle;           // 0x10, degrees
	f32 angleSpeed;      // 0x14, degrees per frame
	f32 innerScale;      // 0x18
	f32 innerScaleSpeed; // 0x1C
	f32 outerScale;      // 0x20
	s32 innerAlpha;      // 0x24
	s32 outerAlpha;      // 0x28
	s32 life;            // 0x2C
	s32 frame;           // 0x30
} TogeFlushData;

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423070;


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

#ifdef MUST_MATCH
// MUST_MATCH: non-const (pinned to .rodata, as kpa_act.c does) so MWCC assumes the store to
// data->outerAlpha may alias vec3_802f8e80 and leaves its three lwz after that store, as retail
// schedules them. A plain `const Vec` makes the scheduler hoist them above the second fctiwz.
__declspec(section ".rodata") Vec vec3_802f8e80 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802f8e80 = { 0.0f, 0.0f, 0.0f };
#endif

EffectEntry* effTogeFlushEntry(s32 type, f32 x, f32 y, f32 z, s32 life) {
	EffectEntry* entry;
	TogeFlushData* data;

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
	TogeFlushData* data = effect->userdata;
	s32 frame;
	Vec pos;
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

	{
		pos = vec3_802f8e80;
		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		dispEntry(4, dispCalcZ(&pos), 2, effTogeFlushDisp, effect);
	}
}

#ifdef MUST_MATCH
/* MUST_MATCH: building the colour inside an inline that also issues the GXSetTevColor call makes MWCC
   allocate the temp and the by-value argument copy together per call site (r1+0x40/0x44, 0x38/0x3c, ...),
   as retail does. A helper that only returns the colour (the usual pattern) leaves them in separate slots. */
static inline void togeSetColor0(s32 a, s32 idx) {
	GXColor c = *(GXColor*)&unk_80423070;
	c.a = a;
	c.r = color_rotation_data[idx + 0];
	c.g = color_rotation_data[idx + 1];
	c.b = color_rotation_data[idx + 2];
	GXSetTevColor(1, c);
}

static inline void togeSetColor1(s32 a) {
	GXColor c = dat_8041e328;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void togeSetColor2(s32 a) {
	GXColor c = dat_8041e32c;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void togeSetColor3(s32 a) {
	GXColor c = dat_8041e330;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void togeSetColor4(s32 a) {
	GXColor c = dat_8041e334;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void togeSetColorA3(s32 a) {
	GXColor c = dat_8041e338;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void togeSetColorA4(s32 a) {
	GXColor c = dat_8041e33c;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void togeSetColorAD(s32 a) {
	GXColor c = dat_8041e340;
	c.a = a;
	GXSetTevColor(1, c);
}
#endif

void effTogeFlushDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	TogeFlushData* data = effect->userdata;
#ifndef MUST_MATCH
	GXColor color;
#endif
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

#ifdef MUST_MATCH
	switch (type) {
	case 0:
		idx %= 24;
		togeSetColor0(colorAlpha, idx);
		break;
	case 1:
		togeSetColor1(colorAlpha);
		break;
	case 2:
		togeSetColor2(colorAlpha);
		break;
	case 3:
		togeSetColor3(colorAlpha);
		break;
	case 4:
		togeSetColor4(colorAlpha);
		break;
	}
#else
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
#endif

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

#ifdef MUST_MATCH
		switch (type) {
		case 3:
			togeSetColorA3(alpha);
			break;
		case 4:
			togeSetColorA4(alpha);
			break;
		default:
			togeSetColorAD(alpha);
			break;
		}
#else
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
#endif

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
