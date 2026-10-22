#include "eff/n64/eff_balloon_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "matching.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSFastCast.h>
#include "math_ext.h"
#include <string.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_magic2.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}
EffectEntry* effBalloonN64Entry(s32 type, f32 x, f32 y, f32 z, f32 targetRadius, f32 angleDeg) {
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
	data->field_0x10 = angleDeg;
	data->field_0x1C = 100;
	data->field_0x14 = targetRadius;
	data->field_0x18 = 0.0f;
	data->field_0x20 = 0;
	return effect;
}

// The loop walks `segs` as an incrementing pointer.
// MUST_MATCH: holding the raw sin/cos in `wave` and fusing `0.1f * wave + 2.0f` at the
// use site (assigned back into `wave`) is what makes MWCC assign the preserved constants
// to f27/f28/f29 = 30/5/2 and emit the in-place `fmadds f0, f21, f0, f29`. The natural
// fused-wave form under #else is the same math but colors them 2/30/5 (99.67%).
void effBalloonMain(EffectEntry* effect) {
	s32 life;
	effBalloonN64Data* data = effect->userdata;
	BalloonSeg* segs = data->field_0x24;
	Vec position = initializeVec((Vec*)&data->x);
	f32 radius;
	f32 angle;
	f32 wave;
	// MUST_MATCH: the sin call in its own statement (result in `sinOffset`) makes MWCC pool
	// 30/5/2 before 1.5, the original's .sdata2 order; the fused form pools 1.5 first.
	f32 sinOffset;
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
#ifdef MUST_MATCH
			wave = (f32)sin(6.2831855f * (f32)(i * 53) / 360.0f);
			sinOffset = (f32)sin(6.2831855f * (30.0f * (5.0f + (f32)i) +
			                                  (wave = 0.1f * wave + 2.0f) * (f32)phase) / 360.0f);
			segs->x += sinOffset * 1.5f;
#else
			wave = 0.1f * (f32)sin(6.2831855f * (f32)(i * 53) / 360.0f) + 2.0f;
			segs->x += 1.5f * (f32)sin(6.2831855f * (30.0f * (5.0f + (f32)i) +
			                                  wave * (f32)phase) / 360.0f);
#endif
#ifdef MUST_MATCH
			wave = (f32)cos(6.2831855f * (f32)(i * 36) / 360.0f);
			segs->y += (f32)cos(6.2831855f * (50.0f * (5.0f + (f32)i) +
			                                  (wave = 0.1f * wave + 2.0f) * (f32)phase) / 360.0f) * 1.5f;
#else
			wave = 0.1f * (f32)cos(6.2831855f * (f32)(i * 36) / 360.0f) + 2.0f;
			segs->y += 1.5f * (f32)cos(6.2831855f * (50.0f * (5.0f + (f32)i) +
			                                  wave * (f32)phase) / 360.0f);
#endif
			segs++;
		}
		data->field_0x24[20].x = 0.0f;
		if (life < 100) {
			data->field_0x24[20].y += 1.0f;
		} else {
			data->field_0x24[20].y = 1.2f * radius;
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

// The shared vertex template from the original's .data (22 x 14 bytes).
BalloonVtx balloon_vtx[22] = {
	{ { 0, 0, 0 }, { 0, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 480 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 160, 512 }, { 0, 0, 0, 255 } },
	{ { 0, 0, 0 }, { 0, 512 }, { 0, 0, 0, 255 } },
};

// Retail inlines a per-vertex helper; each inlining gets its own Vec stack slot and its
// own memcpy, exactly as in effFlameDisp's effFlameSetVtx. The s16<->f32 conversions use the
// SDK's OSs16tof32/OSf32tos16 fast-cast inlines (libs/dolsdk2004/include/dolphin/os/
// OSFastCast.h), which MWCC 2.6 lowers to retail's psq_l/psq_st GQR5 path; plain casts lower
// to the lha/xoris/lfd+fsubs and fctiwz integer path instead. OSInitFastCast re-runs before
// the reads and before the writes, matching retail's GQR2-5 setup.
static inline void effBalloonSetVtx(BalloonVtx* dst, BalloonVtx* src, Mtx mtx) {
	Vec v;
#define BALLOON_VEC (&v)

	memcpy(dst, src, sizeof(BalloonVtx));
	OSInitFastCast(); /* retail's GQR2-5 setup before the s16->f32 reads */
	OSs16tof32(&dst->pos[0], &BALLOON_VEC->x);
	OSs16tof32(&dst->pos[1], &BALLOON_VEC->y);
	OSs16tof32(&dst->pos[2], &BALLOON_VEC->z);
	PSMTXMultVec(mtx, BALLOON_VEC, BALLOON_VEC);
	OSInitFastCast(); /* retail's GQR2-5 setup before the f32->s16 writes */
	// The original holds the memcpy destination in a caller register and stores pos[0]
	// through it; passing &dst->pos[0] inline re-derives the address from the loop pointer.
	{ s16* out = &dst->pos[0]; OSf32tos16(&BALLOON_VEC->x, out); }
	OSf32tos16(&BALLOON_VEC->y, &dst->pos[1]);
	OSf32tos16(&BALLOON_VEC->z, &dst->pos[2]);
#undef BALLOON_VEC
}

// The control flow, constants, GX/TEV state, matrix math and the six OSInitFastCast GQR2-5
// blocks are the original's; the psq_l/psq_st vertex conversions come from the SDK's
// OSs16tof32/OSf32tos16 fast-cast inlines. The declaration order and the opt_dead_assignments
// pragma below reproduce retail's callee-saved colouring (cam r28, life r31, balloon_vtx base r29).
#pragma opt_dead_assignments off // needed: without it MWCC colours cam r31 / life r29, not retail's cam r28 / life r31
void effBalloonDisp(CameraId camId, void* param) {
	// The Mtx block leads and `life` precedes `cam`; with the pragma above that makes MWCC
	// colour life r31 and cam r28 as the original does. transMtx stays before texMtx so the
	// two frame slots land at retail's 0x130/0x100.
	Mtx transMtx;
	Mtx texMtx;
	Mtx rotMtx;
	Mtx concatMtx;
	Mtx resultMtx;
	EffectEntry* effect = param;
	s32 life;
	CameraEntry* cam;
	effBalloonN64Data* data;
	GXTexObj texObj;
	BalloonVtx* vtx;
	s32 i;

	cam = camGetPtr(camId);
	data = effect->userdata;
	vtx = smartAlloc(0x1C0, 3)->address;
	life = data->field_0x1C;

	if (life == 0) {
		effGetTexObjN64(0x5F, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	} else if (life == 1) {
		effGetTexObjN64(0x5E, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	} else {
		effGetTexObjN64(0x5D, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(texMtx, 0.0625f, 0.0625f, 0.0f);
	GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	effBalloonSetVtx(vtx, &balloon_vtx[1], transMtx);

	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->field_0x10);
	PSMTXConcat(transMtx, rotMtx, concatMtx);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXTrans(transMtx, data->field_0x24[20].x, data->field_0x24[20].y, 0.0f);
	PSMTXConcat(rotMtx, transMtx, resultMtx);
	PSMTXConcat(concatMtx, resultMtx, resultMtx);
	effBalloonSetVtx(vtx + 1, &balloon_vtx[0], resultMtx);

	for (i = 0; i < 20; i++) {
		PSMTXTrans(transMtx, data->field_0x24[i].x, data->field_0x24[i].y,
		           0.0f);
		PSMTXConcat(resultMtx, transMtx, transMtx);
		effBalloonSetVtx(&vtx[i + 2], &balloon_vtx[i + 2], transMtx);
	}

	DCFlushRange(vtx, 0x1C0);
	GXInvalidateVtxCache();
	GXLoadPosMtxImm(cam->view, 0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64(vtx);

	if (life < 100) {
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
		tri1(0x15, 2, 1, 0);
		for (i = 0; i < 19; i++) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
			tri1(1, i + 2, i + 3, 1);
		}
	} else {
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri1(0, 1, 2, 0);
		tri1(0, 0x15, 1, 0);
		for (i = 0; i < 19; i++) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
			tri1(1, i + 2, i + 3, 1);
		}
	}
}
#pragma opt_dead_assignments reset

