#pragma once

#include <dolphin/mtx.h>
#include "evt/evt_cmd.h"
#include "memory.h"

typedef struct KpaSenEntry {
    s8 state;      // 0x0
    u8 pad_1;      // 0x1
    u16 timer;     // 0x2
    f32 progress;  // 0x4, state 1 lerps x/z from start to end by it, +0.03 per frame, done at 1.0
    Vec start;     // 0x8
    Vec end;       // 0x14
    u8 stopTexFirst;  // 0x20, calcPose's texId range once the move has ended (state 0x63)
    u8 stopTexLast;   // 0x21
    u8 moveTexFirst;  // 0x22, calcPose's texId range while moving (state 1)
    u8 moveTexLast;   // 0x23
} KpaSenEntry;

extern KpaSenEntry* kpaSen;

USER_FUNC(party_scale);
USER_FUNC(white_off);
USER_FUNC(white_on);
USER_FUNC(bakuhatu_kemuri);
USER_FUNC(kpaSen_toge_syugo);
USER_FUNC(kpaSen_zako_syugo);
void kpaSen_ext_dispent(void);  // TODO: real signature
void kpaSen_ext_shadow_disp(void);  // TODO: real signature
void kpaSen_ext_disp(void);  // TODO: real signature
void kpaSen_ext_main(void);  // TODO: real signature
s32 calcPose(s32 index, s32 first, s32 last, s32 fast);
void kpaSen_ext_init(void);
void kpa_stg5_009_7_cb(s32 type, smartEntry* smart);
USER_FUNC(party_tev);
EVT_DEF(villagerA_init);
EVT_DEF(villagerA_pig_init);
EVT_DEF(villagerA_talk);
EVT_DEF(villagerB_init);
EVT_DEF(villagerB_talk);
EVT_DEF(villagerC_init);
EVT_DEF(villagerC_talk);
EVT_DEF(villagerD_init);
EVT_DEF(villagerD_pig_init);
EVT_DEF(villagerD_talk);
EVT_DEF(villagerE_init);
EVT_DEF(villagerE_pig_init);
EVT_DEF(villagerE_talk);
EVT_DEF(villagerK_init);
EVT_DEF(villagerK_pig_init);
EVT_DEF(villagerK_talk);
EVT_DEF(village_chief_init);
EVT_DEF(village_chief_pig_init);
EVT_DEF(village_chief_talk);
EVT_DEF(chief_mario_talk_init);
EVT_DEF(chief_mario_talk);
EVT_DEF(nise_mario_talk);
EVT_DEF(christine_talk);
EVT_DEF(nokotaro_talk);
EVT_DEF(cloud_talk);
EVT_DEF(yoshi_talk);
EVT_DEF(crow_a_init);
EVT_DEF(crow_b_init);
EVT_DEF(crow_c_init);
EVT_DEF(crow_d_init);
EVT_DEF(crow_e_init);
EVT_DEF(usu00_stage4_coming_event);
EVT_DEF(usu00_3witch_init);
EVT_DEF(usu00_3witch_event);
EVT_DEF(gra00_2witch_find_ranpel_event);
EVT_DEF(usu00_vivian_cry_event);
EVT_DEF(usu00_vivian_joinup_event);
EVT_DEF(usu00_check_vivian);
EVT_DEF(usu00_crowAB_talk_event);
EVT_DEF(usu00_crowCDE_talk_event);
EVT_DEF(chief_pig_event);
EVT_DEF(evt_door_2_1);
EVT_DEF(evt_door_2_2);
EVT_DEF(evt_door2_chk);
EVT_DEF(evt_door2_close);
EVT_DEF(dokan_1_evt);
EVT_DEF(koopa_dokan_evt);
EVT_DEF(door_ie1_koopa_evt);
EVT_DEF(door_ie2_koopa_evt);
EVT_DEF(door_ie3_koopa_evt);
EVT_DEF(usu_00_tree_init);
EVT_DEF(kusa_01_evt);
EVT_DEF(evt_kusa_04);
EVT_DEF(evt_kusa_06);
EVT_DEF(usu_00_kusa_init);
EVT_DEF(evt_ichikorori_ball_get);
EVT_DEF(usu_00_touch_ichikorori_ball);
EVT_DEF(kanbu_regl);
EVT_DEF(kanbu_talk);
EVT_DEF(usu_00_koopa_first_evt);
EVT_DEF(bakuhatu_evt);
EVT_DEF(murabito_out);
EVT_DEF(usu_00_koopa_second_evt);
EVT_DEF(usu_00_init_evt);
