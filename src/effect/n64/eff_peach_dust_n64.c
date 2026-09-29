#include "eff/n64/eff_peach_dust_n64.h"
#include "driver/dispdrv.h"
#include <dolphin/mtx.h>

// .rodata
const Vec vec3_802fb340 = { 0.0f, 0.0f, 0.0f };

// .sdata2
extern const f32 float_0p01_8041f968;
extern const f32 float_0p4_8041f964;
extern const f32 float_0p1_8041f96c;

// .sdata
static u8 scale_data[28] = {
	0x1E, 0x3C, 0x5A, 0x64, 0x68, 0x6A, 0x6C, 0x6E,
	0x70, 0x71, 0x71, 0x6E, 0x6B, 0x67, 0x64, 0x68,
	0x6A, 0x6C, 0x6E, 0x70, 0x71, 0x71, 0x6E, 0x6B,
	0x67, 0x00, 0x00, 0x00,
};

// NONMATCHING (97.06%): all opcodes/operands match; only mwcc register allocation differs:
// the first vec3 word load is scheduled before the userdata load, the particle counter reload
// goes into r6 and is moved to r8, and the u8->float block rotates f1/f2/f0.
void effPeachDustMain(EffectEntry* effect) {
	PeachDustParticle* data;
	Vec position2;
	Vec position;
	s32 colorIndex;
	s32 i;
	s32 active;

	data = effect->userdata;
	position = vec3_802fb340;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;
	data->life--;
	data->counter++;
	colorIndex = data->counter;
	active = 0;
	data++;
	for (i = 0; i < effect->count - 1; i++, data++) {
		data->field_0x28 = (colorIndex + i + i) & 7;
		if (data->life < 0) {
			active++;
		}
		data->life--;
		if (data->life > 30 || data->life < 0) {
			data->field_0x28 = -1;
		}
		else {
			data->counter++;
			colorIndex = data->counter;
			if (data->counter - 1 < 25) {
				data->scale = (f32)scale_data[data->counter - 1] * float_0p01_8041f968 * float_0p4_8041f964;
			}
			else {
				f32 k = float_0p1_8041f96c;
				data->scale = k * (k - data->scale) + data->scale;
			}
			data->x += data->vx;
			data->y += data->vy;
			data->vy += data->gravity;
			data->z += data->vx;
		}
	}
	if (active >= effect->count - 1) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effPeachDustDisp, effect);
	}
}
