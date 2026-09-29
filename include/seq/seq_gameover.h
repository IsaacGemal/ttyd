#pragma once

#include "driver/seqdrv.h"

void seq_gameOverInit(SequenceWork* work);
void seq_gameOverMain(SequenceWork* work);
void seq_gameOverExit(SequenceWork* work);
extern u8 evt_gameover[];
