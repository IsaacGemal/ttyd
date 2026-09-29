// MWCC rematerializes the shared 0.0f constant without this pragma, which drops one saved FPR
// (3 instead of 4) and shifts the whole prologue; keeping propagation off pins f31.
#pragma opt_propagation off

#include "eff/n64/eff_rothammer_fail_n64.h"
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
