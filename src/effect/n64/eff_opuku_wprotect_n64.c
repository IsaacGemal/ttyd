#include "eff/n64/eff_opuku_wprotect_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "mario/mariost.h"
#include "memory.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804231f0;


#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include "math_ext.h"
#include <stdlib.h>

// .rodata scratch vector the original zeroes before filling in the effect position.
const Vec vec3_802fb2e8 = { 0.0f, 0.0f, 0.0f };
// The compiler's int->float magic double (0x4330000080000000) is intentionally not
// defined here: defining it duplicates the pool. Left out, the single pooled entry is
// emitted at the original's .rodata+0x10 offset and resolves to the original address.

// .sdata2 scratch/default tev colours (loaded whole, one channel overwritten).
const GXColor dat_8041f888 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041f88c = { 0x00, 0x00, 0xFF, 0x00 };
// .sbss2 scratch colour the second pass fills in; the reference reads it before writing.

// The 40-point ring the display pass renders: signed 1-byte XYZ (times 10),
// a 16-bit ST pair and RGB8.
typedef struct WprotectPoint {
	s8 x;   // 0x0
	s8 y;   // 0x1
	s8 z;   // 0x2
	u8 pad0; // 0x3
	s16 s;  // 0x4
	s16 t;  // 0x6
	u8 r;   // 0x8
	u8 g;   // 0x9
	u8 b;   // 0xA
	u8 pad1; // 0xB
} WprotectPoint; // 0xC

// One emitted N64 effect vertex: s16 XYZ, s16 ST, RGBA8 (14 bytes).
typedef struct WprotectVtx {
	s16 x; // 0x0
	s16 y; // 0x2
	s16 z; // 0x4
	s16 s; // 0x6
	s16 t; // 0x8
	u8 r;  // 0xA
	u8 g;  // 0xB
	u8 b;  // 0xC
	u8 a;  // 0xD
} WprotectVtx; // 0xE

