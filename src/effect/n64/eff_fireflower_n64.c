#include "eff/n64/eff_fireflower_n64.h"
#include "eff/n64/eff_fire_spark_n64.h"
#include "eff/n64/eff_rothammer_success_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .data float pairs and vertex arrays, in the original's order.
static f32 negone_one_374[2] = { -1.0f, 1.0f };
f32 lbl_8039CA20[2] = { 1.0f, -1.0f };
static f32 one_negone_373[2] = { 1.0f, -1.0f };
f32 lbl_8039CA30[2] = { -1.0f, 1.0f };
static f32 negone_one_360[2] = { -1.0f, 1.0f };
f32 lbl_8039CA40[2] = { 1.0f, -1.0f };
static f32 one_negone_359[2] = { 1.0f, -1.0f };
u32 size4x4_tex4x4_vtx[14] = {
	0xFFFEFFFE, 0x00000000, 0x0000FF00, 0x00FF0002,
	0xFFFE0000, 0x02000000, 0xFF0000FF, 0x00020002,
	0x00000200, 0x0200FF00, 0x00FFFFFE, 0x00020000,
	0x00000200, 0xFF0000FF,
};
static u32 size16x16_tex16x16_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008,
	0xFFF80000, 0x02000000, 0xFFFFFFFF, 0x00080008,
	0x00000200, 0x0200FFFF, 0xFFFFFFF8, 0x00080000,
	0x00000200, 0xFFFFFFFF,
};
static u32 size32x32_tex32x32_vtx[14] = {
	0xFFF00000, 0x00020000, 0x0000FFFF, 0xFFFF0010,
	0x00000002, 0x04000000, 0xFFFFFFFF, 0x00100020,
	0x00020400, 0x0400FFFF, 0xFFFFFFF0, 0x00200002,
	0x00000400, 0xFFFFFFFF,
};

#ifdef MUST_MATCH
// The original reaches the three vertex arrays from the address 8 bytes before its .data.
extern u8 lbl_8039CA10[];
#endif

// .sdata scale table indexed by data->timer in state 1 (signed bytes: loaded with extsb).
static s8 scale_data[] = "27<AFKPUZ_dgigd";

// Building each GXColor and calling GXSetTevColor inside the inline gives the original's
// local/outgoing-argument stack slots (the plain form groups them the wrong way).
static inline void effFireFlowerSetColor1(u8 a) {
	GXColor c = *(GXColor*)&dat_8041eec8;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void effFireFlowerSetColor2(u8 a) {
	GXColor c = *(GXColor*)&dat_8041eecc;
	c.a = a;
	GXSetTevColor(2, c);
}

EffectEntry* effFireFlowerN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effFireFlowerData* data;
	effFireFlowerData* p;
	s32 i;

	effect = effEntry();
	effect->type = "FireFlowerN64";
	effect->count = 0x1f;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireFlowerData) * 0x1f);
	effect->userdata = data;
	effect->callback = effFireFlowerMain;

	if (type == 1) {
		data->mode = 1;
		type = 0;
	}
	data->state = 0;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	if (duration <= 0) {
		duration = 1000;
	}
	data->lifetime = duration;
	data->timer = 0;
	data->field_0x48 = 0;
	data->field_0x3C = 0xff;
	data->scale = 0.5f;
	data->field_0x34 = 0;
	data->field_0x38 = 0;
	data->field_0x28 = 0.0f;
	data->field_0x2C = -10.0f;

	p = &data[1];
	for (i = 1; i < 0x1f; i++, p++) {
		p->type = 1;
		p->x = 0.0f;
		p->y = 0.0f;
		p->z = 0.0f;
		p->velX = 8.0f;
		p->velY = 0.0f;
		p->velZ = (f32)(rand() % 20 - 8);
		p->field_0x20 = 0.05f * (f32)(rand() % 10 - 5);
		p->field_0x24 = (f32)(rand() % 80 + 5);
		p->field_0x3C = 0xff;
		p->field_0x4C = i - 1;
		p->timer = 10;
		p->velX /= 1.5f;
		p->velY /= 1.5f;
		p->velZ /= 1.5f;
		p->field_0x4C *= 2;
	}
	return effect;
}

