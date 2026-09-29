#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>
s32 marioGetoffYoshi(void);
s32 yoshiGetStatus(void);
void evt_set_yoshi_spd(f32 spd);
void evt_set_yoshi_frm(s32 frm);
s32 evt_get_yoshi_frm(void);
void marioYoshiForceCancel(void);
void marioRideYoshi(void);
void yoshi_jumpStand(void);
void yoshi_use(PartyEntry* party);
void inertia(PartyEntry* party);
void yoshi_bye(PartyEntry* party);
void yoshi_exit(PartyEntry* party);
void yoshi_init(PartyEntry* party);
