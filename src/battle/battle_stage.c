#include "battle/battle_stage.h"
#include "battle/battle.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <string.h>
#include <math.h>

//.sbss
static BattleSpotWork work;

//.sbss2
s32 batSpotEntry_28;

//.sdata
static BattleSpotWork* wp = &work;

//.sdata2
s32 batSpotInit_28 = -1;

extern Vec vec3_802f31a8[];
extern Vec vec3_802f31b4[];
extern Vec vec3_802f31c0[];
extern Vec vec3_802f31cc[];
extern f32 float_0_8041c7f0;
extern f32 float_1_8041c7f4;
extern f32 float_1000_8041c814;
extern f32 float_neg1_8041c818;
extern f32 float_32767_8041c81c;
extern f32 float_5000_8041c84c;
extern f32 float_6000_8041c850;
extern f32 float_290_8041c854;
extern f32 float_200_8041c858;
extern f32 float_90_8041c85c;
extern f32 float_neg50_8041c860;
extern u32 unk_80423018;
extern u32 unk_8042301c;
extern u32 unk_80423028;
extern f32 float_deg2rad_8041c820;
extern f32 float_6p2832_8041c824;
extern f32 float_30_8041c828;
extern f32 float_neg640_8041c840;
extern f32 float_640_8041c844;
extern f32 float_105_8041c848;
extern s32 rand(void);

typedef struct BattleStageDark {
	u32 field_0;  //0x0
	u8 field_4;   //0x4
	u8 field_5;   //0x5
	u8 field_6;   //0x6
	u8 field_7;   //0x7
	u8 field_8;   //0x8
	u8 field_9;   //0x9
	u8 pad_A[2];  //0xA
	u32 field_C;  //0xC
	u32 field_10; //0x10
	u32 field_14; //0x14
	u32 field_18; //0x18
	u8 field_1C;  //0x1C
	u8 field_1D;  //0x1D
	u8 pad_1E[2]; //0x1E
} BattleStageDark;

typedef struct BattleStageColor {
	u8 r; //0x0
	u8 g; //0x1
	u8 b; //0x2
	u8 a; //0x3
} BattleStageColor;

typedef struct BattleStageLight {
	u32 flags;              //0x0
	char name[0x20];        //0x4
	BattleStageColor colorCur; //0x24
	BattleStageColor colorFrom; //0x28
	BattleStageColor colorTo;   //0x2C
	s32 colorFrames;        //0x30
	s32 colorFramesTotal;   //0x34
	u8 colorEase;           //0x38
	u8 pad_39[3];           //0x39
	f32 widthCur;           //0x3C
	f32 widthFrom;          //0x40
	f32 widthTo;            //0x44
	s32 widthFrames;        //0x48
	s32 widthFramesTotal;   //0x4C
	u8 widthEase;           //0x50
	u8 pad_51[3];           //0x51
	Vec posCur;             //0x54
	Vec posFrom;            //0x60
	Vec posTo;              //0x6C
	s32 posFrames;          //0x78
	s32 posFramesTotal;     //0x7C
	u8 posEase;             //0x80
	u8 pad_81[3];           //0x81
	Vec targetCur;          //0x84
	Vec targetFrom;         //0x90
	Vec targetTo;           //0x9C
	s32 targetFrames;       //0xA8
	s32 targetFramesTotal;  //0xAC
	u8 targetEase;          //0xB0
	u8 pad_B1[3];           //0xB1
	Vec upCur;              //0xB4
	Vec upFrom;             //0xC0
	Vec upTo;               //0xCC
	s32 upFrames;           //0xD8
	s32 upFramesTotal;      //0xDC
	u8 upEase;              //0xE0
	u8 pad_E1[3];           //0xE1
	void* spline;           //0xE4
} BattleStageLight;

