#include "eff/n64/eff_star_power_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const f32 float_0_8041fe2c;

const char str_802FB730[] = "StarPowerN64";

EffectEntry* effStarPowerN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    EffStarPowerData* data;

    effect = effEntry();
    effect->type = str_802FB730;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffStarPowerData));
    effect->userdata = data;
    effect->callback = effStarPowerMain;
    effect->flags |= 2;
    data->field_0x0 = 1;
    data->type = type;
    data->field_0x14 = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x38 = scale;
    data->color1R = 0xFF;
    data->color1G = 0xFF;
    data->color1B = 0x78;
    data->color2R = 0xFF;
    data->color2G = 0x78;
    data->field_0x30 = 0;
    data->color2A = 0xFF;
    data->color1A = 0xFF;
    data->field_0x3C = float_0_8041fe2c;
    data->field_0x40 = float_0_8041fe2c;
    data->field_0x44 = float_0_8041fe2c;
    return effect;
}
