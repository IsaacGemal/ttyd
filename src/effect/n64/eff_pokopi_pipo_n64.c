#include "eff/n64/eff_pokopi_pipo_n64.h"
#include "driver/dispdrv.h"

#include <dolphin/mtx.h>

typedef struct effPokopiPipoData {
	s32 type;           // 0x0
	f32 x;              // 0x4
	f32 y;              // 0x8
	f32 z;              // 0xC
	f32 field_0x10[6];  // 0x10
	s32 lifetime;       // 0x28
	s32 timer;          // 0x2C
	s32 field_0x30;     // 0x30
	s32 field_0x34;     // 0x34
	s32 field_0x38;     // 0x38
	u32 field_0x3C;     // 0x3C
	f32 field_0x40;     // 0x40
	f32 field_0x44;     // 0x44
	f32 field_0x48;     // 0x48
	f32 field_0x4C;     // 0x4C
	f32 field_0x50;     // 0x50
	f32 field_0x54;     // 0x54
	f32 field_0x58;     // 0x58
	f32 field_0x5C;     // 0x5C
	s32 field_0x60;     // 0x60
	s32 field_0x64;     // 0x64
	s32 field_0x68;     // 0x68
} effPokopiPipoData;

// .sdata2 (pool constants referenced by name so the symbol matches the original)
extern const f32 float_0_8041f9f4;
extern const f32 float_0p01_8041f9f8;
extern const f32 float_40_8041f9fc;
extern const f32 float_neg0p2_8041fa00;
extern const f32 float_neg1_8041fa04;
extern const f32 float_neg4_8041fa08;
extern const f32 float_14p286_8041fa0c;
extern const f32 float_neg20_8041fa10;

// .sdata
u8 w_scale_data[8] = { 0x6E, 0x96, 0x82, 0x6E, 0x64, 0x5F, 0x64, 0x00 };
u8 h_scale_data[8] = { 0x50, 0x3C, 0x50, 0x64, 0x78, 0x6E, 0x64, 0x00 };

void effPokopiPipoMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	effPokopiPipoData* data = effect->userdata;
	s32 frame;
	s32 timer;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	frame = data->field_0x60;
	timer = data->timer;
	if (frame >= 0 && frame < 7) {
		data->field_0x58 = float_0p01_8041f9f8 * (f32)w_scale_data[frame];
		data->field_0x5C = float_0p01_8041f9f8 * (f32)h_scale_data[frame];
		data->field_0x50 -= float_40_8041f9fc;
		data->field_0x60++;
	} else {
		data->field_0x48 += float_neg0p2_8041fa00;
		data->x += data->field_0x44;
		data->y += data->field_0x48;
		data->z += data->field_0x4C;
		data->field_0x50 += data->field_0x54;
	}
	if (timer - 1 == data->field_0x68) {
		data->field_0x48 = data->field_0x48 * float_neg1_8041fa04;
		data->field_0x54 = float_neg4_8041fa08;
		data->field_0x60 = 0;
	} else if (data->field_0x48 < float_0_8041f9f4 && data->y < float_14p286_8041fa0c) {
#ifdef MUST_MATCH
		/* MUST_MATCH: the original computes f - f rather than loading the 0.0f pool entry. */
		data->field_0x48 = data->field_0x48 - data->field_0x48;
#else
		data->field_0x48 = 0.0f;
#endif
		data->y = float_14p286_8041fa0c;
		data->field_0x54 = float_neg20_8041fa10;
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPokopiPipoDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPokopiPipoDisp, effect);
#endif
}
