#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_signal_timing.h"
#include "battle/battle_unit.h"
#include "battle/battle_pad.h"
#include "battle/battle_disp.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// .sdata2 constants used by this unit. Order matches the original .sdata2 (report).
const u32 dat_804216d0 = 0xFFFFFFFF;
const u32 dat_804216d4 = 0xFFFFFFFF;
const u32 dat_804216d8 = 0xFFFFFFFF;
const u32 dat_804216dc = 0xFFFFFFFF;
const f32 float_neg200_804216e0 = -200.0f;
const f32 float_70_804216e4 = 70.0f;
const f32 float_1_804216e8 = 1.0f;
const f32 float_neg288_804216ec = -288.0f;
const f32 float_25_804216f0 = 25.0f;
const f32 float_0_804216f4 = 0.0f;
const f32 float_1p5_804216f8 = 1.5f;
const f32 float_0p5_804216fc = 0.5f;
const f32 float_15_80421700 = 15.0f;
const f32 float_0p8_80421704 = 0.8f;
const f32 float_0p6_80421708 = 0.6f;
const f32 float_20p8_8042170c = 20.800001f;
const f32 float_neg300_80421710 = -300.0f;
const f32 float_30_80421714 = 30.0f;

// The original unit's .rodata: 14 zero vectors, the int->float magic double, then the SFX names.
const Vec vec3_802fedc8[14] = {
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
};
const f64 double_to_int_802fee70 = 4503601774854144.0;
const char str_802FEE78[] = "SFX_AC_PI1";
const char str_802FEE84[] = "SFX_AC_PONE1";
const char str_802FEE94[] = "SFX_AC_COMMAND_OK1";
const char str_802FEEA8[] = "SFX_AC_COMMAND_NG1";

// Local to this file in the original (`.fn actionCommandDisp, local`).
static void actionCommandDisp(f32 x, f32 y);

// Tables defined in the original unit's .sdata. Non-const so mwcc places them in .sdata and uses
// the SDA21 address form (`li r3, sym@sda21`).
u32 _button_table_4dir[4] = {0x00010000, 0x00020000, 0x00040000, 0x00080000};
u32 _button_table_abxy[4] = {0x00000100, 0x00000200, 0x00000400, 0x00000800};
s32 ac_signaltiming_ok_frame_range[7] = {0x14, 0x10, 0x0C, 0x0A, 0x06, 0x04, 0x02};
s32 ac_signaltiming_ok_frame_range_2[7] = {0x1E, 0x1E, 0x1E, 0x1E, 0x19, 0x14, 0x0F};

// SignalTiming's action-command work area at BattleWork+0x1F4C (see the unit header).

// dont_inline: the original keeps _get_ok_frame_range as its own function and battleAcMain calls it
// through a `bl`; without this MWCC's deferred inliner folds the two switch arms into the caller.
#pragma dont_inline on
s32 _get_ok_frame_range(s32 idx) {
    BattleWork* work = _battleWorkPointer;

    switch (*(s32*)((u8*)work + 0x1ccc)) {
    case 1:
        return *(s32*)((u8*)work + 0x1ce4) + ac_signaltiming_ok_frame_range_2[idx];
    default:
        return *(s32*)((u8*)work + 0x1ce4) + ac_signaltiming_ok_frame_range[idx];
    }
}
#pragma dont_inline reset

