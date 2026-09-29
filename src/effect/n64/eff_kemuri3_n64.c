#include "eff/n64/eff_kemuri3_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <math_ext.h>

const Vec vec3_802fa8d8 = { 0.0f, 0.0f, 0.0f };
const char str_802FA8E4[] = "Kemuri3N64";

extern const f32 float_0_8041f2f0;
extern const f32 float_1_8041f2f4;
extern const f32 float_0p3_8041f30c;
extern const f32 float_1p75_8041f310;
extern const f32 float_neg0p08_8041f314;
extern const f32 float_neg3p9_8041f318;
extern const f32 float_6p2832_8041f31c;
extern const f32 float_360_8041f320;

typedef struct effKemuri3Data {
    s32 type;       // 0x0
    u16 unk_04;     // 0x4
    s16 field_06;   // 0x6
    u8 field_08;    // 0x8
    u8 pad_09[3];   // 0x9
    Vec position;   // 0xC
    f32 field_18;   // 0x18
    f32 field_1C;   // 0x1C
    f32 field_20;   // 0x20
    f32 field_24;   // 0x24
    f32 field_28;   // 0x28
    f32 field_2C;   // 0x2C
    Mtx field_30;   // 0x30
    f32 field_60;   // 0x60
    f32 field_64;   // 0x64
    f32 field_68;   // 0x68
    f32 field_6C;   // 0x6C
    f32 field_70;   // 0x70
    f32 field_74;   // 0x74
    f32 field_78;   // 0x78
    f32 field_7C;   // 0x7C
    f32 field_80;   // 0x80
    f32 field_84;   // 0x84
} effKemuri3Data;

EffectEntry* effKemuri3N64Entry(f32 x, f32 y, f32 z, f32 a4) {
    EffectEntry* effect;
    effKemuri3Data* data;
    s32 i;
    s32 angle;
    f32 rad;

    effect = effEntry();
    effect->type = str_802FA8E4;
    effect->count = 5;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri3Data) * 5);
    effect->userdata = data;
    effect->callback = effKemuri3Main;

    for (i = 0; i < 5; i++, data++) {
        data->type = 1;
        data->field_64 = float_0p3_8041f30c;
        data->field_68 = float_0_8041f2f0;
        data->field_6C = float_0_8041f2f0;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_18 = float_1_8041f2f4;
        data->field_1C = float_1_8041f2f4;
        data->field_20 = float_1_8041f2f4;
        data->field_08 = 0xff;
        data->field_06 = 0x3c;
        data->field_24 = float_0_8041f2f0;
        data->field_28 = a4 + (f32)(i * 72);
        data->field_2C = float_0_8041f2f0;
        data->field_70 = float_1p75_8041f310;
        data->field_74 = float_neg0p08_8041f314;
        data->field_78 = float_0_8041f2f0;
        data->field_7C = float_0_8041f2f0;
        data->field_60 = float_neg3p9_8041f318;
        rad = float_6p2832_8041f31c * reviseAngle(data->field_28) / float_360_8041f320;
        data->field_80 = sin(rad);
        data->field_84 = cos(rad);
    }

    return effect;
}
