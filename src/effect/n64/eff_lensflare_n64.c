#include "eff/n64/eff_lensflare_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <stdlib.h>

// .sdata2 constants live in this translation unit
// (see config/G8MJ01/symbols.txt).
extern const f32 float_0p1_8041f628;
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
