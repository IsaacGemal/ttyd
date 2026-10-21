#include "countdown.h"

#include "mario/mariost.h"
#include "pmario_sound.h"
#include <string.h>
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;
// .sdata2 colours of this unit: white (0xFFFFFFFF) for the digits and the colon, and a yellowish 0xFFFFFF99 for the frame.
__declspec(section ".sdata2") u32 dat_80420498 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8042049c = 0xFFFFFF99;
#define COUNTDOWN_WHITE (*(GXColor*)&dat_80420498)
#define COUNTDOWN_YELLOW (*(GXColor*)&dat_8042049c)

//.sbss
// the 4 bytes after wp in the target's .sbss (gap_08_80418B2C_sbss)
#pragma force_active on
static u32 gap_after_wp;
#pragma force_active reset
static CountdownWork* wp;

void countDownInit(void) {
    wp = &gp->countdown;
    memset(&gp->countdown, 0, sizeof(gp->countdown));
}

void countDownSaveReStart(void) {
	CountdownWork* work = wp;
	OSTime ticksPerMs;
	OSTime ms;
	OSTime span;

	if (work->flags & 1) {
		ticksPerMs = OS_TIMER_CLOCK / 1000;
		ms = OSTicksToMilliseconds(work->elapsed);
		if ((u32)(work->limit - ms) <= (u32)work->restartTime) {
			span = (u32)(work->limit - work->restartTime);
			work->elapsed = ticksPerMs * span;
		}
		wp->lastTime = gp->renderTime;
		if (evtGetValue(NULL, GSW(0)) < 0x160) {
			wp->sfxId = psndSFXOn("SFX_STG2_BOMB_TIMER1");
		} else {
			wp->sfxId = psndSFXOn("SFX_STG7_ALART_WARNING1");
		}
	}
}

void countDownStart(s32 limit, s32 restartTime) {
	wp->limit = limit;
	wp->restartTime = restartTime;
	wp->lastTime = gp->renderTime;
	wp->elapsed = 0;
	wp->state = 0;
	wp->counter = 0;
	wp->dx = 0.0f;
	wp->dy = 0.0f;
	wp->x = 180.0f;
	wp->y = -180.0f;
	wp->flags = 0;
	wp->flags |= 1;
	if (evtGetValue(NULL, GSW(0)) < 0x160) {
		wp->sfxId = psndSFXOn("SFX_STG2_BOMB_TIMER1");
	} else {
		wp->sfxId = psndSFXOn("SFX_STG7_ALART_WARNING1");
	}
}

void countDownSetRestartTime(s32 time) {
    wp->restartTime = time;
}

void countDownMain(void) {
	if (wp->flags & 1) {
		switch (wp->state) {
		case 0:
			if (++wp->counter < 30) {
				wp->dx = intplGetValue(0xB, wp->counter, 30, 300.0f, 0.0f);
			} else {
				wp->counter = 0;
				wp->state++;
			}
			break;
		case 1:
			if (wp->flags & 2) {
				psndSFXOff(wp->sfxId);
				wp->sfxId = -1;
			}
			break;
		case 2:
			if (++wp->counter < 30) {
				wp->dx = intplGetValue(0xB, wp->counter, 30, 0.0f, 300.0f);
			} else {
				wp->counter = 0;
				wp->state++;
				wp->flags &= ~1;
				psndSFXOff(wp->sfxId);
			}
			break;
		}
		if (!(wp->flags & 8) && (u32)marioStGetSystemLevel() == 0) {
			wp->elapsed += (s32)((u32)gp->renderTime - (u32)wp->lastTime);
		}
		wp->lastTime = gp->renderTime;
		if (OSTicksToMilliseconds(wp->elapsed) >= wp->limit) {
			wp->flags |= 2;
		}
		dispEntry(CAMERA_2D, 1000.0f, 0, countDownDisp, NULL);
	}
}

void countDownDisp(CameraId camId, void* param) {
	GXColor colDigit;
	GXColor colColon;
	u16 flags;
	s32 sec;
	s32 ms;
	s32 msRaw;
	s32 i;
	s32 digits[3];
	Mtx mtx;
	CountdownWork* work = wp;
	Mtx scale;
	Mtx base;

	colColon = COUNTDOWN_WHITE;
	colDigit = COUNTDOWN_WHITE;
	flags = work->flags;
	msRaw = (s32)(work->limit - OSTicksToMilliseconds(work->elapsed));
	if (flags & 4) {
		return;
	}
	if (flags & 2) {
		msRaw = 0;
	}
	sec = msRaw / 1000;
	ms = msRaw % 1000;
	PSMTXTrans(base, work->x + work->dx, work->y + work->dy, 0.0f);
	PSMTXScale(scale, 2.5f, 1.0f, 1.0f);
	PSMTXConcat(base, scale, mtx);
	iconDispGxCol(mtx, 0x10, 0x1F6, COUNTDOWN_YELLOW);
	digits[0] = sec / 100;
	sec = sec % 100;
	digits[1] = sec / 10;
	digits[2] = sec % 10;
	for (i = 0; i < 3; i++) {
		PSMTXTrans(mtx, (f32)(-50 + i * 30), 8.0f, 0.0f);
		PSMTXConcat(base, mtx, mtx);
		iconDispGxCol(mtx, 0x10, (u16)(digits[i] + 0x1F7), colDigit);
	}
	PSMTXTrans(mtx, 28.0f, 10.0f, 0.0f);
	PSMTXConcat(base, mtx, mtx);
	iconDispGxCol(mtx, 0x10, 0x201, colColon);
	digits[0] = ms / 100;
	digits[1] = (ms % 100) / 10;
	for (i = 0; i < 2; i++) {
		PSMTXTrans(mtx, (f32)(40 + i * 15), 8.0f, 0.0f);
		PSMTXScale(scale, 0.5f, 0.5f, 0.5f);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(base, mtx, mtx);
		iconDispGxCol(mtx, 0x10, (u16)(digits[i] + 0x1F7), colDigit);
	}
}

s32 countDownGetStatus(void) {
    CountdownWork* work = wp;
    return !(work->flags & 1) ? -1 : (work->flags >> 1) & 1;
}

void countDownEnd(void) {
    if ((wp->flags & 1) != 0) {
        wp->state = 2;
    }
}

void countDownStop(void) {
    wp->flags |= 8;
    psndSFXOff(wp->sfxId);
}

void countDownFlagOn(u16 flag) {
    wp->flags |= flag;
}

void countDownFlagOff(u16 flag) {
    wp->flags &= ~flag;
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
