#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(muj_mario_pos_osreport_func);
USER_FUNC(muj_npc_buruburu);
USER_FUNC(muj_npc_fuwafuwa);
USER_FUNC(muj_npc_guruguru);
USER_FUNC(muj_ship_puka_func);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(muj_ship_puka);
EVT_DEF(korutesu_apper);
EVT_DEF(korutesu_vanish);
EVT_DEF(muj_mario_pos_osreport);
// muj_03.c and muj_11.c have no header of their own; their init events are only
// referenced from muj.c's _prolog.
EVT_DEF(muj_03_init_evt);
EVT_DEF(muj_11_init_evt);
// Defined in data/battle/battle_database_muj.c, referenced from muj.c's _prolog.
extern BattleSetupData btlsetup_muj_tbl[16];
extern u32 btl_muj_setup_no_tbl[32];
