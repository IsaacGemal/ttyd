#include "eff/eff_break.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/types.h>

typedef struct BreakEffect {
    s32 field_0x00; // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    s32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    s32 field_0x18; // 0x18
    s32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
} BreakEffect;

const char str_804210C4[] = "Break";
extern const f32 float_0_804210b0;
extern const f32 float_2_804210cc;
extern const f32 float_10_804210d0;

EffectEntry* effBreakEntry(s32 type, s32 a2, f32 x, f32 y, f32 z) {
    EffectEntry* entry;
    BreakEffect* data;
    BreakEffect* elem;
    s32 count;
    s32 i;

    entry = effEntry();
#ifdef MUST_MATCH
    // The original emits a redundant language test that stores 7 on both paths.
    count = 7;
    if ((u32)gp->language != 0) {
        count = 7;
    }
#else
    count = 7;
#endif
    entry->type = str_804210C4;
    entry->count = count;
    data = __memAlloc(HEAP_EFFECT, count * sizeof(BreakEffect));
    entry->userdata = data;
    entry->callback = effBreakMain;
    data->field_0x00 = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x10 = a2;
    data->field_0x20 = float_2_804210cc;

    for (i = 1; i < entry->count; i++) {
        elem = &data[i];
        // Layout mirror of the original: (i-1)*0x24 + 0x12 - ((count-2)*0x24 + 0x18) / 2.
        elem->x = float_2_804210cc * (f32)((i - 1) * 0x24 + 0x12 - ((entry->count - 2) * 0x24 + 0x18) / 2);
        elem->y = float_0_804210b0;
        elem->z = float_0_804210b0;
        elem->field_0x14 = float_10_804210d0;
        elem->field_0x18 = 0;
        elem->field_0x10 = (i - 1) * 0x15;
        elem->field_0x1C = 0;
    }
    return entry;
}
