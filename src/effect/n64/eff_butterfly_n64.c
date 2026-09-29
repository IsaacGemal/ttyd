#include "eff/n64/eff_butterfly_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/mtx.h>

// .rodata
const char str_802FA3C8[] = "ButterflyN64";

// .sdata2
extern const f32 float_50_8041eb90;
extern const f32 float_30_8041eb94;

typedef struct effButterflyData {
    s32 type;       // 0x0
    s32 field_0x4;  // 0x4
    s32 field_0x8;  // 0x8
    Vec position;   // 0xC
    Vec field_0x18; // 0x18
    s32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    f32 field_0x34; // 0x34
    f32 field_0x38; // 0x38
} effButterflyData;

EffectEntry* effButterflyN64Entry(s32 a, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effButterflyData* data;

    effect = effEntry();
    effect->type = str_802FA3C8;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x48);
    effect->userdata = data;
    effect->callback = effButterflyMain;
    effect->flags |= 2;
    data->type = a;
    data->field_0x4 = 0x3E8;
    data->field_0x24 = 0xFF;
    data->position.x = x;
    data->field_0x18.x = x;
    data->position.y = y;
    data->field_0x18.y = y;
    data->position.z = z;
    data->field_0x18.z = z;
    data->field_0x8 = 0;
    data->field_0x2C = 0;
    data->field_0x30 = 0;
    data->field_0x34 = float_50_8041eb90;
    data->field_0x38 = float_30_8041eb94;
    return effect;
}
