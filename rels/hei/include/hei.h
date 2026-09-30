#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

EVT_DEF(hei_00_init_evt);
EVT_DEF(hei_01_init_evt);
EVT_DEF(hei_02_init_evt);
EVT_DEF(hei_03_init_evt);
EVT_DEF(hei_04_init_evt);
EVT_DEF(hei_05_init_evt);
EVT_DEF(hei_06_init_evt);
EVT_DEF(hei_07_init_evt);
EVT_DEF(hei_08_init_evt);
EVT_DEF(hei_09_init_evt);
EVT_DEF(hei_10_init_evt);
EVT_DEF(hei_11_init_evt);
EVT_DEF(hei_12_init_evt);
EVT_DEF(hei_13_init_evt);
EVT_DEF(hei_bt00_init_evt);

extern BattleSetupData btlsetup_hei_tbl[];
extern u32 btl_hei_setup_no_tbl[];

