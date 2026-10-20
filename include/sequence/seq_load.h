#pragma once

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>
s32 seqLoadPrepCardRecheck(EventEntry* event, BOOL isFirstCall);
#include "driver/seqdrv.h"
void seq_loadInit(SequenceWork* work);
void seq_loadMain(SequenceWork* work);
void seq_loadExit(SequenceWork* work);
void loadMain(SequenceWork* work);
void loadDraw(void);
void continueGame(void);
