#include "eff/n64/eff_rappa_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

// .rodata: zero vector the per-effect position helper starts from.
const Vec vec3_802fb4a0 = { 0.0f, 0.0f, 0.0f };

// .rodata: int->float conversion magic double.
const f64 double_to_int_802fb4b0 = 4503601774854144.0;

// .rodata: effect type name.
const char str_802FB4B8[] = "RappaN64";

// Local copy of system.c's inline helper so it inlines here (as in eff_sstar.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effRappaN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	effRappaData* data;
	effRappaData* p;
	s32 i;
	f32 theta;
	f32 scale;
	f32 sinTheta;
	f32 phi;
#ifdef MUST_MATCH
	// MUST_MATCH: these temporaries pin MWCC's FPR assignment and commutative-operand order; the
	// #else branch is the natural form and does not match.
	f32 tmp;
	f32 v;
	f32 c;
#endif

	effect = effEntry();
	if (count == 0) {
		return NULL;
	}
	effect->type = str_802FB4B8;
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, (count + 1) * sizeof(effRappaData));
	effect->userdata = data;
	effect->callback = effRappaMain;
#ifdef MUST_MATCH
	tmp = float_6p2832_8041faf8 * a4;
#else
	theta = float_6p2832_8041faf8 * a4 / float_360_8041fb04;
#endif
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->life = 0x1E;
	data->counter = 0;
	data->alpha = 0xFF;

	p = data + 1;
	for (i = 1; i < count + 1; i++, p++) {
#ifdef MUST_MATCH
		v = (f32)(rand() % 10);
		scale = v * float_0p25_8041fb00 + float_5_8041fafc;
		theta = tmp / float_360_8041fb04;
#else
		scale = (f32)(rand() % 10) * float_0p25_8041fb00 + float_5_8041fafc;
#endif
		p->type = type;
		p->position.x = x;
		p->position.y = y;
		p->position.z = z;
		sinTheta = (f32)sin(theta);
		phi = float_6p2832_8041faf8 * (a5 + (f32)((i & 2) * (i / 2) * 4)) / float_360_8041fb04;
#ifdef MUST_MATCH
		c = (f32)cos(phi);
		p->field_0x10 = c * sinTheta;
#else
		p->field_0x10 = (f32)cos(phi) * sinTheta;
#endif
		p->field_0x14 = (f32)sin(phi);
		p->field_0x18 = (f32)cos(phi) * (f32)cos(theta);
		p->scale = scale;
		p->field_0x24 = scale / float_10_8041faf4;
	}
	return effect;
}

// NONMATCHING (98.29%): the original loads effect->userdata into r7 before the vec3_802fb4a0 words; MWCC GC/2.6 hoists the first two global loads into the prologue slots instead. Everything else matches.
void effRappaMain(EffectEntry* effect) {
	EffectEntry* e;
	effRappaData* data;
	Vec position;
	s32 i;
	s32 life;

	e = effect;
	data = e->userdata;
	position = initializeVec(&data->position);

	data->life--;
	data->counter++;
	life = data->life;
	if (life < 0) {
		effDelete(e);
		return;
	}
	if (life < 10) {
		data->alpha = (s32)((f32)data->alpha * float_0p8_8041faec);
	}

	data++;
	for (i = 1; i < e->count; i++, data++) {
		data->position.x += data->scale * data->field_0x10;
		data->position.y += data->scale * data->field_0x14;
		data->position.z += data->scale * data->field_0x18;
		data->scale *= float_0p9_8041faf0;
		if (life < 10) {
			data->field_0x24 += data->scale / float_10_8041faf4;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRappaDisp, e);
}
