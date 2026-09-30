#include "eff/eff_treasure_map.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const char str_802FE770[];
const char str_804213E4[] = "Z_1";
extern const f32 float_1_804213e8;
extern const f32 float_0_804213dc;

EffectEntry* effTreasureMapEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffTreasureMapData* data;

	entry = effEntry();
	entry->type = str_802FE770;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffTreasureMapData));
	entry->userdata = data;
	entry->callback = effTreasureMapMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->timer = 1000;
	data->alpha = 0xff;
	data->scale = float_1_804213e8;
	data->angle = float_0_804213dc;
	data->animId = -1;
	data->cameraId = 4;
	return entry;
}

void effTreasureMapMain(EffectEntry* effect) {
	EffTreasureMapData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 inBattle;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;
	inBattle = gp->inBattle != 0;

	if (data->animId == -1) {
		if (!animGroupBaseAsync("EFF_treasure_map", inBattle, NULL)) {
			return;
		}
		data->animId = animPoseEntry("EFF_treasure_map", inBattle);
		animPoseSetAnim(data->animId, str_804213E4, TRUE);
	}

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->timer = 16;
	}

	if (data->timer < 1000) {
		data->timer--;
	}
	if (data->timer < 16) {
		data->alpha = data->timer * 16;
	}
	if (data->timer < 0) {
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
		return;
	}

	data->counter++;
	dispEntry(data->cameraId, dispCalcZ(&pos2), 2, effTreasureMapDisp, effect);
}
