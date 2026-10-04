#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "battle/ac/ac_gauge_timing2.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include <string.h>
#pragma pool_data on // the original addresses its .rodata through one base register

extern const f32 float_neg288_80421af4;
#ifdef MUST_MATCH
// Match-only: a non-const .sdata2 definition stops MWCC hoisting the `lwz` of this const global above
// the surrounding case-0 stores; the hoist shifts the whole callee-saved GPR colouring (96.74 -> 98.38).
__declspec(section ".sdata2") s32 dat_80421ad8 = -1;
#else
const s32 dat_80421ad8 = -1;
#endif
extern const s32 dat_80421adc;
extern const s32 dat_80421ae0;
extern const s32 dat_80421ae4;
extern const s32 dat_80421ae8;
extern const s32 dat_80421aec;
extern const s32 dat_80421af0;

// .rodata objects of this unit: ten zero Vecs, then the float-conversion doubles.
const Vec vec3_802fffe0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fffec = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802ffff8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300004 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300010 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_8030001c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300028 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300034 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300040 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_8030004c = { 0.0f, 0.0f, 0.0f };

// GaugeTiming2's per-battle action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct GaugeTiming2Work {
	s32 frame;      // 0x00
	f32 progress;   // 0x04
	s32 field_0x08; // 0x08
	s32 maxFrame;   // 0x0C
	s32 field_0x10; // 0x10
	s32 buttonMask; // 0x14
	f32 value[8];   // 0x18
	s8 state[8];    // 0x38
	s8 flag[8];     // 0x40
	u32 sound[8];   // 0x48
	u16 total;      // 0x68
	s8 field_0x6A;  // 0x6A
	u8 pad_0x6B;    // 0x6B
	s32 timer;      // 0x6C
} GaugeTiming2Work;


static void actionCommandDisp(f32 x, f32 y);