// .data: the ring the display pass transforms; bytes copied verbatim from the original's .obj.
u8 point_data[0x1E0] = {
	0x19, 0x04, 0xE7, 0x00, 0x04, 0x01, 0x00, 0x48, 0xB0, 0xB0, 0xB0, 0x00,
	0x16, 0x00, 0xEA, 0x00, 0x03, 0xC3, 0x00, 0x00, 0xB0, 0xB0, 0xB0, 0x00,
	0xEA, 0x00, 0xEA, 0x00, 0x00, 0x3D, 0x00, 0x00, 0xB0, 0xB0, 0xB0, 0x00,
	0xE7, 0x04, 0xE7, 0x00, 0x00, 0x01, 0x00, 0x48, 0xB0, 0xB0, 0xB0, 0x00,
	0x19, 0x2E, 0xE7, 0x00, 0x04, 0x01, 0x03, 0xB7, 0xB0, 0xB0, 0xB0, 0x00,
	0xE7, 0x2E, 0xE7, 0x00, 0x00, 0x01, 0x03, 0xB7, 0xB0, 0xB0, 0xB0, 0x00,
	0x16, 0x32, 0xEA, 0x00, 0x03, 0x7A, 0x03, 0xE5, 0xAE, 0xD0, 0xD8, 0x00,
	0xEA, 0x32, 0xEA, 0x00, 0x00, 0x85, 0x03, 0xE5, 0xAE, 0xD0, 0xD8, 0x00,
	0xEA, 0x32, 0x16, 0x00, 0x00, 0x85, 0x03, 0xE5, 0xAE, 0xD0, 0xD8, 0x00,
	0x16, 0x32, 0x16, 0x00, 0x03, 0x7A, 0x03, 0xE5, 0xAE, 0xD0, 0xD8, 0x00,
	0xEA, 0x00, 0xEA, 0x00, 0x00, 0x85, 0x00, 0x19, 0x94, 0xA5, 0xB0, 0x00,
	0x16, 0x00, 0xEA, 0x00, 0x03, 0x7A, 0x00, 0x19, 0x94, 0xA5, 0xB0, 0x00,
	0x16, 0x00, 0x16, 0x00, 0x03, 0x7A, 0x00, 0x19, 0x94, 0xA5, 0xB0, 0x00,
	0xEA, 0x00, 0x16, 0x00, 0x00, 0x85, 0x00, 0x19, 0x94, 0xA5, 0xB0, 0x00,
	0xEA, 0x32, 0x16, 0x00, 0x03, 0xC3, 0x03, 0xFD, 0x78, 0x6E, 0x88, 0x00,
	0xEA, 0x32, 0xEA, 0x00, 0x00, 0x3D, 0x03, 0xFD, 0x78, 0x6E, 0x88, 0x00,
	0xE7, 0x2E, 0xE7, 0x00, 0x00, 0x00, 0x03, 0xB7, 0x78, 0x6E, 0x88, 0x00,
	0xE7, 0x2E, 0x19, 0x00, 0x04, 0x00, 0x03, 0xB7, 0x78, 0x6E, 0x88, 0x00,
	0xE7, 0x04, 0xE7, 0x00, 0x00, 0x00, 0x00, 0x48, 0x78, 0x6E, 0x88, 0x00,
	0xE7, 0x04, 0x19, 0x00, 0x04, 0x00, 0x00, 0x48, 0x78, 0x6E, 0x88, 0x00,
	0xEA, 0x00, 0xEA, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x78, 0x6E, 0x88, 0x00,
	0xEA, 0x00, 0x16, 0x00, 0x03, 0xC3, 0x00, 0x00, 0x78, 0x6E, 0x88, 0x00,
	0xEA, 0x32, 0xEA, 0x00, 0x00, 0x3D, 0x03, 0xFD, 0xB0, 0xB0, 0xB0, 0x00,
	0x16, 0x32, 0xEA, 0x00, 0x03, 0xC3, 0x03, 0xFD, 0xB0, 0xB0, 0xB0, 0x00,
	0x16, 0x00, 0x16, 0x00, 0x03, 0xC3, 0x00, 0x00, 0x78, 0x6E, 0x88, 0x00,
	0x16, 0x00, 0xEA, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x78, 0x6E, 0x88, 0x00,
	0x19, 0x04, 0xE7, 0x00, 0x00, 0x00, 0x00, 0x48, 0x78, 0x6E, 0x88, 0x00,
	0x19, 0x04, 0x19, 0x00, 0x04, 0x00, 0x00, 0x48, 0x78, 0x6E, 0x88, 0x00,
	0x19, 0x2E, 0xE7, 0x00, 0x00, 0x00, 0x03, 0xB7, 0x78, 0x6E, 0x88, 0x00,
	0x19, 0x2E, 0x19, 0x00, 0x04, 0x00, 0x03, 0xB7, 0x78, 0x6E, 0x88, 0x00,
	0x16, 0x32, 0xEA, 0x00, 0x00, 0x3D, 0x03, 0xFD, 0x78, 0x6E, 0x88, 0x00,
	0xEA, 0x00, 0x16, 0x00, 0x00, 0x3D, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00,
	0x19, 0x04, 0x19, 0x00, 0x04, 0x01, 0x00, 0x48, 0xF0, 0xF0, 0xF0, 0x00,
	0x19, 0x2E, 0x19, 0x00, 0x04, 0x01, 0x03, 0xB7, 0xF0, 0xF0, 0xF0, 0x00,
	0x16, 0x32, 0x16, 0x00, 0x03, 0xC3, 0x03, 0xFD, 0x78, 0x6E, 0x88, 0x00,
	0x16, 0x00, 0x16, 0x00, 0x03, 0xC3, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00,
	0xE7, 0x04, 0x19, 0x00, 0x00, 0x01, 0x00, 0x48, 0xF0, 0xF0, 0xF0, 0x00,
	0xE7, 0x2E, 0x19, 0x00, 0x00, 0x01, 0x03, 0xB7, 0xF0, 0xF0, 0xF0, 0x00,
	0x16, 0x32, 0x16, 0x00, 0x03, 0xC3, 0x03, 0xFD, 0xF0, 0xF0, 0xF0, 0x00,
	0xEA, 0x32, 0x16, 0x00, 0x00, 0x3D, 0x03, 0xFD, 0xF0, 0xF0, 0xF0, 0x00,
};

