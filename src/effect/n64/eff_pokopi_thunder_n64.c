#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "eff/eff_n64.h"
#include "math_ext.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <stdlib.h>

// Inlined MetroWerks sqrtf and its NaN classification (same helper several other effect
// files carry).
int __float_nan[];
#define NAN (*(f32*)__float_nan)

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

static inline f32 pt_sqrtf(f32 x) {
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

// .rodata is all the compiler's own pool: the zero-Vec template from this inline's compound literal
// (pooled once, when the inline is parsed), then the functions' literals in generation order (Disp's
// two int->float doubles, Entry's type string and sqrtf doubles).
static inline Vec effPokopiThunderZeroVec(void) {
	return (Vec){ 0.0f, 0.0f, 0.0f };
}

// The else-return-NULL block is placed next to the sqrt result; together with a real zero variable
// (so the outer fcmpu compares dist2 first, as retail does) that reproduces the original exactly.
EffectEntry* effPokopiThunderN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale) {
	EffectEntry* effect;
	s32 cameraId;
	EffPokopiThunderData* data;
	f32 dx;
	f32 dy;
	f32 dz;
	f32 dist2;

	// A named zero variable makes MWCC emit `fcmpu cr0, f4, f0` (dist2 first); a literal `0.0f` puts the
	// constant first.
	f32 zero = 0.0f;

	effect = effEntry();
	cameraId = 4;
	if (type >= 10) {
		cameraId = 8;
	}
	type %= 10;
	effect->type = "PokopiThunderN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffPokopiThunderData));
	effect->userdata = data;
	effect->callback = effPokopiThunderMain;
	effect->flags |= 2;

	data->type = type;
	data->timer = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = lifetime;
	}
	data->tev1A = 0xFF;

	// The unnamed scratch fields the original keeps in the struct's reserved runs.
	data->field_0x10 = x;
	data->field_0x14 = y;
	data->field_0x18 = z;
	data->field_0x1C = endX;
	data->field_0x20 = endY;
	data->field_0x24 = endZ;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x4C = scale;

	data->cameraId = cameraId;

	dx = x - endX;
	dy = y - endY;
	dz = z - endZ;
	dist2 = dx * dx + dy * dy + dz * dz;
	if (dist2 != zero) {
		data->field_0x114 = pt_sqrtf(dist2);
	} else {
		return NULL;
	}
	data->field_0x114 = 0.005f * data->field_0x114;
	data->tev1R = 0xFF;
	data->tev1G = 0xDC;
	data->tev1B = 0x14;
	data->tev2R = 0xFF;
	data->tev2G = 0xFF;
	data->tev2B = 0xFF;
	data->field_0x110 = rand() % 360;
	return effect;
}
void effPokopiThunderMain(EffectEntry* effect) {
	EffPokopiThunderData* data;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 timer;

	data = effect->userdata;
	pos = effPokopiThunderZeroVec();
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}

	timer = data->lifetime;
	if (timer < 1000) {
		data->lifetime = timer - 1;
	}
	data->timer++;

	timer = data->lifetime;
	if (timer < 0) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	// MWCC strength-reduces / 2.0f into fmuls by the 0.5 pool constant, and that division form is
	// what reproduces the original's f2/f0 constant registers (a plain * 0.5f allocates f1/f2).
	if (type == 2) {
		if (timer >= 40) {
			data->tev1A = (s32)((f32)data->tev1A / 2.0f);
		}
	} else {
		if (timer >= 11) {
			data->tev1A = (s32)((f32)data->tev1A / 2.0f);
		}
	}

	dispEntry(data->cameraId, dispCalcZ(&pos2), 2, effPokopiThunderDisp, effect);
}

