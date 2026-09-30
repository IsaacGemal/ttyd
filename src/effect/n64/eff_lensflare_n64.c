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
const Vec vec3_802faac8 = { 0.0f, 0.0f, 0.0f };
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

// The original walks the header fields and the entry array through ONE pointer: it loads userdata into
// `data`, uses it as a LensflareData for the header, then steps it once (`for (i = 1, data++; ...)`) into
// the entries. Keeping a separate `entry` variable makes MWCC spill userdata to a caller-saved register
// (r6) and materialise the loop pointer with `addi r31, r6, 0x34`, which flips the later register roles.
void effLensflareMain(EffectEntry* effect) {
    LensflareEntry* data = (LensflareEntry*)effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the relaxed Vec in a scratch local and copies it to a
       second local before dispCalcZ; a single local folds the copy away. */
    Vec position;
#endif
    Vec disp = vec3_802faac8;
    f32 angle;
    s32 i;
#ifdef MUST_MATCH
    /* MUST_MATCH: reading field_18 and field_1C once each keeps them in r0 and r3 as the original does;
       reading the fields directly in each test reloads them and swaps the two registers. */
    s32 fade;
    s32 frame;
#endif

    disp.x = ((LensflareData*)data)->x;
    disp.y = ((LensflareData*)data)->y;
    disp.z = ((LensflareData*)data)->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        ((LensflareData*)data)->field_18 = 30;
    }
    if (((LensflareData*)data)->field_18 < 100) {
        ((LensflareData*)data)->field_18--;
    }
    ((LensflareData*)data)->field_1C++;
#ifdef MUST_MATCH
    fade = ((LensflareData*)data)->field_18;
    if (fade < 0) {
        effDelete(effect);
        return;
    }
    frame = ((LensflareData*)data)->field_1C;
    if (frame <= 20) {
        ((LensflareData*)data)->field_14 = frame * 12;
    } else if (fade < 20) {
        ((LensflareData*)data)->field_14 = fade * 12;
    }
#else
    if (((LensflareData*)data)->field_18 < 0) {
        effDelete(effect);
        return;
    }
    if (((LensflareData*)data)->field_1C <= 20) {
        ((LensflareData*)data)->field_14 = ((LensflareData*)data)->field_1C * 12;
    } else if (((LensflareData*)data)->field_18 < 20) {
        ((LensflareData*)data)->field_14 = ((LensflareData*)data)->field_18 * 12;
    }
#endif

    for (i = 1, data++; i < effect->count; i++, data++) {
        angle = float_6p2832_8041f62c * (float_90_8041f630 * (data->field_2C - float_70_8041f634) / float_10_8041f638) / float_360_8041f63c;
        // field_14 is inside LensflareEntry's padding; the overlapping LensflareData layout names it.
        ((LensflareData*)data)->field_14 = (s32)(float_255_8041f640 * (f32)cos(angle));
        data->field_2C += data->field_30;
        if (data->field_2C > float_80_8041f644) {
            data->field_2C = float_60_8041f614;
            data->field_28 = float_0p1_8041f628 * (f32)(rand() % 16) + float_0p4_8041f648;
#ifdef MUST_MATCH
            /* MUST_MATCH: the natural extern form below makes MWCC CSE 0.1f across the rand() call into
               a callee-saved f24, which grows the frame by 0x10. The literal rematerialises instead. */
            data->field_30 = 0.1f * (f32)(rand() % 11) + 0.1f;
#else
            data->field_30 = float_0p1_8041f628 * (f32)(rand() % 11) + float_0p1_8041f628;
#endif
            data->field_20 = (f32)(rand() % 360);
        }
    }
#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effLensflareDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 2, effLensflareDisp, effect);
#endif
}
