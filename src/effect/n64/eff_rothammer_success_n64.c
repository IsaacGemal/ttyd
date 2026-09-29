#include "eff/n64/eff_rothammer_success_n64.h"
#include "driver/dispdrv.h"

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
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