typedef struct BattleStageWork {
	u32 field_0;                 //0x0
	BattleStageLight lights[12]; //0x4
	BattleStageDark stgDark;     //0xAE4
	BattleStageDark audDark;     //0xB04
	f32 field_B24;               //0xB24
	f32 field_B28;               //0xB28
	f32 field_B2C;               //0xB2C
	f32 field_B30;               //0xB30
	f32 field_B34;               //0xB34
	f32 field_B38;               //0xB38
} BattleStageWork;

// forward declarations
void batSpotDisp(CameraId camId, void* param);
void drawCone(CameraId camId, BattleSpotEntry* entry);
BattleSpotEntry* batSpotEntry(void);
void batSpotInit(void);

BattleStage* BattleStageGetPtr(void) {
    return &_battleWorkPointer->stage;
}

#pragma opt_propagation off
void BattleStageInit(void) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	s32 i;

	memset(stage, 0, 0xB3C);
	stage->field_0 = 0;

	{
		s32 offset = 0;
		for (i = 0; i < 12; i++) {
			BattleStageLight* light = (BattleStageLight*)((u8*)stage->lights + offset);
			light->flags = 0;
			light->spline = NULL;
			offset += 0xe8;
		}
	}

	stage->stgDark.field_0 = 1;
	stage->stgDark.field_5 = 0;
	stage->stgDark.field_4 = 0;
	stage->stgDark.field_6 = 0;
	stage->stgDark.field_8 = 0;
	stage->stgDark.field_C = 0;
	stage->stgDark.field_14 = 0;

	stage->audDark.field_0 = 1;
	stage->audDark.field_5 = 0x8C;
	stage->audDark.field_4 = 0x8C;
	stage->audDark.field_6 = 0;
	stage->audDark.field_8 = 0;
	stage->audDark.field_C = 0;
	stage->audDark.field_14 = 0;

	{
		f32 f90 = float_90_8041c85c;
		f32 fn50 = float_neg50_8041c860;
		f32 f200 = float_200_8041c858;
		f32 f290 = float_290_8041c854;
		stage->field_B24 = f290;
		stage->field_B28 = f200;
		stage->field_B2C = f90;
		stage->field_B30 = stage->field_B24;
		stage->field_B34 = fn50;
		stage->field_B38 = f200;
	}

	BattleStageObjectInit();
	batSpotInit();
}
#pragma opt_propagation on

void BattleStageMain(void) {
}

#pragma opt_propagation off
void BattleStageEnd(void) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	s32 i;

	{
		s32 offset = 0;
		for (i = 0; i < 12; i++) {
			BattleStageLight* light = (BattleStageLight*)((u8*)stage->lights + offset);
			if (light->spline != NULL) {
				__memFree(HEAP_BATTLE, light->spline);
				light->spline = NULL;
			}
			offset += 0xe8;
		}
	}
}
#pragma opt_propagation on

BattleSpotWork* BatSpotGetPtr(void) {
    return wp;
}

#pragma dont_inline on
void batSpotInit(void) {
	Vec* g = vec3_802f31a8;
	int i;
	BattleSpotEntry* entry;

	wp->count = 5;
	wp->entries = __memAlloc(HEAP_BATTLE, sizeof(BattleSpotEntry) * wp->count);
	memset(wp->entries, 0, sizeof(BattleSpotEntry) * wp->count);

	for (i = 0; i < 5; i++) {
		Vec v;
		entry = batSpotEntry();
		v = g[4];
		v.x = 50.0f * i + -100.0f;
		entry->field_4 = v;
		entry->field_10 = g[5];
		entry->field_1C = g[6];
		entry->field_28 = batSpotInit_28;
	}
}

void batSpotMain() {
    s32 count = wp->count;

    while (count-- > 0) {}
    if (wp->skipRender > 0) {
        dispEntry(CAMERA_3D_IMAGE, -1000.0f, 0, batSpotDisp, NULL);
    }
}

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
	for (i = 0; i < wp->count; entry++, i++) {
		if ((entry->flags & 1) && (entry->flags & 2)) {
			GXSetCullMode(2);
			GXSetAlphaUpdate(1);
			GXSetColorUpdate(0);
			GXSetZMode(1, 3, 0);
			GXSetBlendMode(0, 4, 5, 0);
			GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
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
			GXSetChanMatColor(4, *(GXColor*)&entry->field_28);
			drawCone(camId, entry);
		}
	}
}

