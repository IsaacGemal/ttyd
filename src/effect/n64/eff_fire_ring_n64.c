#include "eff/n64/eff_fire_ring_n64.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_fire_dust_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include "math_ext.h"

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041ee6c;
extern const f32 float_neg1p1_8041ee84;
extern const f32 float_0p4_8041ee88;
extern const f32 float_1_8041ee8c;
extern const f32 float_0p1_8041ee90;

const Vec vec3_802fa588 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa598 = 4503601774854144.0;

const char str_802FA5A0[12] = "FireRingN64";

typedef struct effFireRingData {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	u8 pad_10[0xC];  // 0x10
	f32 angle;       // 0x1C
	f32 angleSpeed;  // 0x20
	s32 alpha;       // 0x24
	s32 life;        // 0x28
	s32 counter;     // 0x2C
	f32 rotX;        // 0x30
	f32 rotY;        // 0x34
	f32 rotZ;        // 0x38
	f32 rotW;        // 0x3C
	f32 rotSpeedX;   // 0x40
	f32 rotSpeedY;   // 0x44
	f32 rotSpeedZ;   // 0x48
	f32 rotSpeedW;   // 0x4C
	u8 colorR;       // 0x50
	u8 colorG;       // 0x51
	u8 colorB;       // 0x52
	u8 pad_53;       // 0x53
} effFireRingData;   // 0x54

EffectEntry* effFireRingN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFireRingData* data;

	entry = effEntry();
	entry->type = str_802FA5A0;
	entry->count = 32;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireRingData) * 32);
	entry->userdata = data;
	entry->callback = effFireRingMain;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->alpha = 0xff;
	data->life = life;
	data->counter = 0;
	data->rotX = float_0_8041ee6c;
	data->rotY = float_0_8041ee6c;
	data->rotZ = float_0_8041ee6c;
	data->rotW = float_0_8041ee6c;
	data->rotSpeedX = float_neg1p1_8041ee84;
	data->rotSpeedY = float_0_8041ee6c;
	data->rotSpeedZ = float_0p4_8041ee88;
	data->rotSpeedW = float_1_8041ee8c;
	data->angle = float_0_8041ee6c;
	data->angleSpeed = float_0p1_8041ee90 * scale;
	data->colorR = 0xff;
	data->colorG = 0xff;
	data->colorB = 0x40;

	return entry;
}

void effFireRingMain(EffectEntry* effect) {
	effFireRingData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain / effGlassMain). */
	Vec position;
#endif
	Vec disp = vec3_802fa588;
	s32 life;
	s32 counter;
	f32 max;
	s32 r;
	f32 angle;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->life--;
	data->counter++;

	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->counter;
	if (counter & 1) {
		r = effTblRandN64(0x168, counter);
		angle = float_6p2832_8041ee74 * (f32)r / float_360_8041ee78;
		sin(angle);
		cos(angle);
		effFireDustN64Entry(0, 10, 20, data->x, data->y, data->z, (f32)(counter * 8), float_10_8041ee7c);
	}

	data->angle += data->angleSpeed;
	if (life < 10) {
		data->alpha = life * 0x19;
	}

	max = 128.0f;
	data->rotX += data->rotSpeedX;
	data->rotY += data->rotSpeedY;
	data->rotZ += data->rotSpeedZ;
	data->rotW += data->rotSpeedW;

	if (data->rotX > max) {
		data->rotX -= max;
	}
	if (data->rotY > max) {
		data->rotY -= max;
	}
	if (data->rotZ > max) {
		data->rotZ -= max;
	}
	if (data->rotW > max) {
		data->rotW -= max;
	}
	if (data->rotX < float_0_8041ee6c) {
		data->rotX += float_128_8041ee80;
	}
	if (data->rotY < float_0_8041ee6c) {
		data->rotY += float_128_8041ee80;
	}
	if (data->rotZ < float_0_8041ee6c) {
		data->rotZ += float_128_8041ee80;
	}
	if (data->rotW < float_0_8041ee6c) {
		data->rotW += float_128_8041ee80;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFireRingDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effFireRingDisp, effect);
#endif
}
