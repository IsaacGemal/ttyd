#pragma once

#include "evt/evt_cmd.h"
#include "driver/extdrv.h"
#include "driver/npcdrv.h"

// Per-entry state of the 200 "teresa" boxes managed by senteresa_ext_init/ext_main.
typedef struct TeresaEntry {
    f32 field_0x00; //0x0
    f32 field_0x04; //0x4
    f32 field_0x08; //0x8
    f32 field_0x0C; //0xC
    f32 field_0x10; //0x10
    f32 field_0x14; //0x14
    f32 field_0x18; //0x18
    f32 field_0x1C; //0x1C
    f32 field_0x20; //0x20
    f32 field_0x24; //0x24
    f32 field_0x28; //0x28
    f32 field_0x2C; //0x2C
    f32 field_0x30; //0x30
    f32 field_0x34; //0x34
    f32 field_0x38; //0x38
    s32 field_0x3C; //0x3C
    u8 field_0x40;  //0x40
    u8 field_0x41;  //0x41
    u8 pad_0x42[2]; //0x42
} TeresaEntry;

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
USER_FUNC(senteresa_get_random_position);
USER_FUNC(senteresa_get_seq);
void senteresa_loadTev(void);
void senteresa_loadRenderMode(void);
s32 computeAlpha(TeresaEntry* p, NpcEntry* npc, f32 angle);
void computeMtx(ExtPose* pose, f32 angle);
f32 computeAngle(ExtPose* pose, NpcEntry* npc);
s32 randn(s32 min, s32 max);
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
void senteresa_ext_main(void);  // static in the original; declared here until it is written
