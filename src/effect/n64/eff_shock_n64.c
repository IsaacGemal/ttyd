#include "eff/n64/eff_shock_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 constants referenced by the original.
extern const f32 float_1p1_8041fc68;
extern const f32 float_0p05_8041fc6c;
extern const f32 float_8_8041fc70;
extern const f32 float_0p9_8041fc74;
extern const f32 float_0p8_8041fc78;

// .rodata constants owned by this unit in the original.
const Vec vec3_802fb5c8 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb5d8 = 4503601774854144.0;

EffectEntry* effShockN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6) {
	EffectEntry* effect;
	EffShockData* data;

	effect = effEntry();
	effect->type = "ShockN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffShockData));
	effect->userdata = data;
	effect->callback = effShockMain;

	data->type = type;
	data->field_0x20 = 0;
	data->field_0x1C = 30;
	data->field_0x10 = 0xFF;
	data->field_0x14 = 0xFF;
	data->field_0x18 = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;

	if (type == 2) {
		data->field_0x24 = 1.0f;
	} else {
		data->field_0x24 = 0.0f;
	}

	data->field_0x28 = 0.5f * a5;
	data->field_0x2C = 0.5f * a6;

	switch (type) {
	case 0:
	case 1:
	case 3:
		data->field_0x30 = 0xFF;
		data->field_0x34 = 0xFF;
		data->field_0x38 = 0;
		data->field_0x3C = 0xFF;
		data->field_0x40 = 0xFF;
		data->field_0x44 = 0xFF;
		break;
	default:
		data->field_0x30 = 0x46;
		data->field_0x34 = 0xB4;
		data->field_0x38 = 0x78;
		data->field_0x3C = 0xFF;
		data->field_0x40 = 0xFF;
		data->field_0x44 = 0xFF;
		break;
	}

	return effect;
}

void effShockMain(EffectEntry* effect) {
	EffShockData* data;
	Vec position2;
	Vec position;
	s32 type;
	s32 life;
	s32 frame;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	type = data->type;
	if (data->field_0x1C < 100) {
		data->field_0x1C--;
	}
	data->field_0x20++;
	life = data->field_0x1C;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	frame = data->field_0x20;
	if (type == 2) {
		if (frame <= 20) {
			data->field_0x24 *= float_1p1_8041fc68;
		}
	} else {
		data->field_0x24 += float_0p05_8041fc6c * (float_8_8041fc70 - data->field_0x24);
	}

	data->field_0x14 = (s32)((f32)data->field_0x14 * float_0p9_8041fc74);
	if (life < 5) {
		data->field_0x18 = (s32)((f32)data->field_0x18 * float_0p8_8041fc78);
	}

	if (frame > 6) {
		data->field_0x10 -= 13;
		if (data->field_0x10 < 0) {
			data->field_0x10 = 0;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effShockDisp, effect);
}

#ifdef MUST_MATCH
/* MUST_MATCH: setting each colour inside a void inline (rather than assigning a local then passing it)
   reproduces the original's per-call by-value colour slots, local 0x18/copy 0x1c, 0x10/0x14, 0x08/0x0c
   (same shape as effGlassDisp). */
static inline void effShockSetColor1(EffShockData* data) {
	GXColor color = unk_80423258;
	color.r = data->field_0x30;
	color.g = data->field_0x34;
	color.b = data->field_0x38;
	color.a = data->field_0x18;
	GXSetTevColor(1, color);
}
static inline void effShockSetColor2(EffShockData* data) {
	GXColor color = unk_8042325c;
	color.r = data->field_0x3C;
	color.g = data->field_0x40;
	color.b = data->field_0x44;
	GXSetTevColor(2, color);
}
static inline void effShockSetColor3(EffShockData* data) {
	GXColor color = dat_8041fc50;
	color.a = data->field_0x14;
	GXSetTevColor(1, color);
}
#endif

void effShockDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	EffShockData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color1;
	GXColor color2;
	GXColor color3;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx viewMtx;
	s32 type;

	cam = camGetPtr(cameraId);
	data = effect->userdata;
	type = data->type;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, viewMtx);

#ifdef MUST_MATCH
	effShockSetColor1(data);
	effShockSetColor2(data);
#else
	color1 = unk_80423258;
	color1.r = data->field_0x30;
	color1.g = data->field_0x34;
	color1.b = data->field_0x38;
	color1.a = data->field_0x18;
	GXSetTevColor(1, color1);

	color2 = unk_8042325c;
	color2.r = data->field_0x3C;
	color2.g = data->field_0x40;
	color2.b = data->field_0x44;
	GXSetTevColor(2, color2);
