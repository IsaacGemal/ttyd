#pragma once

#include "evt/evt_cmd.h"

void yuuminigame3_gamerule(void);  // TODO: real signature
USER_FUNC(yuu_roll_reset);
// yuuminigame_disp_3roll: this unit has its own copy; a header already declares it.
// yuuminigame_main_3roll: this unit has its own copy; a header already declares it.
// yuuminigame_init_3roll: this unit has its own copy; a header already declares it.
USER_FUNC(yuuminigame3_opening_chk);
USER_FUNC(yuuminigame3_opening_on_off);
USER_FUNC(yuuminigame3_roll_fix_off);
USER_FUNC(yuuminigame3_roll_fix_on);
USER_FUNC(yuuminigame3_set_phase);
USER_FUNC(unk_yuu_00000f1c);
void unk_yuu_00001054(void);  // TODO: real signature
void unk_yuu_00001160(void);  // TODO: real signature
void unk_yuu_000011e0(void);  // TODO: real signature
void fn_31_112C(void);  // TODO: real signature
USER_FUNC(unk_yuu_00001210);
EVT_DEF(unk_evt_yuu_000152bc);
EVT_DEF(yuu_02_jumpret);
EVT_DEF(yuu_02_kaijo_init);
EVT_DEF(yuu_02_kaijo_init2);
EVT_DEF(minigame03_main);
EVT_DEF(minigame03_start_cam);
EVT_DEF(minigame03_start_cam_cancel);
EVT_DEF(minigame03_start);
EVT_DEF(minigame03_end);
EVT_DEF(s_en1_turn);
EVT_DEF(yuu_02_init_evt);
