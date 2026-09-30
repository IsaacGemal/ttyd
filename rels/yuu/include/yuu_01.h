#pragma once

#include "evt/evt_cmd.h"
#include "evt_yuuminigame.h"
#include <dolphin/mtx.h>

typedef struct Yuuminigame2Data {
    u32 flags;                  // 0x00
    s32 phase;                  // 0x04
    s32 unk08;                  // 0x08
    s32 unk0C;                  // 0x0C
    s32 unk10;                  // 0x10
    s32 unk14;                  // 0x14
    f32 unk18;                  // 0x18
    Vec unk1C;                  // 0x1C
    s32 unk28;                  // 0x28
    f32 unk2C;                  // 0x2C
    f32 unk30;                  // 0x30
    s32 unk34;                  // 0x34
    EventEntry* unk38;          // 0x38
} Yuuminigame2Data;


typedef struct MonteTable {
    f32 xmin;                   // 0x00
    f32 xmax;                   // 0x04
    f32 zmin;                   // 0x08
    f32 zmax;                   // 0x0C
    s32 count;                  // 0x10
} MonteTable;                    // 0x14

s32 yuuminigame2_gamerule(EventEntry* event, BOOL isFirstCall, void* work, Yuuminigame2Data* data);
// yuuminigame_disp_2fune: this unit has its own copy; a header already declares it.
// yuuminigame_main_2fune: this unit has its own copy; a header already declares it.
// yuuminigame_init_2fune: this unit has its own copy; a header already declares it.
USER_FUNC(yuuminigame2_opening_chk);
USER_FUNC(yuuminigame2_opening_on_off);
USER_FUNC(yuuminigame2_get_name);
USER_FUNC(yuuminigame2_set_phase);
USER_FUNC(unk_yuu_00005268);
EVT_DEF(yuu_01_kaijo_init);
EVT_DEF(yuu_01_kaijo_init2);
EVT_DEF(minigame02_main);
EVT_DEF(minigame02_start_cam);
EVT_DEF(minigame02_start_cam_cancel);
EVT_DEF(minigame02_start);
EVT_DEF(minigame02_end);
EVT_DEF(yuu_01_init_evt);
