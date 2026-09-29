#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "math_ext.h"

extern BattleWork* _battleWorkPointer;

f32 sinfd(f32 deg) {
	return (f32)sin(6.2831855f * deg / 360.0f);
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
