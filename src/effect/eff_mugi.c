#include "eff/eff_mugi.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// .rodata constants, declared in their original order so MWCC can address the .rodata base + offset.
const Vec vec3_802f3498 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f34a4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f34b0 = { 0.0f, 0.0f, 0.0f };
const char str_802F34BC[12] = "EFF_Hana";
const char str_802F34C8[12] = "EFF_Mugi";
// dtk padding labels, defined only to reproduce the original's .rodata alignment.
const char gap_04_802F34D4_rodata[4] = { 0 };
const f64 double_0p005_802f34d8 = 0.005;
const char str_802F34E0[12] = "MugiKemuri2";
const f64 double_to_int_802f34f0 = 4503601774854144.0;
const char str_802F34F8[12] = "MugiKemuri";
const char gap_04_802F3504_rodata[4] = { 0 };

// .sdata2 constants.
const char str_8041CA7C[4] = "win";
const char str_8041CA80[4] = "Z_1";
extern const GXColor dat_8041ca68;
extern const GXColor dat_8041ca6c;
extern const f32 float_deg2rad_8041ca70;
extern const f32 float_0_8041ca74;
extern const f32 float_15_8041ca78;
extern const f32 float_0p85_8041ca84;
extern const f32 float_0p004_8041ca88;
extern const f32 float_1_8041ca8c;
extern const f32 float_neg100_8041ca90;
extern const f32 float_0p7_8041ca94;
extern const f32 float_5p4_8041ca98;
extern const f32 float_0p5_8041ca9c;
extern const f32 float_neg0p07_8041caa0;
extern const f32 float_0p003_8041caa4;
extern const f32 float_6p2832_8041caa8;
extern const f32 float_360_8041caac;
extern const f32 float_0p3_8041cab0;
extern const f32 float_1p75_8041cab4;
extern const f32 float_neg0p08_8041cab8;
extern const f32 float_neg3p9_8041cabc;

// 100%: initialises five smoke puffs. The original's callee-saved FPR band is
// f19-f27 for the pooled constants and f28-f31 for the four arguments; MWCC only
// colours it that way when the sin() result is held in a temp before the store
// (see the MUST_MATCH block below).
EffectEntry* effMugiKemuriEntry(f32 float1, f32 float2, f32 float3, f32 float4) {
	EffectEntry* entry = effEntry();
	MugiKemuriWork* work;
	f32 angle;
#ifdef MUST_MATCH
	f32 s;
#endif
	s32 i;

	entry->type = str_802F34F8;
	entry->count = 5;
	work = __memAlloc(HEAP_EFFECT, sizeof(MugiKemuriWork) * 5);
	entry->userdata = work;
	memset(work, 0, sizeof(MugiKemuriWork) * 5);
	entry->callback = effMugiKemuriMain;
	entry->flags |= 2;

	for (i = 0; i < 5; i++, work++) {
		work->active = 1;
		work->scale = float_0p3_8041cab0;
		work->speed = float_0_8041ca74;
		work->unk38 = float_0_8041ca74;
		work->x = float1;
		work->y = float2;
		work->z = float3;
		work->scaleX = float_0p7_8041ca94;
		work->scaleY = float_0p7_8041ca94;
		work->scaleZ = float_0p7_8041ca94;
		work->alpha = 0xFF;
		work->lifetime = 60;
		work->rotX = float_0_8041ca74;
		work->rotY = float4 + (f32)(i * 72);
		work->rotZ = float_0_8041ca74;
		work->time = float_1p75_8041cab4;
		work->timeAdd = float_neg0p08_8041cab8;
		work->timeVel = float_0_8041ca74;
		work->timeVelAdd = float_0_8041ca74;
		work->scaleSpeed = float_neg3p9_8041cabc;
		angle = reviseAngle(work->rotY);
		angle = float_6p2832_8041caa8 * angle / float_360_8041caac;
#ifdef MUST_MATCH
		// Match-only: holding the sin() result in a temp first gives the original's
		// callee-saved FPR allocation (args f28-f31, constants f19-f27); without it
		// MWCC rotates the whole band by four (args f24-f27). The natural form is
		// under #else.
		s = sin(angle);
		work->sinA = s;
#else
		work->sinA = sin(angle);
#endif
		work->cosA = cos(angle);
		work->animPose = -1;
	}
	return entry;
}

