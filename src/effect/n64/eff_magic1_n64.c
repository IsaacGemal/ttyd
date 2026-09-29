#include "eff/n64/eff_magic1_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

// Local copy of the effect's per-instance data; only the fields read by
// effMagic1Disp are named. Offsets match effMagic1N64Entry's stores.
typedef struct effMagic1Data {
    s32 type; // 0x00
    f32 x;    // 0x04
    f32 y;    // 0x08
    f32 z;    // 0x0C
} effMagic1Data;

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
