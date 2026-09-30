#include "eff/n64/eff_balloon_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <math.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_magic2.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effBalloonN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	effBalloonN64Data* data;

	effect = effEntry();
	effect->type = "BalloonN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBalloonN64Data));
	effect->userdata = data;
	effect->callback = effBalloonMain;
	effect->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x1C = 100;
	data->field_0x14 = a4;
	data->field_0x18 = 0.0f;
	data->field_0x20 = 0;
	return effect;
}

// NONMATCHING (97.29%): only callee-saved register colours differ; the instruction sequence is identical.
void effBalloonMain(EffectEntry* effect) {
	effBalloonN64Data* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 life;
	BalloonSeg* segs;
	s32 phase;
	f32 radius;
	f32 angle;
	f32 wave;
	s32 i;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x1C = 99;
	}
	if (data->field_0x1C < 100) {
		data->field_0x1C--;
	}
	data->field_0x20++;
	if (data->field_0x20 > 324000) {
		data->field_0x20 = 256;
	}
	life = data->field_0x1C;
	if (life < 0) {
		effDelete(effect);
	} else {
		phase = data->field_0x20;
		data->field_0x18 = data->field_0x18 + 0.1f * (data->field_0x14 - data->field_0x18);
		radius = data->field_0x18 +
		         0.1f * (data->field_0x18 * (f32)sin(6.2831855f * (f32)(phase * 3) / 360.0f));
		segs = (BalloonSeg*)data->pad;
		for (i = 0; i < 20; i++) {
			angle = 6.2831855f * (17.0f + 360.0f * (f32)i / 21.0f) / 360.0f;
			segs[i].x = radius * (f32)sin(angle);
			segs[i].y = -radius * (f32)cos(angle);
			wave = 0.1f * (f32)sin(6.2831855f * (f32)(i * 53) / 360.0f) + 2.0f;
			segs[i].x += 1.5f * (f32)sin(6.2831855f * (30.0f * (5.0f + (f32)i) +
			                                     (f32)phase * wave) / 360.0f);
			wave = 0.1f * (f32)cos(6.2831855f * (f32)(i * 36) / 360.0f) + 2.0f;
			segs[i].y += 1.5f * (f32)cos(6.2831855f * (50.0f * (5.0f + (f32)i) +
			                                     (f32)phase * wave) / 360.0f);
		}
		*(f32*)&data->pad[0xA0] = 0.0f;
		if (life < 100) {
			*(f32*)&data->pad[0xA4] += 1.0f;
		} else {
			*(f32*)&data->pad[0xA4] = 1.2f * radius;
		}
		dispEntry(4, dispCalcZ(&position), 2, effBalloonDisp, effect);
	}
}