// .data: the two animation tables, byte-for-byte from the original `.4byte` blocks.
u8 yuragi_data[0x108] = {
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x0A, 0x09, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x23, 0x05, 0x19, 0x05, 0x00, 0x19, 0x23, 0x05, 0x0F, 0x00, 0x00, 0x00,
	0x0A, 0x0D, 0x0F, 0x0C, 0x0B, 0x0D, 0x0A, 0x0C, 0x05, 0x01, 0x00, 0x00,
	0x2D, 0x0F, 0x23, 0x0F, 0x00, 0x23, 0x2D, 0x37, 0x19, 0x05, 0x02, 0x00,
	0x0D, 0x0A, 0x0C, 0x0A, 0x0D, 0x0A, 0x0D, 0x0A, 0x0C, 0x09, 0x05, 0x01,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0x0F, 0x05, 0x02, 0x00,
	0x0E, 0x0A, 0x0F, 0x0A, 0x0D, 0x1E, 0x20, 0x1E, 0x18, 0x32, 0x28, 0x3C,
	0x01, 0x05, 0x0F, 0x0F, 0x23, 0x2D, 0x28, 0x23, 0x19, 0x05, 0x02, 0x00,
	0x04, 0x06, 0x0A, 0x05, 0x08, 0x0E, 0x06, 0x0A, 0x0E, 0x23, 0x32, 0x32,
	0x01, 0x0F, 0x19, 0x0F, 0x19, 0x23, 0x2D, 0x28, 0x23, 0x0F, 0x05, 0x00,
	0x02, 0x02, 0x17, 0x0C, 0x08, 0x0F, 0x0F, 0x05, 0x06, 0x12, 0x26, 0x3C,
	0x1E, 0x19, 0x0F, 0x05, 0x0F, 0x0F, 0x19, 0x1E, 0x23, 0x19, 0x05, 0x00,
	0x03, 0x03, 0x03, 0x0B, 0x03, 0x0C, 0x04, 0x05, 0x02, 0x0C, 0x2A, 0x32,
	0x21, 0x21, 0x14, 0x0C, 0x19, 0x11, 0x0F, 0x14, 0x16, 0x14, 0x05, 0x00,
	0x03, 0x03, 0x03, 0x02, 0x0A, 0x02, 0x02, 0x0C, 0x02, 0x02, 0x02, 0x14,
	0x1E, 0x1E, 0x19, 0x19, 0x23, 0x14, 0x0F, 0x0A, 0x0F, 0x0F, 0x05, 0x00,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x06, 0x02, 0x02, 0x02, 0x06, 0x05,
	0x1B, 0x1B, 0x16, 0x16, 0x20, 0x11, 0x0C, 0x07, 0x0C, 0x0C, 0x02, 0x00,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,
};
u8 yuragi_data2[0x108] = {
	0x19, 0x19, 0x19, 0x05, 0x19, 0x05, 0x19, 0x19, 0x05, 0x19, 0x19, 0x19,
	0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0F, 0x0F, 0x0F, 0x19, 0x0F, 0x0F, 0x05, 0x19, 0x19, 0x0F, 0x19, 0x19,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x19, 0x19, 0x19, 0x05, 0x0F, 0x19, 0x05, 0x05, 0x0F, 0x05, 0x05, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x00, 0x00,
	0x19, 0x19, 0x19, 0x05, 0x19, 0x05, 0x19, 0x19, 0x05, 0x0F, 0x05, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x0F, 0x0F, 0x0F, 0x2D, 0x23, 0x2D, 0x23, 0x23, 0x19, 0x23, 0x0F, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x2D, 0x37, 0x23, 0x19, 0x37, 0x0F, 0x2D, 0x19, 0x2D, 0x23, 0x0F, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x55, 0x19, 0x0F, 0x4B, 0x19, 0x41, 0x23, 0x2D, 0x4B, 0x55, 0x0F, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x55, 0x0F, 0x23, 0x37, 0x41, 0x2D, 0x19, 0x4B, 0x23, 0x2D, 0x23, 0x28,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x55, 0x0F, 0x23, 0x37, 0x41, 0x2D, 0x19, 0x4B, 0x23, 0x2D, 0x23, 0x28,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x55, 0x0F, 0x23, 0x37, 0x41, 0x2D, 0x19, 0x4B, 0x23, 0x2D, 0x23, 0x28,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x06, 0x05,
	0x1E, 0x1E, 0x19, 0x19, 0x23, 0x14, 0x0F, 0x0A, 0x0F, 0x0F, 0x05, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x02, 0x00,
};