#endif

	GXSetCullMode(0);

	if ((u32)(type - 1) <= 1 || type == 3) {
#ifdef MUST_MATCH
		if (type == 1) {
			effShockSetColor3(data);
		}
#else
		if (type == 1) {
			color3 = dat_8041fc50;
			color3.a = data->field_0x14;
			GXSetTevColor(1, color3);
		}
#endif

		PSMTXTrans(trans, 0.0f, data->field_0x2C, 0.0f);
		PSMTXScale(scale, 0.25f * data->field_0x24, 0.25f * data->field_0x24, 1.0f);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(viewMtx, trans, trans);
		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);
		GXSetNumChans(1);
		GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
		PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
		GXLoadTexMtxImm(scale, 0x1e, 1);
		effGetTexObjN64(0x85, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 4, 2, 0xA, 0xF);
		GXSetTevAlphaIn(0, 7, 1, 4, 7);

		effSetVtxDescN64((effVtxArray*)testsrc_shock_v);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x90);
		tri2(0x30, 0x31, 0x32, 0, 0x30, 0x32, 0x33, 0);
		tri2(0x31, 0x34, 0x35, 0, 0x31, 0x35, 0x32, 0);
		tri2(0x34, 0x36, 0x37, 0, 0x34, 0x37, 0x35, 0);
		tri2(0x38, 0x30, 0x33, 0, 0x38, 0x33, 0x39, 0);
		tri2(0x3A, 0x38, 0x39, 0, 0x3A, 0x39, 0x3B, 0);
		tri2(0x3C, 0x3A, 0x3B, 0, 0x3C, 0x3B, 0x3D, 0);
		tri2(0x3E, 0x3C, 0x3D, 0, 0x3E, 0x3D, 0x3F, 0);
		tri2(0x36, 0x40, 0x41, 0, 0x36, 0x41, 0x37, 0);
		tri2(0x42, 0x43, 0x44, 0, 0x42, 0x44, 0x45, 0);
		tri2(0x44, 0x46, 0x47, 0, 0x44, 0x47, 0x45, 0);
		tri2(0x48, 0x47, 0x46, 0, 0x48, 0x46, 0x49, 0);
		tri2(0x4A, 0x48, 0x49, 0, 0x4A, 0x49, 0x4B, 0);
		tri2(0x40, 0x4A, 0x4B, 0, 0x40, 0x4B, 0x41, 0);
		tri2(0x4C, 0x3E, 0x3F, 0, 0x4C, 0x3F, 0x4D, 0);
		tri2(0x4E, 0x4F, 0x57, 0, 0x4E, 0x57, 0x58, 0);
		tri2(0x4F, 0x50, 0x59, 0, 0x4F, 0x59, 0x57, 0);
		tri2(0x50, 0x52, 0x5A, 0, 0x50, 0x5A, 0x59, 0);
		tri2(0x51, 0x4E, 0x58, 0, 0x51, 0x58, 0x5B, 0);
		tri2(0x40, 0x51, 0x5B, 0, 0x40, 0x5B, 0x41, 0);
		tri2(0x52, 0x56, 0x5C, 0, 0x52, 0x5C, 0x5A, 0);
		tri2(0x53, 0x4C, 0x4D, 0, 0x53, 0x4D, 0x5D, 0);
		tri2(0x54, 0x53, 0x5D, 0, 0x54, 0x5D, 0x5E, 0);
		tri2(0x55, 0x54, 0x5E, 0, 0x55, 0x5E, 0x5F, 0);
		tri2(0x56, 0x55, 0x5F, 0, 0x56, 0x5F, 0x5C, 0);

		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 4, 2, 0xA, 0xF);
		GXSetTevAlphaIn(0, 7, 7, 7, 1);

		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x90);
		tri2(0x00, 0x01, 0x02, 0, 0x00, 0x02, 0x03, 0);
		tri2(0x01, 0x04, 0x05, 0, 0x01, 0x05, 0x02, 0);
		tri2(0x04, 0x06, 0x07, 0, 0x04, 0x07, 0x05, 0);
		tri2(0x08, 0x00, 0x03, 0, 0x08, 0x03, 0x09, 0);
		tri2(0x0A, 0x08, 0x09, 0, 0x0A, 0x09, 0x0B, 0);
		tri2(0x0C, 0x0A, 0x0B, 0, 0x0C, 0x0B, 0x0D, 0);
		tri2(0x0E, 0x0C, 0x0D, 0, 0x0E, 0x0D, 0x0F, 0);
		tri2(0x06, 0x10, 0x11, 0, 0x06, 0x11, 0x07, 0);
		tri2(0x12, 0x13, 0x14, 0, 0x12, 0x14, 0x15, 0);
		tri2(0x14, 0x16, 0x17, 0, 0x14, 0x17, 0x15, 0);
		tri2(0x18, 0x17, 0x16, 0, 0x18, 0x16, 0x19, 0);
		tri2(0x1A, 0x18, 0x19, 0, 0x1A, 0x19, 0x1B, 0);
		tri2(0x10, 0x1A, 0x1B, 0, 0x10, 0x1B, 0x11, 0);
		tri2(0x1C, 0x0E, 0x0F, 0, 0x1C, 0x0F, 0x1D, 0);
		tri2(0x1E, 0x1F, 0x27, 0, 0x1E, 0x27, 0x28, 0);
		tri2(0x1F, 0x20, 0x29, 0, 0x1F, 0x29, 0x27, 0);
		tri2(0x20, 0x22, 0x2A, 0, 0x20, 0x2A, 0x29, 0);
		tri2(0x21, 0x1E, 0x28, 0, 0x21, 0x28, 0x2B, 0);
		tri2(0x12, 0x21, 0x2B, 0, 0x12, 0x2B, 0x13, 0);
		tri2(0x22, 0x26, 0x2C, 0, 0x22, 0x2C, 0x2A, 0);
		tri2(0x23, 0x1C, 0x1D, 0, 0x23, 0x1D, 0x2D, 0);
		tri2(0x24, 0x23, 0x2D, 0, 0x24, 0x2D, 0x2E, 0);
		tri2(0x25, 0x24, 0x2E, 0, 0x25, 0x2E, 0x2F, 0);
		tri2(0x26, 0x25, 0x2F, 0, 0x26, 0x2F, 0x2C, 0);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const GXColor dat_8041fc50 = { 0xFF, 0xFF, 0x00, 0x00 };
const f32 float_1p1_8041fc68 = 1.1f;
const f32 float_0p05_8041fc6c = 0.05f;
const f32 float_8_8041fc70 = 8.0f;
const f32 float_0p9_8041fc74 = 0.9f;
const f32 float_0p8_8041fc78 = 0.8f;
