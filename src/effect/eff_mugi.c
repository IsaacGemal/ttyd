#include "eff/eff_mugi.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>
#include <string.h>

// .rodata: this zero Vec opens the unit's .rodata (no code here reads it); the two Main functions' Vec
// templates follow it, then the strings and double defined after effMugiKemuri2Main.
const Vec vec3_802f3498 = { 0.0f, 0.0f, 0.0f };
extern const char str_EFF_Hana[12];
extern const char str_EFF_Mugi[12];

// .sdata2 constants.
extern const f32 float_deg2rad_8041ca70;
extern const f32 float_0_8041ca74;

// 100%: initialises five smoke puffs. The original's callee-saved FPR band is
// f19-f27 for the pooled constants and f28-f31 for the four arguments; MWCC only
// colours it that way when the sin() result is held in a temp before the store
// (see the sinAngle temp below).
EffectEntry* effMugiKemuriEntry(f32 x, f32 y, f32 z, f32 baseAngle) {
	EffectEntry* entry = effEntry();
	MugiKemuriWork* work;
	f32 angle;
	f32 sinAngle;
	s32 i;

	entry->type = "MugiKemuri";
	entry->count = 5;
	work = __memAlloc(HEAP_EFFECT, sizeof(MugiKemuriWork) * 5);
	entry->userdata = work;
	memset(work, 0, sizeof(MugiKemuriWork) * 5);
	entry->callback = effMugiKemuriMain;
	entry->flags |= 2;

	for (i = 0; i < 5; i++, work++) {
		work->active = 1;
		work->scale = 0.29999998f;
		work->speed = float_0_8041ca74;
		work->field_0x38 = float_0_8041ca74;
		work->x = x;
		work->y = y;
		work->z = z;
		work->scaleX = 0.7f;
		work->scaleY = 0.7f;
		work->scaleZ = 0.7f;
		work->alpha = 0xFF;
		work->lifetime = 60;
		work->rotX = float_0_8041ca74;
		work->rotY = baseAngle + (f32)(i * 72);
		work->rotZ = float_0_8041ca74;
		work->time = 1.75f;
		work->timeAdd = -0.08f;
		work->timeVel = float_0_8041ca74;
		work->timeVelAdd = float_0_8041ca74;
		work->scaleSpeed = -3.9f;
		angle = reviseAngle(work->rotY);
		angle = 6.2831855f * angle / 360.0f;
		// Holding the sin() result in a temp first gives the original's
		// callee-saved FPR allocation (args f28-f31, constants f19-f27); storing it
		// directly makes MWCC rotate the whole band by four (args f24-f27).
		sinAngle = sin(angle);
		work->sinA = sinAngle;
		work->cosA = cos(angle);
		work->animPose = -1;
	}
	return entry;
}

// The ±10/±90 ternaries must be written `dirSign != 0 ? -N : N` (the branch takes the
// negative case) and the loop must advance `work` itself (`i++, work++` with `work->`),
// which keeps one web for the pointer; with `work[i]` the allocator splits it and swaps
// the entry/work callee-saved registers. The int->float magic still pools as a local
// @NNN@ rather than double_to_int_802f34f0; that symbol name does not affect the score.
EffectEntry* effMugiKemuri2Entry(s32 type, f32 x, f32 y, f32 z, f32 baseAngle, f32 dirSign) {
	EffectEntry* entry = effEntry();
	MugiKemuriWork* work;
	f32 angle;
	s32 i;

	entry->type = "MugiKemuri2";
	entry->count = 5;
	work = __memAlloc(HEAP_EFFECT, sizeof(MugiKemuriWork) * 5);
	entry->userdata = work;
	memset(work, 0, sizeof(MugiKemuriWork) * 5);
	entry->callback = effMugiKemuri2Main;
	entry->flags |= 2;

	for (i = 0; i < 5; i++, work++) {
		work->active = 1;
		work->type = type;
		work->field_0x38 = float_0_8041ca74;
		work->x = x;
		work->y = y;
		work->z = z;
		work->scaleX = 0.7f;
		work->scaleY = 0.7f;
		work->scaleZ = 0.7f;
		work->alpha = 0xFF;
		work->speed = 1.0f;
		work->scale = 5.4f;
		work->scaleSpeed = (f32)((dirSign != float_0_8041ca74) ? -10 : 10);
		work->time = 0.5f;
		work->timeAdd = -0.07f;
		work->timeVel = 0.003f;
		work->timeVelAdd = float_0_8041ca74;
		work->lifetime = 15;
		work->rotY = baseAngle;
		work->rotX = float_0_8041ca74;
		work->rotZ = float_0_8041ca74;
		work->randOff = (f32)(((rand() % 100) >> 4) & 1);
		angle = reviseAngle(baseAngle + (f32)((dirSign != float_0_8041ca74) ? -90 : 90));
		angle = 6.2831855f * angle / 360.0f;
		work->sinA = sin(angle);
		work->cosA = cos(angle);
		work->animPose = -1;
	}
	return entry;
}

