#include "eff/eff_syuryou.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

/* Full work data for the syuryou effect. Its first fields (type/position/scale)
   line up with EffSyuryouData in the header, which las_28.c reads. */
typedef struct SyuryouData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	f32 scale;      // 0x10
	s32 poseId;     // 0x14
	s32 color;      // 0x18
	s32 timer;      // 0x1C
	s32 field_0x20; // 0x20
} SyuryouData;

EffectEntry* effSyuryouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	SyuryouData* data;

	effect = effEntry();
	effect->type = "Syuryou";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, effect->count * sizeof(SyuryouData));
	effect->userdata = data;
	effect->callback = effSyuryouMain;
	effect->flags |= 2;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = 1.0f;
	data->color = 0xFF;
	if (duration <= 0) {
		data->timer = 1000;
	} else {
		data->timer = duration;
	}
	/* Retail stores 0xff here a second time; it is required to match. */
	data->color = 0xFF;
	data->poseId = -1;
	return effect;
}
