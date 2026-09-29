#include "driver/seqdrv.h"
#include "seq/seq_battle.h"
#include "seq/seq_e3.h"
#include "seq/seq_game.h"
#include "seq/seq_gameover.h"
#include "seq/seq_logo.h"
#include "seq/seq_mapchange.h"
#include "seq/seq_title.h"
#include "sequence/seq_load.h"

// Init, Main and Exit of each sequence, in the order seqdrv.c numbers them.
SequenceCallback seq_data_80302E40[8][3] = {
    { seq_logoInit, seq_logoMain, seq_logoExit },
    { seq_titleInit, seq_titleMain, seq_titleExit },
    { seq_gameInit, seq_gameMain, seq_gameExit },
    { seq_mapChangeInit, seq_mapChangeMain, seq_mapChangeExit },
    { seq_battleInit, seq_battleMain, seq_battleExit },
    { seq_gameOverInit, seq_gameOverMain, seq_gameOverExit },
    { seq_loadInit, seq_loadMain, seq_loadExit },
    { seq_e3Init, seq_e3Main, seq_e3Exit },
};