// NONMATCHING (95.32%): callee-saved register allocation (SFX pointers land in r25-r28 here
// instead of the original's r28-r31; work/bd/disp/auto/cont shift accordingly).
#pragma optimize_for_size on // nudges the allocator toward the original's callee-saved register assignment
s32 battleAcMain_SignalTiming(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    AcSignalTimingWork* bd = (AcSignalTimingWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    const char* sfxNg;
    const char* sfxPi1;
    const char* sfxPone1;
    const char* sfxOk;
    s32 cont;
    s32 autoCommand = 0;
    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }
    sfxNg = str_802FEEA8;
    sfxPi1 = str_802FEE78;
    sfxPone1 = str_802FEE84;
    sfxOk = str_802FEE94;
    do {
        s32 pressed = 0;
        cont = 0;
        switch (ac->mAcState) {
        case 0: {
            s32 i;
            ac->result = 1;
            memset(disp, 0, 0x2C);
            disp->iconX = float_neg300_80421710;
            disp->iconY = float_0_804216f4;
            disp->counter = 0x14;
            bd->field_0x00 = 0;
            bd->field_0x04 = 0;
            bd->field_0x10 = 0;
            bd->field_0x08 = 0;
            bd->field_0x0C = 0;
            bd->field_0x2C = ((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work);
            bd->field_0x30 = 0;
            *(s32*)&ac->field_34[0x28] = 0;
            for (i = 0; i < 5; i++) {
                bd->states[i] = 0;
            }
            if (*(s32*)&ac->field_34[0x14] < 3 && *(s32*)&ac->field_34[0x14] >= 1) {
                for (i = 0; i < *(s32*)&ac->field_34[4];) {
                    u32 pick;
                    s32 found = 0;
                    do {
                        s32 j;
                        s32 unique = 1;
                        if (*(s32*)&ac->field_34[0x14] == 1) {
                            pick = _button_table_4dir[irand(4)];
                        } else {
                            pick = _button_table_abxy[irand(4)];
                        }
                        for (j = i - 1; j >= 0; j--) {
                            if (bd->buttons[j] == pick) {
                                unique = 0;
                            }
                        }
                        if (unique) {
                            found = 1;
                        }
                    } while (!found);
                    bd->buttons[i] = pick;
                    i++;
                }
                bd->field_0x34 = bd->buttons[0];
            } else {
                bd->field_0x34 = 0x100;
                for (i = 0; i < *(s32*)&ac->field_34[4]; i++) {
                    bd->buttons[i] = 0x100;
                }
                bd->field_0x34 = bd->buttons[0];
            }
            ac->mAcState = 0x63;
            break;
        }
        case 0x63:
            break;
        case 0x64:
            if (*(s16*)&ac->field_78[0x10] > 0) {
                (*(s16*)&ac->field_78[0x10])--;
                ac->result = 2;
                cont = 1;
                ac->mAcState = 0x3ea;
                break;
            }
            bd->field_0x14 = bd->field_0x08 * *(s32*)&ac->field_34[0xC];
            bd->field_0x18 = (bd->field_0x08 + 1) * *(s32*)&ac->field_34[0xC] - 1;
            bd->field_0x1C = bd->field_0x14;
            bd->field_0x20 = bd->field_0x1C + _get_ok_frame_range(bd->field_0x2C) - 1;
            ac->mAcState = 0x3e8;
            // fallthrough
        case 0x3e8:
            bd->field_0x28 = (bd->field_0x0C + 1) * *(s32*)&ac->field_34[0xC] - 1;
            bd->field_0x0C = bd->field_0x0C + 1;
            if (bd->field_0x0C < *(s32*)&ac->field_34[0x1C] + 1) {
                if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                    psndSFXOn(sfxPi1);
                }
            } else {
                if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                    psndSFXOn(sfxPone1);
                }
            }
            bd->field_0x24 = 0;
            ac->mAcState = 0x3e9;
            // fallthrough
        case 0x3e9: {
            s32 inWindow = 0;
            bd->field_0x10 = bd->field_0x10 + 1;
            if (bd->field_0x24 != 0) {
                bd->field_0x30 = 0;
            } else if (bd->field_0x08 < *(s32*)&ac->field_34[0x1C]) {
                bd->field_0x30 = 0;
            } else if (bd->field_0x10 < bd->field_0x1C || bd->field_0x10 > bd->field_0x20) {
                bd->field_0x30 = 0;
            } else {
                bd->field_0x30 = 1;
            }
            if (BattlePadCheckTrigger(bd->field_0x34) != 0) {
                pressed = 1;
            }
            if (pressed != 0) {
                if (*(s32*)&ac->field_34[0x14] == 2) {
                    if (bd->field_0x34 == BattlePadGetTrigger()) {
                        inWindow = 1;
                    } else {
                        inWindow = 0;
                    }
                } else {
                    inWindow = 1;
                }
            }
            if (autoCommand != 0 && bd->field_0x30 != 0) {
                pressed = 1;
                inWindow = 1;
            }
            if (bd->field_0x00 + bd->field_0x04 >= *(s32*)&ac->field_34[4]) {
                pressed = 0;
            }
            if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C] && pressed != 0 && bd->field_0x24 == 0) {
                bd->field_0x24 = 1;
                if (bd->field_0x10 >= bd->field_0x1C && bd->field_0x10 <= bd->field_0x20 && inWindow != 0) {
                    s32 idx;
                    bd->field_0x00 = bd->field_0x00 + 1;
                    *(s32*)&ac->field_34[0x28] = 1;
                    idx = bd->field_0x08;
                    if (idx >= *(s32*)&ac->field_34[0x1C] &&
                        idx < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                        bd->states[idx - *(s32*)&ac->field_34[0x1C]] = 1;
                        if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                            psndSFXOn(sfxOk);
                        }
                    }
                    if (*(s32*)&ac->field_34[0] & 0x10) {
                        *(s32*)&ac->field_34[0x24] = bd->field_0x00;
                        ac->result &= ~1;
                        if (bd->field_0x00 >= *(s32*)&ac->field_34[0x10]) {
                            ac->result |= 2;
                        }
                        cont = 1;
                        ac->mAcState = 0x3ea;
                    }
                } else {
                    s32 idx = bd->field_0x08;
                    if (idx >= *(s32*)&ac->field_34[0x1C] &&
                        idx < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                        bd->field_0x04 = bd->field_0x04 + 1;
                        if (bd->field_0x10 >= bd->field_0x14) {
                            bd->states[idx - *(s32*)&ac->field_34[0x1C]] = -1;
                            if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                                psndSFXOn(sfxNg);
                            }
                            *(s32*)&ac->field_34[0x28] = -1;
                        } else {
                            bd->field_0x24 = 0;
                        }
                    }
                }
            }
            if (bd->field_0x24 == 0 && bd->field_0x10 > bd->field_0x20) {
                s32 idx;
                bd->field_0x24 = 1;
                *(s32*)&ac->field_34[0x28] = -1;
                idx = bd->field_0x08;
                if (idx >= *(s32*)&ac->field_34[0x1C] &&
                    idx < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                    bd->field_0x04 = bd->field_0x04 + 1;
                    bd->states[idx - *(s32*)&ac->field_34[0x1C]] = -1;
                    if (!(ac->field_0x4 & 1) || !(ac->mAcUnit->mTokenFlags & 0x10)) {
                        psndSFXOn(sfxNg);
                    }
                }
            }
            if (bd->field_0x24 != 0) {
                if (bd->field_0x08 < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                    bd->field_0x08 = bd->field_0x08 + 1;
                    if (bd->field_0x08 < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                        bd->field_0x14 = bd->field_0x08 * *(s32*)&ac->field_34[0xC] - 0xa;
                        bd->field_0x18 = (bd->field_0x08 + 1) * *(s32*)&ac->field_34[0xC] - 1;
                        bd->field_0x1C = bd->field_0x08 * *(s32*)&ac->field_34[0xC];
                        bd->field_0x20 = bd->field_0x1C + _get_ok_frame_range(bd->field_0x2C) - 1;
                        bd->field_0x24 = 0;
                        if (bd->field_0x08 >= *(s32*)&ac->field_34[0x1C] &&
                            bd->field_0x08 < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                            bd->field_0x34 = bd->buttons[bd->field_0x08 - *(s32*)&ac->field_34[0x1C]];
                        }
                    }
                }
            }
            if (bd->field_0x10 >= bd->field_0x0C * *(s32*)&ac->field_34[0xC]) {
                if (bd->field_0x0C < *(s32*)&ac->field_34[0x1C] + *(s32*)&ac->field_34[4]) {
                    ac->mAcState = 0x3e8;
                } else {
                    *(s32*)&ac->field_34[0x24] = bd->field_0x00;
                    ac->result &= ~1;
                    if (bd->field_0x00 >= *(s32*)&ac->field_34[0x10]) {
                        ac->result |= 2;
                    }
                    cont = 1;
                    ac->mAcState = 0x3ea;
                }
            }
            break;
        }
        case 0x3ea:
            ac->mAcState = 0x3eb;
            break;
        case 0x3eb:
            break;
        case 0x3ec:
            ac->field_0x30 |= 1;
            bd->field_0x30 = 0;
            bd->field_0x54 = 0x3c;
            ac->mAcState = 0x3ed;
            break;
        case 0x3ed:
            bd->field_0x54 = bd->field_0x54 - 1;
            if (bd->field_0x54 <= 0) {
                ac->mAcState = 0x3ee;
            }
            break;
        case 0x3ee:
            ac->maincb = NULL;
            ac->dispcb = NULL;
            ac->resultcb = NULL;
            ac->deletecb = NULL;
            break;
        default:
            break;
        }
    } while (cont);

    return 1;
}
#pragma optimize_for_size reset
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

