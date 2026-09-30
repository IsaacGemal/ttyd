#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

EVT_DEF(gon_00_init_evt);
EVT_DEF(gon_01_init_evt);
EVT_DEF(gon_02_init_evt);
EVT_DEF(gon_03_init_evt);
EVT_DEF(gon_04_init_evt);
EVT_DEF(gon_05_init_evt);
EVT_DEF(gon_06_init_evt);
EVT_DEF(gon_07_init_evt);
EVT_DEF(gon_08_init_evt);
EVT_DEF(gon_09_init_evt);
EVT_DEF(gon_10_init_evt);
EVT_DEF(gon_11_init_evt);
EVT_DEF(gon_12_init_evt);
EVT_DEF(gon_13_init_evt);
EVT_DEF(gon_bt00_init_evt);
extern BattleSetupData btlsetup_gon_tbl[];
extern u32 btl_gon_setup_no_tbl[];

void _prolog(void);
void _epilog(void);
void _unresolved(void);
