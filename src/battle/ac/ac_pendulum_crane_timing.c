#include "battle/ac/ac_pendulum_crane_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_pad.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include "pmario_sound.h"
#include "matching.h"
#include <dolphin/gx.h>
#include <stdlib.h>
#include <string.h>

// .rodata
const Vec vec3_802fdba0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fdbac = {0.0f, 0.0f, 0.0f};
const f64 double_to_int_802fdbb8 = 4503601774854144.0;
const char str_802FDBC0[] = "SFX_AC_GAUGE_MOVE1";
const f64 double_to_int_mask_802fdbd8 = 4503599627370496.0;

// .sdata2
const f32 float_0_80420978 = 0.0f;
const f32 float_45_8042097c = 45.0f;
const f32 float_100_80420980 = 100.0f;
const f32 float_90_80420984 = 90.0f;
const f32 float_neg200_80420988 = -200.0f;
const f32 float_25_8042098c = 25.0f;
const f32 float_1_80420990 = 1.0f;
const f32 float_88_80420994 = 88.0f;
const f32 float_50_80420998 = 50.0f;
const f32 float_18_8042099c = 18.0f;
const f32 float_neg288_804209a0 = -288.0f;
const f32 float_neg300_804209a4 = -300.0f;
const f32 float_30_804209a8 = 30.0f;
const f32 float_3_804209ac = 3.0f;
const f32 float_0p5_804209b0 = 0.5f;
const f32 float_270_804209b4 = 270.0f;
const f32 float_neg1_804209b8 = -1.0f;
const f32 float_0p0078125_804209bc = 0.0078125f;
const f32 float_0p95_804209c0 = 0.95f;

// .sdata
typedef struct HpAngle {
	u16 hp;    // 0x0
	u16 angle; // 0x2
} HpAngle;

HpAngle _hp_tbl[] = {
	{0x0014, 0x0002}, {0x0023, 0x0004}, {0x002D, 0x0006}, {0x0037, 0x0008},
	{0x0041, 0x0006}, {0x0050, 0x0004}, {0x0064, 0x0002},
};

// Action-command work area for PendulumCraneTiming, at BattleWork+0x1F4C.
typedef struct PendulumCraneTimingWork {
	s32 trigger;   // 0x00
	s32 frame;     // 0x04
	Vec center;    // 0x08
	Vec position;  // 0x14
	f32 unk20;     // 0x20
	u8 unk24[4];   // 0x24
	f32 amplitude; // 0x28
	f32 angle;     // 0x2C
	f32 angle2;    // 0x30
	s32 accum;     // 0x34
	s32 accum2;    // 0x38
	s32 dir;       // 0x3C
	s32 rate;      // 0x40
	s32 sfxId;     // 0x44
	s32 lastDir;   // 0x48
	s32 timer;     // 0x4C
} PendulumCraneTimingWork;

// don't_inline: the original calls _get_angle_hp/_get_angle_rate (bl) instead of inlining them.
#pragma dont_inline on
s32 _get_angle_hp(s32 hp) {
	HpAngle* entry = _hp_tbl;

	while (hp > entry->hp) {
		entry++;
	}
	return entry->angle;
}

// NONMATCHING (99.20%): only the 100.0f load target (f2 vs f3) and the add/mul destination
// register differ; mwcc folds the sum into f1 instead of f2.
s32 _get_angle_rate(f32 angle) {
	f32 base = compAngle(MATCH_F32(float_0_80420978), angle);
	f32 bumped = base + MATCH_F32(float_45_8042097c);
	f32 scaled = MATCH_F32(float_100_80420980) * bumped;
	s32 rate = (s32)(scaled / MATCH_F32(float_90_80420984));

	if (rate < 0) {
		rate = 0;
	}
	if (rate > 100) {
		rate = 100;
	}
	return rate;
}
#pragma dont_inline reset

static void actionCommandDisp(f32 x, f32 y);

