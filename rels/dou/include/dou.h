#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

extern BattleSetupData btlsetup_dou_tbl[28];
extern u32 btl_dou_setup_no_tbl[56];

EVT_DEF(dou_01_init_evt);
EVT_DEF(dou_02_init_evt);
EVT_DEF(dou_03_init_evt);
EVT_DEF(dou_06_init_evt);
EVT_DEF(dou_08_init_evt);
EVT_DEF(dou_09_init_evt);
EVT_DEF(dou_12_init_evt);
EVT_DEF(dou_13_init_evt);

USER_FUNC(dou_mario_pos_osreport_func);
USER_FUNC(evt_dou_normal_puka);
EVT_DEF(dou_normal_puka);
EVT_DEF(dou_puka_event_sub);
EVT_DEF(dou_puka_event);
EVT_DEF(dou_mario_pos_osreport);
