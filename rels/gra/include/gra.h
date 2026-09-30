#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

void _unresolved(void);
void _epilog(void);
void _prolog(void);
USER_FUNC(evt_gra_vivian_pose_set);

EVT_DEF(gra_00_init_evt);
EVT_DEF(gra_01_init_evt);
EVT_DEF(gra_02_init_evt);
EVT_DEF(gra_03_init_evt);
EVT_DEF(gra_04_init_evt);
EVT_DEF(gra_05_init_evt);
EVT_DEF(gra_06_init_evt);

// Linker-generated constructor/destructor lists for this REL.
typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

// Battle setup tables for this area, defined by the generated
// data/battle/battle_database_gra.c.
extern BattleSetupData btlsetup_gra_tbl[14];
extern u8 btl_gra_setup_no_tbl[];
