#include "eff/eff_batfukidashi.h"
#include "memory.h"

extern const f32 float_0_80420478;
extern const f32 float_1_8042047c;

const char str_802FD4B0[] = "CoinFukidashi";

EffectEntry* effCoinFukidashiEntry(s32 type, s32 coinNum, s32 count, f32 x, f32 y, f32 z) {
    EffectEntry* entry;
    CoinFukidashiData* data;

    entry = effEntry();
    entry->type = str_802FD4B0;
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(CoinFukidashiData));
    entry->userdata = data;
    entry->callback = effCoinFukidashiMain;
    entry->flags |= 2;

    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x10 = float_0_80420478;
    data->scale = float_1_8042047c;
    data->field_0x14 = coinNum;

    if (count <= 0) {
        data->field_0x20 = 1000;
    } else {
        data->field_0x20 = count;
    }
    data->field_0x24 = 0;
    data->alpha = 0;

    return entry;
}
