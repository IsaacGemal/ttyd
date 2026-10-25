#include "battle/ac/ac_lst_rc_keep.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// iconDispGx's definition takes the icon id as s32, but this unit's original passes it zero-extended
// (u16); declare it with the width this file's asm passes (see tools/local_prototype_exceptions.json).
void iconDispGx(f32 scale, Vec* position, s16 flags, u16 iconId);

// LstRcKeep action-command work area at _battleWorkPointer + 0x1F4C. No header declares it.
// It begins where BattleACDispParams ends (the header stops at 0x2BC in BattleActions).
typedef struct LstRcKeepWork {
    s32 countdown;  // 0x00, frames left in the current timing window; reloaded from acParams[stage] when it runs out
    s32 stage;      // 0x04, index into the acParams timing array (0..4)
    s16 leftHold;   // 0x08, frames the stick has been held left (pad bit 0x40000), 0 when released
    s16 cRightHold; // 0x0A, frames the C-stick has been held right (pad bit 0x800000), 0 when released
    s32 endTimer;   // 0x0C, post-command timer
} LstRcKeepWork;

// sdata tables.
s32 ac_lstrckeep_ok_frame[7] = {0x50, 0x46, 0x3C, 0x28, 0x14, 0x0A, 0x05};
char* ac_lstrckeep_iconname[4] = {"_AcSkl1", "_AcSkl2", "_AcSkl3",
                                   "_AcSkl4"};

static void actionCommandDisp(f32 x, f32 y, s32 type);


// `BattleActionCommandGetDifficulty` is declared `s8`, so a plain call makes MWCC emit `extsb r0, r3`
// before the `slwi` table index; the original callee is `lbz r3, 0x1cbd(r3); blr` (zero-extending) and
// the caller does `slwi r0, r3, 2` with no extension, i.e. the return is really `u8`. Calling through a
// function pointer (as ac_stick_keep_left does) suppresses the `extsb`.
s32 battleAcMain_LstRcKeep(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    LstRcKeepWork* dec = (LstRcKeepWork*)&ac->acWork[0];
    BattleACDispParams* disp = &ac->dispParams;
    s32 stateChanged = 0;
    s32 autoCommand = 0;
    s32 cur;

    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        autoCommand = 1;
    }

    for (;;) {
        stateChanged = 0;

        switch (ac->mAcState) {
        case 0:
            ac->result = 1;
            dec->stage = 0;
            memset(disp, 0, 0x2C);
            disp->iconX = -300.0f;
            disp->iconY = 0.0f;
            disp->counter = 0x14;
            ac->mAcState = 0x63;
            break;
        case 0x63:
            break;
        case 0x64:
            dec->countdown = 0x78;
            ac->mAcState = 0x3E8;
            dec->leftHold = 0;
            dec->cRightHold = 0;
            // fall through
        case 0x3E8:
            if ((ac->autoFlags & 1) &&
                (ac->mAcUnit->mTokenFlags & 0x10)) {
                if (ac->autoFlags & 2) {
                    ac->autoTimer--;
                    if (ac->autoTimer >= 0) {
                        break;
                    }
                }
                if (irand(100) >= 0) {
                    ac->result = 0;
                }
                else {
                    ac->result = 2;
                    ac->mResultCount++;
                }
                return 0;
            }
            if (dec->leftHold == 0) {
                if (BattlePadCheckTrigger(0x40000) != 0) {
                    dec->leftHold = 1;
                }
            }
            else {
                dec->leftHold++;
                if (BattlePadCheckNow(0x40000) == 0) {
                    dec->leftHold = 0;
                }
            }
            if (dec->cRightHold == 0) {
                if (BattlePadCheckTrigger(0x800000) != 0) {
                    dec->cRightHold = 1;
                }
            }
            else {
                dec->cRightHold++;
                if (BattlePadCheckNow(0x800000) == 0) {
                    dec->cRightHold = 0;
                }
            }
            if ((dec->leftHold != 0 && dec->cRightHold != 0) ||
                ac->guardFrames > 0 || autoCommand) {
                ac->mAcState = 0x3E9;
                dec->countdown = ac->acParams[0];
                dec->stage = 0;
                dec->leftHold = 0;
                dec->cRightHold = 0;
            }
            else {
                dec->countdown--;
                if (dec->countdown <= 0) {
                    ac->result = 0;
                    stateChanged = 1;
                    ac->mAcState = 0x3EB;
                }
            }
            break;
        case 0x3E9:
            if (ac->guardFrames > 0) {
                (ac->guardFrames)--;
                ac->result = 2;
                stateChanged = 1;
                ac->mAcState = 0x3EB;
            }
            dec->countdown--;
            if (dec->leftHold > 0) {
                dec->leftHold++;
            }
            else if (BattlePadCheckNow(0x40000) == 0) {
                dec->leftHold = 1;
            }
            if (dec->cRightHold > 0) {
                dec->cRightHold++;
            }
            else if (BattlePadCheckNow(0x800000) == 0) {
                dec->cRightHold = 1;
            }
            if (!autoCommand && (dec->leftHold > 5 || dec->cRightHold > 5)) {
                ac->result &= ~1;
                stateChanged = 1;
                ac->mAcState = 0x3EB;
                break;
            }
            cur = dec->countdown;
            if (cur < 0) {
                dec->stage++;
                if (dec->stage >= 5) {
                    ac->result &= ~1;
                    stateChanged = 1;
                    ac->mAcState = 0x3EB;
                }
                else {
                    dec->countdown = ac->acParams[dec->stage];
                }
            }
            else if ((dec->leftHold != 0 && dec->cRightHold != 0) || autoCommand) {
                s32 flag = 0;
                if (dec->stage == 4) {
                    s32 remaining = ac->acParams[4] - cur;
                    if (remaining <= ac_lstrckeep_ok_frame[BattleActionCommandGetDifficulty(work)]) {
                        flag = 1;
                    }
                }
                if (autoCommand) {
                    if (flag) {
                        ac->result |= 2;
                        ac->result &= ~1;
                        stateChanged = 1;
                        ac->mAcState = 0x3EB;
                    }
                }
                else if (flag) {
                    ac->result |= 2;
                    ac->result &= ~1;
                    stateChanged = 1;
                    ac->mAcState = 0x3EB;
                }
                else {
                    ac->result &= ~1;
                    stateChanged = 1;
                    ac->mAcState = 0x3EB;
                }
            }
            break;
        case 0x3EB: {
            s32 n = dec->stage - 1;
            if (n >= 0) {
                s32 i;
                for (i = 0; i <= n; i++) {
                    iconDelete(ac_lstrckeep_iconname[i]);
                }
            }
            ac->mAcState = 0x3EC;
            // fall through
        }
        // fall through
        case 0x3EC:
        case 0x3ED:
            dec->endTimer = 0x3C;
            ac->mAcState = 0x3EE;
            break;
        case 0x3EE:
            dec->endTimer--;
            if (dec->endTimer <= 0) {
                ac->mAcState = 0x3EF;
            }
            break;
        case 0x3EF:
            ac->maincb = NULL;
            ac->dispcb = NULL;
            ac->resultcb = NULL;
            ac->deletecb = NULL;
            break;
        default:
            break;
        }

        if (!stateChanged) {
            break;
        }
    }

    return 1;
}

