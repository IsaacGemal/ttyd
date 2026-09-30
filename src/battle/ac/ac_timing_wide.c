#include "battle/ac/ac_timing_wide.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// Action-command work area for TimingWide, at _battleWorkPointer + 0x1F4C. No header declares it.
typedef struct TimingWideDecWork {
	s32 field_0x00;   // 0x00, icon type
	s8 field_0x04;    // 0x04, press state (-1, 0, 1)
	u8 pad_0x05[3];   // 0x05
	s32 field_0x08;   // 0x08, timing counter
	s32 field_0x0C;   // 0x0C, display sub-state
	s32 field_0x10;   // 0x10, end timer
} TimingWideDecWork;

// rodata vectors, one array (the target uses one base register and offsets).
extern const Vec vec3_802fd630[12];

// sdata2 constants.
extern const f32 float_neg200_80420508;
extern const f32 float_50_8042050c;
extern const f32 float_1_80420510;
extern const f32 float_30_80420514;
extern const f32 float_neg300_80420518;
extern const f32 float_0_8042051c;

static void actionCommandDisp(f32 x, f32 y);

// optimize_for_size: the original actionCommandDisp prologue uses stmw/lmw.
#pragma optimize_for_size on
static void actionCommandDisp(f32 x, f32 y) {
	Vec* vecs = (Vec*)vec3_802fd630;
	BattleWork* work = _battleWorkPointer;
	BattleActions* ac = &work->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	TimingWideDecWork* dec;
	s16 up;
	s16 down;

	camGetPtr(CAMERA_2D);
	dec = (TimingWideDecWork*)&ac->field_0x2BC[0];

	switch (dec->field_0x00) {
	case 0x100:
		up = 0x65;
		down = 0x64;
		break;
	case 0x200:
		up = 0x67;
		down = 0x66;
		break;
	case 0x400:
		up = 0x69;
		down = 0x68;
		break;
	case 0x800:
		up = 0x6B;
		down = 0x6A;
		break;
	case 0x40000:
		up = 0x78;
		down = 0x79;
		break;
	case 0x80000:
		up = 0x7A;
		down = 0x79;
		break;
	default:
		up = 0x78;
		down = 0x79;
		break;
	}

	switch (dec->field_0x0C) {
	case 0:
		if (dec->field_0x04 > 0) {
			Vec v = vecs[0];
			v.x = float_neg200_80420508 + x;
			v.y = float_50_8042050c + y;
			iconDispGx(&v, 0x10, down, float_1_80420510);
		}
		else {
			Vec v = vecs[1];
			v.x = float_neg200_80420508 + x;
			v.y = float_50_8042050c + y;
			iconDispGx(&v, 0x10, up, float_1_80420510);
		}
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		if (!(*(s32*)&ac->field_34[0x00] & 0x10)) {
			if (dec->field_0x04 > 0) {
				Vec v = vecs[2];
				v.x = float_neg200_80420508 + x;
				v.y = float_50_8042050c + y;
				iconDispGx(&v, 0x10, down, float_1_80420510);
			}
			else {
				Vec v = vecs[3];
				v.x = float_neg200_80420508 + x;
				v.y = float_50_8042050c + y;
				iconDispGx(&v, 0x10, up, float_1_80420510);
			}
		}
		else {
			if (dec->field_0x04 > 0) {
				Vec v = vecs[4];
				v.x = float_neg200_80420508 + x;
				v.y = float_50_8042050c + y;
				iconDispGx(&v, 0x10, up, float_1_80420510);
			}
			else {
				Vec v = vecs[5];
				v.x = float_neg200_80420508 + x;
				v.y = float_50_8042050c + y;
				iconDispGx(&v, 0x10, down, float_1_80420510);
			}
		}
		break;
	case 1:
		if (dec->field_0x04 != -1) {
			if (dec->field_0x08 >= *(s32*)&ac->field_34[0x0C] &&
			    dec->field_0x08 < *(s32*)&ac->field_34[0x10]) {
				if (dec->field_0x04 > 0) {
					Vec v = vecs[6];
					v.x = float_neg200_80420508 + x;
					v.y = float_50_8042050c + y;
					iconDispGx(&v, 0x10, up, float_1_80420510);
				}
				else {
					Vec v = vecs[7];
					v.x = float_neg200_80420508 + x;
					v.y = float_50_8042050c + y;
					iconDispGx(&v, 0x10, down, float_1_80420510);
				}
			}
			else {
				if (dec->field_0x04 > 0) {
					Vec v = vecs[8];
					v.x = float_neg200_80420508 + x;
					v.y = float_50_8042050c + y;
					iconDispGx(&v, 0x10, down, float_1_80420510);
				}
				else {
					Vec v = vecs[9];
					v.x = float_neg200_80420508 + x;
					v.y = float_50_8042050c + y;
					iconDispGx(&v, 0x10, up, float_1_80420510);
				}
			}
		}
		else {
			if (dec->field_0x08 >= *(s32*)&ac->field_34[0x0C]) {
				Vec v = vecs[10];
				v.x = float_neg200_80420508 + x;
				v.y = float_50_8042050c + y;
				iconDispGx(&v, 0x10, down, float_1_80420510);
			}
			else {
				Vec v = vecs[11];
				v.x = float_neg200_80420508 + x;
				v.y = float_50_8042050c + y;
				iconDispGx(&v, 0x10, up, float_1_80420510);
			}
		}
		break;
	default:
		break;
	}

	(*(s32*)&disp->field_0x0[0])++;
}
#pragma optimize_for_size reset

