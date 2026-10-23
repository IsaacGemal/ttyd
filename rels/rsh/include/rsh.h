#pragma once

#include "evt/evt_cmd.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

// One npc of the rsh_0N_rnd_npc_N tables, as _get_rnd_npc_data hands it out
typedef struct RndNpcData {
    const char* animName;  // 0x0, evt_npc_entry's model
    const char* tribeName; // 0x4, evt_npc_set_tribe's tribe
    s32* talkEvt;          // 0x8, set as the npc's talk event (evt_rsh_set_rnd_npc_talk_evt)
} RndNpcData;

USER_FUNC(debug_print_func);
USER_FUNC(evt_rsh_set_rnd_npc_talk_evt);
USER_FUNC(evt_rsh_get_rnd_npc_talk_evt);
USER_FUNC(evt_rsh_get_rnd_npc_data);
RndNpcData* _get_rnd_npc_data(s32 type, s32 group, s32 index);
USER_FUNC(evt_rsh_set_rnd_npc);
USER_FUNC(evt_rsh_get_rnd_npc);
void rsh_window_tev(MapTevCallback* param, s32 arg1);
USER_FUNC(img_wait);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(rsh_01_a_init_evt);
EVT_DEF(rsh_01_b_init_evt);
EVT_DEF(rsh_01_c_init_evt);
EVT_DEF(rsh_01_a_init_evt_nidome);
EVT_DEF(rsh_01_b_init_evt_nidome);
EVT_DEF(rsh_01_c_init_evt_nidome);
EVT_DEF(rsh_07_a_init_evt);
EVT_DEF(rsh_07_b_init_evt);
EVT_DEF(rsh_07_c_init_evt);
EVT_DEF(rsh_08_a_init_evt);
extern BattleSetupData btlsetup_rsh_tbl[3];
extern u32 btl_rsh_setup_no_tbl[];
EVT_DEF(simi_check);
EVT_DEF(simi_offscreen_check);
EVT_DEF(kami_check);
EVT_DEF(kami_offscreen_check);
EVT_DEF(evt_rsh_door_init);
EVT_DEF(evt_rsh_door_entry);
EVT_DEF(evt_rsh_door_open);
EVT_DEF(evt_rsh_door_close);
EVT_DEF(evt_rsh_window);
EVT_DEF(evt_rsh_rnd_npc_2_1_talk);
EVT_DEF(evt_rsh_rnd_npc_2_2_talk);
EVT_DEF(evt_rsh_rnd_npc_2_3_talk);
EVT_DEF(evt_rsh_rnd_npc_2_4_talk);
EVT_DEF(evt_rsh_rnd_npc_2_5_talk);
EVT_DEF(evt_rsh_rnd_npc_2_6_talk);
EVT_DEF(evt_rsh_rnd_npc_2_7_talk);
EVT_DEF(evt_rsh_rnd_npc_2_8_talk);
EVT_DEF(evt_rsh_rnd_npc_2_9_talk);
EVT_DEF(evt_rsh_rnd_npc_2_10_talk);
EVT_DEF(evt_rsh_rnd_npc_3_1_talk);
EVT_DEF(evt_rsh_rnd_npc_3_2_talk);
EVT_DEF(evt_rsh_rnd_npc_3_3_talk);
EVT_DEF(evt_rsh_rnd_npc_3_4_talk);
EVT_DEF(evt_rsh_rnd_npc_3_5_talk);
EVT_DEF(evt_rsh_rnd_npc_3_6_talk);
EVT_DEF(evt_rsh_rnd_npc_3_7_talk);
EVT_DEF(evt_rsh_rnd_npc_3_8_talk);
EVT_DEF(evt_rsh_rnd_npc_3_9_talk);
EVT_DEF(evt_rsh_rnd_npc_3_10_talk);
EVT_DEF(evt_rsh_rnd_npc_4_1_talk);
EVT_DEF(evt_rsh_rnd_npc_4_2_talk);
EVT_DEF(evt_rsh_rnd_npc_4_3_talk);
EVT_DEF(evt_rsh_rnd_npc_4_4_talk);
EVT_DEF(evt_rsh_rnd_npc_4_5_talk);
EVT_DEF(evt_rsh_rnd_npc_4_6_talk);
EVT_DEF(evt_rsh_rnd_npc_4_7_talk);
EVT_DEF(evt_rsh_rnd_npc_4_8_talk);
EVT_DEF(evt_rsh_rnd_npc_4_9_talk);
EVT_DEF(evt_rsh_rnd_npc_4_10_talk);
EVT_DEF(evt_rsh_rnd_npc_5_1_talk);
EVT_DEF(evt_rsh_rnd_npc_5_2_talk);
EVT_DEF(evt_rsh_rnd_npc_5_3_talk);
EVT_DEF(evt_rsh_rnd_npc_5_4_talk);
EVT_DEF(evt_rsh_rnd_npc_5_5_talk);
EVT_DEF(evt_rsh_rnd_npc_5_6_talk);
EVT_DEF(evt_rsh_rnd_npc_5_7_talk);
EVT_DEF(evt_rsh_rnd_npc_5_8_talk);
EVT_DEF(evt_rsh_rnd_npc_5_9_talk);
EVT_DEF(evt_rsh_rnd_npc_5_10_talk);
EVT_DEF(debug_print_rnd_npc);
EVT_DEF(evt_rsh_rnd_npc_init);
EVT_DEF(evt_rsh_01_rnd_npc_entry);
EVT_DEF(evt_rsh_03_rnd_npc_entry);
EVT_DEF(evt_rsh_04_rnd_npc_entry);
// The original's debug_print_func copies a 256-byte zero template that is kept as a named
// .rodata object.
typedef struct {
    u8 b[256];
} Zero256;
struct MapObject;
// Match-only pair used by rsh_window_tev: keeping the object pointer and its flags word in one
// scalarised struct makes MWCC allocate them to r5/r6 (two plain locals come out swapped).
typedef struct RshWindowTevObj {
    struct MapObject* obj; // 0x0, param->obj
    u32 flags;             // 0x0, *(u32*)obj->field_0x0
} RshWindowTevObj;
