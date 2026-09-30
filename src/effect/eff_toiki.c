#include "eff/eff_toiki.h"
#include "driver/dispdrv.h"
#include "eff/eff_mist.h"
#include "memory.h"
#include "math_ext.h"

const char str_80421C90[] = "Toiki";
const Vec vec3_80300230 = { 0.0f, 0.0f, 0.0f };

extern const f32 float_0_80421c68;
extern const f32 float_deg2rad_80421c6c;

EffectEntry* effToikiEntry(s32 type, s32 life, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	ToikiData* data;
	f32 angle;

	entry = effEntry();
	entry->type = str_80421C90;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x34);
	entry->userdata = data;
	entry->callback = effToikiMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = 0.5f;
	data->rotation = 0.0f;
	data->life = life;
	data->field_0x2C = 0;
	data->field_0x28 = 255.0f;
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velX = 2.5f * (f32)cos(angle);
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velY = 2.5f * (f32)sin(angle);
	data->velZ = 0.0f;
	data->field_0x24 = 0.0f;
	return entry;
}

void effToikiMain(EffectEntry* effect) {
	ToikiData* data = effect->userdata;
	Vec pos;
	Vec mistPos;
	Vec sp;
	Mtx mtx;
	s32 type;
	f32 angle;
	f32 size;

	sp = vec3_80300230;
	sp.x = data->x;
	sp.y = data->y;
	sp.z = data->z;
	pos = sp;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 1000) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->x += data->velX;
	data->y += data->velY;
	data->z += data->velZ;
	data->velX *= 0.95f;
	data->velY *= 0.95f;
	data->velZ *= 0.95f;
	if (data->life < 0x10) {
		data->field_0x28 = (f32)(data->life * 16);
	}
	if (data->field_0x24 > 1.0f && data->field_0x24 < 4.0f && data->life % 20 == 0) {
		size = 320.0f * data->scale;
		angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
		mistPos.x = data->x + size * (f32)cos(angle);
		mistPos.y = data->y + size * (f32)sin(angle);
		mistPos.z = data->z + 1.0f;
		PSMTXRotRad(mtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_80421c6c);
		PSMTXMultVec(mtx, &mistPos, &mistPos);
		effMistEntry(type + 1, 0x3c, mistPos.x, mistPos.y, mistPos.z, float_0_80421c68);
	}
	data->field_0x2C++;
	data->field_0x24 += 0.015f;
	if (data->field_0x24 > 4.0f) {
		data->field_0x24 = 4.0f;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effToikiDisp, effect);
}
