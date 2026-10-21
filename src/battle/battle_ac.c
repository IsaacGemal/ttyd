#include "battle/battle_ac.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_disp.h"
#include "battle/ac/ac_table.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "pmario_sound.h"

extern ActionCommandEntry ActionCommandList[];
extern BattleWork* _battleWorkPointer;

//local prototypes
void psndSFX_pit(s32 id, s32 pitch);


void BattleActionCommandManagerInit(BattleWork* wp) {
	BtlPad_WorkInit(&wp->actionCommands.pad);
	wp->actionCommands.maincb = NULL;
	wp->actionCommands.resultcb = NULL;
	wp->actionCommands.dispcb = NULL;
	wp->actionCommands.deletecb = NULL;
	wp->actionCommands.field_0x288 = -1;
}

void BattleActionCommandManager(BattleWork* wp) {
	BattleACMainCallback main;
	DispCallback disp;

	BtlPad_WorkUpdate(&wp->actionCommands.pad, 0);
	main = wp->actionCommands.maincb;
	if (main) {
		if (!main(wp)) {
			wp->actionCommands.maincb = NULL;
		}

		disp = wp->actionCommands.dispcb;
		if (disp) {
			dispEntry(CAMERA_2D, 900.0f, 1, disp, wp);
		}
	}
}

s32 BattleActionCommandResult(BattleWork* wp) {
	BattleACResultCallback result;

	result = wp->actionCommands.resultcb;
	if (result) {
		return result(wp);
	}
	else {
		return wp->actionCommands.result & 2;
	}
}

void BattleActionCommandDeclareACResult(BattleWork* work, BattleWeapon* weapon, s32 result) {
	StarPowerInfo* info = &work->impendingBonuses;

	switch (result) {
	case -1:
		BattleAudience_Case_ActionCommandBad(weapon);
		break;
	default:
		info->mWeapon = weapon;
		info->mSpAcSuccessMultiplier = ((f32)result * 0.25f) + 0.5f;
		info->field_0x9 = weapon->field_0x19;
		info->mBingoSlotChance = weapon->mBingoSlotChance;
		BattleAudience_Case_ActionCommandGood(weapon);
		break;
	}
}

void BattleActionCommandSetup(BattleWork* work, s32 id, BattleUnit* unit, s32 a4, s32 a5) {
	ActionCommandEntry* entry = ActionCommandList; // r6

	work->actionCommands.mAcUnit = unit;
	work->actionCommands.field_0x4 = a4;
	work->actionCommands.mAcState = 0;
	work->actionCommands.mResultCount = 0;
	work->actionCommands.result = 1;
	work->actionCommands.maincb = NULL;
	work->actionCommands.dispcb = NULL;
	work->actionCommands.resultcb = NULL;
	work->actionCommands.deletecb = NULL;
	work->actionCommands.field_0x8 = a5;
	work->actionCommands.field_0x30 = 0;
	while (entry->id) {
		if (entry->id == id) {
			work->actionCommands.maincb = entry->main;
			if (!(work->actionCommands.mAcUnit->mTokenFlags & 0x10)){
				work->actionCommands.dispcb = entry->disp;
			}
			work->actionCommands.resultcb = entry->result;
			work->actionCommands.deletecb = entry->delete;
			return;
		}
		entry++;
	}
}

void BattleActionCommandStart(BattleWork* work) {
	work->actionCommands.mAcState = 100;
}

void BattleActionCommandStop(BattleWork* work) {
	if (work->actionCommands.deletecb) {
		work->actionCommands.deletecb(work);
	}
}

typedef struct GuardFrames {
	u8 a, b, c, d, e, f, g;
} GuardFrames;

