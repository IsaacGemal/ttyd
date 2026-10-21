#include "battle/battle_stage.h"
#include "battle/battle.h"
#include "system.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <string.h>
// math_ext.h's out-of-line declarations, not the SDK <math.h>: the SDK header's
// inline sqrt/sqrtf carry `static const` locals that land in this unit's .rodata
// and shift the vec3 pool (see batSpotInit).
#include "math_ext.h"
#include "matching.h"
#include "manager/evtmgr_cmd.h"
#include <stdlib.h>
#include "driver/mapdrv.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern const f32 float_neg1_8041c818;
static void screen_capture(CameraId camId, void* param);  // local to this file in the original

//.sbss
static BattleSpotWork work;

//.sbss2: the six zero words the original's light/spot colour compound literals
// pool. Declared last-declared-first so they land at the original addresses.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u32 unk_8042302c;
__declspec(section ".sdata") u32 unk_80423028;
__declspec(section ".sdata") u32 unk_80423024;
__declspec(section ".sdata") u32 unk_80423020;
__declspec(section ".sdata") u32 unk_8042301c;
__declspec(section ".sdata") u32 unk_80423018;
#pragma section ".sdata" ".sbss"

//.sdata
static BattleSpotWork* wp = &work;

//.sdata: the 7 zero words between wp and inta_tex in the original
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804117E4_sdata[7] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset

//.sdata (local to this file in the original)
static u32 inta_tex[8] = {0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000};

//.sdata (local to this file in the original)
static u32 screen_evt[7] = {0x0000006B, 0x0002005B, (u32)screen_evt_func, 0xFE363C80, 0x0000006D, 0x00000002, 0x00000001};

//.sdata: the trailing zero word of the section
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8041183C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

//.sdata2 objects are defined at the end of the file.

