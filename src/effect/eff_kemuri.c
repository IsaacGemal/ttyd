#include "eff/eff_kemuri.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"

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

// Named sdata2 constants of the original unit (definitions live in the original object's data).
extern const f32 float_1_8041c6b0;
extern const f32 float_0_8041c6bc;
extern const f32 float_360_8041c70c;
extern const f32 float_1000_8041c710;
extern const f32 float_6p2832_8041c714;
extern const f32 float_5_8041c718;
extern const f32 float_0p7_8041c71c;
extern const f32 float_neg20_8041c720;
extern const char str_802F3120[];

// local per the original: .fn effKemuri2Disp, local
static void effKemuri2Disp(CameraId camId, void* param);
// local per the original: .fn effKemuri2Main, local
static void effKemuri2Main(EffectEntry* effect);

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

// NONMATCHING (94.30%): the int->float conversion pools an anonymous @204 double instead of
// double_to_int_802f3110, and the allocator maps the angle (f27/f28 vs f24) and the 1.0/0.7/-20/0.0
// temporaries (f25-f27/f31 vs f26-f28/f31) one register apart; structure, args and constants match.
EffectEntry* effKemuri2Entry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6, f32 float7) {
	EffectEntry* entry = effEntry();
	effKemuri2Data* data;
	f32 one;
	f32 p7;
	f32 neg20;
	f32 zero;
	f32 cosVal;
	f32 sinVal;
	f32 angle;
	f32 z;

	entry->type = str_802F3120;
	entry->count = 1;
	data = (effKemuri2Data*)__memAlloc(HEAP_EFFECT, sizeof(effKemuri2Data));
	entry->userdata = data;
	memset(data, 0, sizeof(effKemuri2Data));
	entry->callback = effKemuri2Main;

	sinVal = float_1000_8041c710 * float6;
	cosVal = float_1000_8041c710 * float7;
	angle = float_360_8041c70c * angleABf(float_0_8041c6bc, float_0_8041c6bc, sinVal, cosVal) / float_6p2832_8041c714;
	data->field_0x78 = reviseAngle(float_360_8041c70c * angleABf(float_0_8041c6bc, float_0_8041c6bc, sinVal, cosVal) / float_6p2832_8041c714 + camGetPtr(CAMERA_3D)->viewYaw);
	data->counter = 0;
	z = float5 + float_5_8041c718;
	angle = float_6p2832_8041c714 * (f32)(s32)angle / float_360_8041c70c;
	one = float_1_8041c6b0;
	p7 = float_0p7_8041c71c;
	neg20 = float_neg20_8041c720;
	zero = float_0_8041c6bc;
	data->field_0x30 = sin(angle);
	data->field_0x34 = cos(angle);
	data->x = float3;
	data->y = float4;
	data->z = z;
	data->field_0x10 = one;
	data->field_0x20 = p7;
	data->field_0x64 = neg20;
	data->field_0x68 = zero;
	data->alpha = 0xFF;
	data->field_0x70 = one;
	data->field_0x74 = one;
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
