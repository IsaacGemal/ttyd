#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(battenSatelliteToOutside);
USER_FUNC(battenSatelliteToInside);
struct BattleUnit;
u8 battenSatelliteTo_Main(struct BattleUnit* unit, s32 index);
u8 battenSatelliteTo_Init(struct BattleUnit* unit, s32 index, u32* table);
extern u32 xlw2[16];
extern u32 xmemo[5];
extern EventEntry* xevt[1];
USER_FUNC(unk_las_0000d538);
struct BattleWorkUnitPart;
void _light_point_callback(void* userData, s32 group, Mtx mtx);
USER_FUNC(btl_hitoffsetx);
USER_FUNC(pose_check_evt_stop);
USER_FUNC(pose_check_evt_run);
USER_FUNC(datastrcopy);
USER_FUNC(enzan_lw_load2);
USER_FUNC(enzan_lw_save2);
USER_FUNC(btlevtcmd_AnimeGetPose);
USER_FUNC(battleGetBattenSatelliteInitPos);
USER_FUNC(check_satellite);
USER_FUNC(evt_eff_firechange00);
USER_FUNC(evt_eff_barriersizechange);
USER_FUNC(evt_eff_colorchange00);
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
