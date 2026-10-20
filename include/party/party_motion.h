#pragma once

typedef struct PartyEntry PartyEntry;

#include "evt/evt_cmd.h"
USER_FUNC(evt_snd_bgmoff);
USER_FUNC(evt_snd_sfxon);
void allPartySlitOn(void);
void allPartySlitOff(void);
void allPartyForceSlitOff(void);
BOOL party_slit(PartyEntry* party);
s32 party_roll(PartyEntry* party);
void partyFollowHeight(PartyEntry* party);
void partyResetTouchWallCt(void);
s32 party_motion_fall(PartyEntry* party);
void party_motion_beside_mario(PartyEntry* party);
void party_motion_jump2(PartyEntry* party);
void party_motion_upstairs(PartyEntry* party);
void party_motion_damage(PartyEntry* party);
void party_motion_behind_mario(PartyEntry* party);
void party_motion_behind_mario_fly(PartyEntry* party);
void partyNormalGround(PartyEntry* party);
void party_motion_stay(PartyEntry* party);
void party_motion_homing_walk(PartyEntry* party);
void party_motion_homing_fly(PartyEntry* party);
void party_motion_jump(PartyEntry* party);
#include <dolphin/mtx.h>
s32 getFrontFloor(PartyEntry* party, Vec* pos, f32 angle);
void party_force_reset_outofscreen(PartyEntry* party);
void party_motion_behind_mario_continue(PartyEntry* party);
void N_partyFollowCloseOn(PartyEntry* party);
void N_partyFollowCloseOff(PartyEntry* party);
void L_partyForceSlitOff(PartyEntry* party);
USER_FUNC(evt_snd_bgm_scope);
USER_FUNC(evt_snd_bgmoff_f);
USER_FUNC(evt_snd_env_lpf);
USER_FUNC(evt_snd_envoff);
USER_FUNC(evt_snd_envoff_f);
USER_FUNC(evt_snd_envon);

USER_FUNC(evt_snd_bgm_freq);
USER_FUNC(evt_snd_bgmon_f);
USER_FUNC(evt_snd_envon_f);
USER_FUNC(evt_snd_set_rev_mode);
USER_FUNC(evt_snd_sfx_all_stop);
USER_FUNC(evt_snd_sfx_dist);
USER_FUNC(evt_snd_sfx_pit);
USER_FUNC(evt_snd_sfx_pos);
USER_FUNC(evt_snd_sfx_vol);
USER_FUNC(evt_snd_sfxchk);
USER_FUNC(evt_snd_sfxoff);
USER_FUNC(evt_snd_sfxon_);
USER_FUNC(evt_snd_sfxon_3d_ex);
USER_FUNC(evt_snd_sfxon__);
USER_FUNC(unk_8014cc24);
void partyUpdateFloor(PartyEntry* party);
void partyChgMotSlitFloor(PartyEntry* party);
