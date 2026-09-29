#include "eff/n64/eff_dogabon_attack_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"

#include <stdlib.h>

typedef struct DogabonAttackData {
    s32 type;           // 0x0
    f32 x;              // 0x4
    f32 y;              // 0x8
    f32 z;              // 0xC
    s32 lifetime;       // 0x10
    s32 field_0x14;     // 0x14
    s32 field_0x18;     // 0x18
    s32 field_0x1C;     // 0x1C
    s32 field_0x20;     // 0x20
    s32 field_0x24;     // 0x24
    f32 scale;          // 0x28
    f32 field_0x2C[25]; // 0x2C
    f32 field_0x90[25]; // 0x90
    f32 field_0xF4[25]; // 0xF4
    f32 field_0x158[25]; // 0x158
    f32 field_0x1BC[25]; // 0x1BC
    f32 field_0x220[25]; // 0x220
    f32 field_0x284[25]; // 0x284
    f32 field_0x2E8[25]; // 0x2E8
    f32 field_0x34C[25]; // 0x34C
    f32 field_0x3B0[25]; // 0x3B0
    f32 field_0x414[25]; // 0x414
    f32 field_0x478[25]; // 0x478
    f32 field_0x4DC[25]; // 0x4DC
    f32 field_0x540[25]; // 0x540
    f32 field_0x5A4[25]; // 0x5A4
    f32 field_0x608[25]; // 0x608
    s32 field_0x66C[25]; // 0x66C
    s32 field_0x6D0[25]; // 0x6D0
    s32 field_0x734[25]; // 0x734
} DogabonAttackData;

static s8 y_data[28] = {
    -5, -5, -5, -4, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0,
    1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 0, 0, 0,
};

extern const f32 float_0_8041ecf4;
extern const f32 float_6p2832_8041ed0c;
extern const f32 float_360_8041ed10;
extern const f32 float_15_8041ed18;
extern const f32 float_120_8041ed28;
const char str_802FA4D0[20] = "DogabonAttackN64";

EffectEntry* effDogabonAttackN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    DogabonAttackData* data;
    s32 i;
    f32 s;
    f32 c;
    f32 angle;
    s32 r;

    effect = effEntry();
    effect->type = str_802FA4D0;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(DogabonAttackData));
    effect->userdata = data;
    effect->callback = effDogabonAttackMain;
    effect->flags |= 2;
    data->type = type;
    data->field_0x14 = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->field_0x24 = 0xFF;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;
    data->field_0x18 = 0x46;
    data->field_0x1C = 0xB4;
    data->field_0x20 = 0x78;

    for (i = 0; i < 25; i++) {
        angle = (f32)(i * 1080 / 25) * float_6p2832_8041ed0c / float_360_8041ed10;
        s = (f32)sin(angle);
        c = (f32)cos(angle);
        r = rand();
        data->field_0x478[i] = (f32)(r % 360 - 180);
        data->field_0x284[i] = float_15_8041ed18 * s;
        data->field_0x2E8[i] = (f32)(y_data[i] + 10);
        data->field_0x34C[i] = float_15_8041ed18 * c;
        data->field_0x158[i] = float_120_8041ed28 * s;
        data->field_0x1BC[i] = (f32)(y_data[i] * 8 + 40);
        data->field_0x220[i] = float_120_8041ed28 * c;
        data->field_0x5A4[i] = float_0_8041ecf4;
        data->field_0x66C[i] = i * 2 + 1;
        data->field_0x6D0[i] = 0;
        data->field_0x734[i] = 0;
        data->field_0x4DC[i] = float_0_8041ecf4;
    }

    return effect;
}
