#include "eff/eff_mahorn.h"
#include "memory.h"

EffectEntry* effMahornEntry(BOOL unused, s32 count, f32 posX, f32 posY, f32 posZ, f32 scale, s32 unk) {
    EffectEntry* effect;
    effMahornData* data;
    s32 i;
    s32 newCount;

    effect = effEntry();
    effect->type = "Mahorn";
    newCount = count + 1;
    effect->count = newCount;
    data = __memAlloc(HEAP_EFFECT, newCount * sizeof(effMahornData));
    effect->userdata = data;
    effect->callback = effMahornMain;
    data->unused = unused;
    data->counter = 0;
    if (unk <= 0) {
        data->timer = 1000;
    }
    else {
        data->timer = unk;
    }
    data->position.x = posX;
    data->position.y = posY;
    data->position.z = posZ;
    data->scale = scale;
    data->alpha = 0xff;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->field_0x14 = 0.0f;
        data->alpha = 0xff;
        data->field_0x24 = i * 4;
    }
    return effect;
}
