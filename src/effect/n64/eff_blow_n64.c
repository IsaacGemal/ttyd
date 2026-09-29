#include "eff/n64/eff_blow_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 word constant of this unit (volatile: forces the per-element reload the original does).
const volatile u32 dat_8041e980 = 0xC8FFFF00;

// .data: retail 16x16 quad vertex array.
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008,
	0xFFF80000, 0x04000000, 0xFFFFFFFF, 0x00080008,
	0x00000400, 0x0400FFFF, 0xFFFFFFF8, 0x00080000,
	0x00000400, 0xFFFFFFFF,
};

static inline void blowSetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041e980;
    color.a = a;
    GXSetTevColor(1, color);
}

void effBlowDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effBlowData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    Mtx base;
    s32 i;
    f32 deg2rad;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xA, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    effGetTexObjN64(0x34, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

    PSMTXTrans(base, data->x, data->y, data->z);
    deg2rad = 0.017453292f;
    for (i = 1, data++; i < effect->count; i++, data++) {
        blowSetColor(data->field_0x58);

        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'y', deg2rad * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXConcat(trans, rot, work);
        PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleX);
        PSMTXConcat(work, scale, work);
        PSMTXRotRad(rot, 'z', deg2rad * data->rotZ);
        PSMTXConcat(work, rot, work);
        PSMTXConcat(base, work, work);
        PSMTXConcat(cam->view, work, work);

        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
