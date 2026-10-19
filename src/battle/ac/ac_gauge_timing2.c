#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "battle/ac/ac_gauge_timing2.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include <string.h>
#pragma pool_data on // the original addresses its .rodata through one base register

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
// `lwz` above the case-0 stores and fixes the case-0 schedule. The original's callee-saved
// allocation (triggered=r29, m=r28, dec=r27, autoFlag=r26) is reached by declaring `triggered` and
// `m` at function scope before `dec`; the ng loop's counter is the function-scope `m`. The auto
// probe writes its result to a separate `hit` flag declared in the case block (it takes over
// autoFlag's r26 once the probe has read it).
// GT2_SFX: the original computes each sound name once in the loop preheader as a compiler temp; a
// MUST_MATCH `((char*)(u32)(name))` round trip makes MWCC's frontend hoist it the same way (as
// battle_stage_object's BSO_SFX and eff_fall's FALL_SFX). A named `const char*` holds a callee-saved
// register of its own and a plain literal is rematerialised at each call. OK_SFX's extra NUL spells
// out the word padding of the original's string object.
#ifdef MUST_MATCH
#define GT2_SFX(name) ((char*)(u32)(name))
#define OK_SFX "SFX_AC_COMMAND_OK1\0"
#else
#define GT2_SFX(name) (name)
#define OK_SFX "SFX_AC_COMMAND_OK1"
#endif

s32 battleAcMain_GaugeTiming2(BattleWork* work) {
	s32 triggered;
	s32 m;
	GaugeTiming2Work* dec = (GaugeTiming2Work*)((u8*)work + 0x1F4C);
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	BattleUnit* unit = work->actionCommands.mAcUnit;
	s32 autoFlag = 0;
	s32 i;

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
		s32 allSet;
		s32 hit;

		triggered = 0;
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
			{
				for (m = 0; m < *(s32*)&work->actionCommands.field_34[0x0C]; m++) {
					if (dec->state[m] == 0) {
						if (dec->progress > 6.0f + dec->value[m]) {
							dec->state[m] = -1;
							psndSFXOn(GT2_SFX("SFX_AC_COMMAND_NG1"));
						}
					}
				}
			}
		}

		dec->field_0x10 = 0;
		{
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				if (dec->state[i] == 0) {
					if (__fabs(dec->progress - dec->value[i]) <= 6.0) {
						dec->field_0x10 = 1;
						if (dec->sound[i] == -1) {
							dec->sound[i] = psndSFXOn(GT2_SFX("SFX_AC_PONE1"));
						}
					}
				}
			}
		}

		hit = 0;
		if (autoFlag) {
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
				hit = 1;
			}
		}

		if (triggered || hit) {
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
						if (hit == 0 && dec->flag[q] != 0) {
							dec->state[q] = -1;
							psndSFXOn(GT2_SFX("SFX_AC_COMMAND_NG1"));
							done = 1;
						}
						else {
							dec->state[q] = 1;
							work->actionCommands.outputParams[0]++;
							psndSFXOn(GT2_SFX(OK_SFX));
							done = 1;
						}
					}
					else {
						if (diff < __fabs(dec->value[1] - dec->value[0])) {
							dec->flag[q] = 1;
							if (q == *(s32*)&work->actionCommands.field_34[0x0C] - 1) {
								dec->state[q] = -1;
								psndSFXOn(GT2_SFX("SFX_AC_COMMAND_NG1"));
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
#define ICON_DISP(v, f, id, s) (((void (*)(f32, Vec*, s16, s32))iconDispGx)((s), (v), (f), (id)))
#else
#define ICON_DISP(v, f, id, s) iconDispGx((s), (v), (f), (id))
#endif

// don't_inline: the original keeps actionCommandDisp as its own local function
// `t16` spells out `-288.0f + x` again instead of reusing `f31`: MWCC's CSE then turns it into a
// copy of the first sum, which moves the `lfs` of -288.0 ahead of `lhz dec->total` in the first
// block the way the original scheduled it.
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

	f31 = -288.0f + x;
	f30 = 49.0f + y;
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
		f32 t16 = 16.0f + (-288.0f + x);
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
#ifdef MUST_MATCH
			// Volatile copy: retail reloads the template Vec here.
			t = *(volatile Vec*)&vec3_802fffe0;
#else
			t = vec3_802fffe0;
#endif
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

// Named .sdata2 objects: the six -1 words the display code passes as GXColors. Defined after the code
// so MWCC emits them ahead of its own literal pool.
const s32 dat_80421adc = -1;
const s32 dat_80421ae0 = -1;
const s32 dat_80421ae4 = -1;
const s32 dat_80421ae8 = -1;
const s32 dat_80421aec = -1;
const s32 dat_80421af0 = -1;
