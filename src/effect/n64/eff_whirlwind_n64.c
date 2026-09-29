#include "eff/n64/eff_whirlwind_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include "mario/mario.h"

typedef struct EffWhirlwindData {
	s32 type;         //0x0
	MarioWork* field_0x4;  //0x4
	f32 x;            //0x8
	f32 y;            //0xC
	f32 z;            //0x10
	f32 field_0x14;   //0x14
	f32 field_0x18;   //0x18
	s32 field_0x1C;   //0x1C
	s32 field_0x20;   //0x20
	s32 field_0x24;   //0x24
	s32 field_0x28;   //0x28
	f32 field_0x2C;   //0x2C
	f32 field_0x30;   //0x30
	f32 field_0x34;   //0x34
	f32 field_0x38;   //0x38
	f32 field_0x3C;   //0x3C
	f32 field_0x40;   //0x40
	u8 field_0x44;    //0x44
	u8 field_0x45;    //0x45
	u8 field_0x46;    //0x46
	u8 field_0x47;    //0x47
} EffWhirlwindData; //0x48

void effWhirlwindMain(EffectEntry* effect) {
	EffWhirlwindData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 life;
	s32 type;
	s32 i;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	data->field_0x24--;
	data->field_0x28++;
	life = data->field_0x24;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	type = data->type;
	switch (type) {
	case 8:
		if (life > 5) {
			data->field_0x1C = (s32)(0.05f * (255 - data->field_0x1C) + data->field_0x1C);
		}
		data->field_0x14 = data->field_0x18 * (f32)life / (f32)data->field_0x20;
		if (life < 10) {
			data->field_0x1C = life * 25;
		}
		break;
	default:
		if (life > 5) {
			data->field_0x1C = (s32)(0.3f * (100 - data->field_0x1C) + data->field_0x1C);
		}
		if (life < 10) {
			data->field_0x1C *= 0.8f;
			data->field_0x14 += 0.1f * (2.0f * data->field_0x18 - data->field_0x14);
		}
		break;
	}

	if (data->field_0x4 != NULL) {
		MarioWork* ptr = data->field_0x4;
		data->x = ptr->position.x;
		data->y = ptr->position.y;
		data->z = ptr->position.z;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		switch (type) {
		case 8:
			if (i & 1) {
				data->field_0x3C += 0.2f * (40.0f - data->field_0x3C);
			} else {
				data->field_0x3C += 0.2f * (-40.0f - data->field_0x3C);
			}
			data->field_0x2C += data->field_0x38;
			data->field_0x30 += data->field_0x3C;
			data->field_0x34 += data->field_0x40;
			break;
		default:
			if (life < 10) {
				data->field_0x30 += data->field_0x3C * (f32)life / 10.0f;
			} else {
				data->field_0x30 += data->field_0x3C;
			}
			break;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effWhirlwindDisp, effect);
}