s32 battleAcResult_LstRcKeep(BattleWork* work) {
    return work->actionCommands.result;
}

// Holding disp and dec as separate pointers (rather than folding them into `param` at each use) makes
// MWCC materialise `disp = param + 0x1F20` in r31 (`addi r31, r4, 0x1f20`) and `dec = param + 0x1F4C` in
// r3, exactly as the original does; leaving a `param`-based use inside case 0x3E9 keeps `param` live and
// folds both into `0x1f34..0x1f50(r31)` instead (same wall ac_timing_wide hit).
void battleAcDisp_LstRcKeep(CameraId camId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    LstRcKeepWork* dec = (LstRcKeepWork*)&ac->acWork[0];
    s32 state = ac->mAcState;

    switch (state) {
    case 0x63:
    case 0x64:
    case 0x3E8:
        disp->field_0x24 = -1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.0f,
                                    30.0f);
        actionCommandDisp(disp->iconX, disp->iconY, disp->field_0x24);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E9:
        disp->field_0x24 = dec->stage - 1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, -300.0f,
                                    30.0f);
        actionCommandDisp(disp->iconX, disp->iconY, disp->field_0x24);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
    case 0x3EC:
        actionCommandDisp(30.0f, 0.0f, disp->field_0x24);
        break;
    case 0x3ED:
    case 0x3EE:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, 30.0f,
                                        -300.0f);
        }
        else {
            disp->iconX = 30.0f;
        }
        actionCommandDisp(disp->iconX, disp->iconY, disp->field_0x24);
        if (disp->counter < 0x3C) {
            disp->counter++;
        }
        break;
    default:
        break;
    }
}

void battleAcDelete_LstRcKeep(BattleWork* work) {
    work->actionCommands.mAcState = 0x3ED;
}


static void actionCommandDisp(f32 x, f32 y, s32 type) {
    s32 off;
    s32 i;

    camGetPtr(CAMERA_2D);

    if (type < 3) {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -168.0f + x;
        v.y = 70.0f + y;
        iconDispGx(1.0f, &v, 0x10, 0x71);
    }
    else {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -168.0f + x;
        v.y = 70.0f + y;
        iconDispGx(1.0f, &v, 0x10, 0x70);
    }

    if (type < 3) {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -232.0f + x;
        v.y = 70.0f + y;
        iconDispGx(1.0f, &v, 0x10, 0x78);
    }
    else {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -232.0f + x;
        v.y = 70.0f + y;
        iconDispGx(1.0f, &v, 0x10, 0x79);
    }

    {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -200.0f + x;
        y = 25.0f + y;
        v.y = y;
        iconDispGx(1.0f, &v, 0x10, 0x8E);
    }

    for (i = 0, off = 0; i < 3; i++, off += 0x21) {
        if (i <= type) {
            Vec v = (Vec){0.0f, 0.0f, 0.0f};
            v.x = x + (f32)(off - 0x102);
            v.y = y;
            iconDispGx(1.0f, &v, 0x10, i + 0x92);
        }
        else {
            Vec v = (Vec){0.0f, 0.0f, 0.0f};
            v.x = x + (f32)(off - 0x102);
            v.y = y;
            iconDispGx(0.5f, &v, 0x10, 0x91);
        }
    }

    if (type < 3) {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -151.0f + x;
        v.y = y;
        iconDispGx(1.0f, &v, 0x10, 0x91);
    }
    else {
        Vec v = (Vec){0.0f, 0.0f, 0.0f};
        v.x = -151.0f + x;
        v.y = y;
        iconDispGx(1.0f, &v, 0x10, 0x95);
    }
}