// Defining dat_80421ad8 non-const (see the .sdata2 definition at the top) stops MWCC hoisting its
// `lwz` above the case-0 stores and fixes the case-0 schedule. Reading
// `autoFlag` into a `doCheck` temp before zeroing it (`s32 doCheck = autoFlag; autoFlag = 0; if
// (doCheck)`) pins the auto-command probe's `li <flag>,0` before its `beq`, which the scheduler only
// did in the original, and lifts the function to 98.73 (#ifdef-free source change: the temp is
// coalesced away by the allocator). Giving the ng loop's counter function scope (`s32 m;` at the top,
// `for (m = 0; ...)` in the loop) instead of a block `{ s32 m; ... }` is what nudges the allocator:
// 99.03 -> 99.09 (the ng/big counters land in the original's r28). The auto-command probe's `p` and
// the big SFX loop's `q` must stay block-scoped (function scope costs 0.1-0.2). The original still
// packs the long-lived webs low (autoFlag=r26, dec=r27) and the later ones high (triggered=r29,
// sfx pointers=r30), while ours keeps dec=r29, autoFlag=r30, triggered=r27, ng/pone strings=r26/r28;
// `add r28, r27, r24` (target) vs `add r23, r29, r24` (ours) shows the pone value-pointer/string
// pair still lands in swapped free registers. Calls, constants, memory accesses and byte size 1628
// all match. This is a pure allocator tie-break, not a source bug: the 6! permutation sweep of the
// function-level declarations, every relevant pragma (opt_lifetimes/opt_propagation/
// opt_common_subs/opt_dead_assignments/global_optimizer/peephole/opt_loop_invariants/
// opt_strength_reduction/optimize_for_size/scheduling/optimization_level/defer_codegen), an `ac`
// intermediate for dec/disp/unit, `&actionCommands.field_0x2BC[0]`, const/volatile/`char*` string
// locals, inlining the SFX string literals, `register`/`volatile` locals, and making the individual
// float_* externs non-const all leave it (or worsen it). Queued for the permuter.
// NONMATCHING (99.09%): callee-saved GPR colouring only (same registers, permuted).
s32 battleAcMain_GaugeTiming2(BattleWork* work) {
	GaugeTiming2Work* dec = (GaugeTiming2Work*)((u8*)work + 0x1F4C);
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	BattleUnit* unit = work->actionCommands.mAcUnit;
	s32 autoFlag = 0;
	s32 i;
	s32 m;

	if (unit->badgesEquipped.AutoCommand != 0) {
		autoFlag = 1;
	}

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = -300.f;
		disp->iconY = 0.0f;
		disp->counter = 0x14;
		disp->value = 0.0f;
		disp->field_0xC = dat_80421ad8;
		dec->frame = 0;
		dec->field_0x08 = 0;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		dec->maxFrame = *(s32*)&work->actionCommands.field_34[0x08];
		dec->field_0x10 = 0;
		dec->progress = 0.0f;
		work->actionCommands.outputParams[0] = 0;
		dec->total = *(s32*)&work->actionCommands.field_34[0x0C] * 0x3C;
		for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
			dec->value[i] = (f32)((100 / *(s32*)&work->actionCommands.field_34[0x0C]) * (i + 1));
			dec->value[i] -= 4.0f;
			dec->state[i] = 0;
			dec->flag[i] = 0;
			dec->sound[i] = -1;
		}
		dec->buttonMask = 0x100;
		work->actionCommands.outputParams[1] = 0;
		break;
	case 0x63:
		break;
	case 0x64:
		if ((work->actionCommands.field_0x4 & 1) && (unit->mTokenFlags & 0x10)) {
			if ((s32)irand(100) >= 0) {
				work->actionCommands.field_0x8 = 0x1E;
				dec->field_0x6A = 0;
			}
			else {
				dec->field_0x6A = 1;
				work->actionCommands.field_0x8 = 0x1E;
			}
			work->actionCommands.mAcState = 0x3E9;
			break;
		}
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8: {
		s32 p;
		s32 triggered = 0;
		s32 allSet;

		dec->frame++;
		if (BattlePadCheckTrigger(dec->buttonMask) != 0) {
			triggered = 1;
		}

		allSet = 1;
		for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
			if (dec->state[i] == 0) {
				allSet = 0;
				break;
			}
		}

		if (dec->frame > dec->maxFrame && allSet) {
			if (work->actionCommands.outputParams[0] > 0) {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
			}
			else {
				work->actionCommands.result = 0;
			}
			work->actionCommands.mAcState = 0x3EA;
			break;
		}

		dec->field_0x08++;
		dec->progress = 100.f * (f32)dec->field_0x08 / (f32)dec->maxFrame;

		{
			const char* ng = "SFX_AC_COMMAND_NG1";
			{
				for (m = 0; m < *(s32*)&work->actionCommands.field_34[0x0C]; m++) {
					if (dec->state[m] == 0) {
						if (dec->progress > 6.0f + dec->value[m]) {
							dec->state[m] = -1;
							psndSFXOn(ng);
						}
					}
				}
			}
		}

		dec->field_0x10 = 0;
		{
			const char* pone = "SFX_AC_PONE1";
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				if (dec->state[i] == 0) {
					if (__fabs(dec->progress - dec->value[i]) <= 6.0) {
						dec->field_0x10 = 1;
						if (dec->sound[i] == -1) {
							dec->sound[i] = psndSFXOn(pone);
						}
					}
				}
			}
		}

		{
			s32 doCheck = autoFlag;
			autoFlag = 0;
			if (doCheck) {
				s32 found;
				found = 0;
				{
					s32 p;
					for (p = 0; p < *(s32*)&work->actionCommands.field_34[0x0C]; p++) {
						if (dec->state[p] == 0 && __fabs(dec->progress - dec->value[p]) <= 6.0) {
							found = 1;
							break;
						}
					}
				}
				if (found) {
					autoFlag = 1;
				}
			}
		}

		if (triggered || autoFlag) {
			const char* ng2 = "SFX_AC_COMMAND_NG1";
			const char* ok = "SFX_AC_COMMAND_OK1\0";
			s32 done = 0;
			{
				s32 q;
				for (q = 0; q < *(s32*)&work->actionCommands.field_34[0x0C]; q++) {
					f64 diff;
					if (dec->state[q] != 0) {
						continue;
					}
					if (done) {
						continue;
					}
					diff = __fabs(dec->progress - dec->value[q]);
					if (diff <= 6.0) {
						if (autoFlag == 0 && dec->flag[q] != 0) {
							dec->state[q] = -1;
							psndSFXOn(ng2);
							done = 1;
						}
						else {
							dec->state[q] = 1;
							work->actionCommands.outputParams[0]++;
							psndSFXOn(ok);
							done = 1;
						}
					}
					else {
						if (diff < __fabs(dec->value[1] - dec->value[0])) {
							dec->flag[q] = 1;
							if (q == *(s32*)&work->actionCommands.field_34[0x0C] - 1) {
								dec->state[q] = -1;
								psndSFXOn(ng2);
								done = 1;
							}
						}
					}
				}
			}
		}
		break;
	}
	case 0x3E9:
		work->actionCommands.field_0x8--;
		if (work->actionCommands.field_0x8 < 0) {
			if (dec->field_0x6A != 0) {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
				work->actionCommands.outputParams[0] = irand(*(s32*)&work->actionCommands.field_34[0x0C]) + 1;
			}
			else {
				work->actionCommands.result = 0;
				work->actionCommands.outputParams[0] = 0;
			}
			work->actionCommands.field_0x30 |= 1;
			return 0;
		}
		break;
	case 0x3EA:
		work->actionCommands.mAcState = 0x3EB;
		// fallthrough
	case 0x3EB:
		work->actionCommands.outputParams[1] = 1;
		work->actionCommands.field_0x30 |= 1;
		dec->timer = 0x3C;
		work->actionCommands.mAcState = 0x3EC;
		// fallthrough
	case 0x3EC:
		dec->timer--;
		if (dec->timer <= 0) {
			work->actionCommands.mAcState = 0x3ED;
		}
		break;
	case 0x3ED:
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