// .sdata
u8* yuragi_data_table[2] = { yuragi_data, yuragi_data2 };

// .sbss2 4-byte gap after the unit's colour template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_80423224_sbss2[4];
#pragma section ".sdata" ".sbss"

// Local view of one 14-byte vertex the original writes (pos/tex/colour, stride 0xE).
typedef struct EffPokopiThunderVtx {
	s16 pos[3];  // 0x0
	s16 tex0[2]; // 0x6
	u8 clr[4];   // 0xA
} EffPokopiThunderVtx;

// The original's .sdata2 float pool, named after its addresses.

static inline void effPokopiThunderDrawStrip(s32 count) {
	s32 j;
	s32 k;
	for (j = 0, k = 0; j < count - 1; j++) {
		GXBegin(0x90, 0, 6);
		tri2(k, k + 2, k + 1, 2, k + 1, k + 2, k + 3, 0);
		k += 2;
	}
}

// The point loop and the vertex loop share one counter `i`: the vertex loop's zero is then the start of
// `i`'s second live range, which MWCC copies into the strength-reduced colour/texture counters as retail does.
void effPokopiThunderDisp(CameraId camId, void* param) {
	s32 timer;
	s32 i;
	CameraEntry* cam;
	s32 t1;
	GXTexObj texObj;
	s32 cr;
	f32 scale;
	f32 t;
	f32 a2;
	s32 type;
	Mtx mtx;
	f32 wx;
	f32 proj[7];
	f32 wy;
	f32 amp;
	f32 f20;
	f32 viewport[6];
	smartEntry* entry;
	f32 f6;
	f32 ct;
	f32 s2;
	EffPokopiThunderData* data;
	f32 ang;
	f32 zero = 0.0f;
	EffPokopiThunderVtx* vtx;
	EffPokopiThunderVtx* v;
	u8* tbl;
	f32 a1;
	s32 u;
	EffectEntry* effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	timer = data->timer;
	cr = data->tev1A;
	type = data->type;
	scale = data->field_0x4C;
	if (data->cameraId != 4) {
		cam = camGetPtr(CAMERA_3D);
		GXGetProjectionv(proj);
		GXGetViewportv(viewport);
		GXSetProjection(cam->projection, cam->projType);
	}
	PSMTXScale(mtx, 0.1f, 0.1f, 0.1f);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXSetTevColor(1, (GXColor){ data->tev1R, data->tev1G, data->tev1B, cr });
	GXSetTevColor(2, (GXColor){ data->tev2R, data->tev2G, data->tev2B, 0x80 });
	GXSetNumChans(0);
	GXSetNumTevStages(4);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 3);
	GXSetTevColorIn(0, 0xf, 8, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 2, 4, 0, 0xf);
	GXSetTevAlphaIn(1, 3, 7, 2, 7);
	GXSetTevOrder(2, 1, 1, 0xff);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xf, 0xf, 0xf, 0);
	GXSetTevAlphaIn(2, 7, 3, 4, 0);
	GXSetTevOrder(3, 0xff, 0xff, 0xff);
	GXSetTevColorOp(3, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
	GXSetTevColorIn(3, 0xf, 0xf, 0xf, 0);
	GXSetTevAlphaIn(3, 7, 1, 0, 7);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
	PSMTXScale(mtx, 0.0078125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	GXLoadTexMtxImm(mtx, 0x21, 1);
	effGetTexObjN64(0x54, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXLoadTexObj(&texObj, 1);
	GXSetCullMode(0);

	t1 = timer - 1;
	if (type != 2) {
		if (t1 < 11) {
			tbl = &yuragi_data_table[type][t1 * 0x18];
		} else {
			tbl = &yuragi_data_table[type][10 * 0x18];
		}
	} else {
		if (t1 < 8) {
			tbl = &yuragi_data_table[0][t1 * 0x18];
		} else if (t1 < 0x24) {
			tbl = &yuragi_data_table[0][(t1 % 6 + 2) * 0x18];
		} else {
			tbl = &yuragi_data_table[0][10 * 0x18];
		}
	}

	for (i = 0; i < 12; i++) {
		s32 idx = data->field_0x110 + (i - timer) * 10;
		t = (f32)tbl[i];
		wx = 0.09090909f * (data->field_0x1C - data->field_0x10);
		ang = 6.2831855f * (f32)(idx % 120 - 60) / 360.0f;
		wy = 0.09090909f * (data->field_0x20 - data->field_0x14);
		f20 = 0.04f * (t * (f32)sin(ang));
		f6 = 0.04f * (t * (f32)cos(ang));
		t = wx * f6 - wy * f20;
		data->ptX[i] = (t) +
		          (0.09090909f * ((f32)i * (data->field_0x1C - data->field_0x10)) + data->field_0x10);
		t = wy * f6;
		data->ptY[i] = (wx * f20 + t) +
		          (0.09090909f * ((f32)i * (data->field_0x20 - data->field_0x14)) + data->field_0x14);
		data->ptZ[i] = zero + (0.09090909f * ((f32)i * (data->field_0x24 - data->field_0x18)) + data->field_0x18);
		data->ptScale[i] = 0.1f * (f32)tbl[i + 12];
	}

	entry = smartAlloc(0x2A0, 3);
	vtx = entry->address;

	v = vtx;
	for (i = 0; i < 12; i++) {
		if (i == 0) {
			amp = 8.0f;
			ang = -angleABf(data->ptY[i + 1], -data->ptX[i + 1], data->ptY[i], -data->ptX[i]);
		} else {
			amp = 8.0f;
			if (i == 11) {
				ang = -90.0f;
			} else {
				a1 = -angleABf(data->ptY[i + 1], -data->ptX[i + 1], data->ptY[i], -data->ptX[i]);
				a2 = -angleABf(data->ptY[i], -data->ptX[i], data->ptY[i - 1], -data->ptX[i - 1]);
				if (a2 - a1 > 180.0f) {
					a1 += 360.0f;
				} else if (a2 - a1 < -180.0f) {
					a2 += 360.0f;
				}
				ang = a1 + a2;
				ang = (ang) / 2.0f;
			}
		}
		u = (0x80 - i * 12) << 5;
		amp = amp * (scale * data->ptScale[i]);
		ang = 6.2831855f * ang / 360.0f;
		s2 = amp * (f32)sin(ang);
		ct = amp * (f32)cos(ang);
		v[0].pos[0] = (s16)(10.0f * (data->ptX[i] + s2));
		v[0].pos[1] = (s16)(10.0f * (data->ptY[i] + ct));
		v[0].pos[2] = (s16)(10.0f * (data->ptZ[i] + zero));
		v[0].tex0[0] = u;
		v[0].tex0[1] = 0;
		v[0].clr[0] = i * 50;
		v[0].clr[1] = i * 120;
		v[0].clr[2] = i * 30;
		v[1].pos[0] = (s16)(10.0f * (data->ptX[i] - s2));
		v[1].pos[1] = (s16)(10.0f * (data->ptY[i] - ct));
		v[1].pos[2] = (s16)(10.0f * (data->ptZ[i] + zero));
		v[1].tex0[0] = u;
		v[1].tex0[1] = 0x400;
		v[1].clr[0] = i * 50;
		v[1].clr[1] = i * 120;
		v[1].clr[2] = i * 30;
		v += 2;
	}

	DCFlushRange(vtx, 0x2A0);
	GXInvalidateVtxCache();
	effSetVtxDescN64(vtx);

	effPokopiThunderDrawStrip(i);

	if (data->cameraId != 4) {
		GXSetProjectionv(proj);
		GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
	}
}
