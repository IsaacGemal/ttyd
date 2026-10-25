#include "battle/ac/ac_pendulum_crane_timing.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_pad.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include "pmario_sound.h"
#include "matching.h"
#include <dolphin/gx.h>
#include <stdlib.h>
#include <string.h>

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

// Emit real functions in retail order; actionCommandDisp pools the native conversion double.
#ifdef MUST_MATCH
#pragma defer_codegen off
#endif

s32 _get_angle_hp(s32 hp) {
	HpAngle* entry = _hp_tbl;

	while (hp > entry->hp) {
		entry++;
	}
	return entry->angle;
}

// 100%: the original reuses the parameter for the adjusted angle, which puts the sum in angle's f2.
s32 _get_angle_rate(f32 angle) {
	s32 rate;

	angle = compAngle(0.0f, angle);
	angle += 45.0f;
	rate = (s32)(100.0f * angle / 90.0f);

	if (rate < 0) {
		rate = 0;
	}
	if (rate > 100) {
		rate = 100;
	}
	return rate;
}

static void actionCommandDisp(f32 x, f32 y);

// 100%: two Vec locals are needed -- the original keeps the first icon's temp at sp+0x14 and the second at
// sp+0x8. The f32 declaration order below is what makes baseX land in f25 (shared with yBottom), matching the
// original's saved-register set.
static void actionCommandDisp(f32 x, f32 y) {
	BattleWork* work = _battleWorkPointer;
	PendulumCraneTimingWork* dec = (PendulumCraneTimingWork*)&work->actionCommands.acWork[0];
	CameraEntry* cam;
	Vec v;
	Vec v2;
	s32 i;
	f32 xBase;
	f32 yBottom;
	f32 yTop;
	f32 baseX;

	camGetPtr(CAMERA_2D);

	baseX = -200.0f + x;
	v = (Vec){0.0f, 0.0f, 0.0f};
	v.x = baseX;
	v.y = 25.0f + y;
	iconDispGx(1.0f, &v, 0x10, 0x8C);

	cam = camGetPtr(CAMERA_2D);
	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(cam->view, 0);

	xBase = baseX - 88.0f;
	yTop = 50.0f + y;
	yBottom = yTop - 18.0f;

	for (i = 0; i < 7; i++) {
		f32 x0;
		f32 x1;

		if (i == 0) {
			x0 = xBase;
		}
		else {
			x0 = xBase + (f32)(((u16*)_hp_tbl)[2 * i - 2] * 175) / 100.0f;
		}
		x1 = xBase + (f32)(((u16*)_hp_tbl)[2 * i] * 175) / 100.0f;
		btlDispGXQuads2DRasta(x0, yTop, x1, yBottom, (u8)((3 - abs(3 - i)) * 60), 0,
		                      (u8)(180 - (3 - abs(3 - i)) * 60), 0xFF);
	}

	v2 = (Vec){0.0f, 0.0f, 0.0f};
	v2.x = -288.0f + x + (f32)(dec->rate * 176) / 100.0f;
	v2.y = 45.0f + y;
	iconDispGx(1.0f, &v2, 0x10, 0x96);
}

void battleAcDelete_PendulumCraneTiming(BattleWork* work) {
	if (!(work->actionCommands.endFlags & 1)) {
		work->actionCommands.mAcState = 0x3EA;
	}
}

void battleAcDisp_PendulumCraneTiming(CameraId camId, void* param) {
	BattleActions* ac = &_battleWorkPointer->actionCommands;
	s32 state = ac->mAcState;
	BattleACDispParams* ag = &ac->dispParams;

	switch (state) {
	case 0x63:
		ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, -300.0f, 30.0f);
		// fallthrough
	case 0x64:
	case 0x3E8:
		actionCommandDisp(ag->iconX, ag->iconY);
		if (ag->counter > 0) {
			ag->counter--;
		}
		break;
	case 0x3EA:
	case 0x3EB:
		if (ag->counter >= 0x28) {
			ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, 30.0f, -300.0f);
		}
		else {
			ag->iconX = 30.0f;
		}
		actionCommandDisp(ag->iconX, ag->iconY);
		if (ag->counter < 0x3C) {
			ag->counter++;
		}
		break;
	}
}

s32 battleAcResult_PendulumCraneTiming(BattleWork* work) {
	return work->actionCommands.result;
}

