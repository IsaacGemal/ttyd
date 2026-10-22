#pragma once

#include "evt/evt_cmd.h"
#include "driver/extdrv.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"

// Per-entry state of the 200 "teresa" boxes managed by senteresa_ext_init/ext_main.
typedef struct TeresaEntry {
    f32 velX; //0x0, added to the pose each frame; set from 4.2f * v.x (jump-out)
    f32 velY; //0x4, upward speed, drops by 0.3f per frame (ballistic fall)
    f32 velZ; //0x8, set from v.z (jump-out) and cos(angle) (spin)
    f32 angle; //0xC, orbit angle in degrees (computePos, +spinSpeed per frame, wraps at 360)
    f32 offsetX; //0x10, x offset: from g_vecRotCenter before computePos rotates it by angle, from Mario once stuck to him
    f32 offsetY; //0x14, y offset from Mario while stuck to him
    f32 offsetZ; //0x18, z offset (same two uses as offsetX)
    f32 phase; //0x1C, bob phase (sin(1.2f * phase), wraps at pi); state 0x15 reuses it as the 0..1 start-to-end lerp
    f32 spinSpeed; //0x20, angular speed added to angle each frame (clamped at 1.2f)
    f32 startX; //0x24, saved pose position when a move starts (lerp source)
    f32 startY; //0x28
    f32 startZ; //0x2C
    f32 endX; //0x30, lerp target (npc position plus a circle offset)
    f32 endY; //0x34
    f32 endZ; //0x38
    s32 timer; //0x3C, frames left in the current state (counts down to 0; state 0x16 counts it up for the shake)
    u8 alpha; //0x40, color.a for the teresa's NPC
    u8 state; //0x41, state-machine step (senteresa_ext_main switch)
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
s32 computeAlpha(TeresaEntry* p, MarioWork* mario, f32 angle);
void computeMtx(ExtPose* pose, f32 angle);
f32 computeAngle(ExtPose* pose, MarioWork* mario);
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
