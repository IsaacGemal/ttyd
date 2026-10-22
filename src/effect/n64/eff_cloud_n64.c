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
#ifdef MUST_MATCH
	// Poking float_0 through a pointer forces a load distinct from the caller's `dist != float_0`
	// compare; a plain reference lets MWCC CSE them and moves effCloudMain's accumulator off f1.
	if (x > *(f32*)&float_0_8041eba0) {
#else
	if (x > float_0_8041eba0) {
#endif
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
#ifdef MUST_MATCH
	// original .rodata is a 12-byte "CloudN64" object plus a 4-byte zero gap_04
	effect->type = "CloudN64\0\0\0\0\0\0\0";
#else
	effect->type = "CloudN64";
#endif
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effCloudData));
	effect->userdata = data;
	effect->callback = effCloudMain;
	effect->flags |= 2;
	data->type = type;
	data->frame = 0;
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
			data->color0R = 0xff;
			data->color0G = 0xff;
			data->color0B = 0xff;
			data->color1R = 0xe1;
			data->color1G = 0xe1;
			data->color1B = 0xe1;
			data->alpha = 0xff;
			break;
		case 1:
			data->color0R = 0xff;
			data->color0G = 0xf5;
			data->color0B = 0xe6;
			data->color1R = 0x2d;
			data->color1G = 0x23;
			data->color1B = 0x0f;
			data->alpha = 0xff;
			break;
		case 2:
			data->color0R = 0xff;
			data->color0G = 0;
			data->color0B = 0;
			data->color1R = 0x9b;
			data->color1G = 0x7d;
			data->color1B = 0x7d;
			data->alpha = 0xff;
			break;
		case 3:
			data->color0R = 0xff;
			data->color0G = 0xff;
			data->color0B = 0xff;
			data->color1R = 0xff;
			data->color1G = 0x41;
			data->color1B = 0x14;
			data->alpha = 0xff;
			break;
		default:
			break;
	}
	data->headX = x;
	data->headY = y;
	data->headZ = z;
	for (i = 0; i < sizeof(data->trailActive) / sizeof(data->trailActive[0]); i++) {
		data->trailActive[i] = 0;
	}
	data->trailHead = 0;
	data->trailX[0] = x;
	data->trailY[0] = y;
	data->trailZ[0] = z;
	data->trailActive[0] = 1;
	data->trailFrame[0] = 0;
	data->trailLength[0] = float_0_8041eba0;
	data->velX = (f32)(rand() % 30 + 10);
	data->velY = float_0_8041eba0;
	data->velZ = float_0_8041eba0;
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
// conversions. `dist != float_0_8041eba0` (rather than the implicit `if (dist)`) keeps every zero
// reference on the unit's named float_0, so .sdata2 stays the original's 56 bytes; the inlined
// sqrtf then pokes float_0 through a pointer (see above) to keep the ring-buffer block's FPR
// allocation (px/py/pz f3/f4/f5, dist accumulator f1, zero f0).
void effCloudMain(EffectEntry* effect) {
	effCloudData* data = effect->userdata;
	Vec position;
	s32 type;
	s32 counter;

	position = makeVec(data->x, data->y, data->z);
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->frame++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	counter = data->frame;
	if (data->lifetime < 16) {
		data->alpha = data->lifetime << 4;
	}

	if (type == 3) {
		data->velX += (f32)(rand() % 10 - 5);
		data->velY += (f32)(rand() % 10 - 5);
		data->headX += data->velX;
		data->headY += data->velY;
		data->headZ += data->velZ;
	}

	{
		s32 index = data->trailHead % 30;
		f32 ox = data->trailX[index];
		f32 oy = data->trailY[index];
		f32 oz = data->trailZ[index];
		f32 px = data->headX;
		f32 py = data->headY;
		f32 pz = data->headZ;

		if (px != ox || py != oy || pz != oz) {
			f32 len = data->trailLength[index];
			f32 dist;
			f32 dx;
			f32 dy;
			f32 dz;

			data->trailHead++;
			if (data->trailHead >= 30) {
				data->trailHead = 0;
			}
			index = data->trailHead;
			dx = px - ox;
			dy = py - oy;
			dz = pz - oz;
			data->trailActive[index] = 1;
			data->trailX[index] = px;
			data->trailY[index] = py;
			data->trailZ[index] = pz;
			data->trailFrame[index] = counter;
			dist = dx * dx + dy * dy + dz * dz;
			if (dist != float_0_8041eba0) {
				dist = sqrtf(dist);
			}
			data->trailLength[index] = len + dist;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCloudDisp, effect);
}

// effCloudDisp's ring loop needed two changes to match: the Vtx pointer is rebuilt per iteration
// (`((cloudVtx*)base) + i * 2`) instead of being a stepped function-scope pointer, which lands it in
// r28; and a running `deg` that only counts active entries drives the alpha angle
// (`(f32)((deg * 180) / (count - 1))`, `deg++` inside the active-entry `if`), which lets MWCC
// strength-reduce `deg * 180` and schedule `addi r27,r27,0xb4` in the original's mid-store slot.
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
	s32 texBase;
	f32 dir;
	s32 alpha;
	s32 prev;
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
	EffectEntry* effect;
	s32 fadeB;
	s32 alpha0;
	f32 wobble;
	s32 j;
	f32 b;
	s32 i;
	s32 k;
	s32 v;
	s32 i2;
	s32 deg;
	effect = param;
	cam = camGetPtr(cameraId);
	data = effect->userdata;
	frame = data->frame;
	alpha0 = data->alpha;
	type = data->type;
	fscale = data->scale;

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

	if (type >= 2) {
		GXSetNumTevStages(2);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	} else {
		GXSetNumTevStages(2);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	}

	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(mtx, float_0p015625_8041eb98, float_0p03125_8041eb9c, float_0_8041eba0);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	effGetTexObjN64(0x83, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);

	GXSetTevColor(GX_TEVREG0, (GXColor){ data->color0R, data->color0G, data->color0B, alpha0 });

	GXSetTevColor(GX_TEVREG1, (GXColor){ data->color1R, data->color1G, data->color1B, 0 });

	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_NONE);

	entry = smartAlloc(0x690, 3);
	base = (effVtxArray*)entry->address;
	memset(base, 0, 0x690);
	effSetVtxDescN64(base);

	texBase = (frame & 0x3F) << 5;

	firstActive = -1;
	count = 0;
	for (i = 0; i < 30; i++) {
		if (data->trailActive[(1 + data->trailHead + i) % 30] != 0) {
			count++;
		}
	}

	if (count - 1 > 0) {
		deg = 0;
		for (i = 0; i < 30; i++) {
			cloudVtx* vtx = ((cloudVtx*)base) + i * 2;
			j = (1 + data->trailHead + i) % 30;
			alpha = (s32)(255.0f * (f32)sin((f32)((deg * 180) / (count - 1)) * float_6p2832_8041eba4 / float_360_8041eba8));
			if (alpha > 200) {
				alpha = 200;
			}
			if (data->trailActive[j] != 0) {
				if (firstActive == -1) {
					k = j + 1;
					if (k >= 30) {
						k = 0;
					}
					dir = -angleABf(data->trailY[k], -data->trailX[k], data->trailY[j], -data->trailX[j]);
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
					a = -angleABf(data->trailY[k], -data->trailX[k], data->trailY[j], -data->trailX[j]);
					b = -angleABf(data->trailY[j], -data->trailX[j], data->trailY[prev], -data->trailX[prev]);
					if (b - a > 180.0f) {
						a += float_360_8041eba8;
					} else if (b - a < -180.0f) {
						b += float_360_8041eba8;
					}
					dir = a + b;
					dir *= 0.5f;
				}

				age = frame - data->trailFrame[j];
				px = data->trailX[j];
				py = data->trailY[j];
				pz = data->trailZ[j];
				wobble = (f32)sin((f32)((frame - data->trailFrame[j] * 80) * 4) * float_6p2832_8041eba4 / float_360_8041eba8);
				width = (f32)age + (3.0f * wobble + 16.0f);
				width *= fscale;

				if (type == 0) {
					s32 t = (s32)(24.0f * data->trailLength[j]);
					texU = t + texBase;
				} else {
					s32 t = (s32)(24.0f * data->trailLength[j]);
					texU = t + texBase;
				}

				fadeR = 0xff;
				fadeG = 0xff;
				fadeB = 0xff - age * 100;
				if (fadeB < 0) {
					fadeG = (s32)(0.8f * (f32)fadeB) + 0xff;
					fadeB = 0;
					if (fadeG < 0) {
						fadeR = (s32)(0.4f * (f32)fadeG) + 0xff;
						fadeG = 0;
						if (fadeR < 0) {
							fadeR = 0;
						}
					}
				}

				angle = float_6p2832_8041eba4 * dir / float_360_8041eba8;
				sx = width * (f32)sin(angle);
				cy = width * (f32)cos(angle);
				deg++;

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
		}

		DCFlushRange(base, 0x690);
		GXInvalidateVtxCache();
		v = firstActive * 2;
		for (i2 = firstActive; i2 < 29; v += 2, i2++) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
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
