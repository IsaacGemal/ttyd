#include "eff/n64/eff_sweat_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"
#include <math.h>
#include <dolphin/mtx.h>

// .rodata / .sdata2 data used by effSweatN64Entry (originally local to this unit).
#ifdef MUST_MATCH
// volatile stops MWCC from hoisting vec3_802fb810's components out of the loop;
// retail reloads them through a base register every iteration.
const volatile Vec vec3_802fb810 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802fb810 = { 0.0f, 0.0f, 0.0f };
#endif
const f64 double_to_int_802fb838 = 4503601774854144.0;
extern const char str_802FB828[];
extern const f32 float_deg2rad_8041ffac;
extern const f32 float_0_8041ffbc;
extern const f32 float_neg1_8041ffb8;
extern const f32 float_16_8041ffd8;

EffectEntry* effSweatN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale, f32 angle) {
	EffectEntry* effect;
	effSweatData* data;
	effSweatData* p;
	Vec axis;
	Mtx mtx;
	s32 count;
	s32 i;
	f32 scaleBase;
	f32 angleOffset;
	f32 newAngle;

	effect = effEntry();
	count = 1;
	switch (type) {
	case 0:
		count = 7;
		break;
	case 1:
		count = 5;
		break;
	}
	effect->type = str_802FB828;
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * 0x34);
	effect->userdata = data;
	effect->callback = effSweatMain;
	scaleBase = float_16_8041ffd8 + scale;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	p = data + 1;
	for (i = 1; i < effect->count; i++, p++) {
		Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
		temp.x = (f32)sin(float_0_8041ffbc);
		temp.z = -(f32)cos(float_0_8041ffbc);
		axis = temp;
		switch (type) {
		case 0:
			angleOffset = (f32)((i - 1) % 3 * 14);
			break;
		case 1:
			angleOffset = (f32)((i - 1) % 4 * 16);
			break;
		}
		if (angle > float_0_8041ffbc) {
			angleOffset = angleOffset * float_neg1_8041ffb8;
		}
		newAngle = angle + angleOffset;
		PSMTXRotAxisRad(mtx, &axis, float_deg2rad_8041ffac * -newAngle);
		p->position.x = mtx[0][1] * scaleBase;
		p->position.y = mtx[1][1] * scaleBase;
		p->position.z = mtx[2][2] * scaleBase;
		p->velocity.x = mtx[0][1];
		p->velocity.y = mtx[1][1];
		p->velocity.z = mtx[2][1];
		p->angle = newAngle;
		p->life = life;
		p->age = 0;
		p->timer = irand(10);
		p->alpha = 0;
		if (type == 0 && i >= 4) {
			p->timer += 0x10;
		}
	}
	return effect;
}

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effSweatMain(EffectEntry* effect) {
	effSweatData* data = effect->userdata;
	Vec disp;
	s32 i;
	s32 dead = 0;

	disp = initializeVec(&data->position);

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			data->timer--;
		} else {
			data->position.x += data->velocity.x;
			data->position.y += data->velocity.y;
			data->position.z += data->velocity.z;
			data->velocity.x *= 0.94f;
			data->velocity.y -= 0.05f;
			if (data->velocity.y < -0.1f) {
				data->velocity.y = -0.1f;
			}
			data->velocity.z *= 0.94f;
			if (data->age < 8) {
				data->alpha = (data->age << 5) + 0x1f;
			}
			if (data->life < 0x10) {
				data->alpha = data->life << 4;
			}
			if (data->alpha > 0xff) {
				data->alpha = 0xff;
			}
			if (data->alpha < 0) {
				data->alpha = 0;
			}
			data->age++;
			data->life--;
			if (data->life < 0) {
				dead++;
			}
		}
	}

	if (dead >= effect->count - 1) {
		effDelete(effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effSweatDisp, effect);
	}
}
