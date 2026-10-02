#pragma once

#include "driver/seqdrv.h"

//for seqdrv
void seq_mapChangeInit(SequenceWork* work);
void seq_mapChangeMain(SequenceWork* work);
void seq_mapChangeExit(SequenceWork* work);
void _relUnLoad(void);
void _unload(const char* arg0, const char* arg1, const char* arg2);
void _load(const char* arg0, const char* arg1, const char* arg2);
