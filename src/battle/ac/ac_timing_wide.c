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

static void actionCommandDisp(f32 x, f32 y);

// optimize_for_size: the original actionCommandDisp prologue uses stmw/lmw.
#pragma optimize_for_size on
static void actionCommandDisp(f32 x, f32 y);

// Holding the work-area pointer in `ac` (rather than spelling out work->actionCommands) makes MWCC
// keep dec, disp and AutoCommand in r31/r29/r28 exactly as the original does. The inner switch's cases are
// written 5,0,1,2,3 so the compiler lays the bodies out in the original's order.
s32 battleAcMain_TimingWide(BattleWork* work) {
	BattleActions* ac = &work->actionCommands;
	TimingWideDecWork* dec = (TimingWideDecWork*)ac->field_0x2BC;
	BattleACDispParams* disp = &ac->dispParams;
	s32 autoCommand = 0;
	s32 cond;

	if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
		autoCommand = 1;
	}

	switch (ac->mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = -300.f;
		disp->iconY = 0.0f;
		disp->counter = 0x14;
		dec->field_0x0C = 0;
		dec->field_0x08 = 0;
		switch (*(s32*)&ac->field_34[4]) {
		case 5:
			dec->field_0x00 = 0x100;
			dec->field_0x04 = 1;
			break;
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
		default:
			dec->field_0x00 = 0x40000;
			dec->field_0x04 = 0;
			break;
		}
		ac->outputParams[0] = 0;
		ac->outputParams[1] = 0;
		ac->outputParams[2] = 0;
		ac->mAcState = 0x63;
		ac->result = 1;
		break;
	case 0x64:
		if ((ac->field_0x4 & 1) &&
		    (ac->mAcUnit->mTokenFlags & 0x10)) {
			if ((s32)irand(100) >= 0) {
				ac->field_0x8 = 0;
				if (*(s32*)&ac->field_34[0x0C] > 0) {
					ac->field_0x8 = irand(*(s32*)&ac->field_34[0x0C]);
				}
				if (ac->field_0x8 <= 0) {
					ac->field_0x8 = *(s32*)&ac->field_34[0x08] + 2;
				}
			}
			else {
				s32 r = (s32)irand(*(s32*)&ac->field_34[0x10] -
				                   *(s32*)&ac->field_34[0x0C]);
				ac->field_0x8 = *(s32*)&ac->field_34[0x0C] + r;
			}
			ac->mAcState = 0x3EA;
			dec->field_0x0C = 2;
		}
		else {
			if (*(s32*)&ac->field_34[4] == 4) {
				ac->mAcState = 0x3E8;
			}
			else {
				ac->mAcState = 0x3E9;
			}
			dec->field_0x0C = 1;
		}
		break;
	case 0x3E8:
		if (BattlePadCheckTrigger(dec->field_0x00)) {
			ac->outputParams[1] = 1;
			ac->mAcState = 0x3E9;
		}
		break;
	case 0x3E9:
		dec->field_0x08++;
		if (dec->field_0x08 > *(s32*)&ac->field_34[0x08]) {
			ac->result = 0;
			ac->mAcState = 0x3EC;
			dec->field_0x0C = 4;
			if (*(s32*)&ac->field_34[0x00] & 8) {
				ac->mAcState = 0x3EB;
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
#ifdef MUST_MATCH
			// battle_pad.h declares these u32, but the original tests the result signed (cmpwi r3,0).
			if ((s32)BattlePadCheckUp(dec->field_0x00) != 0) {
#else
			if (BattlePadCheckUp(dec->field_0x00)) {
#endif
				cond = 1;
			}
			break;
		default:
#ifdef MUST_MATCH
			if ((s32)BattlePadCheckNow(dec->field_0x00) == 0) {
#else
			if (!BattlePadCheckNow(dec->field_0x00)) {
#endif
				cond = 1;
			}
			break;
		}
		if (dec->field_0x08 >= *(s32*)&ac->field_34[0x0C] &&
		    dec->field_0x08 < *(s32*)&ac->field_34[0x10]) {
			ac->outputParams[2] = 1;
		}
		else {
			ac->outputParams[2] = 0;
		}
		if (autoCommand) {
			if (ac->outputParams[2] != 0) {
				cond = 1;
			}
			else {
				cond = 0;
			}
		}
		if (cond != 0) {
			if (ac->outputParams[2] != 0) {
				ac->result = 2;
				ac->mResultCount++;
				ac->outputParams[0] = 1;
			}
			else {
				ac->result = 0;
				ac->outputParams[0] = -1;
			}
			ac->mAcState = 0x3EC;
			dec->field_0x0C = 4;
			if (*(s32*)&ac->field_34[0x00] & 8) {
				ac->mAcState = 0x3EB;
				dec->field_0x0C = 3;
			}
		}
		break;
	case 0x3EA:
		dec->field_0x08++;
		ac->field_0x8--;
		if (dec->field_0x08 > *(s32*)&ac->field_34[0x08]) {
			ac->result = 0;
			return 0;
		}
		if (ac->field_0x8 < 0) {
			if (dec->field_0x08 >= *(s32*)&ac->field_34[0x0C] &&
			    dec->field_0x08 < *(s32*)&ac->field_34[0x10]) {
				ac->result = 2;
				ac->mResultCount++;
				ac->outputParams[0] = 1;
			}
			else {
				ac->result = 0;
				ac->outputParams[0] = -1;
			}
			return 0;
		}
		break;
	case 0x3EC:
		dec->field_0x10 = 0x3C;
		ac->mAcState = 0x3ED;
		dec->field_0x0C = 5;
		// fallthrough
	case 0x3ED:
		dec->field_0x10--;
		if (dec->field_0x10 <= 0) {
			ac->mAcState = 0x3EE;
			dec->field_0x0C = 6;
		}
		break;
	case 0x3EE:
		ac->maincb = NULL;
		ac->dispcb = NULL;
		ac->resultcb = NULL;
		ac->deletecb = NULL;
		return 0;
	default:
		break;
	}

	return 1;
}
#pragma optimize_for_size reset

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

s32 battleAcResult_TimingWide(BattleWork* work) {
	return work->actionCommands.result;
}


// optimize_for_size: battleAcMain's prologue uses stmw/lmw.
#pragma optimize_for_size on

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
			disp->iconX = 30.f;
		}
		else {
			disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.f, 30.f);
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
				disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.f,
				                            -300.f);
			}
			else {
				disp->iconX = 30.f;
			}
		}
		else {
			disp->iconX = 30.f;
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

void battleAcDelete_TimingWide(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EC;
}

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
			v.x = -200.f + x;
			v.y = 50.f + y;
			iconDispGx(1.0f, &v, 0x10, down);
		}
		else {
			Vec v = vecs[1];
			v.x = -200.f + x;
			v.y = 50.f + y;
			iconDispGx(1.0f, &v, 0x10, up);
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
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, down);
			}
			else {
				Vec v = vecs[3];
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, up);
			}
		}
		else {
			if (dec->field_0x04 > 0) {
				Vec v = vecs[4];
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, up);
			}
			else {
				Vec v = vecs[5];
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, down);
			}
		}
		break;
	case 1:
		if (dec->field_0x04 != -1) {
			if (dec->field_0x08 >= *(s32*)&ac->field_34[0x0C] &&
			    dec->field_0x08 < *(s32*)&ac->field_34[0x10]) {
				if (dec->field_0x04 > 0) {
					Vec v = vecs[6];
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, up);
				}
				else {
					Vec v = vecs[7];
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, down);
				}
			}
			else {
				if (dec->field_0x04 > 0) {
					Vec v = vecs[8];
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, down);
				}
				else {
					Vec v = vecs[9];
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, up);
				}
			}
		}
		else {
			if (dec->field_0x08 >= *(s32*)&ac->field_34[0x0C]) {
				Vec v = vecs[10];
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, down);
			}
			else {
				Vec v = vecs[11];
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, up);
			}
		}
		break;
	default:
		break;
	}

	(*(s32*)&disp->field_0x0[0])++;
}
#pragma optimize_for_size reset

const Vec vec3_802fd630[12] = { 0 };
