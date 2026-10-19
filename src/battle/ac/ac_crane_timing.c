#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "battle/battle_sub.h"
#include "driver/camdrv.h"
#include "system.h"
#include "math_ext.h"
#include "matching.h"
#include <string.h>
#include "battle/ac/ac_crane_timing.h"

// .rodata / .sdata2 literals of this unit.
const f64 double_5_802fdd38 = 5.0;
const f64 double_20_802fdd40 = 20.0;

const f32 float_neg300_80420b30 = -300.0f;
const f32 float_0_80420b34 = 0.0f;
const f32 float_1p5_80420b38 = 1.5f;
const f32 float_5_80420b3c = 5.0f;
const f32 float_neg1_80420b40 = -1.0f;

// MATCH_F64 is the f64 analogue of matching.h's MATCH_F32: read the constant through a volatile
// pointer so mwcc loads it from its named symbol instead of pooling a copy.
#ifdef MUST_MATCH
#define MATCH_F64(x) (*(volatile const f64*)&(x))
#else
#define MATCH_F64(x) (x)
#endif

// Action-command work area for CraneTiming, at BattleWork+0x1F4C (inside BattleActions.field_0x2BC).
// No header declares it.
typedef struct CraneTimingWork {
	s32 trigger;   // 0x00, BattlePadCheckTrigger mask
	s32 frame;     // 0x04, frames elapsed
	Vec center;    // 0x08, unit's reference position
	Vec current;   // 0x14, current (animated) position
	f32 velocity;  // 0x20, horizontal velocity
	s32 timer;     // 0x24, input lockout timer
	f32 amplitude; // 0x28, horizontal sway amplitude
	s32 delay;     // 0x2C, end-of-command delay
} CraneTimingWork;

s32 battleAcMain_CraneTiming(BattleWork* work) {
#ifdef MUST_MATCH
	// MWCC 2.6 folds two of the three base pointers (dec into disp) when both are derived directly
	// from `work`; deriving both from the `ac` intermediate keeps work=r28, dec=r31 and disp=r27 in
	// their own registers, as the original does.
	BattleActions* ac = &work->actionCommands;
	CraneTimingWork* dec = (CraneTimingWork*)&ac->field_0x2BC[0];
	BattleACDispParams* disp = &ac->dispParams;
#else
	CraneTimingWork* dec = (CraneTimingWork*)((u8*)work + 0x1F4C);
	BattleACDispParams* disp = &work->actionCommands.dispParams;
#endif
	BattleUnit* unit;
	s32 flag;
	s32 nearCenter;
	s32 trigger;

	flag = 0;
	if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
		flag = 1;
	}
	unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x08]);
	BtlUnit_GetPartsPtr(unit, *(s32*)&work->actionCommands.field_34[0x0C]);

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = MATCH_F32(float_neg300_80420b30);
		disp->iconY = MATCH_F32(float_0_80420b34);
		disp->counter = 0x14;
		dec->frame = 0;
		dec->timer = *(s32*)&work->actionCommands.field_34[0x10];
		dec->amplitude = (f32)irand(*(s32*)&work->actionCommands.field_34[0x14]);
		dec->trigger = 0x100;
		work->actionCommands.outputParams[0] = 0;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		break;
	case 0x64:
		unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x08]);
		BtlUnit_GetPartsPtr(unit, *(s32*)&work->actionCommands.field_34[0x0C]);
		BtlUnit_GetPos(unit, &dec->current.x, &dec->current.y, &dec->current.z);
		dec->center = dec->current;
		dec->frame = 0;
		dec->velocity = MATCH_F32(float_1p5_80420b38);
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8:
		if (dec->timer != 0) {
			dec->timer--;
		}
		dec->frame++;
		dec->current.x += dec->velocity;
		dec->current.y = dec->center.y + MATCH_F32(float_5_80420b3c) * sinfd(reviseAngle((f32)(dec->frame * 5)));

		if (dec->current.x >= dec->center.x + dec->amplitude && dec->velocity > MATCH_F32(float_0_80420b34)) {
			dec->velocity = dec->velocity * MATCH_F32(float_neg1_80420b40);
			dec->amplitude = (f32)irand(*(s32*)&work->actionCommands.field_34[0x14]);
		}
		if (dec->current.x <= dec->center.x - dec->amplitude && dec->velocity < MATCH_F32(float_0_80420b34)) {
			dec->velocity = dec->velocity * MATCH_F32(float_neg1_80420b40);
			dec->amplitude = (f32)irand(*(s32*)&work->actionCommands.field_34[0x14]);
		}
		if (dec->velocity >= MATCH_F32(float_0_80420b34)) {
			unit->faceDirection = 1;
		}
		else {
			unit->faceDirection = -1;
		}
		BtlUnit_SetPos(unit, dec->current.x, dec->current.y, dec->current.z);

		nearCenter = 0;
		if (flag != 0 && __fabsf(dec->center.x - dec->current.x) < MATCH_F64(double_5_802fdd38)) {
			nearCenter = 1;
		}
		trigger = BattlePadCheckTrigger(dec->trigger);
		if ((trigger != 0 && dec->timer <= 0) || (nearCenter != 0 && dec->timer <= 0)) {
			work->actionCommands.outputParams[0] = 1;
			if (__fabsf(dec->center.x - dec->current.x) > MATCH_F64(double_20_802fdd40)) {
				work->actionCommands.result = 0;
			}
			else {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
			}
			work->actionCommands.mAcState = 0x3EA;
			break;
		}
		if (dec->frame >= *(s32*)&work->actionCommands.field_34[0x1C]) {
			work->actionCommands.result = 0;
			work->actionCommands.mAcState = 0x3EA;
		}
		break;
	case 0x3EA:
		dec->delay = 0x3C;
		work->actionCommands.mAcState = 0x3EB;
		// fallthrough
	case 0x3EB:
		dec->delay--;
		if (dec->delay > 0) {
			break;
		}
		work->actionCommands.mAcState = 0x3EC;
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

s32 battleAcResult_CraneTiming(BattleWork* work) {
	return work->actionCommands.result;
}

// No-op in the original (just blr).
void battleAcDisp_CraneTiming(CameraId camId, void* param) {
}

void battleAcDelete_CraneTiming(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EA;
}
