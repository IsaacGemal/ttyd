#include "battle/ac/ac_power_gauge.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>

// actionCommandDisp is this unit's local helper (.fn actionCommandDisp, local).
static void actionCommandDisp(f32 x, f32 y);

// .sdata2 literals referenced by battleAcMain_PowerGauge and battleAcDisp_PowerGauge
// (offsets in REPORT.md).
#ifdef MUST_MATCH
// The original keeps the `lwz dat_80421588` late, after the disp stores; letting MWCC see the
// object as mutable stops it hoisting that load above them. __declspec(section) keeps the object
// in .sdata2, where the original const object lives.
__declspec(section ".sdata2") s32 dat_80421588 = -1;
#else
const s32 dat_80421588 = -1;
#endif

// Runtime work area of the power gauge action command, at BattleWork+0x1F4C (inside
// BattleActions.acWork). No header declares it yet.
typedef struct PowerGaugeDecWork {
    s32 frame;        // 0x0
    f32 gaugePercent; // 0x4
    s32 counter;      // 0x8, input-frame counter
    s32 value;        // 0xC
    s8 flag;          // 0x10
    s8 state;         // 0x11
    s8 flag2;         // 0x12
    u8 pad_0x13;      // 0x13
    u32 button;       // 0x14, BattlePad button mask
    s32 countdown;    // 0x18
} PowerGaugeDecWork;


