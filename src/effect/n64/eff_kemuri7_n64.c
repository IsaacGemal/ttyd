#include "eff/n64/eff_kemuri7_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

// .sdata2 constant (defined by this unit in the original).
extern const f32 float_deg2rad_8041f3fc;

typedef struct effKemuri7Data {
    s32 field_0x0;               // 0x0
    u8 pad_4[2];                 // 0x4
    s16 field_0x6;               // 0x6
    u8 field_0x8;                // 0x8
    u8 pad_9[3];                 // 0x9
    Vec position;                // 0xC
    u8 pad_18[0x28 - 0x18];      // 0x18
    f32 field_0x28;              // 0x28
    u8 pad_2C[0x30 - 0x2C];      // 0x2C
    Mtx field_0x30;              // 0x30
    u8 pad_60[0x88 - 0x60];      // 0x60
} effKemuri7Data;

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

void effKemuri7Main(EffectEntry* effect) {
    effKemuri7Data* data = effect->userdata;
    Vec position;
    Mtx m1;
    Mtx m0;
    s32 i;
    s32 active;

    position = initializeVec(&data->position);
    active = 0;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        if (--data->field_0x6 <= 0) {
            data->field_0x0 = 0;
            continue;
        }
        active = 1;
        PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(m1, 'y', float_deg2rad_8041f3fc * data->field_0x28);
        PSMTXConcat(m0, m1, data->field_0x30);
        data->field_0x8--;
    }

    if (active == 0) {
        effDelete(effect);
    }
    else {
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri7Disp, effect);
    }
}
