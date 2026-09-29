#include <dolphin/types.h>

extern u8 seq_battleExit[];
extern u8 seq_battleInit[];
extern u8 seq_battleMain[];
extern u8 seq_e3Exit[];
extern u8 seq_e3Init[];
extern u8 seq_e3Main[];
extern u8 seq_gameExit[];
extern u8 seq_gameInit[];
extern u8 seq_gameMain[];
extern u8 seq_gameOverExit[];
extern u8 seq_gameOverInit[];
extern u8 seq_gameOverMain[];
extern u8 seq_loadExit[];
extern u8 seq_loadInit[];
extern u8 seq_loadMain[];
extern u8 seq_logoExit[];
extern u8 seq_logoInit[];
extern u8 seq_logoMain[];
extern u8 seq_mapChangeExit[];
extern u8 seq_mapChangeInit[];
extern u8 seq_mapChangeMain[];
extern u8 seq_titleExit[];
extern u8 seq_titleInit[];
extern u8 seq_titleMain[];

u32 seq_data_80302E40[24] = {
    (u32)(void*)seq_logoInit, (u32)(void*)seq_logoMain, (u32)(void*)seq_logoExit, (u32)(void*)seq_titleInit, (u32)(void*)seq_titleMain, (u32)(void*)seq_titleExit, (u32)(void*)seq_gameInit, (u32)(void*)seq_gameMain,
    (u32)(void*)seq_gameExit, (u32)(void*)seq_mapChangeInit, (u32)(void*)seq_mapChangeMain, (u32)(void*)seq_mapChangeExit, (u32)(void*)seq_battleInit, (u32)(void*)seq_battleMain, (u32)(void*)seq_battleExit, (u32)(void*)seq_gameOverInit,
    (u32)(void*)seq_gameOverMain, (u32)(void*)seq_gameOverExit, (u32)(void*)seq_loadInit, (u32)(void*)seq_loadMain, (u32)(void*)seq_loadExit, (u32)(void*)seq_e3Init, (u32)(void*)seq_e3Main, (u32)(void*)seq_e3Exit
};

