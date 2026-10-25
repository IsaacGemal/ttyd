#pragma once

#include "driver/seqdrv.h"
#include "manager/filemgr.h"
#include <dolphin/os.h>

typedef struct LogoWork {
	BOOL progressive; //0x0
	s32 progState; //0x4, progDisp's step: 0 start, 1 ask, 2 confirm message, 3 apply, 4 wait
	s32 toggle; //0x8, 0 = yes, 1 = no
	FileEntry* texture; //0xC
	s32 logoIdx; //0x10, logo texture logoDisp draws (tpl image 0, 1 or 2); seq_logoMain steps it at each fade-in
	u8 pad_0x14[0x18 - 0x14]; //0x14
	OSTime lastInputTime; //0x18, progDisp: OSGetTime() at the last pad input; 10 s without one answers the prompt
	BOOL skipStory; //0x20
	u8 pad_0x24[0x28 - 0x24]; //0x24
} LogoWork;

void seq_logoInit(SequenceWork* work);
void seq_logoMain(SequenceWork* work);
void seq_logoExit(SequenceWork* work);
