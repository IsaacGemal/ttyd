#pragma once

#include "evt/evt_cmd.h"

void yuuminigame2_gamerule(void);  // TODO: real signature
// yuuminigame_disp_2fune: this unit has its own copy; a header already declares it.
// yuuminigame_main_2fune: this unit has its own copy; a header already declares it.
// yuuminigame_init_2fune: this unit has its own copy; a header already declares it.
USER_FUNC(yuuminigame2_opening_chk);
USER_FUNC(yuuminigame2_opening_on_off);
USER_FUNC(yuuminigame2_get_name);
USER_FUNC(yuuminigame2_set_phase);
USER_FUNC(unk_yuu_00005268);
EVT_DEF(yuu_01_kaijo_init);
EVT_DEF(yuu_01_kaijo_init2);
EVT_DEF(minigame02_main);
EVT_DEF(minigame02_start_cam);
EVT_DEF(minigame02_start_cam_cancel);
EVT_DEF(minigame02_start);
EVT_DEF(minigame02_end);
EVT_DEF(yuu_01_init_evt);