#ifdef MUST_MATCH
// The tables are const in retail, but the compiler only keeps retail's direct-symbol `lwzu` reads
// when the globals are declared non-const; an unqualified `const` turns them into `@sda21` accesses.
// `volatile` on the first table stops CSE from reusing the base register for the loaded word.
extern volatile GuardFrames N_normal_guard_frames[];
extern GuardFrames N_superguard_frames[];
__declspec(section ".rodata") volatile GuardFrames N_normal_guard_frames[1] = {
	{ 0x0A, 0x09, 0x08, 0x08, 0x07, 0x06, 0x05 }
};
__declspec(section ".rodata") GuardFrames N_superguard_frames[1] = {
	{ 0x05, 0x04, 0x03, 0x03, 0x02, 0x01, 0x01 }
};
#else
extern const GuardFrames N_normal_guard_frames[];
extern const GuardFrames N_superguard_frames[];
const GuardFrames N_normal_guard_frames[1] = { { 0x0A, 0x09, 0x08, 0x08, 0x07, 0x06, 0x05 } };
const GuardFrames N_superguard_frames[1] = { { 0x05, 0x04, 0x03, 0x03, 0x02, 0x01, 0x01 } };
#endif

// Every instruction matches. The two guard-frame tables are independent global copies, so the
// scheduler is free to emit them in either order; the redundant pointer cast below pins the normal
// table's `lwzu` ahead of the super table's, matching retail. It changes load order only, so it is
// kept under MUST_MATCH with the plain element read in the natural build.
s32 BattleActionCommandCheckDefence(BattleUnit* unit, BattleWeapon* weapon) {
	BattleWork* work = _battleWorkPointer;
	BattleWorkPad* pad = &work->actionCommands.pad;
	GuardFrames normal;
	GuardFrames super;
	s16* guardFrames = &work->actionCommands.guardFrames;
	s32 defended = 0;
	s32 index;
	s32 i;
	s32 superFrames;
	s32 count;
	s32 normalFrames;
	s32 trigger;

#ifdef MUST_MATCH
	normal = *(GuardFrames*)&N_normal_guard_frames[0];
#else
	normal = N_normal_guard_frames[0];
#endif
	super = N_superguard_frames[0];
	work->actionCommands.mDefenseResult = 0;

	index = unit->badgesEquipped.Unsimplifier + 3;
	index -= unit->badgesEquipped.Simplifier;
	if (index < 0) {
		index = 0;
	}
	if (index > 6) {
		index = 6;
	}

	superFrames = ((u8*)&super)[index];
	normalFrames = ((u8*)&normal)[index];

	if (weapon->superguardType != 0 && unit->currentHp > 0) {
		if (*guardFrames > 0) {
			(*guardFrames)--;
			defended = 1;
			work->actionCommands.mDefenseResult = 5;
		}
		else if (unit->badgesEquipped.AutoCommand != 0) {
			defended = 1;
			work->actionCommands.mDefenseResult = 5;
		}
		else {
			for (i = 0; i < superFrames; i++) {
				trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x200);
				if (trigger) {
					break;
				}
			}
			if (i < superFrames) {
				count = 0;
				for (i = 0; i < 15; i++) {
					trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x100);
					if (trigger) {
						count++;
					}
					trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x200);
					if (trigger) {
						count++;
					}
				}
				if (count < 2) {
					defended = 1;
					work->actionCommands.mDefenseResult = 5;
				}
			}
		}
	}

	if (!defended) {
		if (*guardFrames > 0) {
			(*guardFrames)--;
			work->actionCommands.mDefenseResult = 4;
		}
		else {
			count = 0;
			for (i = 0; i < 15; i++) {
				trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x100);
				if (trigger) {
					count++;
				}
				trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x200);
				if (trigger) {
					count++;
				}
			}
			if (count >= 2) {
				work->actionCommands.mDefenseResult = 1;
			}
			else {
				for (i = 0; i < normalFrames; i++) {
					trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x100);
					if (trigger) {
						break;
					}
				}
				if (i >= normalFrames) {
					if (count > 0) {
						work->actionCommands.mDefenseResult = 1;
					}
					else {
						work->actionCommands.mDefenseResult = 2;
					}
				}
				else {
					work->actionCommands.mDefenseResult = 4;
				}
			}
		}
	}

	BtlPad_WorkInit(pad);
	return work->actionCommands.mDefenseResult;
}

