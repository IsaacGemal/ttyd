#include "eff/n64/eff_whirlwind_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "mario/mario.h"
#include <stdlib.h>

// .sdata color tables (original symbols col_r/col_g/col_b at 0x80412D28-0x80412D33).
__declspec(section ".sdata") u8 col_r[4] = { 0xFF, 0xFF, 0x40, 0x00 };
__declspec(section ".sdata") u8 col_g[4] = { 0xFF, 0x40, 0x40, 0x00 };
__declspec(section ".sdata") u8 col_b[4] = { 0xFF, 0x40, 0xFF, 0x00 };

// .data vertex array (original symbol eff_whirlwind_v, 0x16C bytes).
u32 eff_whirlwind_v[0x16C / 4] = {
	0xFFFBFFFE, 0x000903AB, 0x0000FFFF, 0xFFFFFFFB, 0x00020009, 0x03AB0200, 0xFFFFFFFF,
	0xFFF70002, 0x00050355, 0x0200FFFF, 0xFFFFFFF7, 0xFFFE0005, 0x03550000, 0xFFFFFFFF,
	0x0000FFFE, 0x000A0400, 0x0000FFFF, 0xFFFF0000, 0x0002000A, 0x04000200, 0xFFFFFFFF,
	0xFFF60002, 0x00000300, 0x0200FFFF, 0xFFFFFFF6, 0xFFFE0000, 0x03000000, 0xFFFFFFFF,
	0x0005FFFE, 0x00090055, 0x0000FFFF, 0xFFFF0005, 0x00020009, 0x00550200, 0xFFFFFFFF,
	0x00000002, 0x000A0000, 0x0200FFFF, 0xFFFF0000, 0xFFFE000A, 0x00000000, 0xFFFFFFFF,
	0x0009FFFE, 0x000500AB, 0x0000FFFF, 0xFFFF0009, 0x00020005, 0x00AB0200, 0xFFFFFFFF,
	0x000AFFFE, 0x00000100, 0x0000FFFF, 0xFFFF000A, 0x00020000, 0x01000200, 0xFFFFFFFF,
	0x0009FFFE, 0xFFFB0155, 0x0000FFFF, 0xFFFF0009, 0x0002FFFB, 0x01550200, 0xFFFFFFFF,
	0xFFF70002, 0xFFFB02AB, 0x0200FFFF, 0xFFFFFFF7, 0xFFFEFFFB, 0x02AB0000, 0xFFFFFFFF,
	0x0005FFFE, 0xFFF701AB, 0x0000FFFF, 0xFFFF0005, 0x0002FFF7, 0x01AB0200, 0xFFFFFFFF,
	0x0000FFFE, 0xFFF60200, 0x0000FFFF, 0xFFFF0000, 0x0002FFF6, 0x02000200, 0xFFFFFFFF,
	0xFFFBFFFE, 0xFFF70255, 0x0000FFFF, 0xFFFFFFFB, 0x0002FFF7, 0x02550200, 0xFFFFFFFF,
};

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