//drawCone

// TODO: get this to not inline into batSpotInit
BattleSpotEntry* batSpotEntry(void) {
	int i;
	BattleSpotEntry* entry = wp->entries;
	Vec* g = vec3_802f31a8;

	for (i = 0; i < wp->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			break;
		}
	}

	entry->field_4 = g[7];
	entry->field_1C = g[8];
	entry->field_10 = g[9];
	entry->field_28 = batSpotEntry_28;
	entry->flags |= 1;

	return entry;
}
#pragma dont_inline reset

// =====================================================================
// New work appended below.
// =====================================================================

typedef struct ScreenWork {
	void* field_0x0;   //0x0 tex buffer
	s32 field_0x4;     //0x4
	f32 field_0x8;     //0x8
	f32 field_0xC;     //0xC
	f32 field_0x10;    //0x10
	f32 field_0x14;    //0x14
	f32 field_0x18;    //0x18
	f32 field_0x1C;    //0x1C
	s32 field_0x20;    //0x20
	s32 field_0x24;    //0x24
	f32 field_0x28;    //0x28
} ScreenWork;

extern ScreenWork* screen_wp;
extern u32 screen_evt[7];
s32 screen_evt_func(void);
void screen_capture(CameraId camId, void* param);

f32 randf(f32 min, f32 max) {
	f32 r = (f32)rand() / float_32767_8041c81c;
	f32 d = max - min;
	return r * d + min;
}

s32 screen_evt_func(void) {
	dispEntry(8, float_1000_8041c814, 0, screen_capture, NULL);
	return 0;
}

void screen_capture(CameraId camId, void* param) {
	GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
	GXSetTexCopyDst(0x130, 0xF0, 4, 1);
	GXCopyTex(screen_wp->field_0x0, 0);
	GXPixModeSync();
	screen_wp->field_0x4 = 1;
}

s32 tou_gamen_screen_tev_init(EventEntry* event) {
	u32 size;

	size = GXGetTexBufferSize(0x260, 0x1E0, 0, 0, 0);
	screen_wp = _mapAlloc(0x2C);
	screen_wp->field_0x0 = _mapAlloc(size);
	{
		f32 negOne = float_neg1_8041c818;
		f32 zero = float_0_8041c7f0;
		f32 one = float_1_8041c7f4;
		screen_wp->field_0x4 = 0;
		screen_wp->field_0xC = one;
		screen_wp->field_0x8 = one;
		screen_wp->field_0x14 = zero;
		screen_wp->field_0x10 = zero;
		screen_wp->field_0x1C = zero;
		screen_wp->field_0x18 = zero;
		screen_wp->field_0x20 = 0;
		screen_wp->field_0x28 = negOne;
	}

	if (evtGetValue(event, GSW(0)) == 0xA3) {
		screen_wp->field_0x24 = 1;
	}

	evtEntry(screen_evt, 0, 0);
	return 2;
}





#pragma dont_inline on
void BattleStageSetDark(BattleStageDark* dark, u8 a, s32 frames, u8 ease) {
	dark->field_6 = dark->field_4;
	dark->field_8 = a;
	dark->field_C = frames;
	dark->field_14 = frames;
	dark->field_1C = ease;
	if (frames == 0) {
		dark->field_4 = a;
	}
}

void BattleStageSetDarkBase(BattleStageDark* dark, u8 a, s32 frames, u8 ease) {
	dark->field_7 = dark->field_5;
	dark->field_9 = a;
	dark->field_10 = frames;
	dark->field_18 = frames;
	dark->field_1D = ease;
	if (frames == 0) {
		dark->field_5 = a;
	}
}
#pragma dont_inline reset

void BattleStageSetStgDark(u8 a, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDark(&stage->stgDark, a, frames, ease);
}

