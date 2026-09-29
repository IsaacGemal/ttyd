#include "eff/n64/eff_flower_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include <dolphin/mtx.h>

// .sdata2 constants. The first four are used by effFlowerDisp/effFlowerN64Entry;
// they are defined here so the compiler's literal pool lands at their original
// offsets (0x0-0xC), matching the original unit's .sdata2 layout.
const u32 dat_8041efc8 = 0x000000FF;
const f32 float_deg2rad_8041efcc = 0.017453292f;
const f32 float_0p0625_8041efd0 = 0.0625f;
const f32 float_0_8041efd4 = 0.0f;

const Vec vec3_802fa688 = { 0.0f, 0.0f, 0.0f };

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage_n64.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

void effFlowerMain(EffectEntry* effect) {
    effFlowerData* data;
    Vec position;
    s32 type;

    data = effect->userdata;
    position = initializeVec(&data->position);
    type = data->type;
    if (data->field_0x34 < 150) {
        data->field_0x10 -= 0.001f;
        data->position.x += data->field_0x10;
        data->position.z += data->field_0x18;
        data->field_0x24 += data->field_0x2C;
        if (type == 0) {
            if (data->field_0x24 < 10.0f) {
                data->field_0x24 = 10.0f;
                data->field_0x2C = -data->field_0x2C;
            }
            if (data->field_0x24 > 45.0f) {
                data->field_0x24 = 45.0f;
                data->field_0x2C = -data->field_0x2C;
            }
        }
        data->field_0x28 += data->field_0x30;
        if (type == 0) {
            data->field_0x14 = 1.4f;
        } else {
            data->field_0x14 = 1.0f;
        }
    } else {
        data->field_0x20 += 0.0007f;
        data->field_0x1C += data->field_0x20;
        data->field_0x14 += data->field_0x1C;
        if (data->field_0x14 > 1.0f) {
            data->field_0x14 = 1.0f;
        }
    }

    if (type == 1) {
        data->field_0x40 += data->field_0x3C;
        if (data->field_0x40 >= 20) {
            data->field_0x40 = 20;
            data->field_0x3C = -1;
        } else if (data->field_0x40 <= 0) {
            data->field_0x40 = 0;
            data->field_0x3C = 1;
        }
    }

    data->position.y += data->field_0x14;
    data->field_0x34--;
    data->field_0x38++;
    if (data->field_0x34 < 0) {
        effDelete(effect);
        return;
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFlowerDisp, effect);
}