#pragma dont_inline on // retail keeps the BattleACPadCheckRecordTrigger call instead of inlining it
u32 BattleACPadCheckRecordTrigger(s32 chan, s32 mask) {
	return _battleWorkPointer->actionCommands.pad.mButtonsPressedHistory[chan] & mask;
}
#pragma dont_inline reset

s32 BattleActionCommandGetDefenceResult(void) {
	return _battleWorkPointer->actionCommands.mDefenseResult;
}

void BattleActionCommandResetDefenceResult(void) {
	BattleWork* work = _battleWorkPointer;

	work->actionCommands.mDefenseResult = 0;
	BtlPad_WorkInit(&work->actionCommands.pad);
}

s32 BattleActionCommandGetDifficulty(BattleWork* work) {
	return work->actionCommands.mAcDifficulty;
}

void BattleActionCommandSetDifficulty(BattleWork* work, BattleUnit* unit, s32 difficulty) {
	s32 adjusted;

	work->actionCommands.mBaseAcDifficulty = difficulty;
	adjusted = difficulty;
	adjusted -= unit->badgesEquipped.Simplifier;
	adjusted += unit->badgesEquipped.Unsimplifier;
	if (adjusted < 0) {
		adjusted = 0;
	}
	if (adjusted > 6) {
		adjusted = 6;
	}
	work->actionCommands.mAcDifficulty = adjusted;
}

s32 BattleActionCommandGetPrizeLv(BattleWork* wp, BattleUnit* unit, s32 index) {
	s32 lv;

	if (unit != NULL && (u8)unit->damageCode == 0x1E) {
		return -1;
	}

	lv = index + (u8)wp->actionCommands.mAcDifficulty;
	lv--;
	if (lv < 0) {
		lv = 0;
	}
	if (lv >= 7) {
		lv = 6;
	}
	return lv;
}

