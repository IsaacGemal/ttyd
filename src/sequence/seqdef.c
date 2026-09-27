#include "driver/seqdrv.h"
#include "seq/seq_logo.h"
#include "seq/seq_game.h"
#include "seq/seq_e3.h"



SequenceCallback seq_data[8][3] = {
	{seq_logoInit, seq_logoMain, seq_logoExit},
	{NULL, NULL, NULL},
	{seq_gameInit, seq_gameMain, NULL},
	{NULL, NULL, NULL},
	{NULL, NULL, NULL},
	{NULL, NULL, NULL},
	{NULL, NULL, NULL},
	{seq_e3Init, seq_e3Main, seq_e3Exit},
};
