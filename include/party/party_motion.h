#pragma once

typedef struct PartyEntry PartyEntry;

#include "evt/evt_cmd.h"
USER_FUNC(evt_snd_bgmoff);
USER_FUNC(evt_snd_sfxon);
void allPartySlitOn(void);
void allPartyForceSlitOff(void);
BOOL party_slit(PartyEntry* party);
s32 party_roll(PartyEntry* party);
void unk_8014b824(PartyEntry* party);
s32 party_motion_fall(PartyEntry* party);
void party_motion_beside_mario(PartyEntry* party);
void party_motion_jump2(PartyEntry* party);
void party_motion_upstairs(PartyEntry* party);
void party_motion_damage(PartyEntry* party);
void party_motion_behind_mario(PartyEntry* party);
void party_motion_behind_mario_fly(PartyEntry* party);
void unk_8014bbc8(PartyEntry* party);
void party_motion_stay(PartyEntry* party);
void party_motion_homing_walk(PartyEntry* party);
void party_motion_homing_fly(PartyEntry* party);
void party_motion_jump(PartyEntry* party);
#include <dolphin/mtx.h>
s32 getFrontFloor(PartyEntry* party, Vec* pos, f32 angle);
void party_force_reset_outofscreen(PartyEntry* party);
void party_motion_behind_mario_continue(PartyEntry* party);
