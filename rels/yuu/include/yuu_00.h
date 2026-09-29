#pragma once

#include "evt/evt_cmd.h"

void yuuminigame1_gamerule(void);  // TODO: real signature
// yuuminigame_disp_1hikoki: this unit has its own copy; a header already declares it.
// yuuminigame_main_1hikoki: this unit has its own copy; a header already declares it.
// yuuminigame_init_1hikoki: this unit has its own copy; a header already declares it.
USER_FUNC(yuuminigame1_opening_chk);
USER_FUNC(yuuminigame1_opening_on_off);
USER_FUNC(yuuminigame1_set_phase);
USER_FUNC(evt_change_plane);
USER_FUNC(unk_yuu_00004250);
EVT_DEF(yuu_00_kaijo_init);
EVT_DEF(yuu_00_kaijo_init2);
EVT_DEF(light_sub_event);
EVT_DEF(minigame01_main);
EVT_DEF(minigame01_start_cam);
EVT_DEF(minigame01_start_cam_cancel);
EVT_DEF(minigame01_start);
EVT_DEF(minigame01_end);
EVT_DEF(yuu_00_init_evt);
