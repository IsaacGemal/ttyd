#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/ac/ac_gauge_timing2.h"
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
extern const s32 dat_80421ad8;

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
const f64 double_6_80300058 = 6.0;
const f64 double_to_int_80300060 = 4503601774854144.0;
const f64 double_to_int_mask_80300068 = 4503599627370496.0;

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

// NONMATCHING (91.25%): logic matches; MWCC's register allocator assigns work/disp to r23/r22
// instead of r25/r23, and pool-emits the s32->f32 conversion constant at .rodata 0xC8 instead of
// reusing double_to_int at 0x80. Byte size (1628) matches.
s32 battleAcMain_GaugeTiming2(BattleWork* work) {
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	GaugeTiming2Work* dec = (GaugeTiming2Work*)((u8*)work + 0x1F4C);
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
			const char* ng = str_80300070;
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
			const char* pone = str_80300084;
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				if (dec->state[i] == 0) {
					if (__fabs(dec->progress - dec->value[i]) <= double_6_80300058) {
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
			for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x0C]; i++) {
				if (dec->state[i] == 0 && __fabs(dec->progress - dec->value[i]) <= double_6_80300058) {
					found = 1;
					break;
				}
			}
			autoFlag = found;
		}

		if (triggered || autoFlag) {
			const char* ng2 = str_80300070;
			const char* ok = str_80300094;
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
				if (diff <= double_6_80300058) {
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

// NONMATCHING (98.86%): under this file's current -O4,p, MWCC folds the disp pointer into work
// (`mr r31, r4`, then 0x1f34(r31)..0x1f40(r31)); the original materialises it (`addi r31, r4, 0x1f20`).
// Building this file with -O2,p instead makes the function 100% (verified); the source below is the natural one.
void battleAcDisp_GaugeTiming2(CameraId camId, void* param) {
	BattleACDispParams* disp = &((BattleWork*)param)->actionCommands.dispParams;

	switch (((BattleWork*)param)->actionCommands.mAcState) {
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

const char str_80300070[] = "SFX_AC_COMMAND_NG1";
const char str_80300084[] = "SFX_AC_PONE1";
const char str_80300094[] = "SFX_AC_COMMAND_OK1";
