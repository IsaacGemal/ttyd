#include "eff/eff_particle.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

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

// The inline sqrtf below and the two Vec constants share the original .rodata pool.
#pragma pool_data on

// .sdata2 constants. The matching build keeps them writable so the loads reference the
// original symbols instead of being folded into MWCC's own pool.
#ifdef MUST_MATCH
#define SDATA2_CONST
#else
#define SDATA2_CONST const
#endif

SDATA2_CONST f32 float_4_80421ce0 = 4.0f;
SDATA2_CONST f32 float_8_80421ce4 = 8.0f;
SDATA2_CONST f32 float_deg2rad_80421ce8 = 0.017453292f;
SDATA2_CONST f32 float_0p5_80421cec = 0.5f;
SDATA2_CONST f32 float_0_80421cf0 = 0.0f;
SDATA2_CONST f32 float_2_80421cf4 = 2.0f;
SDATA2_CONST f32 float_1_80421cf8 = 1.0f;
SDATA2_CONST f32 float_neg0p03_80421cfc = -0.03f;
SDATA2_CONST f32 float_5_80421d00 = 5.0f;
SDATA2_CONST f32 float_125_80421d04 = 125.0f;
SDATA2_CONST f32 float_25_80421d08 = 25.0f;
SDATA2_CONST f32 float_1000_80421d0c = 1000.0f;
SDATA2_CONST f32 dat_80421d10 = 0.00000000023283064f;
SDATA2_CONST f32 float_neg2_80421d14 = -2.0f;
SDATA2_CONST f32 float_0p25_80421d18 = 0.25f;
SDATA2_CONST f32 float_2p5_80421d1c = 2.5f;

// .rodata
// The zero vectors share the sdata2 trick: non-const under MUST_MATCH so the loads reference the
// original symbols (and MWCC keeps the struct copy instead of folding it), const otherwise.
#ifdef MUST_MATCH
__declspec(section ".rodata") Vec vec3_80300260 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_8030026c = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_80300260 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_8030026c = { 0.0f, 0.0f, 0.0f };
#endif
const f64 double_to_int_mask_80300278 = 4503599627370496.0;
const char str_80300280[12] = "Particle";

// .data. Each entry is one packed GXColor (r, g, b, a).
u32 hibana_col[] = {
	0x5F280000, 0x69350008, 0x73420010, 0x7D4F0018, 0x885D0020, 0x926A0029, 0x9D780031, 0xA7850039,
	0xB1920041, 0xBCA0004A, 0xC6AD0052, 0xD0BA005A, 0xDBC80062, 0xE5D5006A, 0xEFE20073, 0xFAF0007B,
	0xFFF00083, 0xFFF1108B, 0xFFF22194, 0xFFF3329C, 0xFFF444A4, 0xFFF555AC, 0xFFF666B4, 0xFFF777BD,
	0xFFF888C5, 0xFFF999CD, 0xFFFAAAD5, 0xFFFBBBDE, 0xFFFCCCE6, 0xFFFDDDEE, 0xFFFEEEF6, 0xFFFFFFFF,
};

// .sdata
u32 seed = 1;

// .sbss

#define particle_fpclassify(x) __fpclassifyf(x)
#define particle_isnan(x) (particle_fpclassify(x) == FP_NAN)

// Inlined MetroWerks sqrtf. The double literals become .rodata pool entries (double_0p5,
// double_3, double_0) that the original references through the vec3_80300260 base register.
static inline f32 particle_sqrtf(f32 x) {
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
	} else if (particle_isnan(x)) {
		return NAN;
	}
	return x;
}

