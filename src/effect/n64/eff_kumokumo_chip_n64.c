#include "eff/n64/eff_kumokumo_chip_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include "eff/eff_n64.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_8042319c;
__declspec(section ".sdata") GXColor unk_80423198;


__declspec(section ".data") u32 lbl_803A2280[2] = {
    0xBF800000, 0x3F800000,
};

// This unit's .sdata2 literals. float_0_8041f56c stays an extern (like eff_kumokumo_n64.c)
// and is only ever read through the named symbol. float_1_8041f570 is *defined* here:
// with no definition MWCC pools the `1.0f` as an anonymous .sdata2 entry instead of
// referencing the named symbol, which costs one instruction of fuzzy match. Defining it
// makes every `1.0f` in this TU reference the symbol.
extern const f32 float_0p03125_8041f568;
extern const f32 float_0_8041f56c;
const f32 float_1_8041f570 = 1.0f;

// Local vertex data (0x38 bytes); the original symbol is local to this unit's asm.
static u32 size32x32_tex32x32_vtx[0x38 / 4] = {
	0xFFF00000, 0x00000000, 0x00000000, 0x00FF0010,
	0x00000000, 0x04000000, 0x000000FF, 0x00100020,
	0x00000400, 0x04000000, 0x00FFFFF0, 0x00200000,
	0x00000400, 0x000000FF,
};

// .sbss2 colour scratch, global to this unit's asm

const Vec vec3_802faa50 = { 0.0f, 0.0f, 0.0f };
const char str_802FAA5C[] = "KumokumoChipN64";

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 chip_fpclassifyf(f32 x) {
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

#define fpclassify(x) chip_fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 chip_sqrtf(f32 x) {
	const f64 _half = .5;
	const f64 _three = 3.0;

	if (x > 0.0f) {
		f64 guess = __frsqrte(x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

// This entry point sits first in the .c (rule 18: reverse source order).
//
// `if (dist)` reproduces the original's `fcmpu cr0, f7, f3` + `beq`; writing
// `if (dist != 0.0f)` makes MWCC emit `fcmpu cr0, f3, f7`. The truth test is equivalent
// for every float (NaN included).
//
// The unit's pooled sqrtf doubles must sit at vec3+0x20. effKumokumoChipMain used to build
// its zero Vec from a compound literal, which made MWCC emit a dead 12-byte anonymous Vec
// in .rodata at vec3+0x1c and pushed the doubles to +0x28 (four wrong `lfd` offsets). Main
// now reads its userdata and the zero Vec through volatile lvalues under MUST_MATCH (the
// lwz/stw bytes are unchanged); that keeps the scheduler from hoisting the vec3 copy and
// leaves no literal object, so the doubles land at +0x20 and the whole file matches.
EffectEntry* effKumokumoChipN64Entry(s32 type, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 spacing, f32 scale) {
	EffectEntry* effect;
	effKumokumoChipData* data;
	f32 dx;
	f32 dy;
	f32 dz;
	f32 dist;
	s32 count;

	effect = effEntry();
	effect->type = str_802FAA5C;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKumokumoChipData));
	effect->userdata = data;
	effect->callback = effKumokumoChipMain;
	effect->flags |= 2;
	data->type = type;
	data->timer = 0;

	dx = x2 - x;
	dy = y2 - y;
	dz = z2 - z;
	dist = dx * dx + dy * dy + dz * dz;
	if (dist) {
		dist = chip_sqrtf(dist);
	}

	count = (spacing > float_0_8041f56c) ? (s32)(dist / spacing) : 0;
	if (count != 0) {
		f32 inv = float_1_8041f570 / (f32)count;
		data->xVel = dx * inv;
		data->yVel = dy * inv;
		data->zVel = dz * inv;
	} else {
		data->xVel = 0.0f;
		data->yVel = 0.0f;
		data->zVel = 0.0f;
	}

	if (count < 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = count;
	}

	data->a = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;

	switch (type) {
		case 0:
		case 2:
			data->r = 0xd7;
			data->g = 0xd7;
			data->b = 0xb9;
			data->r2 = 0xd7;
			data->g2 = 0xc8;
			data->b2 = 0x64;
			data->a2 = 0xff;
			break;
		default:
			data->r = 0xff;
			data->g = 0xff;
			data->b = 0xff;
			data->r2 = 0xdc;
			data->g2 = 0xf5;
			data->b2 = 0xff;
			data->a2 = 0xff;
			break;
	}
	return effect;
}

void effKumokumoChipMain(EffectEntry* effect) {
	effKumokumoChipData* data;
	s32 lifetime;
	s32 type;
	s32 timer;
	Vec pos2;
	Vec pos;

#ifdef MUST_MATCH
	// Volatile lvalues (same lwz bytes as the plain forms) stop the scheduler hoisting the
	// vec3 copy and stop MWCC emitting a dead anonymous zero Vec, so the unit's pool doubles
	// stay at vec3+0x20. See the comment above effKumokumoChipN64Entry.
	data = ((volatile EffectEntry*)effect)->userdata;
	pos = *(volatile Vec*)&vec3_802faa50;
#else
	data = effect->userdata;
	pos = vec3_802faa50;
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
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	lifetime = data->lifetime;
	if (lifetime < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;
	if (type == 0 || type == 2) {
		if (timer < 16) {
			data->a = (timer << 4) + 0xf;
		}
		else {
			data->a = 0xff;
		}
		if (lifetime < 16) {
			data->a = lifetime << 4;
		}
	}
	else {
		if (timer < 4) {
			data->a = (timer << 6) + 0x3f;
		}
		else {
			data->a = 0xff;
		}
		if (lifetime < 4) {
			data->a = lifetime << 6;
		}
	}
	data->x += data->xVel;
	data->y += data->yVel;
	data->z += data->zVel;

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effKumokumoChipDisp, effect);
}

// The patch-and-set of a TEV colour lives in this inline helper on purpose: it makes MWCC
// place the helper's GXColor local below its by-value argument temporary, which is the
// slot layout the original has.
static inline void chipSetColor(s32 id, s32 r, s32 g, s32 b, s32 a, GXColor* base) {
	GXColor color = *base;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(id, color);
}

void effKumokumoChipDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	effKumokumoChipData* data;
	Mtx mtx;
	Mtx scale;
	GXTexObj texObj;
	s32 alpha;

	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->a;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	chipSetColor(1, data->r, data->g, data->b, alpha, &unk_80423198);
	chipSetColor(2, data->r2, data->g2, data->b2, data->a2, &unk_8042319c);
	effGetTexObjN64(0x6b, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scale, float_0p03125_8041f568, float_0p03125_8041f568, float_0_8041f56c);
	GXLoadTexMtxImm(scale, 0x1e, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 4, 2, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size32x32_tex32x32_vtx);
	GXBegin(0x90, 0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p03125_8041f568 = 0.03125f;
const f32 float_0_8041f56c = 0.0f;
