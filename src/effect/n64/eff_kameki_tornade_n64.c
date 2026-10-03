#include "eff/n64/eff_kameki_tornade_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>
#include "eff/eff_n64.h"
#include "matching.h"
#include "math_ext.h"
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSFastCast.h>
#include <string.h>

typedef struct effKamekiTornadeData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	s32 field_0x10;      // 0x10
	s32 field_0x14;      // 0x14
	s32 field_0x18;      // 0x18
	s32 field_0x1C;      // 0x1C
	s32 field_0x20;      // 0x20
	s32 field_0x24;      // 0x24
	s32 field_0x28;      // 0x28
	s32 field_0x2C;      // 0x2C
	s32 field_0x30;      // 0x30
	f32 scale;           // 0x34
	f32 field_0x38[8];   // 0x38
	f32 field_0x58[8];   // 0x58
	f32 field_0x78[8];   // 0x78
	f32 field_0x98[8];   // 0x98
	f32 field_0xB8[8];   // 0xB8
	f32 field_0xD8[8];   // 0xD8
	f32 field_0xF8[8];   // 0xF8
	f32 field_0x118[8];  // 0x118
	f32 field_0x138;     // 0x138
	f32 field_0x13C;     // 0x13C
	f32 field_0x140;     // 0x140
	f32 field_0x144;     // 0x144
	f32 field_0x148;     // 0x148
} effKamekiTornadeData;

// .sdata2 float constants (named after their original addresses).
extern const f32 float_4_8041f208;
extern const f32 float_5_8041f204;
extern const f32 float_10_8041f1f4;
extern const f32 float_12p96_8041f214;
extern const f32 float_17p28_8041f218;
extern const f32 float_64_8041f21c;
extern const f32 float_90_8041f20c;
extern const f32 float_neg90_8041f210;
extern const f32 float_0p1_8041f1ec;
extern const f32 float_0_8041f1f8;
extern const f32 float_1_8041f220;
extern const f32 float_0p2_8041f224;
extern const f32 float_2_8041f228;
extern const char str_802FA870[];
const f64 double_to_int_802fa868 = 4503601774854144.0;
EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effKamekiTornadeData* data;
	s32 i;

	effect = effEntry();
	effect->type = str_802FA870;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKamekiTornadeData));
	effect->userdata = data;
	effect->callback = effKamekiTornadeMain;
	effect->flags |= 2;
	data->type = a;
	data->field_0x14 = 0;
	if (b <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = b;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0xFF;
	data->field_0x1C = 0xFF;
	data->field_0x20 = 0xFF;
	data->field_0x28 = 0xFF;
	data->field_0x2C = 0xFF;
	data->field_0x30 = 0xEB;
	for (i = 0; i < 8; i++) {
		data->field_0x38[i] = x;
		data->field_0x58[i] = y;
		data->field_0x78[i] = z;
		data->field_0x98[i] = float_0p2_8041f224 * i + float_1_8041f220;
		data->field_0xB8[i] = rand() % 360;
		data->field_0xD8[i] = float_0p1_8041f1ec * (rand() % 100) + float_2_8041f228;
		data->field_0xF8[i] = float_0p1_8041f1ec * (rand() % 30);
		data->field_0x118[i] = float_0_8041f1f8;
	}
	data->field_0x138 = float_0_8041f1f8;
	data->field_0x144 = float_0_8041f1f8;
	data->field_0x13C = float_0_8041f1f8;
	data->field_0x140 = float_0_8041f1f8;
	data->field_0x148 = float_1_8041f220;
	return effect;
}