// don't_inline: the original keeps actionCommandDisp as its own local function (bl from battleAcDisp)
#pragma dont_inline on
// NONMATCHING (89.38%): `baseX` is f27 vs the original f25 (so stack slots/saved regs shift); the loop's
// `100.0f` load is a local `@371` (const-propagated; a volatile read keeps the name but stops hoisting); the
// int->float double is @343 and the final divide's operands are scheduled in a different order.
static void actionCommandDisp(f32 x, f32 y) {
	BattleWork* work = _battleWorkPointer;
	PendulumCraneTimingWork* dec = (PendulumCraneTimingWork*)((u8*)work + 0x1F4C);
	CameraEntry* cam;
	Vec v;
	s32 i;
	f32 baseX;
	f32 negX;
	f32 xBase;
	f32 yTop;
	f32 yBottom;

	camGetPtr(CAMERA_2D);

	baseX = MATCH_F32(float_neg200_80420988) + x;
	v = vec3_802fdba0;
	v.x = baseX;
	v.y = MATCH_F32(float_25_8042098c) + y;
	iconDispGx(&v, 0x10, 0x8C, MATCH_F32(float_1_80420990));

	cam = camGetPtr(CAMERA_2D);
	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(cam->view, 0);

	yTop = MATCH_F32(float_50_80420998) + y;
	xBase = baseX - MATCH_F32(float_88_80420994);
	yBottom = yTop - MATCH_F32(float_18_8042099c);

	for (i = 0; i < 7; i++) {
		f32 x0;
		f32 x1;

		if (i == 0) {
			x0 = xBase;
		}
		else {
			x0 = xBase + (f32)(((u16*)_hp_tbl)[2 * i - 2] * 175) / float_100_80420980;
		}
		x1 = xBase + (f32)(((u16*)_hp_tbl)[2 * i] * 175) / float_100_80420980;
		btlDispGXQuads2DRasta(x0, yTop, x1, yBottom, (u8)((3 - abs(3 - i)) * 60), 0,
		                      (u8)(180 - (3 - abs(3 - i)) * 60), 0xFF);
	}

	v = vec3_802fdbac;
	negX = MATCH_F32(float_neg288_804209a0) + x;
	v.y = MATCH_F32(float_45_8042097c) + y;
	v.x = negX + (f32)(dec->rate * 176) / float_100_80420980;
	iconDispGx(&v, 0x10, 0x96, MATCH_F32(float_1_80420990));
}
#pragma dont_inline reset

void battleAcDelete_PendulumCraneTiming(BattleWork* work) {
	if (!(work->actionCommands.field_0x30 & 1)) {
		work->actionCommands.mAcState = 0x3EA;
	}
}

// NONMATCHING (98.08%): the disp pointer is folded into the BattleWork base (0x1f34 offsets) instead of the
// original's `addi r31, work, 0x1f20`; the original also keeps the work pointer in r3 rather than r31.
void battleAcDisp_PendulumCraneTiming(CameraId camId, void* param) {
	BattleWork* work = _battleWorkPointer;
	s32 state = work->actionCommands.mAcState;
	BattleACDispParams* ag = &work->actionCommands.dispParams;
#ifdef MUST_MATCH
	BattleACDispParams* p; // retail copies the params pointer before the switch and again in case 0x63

	p = ag;
#else
	BattleACDispParams* p = ag;
#endif

	switch (state) {
	case 0x63:
		ag->iconX =
		    intplGetValue(4, 0x14 - ag->counter, 0x14, MATCH_F32(float_neg300_804209a4), MATCH_F32(float_30_804209a8));
#ifdef MUST_MATCH
		p = ag;
#endif
		// fallthrough
	case 0x64:
	case 0x3E8:
		actionCommandDisp(p->iconX, p->iconY);
		if (p->counter > 0) {
			p->counter--;
		}
		break;
	case 0x3EA:
	case 0x3EB:
		if (p->counter >= 0x28) {
			p->iconX = intplGetValue(4, p->counter - 0x28, 0x14, MATCH_F32(float_30_804209a8),
			                          MATCH_F32(float_neg300_804209a4));
		}
		else {
			p->iconX = MATCH_F32(float_30_804209a8);
		}
		actionCommandDisp(p->iconX, p->iconY);
		if (p->counter < 0x3C) {
			p->counter++;
		}
		break;
	}
}

