#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>

// Action-command work area at BattleWork+0x1F4C (ActionCommand::field_0x2BC).
typedef struct AcSignalTimingWork {
    s32 hits;          // 0x0, correct presses
    s32 misses;        // 0x4, failed presses
    s32 curSignal;     // 0x8, index of the signal being played (signals start at acParams[6])
    s32 signalCount;   // 0xC, signals elapsed
    s32 frame;         // 0x10, frame counter
    s32 windowStart;   // 0x14, ok window start (signal start frame; minus 10 when advanced after a latched press)
    s32 windowEnd;     // 0x18, ok window end
    s32 okStart;       // 0x1C, ok window inner start
    s32 okEnd;         // 0x20, ok window inner end
    s32 latched;       // 0x24, 1 once this signal's press or miss has been counted
    s32 signalEnd;     // 0x28, signal end frame
    s32 difficulty;    // 0x2C, BattleActionCommandGetDifficulty at init; the ok frame range table index
    s32 inWindow;      // 0x30, 1 when the frame is inside the ok window for the current signal
    s32 expected;      // 0x34, button mask expected for the current signal
    u32 buttons[5];    // 0x38
    s8 states[8];      // 0x4C, per signal: 1 hit, -1 miss
    s32 endTimer;      // 0x54, frames the result display waits after the action command
} AcSignalTimingWork;

s32 _get_ok_frame_range(s32 idx);
s32 battleAcMain_SignalTiming(BattleWork* work);
s32 battleAcResult_SignalTiming(BattleWork* work);
void battleAcDisp_SignalTiming(CameraId camId, void* param);
void battleAcDelete_SignalTiming(BattleWork* work);
