#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "math_ext.h"

extern BattleWork* _battleWorkPointer;

// .sdata2 constants emitted by this unit
extern const f32 float_neg800_8041c380;
extern const f32 float_800_8041c384;
extern const f32 float_40_8041c388;
#ifdef MUST_MATCH
// volatile: forces atan2f_safety to reload the constants where the original does; without it the
// register allocator keeps them live and picks different registers (89%).
extern const volatile f32 float_0_8041c398;
extern const volatile f32 float_0p0001_8041c3b0;
extern const volatile f64 double_0p0001_802efab0;
#else
extern const f32 float_0_8041c398;
extern const f32 float_0p0001_8041c3b0;
extern const f64 double_0p0001_802efab0;
#endif

f32 sinfd(f32 deg) {
	return (f32)sin(6.2831855f * deg / 360.0f);
}

f32 cosfd(f32 angle) {
	return (f32)cos(6.2831855f * angle / 360.0f);
}

f32 atan2f_safety(f32 y, f32 x) {
	f32 a;

	a = (x >= float_0_8041c398) ? x : -x;
	if (a < double_0p0001_802efab0) {
		x = float_0p0001_8041c3b0;
	}
	a = (y >= float_0_8041c398) ? y : -y;
	if (a < double_0p0001_802efab0) {
		y = float_0p0001_8041c3b0;
	}
	return (f32)atan2(y, x);
}

// NONMATCHING (57.38%): 1 instruction differs, 2 missing, 6 extra, plus register differences.
void btlsubResetMoveColorLvAll(BattleWork* work) {
	BattleUnit* unit;
	int i;

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(work, i);
		if (unit) {
			unit->mMoveColorLv = 0xFF;
		}
	}
}

s32 BtlCompForwardLv(s32 direction, f32 dist) {
	s32 lv = 0;
	s32 result;

	if (dist > float_neg800_8041c380) {
		lv = (s32)((float_800_8041c384 + dist) / float_40_8041c388);
		if (lv > 48) {
			lv = 48;
		}
	}

	result = 48 - lv;
	if (direction >= 0) {
		result = lv;
	}
	return result;
}

// NONMATCHING (70.53%): 7 instructions differ, 7 missing, 3 extra, plus register differences.
s32 BattleTransID(EventEntry* evt, s32 type) {
	BattleUnit* unit;

	switch (type) {
		case -1:
			return -1;
		case -3:
			unit = BattleGetMarioPtr(_battleWorkPointer);
			return unit->unitId;
		case -4:
			unit = BattleGetPartyPtr(_battleWorkPointer);
			if (unit) {
				return unit->unitId;
			}
			else {
				return -1;
			}
		case -5:
			unit = BattleGetSystemPtr(_battleWorkPointer);
			return unit->unitId;
		case -6:
			return _battleWorkPointer->field_424;
		default:
			return type;
	}
}
