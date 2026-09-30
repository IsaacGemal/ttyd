#include "eff/n64/eff_thunderflare_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 data symbols owned by this unit.
extern const f32 float_deg2rad_80420080;
extern const f32 float_0p03125_80420084;
extern const f32 float_0_80420088;
// .sbss2 data symbols owned by this unit.
extern GXColor unk_804232a8;
extern GXColor unk_804232ac;

// .data: retail 16x16 quad vertex array (local to this unit).
static u32 size16x16_tex32x32_vtx[28] = {
	0xFFF8FFF8, 0x00000000, 0x0000FAFA, 0xFAFF0008,
	0xFFF80000, 0x04000000, 0xFAFAFAFF, 0x00080008,
	0x00000400, 0x0400FA00, 0x00FFFFF8, 0x00080000,
	0x00000400, 0xFA0000FF, 0xFFF4FFF4, 0x0000FE00,
	0x0200FAFA, 0xFAFF000C, 0xFFF40000, 0x0200FE00,
	0xFAFAFAFF, 0x000C000C, 0x00000600, 0x0200FA00,
	0x00FFFFF4, 0x000C0000, 0x02000600, 0xFA0000FF,
};

typedef struct EffThunderflareData {
	s32 field_0x00; // 0x00
	s32 type;       // 0x04
	f32 x;          // 0x08
	f32 y;          // 0x0C
	f32 z;          // 0x10
	s32 alpha;      // 0x14
	s32 timer;      // 0x18
	s32 counter;    // 0x1C
	f32 angle1;     // 0x20
	f32 angle2;     // 0x24
	f32 scale;      // 0x28
	f32 radius;     // 0x2C
	s32 colorR;     // 0x30
	s32 colorG;     // 0x34
	s32 colorB;     // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
} EffThunderflareData;

// NONMATCHING (78.74%): instructions differ; see check.py
EffectEntry* effThunderflareN64Entry(s32 type, s32 a2, f32 x, f32 y, f32 z, f32 a6) {
	EffectEntry* entry;
	EffThunderflareData* data;
	EffThunderflareData* elem;
	s32 i;
	s32 j;

	entry = effEntry();
	entry->type = "ThunderflareN64";
	entry->count = 0x1f;
	data = __memAlloc(HEAP_EFFECT, 0x8b8);
	entry->userdata = data;
	entry->callback = effThunderflareMain;
	data->type = type;
	data->counter = 0;
	if (a2 <= 0) {
		data->timer = 100;
	} else {
		data->timer = a2;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;

	switch (type) {
	case 0:
		data->colorR = 0xff;
		data->colorG = 0;
		data->colorB = 0xff;
		data->field_0x3C = 0xff;
		data->field_0x40 = 0xff;
		data->field_0x44 = 0xff;
		break;
	case 1:
	default:
		data->colorR = 0xff;
		data->colorG = 0x14;
		data->colorB = 0x21;
		data->field_0x3C = 0x32;
		data->field_0x40 = 0x14;
		data->field_0x44 = 0x0a;
		break;
	}

	for (i = 1; i < 0x1f; i += 2) {
		elem = &data[i];
		for (j = 0; j < 2; j++, elem++) {
			s32 n = j == 0 ? i - 1 : i;
			elem->angle1 = (f32)(n * 0xca8 / 30);
			elem->angle2 = 0.0f;
			elem->radius = (f32)(n * 100 / 30);
			elem->field_0x00 = 0;
		}
	}
	return entry;
}

static inline void flareSetColor1(s32 r, s32 g, s32 b) {
	GXColor color = unk_804232a8;
	color.r = r;
	color.g = g;
	color.b = b;
	GXSetTevColor(2, color);
}

static inline void flareSetColor2(s32 r, s32 g, s32 b, s32 a) {
	GXColor color = unk_804232ac;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(1, color);
}

// NONMATCHING (96.73%): the four tri2 arguments differ. include/eff/eff_n64.h declares tri2's parameters as
// u16, so the compiler zero-extends each `idx0 + k` with an extra clrlwi; the original passes them without
// truncation, i.e. its prototype was int/s32. That one header line (u16 -> s32) makes this 100.00%.
void effThunderflareDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	EffThunderflareData* data;
	GXTexObj texObj;
	Mtx trans;
	Mtx work;
	Mtx rot;
	Mtx base;
	s32 r;
	s32 g;
	s32 b;
	s32 i;
	s32 idx0;
	s32 idx1;
	s32 idx2;
	s32 idx3;
	f32 scale;

	cam = camGetPtr(camId);
	data = effect->userdata;
	r = data->colorR;
	g = data->colorG;
	b = data->colorB;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_80420080 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, base);

	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 8, 8, 0xF);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 0xFF, 0xFF, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 2, 4, 0, 0xF);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);

	flareSetColor1(data->field_0x3C, data->field_0x40, data->field_0x44);

	data++;
	effGetTexObjN64(0x55, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(work, float_0p03125_80420084, float_0p03125_80420084, float_0_80420088);
	GXLoadTexMtxImm(work, 0x1E, 1);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

	for (i = 1; i < effect->count; i++, data++) {
		scale = data->scale;
		if (scale != float_0_80420088) {
			flareSetColor2(r, g, b, data->alpha);

			PSMTXTrans(trans, data->x, data->y, data->z);
			PSMTXScale(work, scale, scale, scale);
			PSMTXConcat(trans, work, trans);
			PSMTXConcat(base, trans, trans);
			GXLoadPosMtxImm(trans, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x90, 0, 6);
			idx0 = (i & 1) * 4;
			idx1 = idx0 + 1;
			idx2 = idx0 + 2;
			idx3 = idx0 + 3;
			tri2(idx0, idx1, idx2, 0, idx0, idx2, idx3, 0);
		}
	}
}
