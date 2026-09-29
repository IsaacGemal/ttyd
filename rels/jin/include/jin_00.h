#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(teresa_eff);
USER_FUNC(senteresa_mario_spd);
USER_FUNC(senteresa_get_rot_center);
USER_FUNC(senteresa_wait_end_seq);
USER_FUNC(senteresa_end);
USER_FUNC(senteresa_turiageru);
USER_FUNC(senteresa_huttobu);
USER_FUNC(senteresa_dispoff);
USER_FUNC(senteresa_syugo);
USER_FUNC(senteresa_tikaduku_stop);
void senteresa_get_random_position(void);  // TODO: real signature
USER_FUNC(senteresa_get_seq);
void senteresa_loadTev(void);  // TODO: real signature
void senteresa_loadRenderMode(void);  // TODO: real signature
void computeAlpha(void);  // TODO: real signature
void computeMtx(void);  // TODO: real signature
void computeAngle(void);  // TODO: real signature
void randn(void);  // TODO: real signature
USER_FUNC(get_npc_color);
EVT_DEF(n3_open);
EVT_DEF(n3_close);
EVT_DEF(e3_open);
EVT_DEF(e3_close);
EVT_DEF(e1_open);
EVT_DEF(e1_close);
EVT_DEF(e2_open);
EVT_DEF(e2_close);
EVT_DEF(jin_00_dai);
EVT_DEF(jin_00_dai_in_sub);
EVT_DEF(jin_00_dai_in);
EVT_DEF(teresa_n_fadein);
EVT_DEF(teresa_n_fadeout);
EVT_DEF(door_teresa_n);
EVT_DEF(teresa_s_fadein);
EVT_DEF(teresa_s_fadeout);
EVT_DEF(door_teresa_s);
EVT_DEF(first_jin00);
EVT_DEF(nopush_evt);
EVT_DEF(tyuou_teresa_kusukusu);
EVT_DEF(tyuou_teresa_look_mario);
EVT_DEF(tyuou_teresa_init);
EVT_DEF(tyuou_teresa_talk);
EVT_DEF(oku_teresa_init);
EVT_DEF(temae_teresa_init);
EVT_DEF(atomic_teresa_init);
EVT_DEF(senteresa_evt);
EVT_DEF(jin_00_init_evt);
