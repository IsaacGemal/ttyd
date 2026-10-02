#include "eff/n64/eff_flame_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#ifdef MUST_MATCH
#include <dolphin/os/OSFastCast.h>
#endif
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") f32 lbl_8039CDA8[2] = { -1.0f, 1.0f };


// .sbss2 scratch colors referenced by the original display code.
GXColor unk_80423110;
GXColor unk_80423114;

/* MUST_MATCH: retail inlines a per-vertex helper; each inlining gets its own Vec slot and
   the table base argument is passed as `flameTbl + table offset`, producing two adds. */
static inline void effFlameSetVtx(effFlameData* data, Mtx mtx, s32 idx, effFlameVtx* tbl) {
	Vec v;
	memcpy(&data->vtx[idx], &tbl[idx], sizeof(effFlameVtx));
#ifdef MUST_MATCH
	OSInitFastCast();
	OSs16tof32(&data->vtx[idx].pos[0], &v.x);
	OSs16tof32(&data->vtx[idx].pos[1], &v.y);
	OSs16tof32(&data->vtx[idx].pos[2], &v.z);
	PSMTXMultVec(mtx, &v, &v);
	OSInitFastCast();
	OSf32tos16(&v.x, &data->vtx[idx].pos[0]);
	OSf32tos16(&v.y, &data->vtx[idx].pos[1]);
	OSf32tos16(&v.z, &data->vtx[idx].pos[2]);
#else
	v.x = data->vtx[idx].pos[0];
	v.y = data->vtx[idx].pos[1];
	v.z = data->vtx[idx].pos[2];
	PSMTXMultVec(mtx, &v, &v);
	data->vtx[idx].pos[0] = v.x;
	data->vtx[idx].pos[1] = v.y;
	data->vtx[idx].pos[2] = v.z;
#endif
}
EffectEntry* effFlameN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* entry;
	effFlameData* data;

	entry = effEntry();
	entry->type = "FlameN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFlameData));
	entry->userdata = data;
	entry->callback = effFlameMain;
	entry->flags |= 2;

	data->type = type;
	data->life = 100;
	data->counter = 0;
	data->alpha = 0;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->rot = 0.0f;
	data->field_0x40 = 0.0f;
	data->field_0x34 = 0.0f;
	data->field_0x58 = 0.0f;
	data->field_0x4C = 0.0f;

	switch (type) {
	case 0:
		data->color0[0] = 0x4B;
		data->color0[1] = 0x4B;
		data->color0[2] = 0x4B;
		data->color1[0] = 0xFF;
		data->color1[1] = 0xF7;
		data->color1[2] = 0x9B;
		data->color1[3] = 0;
		data->field_0x1C = 2.0f * a5;
		data->field_0x20 = 0.6f * a5;
		data->field_0x24 = a5;
		data->field_0x38 = -0.9f;
		data->field_0x44 = 0.04f;
		data->field_0x3C = 0.04f;
		data->field_0x48 = 0.9f;
		data->field_0x50 = -0.9f;
		data->field_0x5C = 0.04f;
		data->field_0x54 = 0.04f;
		data->field_0x60 = 0.9f;
		break;
	case 1:
		data->color0[0] = 0x4B;
		data->color0[1] = 0x4B;
		data->color0[2] = 0x4B;
		data->color1[0] = 0xFF;
		data->color1[1] = 0;
		data->color1[2] = 0;
		data->color1[3] = 0;
		data->field_0x1C = a5;
		data->field_0x20 = a5;
		data->field_0x24 = a5;
		data->field_0x38 = 0.0f;
		data->field_0x44 = 0.04f;
		data->field_0x3C = 0.04f;
		data->field_0x48 = 0.9f;
		data->field_0x50 = 0.0f;
		data->field_0x5C = 0.04f;
		data->field_0x54 = 0.04f;
		data->field_0x60 = 0.9f;
		break;
	case 2:
		data->color0[0] = 0x19;
		data->color0[1] = 0x79;
		data->color0[2] = 0x8E;
		data->color1[0] = 0x22;
		data->color1[1] = 0;
		data->color1[2] = 0xFF;
		data->color1[3] = 0x51;
		data->field_0x1C = a5;
		data->field_0x20 = a5;
		data->field_0x24 = a5;
		data->field_0x38 = 0.0f;
		data->field_0x44 = 0.04f;
		data->field_0x3C = 0.2f;
		data->field_0x48 = 1.3f;
		data->field_0x50 = 0.0f;
		data->field_0x5C = 0.04f;
		data->field_0x54 = 0.16000001f;
		data->field_0x60 = 1.04f;
		break;
	default:
		data->color0[0] = 0xB3;
		data->color0[1] = 0xA9;
		data->color0[2] = 0x55;
		data->color1[0] = 0x22;
		data->color1[1] = 0;
		data->color1[2] = 0xFF;
		data->color1[3] = 0x3F;
		data->field_0x1C = a5;
		data->field_0x20 = a5;
		data->field_0x24 = a5;
		data->field_0x38 = 0.0f;
		data->field_0x44 = 0.04f;
		data->field_0x3C = 0.1f;
		data->field_0x48 = 1.7f;
		data->field_0x50 = 0.0f;
		data->field_0x5C = 0.04f;
		data->field_0x54 = 0.080000006f;
		data->field_0x60 = 1.36f;
		break;
	}

	data->field_0x10 = data->position.x;
	data->field_0x14 = data->position.y;
	data->field_0x18 = data->position.z;

	return entry;
}
void effFlameMain(EffectEntry* effect) {
	effFlameData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 life;
	s32 type;
	s32 counter;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}
	data->counter++;
	if (data->counter > 0x4F1A0) {
		data->counter = 0x100;
	}
	if (data->life <= 16) {
		data->life--;
	}
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	type = data->type;
	counter = data->counter;
	if (type <= 1) {
		if (counter <= 10) {
			data->alpha = data->alpha + (0x80 - data->alpha) * 0.5f;
		}
	}
	else {
		if (counter <= 10) {
			data->alpha = counter * 0xFF / 10;
		}
	}
	if (life < 16) {
		data->alpha = life * 16;
	}

	if (type == 0) {
		data->field_0x38 += 0.02f * (data->field_0x3C - data->field_0x38);
		data->field_0x44 += 0.02f * (data->field_0x48 - data->field_0x44);
		data->field_0x1C += 0.04f * (data->field_0x24 - data->field_0x1C);
		data->field_0x20 += 0.04f * (data->field_0x24 - data->field_0x20);
	}
	else {
		data->field_0x38 += 0.02f * (data->field_0x3C - data->field_0x38);
		data->field_0x44 += 0.02f * (data->field_0x48 - data->field_0x44);
		data->field_0x50 += 0.02f * (data->field_0x54 - data->field_0x50);
		data->field_0x5C += 0.02f * (data->field_0x60 - data->field_0x5C);
	}

	data->field_0x34 += data->field_0x38;
	if (data->field_0x34 < 0.0f) {
		data->field_0x34 += 64.0f;
	}
	else if (data->field_0x34 > 64.0f) {
		data->field_0x34 -= 64.0f;
	}

	data->field_0x40 += data->field_0x44;
	if (data->field_0x40 < 0.0f) {
		data->field_0x40 += 128.0f;
	}
	else if (data->field_0x40 > 128.0f) {
		data->field_0x40 -= 128.0f;
	}

	data->field_0x4C += data->field_0x50;
	if (data->field_0x4C < 0.0f) {
		data->field_0x4C += 64.0f;
	}
	else if (data->field_0x4C > 64.0f) {
		data->field_0x4C -= 64.0f;
	}

	data->field_0x58 += data->field_0x5C;
	if (data->field_0x58 < 0.0f) {
		data->field_0x58 += 128.0f;
	}
	else if (data->field_0x58 > 128.0f) {
		data->field_0x58 -= 128.0f;
	}

	data->field_0x10 += 0.1f * (data->position.x - data->field_0x10);
	data->field_0x14 += 0.1f * (data->position.y - data->field_0x14);
	data->field_0x18 += 0.1f * (data->position.z - data->field_0x18);

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFlameDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effFlameDisp, effect);
#endif
}

