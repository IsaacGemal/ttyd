#include "eff/eff_sleep.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <math.h>

//local prototypes

EffectEntry* effSleepEntry(s32 a1, s32 a2, f32 x, f32 y, f32 z, f32 scale, f32 rotation) {
	EffectEntry* effect;
	effSleepData* data;
	s32 smth;
	effSleepData* work;
	s32 i;
	f32 radians;
	f32 radius;

	effect = effEntry();
	effect->type = "Sleep";
	effect->count = 4;
	data = __memAlloc(HEAP_EFFECT, effect->count * sizeof(effSleepData));
	effect->userdata = data;
	effect->callback = effSleepMain;
	effect->flags |= 2;
	data->field_0x0 = a1;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->field_0x14 = 0;
	if (a2 <= 0) {
		data->field_0x18 = 1000;
	}
	else {
		data->field_0x18 = a2;
	}
	data->field_0x1C = 0;

	radians = (6.2831855f * rotation) / 360.0f;
	work = data + 1;
	for (i = 1, smth = 16; i < effect->count; i++, smth += 16, work++) {
#ifdef MUST_MATCH
		// The original computes 16*i between the cos call and the multiply; assigning radius
		// inside the expression makes the compiler schedule it that way.
		work->position.x = (radius = 16.0f * (f32)i) * (f32)cos(radians);
#else
		radius = 16.0f * (f32)i;
		work->position.x = radius * (f32)cos(radians);
#endif
		work->position.y = radius * (f32)sin(radians);
		work->position.z = 0.0f;
		work->scale = (0.2f * (f32)i) + 1.0f;
		work->field_0x14 = 0;
		work->field_0x20 = smth;
	}
	return effect;
}

// NONMATCHING (1.08%): empty placeholder, not decompiled yet.
void effSleepMain(EffectEntry* effect) {

}

// NONMATCHING (0.32%): empty placeholder, not decompiled yet.
void effSleepDisp(CameraId camId, EffectEntry* effect) {

}
