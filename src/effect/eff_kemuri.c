#include "eff/eff_kemuri.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include "memory.h"

typedef struct effKemuriData {
	s32 field_0x0; //0x0
	f32 field_0x4; //0x4
	f32 field_0x8; //0x8
	f32 field_0xC; //0xC
	u8 field_0x10[0x40 - 0x10]; //0x10
	s32 field_0x40; //0x40
	f32 field_0x44; //0x44
	f32 field_0x48; //0x48
	f32 field_0x4C; //0x4C
	u8 field_0x50[0x58 - 0x50]; //0x50
	f32 field_0x58; //0x58
	f32 field_0x5C; //0x5C
	u8 field_0x60[0x74 - 0x60]; //0x60
	f32 field_0x74; //0x74
	u8 field_0x78[0x80 - 0x78]; //0x78
	s32 field_0x80; //0x80
	s32 field_0x84; //0x84
	s32 field_0x88; //0x88
} effKemuriData;

// local per the original: .fn effKemuri2Disp, local
static void effKemuri2Disp(CameraId camId, void* param);

// NONMATCHING (99.80%): same instructions; only register numbers or operands differ.
EffectEntry* effKemuriEntry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6) {
	effKemuriData* data;
	s32 numEffects, i;
	
	EffectEntry* entry = effEntry();
	numEffects = 1;
	switch (int2) {
		case 0:
			numEffects = 3;
			break;
		case 1:
			numEffects = 2;
			break;
		case 2:
			numEffects = 2;
			break;
	}
	entry->type = "Kemuri";
	entry->count = numEffects;
	data = (effKemuriData*)__memAlloc(HEAP_EFFECT, sizeof(effKemuriData) * numEffects);
	entry->userdata = data;
	entry->callback = effKemuriMain;

	data->field_0x0 = int2;
	data->field_0x4 = float3;
	data->field_0x8 = float4;
	data->field_0xC = float5;

	data->field_0x40 = 0;

	data->field_0x44 = 1.0f;
	data->field_0x48 = 1.0f;
	data->field_0x74 = 1.0f;
	for (i = 1; i < entry->count; i++) {
		data[i].field_0x4 = 0.0f;
		data[i].field_0x8 = 0.0f;
		data[i].field_0xC = 0.0f;

		data[i].field_0x44 = 1.0f;
		data[i].field_0x48 = 1.0f;
		data[i].field_0x74 = 1.0f;

		data[i].field_0x58 = 0.0f;
		data[i].field_0x5C = 0.0f;

		data[i].field_0x40 = 0;
		data[i].field_0x80 = 0;
		data[i].field_0x84 = 0;

		switch (int2) {
			case 0:
				data[i].field_0x4C = i & 1 ? 3.0f : -3.0f;
				data[i].field_0x88 = 32;
				break;
			case 1:
				data[i].field_0x4C = 0.0f;
				data[i].field_0x88 = 32;
				break;
			case 2:
				data[i].field_0x4C = 0.0f;
				data[i].field_0x88 = 64;
				break;
		}
	}
	return entry;
}

// NONMATCHING (0.52%): empty placeholder, not decompiled yet.
void effKemuriMain(EffectEntry* effect) {

}

// All instructions are present; the only differences are register allocation.
// NONMATCHING (98.63%): the original loads effect->userdata into r7 then copies it to the
// callee-saved r31 before the sin call (we coalesce the load straight into r31), and the alpha
// block's int->float conversion uses f2/f0/f1 where we use f1/f2/f0.
static void effKemuri2Main(EffectEntry* effect) {
	effKemuri2Data* data = effect->userdata;
	s32 i;
	f32 angle;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	angle = (f32)sin(3.1415927f * (f32)data->counter / 30.0f);
	data->counter++;
	if (data->counter > 30) {
		effDelete(effect);
		return;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->x += data->field_0x10 * data->field_0x30;
		data->z += data->field_0x10 * data->field_0x34;
		data->field_0x20 *= 0.9f;
		data->y += data->field_0x20;
		data->field_0x64 *= 0.95f;
		data->field_0x68 += data->field_0x64;
		data->field_0x70 = angle;
		if (data->counter > 15) {
			data->alpha = (s32)((f32)data->alpha * 0.8f);
		}
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri2Disp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKemuri2Disp, effect);
#endif
}

// NONMATCHING (0.25%): 399 missing.
void effKemuriDisp(CameraId camId, void* param) {
	EffectEntry* entry = param;
}
