#include "eff/n64/eff_akari_charge_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

typedef struct EffAkariChargeParticle {
	s32 field_0x0;  //0x0
	f32 field_0x4;  //0x4
	f32 field_0x8;  //0x8
	f32 field_0xC;  //0xC
	s32 life;       //0x10
	s32 counter;    //0x14
	u8 pad_0x18[0x1C];
	f32 scale;      //0x34
	f32 field_0x38; //0x38
	f32 angle;      //0x3C
	s32 index;      //0x40
} EffAkariChargeParticle; //0x44

typedef struct EffAkariChargeWork {
	s32 type;       //0x0
	f32 x;          //0x4
	f32 y;          //0x8
	f32 z;          //0xC
	s32 field_0x10; //0x10
	s32 field_0x14; //0x14
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	s32 field_0x28; //0x28
	s32 field_0x2C; //0x2C
	s32 field_0x30; //0x30
	f32 field_0x34; //0x34
	f32 field_0x38; //0x38
	f32 field_0x3C; //0x3C
	s32 field_0x40; //0x40
	EffAkariChargeParticle particles[21]; //0x44
} EffAkariChargeWork;

EffectEntry* effAkariChargeN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6) {
	EffectEntry* entry;
	EffAkariChargeWork* work;
	EffAkariChargeParticle* p;
	s32 n;

	entry = effEntry();
	entry->type = "AkariChargeN64";
	entry->count = 0x16;
	work = __memAlloc(HEAP_EFFECT, 0x5D8);
	entry->userdata = work;
	entry->callback = effAkariChargeMain;
	entry->flags |= 2;

	work->type = type;
	work->field_0x14 = 0;
	if (a2 <= 0) {
		work->field_0x10 = 1000;
	} else {
		work->field_0x10 = a2;
	}
	work->field_0x24 = 0;
	work->x = a3;
	work->y = a4;
	work->z = a5;
	work->field_0x38 = a6;
	work->field_0x18 = 0xFF;
	work->field_0x1C = 0xE6;
	work->field_0x20 = 0;
	work->field_0x28 = 0xFF;
	work->field_0x2C = 0xFF;
	work->field_0x30 = 0xFF;

	n = 1;
	p = work->particles;
	while (n < 22) {
		p->life = 20;
		p->counter = 0;
		p->angle = (f32)((n - 1) * 360 / 21);
		p->field_0x4 = 0.0f;
		p->field_0x8 = 0.0f;
		p->scale = 1.0f;
		p->index = n & 0xF;
		n++;
		p++;
	}

	return entry;
}
