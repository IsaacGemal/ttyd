#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_signal_timing.h"

// Tables defined in the original unit's .sdata. Non-const so mwcc places them in .sdata and uses
// the SDA21 address form (`li r3, sym@sda21`).
s32 ac_signaltiming_ok_frame_range[7] = {0x14, 0x10, 0x0C, 0x0A, 0x06, 0x04, 0x02};
s32 ac_signaltiming_ok_frame_range_2[7] = {0x1E, 0x1E, 0x1E, 0x1E, 0x19, 0x14, 0x0F};

s32 _get_ok_frame_range(s32 idx) {
    BattleWork* work = _battleWorkPointer;

    switch (*(s32*)((u8*)work + 0x1ccc)) {
    case 1:
        return *(s32*)((u8*)work + 0x1ce4) + ac_signaltiming_ok_frame_range_2[idx];
    default:
        return *(s32*)((u8*)work + 0x1ce4) + ac_signaltiming_ok_frame_range[idx];
    }
}

s32 battleAcResult_SignalTiming(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_SignalTiming(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}