// What is left: (a) the two volatile FP temporaries in the top/left/bottom init rotate - the target
// negates float_8 into f1 and keeps 0.5 in f0, MWCC negates the 0.5 literal instead; writing
// -float_8*float_0p5 gets the negation right but rotates the whole temp set. (b) The GX vertex uses
// literal 0.0f/1.0f and deg2rad/right use literals, so those loads (plus the compiler's own u32->f32
// double) reference anonymous .sdata2/.rodata entries instead of float_0/float_1/float_deg2rad/
// float_2/double_to_int_mask_80300278. Mtx declaration order (trans, rot, scale, local) gives the
// original stack slots; declaring alpha, i, particle in that order gives the original r24/r25/r26;
// the literals force the per-vertex reloads the original has.
// NONMATCHING (99.22%): FP multiply temporaries rotate, and literal constants make the pool
// relocations anonymous.
void effParticleDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	ParticleWork* work = effect->userdata;
	CameraEntry* camera;
	GXTexObj texObj;
	Mtx trans;
	Mtx rot;
	Mtx scale;
	Mtx local;
	GXColor color;
	f32 deg2rad;
	f32 top;
	f32 left;
	f32 bottom;
	f32 right;
	f32 factor;
	s32 alpha;
	s32 i;
	ParticleEntry* particle;

	camera = camGetPtr(camId);
	alpha = work->alpha;
	effGetTexObj(0x61, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	PSMTXTrans(trans, work->position.x, work->position.y, work->position.z);
	PSMTXScale(scale, work->scale, work->scale, work->scale);
	PSMTXConcat(trans, scale, local);

	top = float_4_80421ce0;
	left = -top * float_0p5_80421cec;
	bottom = -float_0p5_80421cec * float_8_80421ce4;
	deg2rad = 0.017453292f;
	right = 2.0f;

	i = work->particles[work->count].next;
	while (i != work->count + 1) {
		particle = &work->particles[i];
		color = *(GXColor*)&hibana_col[particle->life >> 3];
		color.a = (alpha * color.a) / 255;
		GXSetChanMatColor(GX_COLOR0A0, color);
		PSMTXTrans(trans, particle->position.x, particle->position.y, particle->position.z);
		PSMTXConcat(local, trans, trans);
		PSMTXConcat(camera->view, trans, trans);
		PSMTXRotRad(rot, 'y', deg2rad * -camGetPtr(camId)->viewYaw);
		PSMTXConcat(trans, rot, trans);
		factor = (f32)((gp->retraceCount + i) % 360 * 10);
		PSMTXRotRad(rot, 'x', deg2rad * factor);
		PSMTXConcat(trans, rot, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(left, top, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(right, top, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(right, bottom, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(left, bottom, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
		i = particle->next;
	}
}

// Reproduces the original's separate scratch Vec: the compiler keeps the returned temp, and the
// 0x14 copy is what dispCalcZ sees.
static inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = vec3_8030026c;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// The two-slot position Vec matches because `vec3_8030026c` is non-const under MUST_MATCH (see the
// declaration above): a const zero vector lets MWCC fold the copy and hoist `lwz r4,0x0(r5)` above
// the userdata load; the non-const global keeps the copy and the load stays put.
void effParticleMain(EffectEntry* effect) {
	ParticleWork* work = effect->userdata;
	Vec position;
	Vec position2;
	ParticleEntry* particle;
	ParticleEntry* prev;
	s32 i;

	position = makeVec(work->position.x, work->position.y, work->position.z);
	position2 = position;

	if (effect->flags & 0x4) {
		effect->flags &= ~0x4;
		work->life = 16;
	}
	if (work->life < 1000) {
		work->life--;
	}
	if (work->life < 0) {
		effDelete(effect);
		return;
	}
	if (work->life < 16) {
		work->alpha = work->life << 4;
	}
	work->frame++;
	EmitterEmit(effect, &work->emitter);
	work = effect->userdata;

	prev = &work->particles[work->count];
	i = prev->next;
	while (i != work->count + 1) {
		particle = &work->particles[i];
		particle->life--;
		if (particle->life < 0) {
			prev->next = particle->next;
			particle->next = work->particles[work->count + 1].next;
			work->particles[work->count + 1].next = i;
			work->head--;
		} else {
			PSVECAdd(&particle->position, &particle->velocity, &particle->position);
			particle->velocity.y += float_neg0p03_80421cfc;
			prev = particle;
		}
		i = prev->next;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effParticleDisp, effect);
}

// same idea as makeVec but over vec3_80300260 (the entry's direction scratch)
static inline Vec makeVec2(f32 x, f32 y, f32 z) {
	Vec temp = vec3_80300260;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// NONMATCHING (91.74%): the direction Vec/two-slot copy, the `w2` work reload (`0xc(r30)`) in the loop/head
// and the emitter stores all match (non-const `vec3_80300260` + the MUST_MATCH constant array). Still differs:
// the emitter-constant load order (0/5/125/25 vs 25/125/5/0), `frame=0`'s temp register, and the angleX stfs.
EffectEntry* effParticleEntry(s32 type, s32 count, s32 life, f32 x, f32 y, f32 z, f32 velX, f32 velY, f32 velZ) {
	EffectEntry* entry = effEntry();
	ParticleWork* work;
	s32 i;

	entry->type = str_80300280;
	entry->count = 1;
	work = __memAlloc(HEAP_EFFECT, entry->count * 0xB0 + (count + 2) * sizeof(ParticleEntry));
	entry->userdata = work;
	entry->callback = effParticleMain;
	entry->flags |= 2;
	work->type = type;
	work->position.x = x;
	work->position.y = y;
	work->position.z = z;
	work->velocity.x = velX;
	work->velocity.y = velY;
	work->velocity.z = velZ;
	if (life <= 0) {
		work->life = 1000;
	} else {
		work->life = life;
	}
	work->frame = 0;
	work->scale = float_1_80421cf8;
	work->alpha = 0xFF;
	work->count = count;
	work->particles = (ParticleEntry*)((u8*)work + entry->count * 0xB0);
	work->field_0xAC = float_4_80421ce0;
	{
		ParticleWork* w2 = entry->userdata;

		for (i = 1; i < w2->count + 2; i++) {
			w2->particles[i - 1].next = i;
		}
		w2->particles[i - 1].next = 0;
		w2->head = 0;
	}
	PSMTXIdentity(work->emitter.matrix);
#ifdef MUST_MATCH
	// MUST_MATCH: this array order makes MWCC put the four constants in the original's f3/f2/f1/f0
	// (0.0, 5.0, 125.0, 25.0). The natural field-by-field form is under #else.
	{
		f32 c[4];

		c[0] = float_25_80421d08;
		c[1] = float_125_80421d04;
		c[2] = float_5_80421d00;
		c[3] = float_0_80421cf0;
		work->emitter.field_0x3C.z = c[3];
		work->emitter.field_0x3C.y = c[3];
		work->emitter.field_0x3C.x = c[3];
		work->emitter.field_0x48 = work->emitter.field_0x3C;
		work->emitter.field_0x54 = c[3];
		work->emitter.field_0x58 = c[2];
		work->emitter.field_0x5C = c[1];
		work->emitter.field_0x60 = c[0];
	}
#else
	work->emitter.field_0x3C.z = float_0_80421cf0;
	work->emitter.field_0x3C.y = float_0_80421cf0;
	work->emitter.field_0x3C.x = float_0_80421cf0;
	work->emitter.field_0x48 = work->emitter.field_0x3C;
	work->emitter.field_0x54 = float_0_80421cf0;
	work->emitter.field_0x58 = float_5_80421d00;
	work->emitter.field_0x5C = float_125_80421d04;
	work->emitter.field_0x60 = float_25_80421d08;
#endif
	{
		Vec direction;
		Vec direction2;

		direction = makeVec2(work->velocity.x, work->velocity.y, work->velocity.z);
		direction2 = direction;
		PSVECNormalize(&direction2, &direction2);
		work->angleX = -angleABf(float_0_80421cf0, float_0_80421cf0, float_1000_80421d0c * direction2.z,
		                         float_1000_80421d0c * direction2.y);
		work->angleZ = -angleABf(float_0_80421cf0, float_0_80421cf0, float_1000_80421d0c * direction2.x,
		                         float_1000_80421d0c * direction2.y);
	}
	{
		Mtx rotX;
		Mtx rotZ;
		PSMTXRotRad(rotX, 'x', float_deg2rad_80421ce8 * work->angleX);
		PSMTXRotRad(rotZ, 'z', float_deg2rad_80421ce8 * work->angleZ);
		PSMTXConcat(rotX, rotZ, work->emitter.matrix);
	}
	return entry;
}

// dont_inline: the original calls EmitterEmit from effParticleMain (deferred inlining would pull it in).
#pragma dont_inline on
void EmitterEmit(EffectEntry* effect, ParticleEmitter* emitter) {
	ParticleWork* work = effect->userdata;
	u32 oldCount;
	u32 newCount;
	s32 node;
	u32 i;

	oldCount = (u32)emitter->field_0x54;
	emitter->field_0x54 += emitter->field_0x58;
	newCount = (u32)emitter->field_0x54 - oldCount;
	work->head += newCount;
	if (work->head > work->count) {
		newCount -= work->head - work->count;
		work->head = work->count;
	}
	if (newCount != 0) {
		node = work->count + 1;
		for (i = 0; i < newCount; i++) {
			node = work->particles[node].next;
			ParticleEmit(effect, &work->particles[node], emitter);
		}
		{
			s32 saved = work->particles[work->count].next;

			work->particles[work->count].next = work->particles[work->count + 1].next;
			work->particles[work->count + 1].next = work->particles[node].next;
			work->particles[node].next = saved;
		}
	}
}
#pragma dont_inline reset

// Every instruction/call/immediate matches; the residue is the allocator and the data layout. MWCC pools
// its u32->f32 2^52 constant at 0x48(r31) because the file declares double_to_int_mask_80300278 as a user global
// at 0x18(r31), which the original did not (its 0x18 is the anonymous pool double); when the file is ordered and
// laid out as the original that offset matches. The first loop's FP scratch registers rotate (ours
// float_1/dat/float_2/mask/product = f3/f4/f5/f2/f1 vs the target's float_2/dat/mask/float_1/product = f5/f4/f3/f1/f2),
// and the second loop contracts float_2*(dat*conv)-float_1 to fmsubs where the target emits two fmuls + fsubs.
// Hoisting the constants into block locals (the shape below) plus opt_lifetimes off gets closest to the target's
// colouring. opt_lifetimes off: the default lifetime pass colours the loop constants differently (used elsewhere in
// the tree for the same allocator nudge).
// NONMATCHING (91.50%): only the FP scratch-register rotation and the 2^52 pool slot differ, per above.
#pragma opt_lifetimes off
void ParticleEmit(EffectEntry* effect, ParticleEntry* particle, ParticleEmitter* emitter) {
	static s32 use_last;
	static f32 y2;
	ParticleWork* work = effect->userdata;
	f32 random;
	f32 z;
	f32 x;
	f32 radius;
	f32 factor;

	if (use_last) {
		use_last = 0;
		random = y2;
	} else {
		f32 c1 = float_1_80421cf8;
		f32 cd = dat_80421d10;
		f32 c2 = float_2_80421cf4;
		do {
			seed = seed * 0x5EEDF715 + 0x1B0CB173;
			x = cd * (f32)(u32)seed * c2 - c1;
			seed = seed * 0x5EEDF715 + 0x1B0CB173;
			z = cd * (f32)(u32)seed * c2 - c1;
			radius = x * x + z * z;
		} while (radius > c1);
		factor = particle_sqrtf(float_neg2_80421d14 * (f32)log(radius) / radius);
		random = x * factor;
		use_last = 1;
		y2 = z * factor;
	}

	particle->life = (s32)(emitter->field_0x60 * random + emitter->field_0x5C);

	{
		f32 c1 = float_1_80421cf8;
		f32 cd = dat_80421d10;
		f32 c2 = float_2_80421cf4;
		do {
			seed = seed * 0x5EEDF715 + 0x1B0CB173;
			particle->velocity.x = cd * (f32)(u32)seed * c2 - c1;
			seed = seed * 0x5EEDF715 + 0x1B0CB173;
			particle->velocity.z = cd * (f32)(u32)seed * c2 - c1;
			radius = particle->velocity.x * particle->velocity.x + particle->velocity.z * particle->velocity.z;
		} while (radius > c1);
	}
	particle->velocity.x *= float_0p5_80421cec;
	particle->velocity.z *= float_0p5_80421cec;
	radius *= float_0p25_80421d18;
	particle->velocity.y = particle_sqrtf(float_1_80421cf8 - radius);

	PSVECScale(&particle->velocity, &particle->velocity, work->field_0xAC);
	PSMTXMultVec(emitter->matrix, &particle->velocity, &particle->velocity);
	PSVECScale(&particle->velocity, &particle->position, float_2p5_80421d1c);
	PSVECAdd(&emitter->position, &particle->position, &particle->position);
}
#pragma opt_lifetimes reset
