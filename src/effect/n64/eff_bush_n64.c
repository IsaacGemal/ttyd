#include "eff/n64/eff_bush_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "mario/mario.h"
#include "memory.h"
#include <stdlib.h>
#include <math.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>


// .sdata2 constants used by effBushMain

// .rodata: zero Vec copied into the local position before the particle fields are set


static inline void bushSetColor(u8 a) {
	GXColor color = (GXColor){ 0x14, 0x64, 0x14, 0x00 };
	color.a = a;
	GXSetTevColor(1, color);
}

// .data: retail vertex array (two 0x38-byte effVtxArray entries).
static u32 size12x12_tex64x64_vtx[28] = {
	0xFFFAFFFA, 0x00000000, 0x00001955, 0x11FF0006,
	0xFFFA0000, 0x08000000, 0x195511FF, 0x00060006,
	0x00000800, 0x080068AC, 0x10FFFFFA, 0x00060000,
	0x00000800, 0x68AC10FF, 0xFFFAFFFA, 0x00000000,
	0x08001837, 0x21FF0006, 0xFFFA0000, 0x08000800,
	0x183721FF, 0x00060006, 0x00000800, 0x1000306C,
	0x25FFFFFA, 0x00060000, 0x00001000, 0x306C25FF,
};

EffectEntry* effBushN64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	BushN64Data* data;
	BushN64Data* p;
	s32 i;

	effect = effEntry();
	effect->type = "BushN64";
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0x104);
	effect->userdata = data;
	effect->callback = effBushMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->lifetime = 100;
	data->age = 0;
	data->alpha = 0;
	p = data + 1;
	for (i = 1; i < 5; i++, p++) {
		p->x = (f32)(rand() % 30 - 15);
		p->y = 0.0f;
		p->z = (f32)((rand() & 0x1E) - 15);
		p->rotZ = (f32)(rand() & 0x168);
		p->rotY = (f32)(rand() & 0x168);
		p->swayPhase = (f32)((rand() % 100) / 10);
		p->velX = 0.0f;
		p->velY = 0.0f;
	}
	return effect;
}

void effBushMain(EffectEntry* effect) {
	BushN64Data* data = effect->userdata;
	Vec position;
	Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 type;
	s32 life;
	s32 i;
	f32 wave;
	f32 move;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	type = data->type;
	if (data->age >= 5 && data->age < 15) {
		switch (type) {
		case 0:
			if (marioGetPtr()->currMotionId == MARIO_MOTION_STAY) {
				return;
			}
			break;
		case 1:
			break;
		}
	}

	data->lifetime--;
	data->age++;
	if (data->age > 324000) {
		data->age = 256;
	}
	life = data->lifetime;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	if (data->age < 10) {
		data->alpha = (s32)(0.3f * (255 - data->alpha) + data->alpha);
	}
	if (life < 10) {
		data->alpha *= 0.8f;
	}

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		wave = (f32)sin(6.2831855f * (2.0f * data->rotZ) / 360.0f);
		data->velY += -0.05f;
		move = 0.2f * wave;
		data->velX += move;
		if (type == 0) {
			data->velX *= 0.94f;
			wave = (f32)sin(6.2831855f * data->rotZ / 360.0f);
			data->velY *= 0.95f + 0.05f * wave;
		} else {
			data->velY += -0.05f;
			data->velX *= 0.92f;
		}
		data->swayPhase += (f32)(rand() % 50) - 25.0f;
		data->rotZ += 10.0f * (f32)sin(6.2831855f * data->swayPhase / 360.0f);
		data->rotY += 10.0f * (f32)cos(6.2831855f * (data->swayPhase / 2.0f) / 360.0f);
		data->x += data->velX;
		data->y += data->velY;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effBushDisp, effect);
}

void effBushDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	BushN64Data* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	s32 i;
	f32 deg2rad;

	cam = camGetPtr(camId);
	data = effect->userdata;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	bushSetColor(data->alpha);
	data++;
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scale, 4.0f, 1.0f, 0.0f);
	GXLoadTexMtxImm(scale, 0x1e, 1);
	effGetTexObjN64(0x2a, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xa, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 3);
	GXSetVtxDesc(0xb, 3);
	GXSetVtxDesc(0xd, 3);
	GXSetVtxAttrFmt(0, 9, 1, 3, 0);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xc);
	GXSetArray(9, size12x12_tex64x64_vtx, 0xe);
	GXSetArray(0xb, ((effVtxArray*)size12x12_tex64x64_vtx)->clr0, 0xe);
	GXSetArray(0xd, ((effVtxArray*)size12x12_tex64x64_vtx)->tex0, 0xe);

	deg2rad = 0.017453292f;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'z', deg2rad * data->rotZ);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'y', deg2rad * data->rotY);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x90, 0, 0xc);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		tri2(6, 5, 4, 4, 7, 6, 4, 4);
	}
}
