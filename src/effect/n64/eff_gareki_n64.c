#include "eff/n64/eff_gareki_n64.h"
#include "driver/dispdrv.h"
#include "system.h"
#include <dolphin/mtx.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_neg0p1_8041f054;
extern const f32 float_0p8_8041f058;
extern const f32 float_neg0p2_8041f05c;
extern const f32 float_0p9_8041f060;
extern const f32 float_10_8041f064;
extern const f32 float_20_8041f068;

typedef struct GarekiChild {
    f32 x;          // 0x00
    f32 y;          // 0x04
    f32 z;          // 0x08
    u8 pad_0C[4];   // 0x0C
    f32 vx;         // 0x10
    f32 vy;         // 0x14
    f32 offsetX;    // 0x18
    f32 offsetY;    // 0x1C
    u8 pad_20[0xC]; // 0x20
    f32 angleX;     // 0x2C
    u8 pad_30[4];   // 0x30
    f32 angleZ;     // 0x34
    u8 pad_38[8];   // 0x38
} GarekiChild; // 0x40

typedef struct GarekiData {
    Vec position;            // 0x00
    f32 groundY;             // 0x0C
    u8 pad_10[4];            // 0x10
    f32 vy;                  // 0x14
    u8 pad_18[8];            // 0x18
    s32 timer;               // 0x20
    s32 state;               // 0x24
    s32 alpha;               // 0x28
    f32 angleX;              // 0x2C
    f32 angleZ;              // 0x30
    f32 field_34;            // 0x34
    f32 angleVelX;           // 0x38
    f32 angleVelZ;           // 0x3C
    GarekiChild children[];  // 0x40
} GarekiData;

// Local copy of system.c's inline helper so it inlines here.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

void effGarekiMain(EffectEntry* effect) {
    GarekiData* data = effect->userdata;
    Vec position;
    s32 i;

    position = initializeVec(&data->position);

    if (data->state == 0) {
        data->vy += float_neg0p1_8041f054;
        data->position.y += data->vy;
        data->angleX += data->angleVelX;
        data->angleZ += data->angleVelZ;
        if (data->position.y <= data->groundY) {
            data->state = 1;
        }
    }

    if (data->state == 1) {
        data->timer--;
        if (data->timer < 0) {
            effDelete(effect);
            return;
        }
        if (data->timer < 10) {
            data->alpha = (s32)((f32)data->alpha * float_0p8_8041f058);
        }
        data++;
        for (i = 1; i < effect->count; i++, data++) {
            ((GarekiChild*)data)->vy += float_neg0p2_8041f05c;
            ((GarekiChild*)data)->vx *= float_0p9_8041f060;
            ((GarekiChild*)data)->x += ((GarekiChild*)data)->vx;
            ((GarekiChild*)data)->y += ((GarekiChild*)data)->vy;
            ((GarekiChild*)data)->angleX += float_10_8041f064;
            ((GarekiChild*)data)->angleZ += float_20_8041f068;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGarekiDisp, effect);
}

