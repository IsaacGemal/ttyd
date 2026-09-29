#pragma once

#include "evt/evt_cmd.h"

void fire_bros_get_next_target(s32* outUnit, s32* outPart);
void fire_bros_get_target(s32* outUnit, s32* outPart);
USER_FUNC(fire_bros_nagemakuri_set_target);
EVT_DEF(fire_move_event);
EVT_DEF(nagemakuri_judge_last_attack_event);
