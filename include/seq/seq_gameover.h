#pragma once

#include "driver/seqdrv.h"
#include "evt/evt_cmd.h"

void seq_gameOverInit(SequenceWork* work);
void seq_gameOverMain(SequenceWork* work);
void seq_gameOverExit(SequenceWork* work);
EVT_DEF(evt_gameover);
