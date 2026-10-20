#include "eff/n64/eff_thunderflare_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804232ac;
__declspec(section ".sdata") GXColor unk_804232a8;


// .sdata2 data symbols owned by this unit.
extern const f32 float_deg2rad_80420080;
extern const f32 float_0p03125_80420084;
extern const f32 float_0_80420088;
// .rodata objects owned by this unit.
const Vec vec3_802fb890 = { 0.0f, 0.0f, 0.0f };
// The pooled int->float conversion double, named so the `lfd` reloc resolves like the original.
// .sbss2 data symbols owned by this unit.

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
	s32 tevReg0R;   // 0x30, GX_TEVREG0: stage 1's colour where the texture is dark (alpha from `alpha`)
	s32 tevReg0G;   // 0x34
	s32 tevReg0B;   // 0x38
	s32 tevReg1R;   // 0x3C, GX_TEVREG1: stage 1's colour where the texture is bright
	s32 tevReg1G;   // 0x40
	s32 tevReg1B;   // 0x44
} EffThunderflareData;

// The element index-to-value expression must read `i + j - 1`, not `i - 1 + j`:
// the former lets MWCC fold the second iteration back to `i` and reuse r10, the
// latter makes it keep a copy of the pre-increment `i` in r11.
EffectEntry* effThunderflareN64Entry(s32 type, s32 timer, f32 x, f32 y, f32 z, f32 unused) {
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
	if (timer <= 0) {
		data->timer = 100;
	} else {
		data->timer = timer;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;

	switch (type) {
	case 0:
		data->tevReg0R = 0xff;
		data->tevReg0G = 0;
		data->tevReg0B = 0xff;
		data->tevReg1R = 0xff;
		data->tevReg1G = 0xff;
		data->tevReg1B = 0xff;
		break;
	case 1:
	default:
		data->tevReg0R = 0xff;
		data->tevReg0G = 0x14;
		data->tevReg0B = 0x21;
		data->tevReg1R = 0x32;
		data->tevReg1G = 0x14;
		data->tevReg1B = 0x0a;
		break;
	}

	for (i = 1; i < 0x1f; i += 2) {
		elem = &data[i];
		for (j = 0; j < 2; j++, elem++) {
			s32 particle = i + j - 1;
			elem->angle1 = (f32)(particle * 0xca8 / 30);
			elem->angle2 = float_0_80420088;
			elem->radius = (f32)(particle * 100 / 30);
			elem->field_0x00 = 0;
		}
	}
	return entry;
}

static inline void flareSetTevReg1(s32 r, s32 g, s32 b) {
	GXColor color = unk_804232a8;
	color.r = r;
	color.g = g;
	color.b = b;
	GXSetTevColor(GX_TEVREG1, color);
}

static inline void flareSetTevReg0(s32 r, s32 g, s32 b, s32 a) {
	GXColor color = unk_804232ac;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

// Builds the effect position from a zero vector; kept as an inline so the
// temporary schedules the way the original's does.
static inline Vec thunderflareMakeVec(f32 x, f32 y, f32 z) {
	Vec temp = vec3_802fb890;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

#pragma opt_common_subs off // without it MWCC CSEs across the sin/cos calls and picks different callee-saved FPRs (100 lands in f24, not f28)
void effThunderflareMain(EffectEntry* effect) {
	EffThunderflareData* data;
	Vec pos;
	s32 phase;
	s32 i;
	s32 counter;
#ifdef MUST_MATCH
	// MWCC assigns FPRs partly by declaration order; this order plus the
	// angle2raw temp reproduce the original's f18-f23 allocation.
	f32 sin1;
	f32 cos1;
	f32 sin2;
	f32 cos2;
	f32 angle2;
	f32 angle2raw;
	f32 radius;
	f32 angle1;
#endif

	data = effect->userdata;
	pos = thunderflareMakeVec(data->x, data->y, data->z);

	if (data->timer < 100) {
		data->timer--;
	}
	data->counter++;
	if (data->timer < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	if (counter <= 20) {
		data->alpha = counter * 12;
	} else {
		data->alpha = 0xff;
	}
	if (counter & 1) {
		data->alpha = (s32)(float_0p5_8042008c * (f32)data->alpha);
	}

	phase = data->counter & 1;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
#ifdef MUST_MATCH
		// Loading angle2 early (angle2raw) schedules its 2*pi multiply into the
		// sin/cos call's latency slot, as in the original.
		angle1 = float_6p2832_80420090 * data->angle1 / float_360_80420094;
		radius = data->radius;
		angle2raw = data->angle2;
		sin1 = sin(angle1);
		cos1 = cos(angle1);
		angle2 = float_6p2832_80420090 * angle2raw / float_360_80420094;
		sin2 = sin(angle2);
		cos2 = cos(angle2);
#else
		f32 angle1 = float_6p2832_80420090 * data->angle1 / float_360_80420094;
		f32 radius = data->radius;
		f32 angle2 = float_6p2832_80420090 * data->angle2 / float_360_80420094;
		f32 sin1 = sin(angle1);
		f32 cos1 = cos(angle1);
		f32 sin2 = sin(angle2);
		f32 cos2 = cos(angle2);
#endif

		data->x = cos2 * (radius * sin1);
		data->y = cos2 * (radius * cos1);
		data->z = radius * sin2;
		data->alpha = (s32)((float_100_804200a0 - data->radius) * float_255_8042009c * float_2_80420098 / float_100_804200a0);
		if (data->alpha > 0xff) {
			data->alpha = 0xff;
		}
		data->scale = float_0p2_804200a4 + data->radius / float_80_804200a8;
		data->radius -= (f32)(2 + (i & 3) * 2);
		if (data->radius < float_0_80420088) {
			data->radius = float_0_80420088;
			data->scale = float_0_80420088;
		}
		data->angle1 += phase ? 5 : 2;
		data->angle2 += phase ? 2 : 5;
	}
	dispEntry(4, dispCalcZ(&pos), 2, effThunderflareDisp, effect);
}
#pragma opt_common_subs reset
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
	r = data->tevReg0R;
	g = data->tevReg0G;
	b = data->tevReg0B;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_80420080 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, base);

	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);

	flareSetTevReg1(data->tevReg1R, data->tevReg1G, data->tevReg1B);

	data++;
	effGetTexObjN64(0x55, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(work, float_0p03125_80420084, float_0p03125_80420084, float_0_80420088);
	GXLoadTexMtxImm(work, 0x1E, 1);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

	for (i = 1; i < effect->count; i++, data++) {
		scale = data->scale;
		if (scale != float_0_80420088) {
			flareSetTevReg0(r, g, b, data->alpha);

			PSMTXTrans(trans, data->x, data->y, data->z);
			PSMTXScale(work, scale, scale, scale);
			PSMTXConcat(trans, work, trans);
			PSMTXConcat(base, trans, trans);
			GXLoadPosMtxImm(trans, 0);
			GXSetCurrentMtx(0);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			idx0 = (i & 1) * 4;
			idx1 = idx0 + 1;
			idx2 = idx0 + 2;
			idx3 = idx0 + 3;
			tri2(idx0, idx1, idx2, 0, idx0, idx2, idx3, 0);
		}
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_80420080 = 0.017453292f;
const f32 float_0p03125_80420084 = 0.03125f;
const f32 float_0_80420088 = 0.0f;
const f32 float_0p5_8042008c = 0.5f;
const f32 float_6p2832_80420090 = 6.2831855f;
const f32 float_360_80420094 = 360.0f;
const f32 float_2_80420098 = 2.0f;
const f32 float_255_8042009c = 255.0f;
const f32 float_100_804200a0 = 100.0f;
const f32 float_0p2_804200a4 = 0.2f;
const f32 float_80_804200a8 = 80.0f;