s32 battleAcResult_GaugeTiming2(BattleWork* work) {
	return work->actionCommands.result;
}

void battleAcDisp_GaugeTiming2(CameraId camId, void* param) {
	BattleActions* ac = &((BattleWork*)param)->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	s32 state = ac->mAcState;

	switch (state) {
	case 0x63:
		disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.f, 30.f);
		// fallthrough
	case 0x64:
	case 0x3E8:
	case 0x3EA:
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter > 0) {
			disp->counter--;
		}
		break;
	case 0x3EB:
	case 0x3EC:
		if (disp->counter >= 0x28) {
			disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.f, -300.f);
		}
		else {
			disp->iconX = 30.f;
		}
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter < 0x3C) {
			disp->counter++;
		}
		break;
	}
}

void battleAcDelete_GaugeTiming2(BattleWork* work) {
	if ((work->actionCommands.field_0x30 & 1) == 0) {
		work->actionCommands.mAcState = 0x3EB;
	}
}

#ifdef MUST_MATCH
// Match-only: iconDispGx's header declares the icon id `s16`, but the original passes the value
// returned by BattleACGetButtonIcon straight through (no `extsh`); a differently-typed function
// pointer keeps the call a direct `bl iconDispGx`. Same workaround as ac_power_gauge_lv2.c.
#define ICON_DISP(v, f, id, s) (((void (*)(Vec*, s16, s32, f32))iconDispGx)((v), (f), (id), (s)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((v), (f), (id), (s))
#endif

