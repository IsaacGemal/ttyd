#include "eff/n64/eff_balloon_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <math.h>
#include <string.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_magic2.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effBalloonN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	effBalloonN64Data* data;

	effect = effEntry();
	effect->type = "BalloonN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBalloonN64Data));
	effect->userdata = data;
	effect->callback = effBalloonMain;
	effect->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x1C = 100;
	data->field_0x14 = a4;
	data->field_0x18 = 0.0f;
	data->field_0x20 = 0;
	return effect;
}

// The loop walks `segs` as an incrementing pointer and uses `wave * (f32)phase`.
// NONMATCHING (99.67%): only the preserved-float constants {30, 5, 2} are coloured
// rotated one step (target f27/f28/f29 = 30/5/2, ours = 2/30/5); the .sdata2 pool order matches.
void effBalloonMain(EffectEntry* effect) {
	s32 life;
	effBalloonN64Data* data = effect->userdata;
	BalloonSeg* segs = (BalloonSeg*)data->pad;
	Vec position = initializeVec((Vec*)&data->x);
	f32 radius;
	f32 angle;
	f32 wave;
	s32 i;
	s32 phase;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x1C = 99;
	}
	if (data->field_0x1C < 100) {
		data->field_0x1C--;
	}
	data->field_0x20++;
	if (data->field_0x20 > 324000) {
		data->field_0x20 = 256;
	}
	life = data->field_0x1C;
	if (life < 0) {
		effDelete(effect);
	} else {
		phase = data->field_0x20;
		data->field_0x18 += 0.1f * (data->field_0x14 - data->field_0x18);
		radius = data->field_0x18 +
		         0.1f * (data->field_0x18 * (f32)sin(6.2831855f * (f32)(phase * 3) / 360.0f));
		for (i = 0; i < 20; i++) {
			angle = 6.2831855f * (17.0f + 360.0f * (f32)i / 21.0f) / 360.0f;
			segs->x = radius * (f32)sin(angle);
			segs->y = -radius * (f32)cos(angle);
			wave = 0.1f * (f32)sin(6.2831855f * (f32)(i * 53) / 360.0f) + 2.0f;
			segs->x += 1.5f * (f32)sin(6.2831855f * (30.0f * (5.0f + (f32)i) +
			                                  wave * (f32)phase) / 360.0f);
			wave = 0.1f * (f32)cos(6.2831855f * (f32)(i * 36) / 360.0f) + 2.0f;
			segs->y += 1.5f * (f32)cos(6.2831855f * (50.0f * (5.0f + (f32)i) +
			                                  wave * (f32)phase) / 360.0f);
			segs++;
		}
		*(f32*)&data->pad[0xA0] = 0.0f;
		if (life < 100) {
			*(f32*)&data->pad[0xA4] += 1.0f;
		} else {
			*(f32*)&data->pad[0xA4] = 1.2f * radius;
		}
		dispEntry(4, dispCalcZ(&position), 2, effBalloonDisp, effect);
	}
}

// A 14-byte N64 vertex (matches effFlameVtx); the shared table is defined below in .data.
typedef struct BalloonVtx {
	s16 pos[3]; // 0x0, XYZ
	s16 tex[2]; // 0x6, ST
	u8 clr[4];  // 0xA, RGBA8
} BalloonVtx;    // 0xE

extern BalloonVtx balloon_vtx[];

// .sdata2 constants that belong to this unit (named in the original's symbol table).
extern const f32 float_0p0625_8041e940;
extern const f32 float_0_8041e944;
extern const f32 float_deg2rad_8041e948;

