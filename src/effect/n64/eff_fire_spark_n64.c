#include "eff/n64/eff_fire_spark_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

typedef struct effFireSparkData {
    s32 type;             // 0x00
    f32 field_0x04;       // 0x04
    f32 field_0x08;       // 0x08
    f32 field_0x0C;       // 0x0C
    f32 field_0x10;       // 0x10
    f32 field_0x14;       // 0x14
    s32 life;             // 0x18
    s32 field_0x1C;       // 0x1C
    s32 field_0x20;       // 0x20
    s32 field_0x24;       // 0x24
    s32 field_0x28;       // 0x28
    s32 field_0x2C;       // 0x2C
    s32 field_0x30;       // 0x30
    s32 field_0x34;       // 0x34
    s32 field_0x38;       // 0x38
    s32 field_0x3C;       // 0x3C
    f32 field_0x40;       // 0x40
    u8 field_0x44[0x20];  // 0x44
    s32 field_0x64;       // 0x64
    f32 field_0x68;       // 0x68
    f32 field_0x6C;       // 0x6C
} effFireSparkData;

// .sdata2 and .rodata constants named after their original addresses (defined by the split map).
extern const f32 float_30_8041eec0;
extern const f32 float_0p5_8041eeac;
extern const char str_802FA5C8[];

EffectEntry* effFireSparkN64Entry(s32 type, s32 count, s32 life, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8, f32 a9, f32 a10, f32 a11) {
    EffectEntry* effect;
    effFireSparkData* data;
    effFireSparkData* ptr;
    f32 ratio;
    s32 i;

    effect = effEntry();
    effect->type = str_802FA5C8;
    effect->count = count;
    data = __memAlloc(HEAP_EFFECT, sizeof(effFireSparkData) * count);
    effect->userdata = data;
    effect->callback = effFireSparkMain;
    effect->flags |= 2;

    data->type = type;
    data->field_0x1C = 0;
    if (life <= 0) {
        data->life = 1000;
    } else {
        data->life = life;
    }
    data->field_0x2C = 0;
    data->field_0x04 = a4;
    data->field_0x08 = a5;
    data->field_0x0C = a6;
    data->field_0x40 = a9;
    data->field_0x10 = a7;
    data->field_0x14 = a8;
    data->field_0x68 = a10;
    data->field_0x6C = a11;
    data->field_0x20 = 0x7f;
    data->field_0x24 = 0;
    data->field_0x28 = 0;
    data->field_0x30 = 0x7f;
    data->field_0x34 = 0x7f;
    data->field_0x38 = 0;
    data->field_0x3C = 0xff;

    ratio = float_30_8041eec0 / (f32)(count - 1);
    ratio *= float_0p5_8041eeac;
    ptr = data + 1;
    for (i = 1; i < count; i++, ptr++) {
        ptr->field_0x64 = (s32)(-(f32)i * ratio) - 1;
    }

    return effect;
}
