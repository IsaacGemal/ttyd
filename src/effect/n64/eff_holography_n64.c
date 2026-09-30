#include "eff/n64/eff_holography_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2
extern const f32 float_deg2rad_8041f138;
extern const f32 float_2_8041f13c;
extern const f32 float_1_8041f140;
extern const f32 float_0p1_8041f144;
extern f32 float_0p03125_8041f14c; // non-const: a const declaration lets the compiler hoist it into f26
extern const f32 float_0_8041f150;

// .data
extern u8 ach_01_v[];
// .sdata
u8 xlu_data[16] = {
	0xFF, 0xFF, 0xDC, 0xBE, 0xA0, 0x78, 0x50, 0x28,
	0x00, 0x00, 0x28, 0x50, 0x78, 0xA0, 0xBE, 0xDC,
};
// .sbss2
GXColor unk_80423148;
GXColor unk_8042314c;

EffectEntry* effHolographyN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffHolographyData* data;

	effect = effEntry();
	effect->type = "HolographyN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffHolographyData));
	effect->userdata = data;
	effect->callback = effHolographyMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (count <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = count;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = float_0p1_8041f144 * scale;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	return effect;
}

void effHolographyMain(EffectEntry* effect) {
	EffHolographyData* data;
	s32 timer;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 16;
	}

	timer = data->field_0x10;
	if (timer < 1000) {
		data->field_0x10 = timer - 1;
	}
	data->field_0x14++;
	timer = data->field_0x10;

	if (timer < 0) {
		effDelete(effect);
		return;
	}

	if (data->field_0x14 < 16) {
		data->field_0x24 = data->field_0x14 * 16 + 15;
	} else {
		data->field_0x24 = 0xFF;
	}
	if (timer < 16) {
		data->field_0x24 = timer * 16;
	}

	dispEntry(4, dispCalcZ(&pos2), 2, effHolographyDisp, effect);
}

static inline void effHolographySetColor2(s32 frame) {
	GXColor c = unk_80423148;
	c.a = xlu_data[frame % 16];
	GXSetTevColor(2, c);
}

static inline void effHolographySetColor1(EffHolographyData* data, f32 alpha) {
	GXColor c = unk_8042314c;
	c.r = data->field_0x18;
	c.g = data->field_0x1C;
	c.b = data->field_0x20;
	c.a = alpha;
	GXSetTevColor(1, c);
}

void effHolographyDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffHolographyData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	s32 frame;
	s32 alphaBase;
	s32 vtxIndex;
	s32 i;
	f32 alpha;

	cam = camGetPtr(camId);
	data = effect->userdata;
	frame = data->field_0x14;
	alphaBase = data->field_0x24;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f138 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scaleMtx, -data->field_0x28, data->field_0x28, data->field_0x28);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXSetNumChans(0);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 1, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 8, 0, 5, 0xf);
	GXSetTevAlphaIn(1, 4, 0, 1, 7);
	GXSetTevOrder(2, 1, 1, 0xff);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xf, 0xf, 0xf, 0);
	GXSetTevAlphaIn(2, 7, 0, 1, 7);
	effGetTexObjN64(0x93, &texObj);
	GXLoadTexObj(&texObj, 0);
	effGetTexObjN64(0x94, &texObj);
	GXLoadTexObj(&texObj, 1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x1e, 0, 0x7d);

	vtxIndex = 0x36 - frame;
	if (vtxIndex < 0) {
		vtxIndex = 0;
	}
	effSetVtxDescN64((effVtxArray*)(ach_01_v + vtxIndex * 0x134));

	effHolographySetColor2(frame);

	for (i = 0; i < 10; i++) {
		alpha = alphaBase * (float_1_8041f140 - float_0p1_8041f144 * i) * float_2_8041f13c;
		if (alpha > 255.0f) {
			alpha = 255.0f;
		}
		effHolographySetColor1(data, alpha);
		PSMTXScale(scaleMtx, float_0p03125_8041f14c, float_0p03125_8041f14c, float_0_8041f150);
		GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
		GXBegin(0x90, 0, 6);
		switch (i) {
		case 0: tri2(0, 1, 2, 0, 0, 2, 3, 0); break;
		case 1: tri2(1, 4, 5, 0, 1, 5, 2, 0); break;
		case 2: tri2(4, 6, 7, 0, 4, 7, 5, 0); break;
		case 3: tri2(6, 8, 9, 0, 6, 9, 7, 0); break;
		case 4: tri2(8, 10, 11, 0, 8, 11, 9, 0); break;
		case 5: tri2(10, 12, 13, 0, 10, 13, 11, 0); break;
		case 6: tri2(12, 14, 15, 0, 12, 15, 13, 0); break;
		case 7: tri2(14, 16, 17, 0, 14, 17, 15, 0); break;
		case 8: tri2(16, 18, 19, 0, 16, 19, 17, 0); break;
		case 9: tri2(18, 20, 21, 0, 18, 21, 19, 0); break;
		}
	}
}