// Retail inlines a per-vertex helper; each inlining gets its own Vec stack slot and its
// own memcpy, exactly as in effFlameDisp's effFlameSetVtx.
static inline void effBalloonSetVtx(BalloonVtx* vtx, Mtx mtx, s32 idx, BalloonVtx* src) {
	Vec v;

	memcpy(&vtx[idx], src, sizeof(BalloonVtx));
	v.x = vtx[idx].pos[0];
	v.y = vtx[idx].pos[1];
	v.z = vtx[idx].pos[2];
	PSMTXMultVec(mtx, &v, &v);
	vtx[idx].pos[0] = v.x;
	vtx[idx].pos[1] = v.y;
	vtx[idx].pos[2] = v.z;
}

// The control flow, constants and GX calls match, but MWCC 2.6 lowers the s16<->f32 vertex
// conversion as lha/fctiwz instead of the original's psq_l/psq_st (GQR2-5 setup + psq pairs).
// No source form, pragma or compiler version here reproduces the psq path, so the f23-f31 saves,
// the 0x210 frame and the register colours differ. tri1 also takes the header's 3 u16 args, so
// the target's 4th argument and its lack of clrlwi need include/eff/eff_n64.h and
// src/effect/n64/eff_n64.c to declare 4 s32 args.
// NONMATCHING (60.06%): s16<->f32 conversion uses lha/fctiwz, not psq; tri1 takes 4 s32 args.
void effBalloonDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	CameraEntry* cam2;
	effBalloonN64Data* data;
	BalloonVtx* vtx;
	BalloonSeg* segs;
	GXTexObj texObj;
	Mtx texMtx;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx concatMtx;
	Mtx resultMtx;
	s32 life;
	s32 i;

	cam = camGetPtr(camId);
	data = effect->userdata;
	vtx = smartAlloc(0x1C0, 3)->address;
	life = data->field_0x1C;

	if (life == 0) {
		effGetTexObjN64(0x5F, &texObj);
		GXLoadTexObj(&texObj, 0);
	} else if (life == 1) {
		effGetTexObjN64(0x5E, &texObj);
		GXLoadTexObj(&texObj, 0);
	} else {
		effGetTexObjN64(0x5D, &texObj);
		GXLoadTexObj(&texObj, 0);
	}

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(texMtx, float_0p0625_8041e940, float_0p0625_8041e940, float_0_8041e944);
	GXLoadTexMtxImm(texMtx, 0x1E, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xC);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	effBalloonSetVtx(vtx, transMtx, 0, &balloon_vtx[1]);

	PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e948 * data->field_0x10);
	PSMTXConcat(transMtx, rotMtx, concatMtx);
	cam2 = camGetPtr(CAMERA_3D);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041e948 * -cam2->viewYaw);
	PSMTXTrans(transMtx, *(f32*)&data->pad[0xA0], *(f32*)&data->pad[0xA4], float_0_8041e944);
	PSMTXConcat(rotMtx, transMtx, resultMtx);
	PSMTXConcat(concatMtx, resultMtx, resultMtx);
	effBalloonSetVtx(vtx, resultMtx, 1, &balloon_vtx[0]);

	segs = (BalloonSeg*)data->pad;
	for (i = 0; i < 20; i++) {
		PSMTXTrans(transMtx, segs[i].x, segs[i].y, float_0_8041e944);
		PSMTXConcat(resultMtx, transMtx, transMtx);
		effBalloonSetVtx(vtx, transMtx, i + 2, &balloon_vtx[i + 2]);
	}

	DCFlushRange(vtx, 0x1C0);
	GXInvalidateVtxCache();
	GXLoadPosMtxImm(cam->view, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)vtx);

	if (life < 100) {
		GXBegin(0x90, 0, 3);
		tri1(0x15, 2, 1);
		for (i = 0; i < 19; i++) {
			GXBegin(0x90, 0, 3);
			tri1(1, i + 2, i + 3);
		}
	} else {
		GXBegin(0x90, 0, 6);
		tri1(0, 1, 2);
		tri1(0, 0x15, 1);
		for (i = 0; i < 19; i++) {
			GXBegin(0x90, 0, 3);
			tri1(1, i + 2, i + 3);
		}
	}
}
