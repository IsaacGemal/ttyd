#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

EVT_DEF(tik_00_init_evt);
EVT_DEF(tik_01_init_evt);
EVT_DEF(tik_02_init_evt);
EVT_DEF(tik_03_init_evt);
EVT_DEF(tik_04_init_evt);
EVT_DEF(tik_05_init_evt);
EVT_DEF(tik_06_init_evt);
EVT_DEF(tik_07_init_evt);
EVT_DEF(tik_08_init_evt);
EVT_DEF(tik_09_init_evt);
EVT_DEF(tik_10_init_evt);
EVT_DEF(tik_11_init_evt);
EVT_DEF(tik_12_init_evt);
EVT_DEF(tik_13_init_evt);
EVT_DEF(tik_14_init_evt);
EVT_DEF(tik_15_init_evt);
EVT_DEF(tik_16_init_evt);
EVT_DEF(tik_17_init_evt);
EVT_DEF(tik_18_init_evt);
EVT_DEF(tik_19_init_evt);
EVT_DEF(tik_20_init_evt);
EVT_DEF(tik_21_init_evt);
extern BattleSetupData btlsetup_tik_tbl[30];
extern u32 btl_tik_setup_no_tbl[];

USER_FUNC(evt_tik_villager_urouro_check_func);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(tik_villager_urouro);