// NONMATCHING (58.44%): register allocation / stack slots; for the two causes.
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    const volatile Vec* vec = vec3_802fedc8;
    s32 num;
    s32 count;
    AcSignalTimingWork* bd;
    s32 i;
    f32 f31;
    f32 f28;

    camGetPtr(CAMERA_2D);
    bd = (AcSignalTimingWork*)((u8*)work + 0x1F4C);
    num = *(s32*)&work->actionCommands.field_34[4];
    count = bd->field_0x0C;

    if (!(*(s32*)&work->actionCommands.field_34[0] & 1)) {
        Vec t;
        if (bd->field_0x30 != 0) {
            t = vec[0];
            t.x = float_neg200_804216e0 + x;
            t.y = float_70_804216e4 + y;
            iconDispGx(&t, 0x10, 0x65, float_1_804216e8);
        } else {
            t = vec[1];
            t.x = float_neg200_804216e0 + x;
            t.y = float_70_804216e4 + y;
            iconDispGx(&t, 0x10, 0x64, float_1_804216e8);
        }
    }

    {
        s32 limit;
        switch (num) {
        case 1:
            limit = *(s32*)&work->actionCommands.field_34[0x1C] * 2 + 5;
            break;
        case 3:
            limit = *(s32*)&work->actionCommands.field_34[0x1C] * 2 + 0xB;
            break;
        case 4:
            limit = *(s32*)&work->actionCommands.field_34[0x1C] * 2 + 0xD;
            break;
        case 5:
            limit = *(s32*)&work->actionCommands.field_34[0x1C] * 2 + 0xF;
            break;
        default:
            limit = *(s32*)&work->actionCommands.field_34[0x1C] * 2 + 0xB;
            break;
        }
        {
            Vec t = vec[2];
            t.x = float_neg288_804216ec + x;
            f31 = float_25_804216f0 + y;
            t.y = f31;
            iconDispGx(&t, 0x10, 0x8F, float_1_804216e8);
        }
        for (i = 1; i < limit; i++) {
            Vec t = vec[3];
            t.x = x + (f32)(i * 0x10 - 0x120);
            t.y = f31;
            iconDispGx(&t, 0x10, 0x8E, float_1_804216e8);
        }
        {
            Vec t = vec[4];
            t.x = x + (f32)(i * 0x10 - 0x120);
            t.y = f31;
            iconDispGx(&t, 0x10, 0x90, float_1_804216e8);
        }
    }

    if (!(*(s32*)&work->actionCommands.field_34[0] & 4)) {
        Mtx mtx;
        Mtx scale;
        Mtx mtx2;
        s32 number = bd->field_0x00 + *(s32*)&work->actionCommands.field_34[0x18];
        {
            s32 ioff;
            if (*(s32*)&work->actionCommands.field_34[0] & 1) {
                ioff = (num * 0x30) + (*(s32*)&work->actionCommands.field_34[0x1C] << 5) - 0x1a;
            } else {
                ioff = (num * 0x30) + (*(s32*)&work->actionCommands.field_34[0x1C] << 5) - 0x1a;
            }
            PSMTXTrans(mtx, (float_neg200_804216e0 + x) + (f32)ioff, f31, float_0_804216f4);
        }
        if (*(s32*)&work->actionCommands.field_34[0] & 8) {
            PSMTXScale(scale, float_1p5_804216f8, float_1p5_804216f8, float_1p5_804216f8);
        } else {
            PSMTXScale(scale, float_1_804216e8, float_1_804216e8, float_1_804216e8);
        }
        PSMTXConcat(mtx, scale, mtx2);
        iconNumberDispGx(mtx2, number, 0, *(GXColor*)&dat_804216d0);
    }

    for (i = 0; i < *(s32*)&work->actionCommands.field_34[0x1C]; i++) {
        s32 off = i * 0x21 - 0x102;
        if (i >= count) {
            Vec t = vec[7];
            t.x = x + (f32)off;
            t.y = f31;
            iconDispGx(&t, 0x10, 0x91, float_0p5_804216fc);
        } else if (i >= *(s32*)&work->actionCommands.field_34[0x1C]) {
            Vec t = vec[6];
            t.x = x + (f32)off;
            t.y = f31;
            iconDispGx(&t, 0x10, 0x94, float_1_804216e8);
        } else {
            Vec t = vec[5];
            t.x = x + (f32)off;
            t.y = f31;
            iconDispGx(&t, 0x10, 0x92, float_1_804216e8);
        }
    }

    if (!(*(s32*)&work->actionCommands.field_34[0] & 1)) {
        for (i = 0; i < num; i++) {
            s32 off = *(s32*)&work->actionCommands.field_34[0x1C] * 0x21 + i * 0x30 - 0xfa;
            if (i + *(s32*)&work->actionCommands.field_34[0x1C] < count) {
                Vec t = vec[8];
                t.x = x + (f32)off;
                t.y = f31;
                iconDispGx(&t, 0x10, 0x95, float_1_804216e8);
            } else {
                Vec t = vec[9];
                t.x = x + (f32)off;
                t.y = f31;
                iconDispGx(&t, 0x10, 0x91, float_1_804216e8);
            }
        }
    } else if (!(*(s32*)&work->actionCommands.field_34[0] & 2)) {
        f28 = float_15_80421700 + y;
        for (i = 0; i < num; i++) {
            s32 off = *(s32*)&work->actionCommands.field_34[0x1C] * 0x21 + i * 0x30 - 0xfa;
            s32 t0 = i + *(s32*)&work->actionCommands.field_34[0x1C];
            s32 window = *(s32*)&work->actionCommands.field_34[0xC] * t0;
            if (count == t0 && bd->field_0x10 >= window - 5 && bd->field_0x10 < window) {
                Vec t = vec[3];
                t.x = x + (f32)off;
                t.y = f28;
                iconDispGx(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 0), float_0p8_80421704);
            } else {
                s32 delta;
                f32 scale = float_0p8_80421704;
                Vec t = vec[11];
                if (count > t0) {
                    delta = -0xa;
                } else {
                    delta = -2;
                    scale = float_0p6_80421708;
                }
                t.x = x + (f32)off;
                t.y = y + (f32)(delta + 0x19);
                iconDispGx(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 1), scale);
            }
        }
    } else {
        f28 = float_20p8_8042170c + f31;
        for (i = 0; i < num; i++) {
            s32 off = *(s32*)&work->actionCommands.field_34[0x1C] * 0x21 + i * 0x30 - 0xfa;
            s32 t0 = i + *(s32*)&work->actionCommands.field_34[0x1C];
            s32 window = *(s32*)&work->actionCommands.field_34[0xC] * t0;
            if (count == t0 && bd->field_0x10 >= window - 0x14 && bd->field_0x10 <= window) {
                Vec t = vec[3];
                t.x = x + (f32)off;
                t.y = f31;
                iconDispGx(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 0), float_0p8_80421704);
            } else if (count > t0) {
                Vec t = vec[13];
                t.x = x + (f32)off;
                t.y = f31;
                iconDispGx(&t, 0x10, BattleACGetButtonIcon(bd->buttons[i], 1), float_0p8_80421704);
            } else if (i == num - 1) {
                btlDispTexPlane(0x40, *(GXColor*)&dat_804216d4, x + (f32)off, f28, float_0_804216f4,
                                float_0p8_80421704, float_0p8_80421704);
            }
        }
    }

    f28 = f31 - float_25_804216f0;
    for (i = 0; i < num; i++) {
        s32 off = *(s32*)&work->actionCommands.field_34[0x1C] * 0x21 + i * 0x30 - 0xfa;
        if (bd->states[i] == 1) {
            btlDispTexPlane(0x3E, *(GXColor*)&dat_804216d8, x + (f32)off, f28, float_0_804216f4,
                            float_1_804216e8, float_1_804216e8);
        } else if (bd->states[i] == -1) {
            btlDispTexPlane(0x3F, *(GXColor*)&dat_804216dc, x + (f32)off, f28, float_0_804216f4,
                            float_1_804216e8, float_1_804216e8);
        }
    }
}
