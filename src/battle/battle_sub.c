#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "math_ext.h"

extern BattleWork* _battleWorkPointer;

// .sdata2 constants emitted by this unit
extern const f64 double_0p0001_802efab0;

f32 sinfd(f32 deg) {
	return (f32)sin(6.2831855f * deg / 360.0f);
}

f32 cosfd(f32 angle) {
	return (f32)cos(6.2831855f * angle / 360.0f);
}

f32 atan2f_safety(f32 y, f32 x) {
	f32 a;
	f64 eps = 0.0001;

	a = (x >= 0.0f) ? x : -x;
	if (a < eps) {
		x = 0.0001f;
	}
	a = (y >= 0.0f) ? y : -y;
	if (a < eps) {
		y = 0.0001f;
	}
	return (f32)atan2(y, x);
}

// fp_contract off: the original keeps `speed * s` and `*x + ...` as separate fmuls/fadds, and
// the declaration order rad/s/c gives the original f30=rad, f31=s.
#pragma fp_contract off
void btlMovePos(f32* x, f32* z, f32 speed, f32 angle) {
	f32 s;
	f32 rad;
	f32 c;

	rad = 6.2831855f * angle / 360.0f;
	s = (f32)sin(rad);
	c = (f32)cos(rad);
	*x += speed * s;
	*z += -speed * c;
}
#pragma fp_contract reset


// intplCos: the original routes cos through this trivial inline wrapper. Writing (f32)cos(...)
// directly lets MWCC reassociate the surrounding float expression and mis-colours it (98.49%);
// the extra inline call keeps the original's register/operand choices.
static inline f32 intplCos(float arg0) {
	return (f32)cos(arg0);
}

#undef intpl_sub
f32 intpl_sub(s32 mode, f32 start, f32 end, s32 currStep, s32 steps) {
	switch (mode) {
		case 0:
			return start + (((end - start) * ((f32)currStep)) / ((f32)steps));
		case 1:
			return start + (((end - start) * ((f32)(currStep * currStep))) / ((f32)(steps * steps)));
		case 2:
			return start + (((end - start) * ((f32)((currStep * currStep) * currStep))) / ((f32)((steps * steps) * steps)));
		case 3:
			return start + (((end - start) * ((f32)(((currStep * currStep) * currStep) * currStep))) / ((f32)(((steps * steps) * steps) * steps)));
		case 7:
			return end - (((((end - start) * intplCos(4.0f * (3.141592f * (((f32)currStep) / ((f32)steps))))) * ((f32)(steps - currStep))) * ((f32)(steps - currStep))) / (((f32)steps) * ((f32)steps)));
		case 8:
			return end - (((((end - start) * intplCos((4.0f * (3.141592f * (((f32)(currStep * currStep)) / ((f32)steps)))) / 15.0f)) * ((f32)(steps - currStep))) * ((f32)(steps - currStep))) / (((f32)steps) * ((f32)steps)));
		case 4:
			return start + (end - start) - (((end - start) * ((f32)((steps - currStep) * (steps - currStep)))) / ((f32)(steps * steps)));
		case 5:
			return (start + (end - start)) - (((end - start) * ((f32)(((steps - currStep) * (steps - currStep)) * (steps - currStep)))) / ((f32)((steps * steps) * steps)));
		case 6:
			return (start + (end - start)) - (((end - start) * ((f32)((((steps - currStep) * (steps - currStep)) * (steps - currStep)) * (steps - currStep)))) / ((f32)(((steps * steps) * steps) * steps)));
		case 10: {
			f32 v = ((intplCos((4.0f * (3.141592f * (((f32)(currStep * currStep)) / ((f32)steps)))) / 40.0f) * ((f32)(steps - currStep))) * ((f32)(steps - currStep))) / (((f32)steps) * ((f32)steps));

			if (v < 0.0f) {
				v = -v;
			}
			return end - (v * (end - start));
		}
		case 11:
			return start + (end - start) * (1.0f - intplCos((3.141592f * ((f32)currStep)) / ((f32)steps))) / 2.0f;
		case 12:
			return start + ((end - start) * ((f32)sin((1.570796f * ((f32)currStep)) / ((f32)steps))));
		case 13:
			return start + ((end - start) * (1.0f - intplCos((1.570796f * ((f32)currStep)) / ((f32)steps))));
		default:
			return 0.0f;
	}
}
s32 BtlCompForwardLv(s32 direction, f32 dist) {
	s32 lv = 0;
	s32 result;

	if (dist > -800.f) {
		lv = (s32)((800.f + dist) / 40.f);
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

s32 BattleTransID(EventEntry* evt, s32 type) {
	BattleUnit* unit;
	BattleWork* work = _battleWorkPointer;

	if (type < 0) {
		switch (type) {
			case -2:
				return evt->unitId;
			case -3:
				unit = BattleGetMarioPtr(work);
				return unit->unitId;
			case -4:
				unit = BattleGetPartyPtr(work);
				if (unit) {
					return unit->unitId;
				}
				else {
					return -1;
				}
			case -1:
				return -1;
			case -5:
				unit = BattleGetSystemPtr(work);
				return unit->unitId;
			case -6:
				return work->field_424;
		}
	}
	return type;
}

// Two aliases of `work` are needed to reproduce the original's register colouring: the compiler
// then keeps the counter in r29 and the base pointer in r30 (without the aliases it is reversed).
void btlsubResetMoveColorLvAll(BattleWork* work) {
	BattleUnit* unit;
	BattleWork* w = work;
	BattleWork* w2 = w;
	int i;

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(w2, i);
		if (unit) {
			unit->mMoveColorLv = 0xFF;
		}
	}
}
