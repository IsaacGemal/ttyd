#include "eff/n64/eff_breath_fire_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "eff/eff_n64.h"

typedef struct effBreathFireData {
	s32 field_0x00; //0x00
	s32 field_0x04; //0x04
	s32 field_0x08; //0x08
	f32 field_0x0C; //0x0C
	f32 field_0x10; //0x10
	f32 field_0x14; //0x14
	f32 field_0x18; //0x18
	f32 field_0x1C; //0x1C
	f32 field_0x20; //0x20
	f32 field_0x24; //0x24
	f32 field_0x28; //0x28
	f32 field_0x2C; //0x2C
	f32 field_0x30; //0x30
	f32 field_0x34; //0x34
	f32 field_0x38; //0x38
	f32 field_0x3C; //0x3C
	u32 field_0x40; //0x40
	s32 field_0x44; //0x44
	s32 field_0x48; //0x48
	s32 field_0x4C; //0x4C
	f32 field_0x50; //0x50
	f32 field_0x54; //0x54
	f32 field_0x58; //0x58
	f32 field_0x5C; //0x5C
	f32 field_0x60; //0x60
	u32 field_0x64; //0x64
	u32 field_0x68; //0x68
	u32 field_0x6C; //0x6C
	u32 field_0x70; //0x70
	u32 field_0x74; //0x74
	u32 field_0x78; //0x78
} effBreathFireData;

extern const f32 float_0_8041ea8c;
extern const f32 float_4_8041ea94;
extern const f32 float_10_8041ea98;
extern const f32 float_0p05_8041ea9c;
extern const f32 float_2p5_8041eaa0;
extern const f32 float_0p01_8041eaa4;
extern const f32 float_0p04_8041eaa8;
extern const f32 float_0p1_8041eaac;
extern const f32 float_0p2_8041eab0;

const f64 double_to_int_802fa300 = 4503601774854144.0;
const char str_802FA318[] = "BreathFireN64";

#pragma opt_common_subs off // without it MWCC CSEs the two 0.2f pool loads across the rand call into a callee-saved FPR
EffectEntry* effBreathFireN64Entry(s32 a, f32 b, f32 c, s32 d, f32 e, s32 f, f32 g, s32 h, f32 i, f32 j) {
	EffectEntry* entry;
	effBreathFireData* data;

	entry = effEntry();
	entry->type = str_802FA318;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBreathFireData));
	entry->userdata = data;
	entry->callback = effBreathFireMain;
	data->field_0x00 = a;
	data->field_0x04 = d;
	data->field_0x08 = f;
	data->field_0x0C = b;
	data->field_0x10 = c;
	data->field_0x14 = e;
	data->field_0x18 = b;
	data->field_0x1C = c;
	data->field_0x20 = e;
	data->field_0x24 = g;
	data->field_0x28 = i;
	data->field_0x2C = j;
	if (a == 0) {
		data->field_0x38 = float_0p05_8041ea9c;
	}
	else {
		data->field_0x38 = float_0p04_8041eaa8;
	}
	data->field_0x30 = data->field_0x38;
	data->field_0x44 = h;
	data->field_0x48 = h;
	data->field_0x4C = 0;
	data->field_0x40 = 0xFF;
	data->field_0x5C = float_0_8041ea8c;
	data->field_0x3C = float_0p1_8041eaac;
	data->field_0x34 = data->field_0x30;
	if (a == 0) {
		data->field_0x50 = float_0p2_8041eab0 * (g - b) * (f32)(s32)(effTblRandN64(10, f) - 5) * float_0p2_8041eab0;
		data->field_0x54 = float_0p2_8041eab0 * (i - c) * (f32)(s32)(effTblRandN64(10, f + 1) - 5) * float_0p2_8041eab0;
		data->field_0x58 = float_0p2_8041eab0 * (j - e) * (f32)(s32)(effTblRandN64(10, f + 2) - 5) * float_0p2_8041eab0;
	}
	else {
		data->field_0x50 = float_0p2_8041eab0 * (g - b) * (f32)(s32)(effTblRandN64(10, f + 3) - 5);
		data->field_0x54 = float_0p2_8041eab0 * (i - c) * (f32)(s32)(effTblRandN64(10, f + 4) - 5);
		data->field_0x58 = float_0p2_8041eab0 * (j - e) * (f32)(s32)(effTblRandN64(10, f + 5) - 5);
	}
	data->field_0x60 = float_0_8041ea8c;
	data->field_0x64 = 0xFF;
	data->field_0x68 = 0xAA;
	data->field_0x6C = 0x2A;
	data->field_0x70 = 0xF3;
	data->field_0x74 = 0x30;
	data->field_0x78 = 0;

	return entry;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (97.11%): the int->float magic constant is a local anonymous pool literal at .rodata+0x38,
