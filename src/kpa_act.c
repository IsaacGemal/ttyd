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

// The whole instruction sequence matches; only one register swap still differs: the original keeps
// the loop index in r3 and the hoisted `ksdp` base in r6, this build puts the base in r3 and the
// index in r6 (plus the two dependent digits). Digit extraction is an `s8 digits[5]` array filled
// by a `for (j = 0; j != 5; j++)` loop the compiler fully unrolls (that form is what makes the
// magic-constant registers r9/r8/r7 and the saved digits match); `str`/`base` are read before the
// digit block so the base lands in a single register for the string walk.
// NONMATCHING (98.47%)
void kpaAddScorePos(s32 score, Vec* pos) {
    s32 len;
    s32 j;
    s8 digits[5];
    s32 n;
    char* str;
    char* base;
    s32 i;

    if (score > 10000) {
        score = 10000;
    }
    pouchAddKpaScore(score);
    n = score;
    for (i = 0; i < 50; i++) {
        if (ksdp[i].timer != 0) {
            continue;
        }
        str = ksdp[i].text;
        base = str;
        digits[0] = n / 10000;
        n %= 10000;
        digits[1] = n / 1000;
        n %= 1000;
        digits[2] = n / 100;
        n %= 100;
        digits[3] = n / 10;
        digits[4] = n % 10;
        len = 0;
        for (j = 0; j != 5; j++) {
            if (len != 0 || digits[j] != 0) {
                *str++ = digits[j] + '0';
                len++;
            }
        }
        base[len] = 0;
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

// Same shape as kpaAddScorePos (shared body): the digit/string sequence matches; only the
// loop-index / hoisted-`ksdp` base register swap remains (target: index in r3, base in r6).
// NONMATCHING (99.73%)
void kpaAddCoinPos(s32 coins, Vec* pos) {
    s32 len;
    s32 j;
    s8 digits[5];
    s32 n;
    char* str;
    char* base;
    s32 i;

    if (pouchAddKpaCoin(coins)) {
        pouchAddKpaScore(1000);
    }
    coins *= 500;
    if (coins > 10000) {
        coins = 10000;
    }
    pouchAddKpaScore(coins);
    n = coins;
    for (i = 0; i < 50; i++) {
        if (ksdp[i].timer != 0) {
            continue;
        }
        str = ksdp[i].text;
        base = str;
        digits[0] = n / 10000;
        n %= 10000;
        digits[1] = n / 1000;
        n %= 1000;
        digits[2] = n / 100;
        n %= 100;
        digits[3] = n / 10;
        digits[4] = n % 10;
        len = 0;
        for (j = 0; j != 5; j++) {
            if (len != 0 || digits[j] != 0) {
                *str++ = digits[j] + '0';
                len++;
            }
        }
        base[len] = 0;
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

// The vec3 addressing is forced to `.rodata` (lis/addi) with __declspec to match the target;
// the `u32 i` loops are what let the compiler fully unroll the copies and clear without a
// pre-loop guard (same shape as evt_twoddisp_init).
USER_FUNC(kpa_score_disp_init) {
    EventEntry* entry;
    u32 i;

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
    for (i = 0; (s32)i < 50; i++) {
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
