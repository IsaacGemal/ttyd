#include "eff/n64/eff_kemuri12_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041f55c;
const f32 float_8_8041f564 = 8.0f;

// .rodata constants (named after their original addresses).
const Vec vec3_802faa28 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802faa38 = 4503601774854144.0;
const char str_802FAA40[] = "Kemuri12N64";

typedef struct effKemuri12Data {
    u16 type;        // 0x0
    u8 pad_2[2];     // 0x2
    Vec position;    // 0x4
    f32 field_0x10;  // 0x10
    s32 field_0x14;  // 0x14
    s32 field_0x18;  // 0x18
    s32 field_0x1c;  // 0x1c
    f32 field_0x20;  // 0x20
    u8 color[4];     // 0x24
} effKemuri12Data;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effKemuri12N64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    effKemuri12Data* data;

    effect = effEntry();
    effect->type = str_802FAA40;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri12Data));
    effect->userdata = data;
    effect->callback = effKemuri12Main;
    data->field_0x14 = life;
    data->field_0x18 = 0;
    data->field_0x1c = life;
    data->field_0x20 = float_0_8041f55c;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x10 = scale;

    // The colour stores are ordered to reproduce the original's store scheduling.
    switch (type) {
    case 0:
        data->color[2] = 0x00;
        data->color[1] = 0x00;
        data->color[0] = 0x00;
        data->color[3] = 0x82;
        break;
    case 1:
        data->color[0] = 0xff;
        data->color[2] = 0x00;
        data->color[1] = 0x00;
        data->color[3] = 0x82;
        break;
    default:
        data->color[0] = 0xe1;
        data->color[1] = 0xd7;
        data->color[2] = 0xff;
        data->color[3] = 0xb4;
        break;
    }
    return effect;
}

void effKemuri12Main(EffectEntry* effect) {
    EffectEntry* e;
    effKemuri12Data* data;
    Vec position;

    e = effect;
    data = e->userdata;
    position = initializeVec(&data->position);
    data->field_0x20 = float_8_8041f564 * (f32)data->field_0x18 / (f32)data->field_0x1c;
    data->field_0x18++;
    data->field_0x14--;
    if (data->field_0x14 < 0) {
        effDelete(e);
        return;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri12Disp, e);
}
