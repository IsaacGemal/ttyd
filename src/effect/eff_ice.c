#include "eff/eff_ice.h"
#include "driver/effdrv.h"
#include "memory.h"

typedef struct IceData {
	s32 type;    // 0x00
	f32 x;       // 0x04
	f32 y;       // 0x08
	f32 z;       // 0x0C
	f32 scale;   // 0x10
	f32 scaleX;  // 0x14
	f32 scaleY;  // 0x18
	f32 scaleZ;  // 0x1C
	s32 timer;   // 0x20
	s32 frame;   // 0x24
	s32 state;   // 0x28
	void* anim;  // 0x2C
} IceData;


extern const f32 float_1_8041e3f4;

__declspec(section ".sdata2") const char str_8041E418[] = "Ice";
extern u32 ice_effAnimRSTDt[];
__declspec(section ".sdata") u32 ice_effAnimDt[] = { 0x00000000, 0x42EE0000, (u32)ice_effAnimRSTDt };


// effIceMain is local to this file in the original object (.fn effIceMain, local).
static void effIceMain(EffectEntry* effect);

EffectEntry* effIceEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	IceData* data;

	effect = effEntry();
	effect->type = str_8041E418;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x30);
	effect->userdata = data;
	effect->callback = effIceMain;
	effect->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->scaleX = float_1_8041e3f4;
	data->scaleY = float_1_8041e3f4;
	data->scaleZ = float_1_8041e3f4;
	if (count > 0) {
		data->timer = count;
	} else {
		data->timer = 0x3E8;
	}
	data->frame = 0;
	data->state = 0;
	data->anim = effMayaAnimAlloc(ice_effAnimDt);
	effPlayMayaAnim(data->anim);
	return effect;
}
