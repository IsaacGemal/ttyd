#include "eff/n64/eff_guruguru_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <math.h>

// .rodata: scratch vector the original zeroes before filling in the effect position.
const Vec vec3_802fa720 = { 0.0f, 0.0f, 0.0f };

// Inline copy of system.c's helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effGuruguruN64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f) {
    EffectEntry* effect;
    EffGuruguruData* data;
    s32 i;

    effect = effEntry();
    effect->type = "GuruguruN64";
    effect->count = b + 1;
    data = __memAlloc(HEAP_EFFECT, (b + 1) * sizeof(EffGuruguruData));
    effect->userdata = data;
    effect->callback = effGuruguruMain;

    data[0].field_0x00 = a;
    data[0].field_0x04 = c;
    data[0].field_0x08 = d;
    data[0].field_0x0C = e;
    data[0].field_0x10 = 0.0f;
    data[0].field_0x24 = 0;
    data[0].field_0x14 = f;
    data[0].field_0x28 = 1;
    data[0].field_0x2C = 1.0f;

    for (i = 1; i < b + 1; i++) {
        s32 angle = ((i - 1) * 360) / b;

        data[i].field_0x04 = 0.0f;
        data[i].field_0x08 = 0.0f;
        data[i].field_0x0C = 0.0f;
        data[i].field_0x18 = angle;
        data[i].field_0x1C = 20.0f;
        data[i].field_0x20 = 0.0f;
        data[i].field_0x24 = angle;
    }

    return effect;
}

void effGuruguruMain(EffectEntry* effect) {
    EffGuruguruData* data;
    Vec pos;
    f32 scale;
    s32 i;

    data = effect->userdata;
    pos = initializeVec((Vec*)&data->field_0x04);

    if (data->field_0x28 != 0) {
        data->field_0x24++;
        data->field_0x10 += 0.1f * (data->field_0x14 - data->field_0x10);
        scale = data->field_0x10;

        data++;
        for (i = 1; i < effect->count; i++, data++) {
            data->field_0x04 = scale * (f32)sin(6.2831855f * (f32)data->field_0x24 / 360.0f);
            data->field_0x0C = scale * (f32)cos(6.2831855f * (f32)data->field_0x24 / 360.0f) *
                               (f32)cos(6.2831855f * data->field_0x20 / 360.0f);
            data->field_0x08 = scale * (f32)cos(6.2831855f * (f32)data->field_0x24 / 360.0f) *
                               (f32)sin(6.2831855f * data->field_0x20 / 360.0f);
            data->field_0x18 += data->field_0x1C;
            data->field_0x20 = 0.0f;
            data->field_0x24 += 16;
            if (data->field_0x24 > 360) {
                data->field_0x24 -= 360;
            }
        }

        dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effGuruguruDisp, effect);
    }
}
