#include "eff/eff_bomb.h"
#include "driver/dispdrv.h"
#include "eff/eff_confetti.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

// .data tables. Each record is 3 bytes: texcoord offset, scaleX, scaleY (scaled by 100).
static u8 scale_dt[] = {
	0x00, 0x64, 0x64, 0x00, 0x67, 0x67, 0x01, 0x6A, 0x6A, 0x01, 0x6E, 0x6E,
	0x02, 0x71, 0x71, 0x02, 0x75, 0x75, 0x03, 0x78, 0x78, 0x03, 0x7C, 0x7C,
	0x04, 0x7F, 0x7F, 0x04, 0x83, 0x83, 0x05, 0x86, 0x86, 0x05, 0x89, 0x89,
	0x06, 0x8D, 0x8D, 0x06, 0x90, 0x90, 0x07, 0x94, 0x94, 0x07, 0x97, 0x97,
	0x08, 0x9B, 0x9B, 0x08, 0x9E, 0x9E, 0x09, 0xA2, 0xA2, 0x09, 0xA5, 0xA5,
	0x0A, 0xA8, 0xA8, 0x0A, 0xAC, 0xAC, 0x0B, 0xAF, 0xAF, 0x0B, 0xB3, 0xB3,
	0x0C, 0xB6, 0xB6, 0x0C, 0xBA, 0xBA, 0x0D, 0xBD, 0xBD, 0x0D, 0xC1, 0xC1,
	0x0E, 0xC4, 0xC4, 0x0E, 0xC8, 0xC8, 0x0F, 0xC8, 0xC8, 0x0F, 0xC8, 0xC8,
	0x0F, 0xC8, 0xC8, 0x0F, 0xC8, 0xC8, 0x0F, 0xC8, 0xC8, 0x0F, 0xC8, 0xC8,
};
static u8 scale_dt2[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x64, 0x64, 0x00, 0x65, 0x65, 0x01, 0x67, 0x67, 0x01, 0x69, 0x69,
	0x02, 0x6A, 0x6A, 0x02, 0x6C, 0x6C, 0x03, 0x6E, 0x6E, 0x03, 0x70, 0x70,
	0x04, 0x71, 0x71, 0x04, 0x73, 0x73, 0x05, 0x75, 0x75, 0x05, 0x76, 0x76,
	0x06, 0x78, 0x78, 0x06, 0x7A, 0x7A, 0x07, 0x7C, 0x7C, 0x07, 0x7D, 0x7D,
	0x08, 0x7F, 0x7F, 0x08, 0x81, 0x81, 0x09, 0x83, 0x83, 0x09, 0x84, 0x84,
	0x0A, 0x86, 0x86, 0x0A, 0x88, 0x88, 0x0B, 0x89, 0x89, 0x0B, 0x8B, 0x8B,
	0x0C, 0x8D, 0x8D, 0x0C, 0x8F, 0x8F, 0x0D, 0x90, 0x90, 0x0D, 0x92, 0x92,
	0x0E, 0x94, 0x94, 0x0E, 0x96, 0x96, 0x0F, 0x94, 0x94, 0x0F, 0x96, 0x96,
};

// The original has these constants in .sdata2. Declared const, MWCC folds their values into its own pool;
// the matching build keeps them writable so the loads reference the original symbols.
#ifdef MUST_MATCH
#define SDATA2_CONST __declspec(section ".sdata2")
#else
#define SDATA2_CONST const
#endif
SDATA2_CONST GXColor dat_8041e450 = { 0xFF, 0xFF, 0xFF, 0xFF };
SDATA2_CONST GXColor dat_8041e454 = { 0xFF, 0xFF, 0xFF, 0x30 };

// Local to this file in the original (`.fn effBombMain, local`).
static void effBombMain(EffectEntry* effect);
// Local to this file in the original (`.fn effBombDisp, local`).
static void effBombDisp(CameraId camId, void* param);

