#include "battle/ac/ac_repeatedly.h"
#include "battle/battle.h"
#include "system.h"

static void actionCommandDisp(f32 x, f32 y);

s32 battleAcResult_Repeatedly(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_Repeatedly(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
    case 99:
    case 100:
    case 1000:
    case 1001:
        disp->iconX = intplGetValue(4, 20 - disp->counter, 20, -300.0f, 30.0f);
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 1002:
    case 1003:
        if (disp->counter >= 40) {
            disp->iconX = intplGetValue(4, disp->counter - 40, 20, 30.0f, -300.0f);
        } else {
            disp->iconX = 30.0f;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 60) {
            disp->counter++;
        }
        break;
    }
}

void battleAcDelete_Repeatedly(BattleWork* work) {
    switch (*(s32*)&work->actionCommands.field_34[0x04]) {
    case 0xA:
        work->flags &= ~0x20000000;
        break;
    }
    BattleAcGaugeSeDelete();
    work->actionCommands.mAcState = 0x3EA;
}
