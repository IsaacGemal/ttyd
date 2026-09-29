#pragma once

#include "evt/evt_cmd.h"

void chorogun_escape_all(void);  // TODO: real signature
USER_FUNC(chorogun_escape);
USER_FUNC(chorogun_after_attack);
USER_FUNC(chorogun_counterd);
USER_FUNC(chorogun_bound_target);
USER_FUNC(chorogun_dive_target);
USER_FUNC(chorogun_approach_target);
void chorogun_appear(void);  // TODO: real signature
USER_FUNC(chorogun_set_target_id);
USER_FUNC(chorogun_get_alive_num);
void chorogun_ext_disp(void);  // TODO: real signature
void chorogun_ext_main(void);  // TODO: real signature
void chorogun_ext_init(void);  // TODO: real signature
void chorogun_get_ptr(void);  // TODO: real signature
EVT_DEF(unk_evt_hei_00019e30);
EVT_DEF(chorogun_entry);