EffectEntry* effBombEntry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effBombData* data;
	effBombData* work;
	s32 i;
	f32 radius;

	entry = effEntry();
	entry->type = "Bomb";
	entry->count = 9;
	data = __memAlloc(3, entry->count * 0x28);
	entry->userdata = data;
	entry->callback = effBombMain;
	radius = 10.0f * scale;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->counter = 0;
	data->texOffset = 0;
	work = data + 1;
	for (i = 1; i < entry->count; i++, work++) {
		work->angle = 90.0f * (f32)(i - 1) + ((i > 4) ? 45.0f : 0.0f);
		work->position.x = radius * (f32)sin(0.017453292f * work->angle);
		work->position.y = radius * (f32)cos(0.017453292f * work->angle);
		work->position.z = 0.0f;
		work->scaleX = 0.0f;
		work->scaleY = 0.0f;
		work->texOffset = 0;
		work->counter = 0;
	}
	effConfettiEntry(4, 0x28, x, y, z);
	return entry;
}
static void effBombMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	Vec position;
#endif
	effBombData* data = effect->userdata;
	Vec disp = { 0 };
	s32 i;
	s32 counter;
	s32 idx;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif
	counter = data->counter;
	data->counter = counter + 1;
	if (data->counter > 0x24) {
		effDelete(effect);
		return;
	}
	idx = counter * 3;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (i < 5) {
			if ((u32)counter < 0x36) {
				data->texOffset = scale_dt[idx];
				data->scaleX = (f32)scale_dt[idx + 1] / 100.0f;
				data->scaleY = (f32)scale_dt[idx + 2] / 100.0f;
			}
		}
		else {
			if ((u32)counter < 0x36) {
				data->texOffset = scale_dt2[idx];
				data->scaleX = (f32)scale_dt2[idx + 1] / 100.0f;
				data->scaleY = (f32)scale_dt2[idx + 2] / 100.0f;
			}
		}
	}
#ifdef MUST_MATCH
	dispEntry(4, dispCalcZ(&position), 2, effBombDisp, effect);
#else
	dispEntry(4, dispCalcZ(&disp), 2, effBombDisp, effect);
#endif
}

#pragma opt_loop_invariants off // needed: without it effBombDisp 99.71%
static void effBombDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effBombData* data;
	CameraEntry* camera = camGetPtr(camId);
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx view;
	Mtx work;
	GXTexObj tex;
	s32 i;
	f32 radius;
	f32 size;
	f32 one;
	f32 negHalf;
	f32 negOne;
	f32 half;

	data = effect->userdata;
	radius = data->scale;
	effGetTexObj(0x24, &tex);
	GXLoadTexObj(&tex, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetChanMatColor(4, dat_8041e450);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 1);
	GXSetTevAlphaOp(0, 0xE, 0, 0, 1, 1);
	GXSetTevColorIn(0, 0xF, 0xA, 8, 0xF);
	GXSetTevAlphaIn(0, 4, 6, 5, 7);
	GXSetTevKAlphaSel(0, 4);
	GXSetTevOrder(1, 1, 0, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0xE, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 0xF, 0xF, 0xF);
	GXSetTevAlphaIn(1, 4, 6, 2, 7);
	GXSetTevKAlphaSel(1, 4);
	GXSetTevColor(2, dat_8041e454);
	GXSetTevOrder(2, 0xFF, 0xFF, 0xFF);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0, 2, 3, 0xF);
	GXSetTevAlphaIn(2, 0, 7, 1, 1);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(mtx, rot, view);
	PSMTXConcat(camera->view, view, view);
	GXSetCurrentMtx(0);
	size = 30.0f * radius;
	one = 1.0f;
	negOne = -one;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(mtx, 0.0625f * (f32)data->texOffset, 0.0f, 0.0f);
		PSMTXScale(scale, 0.0625f, 1.0f, 1.0f);
		PSMTXConcat(mtx, scale, work);
		GXLoadTexMtxImm(work, 0x1E, 1);
		PSMTXTrans(mtx, -0.05f, -0.05f, 0.0f);
		PSMTXConcat(work, mtx, work);
		GXLoadTexMtxImm(work, 0x21, 1);
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(rot, 'z', 0.017453292f * -data->angle);
		PSMTXScale(scale, size * data->scaleX, size * data->scaleY, size);
		PSMTXConcat(mtx, rot, work);
		PSMTXConcat(work, scale, work);
		PSMTXConcat(view, work, work);
		GXLoadPosMtxImm(work, 0);
		half = 0.5f;
		negHalf = negOne * half;
		GXBegin(0x80, 0, 4);
		GXPosition3f32(negHalf, one, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(half, one, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(half, 0.0f, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(negHalf, 0.0f, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}
#pragma opt_loop_invariants reset
