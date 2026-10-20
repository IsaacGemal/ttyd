#pragma once

#include "driver/dispdrv.h"
#include "eff/eff_mahojin.h"
#include "evt/evt_cmd.h"

USER_FUNC(credits_moamoa_free);
USER_FUNC(credits_moamoa_setup);
void credits_ext_draw(void);
void credits_ext_disp(CameraId cameraId, void* param);
void credits_ext_update(void);
void credits_ext_init(void);
f32 credits_rand_range(f32 min, f32 max);
USER_FUNC(credits_npc_setup);
USER_FUNC(unk_end_000009a4);
USER_FUNC(credits_font_reset);
void credits_free(void);
USER_FUNC(credits_init);
USER_FUNC(credits_record_evt);
USER_FUNC(credits_set_tev_alpha);
USER_FUNC(credits_set_mode);
USER_FUNC(credits_set_overlay_alpha);
USER_FUNC(credits_set_mat_alpha);
USER_FUNC(credits_disp_start);
void credits_draw(CameraId cameraId, void* param);
USER_FUNC(credits_pose_evt);
USER_FUNC(credits_npc_kind);
void credits_pose_disp(CameraId cameraId, void* param);
void credits_pose_gx(s32 wXluStage);
void credits_tev_callback(TevCallbackParam* param);
EVT_DEF(evt_credits_pose_loop);
EVT_DEF(evt_credits_fade);
EVT_DEF(evt_credits_sequence);
EVT_DEF(evt_credits_end);
EVT_DEF(evt_credits_start);
EVT_DEF(evt_credits_gesso);
EVT_DEF(evt_credits_gonbaba);
EVT_DEF(evt_credits_batten2);
EVT_DEF(evt_credits_atomic);
EVT_DEF(evt_credits_korutesu);
EVT_DEF(evt_credits_para);
EVT_DEF(evt_credits_batten);
EVT_DEF(evt_credits_queen);
EVT_DEF(evt_credits_moamoa);