// .data: the small quad the four drops draw; local to this file in the original.
static u32 size8x8_tex32x32_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004, 0xFFFC0000, 0x04000000, 0x000000FF,
	0x00040004, 0x00000400, 0x04000000, 0x00FFFFFC, 0x00040000, 0x00000400, 0x000000FF,
};

// Inline copy of system.c's helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fb2e8;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// Local view of the userdata tail: five float arrays of 4 and a counter array of 4,
// all interleaved (stride 4 bytes), which the original walks with a single moving base.
typedef struct WprotectData {
	s32 type;           // 0x00
	f32 x;              // 0x04
	f32 y;              // 0x08
	f32 z;              // 0x0C
	s32 field_0x10;     // 0x10
	s32 field_0x14;     // 0x14
	s32 field_0x18;     // 0x18
	s32 field_0x1C;     // 0x1C
	s32 field_0x20;     // 0x20
	s32 field_0x24;     // 0x24
	s32 field_0x28;     // 0x28
	f32 scale;          // 0x2C
	f32 field_0x30;     // 0x30
	f32 field_0x34;     // 0x34
	f32 field_0x38[4];  // 0x38
	f32 field_0x48[4];  // 0x48
	f32 field_0x58[4];  // 0x58
	f32 field_0x68[4];  // 0x68
	f32 field_0x78[4];  // 0x78
	s32 field_0x88[4];  // 0x88
} WprotectData; // 0x98
EffectEntry* effOpukuWprotectN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuWprotectData* data;

	effect = effEntry();
	effect->type = "OpukuWprotectN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x98);
	effect->userdata = data;
	effect->callback = effOpukuWprotectMain;
	effect->flags |= 2;

	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = duration;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0x2F;
	data->field_0x1C = 0x7F;
	data->field_0x20 = 0xFF;
	data->field_0x30 = 0.0f;
	data->field_0x34 = 0.0f;
	data->field_0x88 = -1;
	data->field_0x8C = -5;
	data->field_0x90 = -9;
	data->field_0x94 = -0xD;

	return effect;
}
// 0x98
void effOpukuWprotectMain(EffectEntry* effect) {
	WprotectData* data = effect->userdata;
	Vec pos = initializeVec((Vec*)&data->x);
	s32 type = data->type;
	s32 f14;
	s32 i;
	f32 t;
	f32 s;
	f32 sin1;
	f32 sin2;
	f32 a1;
	f32 cos1;
	f32 cos2;

	if (effect->flags & 4) {
		if (type == 1) {
			effect->flags &= ~4;
			data->field_0x10 = 4;
		} else {
			effect->flags &= ~4;
			data->field_0x10 = 0x10;
		}
	}
	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;
	if (data->field_0x14 > 0x4F1A0) {
		data->field_0x14 = 0x100;
	}

	if (data->field_0x10 < 0) {
		if (type == 1) {
			effWaterDamageN64Entry(0, 30, data->x, 24.0f + data->y, data->z, 2.0f);
			effWaterDamageN64Entry(1, 30, data->x, 24.0f + data->y, data->z, 2.0f);
		}
		effDelete(effect);
		return;
	}

	f14 = data->field_0x14;
	if (type == 0) {
		if (data->field_0x10 < 0x10) {
			s32 v = data->field_0x10 << 4;
			data->field_0x24 = v;
			data->field_0x28 = v;
		}
		if (f14 < 0x10) {
			data->field_0x24 = (f14 << 4) + 0xf;
			data->field_0x28 = (f14 << 4) + 0xf;
		}
	} else {
		if (f14 < 4) {
			data->field_0x24 = (f14 << 6) + 0x3f;
			data->field_0x28 = (f14 << 6) + 0x3f;
		}
	}

#ifdef MUST_MATCH
	// Original emits the multiply as `f14*6 + f14` (mulli 6; add) instead of a single *7.
	s = (f32)sin(6.2831855f * (f32)(f14 * 6 + f14) / 360.0f);
#else
	s = (f32)sin(6.2831855f * (f32)(f14 * 7) / 360.0f);
#endif
	data->field_0x1C = (u8)(s32)(32.0f * s + 127.0f);
	t = 0.01f * (f32)f14;
	a1 = 6.2831855f * t / 360.0f;
	sin1 = (f32)sin(a1);
	sin2 = (f32)sin(6.2831855f * (6.0f * t) / 360.0f);
	cos1 = (f32)cos(6.2831855f * (2.0f * t) / 360.0f);
	cos2 = (f32)cos(a1);
	{
		f32 sin2s = 32.0f * sin2;
		data->field_0x30 = 256.0f
		    + sin2s * sin1
		    + 32.0f * cos1
		    + 32.0f * cos2;
	}
	data->field_0x34 += 0.2f;
	if (data->field_0x34 >= 128.0f) {
		data->field_0x34 = 0.0f;
	}

	for (i = 0; i < 4; i++) {
		data->field_0x88[i]++;
		if (data->field_0x88[i] < 0) {
			continue;
		}
		if (data->field_0x88[i] == 0) {
			data->field_0x38[i] = 0.0f;
			data->field_0x48[i] = 240.0f;
			data->field_0x58[i] = 0.2f * (f32)(rand() % 11 - 5);
			data->field_0x68[i] = (f32)(rand() % 11 + 2);
			data->field_0x78[i] = (f32)(rand() % 11 + 1);
		}
		data->field_0x38[i] += data->field_0x58[i];
		data->field_0x48[i] += data->field_0x68[i];
		data->field_0x58[i] *= 0.97f;
		data->field_0x68[i] += 0.1f * (10.0f - data->field_0x68[i]);
		if (data->field_0x48[i] > 480.0f) {
			data->field_0x88[i] = -(rand() % 11) - 1;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effOpukuWprotectDisp, effect);
}

// The display pass: build the 40-point ring from point_data, draw the four drops,
// then two indexed passes over the built vertices. Every coordinate conversion is
// integer magic (fctiwz + lfd/fsubs), so no inline asm is needed.
#ifdef MUST_MATCH
// Build each colour and make the GXSetTevColor call inside an inline: mwcc then places
// each colour local and its by-value copy in the original's stack slots. Returning the
// GXColor and calling outside packs all locals together instead.
static inline void wprotectColor1(s32 a) {
	GXColor c = dat_8041f888;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}

static inline void wprotectColor2(s32 a) {
	GXColor c = dat_8041f88c;
	c.a = a;
	GXSetTevColor(GX_TEVREG1, c);
}

static inline void wprotectColor3(WprotectData* data, s32 a) {
	GXColor c = unk_804231f0;
	c.r = data->field_0x18;
	c.g = data->field_0x1C;
	c.b = data->field_0x20;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}
#endif

void effOpukuWprotectDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	WprotectData* data = effect->userdata;
	s32 i;
	s32 alpha;
	WprotectVtx* vtx;
	WprotectVtx* p;
	WprotectPoint* point;
	GXTexObj texObj;
	Mtx drop;
	Mtx scale;
	Mtx trans;
#ifndef MUST_MATCH
	GXColor color1;
	GXColor color2;
	GXColor color3;
#endif
	s32 counter;
	f32 s1;
	f32 px;
	f32 py;
	f32 pz;
	f32 s2;
	f32 s3;
	f32 d1;
	f32 d2;
	f32 d3;
	f32 t2;

	alpha = data->field_0x24;
	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXConcat(cam->view, trans, trans);

	p = (WprotectVtx*)smartAlloc(0x460, HEAP_EFFECT)->address;
	vtx = p;
	point = (WprotectPoint*)point_data;
	counter = gp->retraceCount >> 1;

	for (i = 0; i < 40; i++) {
		px = (f32)(point->x * 10);
		py = (f32)(point->y * 10);
		pz = (f32)(point->z * 10);
		if (0.0f != py) {
			s1 = (f32)sin((f32)(counter + (s32)effTblRandN64(1000, (s32)(100.0f * pz + (10.0f * py + px)))));
			t2 = 100.0f * py;
			s2 = (f32)sin((f32)(counter + (s32)effTblRandN64(1000, (s32)(t2 + 10.0f * px + pz))));
			s3 = (f32)sin((f32)(counter + (s32)effTblRandN64(1000, (s32)(10.0f * pz + (100.0f * px + py)))));
			d1 = 20.0f * s1;
			d2 = 20.0f * s2;
			d3 = 20.0f * s3;
			px += d1;
			py += d2;
			pz += d3;
		}
		p->x = (s16)px;
		p->y = (s16)py;
		p->z = (s16)pz;
		p->s = point->s;
		p->t = point->t;
		p->r = point->r;
		p->g = point->g;
		p->b = point->b;
		p->a = 0x50;
		p++;
		point++;
	}

#ifdef MUST_MATCH
	wprotectColor1(alpha);
#else
	color1 = dat_8041f888;
	color1.a = alpha;
	GXSetTevColor(GX_TEVREG0, color1);
#endif
#ifdef MUST_MATCH
	wprotectColor2(data->field_0x28);
#else
	color2 = dat_8041f88c;
	color2.a = data->field_0x28;
	GXSetTevColor(GX_TEVREG1, color2);
#endif
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	effGetTexObjN64(0x91, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);

	for (i = 0; i < 4; i++) {
		if (data->field_0x88[i] >= 0) {
			PSMTXScale(scale, 0.1f, 0.1f, 0.1f);
			PSMTXTrans(drop, data->field_0x38[i], data->field_0x48[i], 0.0f);
			PSMTXConcat(scale, drop, drop);
			PSMTXScale(scale, data->field_0x78[i], data->field_0x78[i], data->field_0x78[i]);
			PSMTXConcat(drop, scale, drop);
			PSMTXConcat(trans, drop, drop);
			GXLoadPosMtxImm(drop, 0);
			GXSetCurrentMtx(0);
			effSetVtxDescN64((effVtxArray*)size8x8_tex32x32_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
		}
	}

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A1, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	effGetTexObjN64(0x90, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scale, 0.0625f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
#ifdef MUST_MATCH
	wprotectColor3(data, (s32)(0.3f * (f32)alpha));
#else
	color3 = unk_804231f0;
	color3.r = data->field_0x18;
	color3.g = data->field_0x1C;
	color3.b = data->field_0x20;
	color3.a = (s32)(0.3f * (f32)alpha);
	GXSetTevColor(GX_TEVREG0, color3);
#endif
	GXSetCullMode(GX_CULL_FRONT);

	DCFlushRange(vtx, 0x460);
	GXInvalidateVtxCache();
	PSMTXScale(scale, 0.1f, 0.1f, 0.1f);
	PSMTXConcat(trans, scale, scale);
	GXLoadPosMtxImm(scale, 0);
	GXSetCurrentMtx(0);
	effSetVtxDescN64((effVtxArray*)vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 60);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
	tri2(4, 0, 3, 0, 4, 3, 5, 0);
	tri2(6, 7, 8, 0, 6, 8, 9, 0);
	tri2(10, 11, 12, 0, 10, 12, 13, 0);
	tri2(14, 15, 16, 0, 14, 16, 17, 0);
	tri2(16, 18, 19, 0, 16, 19, 17, 0);
	tri2(18, 20, 21, 0, 18, 21, 19, 0);
	tri2(22, 23, 4, 0, 22, 4, 5, 0);
	tri2(24, 25, 26, 0, 24, 26, 27, 0);
	tri2(26, 28, 29, 0, 26, 29, 27, 0);
	effSetVtxDescN64((effVtxArray*)((u8*)vtx + 0x188));
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 24);
	tri2(0, 2, 6, 0, 0, 6, 1, 0);
	tri2(3, 7, 4, 0, 3, 4, 8, 0);
	tri2(4, 5, 9, 0, 4, 9, 8, 0);
	tri2(5, 10, 11, 0, 5, 11, 9, 0);
}

