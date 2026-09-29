#include "eff/eff_teresa.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>

const char str_802FE944[] = "Teresa";
const Vec vec3_802fe920 = {0.0f, 0.0f, 0.0f};
extern const f32 float_30_8042157c;
extern const f32 float_0_8042155c;

EffectEntry* effTeresaEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffTeresaData* data;
	Vec pos;

	entry = effEntry();
	entry->type = str_802FE944;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffTeresaData));
	entry->userdata = data;
	entry->callback = effTeresaMain;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = (type == 0) ? float_30_8042157c : float_0_8042155c;
	data->field_0x30 = 0;
	data->field_0x2C = 0;
	data->alpha = (f32)((type == 0) ? 0 : 255);
	data->field_0x3C = 0;

	if (type == 0) {
		pos = vec3_802fe920;
		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		psndSFXOn_3D((const char*)0x5D5, &pos);
	}
	return entry;
}
