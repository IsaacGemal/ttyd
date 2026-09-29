#include "eff/n64/eff_wangflush_n64.h"
#include "memory.h"
#include <dolphin/mtx.h>

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
