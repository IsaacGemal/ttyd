#include "eff/n64/eff_magic1_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>

// Local copy of the effect's per-instance data; the fields used by
// effMagic1Disp, effMagic1Main and effMagic1N64Entry are named.
extern const char str_802FAB08[];
extern const f32 float_0_8041f654;
extern const f32 float_1_8041f68c;
extern const f32 float_29_8041f6a4;
extern const f32 float_6p2_8041f6a8;
const f64 double_to_int_802fab00 = 4503601774854144.0;

EffectEntry* effMagic1N64Entry(s32 type, s32 count, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    EffectEntry* effect;
    effMagic1Data* data;
    f32 scale;

    effect = effEntry();
    effect->type = str_802FAB08;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x38);
    effect->userdata = data;
    effect->callback = effMagic1Main;
    effect->flags |= 2;
    data->field_0x2c = 0;
    data->type = type;
    data->field_0x34 = 0xff;
    data->x = x0;
    data->y = y0;
    data->z = z0;
    data->field_0x28 = float_1_8041f68c;
    switch (type) {
    case 0:
        scale = float_1_8041f68c / (f32)count;
        data->field_0x10 = float_0_8041f654;
        data->field_0x14 = float_0_8041f654;
        data->field_0x18 = float_0_8041f654;
        data->field_0x1c = (x1 - x0) * scale;
        data->field_0x20 = (y1 - y0) * scale;
        data->field_0x24 = (z1 - z0) * scale;
        data->field_0x30 = count;
        break;
    default:
        data->field_0x10 = float_29_8041f6a4;
        data->field_0x14 = float_0_8041f654;
        data->field_0x18 = float_0_8041f654;
        data->field_0x1c = float_6p2_8041f6a8;
        data->field_0x20 = float_0_8041f654;
        data->field_0x24 = float_0_8041f654;
        data->field_0x30 = 0x18;
        break;
    }
    return effect;
}

static void main_dl(EffectEntry* effect, Mtx mtx);

void effMagic1Disp(CameraId camId, EffectEntry* effect) {
    effMagic1Data* data;
    CameraEntry* camera;
    s32 type;
    s32 i;
    Mtx mtxA;
    Mtx mtxB;
    Mtx mtxC;
    Mtx mtxD;
    f32 angle;

    camera = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    switch (type) {
    case 0:
        angle = 0.0f;
        break;
    default:
        angle = -camGetPtr(CAMERA_3D)->viewYaw;
        break;
    }
    PSMTXTrans(mtxA, data->x, data->y, data->z);
    PSMTXRotRad(mtxB, 'y', 0.017453292f * angle);
    PSMTXConcat(mtxA, mtxB, mtxC);
    PSMTXConcat(camera->view, mtxC, mtxC);
    switch (type) {
    case 0:
        main_dl(effect, mtxC);
        break;
    case 1:
        for (i = 0; i < 12; i++) {
            PSMTXRotRad(mtxD, 'z', 0.017453292f * (f32)(i * 30));
            PSMTXConcat(mtxC, mtxD, mtxD);
            main_dl(effect, mtxD);
        }
        break;
    }
}
