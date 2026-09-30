#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

USER_FUNC(mobj_set_ry);
USER_FUNC(mobj_color_off);
USER_FUNC(mobj_set_color);
EVT_DEF(box_syutugen);
EVT_DEF(move_dai);
EVT_DEF(hikari_evt);
EVT_DEF(las_00_init_evt);
EVT_DEF(las_01_init_evt);
EVT_DEF(las_02_init_evt);
EVT_DEF(las_03_init_evt);
EVT_DEF(las_04_init_evt);
EVT_DEF(las_05_init_evt);
EVT_DEF(las_06_init_evt);
EVT_DEF(las_07_init_evt);
EVT_DEF(las_08_init_evt);
EVT_DEF(las_09_init_evt);
EVT_DEF(las_10_init_evt);
EVT_DEF(las_11_init_evt);
EVT_DEF(las_12_init_evt);
EVT_DEF(las_13_init_evt);
EVT_DEF(las_14_init_evt);
EVT_DEF(las_15_init_evt);
EVT_DEF(las_16_init_evt);
EVT_DEF(las_17_init_evt);
EVT_DEF(las_18_init_evt);
EVT_DEF(las_19_init_evt);
EVT_DEF(las_20_init_evt);
EVT_DEF(las_21_init_evt);
EVT_DEF(las_22_init_evt);
EVT_DEF(las_23_init_evt);
EVT_DEF(las_24_init_evt);
EVT_DEF(las_25_init_evt);
EVT_DEF(las_26_init_evt);
EVT_DEF(las_27_init_evt);
EVT_DEF(las_28_init_evt);
EVT_DEF(las_29_init_evt);
EVT_DEF(las_30_init_evt);
extern BattleSetupData btlsetup_las_tbl[61];
extern u32 btl_las_setup_no_tbl[];
