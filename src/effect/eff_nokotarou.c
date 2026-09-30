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
const char str_803014B8[] = "Nokotarou";
const f32 float_0_8042248c = 0.0f;
const f32 float_100_80422494 = 100.0f;
const f32 float_1_80422498 = 1.0f;
// externs for the remaining constants the disp/main read from the original unit's data.
extern const char str_803014A4[];
extern const char str_8042249C[4];
extern const char str_804224A0[4];
extern const u32 dat_80422480;
extern const u32 dat_80422484;
extern const f32 float_deg2rad_80422488;
extern const f32 float_2_80422490;
extern const f32 float_0p03125_804224a4;

EffectEntry* effNokotarouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry = effEntry();
	NokotarouWork* work;

	entry->type = str_803014B8;
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
	work->scaleAnim = float_0_8042248c;
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

	disp.x = work->x2;
	disp.y = work->y2;
	disp.z = work->z2;
#ifdef MUST_MATCH
	position = disp;
#endif

	inBattle = gp->inBattle != 0;

	if (!animGroupBaseAsync(str_803014A4, inBattle, NULL)) {
		return;
	}
	if (work->animPose == -1) {
		work->animPose = animPoseEntry(str_803014A4, inBattle);
		animPoseSetAnim(work->animPose, str_8042249C, TRUE);
		work->animPose2 = animPoseEntry(str_803014A4, inBattle);
		animPoseSetAnim(work->animPose2, str_804224A0, TRUE);
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
			t = -work->scaleAnim;
			work->scaleAnim += t * float_0p03125_804224a4;
		} else {
			t = float_1_80422498 - work->scaleAnim;
			work->scaleAnim += t * float_0p03125_804224a4;
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
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_80422488);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	animPoseSetMaterialFlagOn(work->animPose, 0x40);
	nokotarouSetColorMain(work);
	animPoseMain(work->animPose);
	animPoseDrawMtx(work->animPose, mtx, 1, float_0_8042248c, float_2_80422490);
	animPoseDrawMtx(work->animPose, mtx, 2, float_0_8042248c, float_2_80422490);
	animPoseDrawMtx(work->animPose, mtx, 3, float_0_8042248c, float_2_80422490);
	if (work->animPose2 != -1) {
		f32 sx = (work->x2 - work->x) / float_100_80422494;
		f32 sy = (float_1_80422498 + (work->y2 - work->y) / float_100_80422494);
		f32 sz = (float_1_80422498 + (work->z2 - work->z) / float_100_80422494);

		sx = sx * work->scale;
		sy = sy * work->scale;
		sz = sz * work->scale;
		PSMTXTrans(mtx, work->x2, work->y2, work->z2);
		PSMTXScale(scaleMtx, sx, sy, sz);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_80422488);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(work->animPose2, 0x40);
		nokotarouSetColorSub(work);
		animPoseMain(work->animPose2);
		animPoseDrawMtx(work->animPose2, mtx, 1, float_0_8042248c, float_2_80422490);
		animPoseDrawMtx(work->animPose2, mtx, 2, float_0_8042248c, float_2_80422490);
		animPoseDrawMtx(work->animPose2, mtx, 3, float_0_8042248c, float_2_80422490);
	}
}
