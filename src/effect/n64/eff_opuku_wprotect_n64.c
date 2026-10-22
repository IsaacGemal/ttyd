#include "eff/n64/eff_opuku_wprotect_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "mario/mariost.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include "math_ext.h"
#include <stdlib.h>

extern const f32 float_0_8041f890;
extern const f32 float_100_8041f894;
extern const f32 float_10_8041f898;
extern const f32 float_20_8041f89c;
extern const f32 float_0p03125_8041f8a0;
extern const f32 float_0p0625_8041f8a8;
extern const f32 float_0p3_8041f8ac;
extern const f32 float_0p1_8041f8a4;
extern const f32 float_24_8041f8b0;
extern const f32 float_2_8041f8b4;
extern const f32 float_6p2832_8041f8b8;
extern const f32 float_360_8041f8bc;

// The compiler's int->float magic double (0x4330000080000000) is intentionally not
// defined here: defining it duplicates the pool. Left out, the single pooled entry is
// emitted at the original's .rodata+0x10 offset and resolves to the original address.

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

// .data: the small quad the four bubbles draw; local to this file in the original.
static u32 size8x8_tex32x32_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004, 0xFFFC0000, 0x04000000, 0x000000FF,
	0x00040004, 0x00000400, 0x04000000, 0x00FFFFFC, 0x00040000, 0x00000400, 0x000000FF,
};

// Inline copy of system.c's helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

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
	data->timer = 0;
	if (duration <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = duration;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->colorR = 0x2F;
	data->colorG = 0x7F;
	data->colorB = 0xFF;
	data->field_0x30 = float_0_8041f890;
	data->field_0x34 = float_0_8041f890;
	data->bubbleTimer[0] = -1;
	data->bubbleTimer[1] = -5;
	data->bubbleTimer[2] = -9;
	data->bubbleTimer[3] = -0xD;

	return effect;
}
// 0x98
void effOpukuWprotectMain(EffectEntry* effect) {
	effOpukuWprotectData* data = effect->userdata;
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
			data->lifetime = 4;
		} else {
			effect->flags &= ~4;
			data->lifetime = 0x10;
		}
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->timer > 0x4F1A0) {
		data->timer = 0x100;
	}

	if (data->lifetime < 0) {
		if (type == 1) {
			effWaterDamageN64Entry(0, 30, data->x, float_24_8041f8b0 + data->y, data->z, float_2_8041f8b4);
			effWaterDamageN64Entry(1, 30, data->x, float_24_8041f8b0 + data->y, data->z, float_2_8041f8b4);
		}
		effDelete(effect);
		return;
	}

	f14 = data->timer;
	if (type == 0) {
		if (data->lifetime < 0x10) {
			s32 v = data->lifetime << 4;
			data->alpha = v;
			data->baseAlpha = v;
		}
		if (f14 < 0x10) {
			data->alpha = (f14 << 4) + 0xf;
			data->baseAlpha = (f14 << 4) + 0xf;
		}
	} else {
		if (f14 < 4) {
			data->alpha = (f14 << 6) + 0x3f;
			data->baseAlpha = (f14 << 6) + 0x3f;
		}
	}

	// Written as `f14*6 + f14` so it compiles to mulli 6; add instead of a single *7.
	s = (f32)sin(float_6p2832_8041f8b8 * (f32)(f14 * 6 + f14) / float_360_8041f8bc);
	data->colorG = (u8)(s32)(32.0f * s + 127.0f);
	t = 0.01f * (f32)f14;
	a1 = float_6p2832_8041f8b8 * t / float_360_8041f8bc;
	sin1 = (f32)sin(a1);
	sin2 = (f32)sin(float_6p2832_8041f8b8 * (6.0f * t) / float_360_8041f8bc);
	cos1 = (f32)cos(float_6p2832_8041f8b8 * (float_2_8041f8b4 * t) / float_360_8041f8bc);
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
		data->field_0x34 = float_0_8041f890;
	}

	for (i = 0; i < 4; i++) {
		data->bubbleTimer[i]++;
		if (data->bubbleTimer[i] < 0) {
			continue;
		}
		if (data->bubbleTimer[i] == 0) {
			data->bubbleX[i] = float_0_8041f890;
			data->bubbleY[i] = 240.0f;
			data->bubbleVelX[i] = 0.2f * (f32)(rand() % 11 - 5);
			data->bubbleVelY[i] = (f32)(rand() % 11 + 2);
			data->bubbleScale[i] = (f32)(rand() % 11 + 1);
		}
		data->bubbleX[i] += data->bubbleVelX[i];
		data->bubbleY[i] += data->bubbleVelY[i];
		data->bubbleVelX[i] *= 0.97f;
		data->bubbleVelY[i] += float_0p1_8041f8a4 * (float_10_8041f898 - data->bubbleVelY[i]);
		if (data->bubbleY[i] > 480.0f) {
			data->bubbleTimer[i] = -(rand() % 11) - 1;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effOpukuWprotectDisp, effect);
}

