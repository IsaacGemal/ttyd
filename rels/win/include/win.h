#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];
extern BattleSetupData btlsetup_win_tbl[9];
extern u32 btl_win_setup_no_tbl[];

void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(win_01_init_evt);
EVT_DEF(win_02_init_evt);
EVT_DEF(win_05_init_evt);
EVT_DEF(evt_win_door_open_pure);
EVT_DEF(evt_win_door_open);
EVT_DEF(evt_win_door_close_pure);
EVT_DEF(evt_win_door_close);
EVT_DEF(_tree_anime);
EVT_DEF(win_tree_entry);
EVT_DEF(win_puni_walk_se);
