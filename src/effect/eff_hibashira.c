#include "eff/eff_hibashira.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Builds the zero vector from an anonymous literal so the scheduler keeps the
// userdata load ahead of the copy (see docs/CONTEXT.md, Grind 19). The literal
// lands in .rodata at the position the named zero vector used to occupy.
static inline Vec makeHibashiraVec(f32 x, f32 y, f32 z) {
	Vec vec = (Vec){ 0.0f, 0.0f, 0.0f };
	vec.x = x;
	vec.y = y;
	vec.z = z;
	return vec;
}

// .rodata

// .sdata2 white GXColor with zero alpha; the alpha byte is overwritten per pillar segment.
extern const u32 dat_80422338;

inline GXColor makeHibashiraColor(s32 alpha) {
	GXColor color = *(GXColor*)&dat_80422338;
	color.a = alpha;
	return color;
}

EffectEntry* effHibashiraEntry(s32 type, s32 segmentCount, s32 lifetime, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale) {
	EffectEntry* entry;
	HibashiraData* data;
	HibashiraData* elem;
	s32 count;
	s32 i;
	f32 dx;
	f32 dy;
	f32 dz;

	entry = effEntry();

	count = 1;
	switch (type) {
	case 0:
	case 1:
	case 2:
		count = 2;
		break;
	case 3:
	case 4:
	case 5:
		count = segmentCount + 1;
		break;
	}

	entry->type = "hibashira";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(HibashiraData));
	entry->userdata = data;
	entry->callback = effHibashiraMain;
	entry->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->animPose = -1;
	data->field_0x18 = 0xFF;
	if (lifetime <= 0) {
		data->field_0x1C = 1000;
	} else {
		data->field_0x1C = lifetime;
	}
	data->field_0x20 = 0;

	dx = x2 - x;
	dy = y2 - y;
	dz = z2 - z;

	i = 1;
	elem = &data[1];
	for (; i < entry->count; i++, elem++) {
		elem->position.x = (f32)(i - 1) * (dx / (f32)(count - 1));
		elem->position.y = (f32)(i - 1) * (dy / (f32)(count - 1));
		elem->position.z = (f32)(i - 1) * (dz / (f32)(count - 1));
		{
			f32 arcScale = (f32)sin(3.1415927f * (f32)i / (f32)(entry->count - 1));
			elem->scale = 0.5f + arcScale;
		}
		elem->animPose = -1;
		elem->field_0x18 = 0xFF;
		elem->field_0x1C = 0;
		elem->field_0x20 = 0;
		elem->field_0x24 = (i - 1) * 4;
	}

	return entry;
}

void effHibashiraMain(EffectEntry* effect) {
	HibashiraData* data;
	HibashiraData* elem;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local
	   before dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp;
	s32 type;
	s32 inBattle;
	s32 i;

	data = effect->userdata;
	disp = makeHibashiraVec(data->position.x, data->position.y, data->position.z);
#ifdef MUST_MATCH
	position = disp;
#endif

	inBattle = gp->inBattle != 0;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x1C = 16;
	}

	switch (type) {
	case 0:
	case 1:
	case 3:
	case 4:
		if (!animGroupBaseAsync("EFF_hibashira", inBattle, NULL)) {
			return;
		}
		break;
	case 2:
	case 5:
		if (!animGroupBaseAsync("EFF_hibashira2", inBattle, NULL)) {
			return;
		}
		break;
	}

	if (data->field_0x1C < 1000) {
		data->field_0x1C--;
	}
	if (data->field_0x1C < 16) {
		data->field_0x18 = data->field_0x1C << 4;
	}
	if (data->field_0x1C < 0) {
		elem = data;
		for (i = 0; i < effect->count; i++) {
			if (elem->animPose != -1) {
				animPoseRelease(elem->animPose);
			}
			elem++;
		}
		effDelete(effect);
		return;
	}

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x24 != 0) {
			data->field_0x24--;
		} else {
			if (data->animPose == -1) {
				switch (type) {
				case 0:
				case 3:
					data->animPose = animPoseEntry("EFF_hibashira", inBattle);
					animPoseSetAnim(data->animPose, "A_1", TRUE);
					break;
				case 1:
				case 4:
					data->animPose = animPoseEntry("EFF_hibashira", inBattle);
					animPoseSetAnim(data->animPose, "A_2", TRUE);
					break;
				case 2:
				case 5:
					data->animPose = animPoseEntry("EFF_hibashira2", inBattle);
					animPoseSetAnim(data->animPose, "A_1", TRUE);
					break;
				}
			}
			data->field_0x20++;
			if (data->field_0x20 > 1000) {
				data->field_0x20 = 1000;
			}
			if (data->field_0x20 > 5) {
				data->field_0x18 -= 10;
			}
			if (data->field_0x18 < 0) {
				data->field_0x18 = 0;
			}
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effHibashiraDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effHibashiraDisp, effect);
#endif
}

void effHibashiraDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	HibashiraData* data = effect->userdata;
	Mtx spA0;
	Mtx sp70;
	Mtx sp40;
	Mtx sp10;
	s32 i;
	s32 alpha = data->field_0x18;

	PSMTXTrans(spA0, data->position.x, data->position.y, data->position.z);
	PSMTXScale(sp70, data->scale, data->scale, data->scale);
	PSMTXRotRad(sp40, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(spA0, sp40, spA0);
	PSMTXConcat(spA0, sp70, sp10);

	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		if (data->animPose != -1) {
			PSMTXTrans(spA0, data->position.x, data->position.y, data->position.z);
			PSMTXScale(sp70, data->scale, data->scale, data->scale);
			PSMTXConcat(spA0, sp70, spA0);
			PSMTXConcat(sp10, spA0, spA0);

			animPoseSetMaterialFlagOn(data->animPose, 0x40);
			animPoseSetMaterialEvtColor(data->animPose, makeHibashiraColor((data->field_0x18 * alpha) / 255));
			animPoseMain(data->animPose);
			animPoseDrawMtx(data->animPose, spA0, 1, 0.0f, 10.f);
			animPoseDrawMtx(data->animPose, spA0, 2, 0.0f, 10.f);
			animPoseDrawMtx(data->animPose, spA0, 3, 0.0f, 10.f);
		}
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_80422338 = 0xFFFFFF00;
