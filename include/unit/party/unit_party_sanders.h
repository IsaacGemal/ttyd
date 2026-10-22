#pragma once

#include <dolphin/types.h>
#include "manager/evtmgr.h"
#include "battle/battle_database_common.h"

s32 _judge_on_stage(EventEntry* event, BOOL isFirstCall);
s32 _shot_move(EventEntry* event, BOOL isFirstCall);
s32 _make_counterset_weapon(EventEntry* event, BOOL isFirstCall);
s32 _get_bomb_hit_position(EventEntry* event, BOOL isFirstCall);
s32 _sanders_make_extra_work_area(EventEntry* event, BOOL isFirstCall);
extern BattleWeapon partyWeapon_SandersCounterSet;
extern BattleWeapon partyWeapon_SandersNormalAttack;
extern BattleWeapon partyWeapon_SandersSuperBombAttack;
extern BattleWeapon partyWeapon_SandersTimeBombSet;