void battleAcDelete_TimingWide(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EC;
}


void battleAcDisp_TimingWide(CameraId camId, void* param) {
	BattleActions* ac = &((BattleWork*)param)->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	TimingWideDecWork* dec = (TimingWideDecWork*)&ac->field_0x2BC[0];
	s32 flag;

	switch (dec->field_0x0C) {
	case 0:
	case 1:
	case 2:
		if (*(s32*)&ac->field_34[0x00] & 1) {
			disp->iconX = float_30_80420514;
		}
		else {
			disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_80420518, float_30_80420514);
		}
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter > 0) {
			disp->counter--;
		}
		break;
	case 3:
		actionCommandDisp(disp->iconX, disp->iconY);
		break;
	case 4:
	case 5:
		flag = 1;
		if ((*(s32*)&ac->field_34[0x00] & 2) &&
		    (ac->result & 2)) {
			flag = 0;
		}
		if ((*(s32*)&ac->field_34[0x00] & 4) &&
		    !(ac->result & 2)) {
			flag = 0;
		}
		if (disp->counter >= 0x28) {
			if (flag != 0) {
				disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_80420514,
				                            float_neg300_80420518);
			}
			else {
				disp->iconX = float_30_80420514;
			}
		}
		else {
			disp->iconX = float_30_80420514;
		}
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter < 0x3C) {
			disp->counter++;
		}
		break;
	default:
		break;
	}
}


s32 battleAcResult_TimingWide(BattleWork* work) {
	return work->actionCommands.result;
}


