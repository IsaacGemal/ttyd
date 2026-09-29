#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

void _prolog(void);
void _epilog(void);
void _unresolved(void);

EVT_DEF(moo_00_init_evt);
EVT_DEF(moo_01_init_evt);
EVT_DEF(moo_02_init_evt);
EVT_DEF(moo_05_init_evt);
EVT_DEF(moo_06_init_evt);
EVT_DEF(moo_07_init_evt);
extern u32 btl_moo_setup_no_tbl[];
extern BattleSetupData btlsetup_moo_tbl[17];
