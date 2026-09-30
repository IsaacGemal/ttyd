#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "battle/battle_pad.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <string.h>
#include "battle/ac/ac_monosiri_target.h"

// Rectangular work area for the MonosiriTarget action command, at _battleWorkPointer + 0x1F4C.
// No header declares it yet; every field is named from the original's uses.
typedef struct MonosiriTargetWork {
	s32 frame;    // 0x00, counts the interpolation step (can overshoot maxcount)
	s32 maxcount; // 0x04, _getmaxcount()
	Vec hitPos;   // 0x08, target hit position
	Vec curPos;   // 0x14, marker position being interpolated
	Vec startPos; // 0x20, initial marker position
	f32 rotZ;     // 0x2C, rotation about Z, grows by 2 per disp frame
	s32 hitFlag;  // 0x30, non-zero while the marker is within 28 of the start
	s32 counter;  // 0x34, frames spent hit (mod 30 drives the alpha blink)
	u32 sfxId;    // 0x38, psndSFXOn id, -1 when inactive
	s32 timer;    // 0x3C, countdown for the tail states
} MonosiriTargetWork;

#ifdef MUST_MATCH
// The original reads the eleven .rodata vectors through the vec3_80300880 base.
extern const Vec vec3_80300880[11];
#define RODATA_VEC(i, name) (vec3_80300880[i])
#else
extern const Vec vec3_80300880;
extern const Vec vec3_8030088c;
extern const Vec vec3_80300898;
extern const Vec vec3_803008a4;
extern const Vec vec3_803008b0;
extern const Vec vec3_803008bc;
extern const Vec vec3_803008c8;
extern const Vec vec3_803008d4;
extern const Vec vec3_803008e0;
extern const Vec vec3_803008ec;
extern const Vec vec3_803008f8;
#define RODATA_VEC(i, name) (name)
#endif

// .rodata scalars/strings in the original unit.
extern const f64 double_15_80300908;
extern const f64 double_28_80300928;
extern const char str_80300910[];

// .sdata frame table, indexed by BattleActionCommandGetDifficulty().
extern s32 _frame_table[7];

// .sdata2 constants.
extern const GXColor dat_80422058; // 0xFFFFFFFF
extern const GXColor dat_8042205c; // 0x808080FF
extern const GXColor dat_80422060; // 0xFFFFFFFF
extern const GXColor dat_80422064; // 0xFFFFFFFF
extern const GXColor dat_80422068; // 0x808080FF
extern const f32 float_0_8042206c;
extern const f32 float_4_80422070;
extern const f32 float_2_80422074;
extern const f32 float_0p01_80422078;
extern const f32 float_100_8042207c;
extern const f32 float_neg200_80422080;
extern const f32 float_50_80422084;
extern const f32 float_1_80422088;
extern const f32 float_neg300_8042208c;
extern const f32 float_80_80422090;
extern const f32 float_160_80422094;

static void actionCommandDisp(f32 x, f32 y);

// NONMATCHING (67.78%): stack-slot reuse and FPR/GPR roles differ (frame 0x140 vs 0x170).
static void actionCommandDisp(f32 x, f32 y) {
	BattleWork* work = _battleWorkPointer;
	MonosiriTargetWork* mt = (MonosiriTargetWork*)((u8*)work + 0x1F4C);
	Vec screen;
	Vec screen2;
	Vec trans1, trans2, trans3, trans4, trans5;
	Vec scale1, scale2, scale3, scale4, scale5;
	Vec rot1, rot2, rot3, rot4, rot5;
	GXColor color1, color2, color3, color4, color5;
	f32 alpha = float_0_8042206c;

	btlGetScreenPoint(&mt->curPos, &screen);
	trans1 = screen;
	scale1 = RODATA_VEC(0, vec3_80300880);
	rot1 = RODATA_VEC(1, vec3_8030088c);
	color1 = dat_80422058;
	btlDispTex4(0x42, &trans1, &scale1, &rot1, &color1);

	screen.x += float_4_80422070;
	screen.y -= float_4_80422070;
	screen.z -= float_2_80422074;
	trans2 = screen;
	scale2 = RODATA_VEC(2, vec3_80300898);
	rot2 = RODATA_VEC(3, vec3_803008a4);
	color2 = dat_8042205c;
	btlDispTex4(0x42, &trans2, &scale2, &rot2, &color2);

	if (mt->hitFlag != 0) {
		s32 rem = mt->counter % 0x1E;
		if (rem < 0x14) {
			alpha = float_0p01_80422078 *
			        intplGetValue(5, rem, 0x14, float_100_8042207c, float_0_8042206c);
		}
	}

	btlGetScreenPoint(&mt->startPos, &screen2);

	trans3 = screen2;
	scale3 = RODATA_VEC(4, vec3_803008b0);
	rot3 = RODATA_VEC(5, vec3_803008bc);
	rot3.z = mt->rotZ;
	color3 = dat_80422060;
	btlDispTex4(0x41, &trans3, &scale3, &rot3, &color3);

	if (alpha > float_0_8042206c) {
		trans4 = screen2;
		scale4 = RODATA_VEC(6, vec3_803008c8);
		scale4.x = alpha;
		scale4.y = alpha;
		rot4 = RODATA_VEC(7, vec3_803008d4);
		rot4.z = mt->rotZ;
		color4 = dat_80422064;
		btlDispTex4(0x41, &trans4, &scale4, &rot4, &color4);
	}

	screen2.x += float_4_80422070;
	screen2.y -= float_4_80422070;
	screen2.z -= float_2_80422074;
	trans5 = screen2;
	scale5 = RODATA_VEC(8, vec3_803008e0);
	rot5 = RODATA_VEC(9, vec3_803008ec);
	rot5.z = mt->rotZ;
	color5 = dat_80422068;
	btlDispTex4(0x41, &trans5, &scale5, &rot5, &color5);

	{
		s32 pressed = BattleACGetButtonIcon(0x100, 1);
		s32 icon = BattleACGetButtonIcon(0x100, 0);
		Vec v;

		mt->rotZ += float_2_80422074;
		if (fabs(mt->curPos.x - mt->startPos.x) < double_15_80300908) {
			icon = pressed;
		}
		v = RODATA_VEC(10, vec3_803008f8);
		v.x = float_neg200_80422080 + x;
		v.y = float_50_80422084 + y;
		iconDispGx(&v, 0x10, icon, float_1_80422088);
	}
}

