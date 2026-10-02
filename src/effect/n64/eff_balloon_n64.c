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
// own memcpy, exactly as in effFlameDisp's effFlameSetVtx. The s16<->f32 conversions use the
// SDK's OSs16tof32/OSf32tos16 fast-cast inlines (libs/dolsdk2004/include/dolphin/os/
// OSFastCast.h), which MWCC 2.6 lowers to retail's psq_l/psq_st GQR5 path; plain casts lower
// to the lha/xoris/lfd+fsubs and fctiwz integer path instead. OSInitFastCast re-runs before
// the reads and before the writes, matching retail's GQR2-5 setup.
static inline void effBalloonSetVtx(BalloonVtx* dst, BalloonVtx* src, Mtx mtx) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail's __OSf32tos16 temporaries each take a 4-byte slot, making the
	   helper object 16 bytes; the extra f32 reproduces that frame size. Natural Vec under #else. */
	f32 v[4];
#define BALLOON_VEC ((Vec*)v)
#else
	Vec v;
#define BALLOON_VEC (&v)
#endif

	memcpy(dst, src, sizeof(BalloonVtx));
	OSInitFastCast(); /* retail's GQR2-5 setup before the s16->f32 reads */
	OSs16tof32(&dst->pos[0], &BALLOON_VEC->x);
	OSs16tof32(&dst->pos[1], &BALLOON_VEC->y);
	OSs16tof32(&dst->pos[2], &BALLOON_VEC->z);
	PSMTXMultVec(mtx, BALLOON_VEC, BALLOON_VEC);
	OSInitFastCast(); /* retail's GQR2-5 setup before the f32->s16 writes */
	OSf32tos16(&BALLOON_VEC->x, &dst->pos[0]);
	OSf32tos16(&BALLOON_VEC->y, &dst->pos[1]);
	OSf32tos16(&BALLOON_VEC->z, &dst->pos[2]);
#undef BALLOON_VEC
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
// blocks match; the psq_l/psq_st vertex conversions match too (via the SDK OSs16tof32/
// OSf32tos16 fast-cast inlines); tri1WithUnused reproduces the original's 4th (unused) tri1
// argument and its lack of a clrlwi on i+2/i+3.
// The remaining gap is stack layout and register colouring, caused by the SDK header: the
// reconstructed `__OSf32tos16` in libs/dolsdk2004/include/dolphin/os/OSFastCast.h declares its
// temporary as `s16 tmp`, which MWCC allocates 2 bytes apart across calls (frame 0x200). The
// retail header used a 4-byte temporary: changing that one line to `f32 tmp` (plus `f32* ptr
// = &tmp; return *(s16*)&tmp;`) makes the temps land 4 apart, the frame 0x210 and every
// Vec/matrix/texObj offset exact. That one-line header fix is NOT committed here (this file may
// only add prototypes to headers); with it this source reaches 99.89% and only the callee-saved
// colouring remains (retail cam r28 vs ours r31, life r31 vs r29, balloon_vtx base r29 vs r28).
// The MUST_MATCH `f32 v[4]` in effBalloonSetVtx pads each inlined helper object to 16 bytes to
// recover the 0x210 frame without the header fix (99.63% -> 99.73%); it also misaligns the
// Vector slots because a 16-byte object spaces them 16 apart instead of retail's 12.
// tri1's 4-argument form is not in include/eff/eff_n64.h or src/effect/n64/eff_n64.c (both
// declare 3 u16 args); the local tri1WithUnused helper casts around that.
// NONMATCHING (99.73%): everything matches except the __OSf32tos16 temp width (s16 2-byte slots
// here vs 4-byte in retail; see the header note above) and the callee-saved register colouring
// that follows from it.
void effBalloonDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	effBalloonN64Data* data;
	GXTexObj texObj;
	// Declaration order is transMtx before texMtx so MWCC lays the two out at retail's
	// offsets (texMtx 0x100, transMtx 0x130); the usage order below is unchanged.
	BalloonVtx* vtx;
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
	effBalloonSetVtx(vtx, &balloon_vtx[1], transMtx);

	PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e948 * data->field_0x10);
	PSMTXConcat(transMtx, rotMtx, concatMtx);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e948);
	PSMTXTrans(transMtx, *(f32*)&data->pad[0xA0], *(f32*)&data->pad[0xA4], float_0_8041e944);
	PSMTXConcat(rotMtx, transMtx, resultMtx);
	PSMTXConcat(concatMtx, resultMtx, resultMtx);
	effBalloonSetVtx(vtx + 1, &balloon_vtx[0], resultMtx);

	for (i = 0; i < 20; i++) {
		PSMTXTrans(transMtx, *(f32*)(data->pad + i * 8), *(f32*)(data->pad + i * 8 + 4),
		           float_0_8041e944);
		PSMTXConcat(resultMtx, transMtx, transMtx);
		effBalloonSetVtx(&vtx[i + 2], &balloon_vtx[i + 2], transMtx);
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
