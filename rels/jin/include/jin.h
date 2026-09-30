#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

USER_FUNC(evt_jin_vivian_pose_set);
void _prolog(void);
void _epilog(void);
void _unresolved(void);

extern BattleSetupData btlsetup_jin_tbl[];
extern u32 btl_jin_setup_no_tbl[];

EVT_DEF(jin_00_init_evt);
EVT_DEF(jin_01_init_evt);
EVT_DEF(jin_02_init_evt);
EVT_DEF(jin_03_init_evt);
EVT_DEF(jin_04_init_evt);
EVT_DEF(jin_05_init_evt);
EVT_DEF(jin_06_init_evt);
EVT_DEF(jin_07_init_evt);
EVT_DEF(jin_08_init_evt);
EVT_DEF(jin_09_init_evt);
EVT_DEF(jin_10_init_evt);
EVT_DEF(jin_11_init_evt);
