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
#include "manager/evtmgr_cmd.h"
#include <stdlib.h>
static void screen_capture(CameraId camId, void* param);  // local to this file in the original

//.sbss
static BattleSpotWork work;

//.sbss2
s32 batSpotEntry_28;

//.sdata
static BattleSpotWork* wp = &work;

//.sdata (local to this file in the original)
static u32 inta_tex[8] = {0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000};

//.sdata2
s32 dat_8041c7ec = -1;

// .rodata zero/constant vectors this unit emitted; MWCC pools every access
// through the vec3_802f31a8 base, and the int->float conversion double the
// compiler emits lands after them at +0x78 (see batSpotInit).
const Vec vec3_802f31a8[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31b4[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31c0[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31cc[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31d8[] = {{0.0f, 0.0f, -50.0f}};
const Vec vec3_802f31e4[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f31f0[] = {{20.0f, 20.0f, 150.0f}};
const Vec vec3_802f31fc[] = {{0.0f, 0.0f, 0.0f}};
const Vec vec3_802f3208[] = {{1.0f, 1.0f, 1.0f}};
const Vec vec3_802f3214[] = {{0.0f, 0.0f, 0.0f}};
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
extern f32 float_neg1_8041c818;
extern f32 float_32767_8041c81c;
extern f32 float_5000_8041c84c;
extern f32 float_6000_8041c850;
extern u32 unk_80423018;
extern u32 unk_8042301c;
extern u32 unk_80423028;
extern f32 float_deg2rad_8041c820;
extern f32 float_6p2832_8041c824;
extern f32 float_30_8041c828;
extern f32 float_neg640_8041c840;
extern f32 float_640_8041c844;
extern f32 float_105_8041c848;
extern f32 float_2_8041c838;
extern f32 float_0p34907_8041c83c;

typedef struct BattleStageDark {
	u32 field_0;  //0x0
	u8 field_4;   //0x4
	u8 field_5;   //0x5
	u8 field_6;   //0x6
	u8 field_7;   //0x7
	u8 field_8;   //0x8
	u8 field_9;   //0x9
	u8 pad_A[2];  //0xA
	s32 field_C;  //0xC
	s32 field_10; //0x10
	s32 field_14; //0x14
	s32 field_18; //0x18
	s8 field_1C;  //0x1C
	s8 field_1D;  //0x1D
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
	s8 colorEase;           //0x38
	u8 pad_39[3];           //0x39
	f32 widthCur;           //0x3C
	f32 widthFrom;          //0x40
	f32 widthTo;            //0x44
	s32 widthFrames;        //0x48
	s32 widthFramesTotal;   //0x4C
	s8 widthEase;           //0x50
	u8 pad_51[3];           //0x51
	Vec posCur;             //0x54
	Vec posFrom;            //0x60
	Vec posTo;              //0x6C
	s32 posFrames;          //0x78
	s32 posFramesTotal;     //0x7C
	s8 posEase;             //0x80
	u8 pad_81[3];           //0x81
	Vec targetCur;          //0x84
	Vec targetFrom;         //0x90
	Vec targetTo;           //0x9C
	s32 targetFrames;       //0xA8
	s32 targetFramesTotal;  //0xAC
	s8 targetEase;          //0xB0
	u8 pad_B1[3];           //0xB1
	Vec upCur;              //0xB4
	Vec upFrom;             //0xC0
	Vec upTo;               //0xCC
	s32 upFrames;           //0xD8
	s32 upFramesTotal;      //0xDC
	s8 upEase;              //0xE0
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

BattleStage* BattleStageGetPtr(void) {
    return &_battleWorkPointer->stage;
}

// The four .sda2 floats are `const` in the header so MWCC hoists all four lfs,
// matching the B24/B38 stores. The light loop indexes `stage->lights[i]` (not a
// raw byte offset) so the compiler strength-reduces to the original's offset
// register and reuses its zero for the stored `NULL`.
void BattleStageInit(void) {
	s32 i;
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageLight* light;
	memset(stage, 0, 0xB3C);
	stage->field_0 = 0;
	for (i = 0; i < 12; i++) {
		light = &stage->lights[i];
		light->flags = 0;
		light->spline = NULL;
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

	stage->field_B24 = float_290_8041c854;
	stage->field_B28 = float_200_8041c858;
	stage->field_B2C = float_90_8041c85c;
	stage->field_B30 = stage->field_B24;
	stage->field_B34 = float_neg50_8041c860;
	stage->field_B38 = float_200_8041c858;
	BattleStageObjectInit();
	batSpotInit();
}

// The original translation unit called intplGetValue through a prototype listing the two float
// bounds before the step/steps words, so MWCC evaluates the two int->float conversions first and
// colours them r0/r4 as retail does; the ABI is unchanged (ints r3-r5, floats f1-f2).
static inline f32 intplGetValueMain(s32 mode, f32 start, f32 end, s32 currStep, s32 steps) {
#ifdef MUST_MATCH
	return ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(mode, start, end, currStep, steps);
#else
	return intplGetValue(mode, currStep, steps, start, end);
#endif
}

void BattleStageMain(void) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	s32 offset;
	s32 i;
	BattleStageLight* light;

	batSpotMain();

	i = 0;
	offset = 0;
	for (; i < 12; i++, offset += 0xe8) {
		light = (BattleStageLight*)((u8*)stage + offset + 4);

		if (light->flags & 1) {
			if (light->colorFrames > 0) {
				light->colorFrames--;
				light->colorCur.r = intplGetValueMain(light->colorEase,
					light->colorFrom.r, light->colorTo.r,
					light->colorFramesTotal - light->colorFrames, light->colorFramesTotal);
				light->colorCur.g = intplGetValueMain(light->colorEase,
					light->colorFrom.g, light->colorTo.g,
					light->colorFramesTotal - light->colorFrames, light->colorFramesTotal);
				light->colorCur.b = intplGetValueMain(light->colorEase,
					light->colorFrom.b, light->colorTo.b,
					light->colorFramesTotal - light->colorFrames, light->colorFramesTotal);
				if ((light->flags & 2) && light->colorCur.r == 0 && light->colorCur.g == 0 && light->colorCur.b == 0) {
					light->flags = 0;
					continue;
				}
			}

			if (light->widthFrames > 0) {
				light->widthFrames--;
				light->widthCur = intplGetValueMain(light->widthEase,
					light->widthFrom, light->widthTo,
					light->widthFramesTotal - light->widthFrames, light->widthFramesTotal);
			}

			if (light->posFrames > 0) {
				light->posFrames--;
				light->posCur.x = intplGetValueMain(light->posEase,
					light->posFrom.x, light->posTo.x,
					light->posFramesTotal - light->posFrames, light->posFramesTotal);
				light->posCur.y = intplGetValueMain(light->posEase,
					light->posFrom.y, light->posTo.y,
					light->posFramesTotal - light->posFrames, light->posFramesTotal);
				light->posCur.z = intplGetValueMain(light->posEase,
					light->posFrom.z, light->posTo.z,
					light->posFramesTotal - light->posFrames, light->posFramesTotal);
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
					light->targetCur.x = intplGetValueMain(light->targetEase,
						light->targetFrom.x, light->targetTo.x,
						light->targetFramesTotal - light->targetFrames, light->targetFramesTotal);
					light->targetCur.y = intplGetValueMain(light->targetEase,
						light->targetFrom.y, light->targetTo.y,
						light->targetFramesTotal - light->targetFrames, light->targetFramesTotal);
					light->targetCur.z = intplGetValueMain(light->targetEase,
						light->targetFrom.z, light->targetTo.z,
						light->targetFramesTotal - light->targetFrames, light->targetFramesTotal);
				}
				if (light->spline != NULL) {
					__memFree(HEAP_BATTLE, light->spline);
					light->spline = NULL;
				}
			}

			if (light->upFrames > 0) {
				light->upFrames--;
				light->upCur.x = intplGetValueMain(light->upEase,
					light->upFrom.x, light->upTo.x,
					light->upFramesTotal - light->upFrames, light->upFramesTotal);
				light->upCur.y = intplGetValueMain(light->upEase,
					light->upFrom.y, light->upTo.y,
					light->upFramesTotal - light->upFrames, light->upFramesTotal);
				light->upCur.z = intplGetValueMain(light->upEase,
					light->upFrom.z, light->upTo.z,
					light->upFramesTotal - light->upFrames, light->upFramesTotal);
			}
		} else {
			if (light->spline != NULL) {
				__memFree(HEAP_BATTLE, light->spline);
				light->spline = NULL;
			}
		}
	}

	if (stage->stgDark.field_C > 0) {
		stage->stgDark.field_C--;
		stage->stgDark.field_4 = intplGetValueMain(stage->stgDark.field_1C,
			stage->stgDark.field_6, stage->stgDark.field_8,
			stage->stgDark.field_14 - stage->stgDark.field_C, stage->stgDark.field_14);
	}
	if (stage->stgDark.field_10 > 0) {
		stage->stgDark.field_10--;
		stage->stgDark.field_5 = intplGetValueMain(stage->stgDark.field_1D,
			stage->stgDark.field_7, stage->stgDark.field_9,
			stage->stgDark.field_18 - stage->stgDark.field_10, stage->stgDark.field_18);
	}
	if (stage->audDark.field_C > 0) {
		stage->audDark.field_C--;
		stage->audDark.field_4 = intplGetValueMain(stage->audDark.field_1C,
			stage->audDark.field_6, stage->audDark.field_8,
			stage->audDark.field_14 - stage->audDark.field_C, stage->audDark.field_14);
	}
	if (stage->audDark.field_10 > 0) {
		stage->audDark.field_10--;
		stage->audDark.field_5 = intplGetValueMain(stage->audDark.field_1D,
			stage->audDark.field_7, stage->audDark.field_9,
			stage->audDark.field_18 - stage->audDark.field_10, stage->audDark.field_18);
	}
}

void BattleStageEnd(void) {
	s32 i;
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageLight* light;

	for (i = 0; i < 12; i++) {
		light = &stage->lights[i];
		if (light->spline != NULL) {
			__memFree(HEAP_BATTLE, light->spline);
			light->spline = NULL;
		}
	}
}

BattleSpotWork* BatSpotGetPtr(void) {
    return wp;
}

#pragma dont_inline on
// The .rodata is now this unit's own (the vec3 block plus the two conversion
// doubles at +0x78/+0x80; see the definitions near the top). The only
// difference left is the int->float conversion double: the original folds it
// as `lfd f30, 0x78(r31)` off the vec3 base, ours emits its own
// `lis @567@ha; lfd @567@l(r3)`. Both are at the same .rodata offset; MWCC
// picks a separate base for the anonymous literal, and defining the double by
// name made no difference.
// NONMATCHING (96.08%): int->float conversion double uses its own base instead of vec3's.
void batSpotInit(void) {
	const Vec* g = vec3_802f31a8;
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
		entry->field_28 = dat_8041c7ec;
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
	const Vec* g = vec3_802f31a8;

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

// dont_inline: without it the compiler inlines randf into tou_gamen_screen_tev
// (the original leaves it as a call).
#pragma dont_inline on
f32 randf(f32 min, f32 max) {
	f32 r = (f32)rand() / float_32767_8041c81c;
	f32 d = max - min;
	return r * d + min;
}
#pragma dont_inline reset

s32 screen_evt_func(void) {
	dispEntry(8, 1000.0f, 0, screen_capture, NULL);
	return 0;
}

static void screen_capture(CameraId camId, void* param) {
	GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
	GXSetTexCopyDst(0x130, 0xF0, 4, 1);
	GXCopyTex(screen_wp->field_0x0, 0);
	GXPixModeSync();
	screen_wp->field_0x4 = 1;
}

// The named float constants are declared extern but never defined in this unit
// (their data is the original's .sdata2 pool), and going through the names makes
// MWCC reload/hoist them in a different order. Writing the values as literals lets
// it pool them as the original does and matches the schedule byte-for-byte.
s32 tou_gamen_screen_tev_init(EventEntry* event) {
	u32 size;

	size = GXGetTexBufferSize(0x260, 0x1E0, 0, 0, 0);
	screen_wp = _mapAlloc(0x2C);
	screen_wp->field_0x0 = _mapAlloc(size);
	{
		screen_wp->field_0x4 = 0;
		screen_wp->field_0xC = 1.0f;
		screen_wp->field_0x8 = 1.0f;
		screen_wp->field_0x14 = 0.0f;
		screen_wp->field_0x10 = 0.0f;
		screen_wp->field_0x1C = 0.0f;
		screen_wp->field_0x18 = 0.0f;
		screen_wp->field_0x20 = 0;
		screen_wp->field_0x28 = -1.0f;
	}

	if (evtGetValue(event, GSW(0)) == 0xA3) {
		screen_wp->field_0x24 = 1;
	}

	evtEntry(screen_evt, 0, 0);
	return 2;
}

// Event param struct: the original passes the event pointer as a typed struct
// with 32-bit fields at 0xC/0x10/0x14 (same shape as battle/sac/sac_suki.c's
// SukiTevParam).
typedef struct ScreenTevParam {
	u8 pad[0xC];
	s32 field_C;  // 0xC
	s32 field_10; // 0x10
	s32 field_14; // 0x14
} ScreenTevParam;

// The original keeps i/4 (r31) and i%4 (r30) live across the rand() calls and
// recomputes the scaled terms in each inner loop. The second loop's `sgrow`/`sgcol`
// are precomputed with shifts before the count loop so they land in the callee-saved
// registers the original uses; the first loop keeps the `(i/4)*0x100`/`(i%4)*8`
// multiplies (using shifts there rotates the whole loop's register assignment) and
// accumulates into `idx` (`idx = (i/4)*0x100; idx += (j/8)*0x20; ...`) which is what
// makes the first loop's schedule/registers come out. `buf` is declared first for the
// same reason. With opt_propagation off the base/col add into the byte index as the
// original does instead of folding into a buf+col pointer. The min-clamp's `x =
// (ratio > x) ? x : ratio;` reproduces the original's two-branch select. The remaining
// differences are only integer and float register allocation: param lands in r25
// (original r27), the second loop's scaled row in r27 (original r25), the first
// loop's base/col in r7/r8 (original r4/r3), the first loop schedules its (j%8)
// before (j/8)*0x20 in the target (reversed here), and the clamp ratio sits in f0
// (original f1); instructions and structure are otherwise identical. Many variants
// were tried (explicit `base`/`col` locals, shifts vs multiplies, sgrow/sgcol
// declaration order, opt_lifetimes off, pragma sweeps, reordering the local
// declarations) and none beat this: the allocator's colouring of the {param, sgrow,
// sgcol} clique could not be steered from C.
// MATCH-ONLY TRICK: moving the `buf[idx]` ternary into a function-scope `mask` temp
// assigned between the `(j/8)*0x20` and `(j%8)` additions nudged the first loop's
// colouring (99.36% -> 99.39%; found by the decomp-permuter).
// NONMATCHING (99.39%): integer and float register allocation only.
#pragma opt_propagation off // needed: with propagation on the second loop folds col into a buf+col pointer (see above)
void tou_gamen_screen_tev(void* param) {
	int mask;
	GXTexObj texObj;
	Mtx mtx;
	Mtx mtxScale;
	Mtx mtxTrans;
	ScreenTevParam* tev = (ScreenTevParam*)param;

	if (screen_wp->field_0x4 == 0) {
		return;
	}

	if (screen_wp->field_0x24 != 0) {
		u8* buf = (u8*)screen_wp->field_0x0;
		s32 i;
		s32 j;
		s32 k;

		for (i = 0; i < 0x40; i++) {
			for (j = 0; j < 0x40; j++) {
				int idx = (i / 4) * 0x100;
				idx += (j / 8) * 0x20;
				mask = (i & 1) ? 0xFF : 0;
				idx += (j % 8);
				idx += (i % 4) * 8;
				buf[idx] = mask;
			}
			{
				s32 count = 100 + rand() % 100;
				s32 sgrow = (i / 4) << 8;
				s32 sgcol = (i % 4) << 3;
				for (k = 0; k < count; k++) {
					s32 len = rand() % 4 + 2;
					s32 start = rand() % (0x41 - len);
					u8 val = rand() % 0x100;
					for (j = start; j < start + len; j++) {
						s32 idx = (j / 8) * 0x20;
						s32 col = sgcol;
						idx += sgrow;
						idx += (j % 8);
						buf[idx + col] = val;
					}
				}
			}
		}
		DCFlushRange(buf, 0x1000);
		GXInitTexObj(&texObj, buf, 0x40, 0x40, 1, 1, 1, 0);
		GXInitTexObjLOD(&texObj, 0, 0, float_0_8041c7f0, float_0_8041c7f0, float_0_8041c7f0, 0, 0, 0);
		GXLoadTexObj(&texObj, 0);
	} else {
		GXInitTexObj(&texObj, screen_wp->field_0x0, 0x130, 0xF0, 4, 0, 0, 0);
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

	if (screen_wp->field_0x24 == 0) {
		screen_wp->field_0x20++;
		if (screen_wp->field_0x20 % 200 == 0) {
			switch (rand() % 3) {
			case 0:
				screen_wp->field_0x14 = randf(float_0_8041c7f0, (float_1_8041c7f4 - screen_wp->field_0x8) / screen_wp->field_0x8);
				screen_wp->field_0x1C = randf(float_0_8041c7f0, (float_1_8041c7f4 - screen_wp->field_0x8) / screen_wp->field_0x8);
				break;
			case 1:
				screen_wp->field_0xC = randf(float_1_8041c7f4, float_0p25_8041c7f8);
				break;
			case 2:
				break;
			}
		}
		screen_wp->field_0x8 += (screen_wp->field_0xC - screen_wp->field_0x8) / float_60_8041c7fc;
		screen_wp->field_0x10 += (screen_wp->field_0x14 - screen_wp->field_0x10) / float_60_8041c7fc;
		screen_wp->field_0x18 += (screen_wp->field_0x1C - screen_wp->field_0x18) / float_60_8041c7fc;
		PSMTXScale(mtxScale, screen_wp->field_0x8, screen_wp->field_0x8, float_1_8041c7f4);
		{
			f32 ratio = (float_1_8041c7f4 - screen_wp->field_0x8) / screen_wp->field_0x8;
			f32 y = screen_wp->field_0x18;
			if (y > ratio) {
				y = ratio;
			}
			{
				f32 x = screen_wp->field_0x10;
				x = (ratio > x) ? x : ratio;
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

	tev->field_C = 2;
	tev->field_14 = 2;
	tev->field_10 = 2;
}
#pragma opt_propagation reset





#pragma dont_inline on
void BattleStageSetDark(BattleStageDark* dark, u8 a, s32 frames, s32 ease) {
	dark->field_6 = dark->field_4;
	dark->field_8 = a;
	dark->field_C = frames;
	dark->field_14 = frames;
	dark->field_1C = ease;
	if (frames == 0) {
		dark->field_4 = a;
	}
}

void BattleStageSetDarkBase(BattleStageDark* dark, u8 a, s32 frames, s32 ease) {
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

void BattleStageSetStgDark(u8 a, s32 frames, s32 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDark(&stage->stgDark, a, frames, ease);
}

void BattleStageSetStgDarkBase(u8 a, s32 frames, s32 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDarkBase(&stage->stgDark, a, frames, ease);
}

void BattleStageSetAudDark(u8 a, s32 frames, s32 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDark(&stage->audDark, a, frames, ease);
}

void BattleStageSetAudDarkBase(u8 a, s32 frames, s32 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageSetDarkBase(&stage->audDark, a, frames, ease);
}

void BattleStageReturnStgDarkBase(s32 frames, s32 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageDark* dark = &stage->stgDark;
	BattleStageSetDark(dark, dark->field_5, frames, ease);
}

void BattleStageReturnAudDarkBase(s32 frames, s32 ease) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
	BattleStageDark* dark = &stage->audDark;
	BattleStageSetDark(dark, dark->field_5, frames, ease);
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

// dont_inline: without it the compiler inlines getSpline into BattleStageMain
#pragma dont_inline on
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
#pragma dont_inline reset

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

// Copy propagation off: without it BattleStageLightSetTarget stops matching.
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

// Copy propagation off: without it BattleStageLightSetPosition stops matching.
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

// Copy propagation off: without it BattleStageLightSetWidth stops matching.
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

// Copy propagation off: without it BattleStageLightSetLightColor stops matching.
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

// Copy propagation off: without it BattleStageLightRelease stops matching.
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
#pragma dont_inline reset

s32 BattleStageSysLightEntry(const char* name) {
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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

s32 BattleStageGetLightNumberByName(const char* name) {
	s32 offset;
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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


#pragma dont_inline on
void DarkByPoint(u8 color, f32 x, f32 y, f32 z) {
	GXPosition3f32(x, y, z);
	GXColor4u8(0, 0, 0, color);
}
#pragma dont_inline reset

#pragma dont_inline on
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

void BattleStageDisp(void) {
	BattleGetUnitPtr(_battleWorkPointer, _battleWorkPointer->activeUnitId);
	dispEntry(4, float_5000_8041c84c, 2, BattleStageDispDark, NULL);
	dispEntry(4, float_6000_8041c850, 2, BattleStageDispLight, NULL);
}

// Keep drawCone out of line: the original calls it rather than inlining.
#pragma dont_inline on
void drawCone(CameraId camId, BattleSpotEntry* entry) {
	CameraEntry* cam = camGetPtr(camId);
	Mtx mTrans;
	Mtx mScale;
	Mtx mTmp;
	Mtx mRotX;
	Mtx mRotY;
	Mtx mRotZ;
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

	GXPosition3f32(0.0f, 0.0f, 0.0f);
	GXColor4u8(0, 0, 0, ((u8*)entry)[0x2B]);

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
		GXColor4u8(0, 0, 0, ((u8*)entry)[0x2B] / 3);
	}
}
#pragma dont_inline off

// opt_common_subs off: with common subexpression elimination on, MWCC loads the
// shared float constants in the reverse order (f2 before f1). DarkBySquare's
// prototype is (f32..., u8 color) in the original, so the colour argument is
// evaluated last; see the note on its definition.
#pragma opt_common_subs off
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
	{
		BattleStageDark* audDark = &stage->audDark;
		GXSetZMode(1, 6, 0);
		DarkBySquare(float_neg640_8041c840, float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_105_8041c848, audDark->field_4);
		GXSetZMode(1, 1, 0);
		DarkBySquare(float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_neg640_8041c840, float_105_8041c848, audDark->field_4);
	}
	GXSetZMode(1, 1, 0);
	DarkBySquare(float_neg640_8041c840, float_640_8041c844, float_640_8041c844, float_neg640_8041c840, float_105_8041c848, stage->stgDark.field_4);
}

#pragma opt_common_subs reset
void BattleStageDispLight(CameraId camId, void* param) {
	s32 i;
	BattleStageWork* stage = (BattleStageWork*)&_battleWorkPointer->stage;
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
