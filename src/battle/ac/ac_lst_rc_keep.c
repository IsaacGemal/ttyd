#include "battle/ac/ac_lst_rc_keep.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

// LstRcKeep action-command work area at _battleWorkPointer + 0x1F4C. No header declares it.
// It begins where BattleACDispParams ends (the header stops at 0x2BC in BattleActions).
typedef struct LstRcKeepWork {
    s32 field_0x00; // 0x00, countdown timer / current timing window
    s32 field_0x04; // 0x04, index into the field_34 timing array
    s16 field_0x08; // 0x08, A-button (0x4) hold counter
    s16 field_0x0A; // 0x0A, B-button (0x80) hold counter
    s32 field_0x0C; // 0x0C, post-command timer
} LstRcKeepWork;

// rodata strings, one per icon. They are the first objects of the unit's .rodata, so the compiler
// uses str_802F94E0 as the section base for the vector constants that follow it.
const char str_802F94E0[] = "_AcSkl1";
const char str_802F94E8[] = "_AcSkl2";
const char str_802F94F0[] = "_AcSkl3";
const char str_802F94F8[] = "_AcSkl4";

// rodata vectors. The original emitted one symbol per element (0x802F9500..0x802F9560).
const Vec vec3_802f9500 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f950c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f9518 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f9524 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f9530 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f953c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f9548 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f9554 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f9560 = {0.0f, 0.0f, 0.0f};

// sdata2 float constants.
extern const f32 float_neg168_8041e5d8;
extern const f32 float_70_8041e5dc;
extern const f32 float_1_8041e5e0;
extern const f32 float_neg232_8041e5e4;
extern const f32 float_neg200_8041e5e8;
extern const f32 float_25_8041e5ec;
extern const f32 float_0p5_8041e5f0;
extern const f32 float_neg151_8041e5f4;
extern const f32 float_neg300_8041e5f8;
extern const f32 float_30_8041e5fc;
extern const f32 float_0_8041e600;

// sdata tables.
s32 ac_lstrckeep_ok_frame[7] = {0x50, 0x46, 0x3C, 0x28, 0x14, 0x0A, 0x05};
char* ac_lstrckeep_iconname[4] = {(char*)str_802F94E0, (char*)str_802F94E8, (char*)str_802F94F0,
                                   (char*)str_802F94F8};

static void actionCommandDisp(f32 x, f32 y, s32 type);


