#pragma once

#include "evt/evt_cmd.h"

void ext_yuunpc_minigame4_main(void);  // TODO: real signature
void ext_yuunpc_minigame3_main(void);  // TODO: real signature
void ext_yuunpc_minigame2_main(void);  // TODO: real signature
void ext_yuunpc_minigame1_main(void);  // TODO: real signature
void ext_yuunpc_dispent(void);  // TODO: real signature
void yuunpc_disp(void);  // TODO: real signature
void yuunpc_pose_minigame4_main(void);  // TODO: real signature
void ext_yuunpc_minigame4_init(void);  // TODO: real signature
void ext_yuunpc_minigame4_init_sub(void);  // TODO: real signature
void yuunpc_juni(void);  // TODO: real signature
void yuunpc_wp_flag_check(void);  // TODO: real signature
void yuunpc_wp_flag_off(void);  // TODO: real signature
void yuunpc_wp_flag_on(void);  // TODO: real signature
void errper_set_rtn(void);  // TODO: real signature
void goalincheck(void);  // TODO: real signature
void fallcheck(void);  // TODO: real signature
void hitcheck(void);  // TODO: real signature
void yuunpc_pose_minigame3_main(void);  // TODO: real signature
void ext_yuunpc_minigame3_init(void);  // TODO: real signature
void yuunpc_pose_minigame2_main(void);  // TODO: real signature
void ext_yuunpc_minigame2_init(void);  // TODO: real signature
void yuunpc_pose_minigame1_main(void);  // TODO: real signature
void ext_yuunpc_minigame1_init(void);  // TODO: real signature
void yuunpc_pose_sub(void);  // TODO: real signature
void yuunpc_set_jump_pose(void);  // TODO: real signature
void yuunpc_pose_change2(void);  // TODO: real signature
void yuunpc_pose_change(void);  // TODO: real signature
void yuunpc_pose_check(void);  // TODO: real signature
void yuunpc_pose_init(void);  // TODO: real signature
void yuunpc_maphit_init(void);  // TODO: real signature
extern u32 chusan_act_tbl[8];
EVT_DEF(evt_yuunpc_minigame1);
EVT_DEF(evt_yuunpc_minigame2);
EVT_DEF(evt_yuunpc_minigame3);
EVT_DEF(evt_yuunpc_minigame4);
