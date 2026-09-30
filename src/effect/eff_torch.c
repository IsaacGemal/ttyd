#include "eff/eff_torch.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

// .rodata: zero vector the original pools every zero Vec literal in this unit into.
const Vec vec3_802f91d8 = {0.0f, 0.0f, 0.0f};

// .sdata2
const char str_8041E444[] = "Torch";

// .sdata2 constants this unit references by name in retail
extern const f32 float_0p5_8041e44c;
extern const f32 float_1_8041e43c;

EffectEntry* effTorchEntry(s32 id, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry = effEntry();
	effTorchData* data;

	entry->type = str_8041E444;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x20);
	entry->userdata = data;
	entry->callback = effTorchMain;

	data->id = id;
	*(u8*)((u8*)data + 0x1C) = 0;
	// The nested block is not cosmetic: the original keeps this temporary at r1+8 and
	// interleaves its copy with the component writes. A function-scope declaration schedules
	// the three lwz together and does not match.
	{
		Vec position = {0.0f, 0.0f, 0.0f};
		position.x = x;
		position.y = y;
		position.z = z;
		data->position = position;
	}
	*(f32*)((u8*)data + 0x10) = float_0p5_8041e44c * scale;
	*(f32*)((u8*)data + 0x14) = float_1_8041e43c;
	*(f32*)((u8*)data + 0x18) = float_1_8041e43c;
	return entry;
}

void effTorchMain(EffectEntry* effect) {
	effTorchData* data = effect->userdata;

	dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effTorchDisp, effect);
}
