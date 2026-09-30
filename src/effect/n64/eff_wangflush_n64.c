#include "eff/n64/eff_wangflush_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <math.h>

extern const f32 float_0p5_80420120;
extern const f32 float_0p78_80420124;
extern const f32 float_0p29_80420128;
extern const f32 float_0p93_8042012c;
extern const f32 float_6p2832_80420130;
extern const f32 float_360_80420134;
extern const f32 float_0p9_80420138;
extern const f32 float_144_8042013c;
extern const f32 float_64_80420140;

typedef struct effWangFlushData {
	s32 type;         // 0x0
	Vec position;     // 0x4
	f32 rot;          // 0x10
	f32 rotSpeed;     // 0x14
	f32 scale;        // 0x18
	f32 scaleSpeed;   // 0x1C
	f32 alphaScale;   // 0x20
	s32 alpha;        // 0x24
	s32 field_0x28;   // 0x28
	s32 duration;     // 0x2C
	s32 frame;        // 0x30
} effWangFlushData;

EffectEntry* effWangFlushN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effWangFlushData* data;

	effect = effEntry();
	effect->type = "WangflushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effWangFlushData));
	effect->userdata = data;
	effect->callback = effWangFlushMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->duration = duration;
	data->frame = 0;
	data->alpha = 0;
	data->rot = 0.0f;
	if (type == 0) {
		data->rotSpeed = 30.0f;
	} else {
		data->rotSpeed = 0.0f;
	}
	data->scale = 0.0f;
	data->scaleSpeed = 0.8f;
	data->field_0x28 = 0;
	data->alphaScale = 0.0f;
	return effect;
}

void effWangFlushMain(EffectEntry* effect) {
	s32 duration;
	s32 frame;
	effWangFlushData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->duration--;
	data->frame++;
	duration = data->duration;
	if (duration < 0) {
		effDelete(effect);
		return;
	}

	frame = data->frame;
	if (frame <= 6) {
		data->alpha = float_0p5_80420120 * (218 - data->alpha) + data->alpha;
	}
	if (frame > 6) {
		data->alpha = data->alpha * float_0p78_80420124;
	}

	data->scale += data->scaleSpeed;
	data->scaleSpeed += float_0p29_80420128;
	data->rot += data->rotSpeed;
	data->rotSpeed *= float_0p93_8042012c;
	data->alphaScale = float_0p5_80420120 * (f32)sin(float_6p2832_80420130 * (f32)(duration * 50) / float_360_80420134) + float_0p9_80420138;
	data->field_0x28 = float_64_80420140 * (f32)sin(float_6p2832_80420130 * (f32)(duration * 40) / float_360_80420134) + float_144_8042013c;

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effWangFlushDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effWangFlushDisp, effect);
#endif
}