// The ±10/±90 ternaries must be written `float6 != 0 ? -N : N` (the branch takes the
// negative case) and the loop must advance `work` itself (`i++, work++` with `work->`),
// which keeps one web for the pointer; with `work[i]` the allocator splits it and swaps
// the entry/work callee-saved registers. The int->float magic still pools as a local
// @NNN@ rather than double_to_int_802f34f0; that symbol name does not affect the score.
EffectEntry* effMugiKemuri2Entry(s32 int1, f32 float2, f32 float3, f32 float4, f32 float5, f32 float6) {
	EffectEntry* entry = effEntry();
	MugiKemuriWork* work;
	f32 angle;
	s32 i;

	entry->type = str_802F34E0;
	entry->count = 5;
	work = __memAlloc(HEAP_EFFECT, sizeof(MugiKemuriWork) * 5);
	entry->userdata = work;
	memset(work, 0, sizeof(MugiKemuriWork) * 5);
	entry->callback = effMugiKemuri2Main;
	entry->flags |= 2;

	for (i = 0; i < 5; i++, work++) {
		work->active = 1;
		work->type = int1;
		work->unk38 = float_0_8041ca74;
		work->x = float2;
		work->y = float3;
		work->z = float4;
		work->scaleX = float_0p7_8041ca94;
		work->scaleY = float_0p7_8041ca94;
		work->scaleZ = float_0p7_8041ca94;
		work->alpha = 0xFF;
		work->speed = float_1_8041ca8c;
		work->scale = float_5p4_8041ca98;
		work->scaleSpeed = (f32)((float6 != float_0_8041ca74) ? -10 : 10);
		work->time = float_0p5_8041ca9c;
		work->timeAdd = float_neg0p07_8041caa0;
		work->timeVel = float_0p003_8041caa4;
		work->timeVelAdd = float_0_8041ca74;
		work->lifetime = 15;
		work->rotY = float5;
		work->rotX = float_0_8041ca74;
		work->rotZ = float_0_8041ca74;
		work->randOff = (f32)(((rand() % 100) >> 4) & 1);
		angle = reviseAngle(float5 + (f32)((float6 != float_0_8041ca74) ? -90 : 90));
		angle = float_6p2832_8041caa8 * angle / float_360_8041caac;
		work->sinA = sin(angle);
		work->cosA = cos(angle);
		work->animPose = -1;
	}
	return entry;
}

// NONMATCHING (88.95%): the pos/disp Vec pair is coalesced instead of copied,
// EFF_Hana/EFF_Mugi are addressed via their own symbols instead of
// vec3_802f3498+0x24/0x30, and the work pointer lives in r5, not r4.
void effMugiKemuriMain(EffectEntry* effect) {
	MugiKemuriWork* work = effect->userdata;
	MugiKemuriWork* w;
	Vec disp;
	Vec pos;
	s32 i;
	s32 inBattle;
	BOOL anyActive;
	const char* name;

	pos = vec3_802f34a4;
	pos.x = work->x;
	pos.y = work->y;
	pos.z = work->z;
	disp = pos;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		for (i = 0; i < effect->count; i++) {
			if (work[i].animPose != -1) {
				animPoseRelease(work[i].animPose);
			}
		}
		effDelete(effect);
		return;
	}

	w = work;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->animPose == -1) {
			name = strncmp(gp->currentMapName, str_8041CA7C, 3) == 0 ? str_802F34BC : str_802F34C8;
			if (!animGroupBaseAsync(name, inBattle, 0)) {
				return;
			}
			w->animPose = animPoseEntry(name, inBattle);
			animPoseSetAnim(w->animPose, str_8041CA80, 1);
		}
	}

	anyActive = FALSE;
	w = work;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->active == 0) {
			continue;
		}
		if (--w->lifetime <= 0) {
			w->active = 0;
			continue;
		}
		anyActive = TRUE;
		w->scaleSpeed *= float_0p85_8041ca84;
		w->x += w->scaleSpeed * w->sinA;
		w->z += w->scaleSpeed * w->cosA;
		w->timeVel += w->timeVelAdd;
		w->timeAdd += w->timeVel;
		w->time += w->timeAdd;
		w->y += w->time;
		if (w->time < float_0_8041ca74) {
			w->timeVel = float_0p004_8041ca88;
		}
		w->rotX -= float_1_8041ca8c;
		if (w->rotX < float_neg100_8041ca90) {
			w->rotX = float_neg100_8041ca90;
		}
	}

	if (!anyActive) {
		for (i = 0; i < effect->count; i++) {
			if (work[i].animPose != -1) {
				animPoseRelease(work[i].animPose);
			}
		}
		effDelete(effect);
		return;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMugiKemuriDisp, effect);
}