s32 battleAcResult_PendulumCraneTiming(BattleWork* work) {
	return work->actionCommands.result;
}

// NONMATCHING (95.60%): the implicit int->float magic double compiles to a local `@343`/`@346` symbol instead
// of the named `double_to_int_802fdbb8`/`double_to_int_mask_802fdbd8`; the L/R-trigger clamp keeps 1.0f in f4
// instead of reusing f0; a few state-0 loads are scheduled differently. The sfx-id and BattlePadCheckTrigger tests both use `cmpwi` (via a
// `(u32)`/`(s32)` cast) to match the original's `addis`/`cmplwi` and signed tests.
s32 battleAcMain_PendulumCraneTiming(BattleWork* work) {
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	PendulumCraneTimingWork* dec = (PendulumCraneTimingWork*)((u8*)work + 0x1F4C);
	BattleUnit* unit;
	s32 flag = 0;
	s32 hit;
	s32 rate;
	s32 d;
	f32 dirA;
	f32 dirB;
	f32 dirF;
	f32 zero;

	if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
		flag = 1;
	}
	unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x08]);
	BtlUnit_GetPartsPtr(unit, *(s32*)&work->actionCommands.field_34[0x0C]);

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = MATCH_F32(float_neg300_804209a4);
		disp->iconY = MATCH_F32(float_0_80420978);
		disp->counter = 0x14;
		dec->frame = 0;
		dec->angle = MATCH_F32(float_0_80420978);
		dec->accum = *(s32*)&work->actionCommands.field_34[0x14];
		dec->accum2 = 0;
		dec->amplitude = MATCH_F32(float_3_804209ac);
		dec->lastDir = -1;
		dec->trigger = 0x100;
		work->actionCommands.outputParams[0] = 0;
		work->actionCommands.outputParams[1] = _get_angle_hp(0x32);
		work->actionCommands.outputParams[2] = _get_angle_hp(0);
		dec->rate = 0x32;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		dec->sfxId = -1;
		break;
	case 0x64:
		zero = MATCH_F32(float_0_80420978);
		BtlUnit_SetRotateOffset(unit, zero, (f32)*(s32*)&work->actionCommands.field_34[0x10], zero);
		unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x08]);
		BtlUnit_GetPartsPtr(unit, *(s32*)&work->actionCommands.field_34[0x0C]);
		BtlUnit_GetPos(unit, &dec->position.x, &dec->position.y, &dec->position.z);
		dec->center = dec->position;
		dec->frame = 0;
		dec->unk20 = MATCH_F32(float_0p5_804209b0);
		dec->dir = 1;
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8: {
		s32 span;
		f32 delta;
		s32 rem;

		dec->frame++;
		span = *(s32*)&work->actionCommands.field_34[0x14];
		rem = (dec->accum + dec->accum2) % (span * 2);
		dec->angle2 = (f32)(rem * 360 / (span * 2));
		dec->angle = reviseAngle(sinfd(dec->angle2) * MATCH_F32(float_45_8042097c));

		if (dec->angle2 >= MATCH_F32(float_90_80420984) && dec->angle2 < MATCH_F32(float_270_804209b4)) {
			dirF = MATCH_F32(float_neg1_804209b8);
		}
		else {
			dirF = MATCH_F32(float_1_80420990);
		}

		delta = MATCH_F32(float_0_80420978);

		if (*(s32*)&work->actionCommands.field_34[0x00] & 1) {
			PADStatus* pad = &_battleWorkPointer->padWork[0].mPadData[0];
			f32 t;
			f32 limit;

			limit = MATCH_F32(float_1_80420990);
			if (pad->triggerRight > 10 && pad->triggerLeft < 10) {
				if (dirF < MATCH_F32(float_0_80420978)) {
					t = (f32)pad->triggerRight * MATCH_F32(float_0p0078125_804209bc);
					if (t > limit) {
						t = limit;
					}
					delta = (MATCH_F32(float_0p95_804209c0) * -dec->amplitude) * t;
				}
			}
			else if (pad->triggerRight < 10 && pad->triggerLeft > 10) {
				if (dirF > MATCH_F32(float_0_80420978)) {
					t = (f32)pad->triggerLeft * MATCH_F32(float_0p0078125_804209bc);
					if (t > limit) {
						t = limit;
					}
					delta = (MATCH_F32(float_0p95_804209c0) * -dec->amplitude) * t;
				}
			}
		}

		dec->accum2 += delta;

		if (dec->angle2 >= MATCH_F32(float_90_80420984) && dec->angle2 < MATCH_F32(float_270_804209b4)) {
			dirA = MATCH_F32(float_neg1_804209b8);
		}
		else {
			dirA = MATCH_F32(float_1_80420990);
		}

		dec->accum += dec->amplitude;

		span = *(s32*)&work->actionCommands.field_34[0x14];
		rem = (dec->accum + dec->accum2) % (span * 2);
		dec->angle2 = (f32)(rem * 360 / (span * 2));
		dec->angle = reviseAngle(sinfd(dec->angle2) * MATCH_F32(float_45_8042097c));

		zero = MATCH_F32(float_0_80420978);
		BtlUnit_SetRotate(unit, zero, zero, dec->angle);

		if (dec->angle2 >= MATCH_F32(float_90_80420984) && dec->angle2 < MATCH_F32(float_270_804209b4)) {
			dirB = MATCH_F32(float_neg1_804209b8);
		}
		else {
			dirB = MATCH_F32(float_1_80420990);
		}
		if (dirA != dirB) {
			dec->dir = 0;
		}

		rate = _get_angle_rate(dec->angle);
		work->actionCommands.outputParams[1] = _get_angle_hp(rate);
		dec->rate = rate;

		rate = _get_angle_rate(dec->angle);
		d = -1;
		if (rate <= 5) {
			d = 0;
		}
		else if (rate >= 0x5F) {
			d = 1;
		}
		if (d != -1 && d != dec->lastDir) {
			psndSFXOn(str_802FDBC0);
			dec->lastDir = d;
		}

		hit = 0;
		if (flag != 0 && work->actionCommands.outputParams[1] >= _get_angle_hp(0x32)) {
			hit = 1;
		}

		if (BattlePadCheckTrigger(dec->trigger) != 0 || hit != 0) {
			work->actionCommands.outputParams[0] = 1;
			work->actionCommands.result = 2;
			work->actionCommands.mResultCount++;
			work->actionCommands.mAcState = 0x3EA;
		}
		else if (dec->frame > *(s32*)&work->actionCommands.field_34[0x18]) {
			work->actionCommands.result = 0;
			work->actionCommands.mAcState = 0x3EA;
		}
		break;
	}
	case 0x3EA:
		work->actionCommands.field_0x30 |= 1;
		dec->timer = 0x3C;
		if ((u32)dec->sfxId != 0xFFFFFFFFu) {
			psndSFXOff(dec->sfxId);
		}
		work->actionCommands.mAcState = 0x3EB;
		// fallthrough
	case 0x3EB:
		dec->timer--;
		if (dec->timer <= 0) {
			work->actionCommands.mAcState = 0x3EC;
		}
		break;
	case 0x3EC:
		work->actionCommands.maincb = NULL;
		work->actionCommands.dispcb = NULL;
		work->actionCommands.resultcb = NULL;
		work->actionCommands.deletecb = NULL;
		return 0;
	default:
		break;
	}
	return 1;
}
