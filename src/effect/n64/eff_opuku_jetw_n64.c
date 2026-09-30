#include "eff/n64/eff_opuku_jetw_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effOpukuJetwN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect = effEntry();
    effOpukuJetwData* data;

    effect->type = "OpukuJetwN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x44);
    effect->userdata = data;
    effect->callback = effOpukuJetwMain;
    effect->flags |= 2;

    data->type = type;
    data->counter = 0;
    if (duration <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = duration;
    }

    data->field_0x24 = 0xFF;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = scale;
    data->field_0x18 = 0x46;
    data->field_0x1C = 0xB4;
    data->field_0x20 = 0xFF;
    data->field_0x28 = 0xFF;
    data->field_0x2C = 0xFF;
    data->field_0x30 = 0xFF;
    data->field_0x38 = 0.0f;
    data->field_0x3C = 1.0f;
    data->field_0x40 = 1.0f;

    return effect;
}

void effOpukuJetwMain(EffectEntry* effect) {
    effOpukuJetwData* data = effect->userdata;
    Vec position = initializeVec(&data->position);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->counter++;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        if (data->counter < 8) {
            data->field_0x24 = data->counter * 32 + 31;
        }
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOpukuJetwDisp, effect);
    }
}
