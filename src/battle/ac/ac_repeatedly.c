#include "battle/ac/ac_repeatedly.h"
#include "battle/battle.h"
#include "system.h"

static void actionCommandDisp(f32 x, f32 y);

// Float literals this unit emitted into .sdata2 (offsets listed in REPORT.md).
extern const f32 float_10_8041c7e0;
extern const f32 float_0_8041c7d8;
extern const f32 float_0p01_8041c7c0;

// int->float conversion magic the original unit emitted into .rodata.
const f64 double_to_int_802f31a0 = 4503601774854144.0;

// Repeatedly's action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct AcRepeatedlyWork {
    s32 field_0x00; // 0x0
    s32 field_0x04; // 0x4
    s32 field_0x08; // 0x8
    s32 field_0x0C; // 0xC
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    s16 field_0x20; // 0x20
    s16 field_0x22; // 0x22
    s16 field_0x24; // 0x24
    s16 field_0x26; // 0x26
    s16 field_0x28; // 0x28
    u8 field_0x2A;  // 0x2A
    u8 field_0x2B;  // 0x2B
    u8 field_0x2C;  // 0x2C
} AcRepeatedlyWork;

static void _init_param(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    AcRepeatedlyWork* dec = (AcRepeatedlyWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 param;
    s32 timer;

    dec->field_0x2C = 0;
    dec->field_0x0C = 0;

    switch (*(s32*)&ac->field_34[0x04]) {
    case 10:
        work->flags |= 0x20000000;
        break;
    case 8:
        dec->field_0x0C = 0x100;
        // fallthrough
    case 6:
        dec->field_0x08 = 0x40000;
        dec->field_0x04 = 0x40000;
        dec->field_0x00 = 0x40000;
        break;
    case 2:
        dec->field_0x08 = 0x200;
        dec->field_0x04 = 0x200;
        dec->field_0x00 = 0x200;
        break;
    case 3:
        dec->field_0x08 = 0x400;
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        break;
    case 4:
        dec->field_0x08 = 0x800;
        dec->field_0x04 = 0x800;
        dec->field_0x00 = 0x800;
        break;
    case 5:
        dec->field_0x08 = 0x20;
        dec->field_0x04 = 0x20;
        dec->field_0x00 = 0x20;
        break;
    case 11:
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        dec->field_0x08 = 0x200;
        break;
    case 16:
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        dec->field_0x08 = 0x200;
        dec->field_0x22 = irand(0x1E) + 0x3C;
        break;
    case 12:
        dec->field_0x04 = 0x400;
        dec->field_0x00 = 0x400;
        dec->field_0x08 = 0x800;
        break;
    default:
        dec->field_0x08 = 0x100;
        dec->field_0x04 = 0x100;
        dec->field_0x00 = 0x100;
        break;
    }

    param = *(s32*)&ac->field_34[0x08];
    dec->field_0x28 = param;
    dec->field_0x26 = param;
    dec->field_0x10 = (f32)*(s32*)&ac->field_34[0x0C];
    dec->field_0x14 = (f32)*(s32*)&ac->field_34[0x10] / float_10_8041c7e0;
    if (dec->field_0x14 != float_0_8041c7d8) {
        dec->field_0x10 = (f32)irand(100);
    }
    dec->field_0x18 = (f32)*(s32*)&ac->field_34[0x20];
    disp->value = float_0p01_8041c7c0 * dec->field_0x18;
    dec->field_0x20 = 0;
    dec->field_0x24 = *(s32*)&ac->field_34[0x18];
    dec->field_0x1C = float_0p01_8041c7c0 * (f32)*(s32*)&ac->field_34[0x1C];
    dec->field_0x2A = 0xB2;

    timer = *(s16*)&ac->field_78[0x10];
    if (timer > 0) {
        *(s16*)&ac->field_78[0x10] = timer - 1;
        dec->field_0x2B = 1;
    } else {
        dec->field_0x2B = 0;
    }
    ac->outputParams[0] = 0;
}

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
