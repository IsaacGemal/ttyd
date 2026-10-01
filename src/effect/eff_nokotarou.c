#include "eff/eff_nokotarou.h"
#include "driver/effdrv.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/types.h>

// .rodata string; the .sdata2 constants this unit owns, in the original's data order. float_100 and
// float_1 must be defined (not extern): MWCC colours the second pose's stretch FPRs differently when
// it knows their values, which is what makes effNokotarouDisp byte-exact.
// externs for the remaining constants the disp/main read from the original unit's data.
extern const u32 dat_80422480;
extern const u32 dat_80422484;

EffectEntry* effNokotarouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry = effEntry();
	NokotarouWork* work;

	entry->type = "Nokotarou";
	entry->count = 1;
	work = __memAlloc(HEAP_EFFECT, sizeof(NokotarouWork));
	entry->userdata = work;
	entry->callback = effNokotarouMain;
	entry->flags |= 2;

	work->type = type;
	work->x = x;
	work->y = y;
	work->z = z;
	work->x2 = work->x;
	work->y2 = work->y;
	work->z2 = work->z;
	work->scale = scale;
	work->scaleAnim = 0.0f;
	work->alpha = 0xFF;
	if (duration <= 0) {
		work->lifetime = 1000;
	} else {
		work->lifetime = duration;
	}
	work->animPose = -1;
	work->animPose2 = -1;
	return entry;
}

void effNokotarouMain(EffectEntry* effect) {
	BOOL inBattle;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local
	   before dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	NokotarouWork* work = effect->userdata;
	Vec disp = { 0 };
	f32 t;
	f32 k;

	disp.x = work->x2;
	disp.y = work->y2;
	disp.z = work->z2;
#ifdef MUST_MATCH
	position = disp;
#endif

	inBattle = gp->inBattle != 0;

	if (!animGroupBaseAsync("MOBJ_EFF_nokotarou3", inBattle, NULL)) {
		return;
	}
	if (work->animPose == -1) {
		work->animPose = animPoseEntry("MOBJ_EFF_nokotarou3", inBattle);
		animPoseSetAnim(work->animPose, "S_1", TRUE);
		work->animPose2 = animPoseEntry("MOBJ_EFF_nokotarou3", inBattle);
		animPoseSetAnim(work->animPose2, "A_1", TRUE);
	}
	if (effect->flags & 4) {
		effect->flags &= ~4;
		work->lifetime = 0x20;
	}
	if (work->lifetime < 1000) {
		work->lifetime--;
	}
	if (work->lifetime < 0) {
		if (work->animPose != -1) {
			animPoseRelease(work->animPose);
		}
		if (work->animPose2 != -1) {
			animPoseRelease(work->animPose2);
		}
		effDelete(effect);
	} else {
		if (work->lifetime < 0x20) {
			work->alpha = work->lifetime << 3;
			k = 0.03125f;
			t = -work->scaleAnim;
			work->scaleAnim += t * k;
		} else {
			k = 0.03125f;
			t = 1.0f - work->scaleAnim;
			work->scaleAnim += t * k;
		}
#ifdef MUST_MATCH
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effNokotarouDisp, effect);
#else
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effNokotarouDisp, effect);
#endif
	}
}

static inline void nokotarouSetColorMain(NokotarouWork* work) {
	GXColor color = *(GXColor*)&dat_80422480;
	color.a = work->alpha;
	animPoseSetMaterialEvtColor(work->animPose, color);
}

static inline void nokotarouSetColorSub(NokotarouWork* work) {
	GXColor color = *(GXColor*)&dat_80422484;
	color.a = work->alpha;
	animPoseSetMaterialEvtColor(work->animPose2, color);
}

void effNokotarouDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	NokotarouWork* work = effect->userdata;
	Mtx mtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	f32 scale;

	// The original calls camGetPtr once here and discards the result before the first pose test.
	camGetPtr(cameraId);
	scale = work->scaleAnim * work->scale;
	if (work->animPose == -1) {
		return;
	}
	PSMTXTrans(mtx, work->x2, work->y2, work->z2);
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(cameraId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	animPoseSetMaterialFlagOn(work->animPose, 0x40);
	nokotarouSetColorMain(work);
	animPoseMain(work->animPose);
	animPoseDrawMtx(work->animPose, mtx, 1, 0.0f, 2.0f);
	animPoseDrawMtx(work->animPose, mtx, 2, 0.0f, 2.0f);
	animPoseDrawMtx(work->animPose, mtx, 3, 0.0f, 2.0f);
	if (work->animPose2 != -1) {
		f32 sx = (work->x2 - work->x) / 100.f;
		f32 sy = (1.0f + (work->y2 - work->y) / 100.f);
		f32 sz = (1.0f + (work->z2 - work->z) / 100.f);

		sx = sx * work->scale;
		sy = sy * work->scale;
		sz = sz * work->scale;
		PSMTXTrans(mtx, work->x2, work->y2, work->z2);
		PSMTXScale(scaleMtx, sx, sy, sz);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(cameraId)->viewYaw * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(work->animPose2, 0x40);
		nokotarouSetColorSub(work);
		animPoseMain(work->animPose2);
		animPoseDrawMtx(work->animPose2, mtx, 1, 0.0f, 2.0f);
		animPoseDrawMtx(work->animPose2, mtx, 2, 0.0f, 2.0f);
		animPoseDrawMtx(work->animPose2, mtx, 3, 0.0f, 2.0f);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_80422480 = 0xFFFFFF00;
const u32 dat_80422484 = 0xFFFFFF00;