s32 BattleACGetButtonIcon(s32 button, s32 status) {
	s32 icon = 0;

	switch (button) {
	case 0x100:
		if (status) {
			icon = 0x65;
		}
		else {
			icon = 0x64;
		}
		break;
	case 0x200:
		if (status) {
			icon = 0x67;
		}
		else {
			icon = 0x66;
		}
		break;
	case 0x400:
		if (status) {
			icon = 0x69;
		}
		else {
			icon = 0x68;
		}
		break;
	case 0x800:
		if (status) {
			icon = 0x6B;
		}
		else {
			icon = 0x6A;
		}
		break;
	case 0x20:
		if (status) {
			icon = 0x81;
		}
		else {
			icon = 0x80;
		}
		break;
	case 0x40:
		if (status) {
			icon = 0x7F;
		}
		else {
			icon = 0x7E;
		}
		break;
	case 0x10:
		if (status) {
			icon = 0x83;
		}
		else {
			icon = 0x82;
		}
		break;
	case 0x10000:
		if (status) {
			icon = 0x7C;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x20000:
		if (status) {
			icon = 0x76;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x40000:
		if (status) {
			icon = 0x78;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x80000:
		if (status) {
			icon = 0x7A;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x90000:
		if (status) {
			icon = 0x7D;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0xA0000:
		if (status) {
			icon = 0x77;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x50000:
		if (status) {
			icon = 0x7B;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x60000:
		if (status) {
			icon = 0x75;
		}
		else {
			icon = 0x79;
		}
		break;
	}

	return icon;
}

// Returns the current battle work pointer. Reading it through a call is what makes MWCC keep it in
// r30 and the shared gauge-boundary temp in r29, matching retail.
static inline BattleWork* getWorkPointer(void) {
	return _battleWorkPointer;
}

// Draws one gauge segment between two thresholds, both scaled by `type`/100 and offset from x.
// The four edge floats are declared as locals in the order that reproduces the original's f27-f30
// allocation (MWCC colours locals in reverse declaration order).
static inline void gaugeSegment(CameraEntry* cam, s32 x, s32 y, s32 type, s32 prev, s32 next, u8 r, u8 g,
                                u8 b, u8 a) {
	f32 yt, xb, yb, xa;

	xa = (f32)(x - 289 + type * prev / 100);
	yb = (f32)(y + 50);
	xb = (f32)(x - 289 + type * next / 100);
	yt = (f32)(y + 32);

	if (xb >= xa) {
		btlDispGXInit2DRasta();
		GXLoadPosMtxImm(cam->view, 0);
		btlDispGXQuads2DRasta(xa, yb, xb, yt, r, g, b, a);
	}
}

// Draws the full-width background segment (both edges at the left border, so no guard is needed).
static inline void gaugeBackground(CameraEntry* cam, s32 x, s32 y, u8 r, u8 g, u8 b, u8 a) {
	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(cam->view, 0);
	btlDispGXQuads2DRasta((f32)(x - 289), (f32)(y + 50), (f32)(x - 289), (f32)(y + 32), r, g, b, a);
}

// Same, but the right edge is `type * value / 100` measured from the left border (value is a 0..100 fill).
static inline void gaugeProgress(CameraEntry* cam, s32 x, s32 y, s32 type, s32 prev, f32 value, u8 r, u8 g,
                                 u8 b, u8 a) {
	s32 offa = type * prev / 100;
	struct { f32 xa, yb, xb, yt; } e;

	e.xa = (f32)(x - 289 + offa);
	e.yb = (f32)(y + 50);
	e.xb = (f32)(x - 289) + type * value / 100.0f;
	e.yt = (f32)(y + 32);

	if (e.xb >= e.xa) {
		btlDispGXInit2DRasta();
		GXLoadPosMtxImm(cam->view, 0);
		btlDispGXQuads2DRasta(e.xa, e.yb, e.xb, e.yt, r, g, b, a);
	}
}

// Draws a 2px-wide marker at a threshold's boundary position.
static inline void gaugeHighlight(CameraEntry* cam, s32 x, s32 y, s32 type, s32 threshold, u8 r, u8 g,
                                  u8 b, u8 a) {
	s32 off = type * threshold / 100;
	f32 yt, xb, yb, xa;

	xb = (f32)(x - 289 + off);
	yt = (f32)(y + 32);
	xa = xb - 2.0f;
	yb = (f32)(y + 50);

	if (xb >= xa) {
		btlDispGXInit2DRasta();
		GXLoadPosMtxImm(cam->view, 0);
		btlDispGXQuads2DRasta(xa, yb, xb, yt, r, g, b, a);
	}
}

#pragma optimize_for_size on // keeps divw for /100 instead of a magic multiply
void BattleAcDrawGauge(s32 x, s32 y, f32 ratio, s32 type, s32 count, s32 t0, s32 t1, s32 t2, s32 flags) {
	BattleWork* work = getWorkPointer();
	CameraEntry* cam;
	GXColor color = (GXColor){ 0x00, 0xE4, 0x86, 0xFF };
	Vec pos;

	cam = camGetPtr(CAMERA_2D);

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = (f32)(x - 200);
	pos.y = (f32)(y + 25);
	iconDispGx(1.0f, &pos, 0x10, 0x8C);

	if (count >= 2) {
		gaugeSegment(cam, x, y, type, 0, t0, 0x21, 0x21, 0x75, 0xFF);
		if (t1 - t0 > 0) {
			gaugeSegment(cam, x, y, type, t0, t1, 0x1D, 0x23, 0xA3, 0xFF);
		}
		if (t2 - t1 > 0) {
			gaugeSegment(cam, x, y, type, t1, t2, 0x46, 0x0C, 0xB4, 0xFF);
		}
		if (100 - t2 > 0) {
			gaugeSegment(cam, x, y, type, t2, 100, 0x73, 0x0D, 0x13, 0xFF);
		}
	}

	if (count >= 2) {
		gaugeBackground(cam, x, y, 0x2D, 0x38, 0xD2, 0xFF);
		if (t0 > 0) {
			gaugeHighlight(cam, x, y, type, t0, 0x54, 0x28, 0xD1, 0xFF);
		}
		if (t1 - t0 > 0) {
			gaugeHighlight(cam, x, y, type, t1, 0x7D, 0x2C, 0xB5, 0xFF);
		}
		if (t2 - t1 > 0) {
			gaugeHighlight(cam, x, y, type, t2, 0xA1, 0x1B, 0x55, 0xFF);
		}
	}

	if (!(ratio < 0.0f)) {
		if (flags & 3) {
			if (flags & 1) {
				color.r = (u8)(color.r * ((u8*)work)[0x1F2C] / 255);
				color.g = (u8)(color.g * ((u8*)work)[0x1F2D] / 255);
				color.b = (u8)(color.b * ((u8*)work)[0x1F2E] / 255);
				color.a = (u8)(color.a * ((u8*)work)[0x1F2F] / 255);
			} else {
				color = (GXColor){ 0xFF, 0x00, 0x00, 0xFF };
			}
			if (ratio > 0.0f) {
				struct { f32 xa, yb, xb, yt; } e;

				e.xa = (f32)(x - 289);
				e.yb = (f32)(y + 50);
				e.xb = (f32)type * ratio + (f32)(x - 289);
				e.yt = (f32)(y + 32);

				if (e.xb >= e.xa) {
					btlDispGXInit2DRasta();
					GXLoadPosMtxImm(cam->view, 0);
					btlDispGXQuads2DRasta(e.xa, e.yb, e.xb, e.yt, color.r, color.g, color.b, color.a);
				}
			}
		} else {
			if (!(ratio <= 0.0f)) {
				f32 ratio100 = ratio * 100.0f;
				f32 end;

				end = ratio100;
				if (ratio100 > (f32)t0) {
					end = (f32)t0;
				}
				if (end > 0.0f) {
					gaugeProgress(cam, x, y, type, 0, end, 0x00, 0xE4, 0x86, 0xFF);
				}
				end = ratio100;
				if (ratio100 > (f32)t1) {
					end = (f32)t1;
				}
				if (end - (f32)t0 > 0.0f) {
					gaugeProgress(cam, x, y, type, t0, end, 0x2E, 0xB4, 0xF2, 0xFF);
				}
				end = ratio100;
				if (ratio100 > (f32)t2) {
					end = (f32)t2;
				}
				if (end - (f32)t1 > 0.0f) {
					gaugeProgress(cam, x, y, type, t1, end, 0x75, 0x70, 0xFF, 0xFF);
				}
				if (ratio100 > 100.0f) {
					ratio100 = 100.0f;
				}
				if (ratio100 - (f32)t2 > 0.0f) {
					gaugeProgress(cam, x, y, type, t2, ratio100, 0xF3, 0x04, 0xBC, 0xFF);
				}
			}
		}
	}
}
#pragma optimize_for_size reset


void BattleAcGaugeSeInit(void) {
	BattleWork* work = _battleWorkPointer;

	work->actionCommands.field_0x288 = psndSFXOn("SFX_AC_POWER_GAUGE1");
	*(f32*)&work->actionCommands.field_0x28C[0] = 0.0f;
}

void BattleAcGaugeSeUpdate(f32 value) {
	BattleWork* work = _battleWorkPointer;

	if (value < 0.0f) {
		value = 0.0f;
	}
	if (value > 100.0f) {
		value = 100.0f;
	}
	*(f32*)&work->actionCommands.field_0x28C[0] = value;
	if ((u32)work->actionCommands.field_0x288 != -1) {
		psndSFX_pit(work->actionCommands.field_0x288, (s32)(4092.0f * *(f32*)&work->actionCommands.field_0x28C[0] * 0.01f));
	}
}

void BattleAcGaugeSeDelete(void) {
	BattleWork* work = _battleWorkPointer;

	if ((u32)work->actionCommands.field_0x288 != -1) {
		psndSFXOff(work->actionCommands.field_0x288);
		work->actionCommands.field_0x288 = -1;
	}
}
