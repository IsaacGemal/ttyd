#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(zakogun_end_sukima);
USER_FUNC(zakogun_start_sukima);
void zakogun_escape1(void);  // TODO: real signature
USER_FUNC(zakogun_dead1);
USER_FUNC(zakogun_attack1);
void zakogun_ext_main1(void);  // TODO: real signature
void zakogun_ext_init123_common(void);  // TODO: real signature
void zakogun_ext_init1(void);  // TODO: real signature
void zakogun_get_kanbu_id(void);  // TODO: real signature
void zakogun_get_kanbu_id_evtcmd(void);  // TODO: real signature
void zakogun_ext_disp(void);  // TODO: real signature
void zakogun_ext_main(void);  // TODO: real signature
void zakogun_ext_init(void);  // TODO: real signature
void zakogun_get_ptr(void);  // TODO: real signature
USER_FUNC(zakogun_damage_pose_cancel);
USER_FUNC(zakogun_damage_pose);
void zakogun_set_pose(void);  // TODO: real signature
USER_FUNC(zakogun_set_target_id);
EVT_DEF(damage_event_kaiten_hammer);
EVT_DEF(zakogun_entry);