// `BattleActionCommandGetDifficulty` is declared `s8`, so a plain call makes MWCC emit `extsb r0, r3`
// before the `slwi` table index; the original callee is `lbz r3, 0x1cbd(r3); blr` (zero-extending) and
// the caller does `slwi r0, r3, 2` with no extension, i.e. the return is really `u8`. Calling through a
// function pointer (as ac_stick_keep_left does) suppresses the `extsb`.
s32 battleAcMain_LstRcKeep(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    LstRcKeepWork* dec = (LstRcKeepWork*)&ac->field_0x2BC[0];
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
            dec->field_0x04 = 0;
            memset(disp, 0, 0x2C);
            disp->iconX = float_neg300_8041e5f8;
            disp->iconY = float_0_8041e600;
            disp->counter = 0x14;
            ac->mAcState = 0x63;
            break;
        case 0x63:
            break;
        case 0x64:
            dec->field_0x00 = 0x78;
            ac->mAcState = 0x3E8;
            dec->field_0x08 = 0;
            dec->field_0x0A = 0;
            // fall through
        case 0x3E8:
            if ((ac->field_0x4 & 1) &&
                (ac->mAcUnit->mTokenFlags & 0x10)) {
                if (ac->field_0x4 & 2) {
                    ac->field_0x8--;
                    if (ac->field_0x8 >= 0) {
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
            if (dec->field_0x08 == 0) {
                if (BattlePadCheckTrigger(0x40000) != 0) {
                    dec->field_0x08 = 1;
                }
            }
            else {
                dec->field_0x08++;
                if ((s32)BattlePadCheckNow(0x40000) == 0) {
                    dec->field_0x08 = 0;
                }
            }
            if (dec->field_0x0A == 0) {
                if (BattlePadCheckTrigger(0x800000) != 0) {
                    dec->field_0x0A = 1;
                }
            }
            else {
                dec->field_0x0A++;
                if ((s32)BattlePadCheckNow(0x800000) == 0) {
                    dec->field_0x0A = 0;
                }
            }
            if ((dec->field_0x08 != 0 && dec->field_0x0A != 0) ||
                *(s16*)&ac->field_78[0x10] > 0 || autoCommand) {
                ac->mAcState = 0x3E9;
                dec->field_0x00 = *(s32*)&ac->field_34[4];
                dec->field_0x04 = 0;
                dec->field_0x08 = 0;
                dec->field_0x0A = 0;
            }
            else {
                dec->field_0x00--;
                if (dec->field_0x00 <= 0) {
                    ac->result = 0;
                    stateChanged = 1;
                    ac->mAcState = 0x3EB;
                }
            }
            break;
        case 0x3E9:
            if (*(s16*)&ac->field_78[0x10] > 0) {
                (*(s16*)&ac->field_78[0x10])--;
                ac->result = 2;
                stateChanged = 1;
                ac->mAcState = 0x3EB;
            }
            dec->field_0x00--;
            if (dec->field_0x08 > 0) {
                dec->field_0x08++;
            }
            else if ((s32)BattlePadCheckNow(0x40000) == 0) {
                dec->field_0x08 = 1;
            }
            if (dec->field_0x0A > 0) {
                dec->field_0x0A++;
            }
            else if ((s32)BattlePadCheckNow(0x800000) == 0) {
                dec->field_0x0A = 1;
            }
            if (!autoCommand && (dec->field_0x08 > 5 || dec->field_0x0A > 5)) {
                ac->result &= ~1;
                stateChanged = 1;
                ac->mAcState = 0x3EB;
                break;
            }
            cur = dec->field_0x00;
            if (cur < 0) {
                dec->field_0x04++;
                if (dec->field_0x04 >= 5) {
                    ac->result &= ~1;
                    stateChanged = 1;
                    ac->mAcState = 0x3EB;
                }
                else {
                    dec->field_0x00 = *(s32*)(&ac->field_34[4] + dec->field_0x04 * 4);
                }
            }
            else if ((dec->field_0x08 != 0 && dec->field_0x0A != 0) || autoCommand) {
                s32 flag = 0;
                if (dec->field_0x04 == 4) {
                    s32 remaining = *(s32*)&ac->field_34[0x14] - cur;
#ifdef MUST_MATCH
                    // Match-only: the header says `s8` but the callee returns a zero-extended 4-byte value.
                    if (remaining <= ac_lstrckeep_ok_frame[((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work)]) {
#else
                    if (remaining <= ac_lstrckeep_ok_frame[BattleActionCommandGetDifficulty(work)]) {
#endif
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
            s32 n = dec->field_0x04 - 1;
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
            dec->field_0x0C = 0x3C;
            ac->mAcState = 0x3EE;
            break;
        case 0x3EE:
            dec->field_0x0C--;
            if (dec->field_0x0C <= 0) {
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
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;
    LstRcKeepWork* dec = (LstRcKeepWork*)&ac->field_0x2BC[0];
    s32 state = ac->mAcState;

    switch (state) {
    case 0x63:
    case 0x64:
    case 0x3E8:
        *(s32*)&disp->field_0x24[0] = -1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_8041e5f8,
                                    float_30_8041e5fc);
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3E9:
        *(s32*)&disp->field_0x24[0] = dec->field_0x04 - 1;
        disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_8041e5f8,
                                    float_30_8041e5fc);
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
        if (disp->counter > 0) {
            disp->counter--;
        }
        break;
    case 0x3EA:
    case 0x3EB:
    case 0x3EC:
        actionCommandDisp(float_30_8041e5fc, float_0_8041e600, *(s32*)&disp->field_0x24[0]);
        break;
    case 0x3ED:
    case 0x3EE:
        if (disp->counter >= 0x28) {
            disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_8041e5fc,
                                        float_neg300_8041e5f8);
        }
        else {
            disp->iconX = float_30_8041e5fc;
        }
        actionCommandDisp(disp->iconX, disp->iconY, *(s32*)&disp->field_0x24[0]);
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


// The original emits `clrlwi r5, r0, 16` for the loop's icon id: the original iconDispGx reads a
// zero-extended u16 (see iconDispGxAlpha's `clrlwi r5, 16`), but this header declares its iconId `s16`,
// so a plain call emits `extsh`. The function-pointer cast (same workaround as battle_menu_disp) makes
// the call site zero-extend instead; it still compiles to a direct `bl iconDispGx`.
static void actionCommandDisp(f32 x, f32 y, s32 type) {
    s32 off;
    s32 i;

    camGetPtr(CAMERA_2D);

    if (type < 3) {
        Vec v = vec3_802f9500;
        v.x = float_neg168_8041e5d8 + x;
        v.y = float_70_8041e5dc + y;
        iconDispGx(&v, 0x10, 0x71, float_1_8041e5e0);
    }
    else {
        Vec v = vec3_802f950c;
        v.x = float_neg168_8041e5d8 + x;
        v.y = float_70_8041e5dc + y;
        iconDispGx(&v, 0x10, 0x70, float_1_8041e5e0);
    }

    if (type < 3) {
        Vec v = vec3_802f9518;
        v.x = float_neg232_8041e5e4 + x;
        v.y = float_70_8041e5dc + y;
        iconDispGx(&v, 0x10, 0x78, float_1_8041e5e0);
    }
    else {
        Vec v = vec3_802f9524;
        v.x = float_neg232_8041e5e4 + x;
        v.y = float_70_8041e5dc + y;
        iconDispGx(&v, 0x10, 0x79, float_1_8041e5e0);
    }

    {
        Vec v = vec3_802f9530;
        v.x = float_neg200_8041e5e8 + x;
        y = float_25_8041e5ec + y;
        v.y = y;
        iconDispGx(&v, 0x10, 0x8E, float_1_8041e5e0);
    }

    for (i = 0, off = 0; i < 3; i++, off += 0x21) {
        if (i <= type) {
            Vec v = vec3_802f953c;
            v.x = x + (f32)(off - 0x102);
            v.y = y;
#ifdef MUST_MATCH
            // iconDispGx's header says s16 but the original passes a zero-extended u16 (clrlwi, no extsh).
            ((void (*)(Vec*, s16, u16, f32))iconDispGx)(&v, 0x10, (u16)(i + 0x92), float_1_8041e5e0);
#else
            iconDispGx(&v, 0x10, (s16)(i + 0x92), float_1_8041e5e0);
#endif
        }
        else {
            Vec v = vec3_802f9548;
            v.x = x + (f32)(off - 0x102);
            v.y = y;
            iconDispGx(&v, 0x10, 0x91, float_0p5_8041e5f0);
        }
    }

    if (type < 3) {
        Vec v = vec3_802f9554;
        v.x = float_neg151_8041e5f4 + x;
        v.y = y;
        iconDispGx(&v, 0x10, 0x91, float_1_8041e5e0);
    }
    else {
        Vec v = vec3_802f9560;
        v.x = float_neg151_8041e5f4 + x;
        v.y = y;
        iconDispGx(&v, 0x10, 0x95, float_1_8041e5e0);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_neg168_8041e5d8 = -168.0f;
const f32 float_70_8041e5dc = 70.0f;
const f32 float_1_8041e5e0 = 1.0f;
const f32 float_neg232_8041e5e4 = -232.0f;
const f32 float_neg200_8041e5e8 = -200.0f;
const f32 float_25_8041e5ec = 25.0f;
const f32 float_0p5_8041e5f0 = 0.5f;
const f32 float_neg151_8041e5f4 = -151.0f;
const f32 float_neg300_8041e5f8 = -300.0f;
const f32 float_30_8041e5fc = 30.0f;
const f32 float_0_8041e600 = 0.0f;