// not the named double_to_int_802fa300 at +0x10, so Main's base+offset loads are shifted (zero vec +0x18,
// 0.008/0.3 +0x28/+0x30); the inline initializeVec store schedule and the hoisted float_4 load also differ.
void effBreathFireMain(EffectEntry* effect) {
	s32 life;
	effBreathFireData* data = effect->userdata;
	s32 total;
	s32 timer;
	Vec position = initializeVec((Vec*)&data->field_0x0C);
	EffectEntry* newEffect;

	data->field_0x5C = (f32)data->field_0x4C * float_4_8041ea94 / float_10_8041ea98;
	data->field_0x48--;
	data->field_0x4C++;
	life = data->field_0x48;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	timer = data->field_0x4C;
	total = data->field_0x44;
	if (life > 5 && data->field_0x00 == 0) {
		data->field_0x38 = data->field_0x38 + float_0p05_8041ea9c * (float_2p5_8041eaa0 - data->field_0x38);
	}
	data->field_0x0C = data->field_0x18 + (f32)timer * (data->field_0x50 + (data->field_0x24 - data->field_0x18)) / (f32)total;
	data->field_0x10 = data->field_0x1C + (f32)timer * (data->field_0x54 + (data->field_0x28 - data->field_0x1C)) / (f32)total;
	data->field_0x14 = data->field_0x20 + (f32)timer * (data->field_0x58 + (data->field_0x2C - data->field_0x20)) / (f32)total;
	if (data->field_0x00 == 1) {
		data->field_0x60 = data->field_0x60 + float_0p01_8041eaa4 * (f32)timer;
		data->field_0x10 = data->field_0x10 + data->field_0x60;
	}
	if (timer == data->field_0x08 + 1 && data->field_0x04 > 0) {
		newEffect = effBreathFireN64Entry(data->field_0x00, data->field_0x18, data->field_0x1C, data->field_0x04 - 1, data->field_0x20, data->field_0x08, data->field_0x24, total, data->field_0x28, data->field_0x2C);
		((effBreathFireData*)newEffect->userdata)->field_0x64 = data->field_0x64;
		((effBreathFireData*)newEffect->userdata)->field_0x68 = data->field_0x68;
		((effBreathFireData*)newEffect->userdata)->field_0x6C = data->field_0x6C;
		((effBreathFireData*)newEffect->userdata)->field_0x70 = data->field_0x70;
		((effBreathFireData*)newEffect->userdata)->field_0x74 = data->field_0x74;
		((effBreathFireData*)newEffect->userdata)->field_0x78 = data->field_0x78;
		((effBreathFireData*)newEffect->userdata)->field_0x30 = ((effBreathFireData*)newEffect->userdata)->field_0x38 = data->field_0x30;
		((effBreathFireData*)newEffect->userdata)->field_0x34 = data->field_0x34;
		((effBreathFireData*)newEffect->userdata)->field_0x3C = data->field_0x3C;
	}
	if (life < 10 && data->field_0x00 == 0) {
		data->field_0x40 = life * 25;
	}
	if (data->field_0x00 == 1) {
		data->field_0x40 = life * 224 / total;
		data->field_0x38 = 0.008f * (0.3 - data->field_0x38) + data->field_0x38;
	}
	if (data->field_0x00 == 2) {
		data->field_0x40 = life * 224 / total;
		data->field_0x38 = data->field_0x3C * (data->field_0x34 - data->field_0x38) + data->field_0x38;
	}
	dispEntry(4, dispCalcZ(&position), 2, effBreathFireDisp, effect);
}
