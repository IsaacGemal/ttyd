#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_signal_timing.h"
#include "system.h"

// .sdata2 constants used by battleAcDisp_SignalTiming.
const f32 float_neg300_80421710 = -300.0f;
const f32 float_30_80421714 = 30.0f;
const f32 float_0_804216f4 = 0.0f;

// Local to this file in the original (`.fn actionCommandDisp, local`).
static void actionCommandDisp(f32 x, f32 y);

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

void battleAcDisp_SignalTiming(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_80421710, float_30_80421714);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E8:
    case 0x3E9:
    case 0x3EA:
    case 0x3EB:
        actionCommandDisp(float_30_80421714, float_0_804216f4);
        break;
    case 0x3EC:
    case 0x3ED:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_80421714, float_neg300_80421710);
        } else {
            disp->iconX = float_30_80421714;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3C) {
            disp->counter++;
        }
        break;
    default:
        break;
    }
}

void battleAcDelete_SignalTiming(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}