// With common-subexpression elimination on, MWCC reuses the loop temporaries for the 4.0/90/-90
// constants and permutes every FPR in the loop and the 0x138/0x13C/0x140 rotation block; turning it
// off keeps retail's f4/f1/f0 constants and f2/f3 temporaries.
#pragma opt_common_subs off
void effKamekiTornadeMain(EffectEntry* effect) {
	effKamekiTornadeData* data;
	Vec position;
	Vec disp = { 0 };
	s32 i;
	s32 timer;
	f32 scale;
	f32 f5;

	data = effect->userdata;
	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 0x10;
	}
	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;
	if (data->field_0x10 < 0) {
		effDelete(effect);
		return;
	}
	timer = data->field_0x14;
	scale = data->scale;
	if (data->field_0x10 < 0x10) {
		data->field_0x24 = data->field_0x10 << 4;
	}
	if (timer < 0x10) {
		data->field_0x24 = (timer << 4) + 0xF;
	}
	data->field_0x38[0] = data->x;
	data->field_0x58[0] = data->y;
	data->field_0x78[0] = data->z;
	f5 = float_5_8041f204 * scale;
	for (i = 7; i != 0; i--) {
		data->field_0x118[i] = float_4_8041f208 * (data->field_0x38[i - 1] - data->field_0x38[i]);
		data->field_0x38[i] = data->field_0x38[i - 1];
		data->field_0x58[i] = data->field_0x58[i - 1] + f5;
		data->field_0x78[i] = data->field_0x78[i - 1];
		if (data->field_0x118[i] > float_90_8041f20c) {
			data->field_0x118[i] = float_90_8041f20c;
		}
		else if (data->field_0x118[i] < float_neg90_8041f210) {
			data->field_0x118[i] = float_neg90_8041f210;
		}
	}
	data->field_0x138 += float_10_8041f1f4;
	data->field_0x13C += float_12p96_8041f214;
	data->field_0x140 += float_17p28_8041f218;
	if (data->field_0x13C > float_64_8041f21c) {
		data->field_0x13C -= float_64_8041f21c;
	}
	if (data->field_0x140 > float_64_8041f21c) {
		data->field_0x140 -= float_64_8041f21c;
	}
	for (i = 0; i < 8; i++) {
		data->field_0xB8[i] += data->field_0xD8[i];
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKamekiTornadeDisp, effect);
}
#pragma opt_common_subs reset
// Build the two by-value GXColors in helpers so MWCC gives their argument copies the same stack
// slots as the original (the trick from effBattenDisp2/effTimestampSetColor).
#ifdef MUST_MATCH
// MUST_MATCH: building the colour and calling GXSetTevColor inside the inline is what puts the
// build/copy temps at the original's 0x28/0x2c and 0x20/0x24 slots; returning a GXColor from the
// helper puts the first helper's build temp 4 bytes low (same shape as eff_fireworks_n64).
static inline void kamekiTornadeSetColor(effKamekiTornadeData* data, s32 field24) {
	GXColor color = unk_80423160;

	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = (u8)(s32)((f32)field24 * data->field_0x148);
	GXSetTevColor(1, color);
}
static inline void kamekiTornadeSetColor2(effKamekiTornadeData* data) {
	GXColor color = *(GXColor*)&dat_8041f1d8;

	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	GXSetTevColor(2, color);
}
#else
static inline GXColor kamekiTornadeSetColor(effKamekiTornadeData* data, s32 field24) {
	GXColor color = unk_80423160;

	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = (u8)(s32)((f32)field24 * data->field_0x148);
	return color;
}
static inline GXColor kamekiTornadeSetColor2(effKamekiTornadeData* data) {
	GXColor color = *(GXColor*)&dat_8041f1d8;

	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	return color;
}
#endif
#define KT_ZERO MATCH_F32(float_0_8041f1f8)
// One 14-byte vertex: copy the source, then rotate its s16 position through the matrix with the
// SDK fast-cast inlines. Retail inlines this once per vertex, each with its own Vec temp and its
// own GQR2-5 setup (same shape as eff_flame_n64/eff_sstar_n64).
static inline void kamekiTornadeSetVtx(KamekiTornadeVtx* dst, KamekiTornadeVtx* src, Mtx mtx) {
	Vec v;
	memcpy(dst, src, sizeof(KamekiTornadeVtx));
#ifdef MUST_MATCH
	OSInitFastCast();
	OSs16tof32(&dst->pos[0], &v.x);
	OSs16tof32(&dst->pos[1], &v.y);
	OSs16tof32(&dst->pos[2], &v.z);
	PSMTXMultVec(mtx, &v, &v);
	OSInitFastCast();
	OSf32tos16(&v.x, &dst->pos[0]);
	OSf32tos16(&v.y, &dst->pos[1]);
	OSf32tos16(&v.z, &dst->pos[2]);
#else
	v.x = dst->pos[0];
	v.y = dst->pos[1];
	v.z = dst->pos[2];
	PSMTXMultVec(mtx, &v, &v);
	dst->pos[0] = v.x;
	dst->pos[1] = v.y;
	dst->pos[2] = v.z;
#endif
}
// Every instruction, constant and stack slot matches retail; the remaining diffs are register
// colouring only. The ring loop's constants land one FPR higher than retail (f30-f27 vs f29-f26),
// which pushes the outer loop's i2f double (f27 vs f26), its `10` (f20 vs f27) and the vertex
// helper's conversion temps (f26-f24 vs f25-f23) one register high. The ring preheader also
// initialises the strength-reduced `t`/angle accumulators later than retail (`li r20,0; li r27,0`
// instead of retail's `li r27,0; mr r20,r27`).
// NONMATCHING (99.14%): only register colouring differs.
void effKamekiTornadeDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	CameraEntry* cam;
	effKamekiTornadeData* data;
	s32 life;
	KamekiTornadeVtx* tbl;
	KamekiTornadeVtx* vtx;
	GXTexObj texObj;
	Mtx work;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx baseMtx;
	Vec axis;
	f32 scale;
	f32 f19;
	s32 i;
	s32 j;
	s32 field24;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	field24 = data->field_0x24 >> 1;
	life = data->field_0x10;
	scale = data->scale;

	GXSetNumChans(0);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 1, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 8, 0, 0, 0xF);
	GXSetTevAlphaIn(1, 7, 0, 4, 7);
	GXSetTevOrder(2, 0xFF, 0xFF, 0xFF);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 2, 4, 0, 0xF);
	GXSetTevAlphaIn(2, 7, 0, 1, 7);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	effGetTexObjN64(0x6C, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXLoadTexObj(&texObj, 1);

#ifdef MUST_MATCH
	kamekiTornadeSetColor(data, field24);

	kamekiTornadeSetColor2(data);
#else
	GXSetTevColor(1, kamekiTornadeSetColor(data, field24));

	GXSetTevColor(2, kamekiTornadeSetColor2(data));
#endif

	GXSetCullMode(0);

	tbl = smartAlloc(0x1C0, 3)->address;

	{
		KamekiTornadeVtx* q;
		s32 s;
		s32 t;

		for (i = 0, s = 0, t = 0; i < 16; i++, s += 0x80, t += 0x200) {
			KamekiTornadeVtx* p = &tbl[i];
			f32 angle =
			    float_6p2832_8041f1dc * (f32)(i * 0x168 / 0xF) / float_360_8041f1e0;

			p->pos[0] = (s16)((f32)cos(angle) * float_100_8041f1e4);
			p->pos[1] = 0;
			p->pos[2] = (s16)((f32)sin(angle) * float_100_8041f1e4);
			p->tex[0] = s;
			p->tex[1] = 0;
			{
				q = &tbl[i + 16];

				q->pos[0] = (s16)((f32)cos(angle) * float_100_8041f1e4);
				q->pos[1] = 0;
				q->pos[2] = (s16)((f32)sin(angle) * float_100_8041f1e4);
				q->tex[0] = t;
				q->tex[1] = 0x400;
			}
		}
	}

	PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041f1e8 * data->field_0x144);
	PSMTXScale(baseMtx, float_0p1_8041f1ec, float_0p1_8041f1ec, float_0p1_8041f1ec);
	PSMTXConcat(rotMtx, baseMtx, baseMtx);

	f19 = float_0p02_8041f1f0 * (f32)(0x7F - field24);

	for (i = 0; i < 7; i++) {
		vtx = smartAlloc(0x1C0, 3)->address;
		effSetVtxDescN64((effVtxArray*)vtx);

		/* first half: the 0..15 ring, rotated about vec3_802fa84c */
		PSMTXTrans(work, float_10_8041f1f4 * data->field_0x38[i], float_10_8041f1f4 * data->field_0x58[i],
		           float_10_8041f1f4 * data->field_0x78[i]);
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041f1e8 * data->field_0x118[i]);
		PSMTXConcat(work, rotMtx, work);
		axis = vec3_802fa84c;
		PSMTXRotAxisRad(rotMtx, &axis, float_deg2rad_8041f1e8 * (data->field_0x138 + (f32)(i * i)));
		PSMTXConcat(work, rotMtx, work);
		{
			f32 zScale = data->field_0x98[i] * scale;

			if (life < 0x10) {
				zScale += f19;
			}
			PSMTXScale(scaleMtx, zScale, scale, zScale);
		}
		PSMTXConcat(work, scaleMtx, work);
		{
			f32 s;
			f32 theta;
			f32 rad;
			f32 c;

			rad = float_10_8041f1f4 * data->field_0xF8[i];
			theta = float_6p2832_8041f1dc * data->field_0xB8[i] / float_360_8041f1e0;
			s = (f32)sin(theta);
			c = (f32)cos(theta);

			PSMTXTrans(scaleMtx, rad * s, KT_ZERO, rad * c);
		}
		PSMTXConcat(work, scaleMtx, work);
		PSMTXConcat(baseMtx, work, work);
		for (j = 0; j < 16; j++) {
			kamekiTornadeSetVtx(&vtx[j], &tbl[j], work);
		}

		/* second half: the 16..31 ring, rotated about vec3_802fa858 */
		PSMTXTrans(work, float_10_8041f1f4 * data->field_0x38[i + 1],
		           float_10_8041f1f4 * data->field_0x58[i + 1],
		           float_10_8041f1f4 * data->field_0x78[i + 1]);
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041f1e8 * data->field_0x118[i + 1]);
		PSMTXConcat(work, rotMtx, work);
		axis = vec3_802fa858;
		PSMTXRotAxisRad(rotMtx, &axis, float_deg2rad_8041f1e8 * (data->field_0x138 + (f32)(i * i)));
		PSMTXConcat(work, rotMtx, work);
		{
			f32 zScale = data->field_0x98[i + 1] * scale;

			PSMTXScale(scaleMtx, zScale, scale, zScale);
		}
		PSMTXConcat(work, scaleMtx, work);
		{
			f32 rad = float_10_8041f1f4 * data->field_0xF8[i];
			f32 s = (f32)sin(float_6p2832_8041f1dc * data->field_0xB8[i] / float_360_8041f1e0);
			f32 c = (f32)cos(float_6p2832_8041f1dc * data->field_0xB8[i] / float_360_8041f1e0);

			PSMTXTrans(scaleMtx, rad * s, KT_ZERO, rad * c);
		}
		PSMTXConcat(work, scaleMtx, work);
		PSMTXConcat(baseMtx, work, work);
		for (j = 16; j < 32; j++) {
			kamekiTornadeSetVtx(&vtx[j], &tbl[j], work);
		}

		GXInvalidateVtxCache();
		DCFlushRange(vtx, 0x1C0);

		PSMTXScale(scaleMtx, float_0p015625_8041f1fc, float_0p03125_8041f200, KT_ZERO);
		{
#ifdef MUST_MATCH
			/* MUST_MATCH: the literal zero reproduces retail's one-load-plus-`fmr` and keeps the
			   function's scratch FPRs at retail's colours; the named float_0_8041f1f8 shifts them. */
			f32 z = 0.0f;
#else
			f32 z = float_0_8041f1f8;
#endif

			PSMTXTrans(work, data->field_0x13C + (f32)(i * 4), z, z);
		}
		PSMTXConcat(scaleMtx, work, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, 0x1E, 1);

		PSMTXScale(scaleMtx, float_0p015625_8041f1fc, float_0p03125_8041f200, KT_ZERO);
		{
#ifdef MUST_MATCH
			f32 z = 0.0f;
#else
			f32 z = float_0_8041f1f8;
#endif

			PSMTXTrans(work, data->field_0x140 + (f32)(i * 2), z, z);
		}
		PSMTXConcat(scaleMtx, work, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, 0x21, 1);

		GXLoadPosMtxImm(cam->view, 0);
		GXSetCurrentMtx(0);

		for (j = 0; j < 0xF; j++) {
			GXBegin(0x90, 0, 6);
			tri2(j + 0x10, j + 1, j, 0, j + 1, j + 0x10, j + 0x11, 1);
		}
	}
}

