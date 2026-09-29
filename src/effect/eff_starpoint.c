#include "eff/eff_starpoint.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include <dolphin/mtx.h>

typedef struct StarPointData {
    s32 type;       // 0x00
    s32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    Vec position;   // 0x0C
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
} StarPointData;

extern const f32 float_deg2rad_804209c8;

void effStarPointDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    StarPointData* data = effect->userdata;
    StarPointData* elem;
    Mtx mtx;
    Mtx rot;
    Mtx elemMtx;
    s32 i;

    PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', float_deg2rad_804209c8 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(mtx, rot, mtx);

    data++;
    for (i = 1; i < effect->count; i++) {
        if (data->field_0x2C != 0) {
            PSMTXTrans(elemMtx, data->position.x, data->position.y, data->position.z);
            PSMTXConcat(mtx, elemMtx, elemMtx);
            iconDispGx2(elemMtx, 0, 0x18C);
        }
        data++;
    }
}
