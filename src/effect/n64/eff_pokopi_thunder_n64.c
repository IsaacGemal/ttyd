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
// files carry). The two double literals and the comparison zero pool into the original
// .rodata (double_0p5_802fb438, double_3_802fb440, double_0_802fb448).
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

// The sqrtf constants are named .rodata objects in the original; declaring them extern forces the
// inlined Newton iterations to load the symbols instead of materialising fresh pool entries.
extern const f64 double_0p5_802fb438;
extern const f64 double_3_802fb440;
extern const f64 double_0_802fb448;

static inline f32 pt_sqrtf(f32 x) {
	if (x > 0.0f) {
		f64 guess = __frsqrte(x);
		guess = double_0p5_802fb438 * guess * (double_3_802fb440 - guess * guess * x);
		guess = double_0p5_802fb438 * guess * (double_3_802fb440 - guess * guess * x);
		guess = double_0p5_802fb438 * guess * (double_3_802fb440 - guess * guess * x);
		return x * guess;
	} else if (x < double_0_802fb448) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

// .rodata

#ifdef MUST_MATCH
// Immediately generated helpers pool the zero-Vec copy template and the two int->float
// conversion doubles (the unsigned mask first, then the signed one) ahead of the
// "PokopiThunderN64" string, in the original's .rodata order. Nothing calls the conversion
// helpers; the linker strips their unused bodies.
#pragma defer_codegen off
static inline Vec pool_vec3(void) {
	return (Vec){ 0.0f, 0.0f, 0.0f };
}

static f32 pool_i2f_mask(s32 v) {
	return (f32)(u32)v;
}

static f32 pool_i2f_signed(s32 v) {
	return (f32)v;
}
#pragma defer_codegen reset
#else
const Vec vec3_802fb400 = { 0.0f, 0.0f, 0.0f };
#endif

const char str_802FB420[] = "PokopiThunderN64";
// Named in the original .rodata (see the .obj blocks).
const f64 double_0p5_802fb438 = 0.5;
const f64 double_3_802fb440 = 3.0;
const f64 double_0_802fb448 = 0.0;

// The named sqrtf doubles (0x38/0x40/0x48) sit where the original .rodata has them, and the
// else-return-0 block is placed next to the sqrt result; together with a real zero variable (so
// the outer fcmpu compares dist2 first, as retail does) that reproduces the original exactly.
EffectEntry* effPokopiThunderN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale) {
	EffectEntry* effect;
	s32 cameraId;
	EffPokopiThunderData* data;
	f32 dx;
	f32 dy;
	f32 dz;
	f32 dist2;

#ifdef MUST_MATCH
	/* MUST_MATCH: a named zero variable makes MWCC emit `fcmpu cr0, f4, f0` (dist2 first);
	   a literal `0.0f` puts the constant first. */
	f32 zero = 0.0f;
#endif

	effect = effEntry();
	cameraId = 4;
	if (type >= 10) {
		cameraId = 8;
	}
	type %= 10;
	effect->type = str_802FB420;
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
	data->field_0x3C = 0xFF;

	// The unnamed scratch fields the original keeps in the struct's reserved runs.
	*(f32*)((u8*)data + 0x10) = x;
	*(f32*)((u8*)data + 0x14) = y;
	*(f32*)((u8*)data + 0x18) = z;
	*(f32*)((u8*)data + 0x1C) = endX;
	*(f32*)((u8*)data + 0x20) = endY;
	*(f32*)((u8*)data + 0x24) = endZ;
	data->x = x;
	data->y = y;
	data->z = z;
	*(f32*)((u8*)data + 0x4C) = scale;

	data->cameraId = cameraId;

	dx = x - endX;
	dy = y - endY;
	dz = z - endZ;
	dist2 = dx * dx + dy * dy + dz * dz;
#ifdef MUST_MATCH
	if (dist2 != zero) {
#else
	if (dist2 != 0.0f) {
#endif
		*(f32*)((u8*)data + 0x114) = pt_sqrtf(dist2);
	} else {
		return NULL;
	}
	*(f32*)((u8*)data + 0x114) = 0.005f * *(f32*)((u8*)data + 0x114);
	data->field_0x30 = 0xFF;
	data->field_0x34 = 0xDC;
	data->field_0x38 = 0x14;
	data->field_0x40 = 0xFF;
	data->field_0x44 = 0xFF;
	data->field_0x48 = 0xFF;
	*(s32*)((u8*)data + 0x110) = rand() % 360;
	return effect;
}
void effPokopiThunderMain(EffectEntry* effect) {
	EffPokopiThunderData* data;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 timer;

	data = effect->userdata;
#ifdef MUST_MATCH
	pos = pool_vec3();
#else
	pos = vec3_802fb400;
#endif
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
			data->field_0x3C = (s32)((f32)data->field_0x3C / 2.0f);
		}
	} else {
		if (timer >= 11) {
			data->field_0x3C = (s32)((f32)data->field_0x3C / 2.0f);
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

// .sbss2 scratch GXColor followed by the 4-byte section-tail gap. Objects land in reverse
// declaration order, so the gap is declared before unk_80423220.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_80423224_sbss2[4];
__declspec(section ".sdata") GXColor unk_80423220;
#pragma section ".sdata" ".sbss"

// .sdata2 scratch word the original builds the second TEV colour from.
__declspec(section ".sdata2") const u32 dat_8041fa20 = 0x00000080;

// Local view of one 14-byte vertex the original writes (pos/tex/colour, stride 0xE).
typedef struct EffPokopiThunderVtx {
	s16 pos[3];  // 0x0
	s16 tex0[2]; // 0x6
	u8 clr[4];   // 0xA
} EffPokopiThunderVtx;

// The original's .sdata2 float pool, named after its addresses.

// Retail builds each Tev colour from the unk_80423220/dat_8041fa20 globals and passes it straight to
// GXSetTevColor. They are void inlines (not a colour returned and passed on) because that is what makes
// MWCC pair each colour's build slot with its outgoing argument slot; the direct form gives a different
// stack layout (same shape as effPokopiPchargeDisp).
#ifdef MUST_MATCH
static inline void effPokopiThunderSetColor1(EffPokopiThunderData* data, s32 a) {
	GXColor c = unk_80423220;
	c.r = data->field_0x30;
	c.g = data->field_0x34;
	c.b = data->field_0x38;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void effPokopiThunderSetColor2(EffPokopiThunderData* data) {
	GXColor c = *(GXColor*)&dat_8041fa20;
	c.r = data->field_0x40;
	c.g = data->field_0x44;
	c.b = data->field_0x48;
	GXSetTevColor(2, c);
}
#endif

// NONMATCHING (99.85% / 5 differing insns): everything matches except the loop-2 preheader. The target
// zeroes `i` with `li r26,0` and copies that zero into the strength-reduced colour/texture counters
// (`mr r30/r31/r28/r27, r26`); ours zeroes the first counter and copies from it, then zeroes `i` on its own.
// MWCC's second CSE pass only uses a compiler variable (inline local/param) as the copy source, never a
// named local, so the target's loop-2 counter was not a plain named local. Moving the loop into an inline
// helper makes `i` the source but renumbers every helper variable above the named locals and costs ~120
// instructions of colouring.
static inline void effPokopiThunderDrawStrip(s32 count) {
	s32 j;
	s32 k;
	for (j = 0, k = 0; j < count - 1; j++) {
		GXBegin(0x90, 0, 6);
		tri2(k, k + 2, k + 1, 2, k + 1, k + 2, k + 3, 0);
		k += 2;
	}
}

void effPokopiThunderDisp(CameraId camId, void* param) {
	s32 n;
	CameraEntry* cam;
	s32 t1;
	GXTexObj texObj;
	GXColor color;
	s32 cr;
	f32 scale;
	f32 t;
	
	s32 i;
	f32 a2;
	s32 type;
	Mtx mtx;
	f32 wx;
	f32 proj[7];
	f32 wy;
	f32 amp;
	s32 timer;
	f32 f20;
	f32 viewport[6];
	GXColor color2;
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
	cr = data->field_0x3C;
	type = data->type;
	scale = (*(f32*)((u8*)data + 0x4C));
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
#ifdef MUST_MATCH
	effPokopiThunderSetColor1(data, cr);
#else
	color = unk_80423220;
	color.r = data->field_0x30;
	color.g = data->field_0x34;
	color.b = data->field_0x38;
	color.a = cr;
	GXSetTevColor(1, color);
#endif
#ifdef MUST_MATCH
	effPokopiThunderSetColor2(data);
#else
	color2 = *(GXColor*)&dat_8041fa20;
	color2.r = data->field_0x40;
	color2.g = data->field_0x44;
	color2.b = data->field_0x48;
	GXSetTevColor(2, color2);
#endif
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
			tbl = (u8*)yuragi_data_table[type] + t1 * 0x18;
		} else {
			tbl = (u8*)yuragi_data_table[type] + 0xF0;
		}
	} else {
		if (t1 < 8) {
			tbl = (u8*)yuragi_data_table[0] + t1 * 0x18;
		} else if (t1 < 0x24) {
			tbl = (u8*)yuragi_data_table[0] + (t1 % 6 + 2) * 0x18;
		} else {
			tbl = (u8*)yuragi_data_table[0] + 0xF0;
		}
	}

	for (n = 0; n < 12; n++) {
		s32 idx = *(s32*)((u8*)data + 0x110) + (n - timer) * 10;
		t = (f32)tbl[n];
		wx = 0.09090909f * ((*(f32*)((u8*)data + 0x1C)) - (*(f32*)((u8*)data + 0x10)));
		ang = 6.2831855f * (f32)(idx % 120 - 60) / 360.0f;
		wy = 0.09090909f * ((*(f32*)((u8*)data + 0x20)) - (*(f32*)((u8*)data + 0x14)));
		f20 = 0.04f * (t * (f32)sin(ang));
		f6 = 0.04f * (t * (f32)cos(ang));
		t = wx * f6 - wy * f20;
		data->ptX[n] = (t) +
		          (0.09090909f * ((f32)n * ((*(f32*)((u8*)data + 0x1C)) - (*(f32*)((u8*)data + 0x10)))) + (*(f32*)((u8*)data + 0x10)));
		t = wy * f6;
		data->ptY[n] = (wx * f20 + t) +
		          (0.09090909f * ((f32)n * ((*(f32*)((u8*)data + 0x20)) - (*(f32*)((u8*)data + 0x14)))) + (*(f32*)((u8*)data + 0x14)));
		data->ptZ[n] = zero + (0.09090909f * ((f32)n * ((*(f32*)((u8*)data + 0x24)) - (*(f32*)((u8*)data + 0x18)))) + (*(f32*)((u8*)data + 0x18)));
		data->ptScale[n] = 0.1f * (f32)tbl[n + 12];
	}

	entry = smartAlloc(0x2A0, 3);
	vtx = (EffPokopiThunderVtx*)entry->address;

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
	effSetVtxDescN64((effVtxArray*)vtx);

	effPokopiThunderDrawStrip(i);

	if (data->cameraId != 4) {
		GXSetProjectionv(proj);
		GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
	}
}
