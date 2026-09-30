#pragma once

#include <dolphin/types.h>
#include <dolphin/os.h>
#include "driver/camdrv.h"

typedef struct CountdownWork {
	u16 flags; //0x0, 1 running, 2 time is up, 8 stopped
	u8 pad_2[0x8 - 0x2]; //0x2
	OSTime elapsed; //0x8, ticks counted so far
	OSTime lastTime; //0x10, gp->renderTime at the last update
	u32 limit; //0x18, milliseconds
	s32 restartTime; //0x1C, milliseconds
	f32 x; //0x20
	f32 y; //0x24
	f32 dx; //0x28
	f32 dy; //0x2C
	s32 state; //0x30
	s32 counter; //0x34
	s32 sfxId; //0x38
	u8 pad_3C[0x40 - 0x3C]; //0x3C
} CountdownWork;

void countDownInit(void);
void countDownSaveReStart(void);



void countDownMain(void);
void countDownDisp(CameraId camId, void* param);
s32 countDownGetStatus(void);
void countDownEnd(void);
void countDownSetRestartTime(s32 time);
void countDownFlagOn(u16 flag);
void countDownFlagOff(u16 flag);
void countDownStop(void);
void countDownStart(s32 type, s32 time);
