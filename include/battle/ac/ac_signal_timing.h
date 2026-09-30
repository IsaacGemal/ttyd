#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>

// Action-command work area at BattleWork+0x1F4C (ActionCommand::field_0x2BC).
typedef struct AcSignalTimingWork {
    s32 field_0x00; // 0x0, correct presses
    s32 field_0x04; // 0x4, failed presses
    s32 field_0x08; // 0x8, current button index
    s32 field_0x0C; // 0xC, signals elapsed
    s32 field_0x10; // 0x10, frame counter
    s32 field_0x14; // 0x14, ok window start
    s32 field_0x18; // 0x18, ok window end
    s32 field_0x1C; // 0x1C, ok window inner start
    s32 field_0x20; // 0x20, ok window inner end
    s32 field_0x24; // 0x24, pressed latch
    s32 field_0x28; // 0x28, signal end frame
    s32 field_0x2C; // 0x2C, difficulty
    s32 field_0x30; // 0x30, in-window flag
    s32 field_0x34; // 0x34, expected button
    u32 buttons[5]; // 0x38
    u8 states[8];   // 0x4C
    s32 field_0x54; // 0x54, post-AC timer
} AcSignalTimingWork;

s32 _get_ok_frame_range(s32 idx);
s32 battleAcMain_SignalTiming(BattleWork* work);
s32 battleAcResult_SignalTiming(BattleWork* work);
void battleAcDisp_SignalTiming(CameraId camId, void* param);
void battleAcDelete_SignalTiming(BattleWork* work);
