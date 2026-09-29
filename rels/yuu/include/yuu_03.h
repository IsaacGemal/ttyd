#pragma once

#include "evt/evt_cmd.h"

void yuuminigame4_gamerule(void);  // TODO: real signature
// yuuminigame_disp_4slit: this unit has its own copy; a header already declares it.
// yuuminigame_main_4slit: this unit has its own copy; a header already declares it.
// yuuminigame_init_4slit: this unit has its own copy; a header already declares it.
USER_FUNC(evt_ryushi);
USER_FUNC(evt_ryushi_clr);
void evt_ryushi_init(void);  // TODO: real signature
void ryushi_disp(void);  // TODO: real signature
USER_FUNC(yuuminigame4_opening_chk);
USER_FUNC(yuuminigame4_opening_on_off);
USER_FUNC(yuunpc_go);
USER_FUNC(re_init_yuunpc);
void evt_slit_cancel(void);  // TODO: real signature
USER_FUNC(yuuminigame4_set_phase);
void unk_yuu_0000341c(void);  // TODO: real signature
USER_FUNC(unk_yuu_000034bc);
void unk_yuu_0000365c(void);  // TODO: real signature
USER_FUNC(unk_yuu_0000371c);
EVT_DEF(unk_evt_yuu_00015fe0);
EVT_DEF(ryushi_event);
EVT_DEF(yuu_03_kaijo_init);
EVT_DEF(yuu_03_kaijo_init2);
EVT_DEF(minigame04_main);
EVT_DEF(minigame04_start_cam);
EVT_DEF(minigame04_start_cam_cancel);
EVT_DEF(minigame04_start);
EVT_DEF(minigame04_end);
EVT_DEF(yuu_03_init_evt);