// NONMATCHING (99.91%): in the type>=8 branch the (f32)(i-1) int->float conversion is allocated to f0
// instead of the original's f1 (3 register-only diffs: lfd/fsubs f1->f0 and the 0.25f constant f0->f1).
// Every other instruction matches.
EffectEntry* effWhirlwindN64Entry(s32 a1, s32 a2, s32 a3, f32 a4) {
	s32 v31;
	s32 v30;
	s32 v29;
	s32 v28;
	EffectEntry* effect;
	EffWhirlwindData* data;
	EffWhirlwindData* p;
	f32 f;
	s32 i;
	s32 color;
	effect = effEntry();
	effect->type = str_802FA168;
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0x168);
	effect->userdata = data;
	effect->callback = effWhirlwindMain;
	f = 1.2f * a4;
	data->type = a1;
	data->field_0x4 = (MarioWork*)a2;
	data->field_0x14 = f;
	data->field_0x18 = f;
	data->field_0x20 = a3;
	data->field_0x24 = a3;
	data->field_0x1C = 0;
	data->field_0x28 = 0;

	p = data + 1;
	i = 1;
	v31 = 11;
	v30 = 120;
	v29 = 25;
	v28 = 38;
	for (; i < 5; i++, p++) {
		switch (a1) {
		case 0:
		case 2:
		case 4:
		case 6:
			p->x = 0.0f;
			p->y = (f32)v31;
			p->z = 0.0f;
			p->field_0x38 = 0.0f;
			p->field_0x3C = 60.0f;
			p->field_0x40 = 0.0f;
			p->field_0x2C = 0.0f;
			p->field_0x30 = (f32)v30;
			p->field_0x34 = 0.0f;
			p->field_0x14 = 1.0f;
			break;
		case 1:
		case 3:
		case 5:
		case 7:
			p->x = 0.0f;
			p->y = (f32)v31;
			p->z = 0.0f;
			p->field_0x38 = 0.0f;
			p->field_0x3C = -60.0f;
			p->field_0x40 = 0.0f;
			p->field_0x2C = 0.0f;
			p->field_0x30 = (f32)v30;
			p->field_0x34 = 0.0f;
			p->field_0x14 = 1.0f;
			p->field_0x46 = 0xFF;
			p->field_0x45 = 0xFF;
			p->field_0x44 = 0xFF;
			break;
		default:
			p->x = 0.0f;
			p->y = 0.5f * ((MarioWork*)a2)->wPlayerCollisionBox.y;
			p->z = 0.0f;
			p->field_0x38 = (f32)(((rand() % 2) * 2 - 1) * 4);
			p->field_0x3C = 0.0f;
			p->field_0x40 = (f32)(((rand() % 2) * 2 - 1) * 4);
			p->field_0x14 = 0.5f * ((f32)(i - 1) * 0.25f) + 0.5f;
			p->field_0x2C = (f32)v29;
			p->field_0x30 = (f32)((i - 1) * 360 / 4);
			p->field_0x34 = (f32)(360 - v28);
			break;
		}

		switch (a1) {
		case 2:
		case 3:
			color = 1;
			break;
		case 4:
		case 5:
			color = 2;
			break;
		case 6:
		case 7:
			color = (i & 1) + 1;
			break;
		default:
			color = 0;
			break;
		}
		p->field_0x44 = col_r[color];
		p->field_0x45 = col_g[color];
		p->field_0x46 = col_b[color];

		v31 += 7;
		v30 += 120;
		v29 += 25;
		v28 += 38;
	}
	return effect;
}

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

// TEV colour setup for one particle; kept as an inline so the 4-byte colour scratch lands in the
// low stack slot the original uses (same idiom as eff_bush_n64's bushSetColor).
static inline void whirlwindSetColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor color = unk_80423088;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(1, color);
}

void effWhirlwindDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	EffWhirlwindData* data = effect->userdata;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	s32 type;
	s32 alpha;
	f32 scaleA;
	f32 scaleB;
	s32 i;

	alpha = data->field_0x1C;
	scaleA = data->field_0x14;
	scaleB = data->field_0x18;
	type = data->type;

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scale, 0x1e, 1);
	effGetTexObjN64(0x58, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
	GXSetTevAlphaIn(0, 7, 4, 1, 7);

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)eff_whirlwind_v);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'x', 0.017453292f * data->field_0x2C);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x34);
		PSMTXConcat(trans, rot, trans);
		PSMTXScale(scale, scaleA * data->field_0x14, scaleA * data->field_0x14, scaleA * data->field_0x14);
		PSMTXConcat(trans, scale, trans);
		PSMTXRotRad(rot, 'y', 0.017453292f * data->field_0x30);
		PSMTXConcat(trans, rot, rot);

		if (type < 8) {
			PSMTXTrans(trans, 3.0f - 3.0f * (scaleA - scaleB) / scaleB, 0.0f, 0.0f);
			PSMTXConcat(rot, trans, trans);
		} else {
			PSMTXTrans(trans, 2.0f, 0.0f, 0.0f);
			PSMTXConcat(rot, trans, trans);
		}
		PSMTXConcat(work, trans, trans);

		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);

		whirlwindSetColor(data->field_0x44, data->field_0x45, data->field_0x46, alpha);
		GXBegin(0x90, 0, 0x48);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		tri2(4, 5, 1, 0, 4, 1, 0, 0);
		tri2(3, 2, 6, 0, 3, 6, 7, 0);
		tri2(8, 9, 10, 0, 8, 10, 11, 0);
		tri2(12, 13, 9, 0, 12, 9, 8, 0);
		tri2(14, 15, 13, 0, 14, 13, 12, 0);
		tri2(16, 17, 15, 0, 16, 15, 14, 0);
		tri2(7, 6, 18, 0, 7, 18, 19, 0);
		tri2(20, 21, 17, 0, 20, 17, 16, 0);
		tri2(22, 23, 21, 0, 22, 21, 20, 0);
		tri2(24, 25, 23, 0, 24, 23, 22, 0);
		tri2(19, 18, 25, 0, 19, 25, 24, 0);
	}
}
