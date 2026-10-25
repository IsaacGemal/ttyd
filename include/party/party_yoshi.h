#pragma once

typedef struct PartyEntry PartyEntry;
struct HitEntry;

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
void L_getStick(PartyEntry* party, f32* outAngle, f32* outDist);
void checkWall(PartyEntry* party, f32 a, f32 b);
void searchGround2(PartyEntry* party);
void searchGround(PartyEntry* party);
void L_yoshiFlyMove(PartyEntry* party);
void unk_800b45e0(PartyEntry* party);
void L_sweatMain(PartyEntry* party);
void L_sweatEntry(PartyEntry* party, struct HitEntry* hit);
void unk_800b474c(PartyEntry* party);
