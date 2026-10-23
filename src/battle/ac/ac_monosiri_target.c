#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "battle/battle_disp_tex4.h"
#include "battle/battle_pad.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
// math.h pools two sqrt doubles this unit does not have; only its fabs intrinsic is needed.
#define fabs(x) __fabs(x)
#include <string.h>
#include "battle/ac/ac_monosiri_target.h"

u32 _frame_table[7] = {
    0x000000F0, 0x000000B4, 0x00000078, 0x0000005A,
    0x00000046, 0x00000028, 0x00000014,
};

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
#define RODATA_VEC(i, name) (vecs[i])
// double_15 (at +0x88) is part of the same base symbol; reading it through vecs keeps r30 live.
#define RODATA_D15 (*(const f64*)((const u8*)vecs + 0x88))
#else
#define RODATA_D15 double_15_80300908
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

// .sdata frame table, indexed by BattleActionCommandGetDifficulty().

static void actionCommandDisp(f32 x, f32 y);

s32 _getmaxcount(void) {
	return _frame_table[BattleActionCommandGetDifficulty(_battleWorkPointer)];
}

// The `ac` local below makes MWCC keep two separate base pointers (r31=mt, r28=disp) the way the
// original does. The `unit`/`part` declaration order also matters: MWCC assigns `part` to r26 and `unit`
// to r27 only when `unit` is declared first.
s32 battleAcMain_MonosiriTarget(BattleWork* work) {
	BattleActions* ac = &work->actionCommands;
	MonosiriTargetWork* mt = (MonosiriTargetWork*)&ac->field_0x2BC[0];
	BattleACDispParams* disp = &ac->dispParams;
	BattleUnit* unit;
	BattleUnitPart* part;
	s32 autoCommand = 0;

	if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
		autoCommand = 1;
	}

	unit = BattleGetUnitPtr(work, work->actionCommands.acParams[0]);
	part = BtlUnit_GetPartsPtr(unit, work->actionCommands.acParams[1]);

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = -300.0f;
		disp->iconY = 0.0f;
		disp->counter = 0x14;
		mt->frame = 0;
		mt->maxcount = _getmaxcount();
		BtlUnit_GetHitPos(unit, part, &mt->hitPos.x, &mt->hitPos.y, &mt->hitPos.z);
		mt->curPos = mt->hitPos;
		mt->startPos = mt->hitPos;
		mt->hitFlag = 0;
		mt->rotZ = 0.0f;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		mt->sfxId = -1;
		break;
	case 0x64:
		mt->frame = 0;
		mt->sfxId = psndSFXOn("SFX_AC_CURSOR_MOVE1");
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8:
		mt->frame++;
		mt->curPos.x = (mt->hitPos.x - 80.0f) +
		               intplGetValue(0, mt->frame, mt->maxcount, 0.0f, 160.0f);
		if (fabs(mt->curPos.x - mt->startPos.x) < 28.0) {
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

s32 battleAcResult_MonosiriTarget(BattleWork* work) {
	return work->actionCommands.result;
}

void battleAcDisp_MonosiriTarget(CameraId camId, void* param) {
	BattleWork* work = _battleWorkPointer;

	switch (work->actionCommands.mAcState) {
	case 0x3E8:
		actionCommandDisp(0.0f, 0.0f);
		break;
	}
}

void battleAcDelete_MonosiriTarget(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EA;
}

static void actionCommandDisp(f32 x, f32 y) {
#ifdef MUST_MATCH
	Vec* vecs = (Vec*)vec3_80300880;
#endif
	BattleWork* work = _battleWorkPointer;
	MonosiriTargetWork* mt = (MonosiriTargetWork*)&work->actionCommands.field_0x2BC[0];
	Vec screen;
	Vec screen2;
	f32 alpha = 0.0f;

	btlGetScreenPoint(&mt->curPos, &screen);
	{
		Vec trans;
		Vec scale;
		Vec rot;
		GXColor color;
		color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
		rot = RODATA_VEC(1, vec3_8030088c);
		scale = RODATA_VEC(0, vec3_80300880);
		trans = screen;
		btlDispTex4(0x42, &trans, &scale, &rot, &color);
	}

	screen.x += 4.0f;
	screen.y -= 4.0f;
	screen.z -= 2.0f;
	{
		Vec trans;
		Vec scale;
		Vec rot;
		GXColor color;
		color = (GXColor){ 0x80, 0x80, 0x80, 0xFF };
		rot = RODATA_VEC(3, vec3_803008a4);
		scale = RODATA_VEC(2, vec3_80300898);
		trans = screen;
		btlDispTex4(0x42, &trans, &scale, &rot, &color);
	}

	if (mt->hitFlag != 0) {
		s32 rem = mt->counter % 0x1E;
		if (rem < 0x14) {
			alpha = 0.01f *
			        intplGetValue(5, rem, 0x14, 100.0f, 0.0f);
		}
	}

	btlGetScreenPoint(&mt->startPos, &screen2);

	{
		Vec trans;
		Vec scale;
		Vec rot;
		Vec tmp;
		GXColor color;
		color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
		tmp = RODATA_VEC(5, vec3_803008bc);
		tmp.z = mt->rotZ;
		rot = tmp;
		scale = RODATA_VEC(4, vec3_803008b0);
		trans = screen2;
		btlDispTex4(0x41, &trans, &scale, &rot, &color);
	}

	if (alpha > 0.0f) {
		Vec trans;
		Vec scale;
		Vec scaleTmp;
		Vec rot;
		Vec rotTmp;
		GXColor color;
		color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
		rotTmp = RODATA_VEC(7, vec3_803008d4);
		rotTmp.z = mt->rotZ;
		rot = rotTmp;
		scaleTmp = RODATA_VEC(6, vec3_803008c8);
		scaleTmp.x = alpha;
		scaleTmp.y = alpha;
		scale = scaleTmp;
		trans = screen2;
		btlDispTex4(0x41, &trans, &scale, &rot, &color);
	}

	screen2.x += 4.0f;
	screen2.y -= 4.0f;
	screen2.z -= 2.0f;
	{
		Vec trans;
		Vec scale;
		Vec rot;
		Vec tmp;
		GXColor color;
		color = (GXColor){ 0x80, 0x80, 0x80, 0xFF };
		tmp = RODATA_VEC(9, vec3_803008ec);
		tmp.z = mt->rotZ;
		rot = tmp;
		scale = RODATA_VEC(8, vec3_803008e0);
		trans = screen2;
		btlDispTex4(0x41, &trans, &scale, &rot, &color);
	}

	{
		s32 pressed;
		s32 icon;
		Vec v;

		mt->rotZ += 2.0f;
		pressed = BattleACGetButtonIcon(0x100, 1);
		icon = BattleACGetButtonIcon(0x100, 0);
		if (fabs(mt->curPos.x - mt->startPos.x) < RODATA_D15) {
			icon = pressed;
		}
		v = RODATA_VEC(10, vec3_803008f8);
		v.x = -200.0f + x;
		v.y = 50.0f + y;
		iconDispGx(1.0f, &v, 0x10, icon);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
#ifdef MUST_MATCH
const Vec vec3_80300880[11] = {
    { 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
};
#else
const Vec vec3_80300880 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_8030088c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300898 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_803008a4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_803008b0 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_803008bc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_803008c8 = { 0.0f, 0.0f, 1.0f };
const Vec vec3_803008d4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_803008e0 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_803008ec = { 0.0f, 0.0f, 0.0f };
const Vec vec3_803008f8 = { 0.0f, 0.0f, 0.0f };
#endif
// Read only through the vec3_80300880 base under MUST_MATCH, so the linker would drop it.
#pragma force_active on
const f64 double_15_80300908 = 15.0;
#pragma force_active reset
