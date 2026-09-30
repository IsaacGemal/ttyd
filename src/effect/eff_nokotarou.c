#include "eff/eff_nokotarou.h"
#include "driver/effdrv.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/types.h>

// .rodata string and .sdata2 constant, declared in the original unit's data order.
const char str_803014B8[] = "Nokotarou";
const f32 float_0_8042248c = 0.0f;
// externs for the remaining constants effNokotarouMain reads from the original unit's data.
extern const char str_803014A4[];
extern const char str_8042249C[4];
extern const char str_804224A0[4];
extern const f32 float_1_80422498;
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
