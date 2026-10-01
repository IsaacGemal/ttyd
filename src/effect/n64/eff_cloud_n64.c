#include "eff/n64/eff_cloud_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <stdlib.h>
#include <string.h>

int __float_nan[];
#define NAN (*(f32*)__float_nan)

// .sdata2 constants owned by this unit; declared extern here and defined after the functions so the
// front end references the symbols by name instead of folding the literals into fresh pool entries.
extern const f32 float_0p015625_8041eb98;
extern const f32 float_0p03125_8041eb9c;
extern const f32 float_0_8041eba0;
extern const f32 float_6p2832_8041eba4;
extern const f32 float_360_8041eba8;
extern const f32 float_255_8041ebac;
extern const f32 float_180_8041ebb0;
extern const f32 float_neg180_8041ebb4;
extern const f32 float_0p5_8041ebb8;
extern const f32 float_16_8041ebbc;
extern const f32 float_3_8041ebc0;
extern const f32 float_24_8041ebc4;
extern const f32 float_0p8_8041ebc8;
extern const f32 float_0p4_8041ebcc;

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804230c4;
__declspec(section ".sdata") GXColor unk_804230c0;

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
		case 0x7F800000:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_NAN;
			} else {
				return FP_INFINITE;
			}
			break;
		case 0:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_SUBNORMAL;
			} else {
				return FP_ZERO;
			}
			break;
		default:
			return FP_NORMAL;
	}
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
	if (x > 0.0f) {
		f64 guess = __frsqrte(x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

EffectEntry* effCloudN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effCloudData* data;
	s32 i;

	effect = effEntry();
	effect->type = "CloudN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effCloudData));
	effect->userdata = data;
	effect->callback = effCloudMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	switch (type) {
		case 0:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xff;
			data->field_0x20 = 0xff;
			data->field_0x28 = 0xe1;
			data->field_0x2C = 0xe1;
			data->field_0x30 = 0xe1;
			data->field_0x24 = 0xff;
			break;
		case 1:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xf5;
			data->field_0x20 = 0xe6;
			data->field_0x28 = 0x2d;
			data->field_0x2C = 0x23;
			data->field_0x30 = 0x0f;
			data->field_0x24 = 0xff;
			break;
		case 2:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0;
			data->field_0x20 = 0;
			data->field_0x28 = 0x9b;
			data->field_0x2C = 0x7d;
			data->field_0x30 = 0x7d;
			data->field_0x24 = 0xff;
			break;
		case 3:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xff;
			data->field_0x20 = 0xff;
			data->field_0x28 = 0xff;
			data->field_0x2C = 0x41;
			data->field_0x30 = 0x14;
			data->field_0x24 = 0xff;
			break;
		default:
			break;
	}
	data->field_0x38 = x;
	data->field_0x3C = y;
	data->field_0x40 = z;
#ifdef MUST_MATCH
	// The original unrolls this zeroing loop at the source level.
	data->field_0x230[0] = 0;
	data->field_0x230[1] = 0;
	data->field_0x230[2] = 0;
	data->field_0x230[3] = 0;
	data->field_0x230[4] = 0;
	data->field_0x230[5] = 0;
	data->field_0x230[6] = 0;
	data->field_0x230[7] = 0;
	data->field_0x230[8] = 0;
	data->field_0x230[9] = 0;
	data->field_0x230[10] = 0;
	data->field_0x230[11] = 0;
	data->field_0x230[12] = 0;
	data->field_0x230[13] = 0;
	data->field_0x230[14] = 0;
	data->field_0x230[15] = 0;
	data->field_0x230[16] = 0;
	data->field_0x230[17] = 0;
	data->field_0x230[18] = 0;
	data->field_0x230[19] = 0;
	data->field_0x230[20] = 0;
	data->field_0x230[21] = 0;
	data->field_0x230[22] = 0;
	data->field_0x230[23] = 0;
	data->field_0x230[24] = 0;
	data->field_0x230[25] = 0;
	data->field_0x230[26] = 0;
	data->field_0x230[27] = 0;
	data->field_0x230[28] = 0;
	data->field_0x230[29] = 0;
	data->field_0x2A8 = 0;
#else
	for (i = 0; i < 30; i++) {
		data->field_0x230[i] = 0;
	}
	data->field_0x2A8 = 0;
#endif
	data->field_0x50[0] = x;
	data->field_0xC8[0] = y;
	data->field_0x140[0] = z;
	data->field_0x230[0] = 1;
	data->field_0x1B8[0] = 0;
	data->field_0x2AC[0] = 0.0f;
	data->field_0x44 = (f32)(rand() % 30 + 10);
	data->field_0x48 = 0.0f;
	data->field_0x4C = 0.0f;
	return effect;
}

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// effCloudDisp is emitted first in the original (last in this file), which seeds the .rodata pool
// with vec3 then the compiler-generated double_to_int used by both functions' int->float
// conversions. The outer `if (dist)` (not `dist != 0.0f`) gets MWCC to emit the original's
// `fcmpu cr0, f4, f0`, and the literal `0.0f` in the inlined sqrtf keeps the original's FPR
// allocation in the ring-buffer block (px/py/pz f3/f4/f5, dist accumulator f1, zero f0).
void effCloudMain(EffectEntry* effect) {
	effCloudData* data = effect->userdata;
	Vec position;
#ifdef MUST_MATCH
	Vec position2;
#endif
	s32 type;
	s32 counter;

	position = makeVec(data->x, data->y, data->z);
#ifdef MUST_MATCH
	position2 = position;
#endif
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->field_0x14++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	counter = data->field_0x14;
	if (data->lifetime < 16) {
		data->field_0x24 = data->lifetime << 4;
	}

	if (type == 3) {
		data->field_0x44 += (f32)(rand() % 10 - 5);
		data->field_0x48 += (f32)(rand() % 10 - 5);
		data->field_0x38 += data->field_0x44;
		data->field_0x3C += data->field_0x48;
		data->field_0x40 += data->field_0x4C;
	}

	{
		s32 index = data->field_0x2A8 % 30;
		f32 ox = data->field_0x50[index];
		f32 oy = data->field_0xC8[index];
		f32 oz = data->field_0x140[index];
		f32 px = data->field_0x38;
		f32 py = data->field_0x3C;
		f32 pz = data->field_0x40;

		if (px != ox || py != oy || pz != oz) {
			f32 len = data->field_0x2AC[index];
			f32 dist;
			f32 dx;
			f32 dy;
			f32 dz;

			data->field_0x2A8++;
			if (data->field_0x2A8 >= 30) {
				data->field_0x2A8 = 0;
			}
			index = data->field_0x2A8;
			dx = px - ox;
			dy = py - oy;
			dz = pz - oz;
			data->field_0x230[index] = 1;
			data->field_0x50[index] = px;
			data->field_0xC8[index] = py;
			data->field_0x140[index] = pz;
			data->field_0x1B8[index] = counter;
			dist = dx * dx + dy * dy + dz * dz;
			if (dist) {
				dist = sqrtf(dist);
			}
			data->field_0x2AC[index] = len + dist;
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effCloudDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCloudDisp, effect);
#endif
}

// Two helpers that each build the TEV colour locally and pass it by value: this puts mwcc's three
// colour stack slots where the target has them (local r1+0x10, first copy r1+0x14, second copy r1+0xc).
// The plain assignment shape lands them 4 bytes lower and shifts mtx/texObj down too.
#ifdef MUST_MATCH
static inline void effCloudTevColor1(s32 r, s32 g, s32 b, s32 a) {
	GXColor c = unk_804230c0;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void effCloudTevColor2(s32 r, s32 g, s32 b) {
	GXColor c = unk_804230c4;
	c.r = r;
	c.g = g;
	c.b = b;
	GXSetTevColor(2, c);
}
#endif

// NONMATCHING (99.26%): logic/constants/GX calls/register set match; still differs in global graph
// colouring (texBase/vtx land r31/r19 vs r19/r28, rotating the loop-constant trio; `prev` coalesces
// into r15; fadeB conversion swaps r3/r4; second loop's i/v swap; i*180 increment scheduled late).
void effCloudDisp(CameraId cameraId, void* param) {
	effCloudData* data;
	s32 frame;
	f32 fscale;
	s32 count;
	s32 type;
	CameraEntry* cam;
	effVtxArray* base;
	s32 firstActive;
	s32 fadeG;
	GXTexObj texObj;
	f32 cy;
	cloudVtx* vtx;
	f32 dir;
	s32 alpha;
	s32 v;
	f32 px, py, pz;
	smartEntry* entry;
	s32 texU;
	f32 angle;
	s32 fadeR;
	Mtx mtx;
	f32 width;
	f32 a;
	f32 sx;
	s32 age;
#ifdef MUST_MATCH
	// colours are built in the MUST_MATCH inlines above
#else
	GXColor color;
#endif
	EffectEntry* effect;
	s32 fadeB;
	s32 alpha0;
	f32 wobble;
	s32 j;
	f32 b;
	s32 texBase;
	s32 i;
	s32 k;
	s32 prev;

	effect = param;
	cam = camGetPtr(cameraId);
	data = effect->userdata;
	frame = data->field_0x14;
	alpha0 = data->field_0x24;
	type = data->type;
	fscale = data->scale;

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);

	if (type >= 2) {
		GXSetNumTevStages(2);
		GXSetTevOrder(0, 0, 0, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 4, 10, 8, 15);
		GXSetTevAlphaIn(0, 7, 5, 4, 7);
		GXSetTevOrder(1, 0xff, 0xff, 0xff);
		GXSetTevColorOp(1, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
		GXSetTevColorIn(1, 15, 15, 15, 0);
		GXSetTevAlphaIn(1, 7, 0, 1, 7);
	} else {
		GXSetNumTevStages(2);
		GXSetTevOrder(0, 0, 0, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 4, 2, 8, 15);
		GXSetTevAlphaIn(0, 7, 5, 4, 7);
		GXSetTevOrder(1, 0xff, 0xff, 0xff);
		GXSetTevColorOp(1, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
		GXSetTevColorIn(1, 15, 15, 15, 0);
		GXSetTevAlphaIn(1, 7, 0, 1, 7);
	}

	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(mtx, float_0p015625_8041eb98, float_0p03125_8041eb9c, float_0_8041eba0);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	effGetTexObjN64(0x83, &texObj);
	GXLoadTexObj(&texObj, 0);

#ifdef MUST_MATCH
	effCloudTevColor1(data->field_0x18, data->field_0x1C, data->field_0x20, alpha0);

	effCloudTevColor2(data->field_0x28, data->field_0x2C, data->field_0x30);
#else
	color = *(GXColor*)&unk_804230c0;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = alpha0;
	GXSetTevColor(1, color);

	color = *(GXColor*)&unk_804230c4;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	GXSetTevColor(2, color);
#endif

	GXLoadPosMtxImm(cam->view, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(0);

	entry = smartAlloc(0x690, 3);
	base = (effVtxArray*)entry->address;
	memset(base, 0, 0x690);
	effSetVtxDescN64(base);

	texBase = (frame & 0x3F) << 5;

	firstActive = -1;
	count = 0;
	for (i = 0; i < 30; i++) {
		if (data->field_0x230[(1 + data->field_0x2A8 + i) % 30] != 0) {
			count++;
		}
	}

	if (count - 1 > 0) {
		vtx = (cloudVtx*)base;
		for (i = 0; i < 30; i++) {
			j = (1 + data->field_0x2A8 + i) % 30;
			alpha = (s32)(float_255_8041ebac * (f32)sin((f32)((i * 180) / (count - 1)) * float_6p2832_8041eba4 / float_360_8041eba8));
			if (alpha > 200) {
				alpha = 200;
			}
			if (data->field_0x230[j] != 0) {
				if (firstActive == -1) {
					k = j + 1;
					if (k >= 30) {
						k = 0;
					}
					dir = -angleABf(data->field_0xC8[k], -data->field_0x50[k], data->field_0xC8[j], -data->field_0x50[j]);
					firstActive = i;
				} else if (i != 29) {
					k = j + 1;
					prev = j - 1;
					if (k >= 30) {
						k = 0;
					}
					if (prev < 0) {
						prev = 29;
					}
					a = -angleABf(data->field_0xC8[k], -data->field_0x50[k], data->field_0xC8[j], -data->field_0x50[j]);
					b = -angleABf(data->field_0xC8[j], -data->field_0x50[j], data->field_0xC8[prev], -data->field_0x50[prev]);
					if (b - a > float_180_8041ebb0) {
						a += float_360_8041eba8;
					} else if (b - a < float_neg180_8041ebb4) {
						b += float_360_8041eba8;
					}
					dir = a + b;
					dir *= float_0p5_8041ebb8;
				}

				age = frame - data->field_0x1B8[j];
				px = data->field_0x50[j];
				py = data->field_0xC8[j];
				pz = data->field_0x140[j];
				wobble = (f32)sin((f32)((frame - data->field_0x1B8[j] * 80) * 4) * float_6p2832_8041eba4 / float_360_8041eba8);
				width = (f32)age + (float_3_8041ebc0 * wobble + float_16_8041ebbc);
				width *= fscale;

				if (type == 0) {
					s32 t = (s32)(float_24_8041ebc4 * data->field_0x2AC[j]);
					texU = t + texBase;
				} else {
					s32 t = (s32)(float_24_8041ebc4 * data->field_0x2AC[j]);
					texU = t + texBase;
				}

				fadeR = 0xff;
				fadeG = 0xff;
				fadeB = 0xff - age * 100;
				if (fadeB < 0) {
					s32 t = (s32)(float_0p8_8041ebc8 * (f32)fadeB);
					fadeB = 0;
					fadeG = t + 0xff;
					if (fadeG < 0) {
						s32 t2 = (s32)(float_0p4_8041ebcc * (f32)fadeG);
						fadeG = 0;
						fadeR = t2 + 0xff;
						if (fadeR < 0) {
							fadeR = 0;
						}
					}
				}

				angle = float_6p2832_8041eba4 * dir / float_360_8041eba8;
				sx = width * (f32)sin(angle);
				cy = width * (f32)cos(angle);

				vtx[0].x = (s16)(px + sx);
				vtx[0].y = (s16)(py + cy);
				vtx[0].z = (s16)(pz + float_0_8041eba0);
				vtx[0].u = texU;
				vtx[0].v = 0x400;
				vtx[0].r = fadeR;
				vtx[0].g = fadeG;
				vtx[0].b = fadeB;
				vtx[0].a = alpha;

				vtx[1].x = (s16)(px - sx);
				vtx[1].y = (s16)(py - cy);
				vtx[1].z = (s16)(pz + float_0_8041eba0);
				vtx[1].u = texU;
				vtx[1].v = 0;
				vtx[1].r = fadeR;
				vtx[1].g = fadeG;
				vtx[1].b = fadeB;
				vtx[1].a = alpha;
			}
			vtx += 2;
		}

		DCFlushRange(base, 0x690);
		GXInvalidateVtxCache();
		v = firstActive * 2;
		for (i = firstActive; i < 29; i++, v += 2) {
			GXBegin(0x90, 0, 6);
			tri2(v, v + 2, v + 1, v, v + 1, v + 2, v + 3, v);
		}
	}
}

// .sdata2 constants owned by this unit (see config/G8MJ01/splits.txt).
const f32 float_0p015625_8041eb98 = 0.015625f;
const f32 float_0p03125_8041eb9c = 0.03125f;
const f32 float_0_8041eba0 = 0.0f;
const f32 float_6p2832_8041eba4 = 6.2831855f;
const f32 float_360_8041eba8 = 360.0f;
const f32 float_255_8041ebac = 255.0f;
const f32 float_180_8041ebb0 = 180.0f;
const f32 float_neg180_8041ebb4 = -180.0f;
const f32 float_0p5_8041ebb8 = 0.5f;
const f32 float_16_8041ebbc = 16.0f;
const f32 float_3_8041ebc0 = 3.0f;
const f32 float_24_8041ebc4 = 24.0f;
const f32 float_0p8_8041ebc8 = 0.8f;
const f32 float_0p4_8041ebcc = 0.4f;