// 100%: the action-command state machine. `span` holds acParams[4]*2 in one value to get the target's
// r4 allocation; `bw`/`delta` are read before the direction test so the global and 0.0f loads hoist as they do
// in the target. The sfx-id and BattlePadCheckTrigger tests use a `(u32)`/`(s32)` cast to match the original's
// `addis`/`cmplwi` and signed tests.
s32 battleAcMain_PendulumCraneTiming(BattleWork* work) {
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	PendulumCraneTimingWork* dec = (PendulumCraneTimingWork*)&work->actionCommands.acWork[0];
	BattleUnit* unit;
	s32 flag = 0;
	s32 hit;
	s32 rate;
	s32 d;
	f32 dirA;
	f32 dirB;
	f32 dirF;

	if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
		flag = 1;
	}
	unit = BattleGetUnitPtr(work, work->actionCommands.acParams[1]);
	BtlUnit_GetPartsPtr(unit, work->actionCommands.acParams[2]);

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = -300.0f;
		disp->iconY = 0.0f;
		disp->counter = 0x14;
		dec->frame = 0;
		dec->angle = 0.0f;
		dec->accum = work->actionCommands.acParams[4];
		dec->accum2 = 0;
		dec->amplitude = 3.0f;
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
		BtlUnit_SetRotateOffset(unit, 0.0f, (f32)work->actionCommands.acParams[3], 0.0f);
		unit = BattleGetUnitPtr(work, work->actionCommands.acParams[1]);
		BtlUnit_GetPartsPtr(unit, work->actionCommands.acParams[2]);
		BtlUnit_GetPos(unit, &dec->position.x, &dec->position.y, &dec->position.z);
		dec->center = dec->position;
		dec->frame = 0;
		dec->unk20 = 0.5f;
		dec->dir = 1;
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8: {
		s32 span;
		f32 delta;
		s32 rem;
		BattleWork* bw;

		dec->frame++;
		span = work->actionCommands.acParams[4] * 2;
		rem = (dec->accum + dec->accum2) % span;
		dec->angle2 = (f32)(rem * 360 / span);
		dec->angle = reviseAngle(sinfd(dec->angle2) * 45.0f);

		bw = _battleWorkPointer;
		delta = 0.0f;

		if (dec->angle2 >= 90.0f && dec->angle2 < 270.0f) {
			dirF = -1.0f;
		}
		else {
			dirF = 1.0f;
		}

		if (work->actionCommands.acFlag & 1) {
			PADStatus* pad = &bw->padWork[0].mPadData[0];
			f32 t;
			f32 one;

			if (pad->triggerRight > 10 && pad->triggerLeft < 10) {
				if (dirF < 0.0f) {
					t = pad->triggerRight / 128.0f;
					one = 1.0f;
					if (t > one) {
						t = one;
					}
					delta = 0.95f * -dec->amplitude * t;
				}
			}
			else if (pad->triggerRight < 10 && pad->triggerLeft > 10) {
				if (dirF > 0.0f) {
					t = pad->triggerLeft / 128.0f;
					one = 1.0f;
					if (t > one) {
						t = one;
					}
					delta = 0.95f * -dec->amplitude * t;
				}
			}
		}

		dec->accum2 += delta;

		if (dec->angle2 >= 90.0f && dec->angle2 < 270.0f) {
			dirA = -1.0f;
		}
		else {
			dirA = 1.0f;
		}

		dec->accum += dec->amplitude;

		span = work->actionCommands.acParams[4] * 2;
		rem = (dec->accum + dec->accum2) % span;
		dec->angle2 = (f32)(rem * 360 / span);
		dec->angle = reviseAngle(sinfd(dec->angle2) * 45.0f);

		BtlUnit_SetRotate(unit, 0.0f, 0.0f, dec->angle);

		if (dec->angle2 >= 90.0f && dec->angle2 < 270.0f) {
			dirB = -1.0f;
		}
		else {
			dirB = 1.0f;
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
			psndSFXOn("SFX_AC_GAUGE_MOVE1");
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
		else if (dec->frame > work->actionCommands.acParams[5]) {
			work->actionCommands.result = 0;
			work->actionCommands.mAcState = 0x3EA;
		}
		break;
	}
	case 0x3EA:
		work->actionCommands.endFlags |= 1;
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

// 4-byte zero word that ends the unit's .sdata: explicit_zero_data keeps the zero word instead of
// dropping it, force_active keeps it through the linker, and the section keeps it out of .bss.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") static u32 gap_07_804141CC_sdata[1] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset
