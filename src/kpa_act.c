#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/dispdrv.h"
#include "manager/fontmgr.h"
#include "memory.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "kpa_act.h"

// Work entry for one floating KPA score/coin popup. Pool is allocated in kpa_score_disp_init.
typedef struct KpaScoreDispEntry {
    /* 0x00 */ char text[8];
    /* 0x08 */ u8 timer;
    /* 0x09 */ u8 pad_09[3];
    /* 0x0C */ Vec pos;
} KpaScoreDispEntry; // 0x18

// Compiler-pool constants owned by this translation unit (see config/G8MJ01/symbols.txt).
const GXColor dat_80420d80 = { 0xFF, 0xFF, 0xFF, 0xFF };
const f32 float_0p5_80420d84 = 0.5f;
const f32 float_10_80420d88 = 10.0f;
const f32 float_20_80420d8c = 20.0f;
const f32 float_0_80420d90 = 0.0f;
const f32 float_1_80420d94 = 1.0f;
const f32 float_300_80420d98 = 300.0f;

// .rodata constants.
__declspec(section ".rodata") Vec vec3_802fe200 = {0.0f, 0.0f, 0.0f};
const f64 double_to_int_mask_802fe210 = 4503599627370496.0;

// Pool of 50 popups, allocated lazily in kpa_score_disp_init.
KpaScoreDispEntry* ksdp;

// Runtime event that keeps the popup pool ticking. Lives in .sdata.
EVT_BEGIN(kpa_score_disp_evt)
LOOP(0)
CALL(kpa_score_disp_main)
WAIT_FRAMES(1)
END_LOOP
RETURN
EVT_END()