void BattleStageSetStgDarkBase(u8 a, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDarkBase(&stage->stgDark, a, frames, ease);
}

void BattleStageSetAudDark(u8 a, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDark(&stage->audDark, a, frames, ease);
}

void BattleStageSetAudDarkBase(u8 a, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDarkBase(&stage->audDark, a, frames, ease);
}

void BattleStageReturnStgDarkBase(s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageDark* dark = &stage->stgDark;
	BattleStageSetDark(dark, dark->field_5, frames, ease);
}

void BattleStageReturnAudDarkBase(s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageDark* dark = &stage->audDark;
	BattleStageSetDark(dark, dark->field_5, frames, ease);
}

void BattleStageOnLightInCommand(void) {
}

void BattleStageOffLightInCommand(void) {
}

void BattleStageOnLightFriendInCommand(void) {
}

void BattleStageOffLightFriendInCommand(void) {
}

void BattleStageOnLightEnemyInCommand(void) {
}

void BattleStageOffLightEnemyInCommand(void) {
}

f32 getSpline(f32* a, f32* b, f32* c, f32* d, s32* knots, s32 count, s32 value) {
	s32 i;

	for (i = count - 1; i >= 0; i--) {
		if ((f32)(value - knots[i]) >= 0.0f) {
			break;
		}
	}

	{
		f32 t = (f32)(value - knots[i]);
		return a[i] + t * (b[i] + t * (c[i] + t * d[i]));
	}
}