void effMugiKemuriMain(EffectEntry* effect) {
	MugiKemuriWork* work;
	MugiKemuriWork* w;
	Vec disp;
	Vec pos;
	s32 i;
	s32 j;
	s32 inBattle;
	BOOL anyActive;
	const char* name;

	work = effect->userdata;
	pos = (Vec){ work->x, work->y, work->z };
	disp = pos;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		w = effect->userdata;
		// A separate index keeps i and w in the original's registers; reusing i swaps them.
		for (j = 0; j < effect->count; j++, w++) {
			if (w->animPose != -1) {
				animPoseRelease(w->animPose);
			}
		}
		effDelete(effect);
		return;
	}

	w = work;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->animPose == -1) {
			name = strncmp(gp->currentMapName, "win", 3) == 0 ? str_EFF_Hana : str_EFF_Mugi;
			if (!animGroupBaseAsync(name, inBattle, 0)) {
				return;
			}
			w->animPose = animPoseEntry(name, inBattle);
			animPoseSetAnim(w->animPose, "Z_1", 1);
		}
	}

	anyActive = FALSE;
	w = effect->userdata;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->active == 0) {
			continue;
		}
		if (--w->lifetime <= 0) {
			w->active = 0;
			continue;
		}
		anyActive = TRUE;
		w->scaleSpeed *= 0.85f;
		w->x += w->scaleSpeed * w->sinA;
		w->z += w->scaleSpeed * w->cosA;
		w->timeVel += w->timeVelAdd;
		w->timeAdd += w->timeVel;
		w->time += w->timeAdd;
		w->y += w->time;
		if (w->time < float_0_8041ca74) {
			w->timeVel = 0.004f;
		}
		w->rotX -= 1.0f;
		if (w->rotX < -100.0f) {
			w->rotX = -100.0f;
		}
	}

	if (!anyActive) {
		w = effect->userdata;
		for (i = 0; i < effect->count; i++, w++) {
			if (w->animPose != -1) {
				animPoseRelease(w->animPose);
			}
		}
		effDelete(effect);
		return;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMugiKemuriDisp, effect);
}

void effMugiKemuri2Main(EffectEntry* effect) {
	MugiKemuriWork* work;
	MugiKemuriWork* w;
	MarioWork* mario;
	Vec disp;
	Vec pos;
	s32 i;
	s32 j;
	BOOL anyActive;
	s32 inBattle;
	const char* name;

	work = effect->userdata;
	pos = (Vec){ work->x, work->y, work->z };
	disp = pos;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		w = effect->userdata;
		// A separate index keeps i and w in the original's registers; reusing i swaps them.
		for (j = 0; j < effect->count; j++, w++) {
			if (w->animPose != -1) {
				animPoseRelease(w->animPose);
			}
		}
		effDelete(effect);
		return;
	}

	w = work;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->animPose == -1) {
			name = strncmp(gp->currentMapName, "win", 3) == 0 ? str_EFF_Hana : str_EFF_Mugi;
			if (!animGroupBaseAsync(name, inBattle, 0)) {
				return;
			}
			w->animPose = animPoseEntry(name, inBattle);
			animPoseSetAnim(w->animPose, "Z_1", 1);
		}
	}

	anyActive = FALSE;
	w = effect->userdata;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->active == 0) {
			continue;
		}
		if (--w->lifetime <= 0) {
			w->active = 0;
			continue;
		}
		anyActive = TRUE;
		mario = marioGetPtr();
		if (w->type == 0 && w->speed != float_0_8041ca74 && mario->currMotionId == 0) {
			w->lifetime++;
			continue;
		}
		w->timeVel += w->timeVelAdd;
		w->timeAdd += w->timeVel;
		w->time += w->timeAdd;
		w->rotZ += w->time * w->scaleSpeed;
		w->x -= w->sinA * (w->time * w->scale);
		w->z -= w->cosA * (w->time * w->scale);
		w->alpha -= 12;
		if (w->time < float_0_8041ca74) {
			w->speed = float_0_8041ca74;
		}
	}

	if (!anyActive) {
		BOOL still = FALSE;
		w = effect->userdata;
		for (i = 0; i < effect->count; i++, w++) {
			w->alpha -= 10;
			if (w->alpha < 0) {
				w->alpha = 0;
			} else {
				still = TRUE;
			}
		}
		if (!still) {
			w = effect->userdata;
			for (i = 0; i < effect->count; i++, w++) {
				if (w->animPose != -1) {
					animPoseRelease(w->animPose);
				}
			}
			effDelete(effect);
			return;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMugiKemuri2Disp, effect);
}

