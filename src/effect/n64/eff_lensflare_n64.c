#include "eff/n64/eff_lensflare_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>

// .sdata2 constants live in this translation unit
// (see config/G8MJ01/symbols.txt).
extern const f32 float_60_8041f614;
extern const f32 float_0p1_8041f628;
extern const f32 float_6p2832_8041f62c;
extern const f32 float_90_8041f630;
extern const f32 float_70_8041f634;
extern const f32 float_10_8041f638;
extern const f32 float_360_8041f63c;
extern const f32 float_255_8041f640;
extern const f32 float_80_8041f644;
extern const f32 float_0p4_8041f648;

// .rodata
const char str_802FAAE0[16] = "LensflareN64";

EffectEntry* effLensflareN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2) {
    EffectEntry* effect;
    LensflareData* data;
    LensflareEntry* entry;
    s32 i;

    effect = effEntry();
    effect->type = str_802FAAE0;
    effect->count = 6;
    data = __memAlloc(HEAP_EFFECT, sizeof(LensflareData));
    effect->userdata = data;
    effect->callback = effLensflareMain;
    effect->flags |= 2;
    data->field_4 = type;
    data->field_1C = 0;
    if (a1 <= 0) {
        data->field_18 = 100;
    } else {
        data->field_18 = a1;
    }
    data->field_14 = 0;
    data->x = x;
    data->y = y;
    data->z = z;

    entry = data->entries;
    for (i = 1; i < 6; i++) {
        entry->field_20 = (f32)(rand() % 360);
        entry->field_2C = (f32)((i * 2) % 20 + 60);
        entry->field_28 = float_0p1_8041f628 * (f32)(rand() % 16) + float_0p4_8041f648;
        entry->field_30 = float_0p1_8041f628 * (f32)(rand() % 11) + float_0p1_8041f628;
        entry++;
    }
    return effect;
}

// The logic and every instruction are exact; only MWCC's register allocation differs. The original
// coalesces `data` and the loop `entry` into r31 (data is live in r6 here and the loop pointer is
// materialised with `addi r31, r6, 0x34`), so field_18 is tested in r3 instead of r0 and the loop-setup
// `li r30, 1` / `addi r31, ...` order flips. No source form (single variable, casts, scoping, if/else,
// for-init, const/register/volatile) or -O/-opt pragma combination tried made MWCC coalesce the two live
// ranges. The 0.1f literal on field_30 is required to stop MWCC CSE-ing the extern float_0p1_8041f628
// across the intervening rand() call into a callee-saved f24, which would grow the frame by 0x10 and
// shift every save slot.
// NONMATCHING (98.88%): only `data`'s register (r6 vs r31) and the dependent loop-setup order differ.
void effLensflareMain(EffectEntry* effect) {
    LensflareData* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the relaxed Vec in a scratch local and copies it to a
       second local before dispCalcZ; a single local folds the copy away. */
    Vec position;
#endif
    Vec disp = { 0.0f, 0.0f, 0.0f };
    LensflareEntry* entry;
    f32 angle;
    s32 i;

    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_18 = 30;
    }
    if (data->field_18 < 100) {
        data->field_18--;
    }
    data->field_1C++;
    if (data->field_18 < 0) {
        effDelete(effect);
        return;
    }
    if (data->field_1C <= 20) {
        data->field_14 = data->field_1C * 12;
    } else if (data->field_18 < 20) {
        data->field_14 = data->field_18 * 12;
    }

    i = 1;
    entry = &data->entries[i - 1];
    for (; i < effect->count; i++, entry++) {
        angle = float_6p2832_8041f62c * (float_90_8041f630 * (entry->field_2C - float_70_8041f634) / float_10_8041f638) / float_360_8041f63c;
        // field_14 is inside the padded region of LensflareEntry (no named field).
        *(s32*)((u8*)entry + 0x14) = (s32)(float_255_8041f640 * (f32)cos(angle));
        entry->field_2C += entry->field_30;
        if (entry->field_2C > float_80_8041f644) {
            entry->field_2C = float_60_8041f614;
            entry->field_28 = float_0p1_8041f628 * (f32)(rand() % 16) + float_0p4_8041f648;
#ifdef MUST_MATCH
            /* MUST_MATCH: the natural extern form below makes MWCC CSE 0.1f across the rand() call into
               a callee-saved f24, which grows the frame by 0x10. The literal rematerialises instead. */
            entry->field_30 = 0.1f * (f32)(rand() % 11) + 0.1f;
#else
            entry->field_30 = float_0p1_8041f628 * (f32)(rand() % 11) + float_0p1_8041f628;
#endif
            entry->field_20 = (f32)(rand() % 360);
        }
    }
#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effLensflareDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 2, effLensflareDisp, effect);
#endif
}