// ------------------------------------------------------------ light setters
#pragma opt_propagation off
void BattleStageLightSetUpVec(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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
#pragma opt_propagation on

#pragma opt_propagation off
#pragma dont_inline on
void BattleStageLightSetTarget(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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
#pragma opt_propagation on

#pragma opt_propagation off
void BattleStageLightSetPosition(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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
#pragma opt_propagation on

#pragma opt_propagation off
void BattleStageLightSetWidth(s32 idx, s32 width, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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
#pragma opt_propagation on

#pragma opt_propagation off
void BattleStageLightSetLightColor(s32 idx, u8 r, u8 g, u8 b, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	light->colorFrom = (BattleStageColor){light->colorCur.r, light->colorCur.g, light->colorCur.b, 0};
	light->colorTo = (BattleStageColor){r, g, b, 0};
	light->colorFrames = frames;
	light->colorFramesTotal = frames;
	light->colorEase = ease;
	if (frames == 0) {
		light->colorCur = (BattleStageColor){light->colorCur.r, light->colorCur.g, light->colorCur.b, 0};
	}
}
#pragma opt_propagation on

#pragma opt_propagation off
void BattleStageLightRelease(s32 idx, s32 frames, u8 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageLight* light = &stage->lights[idx];

	if (light->flags & 1) {
		if (frames == 0) {
			light->flags = 0;
		} else {
			light->flags |= 2;
			light->colorFrom = (BattleStageColor){light->colorCur.r, light->colorCur.g, light->colorCur.b, 0};
			light->colorTo = *(BattleStageColor*)&unk_8042301c;
			light->colorFrames = frames;
			light->colorFramesTotal = frames;
			light->colorEase = ease;
		}
	}
}
#pragma opt_propagation on

#pragma opt_propagation off
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
#pragma opt_propagation on
#pragma dont_inline reset

#pragma opt_propagation off
s32 BattleStageSysLightEntry(const char* name) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	s32 i;
	s32 offset = 0;

	for (i = 0; i <= 6; i++) {
		BattleStageLight* light = (BattleStageLight*)((u8*)stage + (4 + offset));
		if (light->flags & 1) {
			strcmp(light->name, name);
		}
		offset += 0xe8;
	}

	i = 0;
	offset = 0;
	{
		s32 count = 7;
		do {
			BattleStageLight* light = (BattleStageLight*)((u8*)stage + (4 + offset));
			if (!(light->flags & 1)) {
				light->flags = 1;
				strcpy(light->name, name);
				break;
			}
			i++;
			offset += 0xe8;
		} while (--count);
	}

	return i;
}
#pragma opt_propagation on

#pragma opt_propagation off
s32 BattleStageGetLightNumberByName(const char* name) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	s32 i;

	{
		BattleStageLight* lights = stage->lights;
		s32 offset = 0;
		for (i = 0; i < 12; i++) {
			BattleStageLight* light = (BattleStageLight*)((u8*)lights + offset);
			if ((light->flags & 1) && strcmp(light->name, name) == 0) {
				return i;
			}
			offset += 0xe8;
		}
	}

	return -1;
}
#pragma opt_propagation on

void BattleStageDispDark(CameraId camId, void* param);
void BattleStageDispLight(CameraId camId, void* param);

#pragma dont_inline on
void DarkByPoint(u8 color, f32 x, f32 y, f32 z) {
	GXPosition3f32(x, y, z);
	GXColor4u8(0, 0, 0, color);
}
#pragma dont_inline reset

#pragma dont_inline on
void DarkBySquare(u8 color, f32 a, f32 b, f32 c, f32 d, f32 z) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	DarkByPoint(color, a, b, z);
	DarkByPoint(color, c, b, z);
	DarkByPoint(color, c, d, z);
	DarkByPoint(color, a, d, z);
}
#pragma dont_inline reset

void BattleStageDisp(void) {
	BattleGetUnitPtr(_battleWorkPointer, _battleWorkPointer->activeUnitId);
	dispEntry(4, float_5000_8041c84c, 2, BattleStageDispDark, NULL);
	dispEntry(4, float_6000_8041c850, 2, BattleStageDispLight, NULL);
}

#pragma dont_inline on
void drawCone(CameraId camId, BattleSpotEntry* entry) {
	CameraEntry* cam = camGetPtr(camId);
	Mtx mTrans;
	Mtx mScale;
	Mtx mTmp;
	Mtx mRotX;
	Mtx mRotY;
	Mtx mRotZ;
	s32 color;
	s32 i;

	PSMTXTrans(mTrans, entry->field_4.x, entry->field_4.y, entry->field_4.z);
	PSMTXScale(mScale, entry->field_1C.x, entry->field_1C.y, entry->field_1C.z);
	PSMTXRotRad(mRotX, 'x', float_deg2rad_8041c820 * entry->field_10.x);
	PSMTXRotRad(mRotY, 'y', float_deg2rad_8041c820 * entry->field_10.y);
	PSMTXRotRad(mRotZ, 'z', float_deg2rad_8041c820 * entry->field_10.z);
	PSMTXConcat(mRotZ, mRotY, mRotY);
	PSMTXConcat(mRotY, mRotX, mTmp);
	PSMTXConcat(cam->view, mTrans, mTrans);
	PSMTXConcat(mTrans, mTmp, mTmp);
	PSMTXConcat(mTmp, mScale, mTmp);
	GXLoadPosMtxImm(mTmp, 0);
	GXSetCurrentMtx(0);
	GXBegin(0xA0, 0, 0x20);

	color = ((u8*)entry)[0x2B];
	GXPosition3f32(float_0_8041c7f0, float_0_8041c7f0, float_0_8041c7f0);
	GXColor4u8(0, 0, 0, color);

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
		GXPosition3f32(c, s, float_neg1_8041c818);
		GXColor4u8(0, 0, 0, color / 3);
	}
}
#pragma dont_inline off

void BattleStageDispDark(CameraId camId, void* param) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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
	GXSetZMode(1, 6, 0);
	DarkBySquare(stage->audDark.field_4, float_neg640_8041c840, float_640_8041c844, float_neg640_8041c840, float_640_8041c844, float_105_8041c848);
	GXSetZMode(1, 1, 0);
	DarkBySquare(stage->audDark.field_4, float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_neg640_8041c840, float_105_8041c848);
	GXSetZMode(1, 1, 0);
	DarkBySquare(stage->stgDark.field_4, float_neg640_8041c840, float_640_8041c844, float_neg640_8041c840, float_640_8041c844, float_105_8041c848);
}
