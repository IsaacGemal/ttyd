#pragma once

#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

USER_FUNC(evt_nokonoko_urouro_check_func);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(nokonoko_urouro_regl);

extern BattleSetupData btlsetup_nok_tbl[2];
extern char* btl_nok_setup_no_tbl[4];