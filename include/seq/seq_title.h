#pragma once

#include "driver/seqdrv.h"
#include "driver/camdrv.h"
#include "manager/filemgr.h"
#include <dolphin/types.h>

typedef struct TitleWork {
	FileEntry* texture; //0x0
	u8 pad_0x4[0x8 - 0x4]; //0x4
	s32 step; //0x8
	f32 tex5PosX; //0xC
	f32 tex5PosY; //0x10
	f32 tex6PosX; //0x14
	f32 tex6PosY; //0x18
	f32 fade; //0x1C
	s32 phaseFrame; //0x20
	s32 waitTimer; //0x24
	s32 idleTimer; //0x28
	s32 pulseCounter; //0x2C
	s32 debug; //0x30
} TitleWork;

const char* DbgBtlSel_GetMsgDataPtr(void);
s32 getDebugMode(void);
void L_titleInit(SequenceWork* work);
void titleMain(SequenceWork* work);
void disp1(CameraId cameraId, void* param);
void disp2(CameraId cameraId, void* param);
void seq_titleInit(SequenceWork* work);
void seq_titleMain(SequenceWork* work);
void seq_titleExit(SequenceWork* work);
s32 N_getDebugMode(void);
