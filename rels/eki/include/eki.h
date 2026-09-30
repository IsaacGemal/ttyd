#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

extern s32 eki_00_init_evt[];
extern s32 eki_01_init_evt[];
extern s32 eki_02_init_evt[];
extern s32 eki_03_init_evt[];
extern s32 eki_04_init_evt[];
extern s32 eki_05_init_evt[];
extern s32 eki_06_init_evt[];

extern BattleSetupData btlsetup_eki_tbl[20];
extern u32 btl_eki_setup_no_tbl[];

typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

