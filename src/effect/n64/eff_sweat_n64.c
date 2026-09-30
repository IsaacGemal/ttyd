#include "eff/n64/eff_sweat_n64.h"
#include "matching.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
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
extern const f32 float_9p6_8041ffb0;
extern const f32 float_6p4_8041ffb4;
extern const f32 float_0p5_8041ffc0;
extern const f32 float_4p8_8041ffc4;
extern const f32 float_1_8041ffc8;
extern const u32 dat_8041ffa8;

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
		Vec temp = vec3_802fb810;
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

static inline void effSweatSetColor(u8 alpha) {
	GXColor color = *(GXColor*)&dat_8041ffa8;
	color.a = alpha;
	GXSetChanMatColor(4, color);
}

// MATCH_F32 forces float_0/float_1 to reload before every FIFO store (a file-scope volatile would
// break effSweatN64Entry, which shares float_0).
void effSweatDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSweatData* data;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx trans;
	Mtx rot;
	Mtx view;
	s32 i;
	f32 negHeight;
	// Declaration order is part of the match: y1 first makes MWCC pin it to f1 and shift the rest.
	f32 y1, x0, x1, y0;

	camera = camGetPtr(camId);
	data = effect->userdata;
	PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041ffac * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, view);
	PSMTXConcat(camera->view, view, view);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
	GXSetTevAlphaIn(0, 7, 5, 4, 7);
	GXSetCullMode(0);
	effGetTexObj(0x5D, &tex);
	GXLoadTexObj(&tex, 0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	negHeight = -float_9p6_8041ffb0;
	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			continue;
		}
		effSweatSetColor(data->alpha);

		PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(rot, 'z', float_deg2rad_8041ffac * data->angle);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(view, trans, trans);
		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);

		GXBegin(0x80, 0, 4);
		x1 = float_6p4_8041ffb4;
		if (data->angle > float_0_8041ffbc) {
			x1 = x1 * float_neg1_8041ffb8;
		}
#ifdef MUST_MATCH
		// Splitting the negate from the scaling lets MWCC coalesce it into x0's register, as retail
		// does; the natural single expression gets a separate temp. y1 is declared before x0 for the
		// same reason (its live range pins f1). Same values as the #else form.
		x0 = -x1;
		x1 = x1 * float_0p5_8041ffc0;
		x0 = x0 * float_0p5_8041ffc0;
#else
		x0 = float_0p5_8041ffc0 * -x1;
		x1 = x1 * float_0p5_8041ffc0;
#endif
		y1 = float_4p8_8041ffc4;
		y0 = negHeight * float_0p5_8041ffc0;
		GXWGFifo.f32 = x0;
		GXWGFifo.f32 = y1;
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = x1;
		GXWGFifo.f32 = y1;
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = MATCH_F32(float_1_8041ffc8);
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = x1;
		GXWGFifo.f32 = y0;
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = MATCH_F32(float_1_8041ffc8);
		GXWGFifo.f32 = MATCH_F32(float_1_8041ffc8);
		GXWGFifo.f32 = x0;
		GXWGFifo.f32 = y0;
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = MATCH_F32(float_0_8041ffbc);
		GXWGFifo.f32 = MATCH_F32(float_1_8041ffc8);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_8041ffa8 = 0xFFFFFF00;
const f32 float_deg2rad_8041ffac = 0.017453292f;
const f32 float_9p6_8041ffb0 = 9.6f;
const f32 float_6p4_8041ffb4 = 6.4f;
const f32 float_neg1_8041ffb8 = -1.0f;
const f32 float_0_8041ffbc = 0.0f;
const f32 float_0p5_8041ffc0 = 0.5f;
const f32 float_4p8_8041ffc4 = 4.8f;
const f32 float_1_8041ffc8 = 1.0f;
const f32 float_16_8041ffd8 = 16.0f;