// The display pass: build the 40-point ring from point_data, draw the four bubbles,
// then two indexed passes over the built vertices. Every coordinate conversion is
// integer magic (fctiwz + lfd/fsubs), so no inline asm is needed.
void effOpukuWprotectDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effOpukuWprotectData* data = effect->userdata;
	s32 i;
	s32 alpha;
	WprotectVtx* vtx;
	WprotectVtx* p;
	WprotectPoint* point;
	GXTexObj texObj;
	Mtx bubble;
	Mtx scale;
	Mtx trans;
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

	alpha = data->alpha;
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
		if (float_0_8041f890 != py) {
			s1 = (f32)sin((f32)(counter + effTblRandN64(1000, (s32)(float_100_8041f894 * pz + (float_10_8041f898 * py + px)))));
			t2 = float_100_8041f894 * py;
			s2 = (f32)sin((f32)(counter + effTblRandN64(1000, (s32)(t2 + float_10_8041f898 * px + pz))));
			s3 = (f32)sin((f32)(counter + effTblRandN64(1000, (s32)(float_10_8041f898 * pz + (float_100_8041f894 * px + py)))));
			d1 = float_20_8041f89c * s1;
			d2 = float_20_8041f89c * s2;
			d3 = float_20_8041f89c * s3;
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

	GXSetTevColor(GX_TEVREG0, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
	GXSetTevColor(GX_TEVREG1, (GXColor){ 0x00, 0x00, 0xFF, data->baseAlpha });
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
	PSMTXScale(scale, float_0p03125_8041f8a0, float_0p03125_8041f8a0, float_0_8041f890);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);

	for (i = 0; i < 4; i++) {
		if (data->bubbleTimer[i] >= 0) {
			PSMTXScale(scale, float_0p1_8041f8a4, float_0p1_8041f8a4, float_0p1_8041f8a4);
			PSMTXTrans(bubble, data->bubbleX[i], data->bubbleY[i], float_0_8041f890);
			PSMTXConcat(scale, bubble, bubble);
			PSMTXScale(scale, data->bubbleScale[i], data->bubbleScale[i], data->bubbleScale[i]);
			PSMTXConcat(bubble, scale, bubble);
			PSMTXConcat(trans, bubble, bubble);
			GXLoadPosMtxImm(bubble, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			effSetVtxDescN64((effVtxArray*)size8x8_tex32x32_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
		}
	}

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
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
	PSMTXScale(scale, float_0p0625_8041f8a8, float_0p03125_8041f8a0, float_0_8041f890);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	GXSetTevColor(GX_TEVREG0, (GXColor){ data->colorR, data->colorG, data->colorB, (s32)(float_0p3_8041f8ac * (f32)alpha) });
	GXSetCullMode(GX_CULL_FRONT);

	DCFlushRange(vtx, 0x460);
	GXInvalidateVtxCache();
	PSMTXScale(scale, float_0p1_8041f8a4, float_0p1_8041f8a4, float_0p1_8041f8a4);
	PSMTXConcat(trans, scale, scale);
	GXLoadPosMtxImm(scale, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
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
	effSetVtxDescN64((effVtxArray*)(vtx + 28));
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 24);
	tri2(0, 2, 6, 0, 0, 6, 1, 0);
	tri2(3, 7, 4, 0, 3, 4, 8, 0);
	tri2(4, 5, 9, 0, 4, 9, 8, 0);
	tri2(5, 10, 11, 0, 5, 11, 9, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0_8041f890 = 0.0f;
const f32 float_100_8041f894 = 100.0f;
const f32 float_10_8041f898 = 10.0f;
const f32 float_20_8041f89c = 20.0f;
const f32 float_0p03125_8041f8a0 = 0.03125f;
const f32 float_0p1_8041f8a4 = 0.1f;
const f32 float_0p0625_8041f8a8 = 0.0625f;
const f32 float_0p3_8041f8ac = 0.3f;
const f32 float_24_8041f8b0 = 24.0f;
const f32 float_2_8041f8b4 = 2.0f;
const f32 float_6p2832_8041f8b8 = 6.2831855f;
const f32 float_360_8041f8bc = 360.0f;
