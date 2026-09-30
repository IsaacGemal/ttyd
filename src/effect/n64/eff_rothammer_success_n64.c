#include "eff/n64/eff_rothammer_success_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <math.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effRotHammerSuccessN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, s32 a6) {
    EffectEntry* effect;
    effRotHammerSuccessData* data;
    f32 angle;
    s32 i;

    effect = effEntry();
    effect->type = "RotHammerSuccessN64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * sizeof(effRotHammerSuccessData));
    effect->userdata = data;
    effect->callback = effRotHammerSuccessMain;
    data->field_0x24 = a6;
    data->field_0x28 = 0;
    data->field_0x2c = a6;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    for (i = 1, data++; i < count + 1; i++, data++) {
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        angle = 6.2831855f * (f32)((i - 1) * 360 / count) / 360.0f;
        data->velocity.x = 8.0f * (f32)sin(angle);
        data->velocity.y = 8.0f * (f32)cos(angle);
        data->velocity.z = 0.0f;
        data->angle = 0.0f;
        data->field_0x20 = 20.0f;
        data->scale = 1.0f;
    }
    return effect;
}

void effRotHammerSuccessMain(EffectEntry* effect) {
    effRotHammerSuccessData* data = effect->userdata;
    Vec position2;
    Vec position = initializeVec(&data->position);
    s32 life;
    s32 life2;
    s32 i;
    s32 max;

    position2 = position;
    data->field_0x28++;
    data->field_0x24--;
    if (data->field_0x24 < 0) {
        effDelete(effect);
        return;
    }

    life = data->field_0x24;
    max = data->field_0x2c;
    life2 = life * 2;
    for (i = 1, data++; i < effect->count; i++, data++) {
        if (life > 5) {
            data->velocity.x *= 0.8f;
            data->velocity.y *= 0.8f;
            data->velocity.z *= 0.8f;
        }
        if (life < 5) {
            data->field_0x20 *= 0.8f;
            data->velocity.x *= 0.7f;
            data->velocity.y *= 0.7f;
            data->velocity.z *= 0.7f;
        }
        if (life2 < max) {
            data->scale += 0.2f * -data->scale;
        }
        data->position.x += data->velocity.x;
        data->position.y += data->velocity.y;
        data->position.z = 0.0f;
        data->angle += data->field_0x20;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effRotHammerSuccessDisp, effect);
}
