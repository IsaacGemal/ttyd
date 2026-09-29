#include "eff/n64/eff_cool_leaf_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>

extern const f32 float_0_8041ec60;
extern const f32 float_0p1_8041ecb0;
extern const f32 float_neg3_8041ecb4;
extern const f32 float_3_8041ecb8;
extern const f32 float_2_8041ec70;
extern const f32 float_neg6_8041ecbc;
extern const f32 float_6_8041ecc0;
extern const f32 float_0p008_8041ecc4;
extern const f32 float_0p05_8041ec88;

const f64 double_to_int_802fa470 = 4503601774854144.0;
const char str_802FA478[] = "CoolLeafN64";

typedef struct effCoolLeafData {
    s32 type;        // 0x00
    f32 x;           // 0x04
    f32 y;           // 0x08
    f32 z;           // 0x0C
    f32 field_0x10;  // 0x10
    f32 field_0x14;  // 0x14
    f32 field_0x18;  // 0x18
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    s32 field_0x24;  // 0x24
    s32 field_0x28;  // 0x28
    s32 field_0x2C;  // 0x2C
    f32 field_0x30;  // 0x30
    f32 field_0x34;  // 0x34
    f32 field_0x38;  // 0x38
} effCoolLeafData;

EffectEntry* effCoolLeafN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effCoolLeafData* data;
    effCoolLeafData* ptr;
    s32 i;
    s32 rnd01;
    s32 rnd11;
    s32 rnd361;
    s32 rnd101;

    effect = effEntry();
    effect->type = str_802FA478;
    effect->count = 5;
    data = __memAlloc(HEAP_EFFECT, sizeof(effCoolLeafData) * 5);
    effect->userdata = data;
    effect->callback = effCoolLeafMain;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x28 = 100;
    data->field_0x2C = 0;
    data->field_0x24 = 0;
    ptr = data + 1;
    for (i = 1; i < 5; i++, ptr++) {
        rnd01 = rand() % 51;
        rnd11 = rand() % 21;
        rnd361 = rand() % 361;
        rnd101 = rand() % 101;
        ptr->x = (f32)(rnd01 + 75);
        ptr->y = (f32)(rnd11 + 70);
        ptr->z = float_0_8041ec60;
        ptr->field_0x18 = (f32)rnd361;
        ptr->field_0x20 = (f32)(360 - rnd361);
        switch (type) {
            case 0:
                ptr->field_0x1C = float_0p1_8041ecb0 * (f32)rnd101;
                ptr->field_0x10 = float_0_8041ec60;
                ptr->field_0x14 = float_0_8041ec60;
                ptr->field_0x30 = float_neg3_8041ecb4;
                ptr->field_0x34 = float_0_8041ec60;
                ptr->field_0x38 = float_3_8041ecb8;
                break;
            case 1:
                ptr->field_0x1C = float_0p1_8041ecb0 * (f32)rnd101 * float_2_8041ec70;
                ptr->field_0x10 = float_0_8041ec60;
                ptr->field_0x14 = float_0_8041ec60;
                ptr->field_0x30 = float_neg6_8041ecbc;
                ptr->field_0x34 = float_0_8041ec60;
                ptr->field_0x38 = float_6_8041ecc0;
                break;
            case 2:
                ptr->field_0x1C = float_0p1_8041ecb0 * (f32)rnd101 * float_2_8041ec70;
                ptr->field_0x10 = -ptr->x * (float_0p008_8041ecc4 * (f32)(rand() % 11) + float_0p05_8041ec88);
                ptr->field_0x14 = -ptr->y * (float_0p008_8041ecc4 * (f32)(rand() % 11) + float_0p05_8041ec88);
                break;
            default:
                break;
        }
    }
    return effect;
}
