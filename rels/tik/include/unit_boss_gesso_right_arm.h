#pragma once

#include "evt/evt_cmd.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

EVT_DEF(unk_evt_tik_00029068);
EVT_DEF(gesso_arm_dead_event);
EVT_DEF(gesso_fall_check);
EVT_DEF(gesso_arm_damage_pose_event);

extern BattleUnitDefenseAttr R_defence_attr_turn;
extern BattleUnitDefense R_defence_turn;
extern BattleVulnerableStatus R_regist_turn;
extern BattleDispPose gesso_pose_table_turn[10];