// NONMATCHING (95.82%): the 24 inlined vertex transforms convert the s16 positions with the SDK's paired-single
// inlines (`OSs16tof32`/`OSf32tos16` in OSFastCast.h, GQR5 type 7) after `OSInitFastCast()`, as the original does.
// What differs: the frame is 0x490 against the original's 0x4c0 (its 24 Vec slots sit at 0x138-0x24c), the
// table address is passed as `flameTbl + offset` and the original keeps that add separate from the per-vertex
// `+ idx * 14` where this build folds the two, and the destination address is recomputed per call in the
// original but kept in a register here. The end-of-draw type test picks the shorter branch polarity.
void effFlameDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFlameVtx* flameTbl = (effFlameVtx*)lbl_8039CDA8;
	CameraEntry* cam = camGetPtr(camId);
	effFlameData* data = effect->userdata;
	s32 type = data->type;
	s32 alpha = data->alpha;
	Mtx mtx1;
	Mtx mtx2;
	Mtx mtx3;
	GXTexObj texObj;
	GXColor color;

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);

	switch (type) {
	case 0:
	case 1:
		GXSetNumTevStages(4);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 3);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 3);
		GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
		GXSetTevAlphaIn(0, 7, 7, 7, 4);
		GXSetTevOrder(1, 1, 1, 0xFF);
		GXSetTevColorOp(1, 0, 0, 0, 0, 0);
		GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
		GXSetTevColorIn(1, 0xF, 6, 6, 0xC);
		GXSetTevAlphaIn(1, 4, 7, 3, 4);
		GXSetTevOrder(2, 1, 1, 0xFF);
		GXSetTevColorOp(2, 1, 0, 0, 1, 0);
		GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
		GXSetTevColorIn(2, 0xF, 6, 8, 0);
		GXSetTevAlphaIn(2, 7, 3, 0, 7);
		GXSetTevOrder(3, 0xFF, 0xFF, 0xFF);
		GXSetTevColorOp(3, 1, 0, 0, 1, 0);
		GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
		GXSetTevColorIn(3, 0xF, 2, 0, 4);
		GXSetTevAlphaIn(3, 7, 1, 0, 7);
		effGetTexObjN64(0x2F, &texObj);
		GXLoadTexObj(&texObj, 0);
		effGetTexObjN64(0x30, &texObj);
		GXLoadTexObj(&texObj, 1);
		break;
	case 2:
		GXSetNumTevStages(5);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 3);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 3);
		GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
		GXSetTevAlphaIn(0, 7, 7, 7, 4);
		GXSetTevOrder(1, 1, 1, 0xFF);
		GXSetTevColorOp(1, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(1, 0, 0, 0, 0, 0);
		GXSetTevColorIn(1, 0xF, 2, 8, 2);
		GXSetTevAlphaIn(1, 7, 3, 4, 3);
		GXSetTevOrder(2, 0xFF, 0xFF, 0xFF);
		GXSetTevColorOp(2, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(2, 1, 0, 0, 1, 0);
		GXSetTevColorIn(2, 0xF, 0xF, 0xF, 0);
		GXSetTevAlphaIn(2, 7, 2, 3, 0);
		GXSetTevOrder(3, 0xFF, 0xFF, 0xFF);
		GXSetTevColorOp(3, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(3, 0, 0, 0, 0, 0);
		GXSetTevColorIn(3, 0xF, 0xF, 0xF, 0);
		GXSetTevAlphaIn(3, 7, 0, 1, 7);
		GXSetTevOrder(4, 0xFF, 0xFF, 0xFF);
		GXSetTevColorOp(4, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(4, 1, 0, 0, 1, 0);
		GXSetTevColorIn(4, 0xF, 0xF, 0xF, 0);
		GXSetTevAlphaIn(4, 7, 2, 1, 0);
		effGetTexObjN64(0x31, &texObj);
		GXLoadTexObj(&texObj, 0);
		effGetTexObjN64(0x32, &texObj);
		GXLoadTexObj(&texObj, 1);
		break;
	}

	// first half of the vertices
	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	if (type == 2) {
		PSMTXRotRad(mtx2, 'y', data->rot * 0.017453292f);
	}
	else {
		cam = camGetPtr(CAMERA_3D);
		PSMTXRotRad(mtx2, 'y', -cam->viewYaw * 0.017453292f);
	}
	PSMTXConcat(mtx1, mtx2, mtx3);
	PSMTXScale(mtx2, data->field_0x1C, data->field_0x20, 1.0f);
	PSMTXConcat(mtx3, mtx2, mtx3);
	if (type == 0) {
		PSMTXTrans(mtx1, 10.0f * (-(data->field_0x1C - data->field_0x24) / data->field_0x24), 0.0f, 0.0f);
		PSMTXConcat(mtx3, mtx1, mtx3);
	}
	switch (type) {
	case 0:
		effFlameSetVtx(data, mtx3, 2, (effFlameVtx*)((u8*)flameTbl + 0x40));
		effFlameSetVtx(data, mtx3, 3, (effFlameVtx*)((u8*)flameTbl + 0x40));
		effFlameSetVtx(data, mtx3, 6, (effFlameVtx*)((u8*)flameTbl + 0x40));
		effFlameSetVtx(data, mtx3, 7, (effFlameVtx*)((u8*)flameTbl + 0x40));
		break;
	case 1:
		effFlameSetVtx(data, mtx3, 2, (effFlameVtx*)((u8*)flameTbl + 0x120));
		effFlameSetVtx(data, mtx3, 3, (effFlameVtx*)((u8*)flameTbl + 0x120));
		effFlameSetVtx(data, mtx3, 6, (effFlameVtx*)((u8*)flameTbl + 0x120));
		effFlameSetVtx(data, mtx3, 7, (effFlameVtx*)((u8*)flameTbl + 0x120));
		break;
	default:
		effFlameSetVtx(data, mtx3, 2, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		effFlameSetVtx(data, mtx3, 3, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		effFlameSetVtx(data, mtx3, 6, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		effFlameSetVtx(data, mtx3, 7, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		break;
	}

	// second half of the vertices
	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	if (type == 2) {
		PSMTXRotRad(mtx2, 'y', data->rot * 0.017453292f);
	}
	else {
		cam = camGetPtr(CAMERA_3D);
		PSMTXRotRad(mtx2, 'y', -cam->viewYaw * 0.017453292f);
	}
	PSMTXConcat(mtx1, mtx2, mtx3);
	PSMTXScale(mtx2, data->field_0x1C, data->field_0x20, 1.0f);
	PSMTXConcat(mtx3, mtx2, mtx3);
	if (type == 0) {
		PSMTXTrans(mtx1, 10.0f * (-(data->field_0x1C - data->field_0x24) / data->field_0x24), 0.0f, 0.0f);
		PSMTXConcat(mtx3, mtx1, mtx3);
	}
	switch (type) {
	case 0:
		effFlameSetVtx(data, mtx3, 0, (effFlameVtx*)((u8*)flameTbl + 0x40));
		effFlameSetVtx(data, mtx3, 1, (effFlameVtx*)((u8*)flameTbl + 0x40));
		effFlameSetVtx(data, mtx3, 4, (effFlameVtx*)((u8*)flameTbl + 0x40));
		effFlameSetVtx(data, mtx3, 5, (effFlameVtx*)((u8*)flameTbl + 0x40));
		break;
	case 1:
		effFlameSetVtx(data, mtx3, 0, (effFlameVtx*)((u8*)flameTbl + 0x120));
		effFlameSetVtx(data, mtx3, 1, (effFlameVtx*)((u8*)flameTbl + 0x120));
		effFlameSetVtx(data, mtx3, 4, (effFlameVtx*)((u8*)flameTbl + 0x120));
		effFlameSetVtx(data, mtx3, 5, (effFlameVtx*)((u8*)flameTbl + 0x120));
		break;
	default:
		effFlameSetVtx(data, mtx3, 0, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		effFlameSetVtx(data, mtx3, 1, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		effFlameSetVtx(data, mtx3, 4, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		effFlameSetVtx(data, mtx3, 5, (effFlameVtx*)((u8*)flameTbl + 0xB0));
		break;
	}

	GXInvalidateVtxCache();
	DCFlushRange(data->vtx, 0x70);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)data->vtx);
	GXLoadPosMtxImm(cam->view, 0);
	GXSetCurrentMtx(0);

	color = unk_80423110;
	color.r = data->color0[0];
	color.g = data->color0[1];
	color.b = data->color0[2];
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);

	color = unk_80423114;
	color.r = data->color1[0];
	color.g = data->color1[1];
	color.b = data->color1[2];
	color.a = data->color1[3];
	GXSetTevColor(GX_TEVREG1, color);

	PSMTXScale(mtx2, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(mtx2, 0x1E, 1);
	PSMTXScale(mtx2, 0.03125f, 0.0078125f, 0.0f);
	PSMTXTrans(mtx1, data->field_0x34, data->field_0x40, 0.0f);
	PSMTXConcat(mtx2, mtx1, mtx1);
	GXLoadTexMtxImm(mtx1, 0x21, 1);
	GXBegin(0x90, 0, 6);
	tri2(4, 5, 6, 0, 4, 6, 7, 0);

	PSMTXScale(mtx2, 0.03125f, 0.0078125f, 0.0f);
	PSMTXTrans(mtx1, 30.0f + data->field_0x4C, 30.0f + data->field_0x58, 0.0f);
	PSMTXConcat(mtx2, mtx1, mtx1);
	GXLoadTexMtxImm(mtx1, 0x21, 1);
	if (type < 2 && type >= 0) {
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
	else {
		GXBegin(0x90, 0, 6);
		tri2(2, 1, 0, 0, 3, 2, 0, 0);
	}
}
