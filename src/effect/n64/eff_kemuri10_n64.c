#include "eff/n64/eff_kemuri10_n64.h"
#include "driver/dispdrv.h"

#include <dolphin/mtx.h>

typedef struct effKemuri10Data {
    s32 field_0x0;    // 0x0
    Vec position;     // 0x4
    f32 field_0x10;   // 0x10
    f32 field_0x14;   // 0x14
    f32 field_0x18;   // 0x18
    f32 field_0x1C;   // 0x1C
    f32 field_0x20;   // 0x20
    f32 field_0x24;   // 0x24
    f32 field_0x28;   // 0x28
    f32 field_0x2C;   // 0x2C
    f32 field_0x30;   // 0x30
    f32 field_0x34;   // 0x34
    f32 field_0x38;   // 0x38
    f32 field_0x3C;   // 0x3C
    f32 field_0x40;   // 0x40
    f32 field_0x44;   // 0x44
    f32 field_0x48;   // 0x48
    f32 field_0x4C;   // 0x4C
    f32 field_0x50;   // 0x50
    f32 field_0x54;   // 0x54
    s32 field_0x58;   // 0x58
    s32 field_0x5C;   // 0x5C
    s32 field_0x60;   // 0x60
    u8 field_0x64[8]; // 0x64
    s32 field_0x6C;   // 0x6C
} effKemuri10Data;

// .rodata vector the original copies before filling in the particle position.
const Vec vec3_802fa9c0 = { 0.0f, 0.0f, 0.0f };

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p3_8041f4bc;
extern const f32 float_0p8_8041f4c0;
extern const f32 float_0p6_8041f4c4;
extern const f32 float_0p7_8041f4c8;
extern const f32 float_1p05_8041f4cc;

// NONMATCHING (97.02%): only the prologue's load order differs. MWCC hoists the
// three vec3_802fa9c0 loads above the r30/r31 saves; the original issues the
// data-pointer load (lwz r4, 0xc(r3)) first, then the vec3 loads.
void effKemuri10Main(EffectEntry* effect) {
    effKemuri10Data* data;
    Vec position2;
    Vec position;
    s32 camera;
    s32 phase;
    s32 i;

    data = effect->userdata;
    position = vec3_802fa9c0;
    position.x = data->position.x;
    position.y = data->position.y;
    position.z = data->position.z;
    position2 = position;
    camera = data->field_0x6C;
    data->field_0x5C--;
    data->field_0x60++;
    phase = data->field_0x5C;
    if (phase < 0) {
        effDelete(effect);
        return;
    }

    for (i = 0; i < effect->count; i++, data++) {
        if (phase > 5) {
            data->field_0x58 = (s32)(float_0p3_8041f4bc * (f32)(255 - data->field_0x58) + (f32)data->field_0x58);
            data->field_0x10 *= float_0p8_8041f4c0;
            data->field_0x14 *= float_0p8_8041f4c0;
            data->field_0x18 *= float_0p8_8041f4c0;
        }
        if (phase < 5) {
            data->field_0x58 = (s32)((f32)data->field_0x58 * float_0p6_8041f4c4);
            data->field_0x10 *= float_0p7_8041f4c8;
            data->field_0x14 *= float_0p7_8041f4c8;
            data->field_0x18 *= float_0p7_8041f4c8;
        }
        data->position.x += data->field_0x10;
        data->position.y += data->field_0x14;
        data->position.z += data->field_0x18;
        data->field_0x4C += data->field_0x50;
        data->field_0x54 *= float_1p05_8041f4cc;
    }

    dispEntry(camera, dispCalcZ(&position2), 2, effKemuri10Disp, effect);
}
