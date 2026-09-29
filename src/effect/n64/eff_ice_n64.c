#include "eff/n64/eff_ice_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct effIceData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
} effIceData;

static void effIceMain(EffectEntry* effect);

EffectEntry* effIceN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effIceData* data;
    effIceData* ptr;
    s32 i;

    effect = effEntry();
    effect->type = "IceN64";
    effect->count = 9;
    data = __memAlloc(HEAP_EFFECT, sizeof(effIceData) * 9);
    effect->userdata = data;
    effect->callback = effIceMain;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x28 = 30;
    data->field_0x2C = 0;
    data->field_0x24 = 0;
    ptr = data + 1;
    for (i = 1; i < 9; i++, ptr++) {
        ptr->x = (f32)(rand() % 10 - 5);
        ptr->y = (f32)(rand() % 10 + 5);
        ptr->z = 10.0f;
        ptr->field_0x18 = (f32)(rand() % 360);
        ptr->field_0x20 = (f32)(rand() % 360);
        ptr->field_0x1C = (f32)(rand() % 100 / 10);
        ptr->field_0x10 = 0.0f;
        ptr->field_0x14 = 2.0f;
    }
    return effect;
}
