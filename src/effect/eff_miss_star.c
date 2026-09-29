#include "eff/eff_miss_star.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "system.h"
#include <dolphin/mtx.h>

typedef struct MissStarData {
    Vec position;   // 0x0
    f32 vx;         // 0xC
    f32 vy;         // 0x10
    f32 vz;         // 0x14
    f32 dvx;        // 0x18
    f32 dvy;        // 0x1C
    f32 dvz;        // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    f32 angle;      // 0x2C
    f32 angleVel;   // 0x30
    f32 field_0x34;  // 0x34
    f32 scale;      // 0x38
    s32 field_0x3C; // 0x3C
    f32 field_0x40; // 0x40
} MissStarData;

const Vec vec3_802f3678 = { 0.0f, 0.0f, 0.0f };

// .sdata2
extern const f32 float_0p8_8041cb74;
extern const f32 float_0p5_8041cb78;

void effMissStarMain(EffectEntry* effect) {
    MissStarData* data;
    Vec position;
    Vec disp;
    f32 angleVelScale;
    s32 i;

    data = effect->userdata;

    disp = vec3_802f3678;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;
    angleVelScale = float_0p8_8041cb74;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x28 != 0) {
            data->field_0x28--;
            if (data->field_0x28 == 0) {
                break;
            }
        }

        if (data->field_0x24 >= 0) {
            data->position.x += data->dvx;
            data->position.y += data->dvy;
            data->position.z += data->dvz;
        }

        data->field_0x34 = -camGetPtr(CAMERA_3D)->viewYaw;
        data->angle = data->angle + data->angleVel;
        data->angle = reviseAngle(data->angle);
        data->angleVel *= angleVelScale;

        if (data->field_0x24 < 10) {
            data->vx *= float_0p5_8041cb78;
            data->vy *= float_0p5_8041cb78;
            data->vz *= float_0p5_8041cb78;
            data->dvx = data->vx;
            data->dvy = data->vy;
            data->dvz = data->vz;
        }

        data->field_0x24--;
        if (data->field_0x24 < 0) {
            data->field_0x3C--;
            if (data->field_0x3C < 0) {
                effDelete(effect);
                return;
            }
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effMissStarDisp, effect);
}
