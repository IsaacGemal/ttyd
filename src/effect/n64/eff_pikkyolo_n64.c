#include "eff/n64/eff_pikkyolo_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

const Vec vec3_802fb370 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb380 = 4503601774854144.0;
const char str_802FB388[12] = "PikkyoloN64";

GXColor unk_80423200;
GXColor unk_80423204;

static u32 size16x16_tex16x16_vtx[0xE0 / 4] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x02000000, 0x000000FF, 0x00080008,
	0x00000200, 0x02000000, 0x00FFFFF8, 0x00080000,
	0x00000200, 0x000000FF,
	0xFFF8FFF8, 0x00000000, 0x02000000, 0x00FF0008,
	0xFFF80000, 0x02000200, 0x000000FF, 0x00080008,
	0x00000200, 0x04000000, 0x00FFFFF8, 0x00080000,
	0x00000400, 0x000000FF,
	0xFFF8FFF8, 0x00000000, 0x04000000, 0x00FF0008,
	0xFFF80000, 0x02000400, 0x000000FF, 0x00080008,
	0x00000200, 0x06000000, 0x00FFFFF8, 0x00080000,
	0x00000600, 0x000000FF,
	0xFFF8FFF8, 0x00000000, 0x06000000, 0x00FF0008,
	0xFFF80000, 0x02000600, 0x000000FF, 0x00080008,
	0x00000200, 0x08000000, 0x00FFFFF8, 0x00080000,
	0x00000800, 0x000000FF,
};

EffectEntry* effPikkyoloN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effPikkyoloN64Data* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FB388;
	entry->count = 6;
	data = __memAlloc(HEAP_EFFECT, sizeof(effPikkyoloN64Data) * 6);
	entry->userdata = data;
	entry->callback = effPikkyoloMain;
	entry->flags |= 2;
	data->field_0x0 = type;
	data->timer = 0;
	if (life <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = life;
	}
	data->a = 0xff;

	for (i = 0; i < entry->count; i++, data++) {
		data->position.x = x;
		data->position.y = y;
		data->position.z = z;
		data->velocityX = 0.5f * (f32)(rand() % 11 - 5);
		data->velocityY = 0.8f * (f32)(rand() % 6 + 1);
		data->velocityZ = 0.5f * (f32)(rand() % 10 - 5);
		data->scale = scale;
		data->r = data->r2 = rand() % 256;
		data->g = data->g2 = rand() % (0xff - data->r2);
		data->b = data->b2 = 0xff - data->r2 - data->g2;
		data->r += 0xa8;
		data->g += 0xa8;
		data->b += 0xa8;
		if (data->r > 0xff) {
			data->r = 0xff;
		}
		if (data->g > 0xff) {
			data->g = 0xff;
		}
		if (data->b > 0xff) {
			data->b = 0xff;
		}
		data->r2 *= 0.6f;
		data->g2 *= 0.6f;
		data->b2 *= 0.6f;
		data->angle = (f32)(rand() % 360);
	}
	return entry;
}

void effPikkyoloMain(EffectEntry* effect) {
	Vec position;
	Vec disp = { 0.0f, 0.0f, 0.0f };
	effPikkyoloN64Data* data = effect->userdata;
	s32 i;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->lifetime < 8) {
		data->a = data->lifetime << 5;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->velocityY -= 0.5f;
		data->position.x += data->velocityX;
		data->position.y += data->velocityY;
		data->position.z += data->velocityZ;
		if (data->position.y < 0.0f && data->velocityY < 0.0f) {
			data->position.y = 0.0f;
			data->velocityY *= -0.7f;
		}
		data->angle += (f32)(((i & 1) * 2 - 1) * 14);
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPikkyoloDisp, effect);
}

static inline void effPikkyoloSetColor1(effPikkyoloN64Data* data, s32 alpha) {
	GXColor color = unk_80423200;
	color.r = data->r;
	color.g = data->g;
	color.b = data->b;
	color.a = alpha;
	GXSetTevColor(1, color);
}

static inline void effPikkyoloSetColor2(effPikkyoloN64Data* data) {
	GXColor color = unk_80423204;
	color.r = data->r2;
	color.g = data->g2;
	color.b = data->b2;
	color.a = data->a2;
	GXSetTevColor(2, color);
}

void effPikkyoloDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effPikkyoloN64Data* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	s32 i;
	s32 alpha;
	s32 base;
	f32 scale;

	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->a;
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 4, 2, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	effGetTexObjN64(0x41, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scaleMtx, 0.0625f, 0.015625f, 0.0f);
	GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex16x16_vtx);

	for (i = 0; i < effect->count; i++, data++) {
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		scale = 0.5f * data->scale;
		PSMTXScale(scaleMtx, scale, scale, scale);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXRotRad(rotMtx, 'z', data->angle * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(cam->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		effPikkyoloSetColor1(data, alpha);
		effPikkyoloSetColor2(data);
		GXBegin(0x90, 0, 6);
		base = (i & 3) * 4;
		tri2(base, base + 1, base + 2, base, base, base + 2, base + 3, base);
	}
}
