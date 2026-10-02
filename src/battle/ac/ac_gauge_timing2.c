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

extern const f32 float_neg300_80421b38;
extern const f32 float_30_80421b3c;
extern const f32 float_0_80421b00;
extern const f32 float_4_80421b40;
extern const f32 float_6_80421b44;
extern const f32 float_100_80421b14;
extern const f32 float_neg288_80421af4;
extern const f32 float_49_80421af8;
extern const f32 float_8_80421afc;
extern const f32 float_1_80421b04;
extern const f32 float_2_80421b08;
extern const f32 float_16_80421b0c;
extern const f32 float_neg1_80421b10;
extern const f32 float_23_80421b18;
extern const f32 float_0p75_80421b1c;
extern const f32 float_28_80421b20;
extern const f32 float_neg225_80421b24;
extern const f32 float_55_80421b28;
extern const f32 float_25_80421b2c;
extern const f32 float_neg200_80421b30;
extern const f32 float_60_80421b34;
extern const s32 dat_80421ad8;
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

extern const char str_80300070[];
extern const char str_80300084[];
extern const char str_80300094[];

static void actionCommandDisp(f32 x, f32 y);

// The callee-saved GPR colouring is rotated by two (dec=r29 vs r27, autoFlag=r28 vs r26, loop
// index r30 vs r28, found/done swap r26/r29); a few load-scheduling slots in the `case 0`
// initialiser and the autoFlag block are the only structural diffs.
// NONMATCHING (96.48%): register rotation above; byte size 1628 matches.
s32 battleAcMain_GaugeTiming2(BattleWork* work) {
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
		disp->iconX = float_neg300_80421b38;
		disp->iconY = float_0_80421b00;
		disp->counter = 0x14;
		disp->value = float_0_80421b00;
		disp->field_0xC = dat_80421ad8;
		dec->frame = 0;
		dec->field_0x08 = 0;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		dec->maxFrame = *(s32*)&work->actionCommands.field_34[0x08];
		dec->field_0x10 = 0;
		dec->progress = float_0_80421b00;
		work->actionCommands.outputParams[0] = 0;
		dec->total = *(s32*)&work->actionCommands.field_34[0x0C] * 0x3C;
		for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
			dec->value[i] = (f32)((100 / *(s32*)&work->actionCommands.field_34[0x0C]) * (i + 1));
			dec->value[i] -= float_4_80421b40;
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
		dec->progress = float_100_80421b14 * (f32)dec->field_0x08 / (f32)dec->maxFrame;

		{
			const char* ng = "SFX_AC_COMMAND_NG1";
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				if (dec->state[i] == 0) {
					if (dec->progress > float_6_80421b44 + dec->value[i]) {
						dec->state[i] = -1;
						psndSFXOn(ng);
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

		if (autoFlag) {
			s32 found = 0;
			autoFlag = 0;
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				if (dec->state[i] == 0 && __fabs(dec->progress - dec->value[i]) <= 6.0) {
					found = 1;
					break;
				}
			}
			if (found) {
				autoFlag = 1;
			}
		}

		if (triggered || autoFlag) {
			const char* ng2 = "SFX_AC_COMMAND_NG1";
			const char* ok = "SFX_AC_COMMAND_OK1";
			s32 done = 0;
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				f64 diff;
				if (dec->state[i] != 0) {
					continue;
				}
				if (done) {
					continue;
				}
				diff = __fabs(dec->progress - dec->value[i]);
				if (diff <= 6.0) {
					if (autoFlag == 0 && dec->flag[i] != 0) {
						dec->state[i] = -1;
						psndSFXOn(ng2);
					}
					else {
						dec->state[i] = 1;
						work->actionCommands.outputParams[0]++;
						psndSFXOn(ok);
					}
					done = 1;
				}
				else {
					if (diff < __fabs(dec->value[1] - dec->value[0])) {
						dec->flag[i] = 1;
						if (i == *(s32*)&work->actionCommands.field_34[0x0C] - 1) {
							dec->state[i] = -1;
							psndSFXOn(ng2);
							done = 1;
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
		work->actionCommands.resultcb = NULL;
		work->actionCommands.dispcb = NULL;
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
		disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_80421b38, float_30_80421b3c);
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
			disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_80421b3c, float_neg300_80421b38);
		}
		else {
			disp->iconX = float_30_80421b3c;
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
// NONMATCHING (98.69%): one top-of-function instruction-scheduling slot.
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

	f30 = float_49_80421af8 + y;
	f31 = float_neg288_80421af4 + x;
	f27 = f30 - float_8_80421afc;

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
		    f31, f27, float_0_80421b00, float_1_80421b04, float_1_80421b04, 0x43, &color, 0);
	}

	f26 = float_2_80421b08 + (float_8_80421afc + f31);

	{
	for (i = 0; i < n; i++) {
		s32 off = i * 4;
		if (i != n - 1) {
			GXColor color = *(GXColor*)&dat_80421ae0;
			((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
			    f26 + (f32)off, f27, float_0_80421b00, float_1_80421b04, float_1_80421b04, 0x44, &color, 0);
		}
		else {
			GXColor color = *(GXColor*)&dat_80421ae4;
			((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
			    f26 + (f32)off + (f32)rem, f27, float_0_80421b00, float_1_80421b04, float_1_80421b04, 0x44, &color, 0);
		}
	}
	}

	{
		f32 t16 = float_16_80421b0c + f31;
		GXColor color = *(GXColor*)&dat_80421ae8;
		((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
		    t16 + (f32)count, f27, float_0_80421b00, float_neg1_80421b10, float_1_80421b04, 0x43, &color, 0);
	}

	{
		f32 v = dec->progress / float_100_80421b14;
		if (v > float_1_80421b04) {
			v = float_1_80421b04;
		}
		disp->value = v;
	}

	cam = camGetPtr(CAMERA_2D);

	{
		f32 barY = f30 - float_16_80421b0c;
		f32 progressX = f31 + (f32)dec->total * dec->progress / float_100_80421b14;
		if (progressX >= f31) {
			btlDispGXInit2DRasta();
			GXLoadPosMtxImm(cam->view, 0);
			btlDispGXQuads2DRasta(f31, f30, progressX - float_1_80421b04, barY, 0xFF, 0, 0, 0xFF);
		}
	}

	{
		f32 iy = float_23_80421b18 + y;
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
			t.x = f31 + (f32)dec->total * v / float_100_80421b14;
			t.y = iy;
			ICON_DISP(&t, 0x10, icon, float_0p75_80421b1c);
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
			t.x = float_neg225_80421b24 + x + float_28_80421b20;
			t.y = float_55_80421b28 + y;
			ICON_DISP(&t, 0x10, icon0, float_1_80421b04);
			break;
		}
		case 0x3E8:
			if (dec->field_0x10 != 0) {
				Vec t = vec3_80300028;
				t.x = float_neg225_80421b24 + x + float_28_80421b20;
				t.y = float_55_80421b28 + y;
				ICON_DISP(&t, 0x10, icon1, float_1_80421b04);
			}
			else {
				Vec t = vec3_8030004c;
				t.x = float_neg225_80421b24 + x + float_28_80421b20;
				t.y = float_55_80421b28 + y;
				ICON_DISP(&t, 0x10, icon0, float_1_80421b04);
			}
			break;
		default:
			break;
		}
	}

	{
		f32 ty;
		f32 sy = float_25_80421b2c + y;
		ty = sy - float_25_80421b2c;

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
				    f31 + (f32)dec->total * dec->value[i] / float_100_80421b14, ty, float_0_80421b00,
				    float_1_80421b04, float_1_80421b04, icon, &color, 0);
			}
		}

		{
			Mtx mtx2;
			Mtx mtx;
			Mtx scale;
			s32 number = ac->outputParams[0] + *(s32*)&ac->field_34[0x10];
			f32 tx = float_neg200_80421b30 + x;
			PSMTXTrans(mtx, tx + (f32)dec->total - float_60_80421b34, sy, float_0_80421b00);
			PSMTXScale(scale, float_1_80421b04, float_1_80421b04, float_1_80421b04);
			PSMTXConcat(mtx, scale, mtx2);
			iconNumberDispGx(mtx2, number, 0, *(GXColor*)&dat_80421af0);
		}
	}
}