// don't_inline: the original keeps actionCommandDisp as its own local function
// Written from scratch; byte size 1652 matches. All callee-saved register colouring now matches.
// The only remaining diff is one scheduler slot near the top: the original issues `lfs f1,
// float_neg288` before `lhz dec->total` and computes `f31` before `subic`, mine the reverse.
// NONMATCHING (99.21%): one top-of-function instruction-scheduling slot.
#pragma opt_common_subs off // MWCC's CSE pass shifts the callee-saved FPR set by one; off restores it
static void actionCommandDisp(f32 x, f32 y) {
	BattleWork* work = _battleWorkPointer;
	BattleActions* ac = &work->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	GaugeTiming2Work* dec;
	CameraEntry* cam;
	s32 n;
	s32 rem;
	s32 count;
	s32 last;
	s32 i;
	s32 off;
	f32 f31;
	f32 f30;
	f32 f27;
	f32 f26;

	camGetPtr(CAMERA_2D);
	dec = (GaugeTiming2Work*)&ac->field_0x2BC[0];
	cam = camGetPtr(CAMERA_2D);

	f30 = 49.0f + y;
	f31 = float_neg288_80421af4 + x;
	f27 = f30 - 8.0f;

	count = dec->total - 0x10;
	n = 0;
	rem = 0;
	if (count > 0) {
		n = (count + 3) / 4;
		rem = count - n * 4;
	}

	btlDispGXInit2DRasta();
	GXLoadPosMtxImm(cam->view, 0);

	{
		GXColor color = *(GXColor*)&dat_80421adc;
		((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
		    f31, f27, 0.0f, 1.0f, 1.0f, 0x43, &color, 0);
	}

	f26 = 2.0f + (8.0f + f31);

	{
	for (i = 0; i < n; i++) {
		s32 off = i * 4;
		if (i != n - 1) {
			GXColor color = *(GXColor*)&dat_80421ae0;
			((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
			    f26 + (f32)off, f27, 0.0f, 1.0f, 1.0f, 0x44, &color, 0);
		}
		else {
			GXColor color = *(GXColor*)&dat_80421ae4;
			((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
			    f26 + (f32)off + (f32)rem, f27, 0.0f, 1.0f, 1.0f, 0x44, &color, 0);
		}
	}
	}

	{
		f32 t16 = 16.0f + f31;
		GXColor color = *(GXColor*)&dat_80421ae8;
		((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
		    t16 + (f32)count, f27, 0.0f, -1.0f, 1.0f, 0x43, &color, 0);
	}

	{
		f32 v = dec->progress / 100.f;
		if (v > 1.0f) {
			v = 1.0f;
		}
		disp->value = v;
	}

	cam = camGetPtr(CAMERA_2D);

	{
		f32 barY = f30 - 16.0f;
		f32 progressX = f31 + (f32)dec->total * dec->progress / 100.f;
		if (progressX >= f31) {
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(cam->view, 0);
			btlDispGXQuads2DRasta(f31, f30, progressX - 1.0f, barY, 0xFF, 0, 0, 0xFF);
		}
	}

	{
		f32 iy = 23.0f + y;
		s32 cnt = *(s32*)&ac->field_34[0x0C];
		for (i = 0; i < cnt; i++) {
			f32 v = dec->value[i];
			s32 icon;
			Vec t;
			if (v <= dec->progress || __fabs(dec->progress - v) <= 6.0) {
				icon = 0x95;
			}
			else {
				icon = 0x91;
			}
			t = *(volatile Vec*)&vec3_802fffe0;
			t.x = f31 + (f32)dec->total * v / 100.f;
			t.y = iy;
			ICON_DISP(&t, 0x10, icon, 0.75f);
		}
	}

	{
		s32 icon1 = BattleACGetButtonIcon(0x100, 1);
		s32 icon0 = BattleACGetButtonIcon(0x100, 0);
		switch (ac->mAcState) {
		case 0x63:
		case 0x64:
		case 0x3EA:
		case 0x3EB:
		case 0x3EC: {
			Vec t = vec3_80300004;
			t.x = -225.0f + x + 28.0f;
			t.y = 55.0f + y;
			ICON_DISP(&t, 0x10, icon0, 1.0f);
			break;
		}
		case 0x3E8:
			if (dec->field_0x10 != 0) {
				Vec t = vec3_80300028;
				t.x = -225.0f + x + 28.0f;
				t.y = 55.0f + y;
				ICON_DISP(&t, 0x10, icon1, 1.0f);
			}
			else {
				Vec t = vec3_8030004c;
				t.x = -225.0f + x + 28.0f;
				t.y = 55.0f + y;
				ICON_DISP(&t, 0x10, icon0, 1.0f);
			}
			break;
		default:
			break;
		}
	}

	{
		f32 ty;
		f32 sy = 25.0f + y;
		ty = sy - 25.0f;

		for (i = 0; i < *(s32*)&ac->field_34[0x0C]; i++) {
			u16 icon;
			switch (dec->state[i]) {
			case 1:
				icon = 0x3E;
				break;
			case -1:
				icon = 0x3F;
				break;
			default:
				icon = 0x45;
				break;
			}
			if (icon != 0x45) {
				GXColor color = *(GXColor*)&dat_80421aec;
				((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
				    f31 + (f32)dec->total * dec->value[i] / 100.f, ty, 0.0f,
				    1.0f, 1.0f, icon, &color, 0);
			}
		}

		{
			Mtx mtx2;
			Mtx mtx;
			Mtx scale;
			s32 number = ac->outputParams[0] + *(s32*)&ac->field_34[0x10];
			f32 tx = -200.f + x;
			PSMTXTrans(mtx, tx + (f32)dec->total - 60.f, sy, 0.0f);
			PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
			PSMTXConcat(mtx, scale, mtx2);
			iconNumberDispGx(mtx2, number, 0, *(GXColor*)&dat_80421af0);
		}
	}
}
#pragma opt_common_subs reset

// Named .sdata2 objects: the six -1 words the display code passes as GXColors plus the first
// float constant. Defined after the code so MWCC emits them ahead of its own literal pool.
const s32 dat_80421adc = -1;
const s32 dat_80421ae0 = -1;
const s32 dat_80421ae4 = -1;
const s32 dat_80421ae8 = -1;
const s32 dat_80421aec = -1;
const s32 dat_80421af0 = -1;
const f32 float_neg288_80421af4 = -288.0f;