USER_FUNC(kpa_chg_pose) {
    marioChgPose((const char*)evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

// The remainder split (which register holds the running score, when each digit is sign-extended,
// and the order the /10000, /1000, /100, /10 magic constants are loaded) still differs. The original
// uses r31 for score, r9/r8/r7/r5/r3 for the five digits and r4 for the loop pointer.
// NONMATCHING (84.21%)
void kpaAddScorePos(s32 score, Vec* pos) {
    s32 i;
    s32 len;
    s8 ten_thousands;
    s8 thousands;
    s8 hundreds;
    s8 tens;
    s8 ones;
    char* str;

    if (score > 10000) {
        score = 10000;
    }
    pouchAddKpaScore(score);
    for (i = 0; i < 50; i++) {
        if (ksdp[i].timer != 0) {
            continue;
        }
        ten_thousands = score / 10000;
        score %= 10000;
        thousands = score / 1000;
        score %= 1000;
        hundreds = score / 100;
        score %= 100;
        tens = score / 10;
        ones = score % 10;
        str = ksdp[i].text;
        len = 0;
        if (len != 0 || ten_thousands != 0) {
            *str = ten_thousands + '0';
            len = 1;
            str++;
        }
        if (len != 0 || thousands != 0) {
            *str++ = thousands + '0';
            len++;
        }
        if (len != 0 || hundreds != 0) {
            *str++ = hundreds + '0';
            len++;
        }
        if (len != 0 || tens != 0) {
            *str++ = tens + '0';
            len++;
        }
        if (len != 0 || ones != 0) {
            *str = ones + '0';
            len++;
        }
        ksdp[i].text[len] = 0;
        ksdp[i].timer = 30;
        ksdp[i].pos.x = pos->x - float_0p5_80420d84 * (float_10_80420d88 * (f32)strlen(ksdp[i].text));
        ksdp[i].pos.y = pos->y + float_20_80420d8c;
        ksdp[i].pos.z = float_0_80420d90;
        break;
    }
}

void kpaAddScore(s32 score) {
    if (score > 10000) {
        score = 10000;
    }
    pouchAddKpaScore(score);
}

// NONMATCHING (85.90%)
void kpaAddCoinPos(s32 coins, Vec* pos) {
    s32 i;
    s32 len;
    s8 ten_thousands;
    s8 thousands;
    s8 hundreds;
    s8 tens;
    s8 ones;
    char* str;

    if (pouchAddKpaCoin(coins)) {
        pouchAddKpaScore(1000);
    }
    coins *= 500;
    if (coins > 10000) {
        coins = 10000;
    }
    pouchAddKpaScore(coins);
    for (i = 0; i < 50; i++) {
        if (ksdp[i].timer != 0) {
            continue;
        }
        ten_thousands = coins / 10000;
        coins %= 10000;
        thousands = coins / 1000;
        coins %= 1000;
        hundreds = coins / 100;
        coins %= 100;
        tens = coins / 10;
        ones = coins % 10;
        str = ksdp[i].text;
        len = 0;
        if (len != 0 || ten_thousands != 0) {
            *str = ten_thousands + '0';
            len = 1;
            str++;
        }
        if (len != 0 || thousands != 0) {
            *str++ = thousands + '0';
            len++;
        }
        if (len != 0 || hundreds != 0) {
            *str++ = hundreds + '0';
            len++;
        }
        if (len != 0 || tens != 0) {
            *str++ = tens + '0';
            len++;
        }
        if (len != 0 || ones != 0) {
            *str = ones + '0';
            len++;
        }
        ksdp[i].text[len] = 0;
        ksdp[i].timer = 30;
        ksdp[i].pos.x = pos->x - float_0p5_80420d84 * (float_10_80420d88 * (f32)strlen(ksdp[i].text));
        ksdp[i].pos.y = pos->y + float_20_80420d8c;
        ksdp[i].pos.z = float_0_80420d90;
        break;
    }
}

void kpaAddCoin(s32 coins) {
    s32 score;

    if (pouchAddKpaCoin(coins)) {
        pouchAddKpaScore(1000);
    }
    score = coins * 500;
    if (score > 10000) {
        score = 10000;
    }
    pouchAddKpaScore(score);
}

// The copy of unitId/ownerNPC/lwData/lfData/uwBase/ufBase, the `_mapAlloc(0x4B0)` and the
// 50-entry pool clear are all present and in order. What is left: the target hoists the
// `_mapAlloc` size load (`li r3, 0x4b0`) to just after `evtEntryType` and has no "at least one
// iteration" guard, while this build emits a guard before each unrolled copy. The vec3
// addressing is forced to `.rodata` (lis/addi) with __declspec to match the target.
// NONMATCHING (96.74%)
USER_FUNC(kpa_score_disp_init) {
    EventEntry* entry;
    s32 i;

    entry = evtEntryType(kpa_score_disp_evt, event->priority, 0, event->type);
    entry->unitId = event->unitId;
    entry->ownerNPC = event->ownerNPC;
    for (i = 0; i < 16; i++) {
        entry->lwData[i] = event->lwData[i];
    }
    for (i = 0; i < 3; i++) {
        entry->lfData[i] = event->lfData[i];
    }
    entry->uwBase = event->uwBase;
    entry->ufBase = event->ufBase;
    ksdp = (KpaScoreDispEntry*)_mapAlloc(0x4B0);
    for (i = 0; i < 50; i++) {
        ksdp[i].text[0] = 0;
        ksdp[i].timer = 0;
        ksdp[i].pos = vec3_802fe200;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_score_disp_main) {
    s32 i;

    for (i = 0; i < 50; i++) {
        if (ksdp[i].timer != 0) {
            ksdp[i].pos.y += float_1_80420d94;
            ksdp[i].timer--;
        }
    }
    dispEntry(4, float_0_80420d90, 2, kpaScoreDisp, NULL);
    return EVT_RETURN_DONE;
}

void kpaScoreDisp(CameraId cameraId, void* param) {
    Mtx trans;
    Mtx scale;
    GXColor color;
    s32 i;

    PSMTXScale(scale, float_0p5_80420d84, float_0p5_80420d84, float_0p5_80420d84);
    FontDrawStart();
    color = dat_80420d80;
    FontDrawColor(&color);
    for (i = 0; i < 50; i++) {
        if (ksdp[i].timer != 0) {
            PSMTXTrans(trans, ksdp[i].pos.x, ksdp[i].pos.y, float_300_80420d98);
            PSMTXConcat(trans, scale, trans);
            FontDrawStringMtx(trans, ksdp[i].text);
        }
    }
}