// NONMATCHING (86.15%): the pos/disp Vec pair is coalesced instead of copied,
// the rodata strings are addressed directly instead of base+0x24/0x30, and the
// work pointer/FPR allocation differs.
void effMugiKemuri2Main(EffectEntry* effect) {
	MugiKemuriWork* work = effect->userdata;
	MugiKemuriWork* w;
	MarioWork* mario;
	Vec disp;
	Vec pos;
	s32 i;
	s32 inBattle;
	BOOL anyActive;
	const char* name;

	pos = vec3_802f34b0;
	pos.x = work->x;
	pos.y = work->y;
	pos.z = work->z;
	disp = pos;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		for (i = 0; i < effect->count; i++) {
			if (work[i].animPose != -1) {
				animPoseRelease(work[i].animPose);
			}
		}
		effDelete(effect);
		return;
	}

	w = work;
	for (i = 0; i < effect->count; i++, w++) {
		if (w->animPose == -1) {
			name = strncmp(gp->currentMapName, str_8041CA7C, 3) == 0 ? str_802F34BC : str_802F34C8;
			if (!animGroupBaseAsync(name, inBattle, 0)) {
				return;
			}
			w->animPose = animPoseEntry(name, inBattle);
			animPoseSetAnim(w->animPose, str_8041CA80, 1);
		}
	}

	anyActive = FALSE;
	w = work;
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
		w = work;
		for (i = 0; i < effect->count; i++, w++) {
			w->alpha -= 10;
			if (w->alpha >= 0) {
				still = TRUE;
			} else {
				w->alpha = 0;
			}
		}
		if (!still) {
			for (i = 0; i < effect->count; i++) {
				if (work[i].animPose != -1) {
					animPoseRelease(work[i].animPose);
				}
			}
			effDelete(effect);
			return;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMugiKemuri2Disp, effect);
}

#ifdef MUST_MATCH
// Returning the colour from an inline makes mwcc put the local GXColor at r1+0x8 and the
// outgoing-argument copy at r1+0xc, the way the original does; building it inline in the caller
// puts that pair the other way round.
static inline GXColor mugiKemuriColor(u8 alpha) {
	GXColor c = dat_8041ca68;
	c.a = alpha;
	return c;
}

// Same stack-slot reason as mugiKemuriColor, for the second colour constant used by
// effMugiKemuri2Disp (its original loads dat_8041ca6c).
static inline GXColor mugiKemuri2Color(u8 alpha) {
	GXColor c = dat_8041ca6c;
	c.a = alpha;
	return c;
}
#endif

void effMugiKemuriDisp(CameraId cameraId, void* param) {
	EffectEntry* entry = param;
	MugiKemuriWork* work = entry->userdata;
	Mtx scale, trans, rotX, rotY, rotZ;
	GXColor color;
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
#ifdef MUST_MATCH
			color = mugiKemuriColor(work->alpha);
#else
			color = dat_8041ca68;
			color.a = work->alpha;
#endif
			animPoseSetMaterialEvtColor(work->animPose, color);
			animPoseMain(work->animPose);
			animPoseDrawMtx(work->animPose, trans, 1, float_0_8041ca74, float_15_8041ca78);
			animPoseDrawMtx(work->animPose, trans, 2, float_0_8041ca74, float_15_8041ca78);
			animPoseDrawMtx(work->animPose, trans, 3, float_0_8041ca74, float_15_8041ca78);
		}
	}
}

void effMugiKemuri2Disp(CameraId cameraId, void* param) {
	EffectEntry* entry = param;
	MugiKemuriWork* work = entry->userdata;
	Mtx scale, trans, rotX, rotY, rotZ;
	GXColor color;
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
#ifdef MUST_MATCH
			color = mugiKemuri2Color(work->alpha);
#else
			color = dat_8041ca6c;
			color.a = work->alpha;
#endif
			animPoseSetMaterialEvtColor(work->animPose, color);
			animPoseMain(work->animPose);
			animPoseDrawMtx(work->animPose, trans, 1, float_0_8041ca74, float_15_8041ca78);
			animPoseDrawMtx(work->animPose, trans, 2, float_0_8041ca74, float_15_8041ca78);
			animPoseDrawMtx(work->animPose, trans, 3, float_0_8041ca74, float_15_8041ca78);
		}
	}
}