// (1) case 0 hoists `li r4, 7` above the mode test; the original emits it after `lfs f2, data->y`.
// (2) case 6 coalesces `field_0x28 + 18.0f` into f1; the original keeps it in f2 and puts
// double_to_int in f3 and 0.9f in f1. Both are scheduler/register-allocation choices that could not
// be steered from C (tried compound vs plain assignment, operand order, temps, and the opt pragmas).
// NONMATCHING (98.96%): see the two differences above.
void effFireFlowerMain(EffectEntry* effect) {
	effFireFlowerData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 i;
	s32 timer;
	s32 state;
	f32 fx;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->timer++;
	data->field_0x48++;
	state = data->state;
	if (state == 7) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	switch (state) {
	case 0:
		if (data->mode == 0) {
			fx = data->x - 10.0f;
		} else {
			fx = 10.0f + data->x;
		}
		effRotHammerSuccessN64Entry(0, 7, fx, data->y, data->z, 0x14);
		data->field_0x34 = 0;
		data->field_0x38 = 0;
		data->state++;
		data->timer = 0;
		break;
	case 1:
		if (timer >= 0x10) {
			data->state++;
			data->timer = 0;
		} else {
			data->scale = (f32)scale_data[timer - 1] * 0.01f;
			data->field_0x34 = 0;
			data->field_0x38 = 0;
		}
		break;
	case 2:
		if (timer >= 4) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x34 = 1;
		}
		break;
	case 3:
		if (timer >= 0x1E) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x34 = 2;
		}
		break;
	case 4:
		if (timer > data->lifetime) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x2C = 0.0f;
			data->field_0x34 = 3;
			if ((timer >> 2) & 1) {
				data->scale = 1.0f;
			} else {
				data->scale = 1.05f;
			}
		}
		break;
	case 5:
		if (timer >= 0x14) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x34 = 0;
			data->field_0x2C = -10.0f;
		}
		break;
	case 6:
		data->field_0x34 = 0;
		data->field_0x28 += 18.0f;
		data->field_0x3C = (s32)((f32)data->field_0x3C * 0.9f);
		if (data->field_0x28 >= 720.0f) {
			data->field_0x28 = 720.0f;
			data->state++;
		}
		break;
	}

	state = data->state;
	data++;
	if (state >= 4 && state <= 6) {
		for (i = 1; i < effect->count; i++, data++) {
			if (data->field_0x4C > 0) {
				data->field_0x4C--;
				if (data->field_0x4C > 0) {
					continue;
				}
			}
			data->velY += data->field_0x20;
			data->x += data->velX;
			data->y += data->velY;
			data->z += data->velZ;
			if (state == 4 && data->x > 200.0f) {
				data->x = 0.0f;
				data->y = 0.0f;
				data->velY = 0.0f;
				data->z = 0.0f;
			}
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFireFlowerDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effFireFlowerDisp, effect);
#endif
}

void effFireFlowerDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	CameraEntry* cam;
	effFireFlowerData* data;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx m;
	Mtx m2;
	s32 state;
	s32 i;
	s32 field34;
	f32 scaleVal;
	f32 field28;
	f32 field2C;
#ifdef MUST_MATCH
	u8* vtx = lbl_8039CA10;
#endif
	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	// Cache the fields the original keeps in callee-saved registers across the calls below.
	field34 = data->field_0x34;
	scaleVal = data->scale;
	state = data->state;
	field28 = data->field_0x28;
	field2C = data->field_0x2C;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXScale(scale, data->mode != 0 ? -1.0f : 1.0f, 1.0f, 1.0f);
	PSMTXConcat(trans, rot, m);
	PSMTXConcat(m, scale, m);

	effFireFlowerSetColor1(data->field_0x3C);
	effFireFlowerSetColor2(data->field_0x38);

	GXSetCullMode(0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);

	if (state >= 4 && state <= 6) {
		PSMTXTrans(trans, 0.0f, 16.0f, 0.0f);
		PSMTXConcat(m, trans, m2);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
		GXSetTevAlphaIn(0, 7, 4, 5, 7);
		effGetTexObjN64(0x36, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
		PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
		GXLoadTexMtxImm(scale, 0x1e, 1);

		for (i = 1, data++; i < effect->count; i++, data++) {
			if (data->field_0x4C != 0) {
				continue;
			}
			PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x24);
			PSMTXTrans(trans, data->x, data->y, data->z);
			PSMTXConcat(rot, trans, trans);
			PSMTXConcat(m2, trans, trans);
			PSMTXConcat(cam->view, trans, trans);
			GXLoadPosMtxImm(trans, 0);
			GXSetCurrentMtx(0);
			if (data->type != 0) {
#ifdef MUST_MATCH
				effSetVtxDescN64((effVtxArray*)(vtx + 0x78));
#else
				effSetVtxDescN64((effVtxArray*)size16x16_tex16x16_vtx);
#endif
			} else {
#ifdef MUST_MATCH
				effSetVtxDescN64((effVtxArray*)(vtx + 0x40));
#else
				effSetVtxDescN64((effVtxArray*)size4x4_tex4x4_vtx);
#endif
			}
			GXBegin(0x90, 0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
		}
	}

	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 1, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0, 8, 5, 0xf);
	GXSetTevAlphaIn(1, 0, 4, 2, 7);
	GXSetTevOrder(2, 0xff, 0xff, 0xff);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xf, 0xf, 0xf, 0);
	GXSetTevAlphaIn(2, 7, 1, 0, 7);
	effGetTexObjN64(0x37, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXLoadTexObj(&texObj, 1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
	PSMTXScale(scale, 0.03125f, 0.0078125f, 0.0f);
	PSMTXTrans(trans, 0.0f, (f32)(3 - field34) * 32.0f, 0.0f);
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, 0x1e, 1);
	PSMTXTrans(trans, 0.0f, (f32)(2 - field34) * 32.0f, 0.0f);
	PSMTXConcat(scale, trans, trans);
	GXLoadTexMtxImm(trans, 0x21, 1);
	PSMTXTrans(trans, field2C, 0.0f, 0.0f);
	PSMTXScale(scale, scaleVal, scaleVal, scaleVal);
	PSMTXConcat(trans, scale, trans);
	PSMTXRotRad(rot, 'y', 0.017453292f * field28);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(m, trans, trans);
	PSMTXConcat(cam->view, trans, trans);
	GXLoadPosMtxImm(trans, 0);
	GXSetCurrentMtx(0);
#ifdef MUST_MATCH
	effSetVtxDescN64((effVtxArray*)(vtx + 0xb0));
#else
	effSetVtxDescN64((effVtxArray*)size32x32_tex32x32_vtx);
#endif
	GXBegin(0x90, 0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}
