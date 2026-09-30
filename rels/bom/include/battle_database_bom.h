#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

USER_FUNC(_snow_control);
EVT_DEF(bgset_bom_snow_event);
extern BattleSetupData btlsetup_bom_tbl[8];
extern u32 btl_bom_setup_no_tbl[];
