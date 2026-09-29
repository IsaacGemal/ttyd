#include "eff/n64/eff_kemuri12_n64.h"
#include "driver/dispdrv.h"
#include "system.h"

#include <dolphin/mtx.h>

// .sdata2 float constant (named after its original address).
const f32 float_8_8041f564 = 8.0f;

// .rodata constants (named after their original addresses).
const Vec vec3_802faa28 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802faa38 = 4503601774854144.0;

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
