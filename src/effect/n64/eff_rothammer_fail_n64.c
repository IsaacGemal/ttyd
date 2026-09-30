// MWCC rematerializes the shared 0.0f constant without this pragma, which drops one saved FPR
// (3 instead of 4) and shifts the whole prologue; keeping propagation off pins f31.
#pragma opt_propagation off

#include "eff/n64/eff_rothammer_fail_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <math.h>

typedef struct effRotHammerFailParticle {
	u8 field_0x00[4];    // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	f32 field_0x10;      // 0x10
	f32 field_0x14;      // 0x14
	f32 field_0x18;      // 0x18
	u8 field_0x1C[0x18]; // 0x1C
} effRotHammerFailParticle; // 0x34

typedef struct effRotHammerFailData {
	u16 type;                          // 0x00
	u8 pad_0x02[2];                    // 0x02
	f32 x;                             // 0x04
	f32 y;                             // 0x08
	f32 z;                             // 0x0C
	u8 field_0x10[0xC];                // 0x10
	f32 field_0x1C;                    // 0x1C
	f32 field_0x20;                    // 0x20
	s32 field_0x24;                    // 0x24
	s32 field_0x28;                    // 0x28
	s32 field_0x2C;                    // 0x2C
	f32 field_0x30;                    // 0x30
	effRotHammerFailParticle particle; // 0x34
} effRotHammerFailData;

// The original walks the effect's two 0x34-byte records as an array; this is the record view the
// Main callback iterates (the combined effRotHammerFailData above is the Entry allocation view).
typedef struct effRotHammerFailRecord {
	u16 type;        // 0x00
	u8 pad_0x02[2];  // 0x02
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 field_0x10;  // 0x10
	f32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	s32 field_0x24;  // 0x24
	s32 field_0x28;  // 0x28
	s32 field_0x2C;  // 0x2C
	f32 field_0x30;  // 0x30
} effRotHammerFailRecord; // 0x34

// The target keeps the header base in volatile r3 and materializes `particle = data + 0x34` in
// r30 (addi r30, r3, 0x34); MWCC folds the particle pointer back into the live data base, so
// this version reloads userdata (lwz r30, 0xc(r31)) and uses data+0x38/... instead of
// particle+0x4/... . Everything else (FPR saves, header stores, call sequence, epilogue) matches.
// NONMATCHING (88.74%): see above; particle base/offset pair is the only real difference.
EffectEntry* effRotHammerFailN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effRotHammerFailData* data;
	effRotHammerFailParticle* particle;
	u8* base;
	f32 zero;

	effect = effEntry();
	effect->type = "RotHammerFailN64";
	effect->count = 2;
	data = __memAlloc(HEAP_EFFECT, sizeof(effRotHammerFailData));
	effect->userdata = data;
	effect->callback = effRotHammerFailMain;
	zero = 0.0f;
	base = (u8*)effect->userdata;
	particle = (effRotHammerFailParticle*)(base + 0x34);
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x1C = 10.0f;
	data->field_0x20 = 30.0f;
	data->field_0x24 = life;
	data->field_0x28 = 0;
	data->field_0x2C = life;
	particle->z = particle->y = particle->x = data->field_0x30 = zero;
	particle->field_0x10 = sin(zero);
	particle->field_0x14 = zero;
	particle->field_0x18 = cos(zero);
	return effect;
}

void effRotHammerFailMain(EffectEntry* effect) {
	effRotHammerFailRecord* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail copies a shared zero Vec into a scratch slot, overwrites it with
	   data->x/y/z, then copies it into a second local before dispCalcZ (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 i;
	f32 scale;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->field_0x30 = float_7_8041fbe4 * data->field_0x28 / data->field_0x2C;
	data->field_0x28 += 1;
	data->field_0x24 -= 1;
	if (data->field_0x24 < 0) {
		effDelete(effect);
		return;
	}

	data->field_0x1C += float_0p1_8041fbe8 * (data->field_0x20 - data->field_0x1C);
	scale = data->field_0x1C;
	for (i = 1, data++; i < effect->count; i++, data++) {
		data->field_0x14 += float_0p04_8041fbec;
		data->field_0x14 *= float_0p9_8041fbf0;
		data->x = scale * data->field_0x10;
		data->y += data->field_0x14;
		data->z = scale * data->field_0x18;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRotHammerFailDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effRotHammerFailDisp, effect);
#endif
}