const char str_EFF_Hana[12] = "EFF_Hana";
const char str_EFF_Mugi[12] = "EFF_Mugi";
const f64 double_0p005_802f34d8 = 0.005;

void effMugiKemuriDisp(CameraId cameraId, void* param) {
	EffectEntry* entry = param;
	MugiKemuriWork* work = entry->userdata;
	Mtx scale, trans, rotX, rotY, rotZ;
	s32 i;

	for (i = 0; i < entry->count; i++, work++) {
		if (work->active == 0 && work->alpha == 0) {
			continue;
		}
		PSMTXRotRad(rotX, 'x', float_deg2rad_8041ca70 * work->rotX);
		PSMTXRotRad(rotY, 'y', float_deg2rad_8041ca70 * work->rotY);
		PSMTXRotRad(rotZ, 'z', float_deg2rad_8041ca70 * work->rotZ);
		PSMTXTrans(trans, work->x, work->y, work->z);
		PSMTXScale(scale, work->scaleX, work->scaleY, work->scaleZ);
		PSMTXConcat(rotX, rotZ, rotZ);
		PSMTXConcat(rotY, rotZ, rotZ);
		PSMTXConcat(rotZ, scale, scale);
		PSMTXConcat(trans, scale, trans);
		if (work->animPose != -1) {
			animPoseSetMaterialFlagOn(work->animPose, 0x40);
			animPoseSetMaterialEvtColor(work->animPose, (GXColor){ 0xFF, 0xFF, 0xFF, work->alpha });
			animPoseMain(work->animPose);
			animPoseDrawMtx(work->animPose, trans, 1, float_0_8041ca74, 15.0f);
			animPoseDrawMtx(work->animPose, trans, 2, float_0_8041ca74, 15.0f);
			animPoseDrawMtx(work->animPose, trans, 3, float_0_8041ca74, 15.0f);
		}
	}
}

void effMugiKemuri2Disp(CameraId cameraId, void* param) {
	EffectEntry* entry = param;
	MugiKemuriWork* work = entry->userdata;
	Mtx scale, trans, rotX, rotY, rotZ;
	s32 i;

	for (i = 0; i < entry->count; i++, work++) {
		if (work->active == 0 && work->alpha == 0) {
			continue;
		}
		PSMTXRotRad(rotX, 'x', float_deg2rad_8041ca70 * work->rotX);
		PSMTXRotRad(rotY, 'y', float_deg2rad_8041ca70 * work->rotY);
		PSMTXRotRad(rotZ, 'z', float_deg2rad_8041ca70 * work->rotZ);
		PSMTXTrans(trans, work->x, work->y, work->z);
		PSMTXScale(scale, work->scaleX, work->scaleY, work->scaleZ);
		PSMTXConcat(rotX, rotZ, rotZ);
		PSMTXConcat(rotY, rotZ, rotZ);
		PSMTXConcat(rotZ, scale, scale);
		PSMTXConcat(trans, scale, trans);
		if (work->animPose != -1) {
			animPoseSetMaterialFlagOn(work->animPose, 0x40);
			animPoseSetMaterialEvtColor(work->animPose, (GXColor){ 0xFF, 0xFF, 0xFF, work->alpha });
			animPoseMain(work->animPose);
			animPoseDrawMtx(work->animPose, trans, 1, float_0_8041ca74, 15.0f);
			animPoseDrawMtx(work->animPose, trans, 2, float_0_8041ca74, 15.0f);
			animPoseDrawMtx(work->animPose, trans, 3, float_0_8041ca74, 15.0f);
		}
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_8041ca70 = 0.017453292f;
const f32 float_0_8041ca74 = 0.0f;
