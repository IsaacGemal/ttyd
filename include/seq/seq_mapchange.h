#pragma once

#include "driver/seqdrv.h"

//for seqdrv
void seq_mapChangeInit(SequenceWork* work);
void seq_mapChangeMain(SequenceWork* work);
void seq_mapChangeExit(SequenceWork* work);
void _relUnLoad(void);
void _unload(const char* arg0, const char* arg1, const char* arg2);
void _load(const char* arg0, const char* arg1, const char* arg2);

// .rodata/.sdata2 string objects this unit defines and refers to by name
extern const char str_tou_03[];
extern const char str_relPathFormat[];
extern const char str_tou[];
extern const char str_tou2[];
extern const char str_tst[];
extern const char str_jon[];

// Two-pointer view of table_802C1028; the original copies both words in one 64-bit move.
#ifdef MUST_MATCH
typedef struct { const char* a; const char* b; } MapLangPair;
#endif
