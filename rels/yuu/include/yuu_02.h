#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include <dolphin/mtx.h>

typedef struct MonteEntryField8 {
    f32 field_0x0;                 // 0x00
    Vec pos;                       // 0x04
    f32 scale;                     // 0x10
    u8 pad_14[0x38 - 0x14];        // 0x14
} MonteEntryField8;                // 0x38

typedef struct MonteEntry {
    u8 hidden;                     // 0x00, monte_disp/monte_single_disp draw the chip only while 0
    u8 pad_01;                     // 0x01
    s16 count;                     // 0x02
    u8 state;                      // 0x04, fall_rtn: 0 until Mario picks the chip up (or it is left out), then 1
    u8 pad_05[0x08 - 0x05];        // 0x05
    MonteEntryField8 field_0x8;    // 0x08
    struct YuuminigameWork* work;  // 0x40
} MonteEntry;

typedef struct MonteTableEntry {
    f32 x0;                        // 0x00
    f32 x1;                        // 0x04
    f32 z0;                        // 0x08
    f32 z1;                        // 0x0C
    s32 count;                     // 0x10
    s32 type;                      // 0x14
} MonteTableEntry;

typedef struct YuuMinigame3Data {
    u32 flags;                     // 0x00
    s32 phase;                     // 0x04
    s32 counter;                   // 0x08
    s32 field_0C;                  // 0x0C, decremented per fall; at the end turned into the ranking's mult
    s32 index;                     // 0x10
    EventEntry* event;             // 0x14
    Vec position;                  // 0x18
    s32 monteCount;                // 0x24, chips picked up (fall_rtn), the ranking's bonus (minigame03_end LW(2))
    EventEntry* viewEvent;         // 0x28
    u8 pad_2C[0x40 - 0x2C];        // 0x2C
} YuuMinigame3Data;

#include "evt_yuuminigame.h"

s32 yuuminigame3_gamerule(EventEntry* event, BOOL isFirstCall, void* work, YuuMinigame3Data* data);
USER_FUNC(yuu_roll_reset);
USER_FUNC(yuuminigame3_opening_chk);
USER_FUNC(yuuminigame3_opening_on_off);
USER_FUNC(yuuminigame3_roll_fix_off);
USER_FUNC(yuuminigame3_roll_fix_on);
USER_FUNC(yuuminigame3_set_phase);
USER_FUNC(monte_single_init);
void monte_single_disp(CameraId cameraId, void* param);
USER_FUNC(unk_yuu_00001160);
void unk_yuu_000011e0(s32 index);
void fn_31_112C(s32 index);
USER_FUNC(minigame03_cam_reset);
EVT_DEF(monte_single_event);
EVT_DEF(yuu_02_jumpret);
EVT_DEF(yuu_02_kaijo_init);
EVT_DEF(yuu_02_kaijo_init2);
EVT_DEF(minigame03_main);
EVT_DEF(minigame03_start_cam);
EVT_DEF(minigame03_start_cam_cancel);
EVT_DEF(minigame03_start);
EVT_DEF(minigame03_end);
EVT_DEF(s_en1_turn);
EVT_DEF(yuu_02_init_evt);
