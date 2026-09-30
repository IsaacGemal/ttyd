#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

void _unresolved(void);
void _epilog(void);
void _prolog(void);

EVT_DEF(pik_00_init_evt);
EVT_DEF(pik_01_init_evt);
EVT_DEF(pik_02_init_evt);
EVT_DEF(pik_03_init_evt);
EVT_DEF(pik_04_init_evt);

extern BattleSetupData btlsetup_pik_tbl[11];
extern u32 btl_pik_setup_no_tbl[22];