void battleAcDelete_MonosiriTarget(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EA;
}

void battleAcDisp_MonosiriTarget(CameraId camId, void* param) {
	BattleWork* work = _battleWorkPointer;

	switch (work->actionCommands.mAcState) {
	case 0x3E8:
		actionCommandDisp(float_0_8042206c, float_0_8042206c);
		break;
	}
}

s32 battleAcResult_MonosiriTarget(BattleWork* work) {
	return work->actionCommands.result;
}

// The `ac` + `field_0x2BC[0]` access below is a match-only form: addressing the work area through
// the header's named field, rather than casting `work + 0x1F4C`, is what makes MWCC keep two
// separate base pointers (r31=mt, r28=disp) the way the original does. The natural form is under
// #else. The `unit`/`part` declaration order also matters: MWCC assigns `part` to r26 and `unit`
// to r27 only when `unit` is declared first.
s32 battleAcMain_MonosiriTarget(BattleWork* work) {
#ifdef MUST_MATCH
	BattleActions* ac = &work->actionCommands;
	MonosiriTargetWork* mt = (MonosiriTargetWork*)&ac->field_0x2BC[0];
	BattleACDispParams* disp = &ac->dispParams;
#else
	MonosiriTargetWork* mt = (MonosiriTargetWork*)((u8*)work + 0x1F4C);
	BattleACDispParams* disp = &work->actionCommands.dispParams;
#endif
	BattleUnit* unit;
	BattleUnitPart* part;
	s32 autoCommand = 0;

	if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
		autoCommand = 1;
	}

	unit = BattleGetUnitPtr(work, *(s32*)((u8*)work + 0x1CC8));
	part = BtlUnit_GetPartsPtr(unit, *(s32*)((u8*)work + 0x1CCC));

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = float_neg300_8042208c;
		disp->iconY = float_0_8042206c;
		disp->counter = 0x14;
		mt->frame = 0;
		mt->maxcount = _getmaxcount();
		BtlUnit_GetHitPos(unit, part, &mt->hitPos.x, &mt->hitPos.y, &mt->hitPos.z);
		mt->curPos = mt->hitPos;
		mt->startPos = mt->hitPos;
		mt->hitFlag = 0;
		mt->rotZ = float_0_8042206c;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		mt->sfxId = -1;
		break;
	case 0x64:
		mt->frame = 0;
		mt->sfxId = psndSFXOn(str_80300910);
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8:
		mt->frame++;
		mt->curPos.x = (mt->hitPos.x - float_80_80422090) +
		               intplGetValue(0, mt->frame, mt->maxcount, float_0_8042206c, float_160_80422094);
		if (fabs(mt->curPos.x - mt->startPos.x) < double_28_80300928) {
			mt->hitFlag = 1;
		}
		else {
			mt->hitFlag = 0;
		}
		if (mt->hitFlag == 0) {
			mt->counter = 0;
		}
		else {
			mt->counter++;
		}
		if (mt->frame >= mt->maxcount) {
			work->actionCommands.result = 0;
			work->actionCommands.mAcState = 0x3EA;
		}
		else if (BattlePadCheckTrigger(0x100) || autoCommand) {
			if (mt->hitFlag != 0) {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
			}
			else {
				if (autoCommand) {
					break;
				}
				work->actionCommands.result = 0;
			}
			work->actionCommands.mAcState = 0x3EA;
		}
		break;
	case 0x3EA:
		mt->timer = 0x3C;
		work->actionCommands.mAcState = 0x3EB;
		if (mt->sfxId != -1) {
			psndSFXOff(mt->sfxId);
		}
		// fallthrough
	case 0x3EB:
		mt->timer--;
		if (mt->timer <= 0) {
			work->actionCommands.mAcState = 0x3EC;
		}
		break;
	case 0x3EC:
		work->actionCommands.maincb = NULL;
		work->actionCommands.dispcb = NULL;
		work->actionCommands.resultcb = NULL;
		work->actionCommands.deletecb = NULL;
		return 0;
	}
	return 1;
}

// NONMATCHING (74.58%): `extsb r0, r3` before `slwi`; battle_ac.h wrongly says s8 (real type u8).
// dont_inline: the original keeps _getmaxcount as a real call (`bl _getmaxcount`), not an inline.
#pragma dont_inline on
s32 _getmaxcount(void) {
	return _frame_table[BattleActionCommandGetDifficulty(_battleWorkPointer)];
}
#pragma dont_inline reset
