#include "eff/eff_nokotarou.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/types.h>

// .rodata string and .sdata2 constant, declared in the original unit's data order.
const char str_803014B8[] = "Nokotarou";
const f32 float_0_8042248c = 0.0f;

EffectEntry* effNokotarouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry = effEntry();
	NokotarouWork* work;

	entry->type = str_803014B8;
	entry->count = 1;
	work = __memAlloc(HEAP_EFFECT, sizeof(NokotarouWork));
	entry->userdata = work;
	entry->callback = effNokotarouMain;
	entry->flags |= 2;

	work->type = type;
	work->x = x;
	work->y = y;
	work->z = z;
	work->x2 = work->x;
	work->y2 = work->y;
	work->z2 = work->z;
	work->scale = scale;
	work->scaleAnim = float_0_8042248c;
	work->alpha = 0xFF;
	if (duration <= 0) {
		work->lifetime = 1000;
	} else {
		work->lifetime = duration;
	}
	work->animPose = -1;
	work->animPose2 = -1;
	return entry;
}
