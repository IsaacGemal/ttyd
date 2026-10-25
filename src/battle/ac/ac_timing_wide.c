#include "battle/ac/ac_timing_wide.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>

// Action-command work area for TimingWide, at _battleWorkPointer + 0x1F4C. No header declares it.
typedef struct TimingWideDecWork {
	s32 buttonMask;   // 0x00, BattlePad mask of the button (0x100, 0x40000 or 0x80000)
	s8 pressMode;     // 0x04, 1 checks the trigger, -1 checks the release (CheckUp), 0 checks the button is not held
	u8 pad_0x05[3];   // 0x05
	s32 frame;        // 0x08, timing counter, compared with acParams[1] (limit) and acParams[2..3] (ok window)
	s32 phase;        // 0x0C, display sub-state (0..6)
	s32 endTimer;     // 0x10, end timer
} TimingWideDecWork;

// sdata2 constants.

static void actionCommandDisp(f32 x, f32 y);

// Holding the work-area pointer in `ac` (rather than spelling out work->actionCommands) makes MWCC
// keep dec, disp and AutoCommand in r31/r29/r28 exactly as the original does. The inner switch's cases are
// written 5,0,1,2,3 so the compiler lays the bodies out in the original's order.
s32 battleAcMain_TimingWide(BattleWork* work) {
	BattleActions* ac = &work->actionCommands;
	TimingWideDecWork* dec = (TimingWideDecWork*)ac->acWork;
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
		dec->phase = 0;
		dec->frame = 0;
		switch (ac->acParams[0]) {
		case 5:
			dec->buttonMask = 0x100;
			dec->pressMode = 1;
			break;
		case 0:
			dec->buttonMask = 0x40000;
			dec->pressMode = 0;
			break;
		case 1:
			dec->buttonMask = 0x80000;
			dec->pressMode = 0;
			break;
		case 2:
			dec->buttonMask = 0x40000;
			dec->pressMode = -1;
			break;
		case 3:
			dec->buttonMask = 0x80000;
			dec->pressMode = -1;
			break;
		default:
			dec->buttonMask = 0x40000;
			dec->pressMode = 0;
			break;
		}
		ac->outputParams[0] = 0;
		ac->outputParams[1] = 0;
		ac->outputParams[2] = 0;
		ac->mAcState = 0x63;
		ac->result = 1;
		break;
	case 0x64:
		if ((ac->autoFlags & 1) &&
		    (ac->mAcUnit->mTokenFlags & 0x10)) {
			if (irand(100) >= 0) {
				ac->autoTimer = 0;
				if (ac->acParams[2] > 0) {
					ac->autoTimer = irand(ac->acParams[2]);
				}
				if (ac->autoTimer <= 0) {
					ac->autoTimer = ac->acParams[1] + 2;
				}
			}
			else {
				s32 r = irand(ac->acParams[3] -
				                   ac->acParams[2]);
				ac->autoTimer = ac->acParams[2] + r;
			}
			ac->mAcState = 0x3EA;
			dec->phase = 2;
		}
		else {
			if (ac->acParams[0] == 4) {
				ac->mAcState = 0x3E8;
			}
			else {
				ac->mAcState = 0x3E9;
			}
			dec->phase = 1;
		}
		break;
	case 0x3E8:
		if (BattlePadCheckTrigger(dec->buttonMask)) {
			ac->outputParams[1] = 1;
			ac->mAcState = 0x3E9;
		}
		break;
	case 0x3E9:
		dec->frame++;
		if (dec->frame > ac->acParams[1]) {
			ac->result = 0;
			ac->mAcState = 0x3EC;
			dec->phase = 4;
			if (ac->acFlag & 8) {
				ac->mAcState = 0x3EB;
				dec->phase = 3;
			}
			break;
		}
		cond = 0;
		switch (dec->pressMode) {
		case 1:
			if (BattlePadCheckTrigger(dec->buttonMask)) {
				cond = 1;
			}
			break;
		case -1:
			if (BattlePadCheckUp(dec->buttonMask)) {
				cond = 1;
			}
			break;
		default:
			if (!BattlePadCheckNow(dec->buttonMask)) {
				cond = 1;
			}
			break;
		}
		if (dec->frame >= ac->acParams[2] &&
		    dec->frame < ac->acParams[3]) {
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
			dec->phase = 4;
			if (ac->acFlag & 8) {
				ac->mAcState = 0x3EB;
				dec->phase = 3;
			}
		}
		break;
	case 0x3EA:
		dec->frame++;
		ac->autoTimer--;
		if (dec->frame > ac->acParams[1]) {
			ac->result = 0;
			return 0;
		}
		if (ac->autoTimer < 0) {
			if (dec->frame >= ac->acParams[2] &&
			    dec->frame < ac->acParams[3]) {
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
		dec->endTimer = 0x3C;
		ac->mAcState = 0x3ED;
		dec->phase = 5;
		// fallthrough
	case 0x3ED:
		dec->endTimer--;
		if (dec->endTimer <= 0) {
			ac->mAcState = 0x3EE;
			dec->phase = 6;
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

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

s32 battleAcResult_TimingWide(BattleWork* work) {
	return work->actionCommands.result;
}

void battleAcDisp_TimingWide(CameraId camId, void* param) {
	BattleWork* work = param;
	BattleActions* ac = &work->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	TimingWideDecWork* dec = (TimingWideDecWork*)&ac->acWork[0];
	s32 flag;

	switch (dec->phase) {
	case 0:
	case 1:
	case 2:
		if (ac->acFlag & 1) {
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
		if ((ac->acFlag & 2) &&
		    (ac->result & 2)) {
			flag = 0;
		}
		if ((ac->acFlag & 4) &&
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
	BattleWork* work = _battleWorkPointer;
	BattleActions* ac = &work->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	TimingWideDecWork* dec;
	s32 up;
	s32 down;

	camGetPtr(CAMERA_2D);
	dec = (TimingWideDecWork*)&ac->acWork[0];

	switch (dec->buttonMask) {
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

	switch (dec->phase) {
	case 0:
		if (dec->pressMode > 0) {
			Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
			v.x = -200.f + x;
			v.y = 50.f + y;
			iconDispGx(1.0f, &v, 0x10, down);
		}
		else {
			Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
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
		if (!(ac->acFlag & 0x10)) {
			if (dec->pressMode > 0) {
				Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, down);
			}
			else {
				Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, up);
			}
		}
		else {
			if (dec->pressMode > 0) {
				Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, up);
			}
			else {
				Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, down);
			}
		}
		break;
	case 1:
		if (dec->pressMode != -1) {
			if (dec->frame >= ac->acParams[2] &&
			    dec->frame < ac->acParams[3]) {
				if (dec->pressMode > 0) {
					Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, up);
				}
				else {
					Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, down);
				}
			}
			else {
				if (dec->pressMode > 0) {
					Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, down);
				}
				else {
					Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
					v.x = -200.f + x;
					v.y = 50.f + y;
					iconDispGx(1.0f, &v, 0x10, up);
				}
			}
		}
		else {
			if (dec->frame >= ac->acParams[2]) {
				Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, down);
			}
			else {
				Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
				v.x = -200.f + x;
				v.y = 50.f + y;
				iconDispGx(1.0f, &v, 0x10, up);
			}
		}
		break;
	default:
		break;
	}

	disp->dispFrame++;
}