// optimize_for_size: battleAcMain's prologue uses stmw/lmw.
#pragma optimize_for_size on
// NONMATCHING (96.24%): the state machine and all logic match; MWCC picked different callee-saved
// registers. The original keeps work r30, dec r31, disp r29 (saving r28-r31); ours keeps work r31, disp r30,
// autoCommand r29 and rematerialises dec (saving r29-r31). ~141 instructions differ only in register fields.
s32 battleAcMain_TimingWide(BattleWork* work) {
	TimingWideDecWork* dec = (TimingWideDecWork*)((u8*)work + 0x1F4C);
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	s32 autoCommand = 0;
	s32 cond;

	if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
		autoCommand = 1;
	}

	switch (work->actionCommands.mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = float_neg300_80420518;
		disp->iconY = float_0_8042051c;
		disp->counter = 0x14;
		dec->field_0x0C = 0;
		dec->field_0x08 = 0;
		switch (*(s32*)&work->actionCommands.field_34[4]) {
		case 0:
			dec->field_0x00 = 0x40000;
			dec->field_0x04 = 0;
			break;
		case 1:
			dec->field_0x00 = 0x80000;
			dec->field_0x04 = 0;
			break;
		case 2:
			dec->field_0x00 = 0x40000;
			dec->field_0x04 = -1;
			break;
		case 3:
			dec->field_0x00 = 0x80000;
			dec->field_0x04 = -1;
			break;
		case 5:
			dec->field_0x00 = 0x100;
			dec->field_0x04 = 1;
			break;
		default:
			dec->field_0x00 = 0x40000;
			dec->field_0x04 = 0;
			break;
		}
		work->actionCommands.outputParams[0] = 0;
		work->actionCommands.outputParams[1] = 0;
		work->actionCommands.outputParams[2] = 0;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		break;
	case 0x64:
		if ((work->actionCommands.field_0x4 & 1) &&
		    (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
			if ((s32)irand(100) >= 0) {
				work->actionCommands.field_0x8 = 0;
				if (*(s32*)&work->actionCommands.field_34[0x0C] > 0) {
					work->actionCommands.field_0x8 = irand(*(s32*)&work->actionCommands.field_34[0x0C]);
				}
				if (work->actionCommands.field_0x8 <= 0) {
					work->actionCommands.field_0x8 = *(s32*)&work->actionCommands.field_34[0x08] + 2;
				}
			}
			else {
				s32 r = (s32)irand(*(s32*)&work->actionCommands.field_34[0x10] -
				                   *(s32*)&work->actionCommands.field_34[0x0C]);
				work->actionCommands.field_0x8 = *(s32*)&work->actionCommands.field_34[0x0C] + r;
			}
			work->actionCommands.mAcState = 0x3EA;
			dec->field_0x0C = 2;
		}
		else {
			if (*(s32*)&work->actionCommands.field_34[4] == 4) {
				work->actionCommands.mAcState = 0x3E8;
			}
			else {
				work->actionCommands.mAcState = 0x3E9;
			}
			dec->field_0x0C = 1;
		}
		break;
	case 0x3E8:
		if (BattlePadCheckTrigger(dec->field_0x00)) {
			work->actionCommands.outputParams[1] = 1;
			work->actionCommands.mAcState = 0x3E9;
		}
		break;
	case 0x3E9:
		dec->field_0x08++;
		if (dec->field_0x08 > *(s32*)&work->actionCommands.field_34[0x08]) {
			work->actionCommands.result = 0;
			work->actionCommands.mAcState = 0x3EC;
			dec->field_0x0C = 4;
			if (*(s32*)&work->actionCommands.field_34[0x00] & 8) {
				work->actionCommands.mAcState = 0x3EB;
				dec->field_0x0C = 3;
			}
			break;
		}
		cond = 0;
		switch (dec->field_0x04) {
		case 1:
			if (BattlePadCheckTrigger(dec->field_0x00)) {
				cond = 1;
			}
			break;
		case -1:
			if (BattlePadCheckUp(dec->field_0x00)) {
				cond = 1;
			}
			break;
		default:
			if (!BattlePadCheckNow(dec->field_0x00)) {
				cond = 1;
			}
			break;
		}
		if (dec->field_0x08 >= *(s32*)&work->actionCommands.field_34[0x0C] &&
		    dec->field_0x08 < *(s32*)&work->actionCommands.field_34[0x10]) {
			work->actionCommands.outputParams[2] = 1;
		}
		else {
			work->actionCommands.outputParams[2] = 0;
		}
		if (autoCommand) {
			if (work->actionCommands.outputParams[2] == 0) {
				cond = 0;
			}
			else {
				cond = 1;
			}
		}
		if (cond != 0) {
			if (work->actionCommands.outputParams[2] != 0) {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
				work->actionCommands.outputParams[0] = 1;
			}
			else {
				work->actionCommands.result = 0;
				work->actionCommands.outputParams[0] = -1;
			}
			work->actionCommands.mAcState = 0x3EC;
			dec->field_0x0C = 4;
			if (*(s32*)&work->actionCommands.field_34[0x00] & 8) {
				work->actionCommands.mAcState = 0x3EB;
				dec->field_0x0C = 3;
			}
		}
		break;
	case 0x3EA:
		dec->field_0x08++;
		work->actionCommands.field_0x8--;
		if (dec->field_0x08 > *(s32*)&work->actionCommands.field_34[0x08]) {
			work->actionCommands.result = 0;
			return 0;
		}
		if (work->actionCommands.field_0x8 < 0) {
			if (dec->field_0x08 >= *(s32*)&work->actionCommands.field_34[0x0C] &&
			    dec->field_0x08 < *(s32*)&work->actionCommands.field_34[0x10]) {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
				work->actionCommands.outputParams[0] = 1;
			}
			else {
				work->actionCommands.result = 0;
				work->actionCommands.outputParams[0] = -1;
			}
			return 0;
		}
		break;
	case 0x3EC:
		dec->field_0x10 = 0x3C;
		work->actionCommands.mAcState = 0x3ED;
		dec->field_0x0C = 5;
		// fallthrough
	case 0x3ED:
		dec->field_0x10--;
		if (dec->field_0x10 <= 0) {
			work->actionCommands.mAcState = 0x3EE;
			dec->field_0x0C = 6;
		}
		break;
	case 0x3EE:
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
#pragma optimize_for_size reset
