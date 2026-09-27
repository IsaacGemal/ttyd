#include "eff/eff_irekaeru.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

typedef struct EffIrekaeruData {
	s32 type;      // 0x0
	f32 x;         // 0x4
	f32 y;         // 0x8
	f32 z;         // 0xC
	f32 scale;     // 0x10
	s32 poseId;    // 0x14
	s32 state;     // 0x18
	s32 field_0x1C; // 0x1C
	s32 alpha;     // 0x20
} EffIrekaeruData;

extern const u32 dat_80421c00;
extern const f32 float_deg2rad_80421c04;
extern const f32 float_0_80421c08;
extern const f32 float_2_80421c0c;
extern const f32 float_1_80421c14;
extern const f32 float_0p75_80421c18;

const Vec vec3_80300198 = {0.0f, 0.0f, 0.0f};
const char str_803001A4[] = "EFF_Irekae";
const char str_803001B0[] = "Irekaeru";
const char str_80421C10[] = "Z_1";

// NONMATCHING (99.93%): only the GXColor stack slots differ. The target builds the color in a temp at r1+8,
// copies it to r1+0xc and passes r1+0xc; mwcc builds at r1+0xc, copies to r1+8 and passes r1+8. Every
// instruction matches otherwise.
void effIrekaeruDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffIrekaeruData* userdata = effect->userdata;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXColor color;

	if (userdata->poseId != -1) {
		PSMTXTrans(mtx, userdata->x, userdata->y, userdata->z);
		PSMTXScale(scaleMtx, userdata->scale, userdata->scale, userdata->scale);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80421c04);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(userdata->poseId, 0x40);
		color = *(GXColor*)&dat_80421c00;
		color.a = userdata->alpha;
		animPoseSetMaterialEvtColor(userdata->poseId, color);
		animPoseMain(userdata->poseId);
		animPoseDrawMtx(userdata->poseId, mtx, 1, float_0_80421c08, float_2_80421c0c);
		animPoseDrawMtx(userdata->poseId, mtx, 2, float_0_80421c08, float_2_80421c0c);
		animPoseDrawMtx(userdata->poseId, mtx, 3, float_0_80421c08, float_2_80421c0c);
	}
}

void effIrekaeruMain(EffectEntry* effect) {
	EffIrekaeruData* userdata = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 inBattle;

	pos = (Vec){0.0f, 0.0f, 0.0f};
	pos.x = userdata->x;
	pos.y = userdata->y;
	pos.z = userdata->z;
	pos2 = pos;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (userdata->poseId != -1) {
			animPoseRelease(userdata->poseId);
		}
		effDelete(effect);
		return;
	}

	switch (userdata->state) {
		case 0:
			if (!animGroupBaseAsync(str_803001A4, inBattle, NULL)) {
				return;
			}
			userdata->poseId = animPoseEntry(str_803001A4, inBattle);
			animPoseSetAnim(userdata->poseId, str_80421C10, TRUE);
			userdata->state++;
			break;
		case 1:
			if (animPoseGetLoopTimes(userdata->poseId) > float_1_80421c14) {
				animPoseRelease(userdata->poseId);
				effDelete(effect);
				return;
			}
			break;
	}

	if (animPoseGetLoopTimes(userdata->poseId) < float_0p75_80421c18) {
		userdata->alpha += 20;
		if (userdata->alpha > 0xFF) {
			userdata->alpha = 0xFF;
		}
	} else {
		userdata->alpha -= 20;
		if (userdata->alpha < 0) {
			userdata->alpha = 0;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effIrekaeruDisp, effect);
}

EffectEntry* effIrekaeruEntry(s32 type, f32 x, f32 y, f32 z) {
	EffIrekaeruData* userdata;
	EffectEntry* effect;

	effect = effEntry();
	effect->type = str_803001B0;
	effect->count = 1;
	userdata = __memAlloc(HEAP_EFFECT, sizeof(EffIrekaeruData) * effect->count);
	effect->userdata = userdata;
	effect->callback = effIrekaeruMain;
	effect->flags |= 2;

	userdata->type = type;
	userdata->x = x;
	userdata->y = y;
	userdata->z = z;
	userdata->scale = float_1_80421c14;
	userdata->poseId = -1;
	userdata->state = 0;
	userdata->field_0x1C = 0;
	userdata->alpha = 0;

	return effect;
}
