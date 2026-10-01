#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "math_ext.h"

// battle_sub.c is compiled -O4,s in the original: functions below use stmw/lmw and the
// size-oriented scheduler. configure.py has no extra_cflags for this file, so emulate it here.
#pragma optimize_for_size on

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
#pragma fp_contract on

// opt_common_subs off: without it MWCC hoists the shared (steps-currStep) above the cos call
// and stores the difference in r30, while the original keeps currStep in r30 and subtracts after cos.
// inline_fn: the original routes cos through this trivial inline wrapper. Writing (f32)cos(...)
// directly lets MWCC reassociate the surrounding float expression and mis-colours it (98.49%);
// the extra inline call keeps the original's register/operand choices.
inline f32 inline_fn(float arg0) {
	return (f32)cos(arg0);
}

f32 intpl_sub(s32 mode, s32 currStep, s32 steps, f32 start, f32 end) {
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
			return end - (((((end - start) * inline_fn(4.0f * (3.141592f * (((f32)currStep) / ((f32)steps))))) * ((f32)(steps - currStep))) * ((f32)(steps - currStep))) / (((f32)steps) * ((f32)steps)));
		case 8:
			return end - (((((end - start) * inline_fn((4.0f * (3.141592f * (((f32)(currStep * currStep)) / ((f32)steps)))) / 15.0f)) * ((f32)(steps - currStep))) * ((f32)(steps - currStep))) / (((f32)steps) * ((f32)steps)));
		case 4:
			return start + (end - start) - (((end - start) * ((f32)((steps - currStep) * (steps - currStep)))) / ((f32)(steps * steps)));
		case 5:
			return (start + (end - start)) - (((end - start) * ((f32)(((steps - currStep) * (steps - currStep)) * (steps - currStep)))) / ((f32)((steps * steps) * steps)));
		case 6:
			return (start + (end - start)) - (((end - start) * ((f32)((((steps - currStep) * (steps - currStep)) * (steps - currStep)) * (steps - currStep)))) / ((f32)(((steps * steps) * steps) * steps)));
		case 10: {
			f32 v = ((inline_fn((4.0f * (3.141592f * (((f32)(currStep * currStep)) / ((f32)steps)))) / 40.0f) * ((f32)(steps - currStep))) * ((f32)(steps - currStep))) / (((f32)steps) * ((f32)steps));

			if (v < 0.0f) {
				v = -v;
			}
			return end - (v * (end - start));
		}
		case 11:
			return start + (end - start) * (1.0f - inline_fn((3.141592f * ((f32)currStep)) / ((f32)steps))) / 2.0f;
		case 12:
			return start + ((end - start) * ((f32)sin((1.570796f * ((f32)currStep)) / ((f32)steps))));
		case 13:
			return start + ((end - start) * (1.0f - inline_fn((1.570796f * ((f32)currStep)) / ((f32)steps))));
		default:
			return 0.0f;
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
