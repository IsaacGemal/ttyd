#include "eff/n64/eff_balloon_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "matching.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#ifdef MUST_MATCH
#include <dolphin/os/OSFastCast.h>
#endif
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

// The loop walks `segs` as an incrementing pointer.
// MUST_MATCH: holding the raw sin/cos in `wave` and fusing `0.1f * wave + 2.0f` at the
// use site (assigned back into `wave`) is what makes MWCC assign the preserved constants
// to f27/f28/f29 = 30/5/2 and emit the in-place `fmadds f0, f21, f0, f29`. The natural
// fused-wave form under #else is the same math but colors them 2/30/5 (99.67%).
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
#ifdef MUST_MATCH
			wave = (f32)sin(6.2831855f * (f32)(i * 53) / 360.0f);
			segs->x += (f32)sin(6.2831855f * (30.0f * (5.0f + (f32)i) +
			                                  (wave = 0.1f * wave + 2.0f) * (f32)phase) / 360.0f) * 1.5f;
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
// own memcpy, exactly as in effFlameDisp's effFlameSetVtx. Retail re-runs OSInitFastCast
// before the s16->f32 reads and before the f32->s16 writes, so the GQR2-5 setup matches;
// MWCC 2.6 still lowers the casts to the integer path (see eff_flame_n64.c).
#ifdef MUST_MATCH
// Retail's paired-single path reserves a second 12-byte Vec slot per inlining, giving the
// original's 0x48 low scratch and putting texObj/matrices at their real stack offsets.
#define BALLOON_VTX_VECS 2
#else
#define BALLOON_VTX_VECS 1
#endif
static inline void effBalloonSetVtx(BalloonVtx* vtx, Mtx mtx, s32 idx, BalloonVtx* src) {
	Vec v[BALLOON_VTX_VECS];

	memcpy(&vtx[idx], src, sizeof(BalloonVtx));
#ifdef MUST_MATCH
	OSInitFastCast();
#endif
	v[0].x = vtx[idx].pos[0];
	v[0].y = vtx[idx].pos[1];
	v[0].z = vtx[idx].pos[2];
	PSMTXMultVec(mtx, &v[0], &v[0]);
#ifdef MUST_MATCH
	OSInitFastCast();
#endif
	vtx[idx].pos[0] = v[0].x;
	vtx[idx].pos[1] = v[0].y;
	vtx[idx].pos[2] = v[0].z;
}

// The retail tri1 keeps a 4th, unused argument (eff_balloon_n64.s passes 0 for the fixed
// triangles and 1 for the loop ones). The header declares only three u16 parameters; the
// cast to four s32 parameters also keeps the caller from truncating i+2/i+3 with clrlwi.
#ifdef MUST_MATCH
static inline void tri1WithUnused(s32 x, s32 y, s32 z, s32 unused) {
	((void (*)(s32, s32, s32, s32))tri1)(x, y, z, unused);
}
#endif

// The control flow, constants, GX/TEV state, matrix math and the six OSInitFastCast GQR2-5
// blocks match; tri1WithUnused reproduces the original's 4th (unused) tri1 argument and its
// lack of a clrlwi on i+2/i+3; and with the Vec[2] padding and transMtx/texMtx declaration
// order, texObj (0x50) and all six matrices (0x70-0x160) land at retail's offsets.
// The remaining gap is the s16<->f32 vertex conversion: retail loads/stores each position
// component with psq_l/psq_st (GQR5, s16), while MWCC 2.6 lowers the same `v = vtx->pos[n]`
// / `vtx->pos[n] = v` source to the lha/xoris/lfd+fsubs and fctiwz integer path. That leaves
// retail with f23-f31 live (frame 0x210) where we keep only f31 (frame 0x1f0), so the save
// area, the low Vec slots (retail 0x2c/0x38/0x44 vs ours 0x8/0x20/0x38) and every register
// colour differ. No source form, pragma or GC compiler version here emits the psq path (see
// eff_flame_n64.c / eff_opuku_wcurtain_n64.c), so this cannot reach 100% with the current
// toolchain. tri1's 4-argument form is not in include/eff/eff_n64.h or src/effect/n64/eff_n64.c
// (both declare 3 u16 args); the local tri1WithUnused helper casts around that.
// NONMATCHING (67.72%): s16<->f32 conversion uses lha/fctiwz, not retail's psq_l/psq_st.
void effBalloonDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	CameraEntry* cam2;
	effBalloonN64Data* data;
	BalloonVtx* vtx;
	BalloonSeg* segs;
	GXTexObj texObj;
	// Declaration order is transMtx before texMtx so MWCC lays the two out at retail's
	// offsets (texMtx 0x100, transMtx 0x130); the usage order below is unchanged.
	Mtx transMtx;
	Mtx texMtx;
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
	PSMTXRotRad(rotMtx, 'y', -cam2->viewYaw * float_deg2rad_8041e948);
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
#ifdef MUST_MATCH
		tri1WithUnused(0x15, 2, 1, 0);
#else
		tri1(0x15, 2, 1);
#endif
		for (i = 0; i < 19; i++) {
			GXBegin(0x90, 0, 3);
#ifdef MUST_MATCH
			tri1WithUnused(1, i + 2, i + 3, 1);
#else
			tri1(1, i + 2, i + 3);
#endif
		}
	} else {
		GXBegin(0x90, 0, 6);
#ifdef MUST_MATCH
		tri1WithUnused(0, 1, 2, 0);
		tri1WithUnused(0, 0x15, 1, 0);
#else
		tri1(0, 1, 2);
		tri1(0, 0x15, 1);
#endif
		for (i = 0; i < 19; i++) {
			GXBegin(0x90, 0, 3);
#ifdef MUST_MATCH
			tri1WithUnused(1, i + 2, i + 3, 1);
#else
			tri1(1, i + 2, i + 3);
#endif
		}
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p0625_8041e940 = 0.0625f;
const f32 float_0_8041e944 = 0.0f;
const f32 float_deg2rad_8041e948 = 0.017453292f;
