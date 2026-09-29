#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(battenSatelliteToOutside);
USER_FUNC(battenSatelliteToInside);
void battenSatelliteTo_Main(void);  // TODO: real signature
void battenSatelliteTo_Init(void);  // TODO: real signature
USER_FUNC(unk_las_0000d538);
void _light_point_callback(void);  // TODO: real signature
USER_FUNC(btl_hitoffsetx);
USER_FUNC(pose_check_evt_stop);
USER_FUNC(pose_check_evt_run);
USER_FUNC(datastrcopy);
USER_FUNC(enzan_lw_load2);
USER_FUNC(enzan_lw_save2);
USER_FUNC(btlevtcmd_AnimeGetPose);
void battleGetBattenSatelliteInitPos(void);  // TODO: real signature
USER_FUNC(check_satellite);
void evt_eff_firechange00(void);  // TODO: real signature
void evt_eff_barriersizechange(void);  // TODO: real signature
void evt_eff_colorchange00(void);  // TODO: real signature
USER_FUNC(evt_eff_pokopi_thunder);
EVT_DEF(batten_leader_zoom);
EVT_DEF(hakai_snd_event);
EVT_DEF(light_anim_rtn);
EVT_DEF(light_rod_anim_rtn);
EVT_DEF(light_pose_change);
EVT_DEF(maiframeanimcheck);
EVT_DEF(damage_sub_event);
EVT_DEF(monster_before_phase_event);
EVT_DEF(sub_call_satellite);
EVT_DEF(player_before_phase_event);
EVT_DEF(batten_leader_thunder_attack_event);
EVT_DEF(batten_leader_fire_attack_event);
EVT_DEF(batten_leader_ice_attack_event);
EVT_DEF(batten_leader_shikaeshi_attack_event);
EVT_DEF(batten_leader_yokeyoke_attack_event);
EVT_DEF(batten_leader_stop_attack_event);