// The near-miss this function had: MWCC hoists `lwz dat_80421588` above the disp stores
// (fixed by the mutable dat declaration above).
s32 battleAcMain_PowerGauge(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    PowerGaugeDecWork* dec = (PowerGaugeDecWork*)&ac->acWork[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 autoCommand = 0;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    switch (work->actionCommands.mAcState) {
    case 0x63:
        break;
    case 0x0:
        memset(disp, 0, 0x2C);
        disp->iconX = -300.f;
        disp->iconY = 0.0f;
        disp->counter = 0x14;
        disp->value = 0.0f;
        disp->gaugeColor.rgba = dat_80421588;
        dec->frame = 0;
        dec->counter = 0;
        work->actionCommands.outputParams[0] = 0;
        work->actionCommands.mAcState = 0x63;
        work->actionCommands.result = 1;
        dec->value = work->actionCommands.acParams[1] + work->actionCommands.acParams[2];
        dec->gaugePercent = 0.0f;
        switch (work->actionCommands.acParams[0]) {
        case 0:
            dec->flag2 = 1;
            dec->button = 0x60;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 1:
            dec->flag2 = 1;
            dec->button = 0x100;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        case 2:
            dec->flag2 = 0;
            dec->button = 0x100;
            dec->state = 0;
            break;
        case 3:
            dec->flag2 = 0;
            dec->button = 0x20;
            dec->state = 0;
            break;
        default:
            dec->flag2 = 1;
            dec->button = 0x100;
            dec->state = 1;
            work->actionCommands.outputParams[0] = 1;
            break;
        }
        break;
    case 0x64:
        if ((work->actionCommands.autoFlags & 1) &&
            (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
            if (irand(100) >= 0) {
                work->actionCommands.autoTimer = 0x1E;
                dec->flag = 0;
            }
            else {
                dec->flag = 1;
                work->actionCommands.autoTimer = 0x1E;
            }
            work->actionCommands.outputParams[0] = 1;
            work->actionCommands.mAcState = 0x3EA;
            break;
        }
        if (dec->flag2 != 0) {
            work->actionCommands.mAcState = 0x3E9;
        }
        else {
            work->actionCommands.mAcState = 0x3E8;
        }
        if (dec->flag2 != 0) {
            BattleAcGaugeSeInit();
        }
        // fallthrough
    case 0x3E8:
    case 0x3E9:
        dec->frame++;
        switch (dec->state) {
        case 0:
            if (dec->flag2 == 0 && (BattlePadGetNow() == dec->button || autoCommand != 0)) {
                dec->state = 1;
                work->actionCommands.mAcState = 0x3E9;
                dec->frame = 0;
                work->actionCommands.outputParams[0] = 1;
                BattleAcGaugeSeInit();
            }
            break;
        case 1:
            if (dec->flag2 != 0) {
                if (BattlePadGetNow() == dec->button) {
                    if (BattlePadCheckTrigger(dec->button) != 0) {
                        dec->state = 2;
                    }
                }
            }
            else {
                if (BattlePadCheckNow(dec->button) == 0) {
                    if (BattlePadCheckUp(dec->button) != 0) {
                        dec->state = 2;
                    }
                }
            }
            break;
        }
        if ((work->actionCommands.acFlag & 1) == 0) {
            dec->counter++;
        }
        else if (dec->state == 1) {
            dec->counter++;
        }
        if (work->actionCommands.mAcState == 0x3E8) {
            if (dec->frame > 0x78) {
                dec->frame = 0;
                work->actionCommands.result = 0;
                dec->gaugePercent = 0.0f;
                work->actionCommands.mAcState = 0x3EB;
                work->actionCommands.outputParams[0] = 2;
                break;
            }
        }
        else {
            if (dec->frame > dec->value) {
                dec->frame = 0;
                work->actionCommands.result = 0;
                dec->gaugePercent = 101.0f;
                work->actionCommands.mAcState = 0x3EB;
                break;
            }
        }
        if (dec->frame <= work->actionCommands.acParams[1]) {
            dec->gaugePercent = intplGetValue(work->actionCommands.acParams[3], dec->counter,
                                              work->actionCommands.acParams[1], 0.0f, 100.f);
        }
        else {
            dec->gaugePercent = 100.f;
        }
        BattleAcGaugeSeUpdate(dec->gaugePercent);
        if (autoCommand != 0) {
            if (dec->gaugePercent >= (f32)work->actionCommands.acParams[4]) {
                dec->state = 2;
            }
            else {
                dec->state = 1;
            }
        }
        if (dec->state == 2) {
            if (dec->gaugePercent < (f32)work->actionCommands.acParams[4]) {
                work->actionCommands.result = 0;
                work->actionCommands.result &= ~2;
            }
            else {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            work->actionCommands.mAcState = 0x3EB;
        }
        break;
    case 0x3EA:
        work->actionCommands.autoTimer--;
        if (work->actionCommands.autoTimer < 0) {
            if (dec->flag != 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            }
            else {
                work->actionCommands.result = 0;
            }
            work->actionCommands.endFlags |= 1;
            dec->countdown = 0x3C;
            work->actionCommands.mAcState = 0x3ED;
            return 0;
        }
        break;
    case 0x3EB:
        work->actionCommands.mAcState = 0x3EC;
        // fallthrough
    case 0x3EC:
        work->actionCommands.endFlags |= 1;
        BattleAcGaugeSeDelete();
        dec->countdown = 0x3C;
        work->actionCommands.mAcState = 0x3ED;
        // fallthrough
    case 0x3ED:
        dec->countdown--;
        if (dec->countdown <= 0) {
            work->actionCommands.mAcState = 0x3EE;
        }
        break;
    case 0x3EE:
        work->actionCommands.maincb = NULL;
        work->actionCommands.dispcb = NULL;
        work->actionCommands.resultcb = NULL;
        work->actionCommands.deletecb = NULL;
        return 0;
    }
    return 1;
}

s32 battleAcResult_PowerGauge(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_PowerGauge(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    s32 state = ac->mAcState;

    switch (state) {
    case 0x63:
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.f, 30.f);
        // fallthrough
    case 0x64:
    case 0x3e8:
    case 0x3e9:
    case 0x3eb:
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3ec:
    case 0x3ed:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.f, -300.f);
        } else {
            disp->iconX = 30.f;
        }
        actionCommandDisp(disp->iconX, disp->iconY);
        if (disp->counter < 0x3c) {
            disp->counter++;
        }
        break;
    }
}

void battleAcDelete_PowerGauge(BattleWork* work) {
    if (!(work->actionCommands.endFlags & 1)) {
        work->actionCommands.mAcState = 0x3EC;
    }
}

static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    PowerGaugeDecWork* dec;
    s32 iconA;
    s32 iconB;
    s32 iconC;
    s32 iconD;
    f32 ratio;
    // Copies of the parameters: using x/y directly lets their argument live ranges (f1/f2) merge
    // with the body's float temporaries and costs the f30/f31 save (98.96%).
    f32 px = x;
    f32 py = y;

    camGetPtr(CAMERA_2D);
    dec = (PowerGaugeDecWork*)&ac->acWork[0];

    ratio = dec->gaugePercent / 100.f;
    if (ratio > 1.0f) {
        ratio = 1.0f;
    }
    disp->value = ratio;

    if (ac->acFlag & 2) {
        BattleAcDrawGauge((s32)px, (s32)py, ratio, 0xb2, 1, 100, 100, 100, 0);
    }
    else {
        BattleAcDrawGauge((s32)px, (s32)py, ratio, 0xb2, 2, ac->acParams[4], 100, 100, 0);
    }

    if ((ac->acFlag & 4) == 0) {
        Vec pos = (Vec){0.0f, 0.0f, 0.0f};
        pos.x = -288.0f + px +
                (f32)(ac->acParams[4] * 0xb0) / 100.f;
        pos.y = 45.0f + py;
        iconDispGx(1.0f, &pos, 0x10, 0x96);
    }
    else {
        s32 color;

        if (dec->gaugePercent >= (f32)ac->acParams[4] &&
            dec->gaugePercent <= 100.f) {
            color = 0x95;
        }
        else {
            color = 0x91;
        }
        if (ac->acFlag & 8) {
            Vec pos = (Vec){0.0f, 0.0f, 0.0f};
            pos.x = -200.f + px + 120.f - 12.0f;
            pos.y = 18.0f + py;
            iconDispGx(1.0f, &pos, 0x10, color);
        }
        else {
            Vec pos = (Vec){0.0f, 0.0f, 0.0f};
            pos.x = -200.f + px + 120.f;
            pos.y = 18.0f + py;
            iconDispGx(1.0f, &pos, 0x10, color);
        }
    }

    switch (ac->acParams[0]) {
    case 0:
        iconA = 0x7f;
        iconB = 0x7e;
        iconC = 0x81;
        iconD = 0x80;
        break;
    case 1:
        iconA = BattleACGetButtonIcon(0x100, 1);
        iconB = BattleACGetButtonIcon(0x100, 0);
        iconC = 0;
        iconD = 0;
        break;
    case 2:
        iconA = BattleACGetButtonIcon(0x100, 1);
        iconB = BattleACGetButtonIcon(0x100, 0);
        iconC = 0;
        iconD = 0;
        break;
    case 3:
        iconA = BattleACGetButtonIcon(0x20, 1);
        iconB = BattleACGetButtonIcon(0x20, 0);
        iconC = 0;
        iconD = 0;
        break;
    }

    switch (ac->mAcState) {
    case 0x63:
    case 0x64:
    case 0x3eb:
    case 0x3ec:
    case 0x3ed:
        if ((iconC & 0xffff) != 0) {
            px = -225.0f + px;
            py = 80.f + py;
            {
                Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                pos.x = px;
                pos.y = py;
                iconDispGx(1.0f, &pos, 0x10, iconB);
            }
            {
                Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                pos.x = 56.0f + px;
                pos.y = py;
                iconDispGx(1.0f, &pos, 0x10, iconD);
            }
        }
        else {
            Vec pos = (Vec){0.0f, 0.0f, 0.0f};
            pos.x = -225.0f + px + 28.0f;
            pos.y = 80.f + py;
            iconDispGx(1.0f, &pos, 0x10, iconB);
        }
        break;
    case 0x3e8:
    case 0x3e9:
        if (dec->flag2 != 0) {
            if (dec->gaugePercent >= (f32)ac->acParams[4] &&
                dec->gaugePercent <= 100.f) {
                if ((iconC & 0xffff) != 0) {
                    px = -225.0f + px;
                    py = 80.f + py;
                    {
                        Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                        pos.x = px;
                        pos.y = py;
                        iconDispGx(1.0f, &pos, 0x10, iconA);
                    }
                    {
                        Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                        pos.x = 56.0f + px;
                        pos.y = py;
                        iconDispGx(1.0f, &pos, 0x10, iconC);
                    }
                }
                else {
                    Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = -225.0f + px + 28.0f;
                    pos.y = 80.f + py;
                    iconDispGx(1.0f, &pos, 0x10, iconA);
                }
            }
            else {
                if ((iconC & 0xffff) != 0) {
                    px = -225.0f + px;
                    py = 80.f + py;
                    {
                        Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                        pos.x = px;
                        pos.y = py;
                        iconDispGx(1.0f, &pos, 0x10, iconB);
                    }
                    {
                        Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                        pos.x = 56.0f + px;
                        pos.y = py;
                        iconDispGx(1.0f, &pos, 0x10, iconD);
                    }
                }
                else {
                    Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = -225.0f + px + 28.0f;
                    pos.y = 80.f + py;
                    iconDispGx(1.0f, &pos, 0x10, iconB);
                }
            }
        }
        else if ((dec->gaugePercent >= (f32)ac->acParams[4] &&
                  dec->gaugePercent <= 100.f) ||
                 ac->outputParams[0] == 0) {
            if ((iconC & 0xffff) != 0) {
                px = -225.0f + px;
                py = 80.f + py;
                {
                    Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = px;
                    pos.y = py;
                    iconDispGx(1.0f, &pos, 0x10, iconB);
                }
                {
                    Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = 56.0f + px;
                    pos.y = py;
                    iconDispGx(1.0f, &pos, 0x10, iconD);
                }
            }
            else {
                Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                pos.x = -225.0f + px + 28.0f;
                pos.y = 80.f + py;
                iconDispGx(1.0f, &pos, 0x10, iconB);
            }
        }
        else {
            if ((iconC & 0xffff) != 0) {
                px = -225.0f + px;
                py = 80.f + py;
                {
                    Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = px;
                    pos.y = py;
                    iconDispGx(1.0f, &pos, 0x10, iconA);
                }
                {
                    Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = 56.0f + px;
                    pos.y = py;
                    iconDispGx(1.0f, &pos, 0x10, iconC);
                }
            }
            else {
                Vec pos = (Vec){0.0f, 0.0f, 0.0f};
                pos.x = -225.0f + px + 28.0f;
                pos.y = 80.f + py;
                iconDispGx(1.0f, &pos, 0x10, iconA);
            }
        }
        break;
    }
}

// .sdata2 definitions of this unit, in the original's order.
