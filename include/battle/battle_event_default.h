#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>

extern s32 btldefaultevt_Damage[];
extern s32 btldefaultevt_Wait[];
extern s32 btldefaultevt_Defend[];
extern s32 btldefaultevt_DefendParty[];
extern s32 btldefaultevt_Appeal[];
extern s32 btldefaultevt_Escape[];
extern s32 btldefaultevt_ChangeParty[];
extern s32 btldefaultevt_ProtectPartner[];
extern s32 btldefaultevt_ChangePosition[];
extern s32 btldefaultevt_LectureEnd[];
extern s32 btldefaultevt_BiribiriMove[];
extern s32 btldefaultevt_CantMoveZeroGravity[];

EVT_DEF(btldefaultevt_SuitoruBadgeEffect);
EVT_DEF(btldefaultevt_Confuse);
extern s32 btldefaultevt_Confuse[];
extern s32 btldefaultevt_Dummy[];
USER_FUNC(_btl_escape_reset_move_color_lv_player_unit_all);
USER_FUNC(_check_pose_stay);
USER_FUNC(_get_escape_rate);
USER_FUNC(_get_flower_suitoru_point);
USER_FUNC(_get_heart_suitoru_point);
USER_FUNC(_set_escape_ac_hlp_msg);
USER_FUNC(btlevtcmd_audience_code_escape);
USER_FUNC(btlevtcmd_audience_code_escape_fail);
USER_FUNC(btlevtcmd_audience_code_escape_success);
USER_FUNC(_backfire);
