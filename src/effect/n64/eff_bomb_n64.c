#include "eff/n64/eff_bomb_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_confetti_n64.h"
#include "eff/n64/eff_kemuri8_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>

// Data owned by the bomb effect. The comments give the offsets the original uses.
typedef struct effBombN64Data {
	s32 type;          // 0x00
	f32 x;             // 0x04
	f32 y;             // 0x08
	f32 z;             // 0x0C
	u8 pad_0x10[0x20]; // 0x10
	s32 field_0x30;    // 0x30
	s32 field_0x34;    // 0x34
	u8 pad_0x38[0x4];  // 0x38
	s32 field_0x3C;    // 0x3C
	s32 camId;         // 0x40
	s32 field_0x44;    // 0x44
	u8 pad_0x48[0x1C]; // 0x48
	f32 field_0x64;    // 0x64
	f32 field_0x68;    // 0x68
	u8 pad_0x6C[0x10]; // 0x6C
	s32 field_0x7C;    // 0x7C
	u8 pad_0x80[0x8];  // 0x80
	s32 field_0x88;    // 0x88
	u8 pad_0x8C[0x1C]; // 0x8C
	f32 field_0xA8;    // 0xA8
	f32 field_0xAC;    // 0xAC
	f32 field_0xB0;    // 0xB0
	f32 field_0xB4;    // 0xB4
	u8 pad_0xB8[0x8];  // 0xB8
	s32 field_0xC0;    // 0xC0
	u8 pad_0xC4[0x8];  // 0xC4
} effBombN64Data;

// .sdata2 float constants, named after their original addresses.
extern const f32 float_0_8041e9e0;
extern const f32 float_1_8041e9e4;
extern const f32 float_0p4_8041e9e8;
extern const f32 float_0p6_8041e9ec;
extern const f32 float_255_8041e9f0;
extern const f32 float_0p9_8041e9f4;
extern const f32 float_0p1_8041e9f8;
extern const f32 float_0p5_8041e9fc;
extern const f32 float_3_8041ea00;
extern const f32 float_9p9_8041ea04;
extern const f32 float_1p6_8041ea08;
extern const f32 float_13p9_8041ea0c;
extern const f32 float_2p4_8041ea10;
extern const f32 float_17p8_8041ea14;
extern const f32 float_3p2_8041ea18;
extern const f32 float_30_8041ea1c;

// Local to this file in the original (`.fn effBombMain, local`).
static void effBombMain(EffectEntry* effect);
// Local to this file in the original (`.fn effBombDisp, local`).
static void effBombDisp(CameraId camId, void* param);

void effBombN64SetCamId(EffectEntry* entry, s32 camId) {
	effBombN64Data* data = entry->userdata;
	data->camId = camId;
}

EffectEntry* effBombN64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effBombN64Data* data;
	s32 flag;

	effect = effEntry();
	flag = 0;
	if (type >= 3) {
		flag = 1;
		type -= 3;
	}
	effect->type = "BombN64";
	effect->count = 3;
	data = __memAlloc(HEAP_EFFECT, 0xCC);
	effect->userdata = data;
	effect->callback = effBombMain;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x34 = 0;
	data->field_0x30 = 60;
	data->type = type;
	data->field_0x3C = flag;
	data->camId = 4;
	data->field_0x44 = (type == 0) ? -1 : type;
	data->field_0x7C = 0;
	data->field_0x64 = (type == 1) ? float_1_8041e9e4 : float_3_8041ea00;
	data->field_0x68 = (type == 1) ? float_1_8041e9e4 : float_3_8041ea00;
	data->field_0x88 = type;
	data->field_0xC0 = 0xFF;
	switch (type) {
	case 0:
		data->field_0xA8 = float_9p9_8041ea04;
		data->field_0xAC = float_1p6_8041ea08;
		break;
	case 1:
		data->field_0xA8 = float_13p9_8041ea0c;
		data->field_0xAC = float_2p4_8041ea10;
		break;
	case 2:
		data->field_0xA8 = float_17p8_8041ea14;
		data->field_0xAC = float_3p2_8041ea18;
		break;
	}
	data->field_0xB0 = float_0_8041e9e0;
	data->field_0xB4 = float_30_8041ea1c;
	return effect;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (99.49%): the two `timer >= 8` else-blocks compute the product in f0 instead of f1, rotating the
// int->float double and 0.9 temporaries (build f1/f2; target f2/f0). Tried `=`/reversed operands/temps and pragmas.
static void effBombMain(EffectEntry* effect) {
	effBombN64Data* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 type;
	s32 timer;
	s32 camId = data->camId;

	data->field_0x30--;
	if (data->field_0x30 < 0) {
		effDelete(effect);
		return;
	}

	data->field_0x34++;
	timer = data->field_0x34;
	type = data->type;

	if (timer == 7) {
		if (data->field_0x3C == 0) {
			effKemuri8N64SetCamId(effKemuri8N64Entry(type, data->x, data->y, data->z), camId);
		} else {
			effKemuri8N64SetCamId(effKemuri8N64Entry(type + 3, data->x, data->y, data->z), camId);
		}
	} else if (timer == 1) {
		effConfettiN64SetCamId(effConfettiN64Entry(type + 4, 50, data->x, data->y, data->z, 1.0f), camId);
	}

	data->field_0x64 += data->field_0x68;
	if (timer < 8) {
		data->field_0x68 += (type == 2) ? 1.0f : 0.4f;
		data->field_0x7C = 0.6f * (255.0f - (f32)data->field_0x7C);
	} else {
		data->field_0x68 *= 0.6f;
		data->field_0x7C = (f32)data->field_0x7C * 0.9f;
	}

	if (timer >= 3) {
		if (timer == 3) {
			data->field_0xA8 = 1.0f;
		}
		data->field_0xA8 += data->field_0xAC;
	}

	if (timer >= 3) {
		if (timer < 6) {
			data->field_0xAC += 0.1f;
		} else {
			data->field_0xAC *= 0.5f;
			data->field_0xC0 = (f32)data->field_0xC0 * 0.9f;
		}
	}

	data->field_0xB0 += data->field_0xB4;
	dispEntry(camId, dispCalcZ(&position), 2, effBombDisp, effect);
}
