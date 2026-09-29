#include "eff/n64/eff_fall_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include <dolphin/mtx.h>
#include <math.h>

typedef struct effFallLeafParticle {
	s32 field_0x00;       // 0x00
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	f32 field_0x10;       // 0x10
	f32 field_0x14;       // 0x14
	f32 field_0x18;       // 0x18
	f32 field_0x1C;       // 0x1C
	f32 field_0x20;       // 0x20
	u8 field_0x24[0x0C];  // 0x24
} effFallLeafParticle;

typedef struct effFallLeafData {
	s32 type;                        // 0x00
	f32 x;                           // 0x04
	f32 y;                           // 0x08
	f32 z;                           // 0x0C
	u8 field_0x10[0x14];             // 0x10
	s32 field_0x24;                  // 0x24
	s32 life;                        // 0x28
	s32 counter;                     // 0x2C
	effFallLeafParticle particle[4]; // 0x30
} effFallLeafData;

extern const f32 float_0p3_8041edd4;
extern const f32 float_0p8_8041edd8;
extern const f32 float_6p2832_8041eddc;
extern const f32 float_2_8041ede0;
extern const f32 float_360_8041ede4;
extern const f32 float_neg0p05_8041ede8;
extern const f32 float_0p2_8041edec;
extern const f32 float_0p94_8041edf0;
extern const f32 float_0p95_8041edf4;
extern const f32 float_0p05_8041edf8;
extern const f32 float_0p92_8041edfc;
extern const f32 float_10_8041ee00;
extern const f32 float_0p5_8041ee04;

const Vec vec3_802fa538 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa548 = 4503601774854144.0;

// The code and the original's instruction sequence line up; what still
// differs is float callee-saved register allocation. The original puts the loop-invariant
// constants in f25..f31 with the last one (0.5) wrapping to f24 (load order 6.2832, 2, 360,
// -0.05, 0.2, double_to_int, 10, 0.5); MWCC instead assigns them f24..f31, so every constant
// operand in the loop is one register low. The few non-register diffs are the hoisted setup
// order (part of the constant loads around the `i = 1` / particle-pointer init) that follows
// from that allocation; declaring the 0.5 as a variable does move it to f24 and fixes the
// constants but hoists its load to the top and scores lower.
// NONMATCHING (95.41%): float callee-saved register allocation is one register low (see above).
void effFallLeafMain(EffectEntry* effect) {
	effFallLeafData* data = effect->userdata;
	effFallLeafParticle* particle;
	Vec position2;
	Vec position;
	s32 type;
	s32 life;
	s32 counter;
	s32 i;
	s32 idx;
	f32 s;
	f32 t;

	position = vec3_802fa538;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->life--;
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	type = data->type;
	life = data->life;
	if (counter < 10) {
		data->field_0x24 = (s32)(float_0p3_8041edd4 * (f32)(0xFF - data->field_0x24) + (f32)data->field_0x24);
	}
	if (life < 10) {
		data->field_0x24 = (s32)((f32)data->field_0x24 * float_0p8_8041edd8);
	}
	idx = counter + 20;
	i = 1;
	particle = &data->particle[0];
	for (; i < effect->count; i++, particle++, idx += 20) {
		s = (f32)sin(float_6p2832_8041eddc * (float_2_8041ede0 * particle->field_0x18) / float_360_8041ede4);
		particle->field_0x14 += float_neg0p05_8041ede8;
		t = float_0p2_8041edec * s;
		particle->field_0x10 += t;
		switch (type) {
		case 0:
		case 2:
		case 3:
		case 4:
			particle->field_0x10 *= float_0p94_8041edf0;
			particle->field_0x14 *= float_0p05_8041edf8 * (f32)sin(float_6p2832_8041eddc * particle->field_0x18 / float_360_8041ede4) + float_0p95_8041edf4;
			break;
		case 1:
			particle->field_0x14 += float_neg0p05_8041ede8;
			particle->field_0x10 *= float_0p92_8041edfc;
			break;
		}
		particle->field_0x1C += (f32)(s32)(effTblRandN64(50, idx) - 25);
		particle->field_0x18 += float_10_8041ee00 * (f32)sin(float_6p2832_8041eddc * particle->field_0x1C / float_360_8041ede4);
		particle->field_0x20 += float_10_8041ee00 * (f32)cos(float_6p2832_8041eddc * (particle->field_0x1C * float_0p5_8041ee04) / float_360_8041ede4);
		particle->x += particle->field_0x10;
		particle->y += particle->field_0x14;
	}
	dispEntry(4, dispCalcZ(&position2), 2, effFallLeafDisp, effect);
}
