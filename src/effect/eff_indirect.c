#include "eff/eff_indirect.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct IndirectData {
	s32 type;              // 0x00
	f32 x;                 // 0x04
	f32 y;                 // 0x08
	f32 z;                 // 0x0C
	f32 rotX;              // 0x10
	f32 rotY;              // 0x14
	f32 rotZ;              // 0x18
	f32 scale;             // 0x1C
	u8 alpha;              // 0x20
	u8 pad_21[7];          // 0x21
	s32 timer;             // 0x28
	s32 field_2C;          // 0x2C
	f32 field_30;          // 0x30
	smartEntry* texBuffer; // 0x34
} IndirectData;

extern const char str_8030134C[];
extern const f32 float_0_804222ec;
extern const f32 float_neg0p05_804222f4;
extern const f32 float_neg5_804222f0;
extern const f32 float_1_804222dc;

EffectEntry* effIndirectEntry(s32 type, s32 timer, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	IndirectData* data;

	entry = effEntry();
	entry->type = str_8030134C;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(IndirectData));
	entry->userdata = data;
	entry->callback = effIndirectMain;
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = float_0_804222ec;
	data->alpha = 0xFF;
	data->texBuffer = smartAlloc(GXGetTexBufferSize(0x40, 0x40, 3, 0, 0), 0);
	data->rotX = float_0_804222ec;
	data->rotY = float_0_804222ec;
	data->rotZ = float_0_804222ec;
	data->timer = timer;
	data->field_2C = 0;
	data->field_30 = float_neg0p05_804222f4;

	return entry;
}

void effIndirectMain(EffectEntry* effect) {
	IndirectData* data;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	switch (data->type) {
	case 0:
	case 1:
		if (effect->flags & 4) {
			effect->flags &= ~4;
			data->timer = 0x10;
		}
		if (data->timer < 1000) {
			data->timer--;
		}
		if (data->timer < 0) {
			smartFree(data->texBuffer);
			effDelete(effect);
			return;
		}
		if (data->timer < 0x20) {
			data->alpha = data->timer << 3;
		}
		data->rotZ += float_neg5_804222f0;
		data->scale += float_1_804222dc;
		break;
	}

	dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&pos2), 2, effIndirectDisp, effect);
}