// .rodata zero/constant vectors this unit emitted; MWCC pools every access
// through the vec3_802f31a8 base, and the int->float conversion double the
// compiler emits lands after them at +0x78 (see batSpotInit).
const Vec vec3_802f31a8[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31b4[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31c0[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31cc[] = {{0.0f, 0.0f, 0.0f}};
extern const f32 float_0_8041c7f0;
extern const f32 float_1_8041c7f4;
extern const f32 float_0p25_8041c7f8;
extern const f32 float_60_8041c7fc;
extern const f32 float_4p75_8041c800;
extern const f32 float_3p75_8041c804;
extern const f32 float_16_8041c808;
extern const f32 float_6_8041c80c;
extern const f32 float_100_8041c810;
extern const f32 float_1000_8041c814;
extern f32 float_32767_8041c81c;
extern f32 float_5000_8041c84c;
extern f32 float_6000_8041c850;
extern f32 float_deg2rad_8041c820;
extern f32 float_6p2832_8041c824;
extern f32 float_30_8041c828;
extern f32 float_neg640_8041c840;
extern f32 float_640_8041c844;
extern f32 float_105_8041c848;
extern f32 float_2_8041c838;
extern f32 float_0p34907_8041c83c;
extern f32 float_neg1000_8041c82c;
extern s32 dat_8041c7ec;

typedef struct ScreenWork {
	void* texBuffer;      //0x0
	s32 captured;         //0x4, set by screen_capture; the effect runs only after a capture
	f32 scale;            //0x8, eases toward scaleTarget
	f32 scaleTarget;      //0xC
	f32 offsetX;          //0x10, eases toward offsetXTarget
	f32 offsetXTarget;    //0x14
	f32 offsetY;          //0x18, eases toward offsetYTarget
	f32 offsetYTarget;    //0x1C
	s32 tick;             //0x20, counts frames; retargets every 200
	s32 noiseMode;        //0x24, 1 builds the noise texture, 0 uses the captured screen
	f32 field_0x28;       //0x28, set to -1.0 in tou_gamen_screen_tev_init and never read
} ScreenWork;

// forward declarations

BattleStage* BattleStageGetPtr(void) {
    return &_battleWorkPointer->stage;
}

// The four .sda2 floats are `const` in the header so MWCC hoists all four lfs,
// matching the B24/B38 stores. The light loop indexes `stage->lights[i]` (not a
// raw byte offset) so the compiler strength-reduces to the original's offset
// register and reuses its zero for the stored `NULL`.
void BattleStageInit(void) {
	s32 i;
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light;
	memset(stage, 0, sizeof(BattleStage));
	stage->field_0 = 0;
	for (i = 0; i < 12; i++) {
		light = &stage->lights[i];
		light->flags = 0;
		light->spline = NULL;
	}
	stage->stgDark.field_0 = 1;
	stage->stgDark.baseAlpha = 0;
	stage->stgDark.alpha = 0;
	stage->stgDark.alphaFrom = 0;
	stage->stgDark.alphaTo = 0;
	stage->stgDark.alphaFrames = 0;
	stage->stgDark.alphaFramesTotal = 0;

	stage->audDark.field_0 = 1;
	stage->audDark.baseAlpha = 0x8C;
	stage->audDark.alpha = 0x8C;
	stage->audDark.alphaFrom = 0;
	stage->audDark.alphaTo = 0;
	stage->audDark.alphaFrames = 0;
	stage->audDark.alphaFramesTotal = 0;

	stage->stageSize.x = float_290_8041c854;
	stage->stageSize.y = float_200_8041c858;
	stage->stageSize.z = float_90_8041c85c;
	stage->audienceAreaSize.x = stage->stageSize.x;
	stage->audienceAreaSize.y = float_neg50_8041c860;
	stage->audienceAreaSize.z = float_200_8041c858;
	BattleStageObjectInit();
	batSpotInit();
}

void BattleStageMain(void) {
	BattleStage* stage = &_battleWorkPointer->stage;
	s32 i;
	BattleStageLight* light;

	batSpotMain();

	for (i = 0; i < 12; i++) {
		light = &stage->lights[i];

		if (light->flags & 1) {
			if (light->colorFrames > 0) {
				light->colorFrames--;
				light->colorCur.r = intplGetValue(light->colorEase, light->colorFramesTotal - light->colorFrames,
				                                  light->colorFramesTotal, light->colorFrom.r, light->colorTo.r);
				light->colorCur.g = intplGetValue(light->colorEase, light->colorFramesTotal - light->colorFrames,
				                                  light->colorFramesTotal, light->colorFrom.g, light->colorTo.g);
				light->colorCur.b = intplGetValue(light->colorEase, light->colorFramesTotal - light->colorFrames,
				                                  light->colorFramesTotal, light->colorFrom.b, light->colorTo.b);
				if ((light->flags & 2) && light->colorCur.r == 0 && light->colorCur.g == 0 && light->colorCur.b == 0) {
					light->flags = 0;
					continue;
				}
			}

			if (light->widthFrames > 0) {
				light->widthFrames--;
				light->widthCur = intplGetValue(light->widthEase, light->widthFramesTotal - light->widthFrames,
				                                light->widthFramesTotal, light->widthFrom, light->widthTo);
			}

			if (light->posFrames > 0) {
				light->posFrames--;
				light->posCur.x = intplGetValue(light->posEase, light->posFramesTotal - light->posFrames,
				                                light->posFramesTotal, light->posFrom.x, light->posTo.x);
				light->posCur.y = intplGetValue(light->posEase, light->posFramesTotal - light->posFrames,
				                                light->posFramesTotal, light->posFrom.y, light->posTo.y);
				light->posCur.z = intplGetValue(light->posEase, light->posFramesTotal - light->posFrames,
				                                light->posFramesTotal, light->posFrom.z, light->posTo.z);
			}

			if (light->flags & 4) {
				s32* spline = (s32*)light->spline;

				light->targetCur.x = getSpline((f32*)(spline + 9), (f32*)(spline + 17), (f32*)(spline + 25),
					(f32*)(spline + 33), spline + 1, spline[0], light->targetFrames);
				light->targetCur.y = getSpline((f32*)(spline + 41), (f32*)(spline + 49), (f32*)(spline + 57),
					(f32*)(spline + 65), spline + 1, spline[0], light->targetFrames);
				light->targetCur.z = getSpline((f32*)(spline + 73), (f32*)(spline + 81), (f32*)(spline + 89),
					(f32*)(spline + 97), spline + 1, spline[0], light->targetFrames);
				light->targetFrames++;
				if (light->targetFrames >= spline[spline[0]]) {
					if (light->flags & 8) {
						light->targetFrames -= spline[spline[0]];
					} else {
						light->targetFrames = spline[spline[0]];
					}
				}
			} else {
				if (light->targetFrames > 0) {
					light->targetFrames--;
					light->targetCur.x = intplGetValue(light->targetEase,
					                                   light->targetFramesTotal - light->targetFrames,
					                                   light->targetFramesTotal, light->targetFrom.x,
					                                   light->targetTo.x);
					light->targetCur.y = intplGetValue(light->targetEase,
					                                   light->targetFramesTotal - light->targetFrames,
					                                   light->targetFramesTotal, light->targetFrom.y,
					                                   light->targetTo.y);
					light->targetCur.z = intplGetValue(light->targetEase,
					                                   light->targetFramesTotal - light->targetFrames,
					                                   light->targetFramesTotal, light->targetFrom.z,
					                                   light->targetTo.z);
				}
				if (light->spline != NULL) {
					__memFree(HEAP_BATTLE, light->spline);
					light->spline = NULL;
				}
			}

			if (light->upFrames > 0) {
				light->upFrames--;
				light->upCur.x = intplGetValue(light->upEase, light->upFramesTotal - light->upFrames,
				                               light->upFramesTotal, light->upFrom.x, light->upTo.x);
				light->upCur.y = intplGetValue(light->upEase, light->upFramesTotal - light->upFrames,
				                               light->upFramesTotal, light->upFrom.y, light->upTo.y);
				light->upCur.z = intplGetValue(light->upEase, light->upFramesTotal - light->upFrames,
				                               light->upFramesTotal, light->upFrom.z, light->upTo.z);
			}
		} else {
			if (light->spline != NULL) {
				__memFree(HEAP_BATTLE, light->spline);
				light->spline = NULL;
			}
		}
	}

	if (stage->stgDark.alphaFrames > 0) {
		stage->stgDark.alphaFrames--;
		stage->stgDark.alpha = intplGetValue(stage->stgDark.alphaEase,
		                                     stage->stgDark.alphaFramesTotal - stage->stgDark.alphaFrames,
		                                     stage->stgDark.alphaFramesTotal, stage->stgDark.alphaFrom,
		                                     stage->stgDark.alphaTo);
	}
	if (stage->stgDark.baseFrames > 0) {
		stage->stgDark.baseFrames--;
		stage->stgDark.baseAlpha = intplGetValue(stage->stgDark.baseEase,
		                                         stage->stgDark.baseFramesTotal - stage->stgDark.baseFrames,
		                                         stage->stgDark.baseFramesTotal, stage->stgDark.baseFrom,
		                                         stage->stgDark.baseTo);
	}
	if (stage->audDark.alphaFrames > 0) {
		stage->audDark.alphaFrames--;
		stage->audDark.alpha = intplGetValue(stage->audDark.alphaEase,
		                                     stage->audDark.alphaFramesTotal - stage->audDark.alphaFrames,
		                                     stage->audDark.alphaFramesTotal, stage->audDark.alphaFrom,
		                                     stage->audDark.alphaTo);
	}
	if (stage->audDark.baseFrames > 0) {
		stage->audDark.baseFrames--;
		stage->audDark.baseAlpha = intplGetValue(stage->audDark.baseEase,
		                                         stage->audDark.baseFramesTotal - stage->audDark.baseFrames,
		                                         stage->audDark.baseFramesTotal, stage->audDark.baseFrom,
		                                         stage->audDark.baseTo);
	}
}

void BattleStageEnd(void) {
	s32 i;
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light;

	for (i = 0; i < 12; i++) {
		light = &stage->lights[i];
		if (light->spline != NULL) {
			__memFree(HEAP_BATTLE, light->spline);
			light->spline = NULL;
		}
	}
}

void BattleStageDisp(void) {
	BattleGetUnitPtr(_battleWorkPointer, _battleWorkPointer->activeUnitId);
	dispEntry(4, float_5000_8041c84c, 2, BattleStageDispDark, NULL);
	dispEntry(4, float_6000_8041c850, 2, BattleStageDispLight, NULL);
}

#pragma dont_inline on // keep DarkByPoint out of line: the original calls it.
void DarkByPoint(u8 color, f32 x, f32 y, f32 z) {
	GXPosition3f32(x, y, z);
	GXColor4u8(0, 0, 0, color);
}

// Keep DarkBySquare out of line: the original calls it.
// The original declares the colour last (its caller evaluates it after the
// floats); the ABI is identical, so this compiles to the same body either way.
void DarkBySquare(f32 a, f32 b, f32 c, f32 d, f32 z, u8 color) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	DarkByPoint(color, a, b, z);
	DarkByPoint(color, c, b, z);
	DarkByPoint(color, c, d, z);
	DarkByPoint(color, a, d, z);
}
#pragma dont_inline reset

// opt_common_subs off: with common subexpression elimination on, MWCC loads the
// shared float constants in the reverse order (f2 before f1). DarkBySquare's
// prototype is (f32..., u8 color) in the original, so the colour argument is
// evaluated last; see the note on its definition.
#pragma opt_common_subs off
void BattleStageDispDark(CameraId camId, void* param) {
	BattleStage* stage = &_battleWorkPointer->stage;
	CameraEntry* cam = camGetPtr(4);

	GXSetCullMode(2);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetBlendMode(1, 4, 5, 5);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xB, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 1, 1, 0, 2, 2);
	GXSetNumTexGens(0);
	GXLoadPosMtxImm(cam->view, 0);
	GXSetNumTevStages(1);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaIn(0, 7, 7, 7, 5);
	GXSetTevOrder(0, 0xFF, 0xFF, 4);
	{
		BattleStageDark* audDark = &stage->audDark;
		GXSetZMode(1, 6, 0);
		DarkBySquare(float_neg640_8041c840, float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_105_8041c848, audDark->alpha);
		GXSetZMode(1, 1, 0);
		DarkBySquare(float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_neg640_8041c840, float_105_8041c848, audDark->alpha);
	}
	GXSetZMode(1, 1, 0);
	DarkBySquare(float_neg640_8041c840, float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_105_8041c848, stage->stgDark.alpha);
}
#pragma opt_common_subs reset

void BattleStageDispLight(CameraId camId, void* param) {
	s32 i;
	BattleStage* stage = &_battleWorkPointer->stage;
	CameraEntry* cam = camGetPtr(4);

	GXSetCullMode(2);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetBlendMode(1, 2, 1, 5);
	GXSetZMode(1, 3, 0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xB, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(2, 0, 1, 1, 0, 2, 2);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaIn(0, 7, 7, 7, 7);
	GXSetTevOrder(0, 0, 0, 4);
	GXLoadPosMtxImm(cam->view, 0);

	for (i = 0; i < 12; i++) {
		if (stage->lights[i].flags & 1) {
			Mtx mtx;
			Vec up;
			Vec axis;
			Vec dir;
			Vec base;
			Vec p1;
			Vec p2;
			f32 width;
			f32 two;
			s32 j;
			u8 r, g, b;

			dir = vec3_802f31a8[0];
			dir.x = stage->lights[i].targetCur.x - stage->lights[i].posCur.x;
			dir.y = stage->lights[i].targetCur.y - stage->lights[i].posCur.y;
			dir.z = stage->lights[i].targetCur.z - stage->lights[i].posCur.z;
			axis = dir;
			width = stage->lights[i].widthCur;
			up = stage->lights[i].upCur;
			two = float_2_8041c838;

			for (j = 0; j < 18; j++) {
				f32 px = stage->lights[i].posCur.x;
				f32 py = stage->lights[i].posCur.y;
				f32 pz = stage->lights[i].posCur.z;
				f32 tx = stage->lights[i].targetCur.x;
				f32 ty = stage->lights[i].targetCur.y;
				f32 tz = stage->lights[i].targetCur.z;

				base.x = px + two * (tx - px);
				base.y = py + two * (ty - py);
				base.z = pz + two * (tz - pz);

				p1.x = base.x + up.x * width;
				p1.y = base.y + up.y * width;
				p1.z = base.z + up.z * width;

				PSMTXRotAxisRad(mtx, &axis, float_0p34907_8041c83c);
				PSMTXMultVec(mtx, &up, &up);
				PSVECNormalize(&up, &up);

				p2.x = base.x + up.x * width;
				p2.y = base.y + up.y * width;
				p2.z = base.z + up.z * width;

				r = stage->lights[i].colorCur.r;
				g = stage->lights[i].colorCur.g;
				b = stage->lights[i].colorCur.b;

				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
				GXPosition3f32(px, py, pz);
				GXColor4u8(r, g, b, 0);
				GXPosition3f32(p1.x, p1.y, p1.z);
				GXColor4u8(r, g, b, 0);
				GXPosition3f32(p2.x, p2.y, p2.z);
				GXColor4u8(r, g, b, 0);
			}
		}
	}
}

void BattleStageOnLightInCommand(void) {
}

void BattleStageOffLightInCommand(s32 arg0, s32 arg1) {
}

void BattleStageOnLightFriendInCommand(void) {
}

void BattleStageOffLightFriendInCommand(void) {
}

void BattleStageOnLightEnemyInCommand(void) {
}

void BattleStageOffLightEnemyInCommand(void) {
}

s32 BattleStageGetLightNumberByName(const char* name) {
	s32 offset;
	BattleStage* stage = &_battleWorkPointer->stage;
	s32 i;

	i = 0;
	offset = 0;
	for (; i < 12; i++) {
		u8* p = (u8*)stage + offset;
		BattleStageLight* light = (BattleStageLight*)(p + 4);
		if ((light->flags & 1) && strcmp(light->name, name) == 0) {
			return i;
		}
		offset += 0xe8;
	}

	return -1;
}

s32 BattleStageSysLightEntry(const char* name) {
	BattleStage* stage = &_battleWorkPointer->stage;
	s32 i;

	for (i = 0; i <= 6; i++) {
		BattleStageLight* light = &stage->lights[i];
		if (light->flags & 1) {
			strcmp(light->name, name);
		}
	}

	i = 0;
	for (; i < 7; i++) {
		BattleStageLight* light = &stage->lights[i];
		if (!(light->flags & 1)) {
			light->flags = 1;
			strcpy(light->name, name);
			break;
		}
	}

	return i;
}

// Copy propagation off: without it BattleStageLightRelease stops matching.
#pragma opt_propagation off
void BattleStageLightRelease(s32 idx, s32 frames, u8 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	if (light->flags & 1) {
		if (frames == 0) {
			light->flags = 0;
		} else {
			light->flags |= 2;
			{
				BattleStageColor c = *(BattleStageColor*)&unk_80423018;
				c.r = light->colorCur.r;
				c.g = light->colorCur.g;
				c.b = light->colorCur.b;
				light->colorFrom = c;
			}
			light->colorTo = *(BattleStageColor*)&unk_8042301c;
			light->colorFrames = frames;
			light->colorFramesTotal = frames;
			light->colorEase = ease;
		}
	}
}

// Copy propagation off: without it BattleStageLightSetLightColor stops matching.
void BattleStageLightSetLightColor(s32 idx, u8 r, u8 g, u8 b, s32 frames, u8 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	{
		BattleStageColor c = *(BattleStageColor*)&unk_80423020;
		c.r = light->colorCur.r;
		c.g = light->colorCur.g;
		c.b = light->colorCur.b;
		light->colorFrom = c;
	}
	{
		BattleStageColor c = *(BattleStageColor*)&unk_80423024;
		c.r = r;
		c.g = g;
		c.b = b;
		light->colorTo = c;
	}
	light->colorFrames = frames;
	light->colorFramesTotal = frames;
	light->colorEase = ease;
	if (frames == 0) {
		light->colorCur = (BattleStageColor){light->colorCur.r, light->colorCur.g, light->colorCur.b, 0xFF};
	}
}

// Copy propagation off: without it BattleStageLightSetWidth stops matching.
void BattleStageLightSetWidth(s32 idx, s32 width, s32 frames, u8 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	light->widthFrom = light->widthCur;
	light->widthTo = (f32)width;
	light->widthFrames = frames;
	light->widthFramesTotal = frames;
	light->widthEase = ease;
	if (frames == 0) {
		light->widthCur = (f32)width;
	}
}

// Copy propagation off: without it BattleStageLightSetPosition stops matching.
void BattleStageLightSetPosition(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	light->posFrom.x = light->posCur.x;
	light->posFrom.y = light->posCur.y;
	light->posFrom.z = light->posCur.z;
	light->posTo.x = x;
	light->posTo.y = y;
	light->posTo.z = z;
	light->posFrames = frames;
	light->posFramesTotal = frames;
	light->posEase = ease;
	if (frames == 0) {
		Vec tmp = vec3_802f31b4[0];
		tmp.x = x;
		tmp.y = y;
		tmp.z = z;
		light->posCur = tmp;
	}
}

// Copy propagation off: without it BattleStageLightSetTarget stops matching.
#pragma dont_inline on
void BattleStageLightSetTarget(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	light->targetFrom.x = light->targetCur.x;
	light->targetFrom.y = light->targetCur.y;
	light->targetFrom.z = light->targetCur.z;
	light->targetTo.x = x;
	light->targetTo.y = y;
	light->targetTo.z = z;
	light->targetFrames = frames;
	light->targetFramesTotal = frames;
	light->targetEase = ease;
	light->flags &= ~4;
	if (frames == 0) {
		Vec tmp = vec3_802f31c0[0];
		tmp.x = x;
		tmp.y = y;
		tmp.z = z;
		light->targetCur = tmp;
	}
}
#pragma dont_inline reset
#pragma opt_propagation reset

void BattleStageLightSetTargetByUnit(s32 idx, s32 unitId, f32 x, f32 y, f32 z, s32 frames, u8 ease) {
	BattleUnit* unit;
	f32 ux, uy, uz;

	unit = BattleGetUnitPtr(_battleWorkPointer, unitId);
	BtlUnit_GetPos(unit, &ux, &uy, &uz);
	ux = ux + x;
	uy = uy + y;
	uz = uz + z;
	BattleStageLightSetTarget(idx, frames, ux, uy, uz, ease);
}

// ------------------------------------------------------------ light setters
#pragma opt_propagation off // needed: without it BattleStageLightSetUpVec 92.12%
void BattleStageLightSetUpVec(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	light->upFrom.x = light->upCur.x;
	light->upFrom.y = light->upCur.y;
	light->upFrom.z = light->upCur.z;
	light->upTo.x = x;
	light->upTo.y = y;
	light->upTo.z = z;
	light->upFrames = frames;
	light->upFramesTotal = frames;
	light->upEase = ease;
	if (frames == 0) {
		Vec tmp = vec3_802f31cc[0];
		tmp.x = x;
		tmp.y = y;
		tmp.z = z;
		light->upCur = tmp;
	}
}
#pragma opt_propagation reset

#pragma dont_inline on // keep SetDark out of line: the wrappers call it.
void BattleStageSetDark(BattleStageDark* dark, u8 a, s32 frames, s32 ease) {
	dark->alphaFrom = dark->alpha;
	dark->alphaTo = a;
	dark->alphaFrames = frames;
	dark->alphaFramesTotal = frames;
	dark->alphaEase = ease;
	if (frames == 0) {
		dark->alpha = a;
	}
}

// Keep SetDarkBase out of line: the wrappers call it.
void BattleStageSetDarkBase(BattleStageDark* dark, u8 a, s32 frames, s32 ease) {
	dark->baseFrom = dark->baseAlpha;
	dark->baseTo = a;
	dark->baseFrames = frames;
	dark->baseFramesTotal = frames;
	dark->baseEase = ease;
	if (frames == 0) {
		dark->baseAlpha = a;
	}
}
#pragma dont_inline reset

void BattleStageSetStgDark(u8 a, s32 frames, s32 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageSetDark(&stage->stgDark, a, frames, ease);
}

void BattleStageSetStgDarkBase(u8 a, s32 frames, s32 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageSetDarkBase(&stage->stgDark, a, frames, ease);
}

void BattleStageReturnStgDarkBase(s32 frames, s32 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageDark* dark = &stage->stgDark;
	BattleStageSetDark(dark, dark->baseAlpha, frames, ease);
}

void BattleStageSetAudDark(u8 a, s32 frames, s32 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageSetDark(&stage->audDark, a, frames, ease);
}

void BattleStageSetAudDarkBase(u8 a, s32 frames, s32 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageSetDarkBase(&stage->audDark, a, frames, ease);
}

void BattleStageReturnAudDarkBase(s32 frames, s32 ease) {
	BattleStage* stage = &_battleWorkPointer->stage;
	BattleStageDark* dark = &stage->audDark;
	BattleStageSetDark(dark, dark->baseAlpha, frames, ease);
}

// dont_inline: without it the compiler inlines getSpline into BattleStageMain
// opt_loop_invariants off: with it on, MWCC hoists float_0 and the int->float
// conversion constant in the opposite register order (f2/f1 instead of f0/f2).
#pragma opt_loop_invariants off
#pragma dont_inline on
f32 getSpline(f32* a, f32* b, f32* c, f32* d, s32* knots, s32 count, s32 value) {
	s32 i;

	for (i = count - 1; i >= 0; i--) {
		if ((f32)(value - knots[i]) >= float_0_8041c7f0) {
			break;
		}
	}

	{
		f32 t = (f32)(value - knots[i]);
		return a[i] + t * (b[i] + t * (c[i] + t * d[i]));
	}
}
#pragma dont_inline reset
#pragma opt_loop_invariants reset

BattleSpotWork* BatSpotGetPtr(void) {
    return wp;
}

#pragma dont_inline on
// The (Vec){...} compound literals are deliberate: their parse-time templates
// become the .rodata words at +0x30/+0x3c/+0x48, which lets MWCC address them
// (and the int->float conversion double at +0x78) off the vec3_802f31a8 base,
// exactly as the original does. Naming those three vectors as globals instead
// emits the templates after the named objects and gives the double its own
// base. vec3_802f31fc/208/214 must be defined after this function (see below)
// for the same reason.
void batSpotInit(void) {
	int i;
	BattleSpotEntry* entry;

	wp->count = 5;
	wp->entries = __memAlloc(HEAP_BATTLE, sizeof(BattleSpotEntry) * wp->count);
	memset(wp->entries, 0, sizeof(BattleSpotEntry) * wp->count);

	for (i = 0; i < 5; i++) {
		Vec v;
		entry = batSpotEntry();
		v = (Vec){0.0f, 0.0f, -50.0f};
		v.x = float_50_8041c834 * i + float_neg100_8041c830;
		entry->pos = v;
		entry->rot = (Vec){0.0f, 0.0f, 0.0f};
		entry->scale = (Vec){20.0f, 20.0f, 150.0f};
		entry->color = *(GXColor*)&dat_8041c7ec;
	}
}

// Defined here (not at the top) so the compound-literal templates batSpotInit
// pools land at +0x30 before these named vectors; the original's .rodata is
// vec3 a8/b4/c0/cc, then the d8/e4/f0 templates, then fc/208/214.
const Vec vec3_802f31fc[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f3208[] = {{1.0f, 1.0f, 1.0f}};
const Vec vec3_802f3214[] = {{0.0f, 0.0f, 0.0f}};

// Keep batSpotMain out of line: BattleStageMain calls it.
void batSpotMain() {
    s32 count = wp->count;

    while (count-- > 0) {}
    if (wp->skipRender > 0) {
        dispEntry(CAMERA_3D_IMAGE, float_neg1000_8041c82c, 0, batSpotDisp, NULL);
    }
}
#pragma dont_inline reset

void batSpotDisp(CameraId camId, void* param) {
	s32 i;
	BattleSpotEntry* entry;

	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xB, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0xFF, 0xFF, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
	GXSetTevAlphaIn(0, 7, 7, 7, 5);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetNumChans(1);

	entry = wp->entries;
	for (i = 0; i < wp->count; i++, entry++) {
		if ((entry->flags & 1) && (entry->flags & 2)) {
			GXSetCullMode(2);
			GXSetAlphaUpdate(1);
			GXSetColorUpdate(0);
			GXSetZMode(1, 3, 0);
			GXSetBlendMode(0, 4, 5, 0);
			GXSetChanCtrl(4, 0, 0, 1, 0, 2, 2);
			drawCone(camId, entry);

			GXSetCullMode(1);
			GXSetAlphaUpdate(1);
			GXSetColorUpdate(0);
			GXSetZMode(1, 3, 0);
			GXSetBlendMode(0, 4, 5, 0);
			GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
			GXSetChanMatColor(4, *(GXColor*)&unk_80423028);
			drawCone(camId, entry);

			GXSetCullMode(2);
			GXSetAlphaUpdate(0);
			GXSetColorUpdate(1);
			GXSetZMode(1, 3, 0);
			GXSetBlendMode(1, 6, 7, 0);
			GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
			GXSetChanMatColor(4, entry->color);
			drawCone(camId, entry);
		}
	}
}

void drawCone(CameraId camId, BattleSpotEntry* entry) {
	CameraEntry* cam = camGetPtr(camId);
	Mtx mTrans;
	Mtx mScale;
	Mtx mTmp;
	Mtx mRotX;
	Mtx mRotY;
	Mtx mRotZ;
	s32 i;

	PSMTXTrans(mTrans, entry->pos.x, entry->pos.y, entry->pos.z);
	PSMTXScale(mScale, entry->scale.x, entry->scale.y, entry->scale.z);
	PSMTXRotRad(mRotX, 'x', float_deg2rad_8041c820 * entry->rot.x);
	PSMTXRotRad(mRotY, 'y', float_deg2rad_8041c820 * entry->rot.y);
	PSMTXRotRad(mRotZ, 'z', float_deg2rad_8041c820 * entry->rot.z);
	PSMTXConcat(mRotZ, mRotY, mRotY);
	PSMTXConcat(mRotY, mRotX, mTmp);
	PSMTXConcat(cam->view, mTrans, mTrans);
	PSMTXConcat(mTrans, mTmp, mTmp);
	PSMTXConcat(mTmp, mScale, mTmp);
	GXLoadPosMtxImm(mTmp, 0);
	GXSetCurrentMtx(0);
	GXBegin(0xA0, 0, 0x20);

#ifdef MUST_MATCH
	GXWGFifo.f32 = MATCH_F32(float_0_8041c7f0);
	GXWGFifo.f32 = MATCH_F32(float_0_8041c7f0);
	GXWGFifo.f32 = MATCH_F32(float_0_8041c7f0);
#else
	GXPosition3f32(float_0_8041c7f0, float_0_8041c7f0, float_0_8041c7f0);
#endif
	GXColor4u8(0, 0, 0, entry->color.a);

	for (i = 0; i < 0x1F; i++) {
		f32 angle;
		f32 c;
		f32 s;

		if (i == 0x1E) {
			angle = float_0_8041c7f0;
		} else {
			angle = float_6p2832_8041c824 * (f32)(-i) / float_30_8041c828;
		}
		s = sin(angle);
		c = cos(angle);
#ifdef MUST_MATCH
		GXWGFifo.f32 = c;
		GXWGFifo.f32 = s;
		GXWGFifo.f32 = MATCH_F32(float_neg1_8041c818);
#else
		GXPosition3f32(c, s, float_neg1_8041c818);
#endif
		GXColor4u8(0, 0, 0, entry->color.a / 3);
	}
}

//drawCone

// TODO: get this to not inline into batSpotInit
#pragma dont_inline on
BattleSpotEntry* batSpotEntry(void) {
	int i;
	BattleSpotEntry* entry = wp->entries;
	const Vec* g = vec3_802f31a8;

	for (i = 0; i < wp->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			break;
		}
	}

	entry->pos = g[7];
	entry->scale = g[8];
	entry->rot = g[9];
	entry->color = *(GXColor*)&unk_8042302c;
	entry->flags |= 1;

	return entry;
}

// =====================================================================
// New work appended below.
// =====================================================================


static ScreenWork* screen_wp;

// dont_inline: without it the compiler inlines randf into tou_gamen_screen_tev
// (the original leaves it as a call).
f32 randf(f32 min, f32 max) {
	f32 r = (f32)rand() / float_32767_8041c81c;
	f32 d = max - min;
	return r * d + min;
}
#pragma dont_inline reset

static void screen_capture(CameraId camId, void* param) {
	GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
	GXSetTexCopyDst(0x130, 0xF0, 4, 1);
	GXCopyTex(screen_wp->texBuffer, 0);
	GXPixModeSync();
	screen_wp->captured = 1;
}

s32 screen_evt_func(void) {
	dispEntry(8, float_1000_8041c814, 0, screen_capture, NULL);
	return 0;
}

// float_neg1 is const so MWCC hoists its single load to the top of the function,
// where the original loads it, instead of just before the field_0x28 store.
s32 tou_gamen_screen_tev_init(EventEntry* event) {
	u32 size;

	size = GXGetTexBufferSize(0x260, 0x1E0, 0, 0, 0);
	screen_wp = _mapAlloc(sizeof(ScreenWork));
	screen_wp->texBuffer = _mapAlloc(size);
	{
		screen_wp->captured = 0;
		screen_wp->scaleTarget = float_1_8041c7f4;
		screen_wp->scale = float_1_8041c7f4;
		screen_wp->offsetXTarget = float_0_8041c7f0;
		screen_wp->offsetX = float_0_8041c7f0;
		screen_wp->offsetYTarget = float_0_8041c7f0;
		screen_wp->offsetY = float_0_8041c7f0;
		screen_wp->tick = 0;
		screen_wp->field_0x28 = float_neg1_8041c818;
	}

	if (evtGetValue(event, GSW(0)) == 0xA3) {
		screen_wp->noiseMode = 1;
	}

	evtEntry(screen_evt, 0, 0);
	return 2;
}

// The original keeps i/4 (r31) and i%4 (r30) live across the rand() calls. The
// second loop's `sgrow`/`sgcol` are precomputed with shifts before the count loop so
// they land in the callee-saved registers the original uses. The first loop assigns
// `base = i / 4` and `col = i % 4` inside the loop and shifts them in the sum: the
// frontend hoists only the divisions, so the shifts stay loop-body code until the
// backend hoists them, which gives the original's register colouring (the same body
// as tou2/tou_03's screen_tev, whose original is identical). With opt_propagation off
// the base/col add into the byte index as the original does instead of folding into a
// buf+col pointer. The min-clamp's `x = (x > ratio) ? ratio : x;` reproduces the
// original's two-branch select. The `zero` alias and the function-scope counters fix the
// pointer web's vreg, and the split idx2 temporaries keep the running index in its own web.
#pragma opt_propagation off // needed: with propagation on the second loop folds col into a buf+col pointer (see above)
void tou_gamen_screen_tev(void* param) {
	GXTexObj texObj;
	Mtx mtx;
	Mtx mtxScale;
	Mtx mtxTrans;
#ifdef MUST_MATCH
	// MUST_MATCH: hoisting the first loop's counters at function scope makes the
	// alias web below the vreg where the original's parameter (r27) sits, and the
	// `+ zero` keeps that alias alive so it colours as a named web instead of folding
	// back into the parameter.
	u8* buf;
	s32 i;
	s32 j;
	s32 k;
	s32 zero = 0;
	MapTevCallback* tev = (MapTevCallback*)((u8*)param + zero);
#else
	// Natural form: the counters live in the block that uses them and the pointer is
	// the parameter itself.
	MapTevCallback* tev = (MapTevCallback*)param;
#endif

	if (screen_wp->captured == 0) {
		return;
	}

	if (screen_wp->noiseMode != 0) {
#ifdef MUST_MATCH
		buf = (u8*)screen_wp->texBuffer;
#else
		u8* buf = (u8*)screen_wp->texBuffer;
		s32 i;
		s32 j;
		s32 k;
#endif

		for (i = 0; i < 0x40; i++) {
			for (j = 0; j < 0x40; j++) {
				int base, row, col, mask, pct, idx;
				row = j / 8;
				pct = j % 8;
				mask = (i & 1) ? 0xFF : 0;
				base = i / 4;
				col = i % 4;
				idx = row * 0x20 + base * 0x100;
				idx += pct;
				idx += col * 8;
				buf[idx] = mask;
			}
			{
				s32 count = 100 + rand() % 100;
				// sgcol is declared before sgrow but assigned after it, giving sgcol the higher vreg the
				// original uses while sgrow is still computed first in program order.
				s32 sgcol;
				s32 sgrow;
				sgrow = (i / 4) << 8;
				sgcol = (i % 4) << 3;
				for (k = 0; k < count; k++) {
					s32 len = rand() % 4 + 2;
					s32 start = rand() % (0x41 - len);
					u8 val = rand() % 0x100;
					for (j = start; j < start + len; j++) {
						// The split idx2 temporary keeps the running index in its own web.
						s32 idx;
						s32 col;
						s32 idx2;
						idx = (j / 8) * 0x20;
						col = sgcol;
						idx += sgrow;
						idx2 = idx + (j % 8);
						buf[idx2 + col] = val;
					}
				}
			}
		}
		DCFlushRange(buf, 0x1000);
		GXInitTexObj(&texObj, buf, 0x40, 0x40, 1, 1, 1, 0);
		GXInitTexObjLOD(&texObj, 0, 0, float_0_8041c7f0, float_0_8041c7f0, float_0_8041c7f0, 0, 0, 0);
		GXLoadTexObj(&texObj, 0);
	} else {
		GXInitTexObj(&texObj, screen_wp->texBuffer, 0x130, 0xF0, 4, 0, 0, 0);
		GXInitTexObjLOD(&texObj, 0, 0, float_0_8041c7f0, float_0_8041c7f0, float_0_8041c7f0, 0, 0, 0);
	}
	GXLoadTexObj(&texObj, 0);

	GXInitTexObj(&texObj, inta_tex, 4, 8, 0, 1, 1, 0);
	GXInitTexObjLOD(&texObj, 1, 1, float_0_8041c7f0, float_0_8041c7f0, float_0_8041c7f0, 0, 0, 0);
	GXLoadTexObj(&texObj, 1);

	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 7);
	GXSetTevOrder(1, 1, 1, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 8, 0, 0xF);
	GXSetTevAlphaIn(1, 7, 7, 7, 6);

	if (screen_wp->noiseMode == 0) {
		screen_wp->tick++;
		if (screen_wp->tick % 200 == 0) {
			switch (rand() % 3) {
			case 0:
				screen_wp->offsetXTarget = randf(float_0_8041c7f0, (float_1_8041c7f4 - screen_wp->scale) / screen_wp->scale);
				screen_wp->offsetYTarget = randf(float_0_8041c7f0, (float_1_8041c7f4 - screen_wp->scale) / screen_wp->scale);
				break;
			case 1:
				screen_wp->scaleTarget = randf(float_1_8041c7f4, float_0p25_8041c7f8);
				break;
			case 2:
				break;
			}
		}
		screen_wp->scale += (screen_wp->scaleTarget - screen_wp->scale) / float_60_8041c7fc;
		screen_wp->offsetX += (screen_wp->offsetXTarget - screen_wp->offsetX) / float_60_8041c7fc;
		screen_wp->offsetY += (screen_wp->offsetYTarget - screen_wp->offsetY) / float_60_8041c7fc;
		PSMTXScale(mtxScale, screen_wp->scale, screen_wp->scale, float_1_8041c7f4);
		{
			f32 x;
			f32 ratio = (float_1_8041c7f4 - screen_wp->scale) / screen_wp->scale;
			f32 y = screen_wp->offsetY;
			if (y > ratio) {
				y = ratio;
			}
			{
				x = screen_wp->offsetX;
				x = (x > ratio) ? ratio : x;
				PSMTXTrans(mtxTrans, x, y, float_0_8041c7f0);
			}
		}
		PSMTXConcat(mtxScale, mtxTrans, mtx);
	} else {
		PSMTXScale(mtx, float_4p75_8041c800, float_3p75_8041c804, float_0_8041c7f0);
	}

	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXLoadTexMtxImm(mtx, 0x1E, 1);
	{
		CameraEntry* cam = camGetPtr(4);
		PSMTXScale(mtx, float_1_8041c7f4, float_6_8041c80c * ((float_100_8041c810 - cam->cameraPos.z) / float_1000_8041c814) + float_16_8041c808, float_1_8041c7f4);
	}
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	GXLoadTexMtxImm(mtx, 0x21, 1);

	tev->tevStage = 2;
	tev->texCoord = 2;
	tev->texMap = 2;
}
#pragma opt_propagation reset

// .sdata2 objects owned by this unit, in the original's order. Defined after the
// code so MWCC emits them before its own pooled literals. dat_8041c7e8's 0xFF
// word is the compiler's pool entry for the colour literal in
// BattleStageLightSetLightColor, so it is not defined here as a separate object.
__declspec(section ".sdata2") s32 dat_8041c7ec = -1;
#pragma explicit_zero_data on
__declspec(section ".sdata2") const f32 float_0_8041c7f0 = 0.0f;
#pragma explicit_zero_data reset
__declspec(section ".sdata2") const f32 float_1_8041c7f4 = 1.0f;
__declspec(section ".sdata2") const f32 float_0p25_8041c7f8 = 0.25f;
__declspec(section ".sdata2") const f32 float_60_8041c7fc = 60.f;
__declspec(section ".sdata2") const f32 float_4p75_8041c800 = 4.75f;
__declspec(section ".sdata2") const f32 float_3p75_8041c804 = 3.75f;
__declspec(section ".sdata2") const f32 float_16_8041c808 = 16.0f;
__declspec(section ".sdata2") const f32 float_6_8041c80c = 6.0f;
__declspec(section ".sdata2") const f32 float_100_8041c810 = 100.f;
__declspec(section ".sdata2") const f32 float_1000_8041c814 = 1000.f;
__declspec(section ".sdata2") const f32 float_neg1_8041c818 = -1.0f;
__declspec(section ".sdata2") f32 float_32767_8041c81c = 32767.0f;
__declspec(section ".sdata2") f32 float_deg2rad_8041c820 = 0.017453292f;
__declspec(section ".sdata2") f32 float_6p2832_8041c824 = 6.2831855f;
__declspec(section ".sdata2") f32 float_30_8041c828 = 30.f;
__declspec(section ".sdata2") f32 float_neg1000_8041c82c = -1000.f;
__declspec(section ".sdata2") const f32 float_neg100_8041c830 = -100.f;
__declspec(section ".sdata2") const f32 float_50_8041c834 = 50.f;
__declspec(section ".sdata2") f32 float_2_8041c838 = 2.0f;
__declspec(section ".sdata2") f32 float_0p34907_8041c83c = 0.34906584f;
__declspec(section ".sdata2") f32 float_neg640_8041c840 = -640.f;
__declspec(section ".sdata2") f32 float_640_8041c844 = 640.f;
__declspec(section ".sdata2") f32 float_105_8041c848 = 105.f;
__declspec(section ".sdata2") f32 float_5000_8041c84c = 5000.f;
__declspec(section ".sdata2") f32 float_6000_8041c850 = 6000.f;
__declspec(section ".sdata2") const f32 float_290_8041c854 = 290.f;
__declspec(section ".sdata2") const f32 float_200_8041c858 = 200.f;
__declspec(section ".sdata2") const f32 float_90_8041c85c = 90.f;
__declspec(section ".sdata2") const f32 float_neg50_8041c860 = -50.f;
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") u32 gap_09_8041C864_sdata2 = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
