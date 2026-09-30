#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

EVT_DEF(aji_00_init_evt);
EVT_DEF(aji_01_init_evt);
EVT_DEF(aji_02_init_evt);
EVT_DEF(aji_03_init_evt);
EVT_DEF(aji_04_init_evt);
EVT_DEF(aji_05_init_evt);
EVT_DEF(aji_06_init_evt);
EVT_DEF(aji_07_init_evt);
EVT_DEF(aji_08_init_evt);
EVT_DEF(aji_09_init_evt);
EVT_DEF(aji_10_init_evt);
EVT_DEF(aji_11_init_evt);
EVT_DEF(aji_12_init_evt);
EVT_DEF(aji_13_init_evt);
EVT_DEF(aji_14_init_evt);
EVT_DEF(aji_15_init_evt);
EVT_DEF(aji_16_init_evt);
EVT_DEF(aji_17_init_evt);
EVT_DEF(aji_18_init_evt);
EVT_DEF(aji_19_init_evt);

extern BattleSetupData btlsetup_aji_tbl[24];
extern u32 btl_aji_setup_no_tbl[48];

void _unresolved(void);
void _epilog(void);
void _prolog(void);
