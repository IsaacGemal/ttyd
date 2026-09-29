#include "eff/n64/eff_kemuri9_n64.h"
#include "memory.h"
#include <math.h>

// .sdata2 float constants (named after their original addresses in the asm).
extern const f32 float_0_8041f488;
extern const f32 float_10_8041f4a4;
extern const f32 float_6p2832_8041f4a8;
extern const f32 float_360_8041f4ac;

// .rodata constants (named after their original addresses).
const f64 double_to_int_802fa9a8 = 4503601774854144.0;
const char str_802FA9B0[] = "Kemuri9N64";

typedef struct effKemuri9Particle {
    u8 pad_00[4];                                // 0x00
    f32 x;                                       // 0x04
    f32 y;                                       // 0x08
    f32 z;                                       // 0x0C
    f32 field_0x10;                              // 0x10
    f32 field_0x14;                              // 0x14
    f32 field_0x18;                              // 0x18
    u8 pad_1C[0x40 - 0x1C];                      // 0x1C
} effKemuri9Particle;

typedef struct effKemuri9Data {
    u16 type;      // 0x00
    u8 pad_02[2];  // 0x02
    f32 x;         // 0x04
    f32 y;         // 0x08
    f32 z;         // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 scale;      // 0x1C
    f32 targetScale; // 0x20
    s32 life;       // 0x24
    s32 age;        // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    s32 field_0x34; // 0x34
    s32 field_0x38; // 0x38
    s32 field_0x3C; // 0x3C
} effKemuri9Data;

EffectEntry* effKemuri9N64Entry(s32 type, s32 count, s32 a2, f32 x, f32 y, f32 z, f32 a6, f32 a7) {
    EffectEntry* effect;
    effKemuri9Data* data;
    effKemuri9Particle* p;
    s32 i;
    f32 angle;

    effect = effEntry();
    effect->type = str_802FA9B0;
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x40);
    effect->userdata = data;
    effect->callback = effKemuri9Main;

    data->life = a2;
    data->age = 0;
    data->field_0x2C = a2;
    data->field_0x30 = 0;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = float_10_8041f4a4;
    data->targetScale = a6;
    data->field_0x30 = 0;
    data->field_0x34 = 0;
    data->field_0x3C = a2;
    data->field_0x38 = 0;

    p = (effKemuri9Particle*)(data + 1);
    for (i = 1; i < count + 1; i++, p++) {
        p->x = float_0_8041f488;
        p->y = float_0_8041f488;
        p->z = float_0_8041f488;
        angle = float_6p2832_8041f4a8 * (a7 + (f32)(((i - 1) * 360) / count)) / float_360_8041f4ac;
        p->field_0x10 = sin(angle);
        p->field_0x14 = float_0_8041f488;
        p->field_0x18 = cos(angle);
    }
    return effect;
}
