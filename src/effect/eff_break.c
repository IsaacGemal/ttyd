#include "eff/eff_break.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_stardust_n64.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>
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
extern const f32 float_20_804210c0;
extern const Vec vec3_802fe610;
extern const f32 ty_data[21];
extern const f32 ty_data2[21];

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

// NONMATCHING (99.94%): the constant multiply is encoded as fmuls f31, f1, f0; the original emits fmuls f31, f0, f1.
// Same registers (f0 = float_20_804210c0, f1 = data->field_0x20), only the commutative operand order differs. Stating the
// constant first makes the same instructions but shuffles the float file (f0<->f1), which costs 9 more instructions.
void effBreakMain(EffectEntry* effect) {
    BreakEffect* data = effect->userdata;
    Vec position;
    Vec disp;
    f32 scale;
    s32 max;
    s32 i;
    s32 done;

    disp = vec3_802fe610;
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
    position = disp;
    scale = data->field_0x20 * float_20_804210c0;
    max = data->field_0x10;

    done = 0;
    data = &data[1];
    for (i = 1; i < effect->count; i++, data++) {
        switch (data->field_0x1C) {
        case 0:
            data->field_0x10--;
            if (data->field_0x10 <= 0) {
                data->field_0x10 = 0;
                data->field_0x1C++;
            }
            break;
        case 1:
            if ((u32)data->field_0x10 < 0x15) {
                data->field_0x14 = ty_data[data->field_0x10];
                data->field_0x10++;
                if (float_0_804210b0 == data->field_0x14) {
                    effStardustN64SetDrawCam(
                        effStardustN64Entry(1, position.x + data->x, position.y + data->y, position.z + data->z, scale), 8);
                }
            } else {
                data->field_0x10 = (effect->count - 1 - i) * 0x15;
                data->field_0x1C++;
            }
            break;
        case 2:
            data->field_0x10--;
            if (data->field_0x10 <= 0) {
                data->field_0x10 = 0;
                data->field_0x1C++;
            }
            break;
        case 3:
            data->field_0x10++;
            if (data->field_0x10 > max) {
                data->field_0x10 = 0;
                data->field_0x1C++;
            }
            break;
        case 4:
            if ((u32)data->field_0x10 < 0x15) {
                data->field_0x14 = ty_data2[data->field_0x10];
                if ((i & 1) == 0) {
                    data->field_0x14 = -data->field_0x14;
                }
                data->field_0x10++;
            } else {
                data->field_0x1C++;
            }
            break;
        case 5:
            done++;
            break;
        }
    }

    if (done >= effect->count - 1) {
        effDelete(effect);
    } else {
        dispEntry(8, dispCalcZ(&position), 2, effBreakDisp, effect);
    }
}
