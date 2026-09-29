#include "eff/n64/eff_akari_pose_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_deg2rad_8041e8d0;
extern const f32 float_0p03125_8041e8d4;
extern const f32 float_0_8041e8d8;

// .sbss2 scratch GXColor the original builds its TEV colour from.
extern GXColor unk_80423098;

// .data vertex array, local to this file (0x38 bytes).
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x04000000, 0x000000FF,
    0x00080008, 0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000, 0x00000400, 0x000000FF,
};

typedef struct effAkariPosePart {
    u8 pad_0x00[0x4];  // 0x00
    f32 x;             // 0x04
    f32 y;             // 0x08
    u8 pad_0x0C[0x14]; // 0x0C
    s32 field_0x20;    // 0x20
    f32 field_0x24;    // 0x24
    u8 pad_0x28[0x8];  // 0x28
    s32 field_0x30;    // 0x30
    f32 field_0x34;    // 0x34
} effAkariPosePart;

typedef struct effAkariPoseData {
    u8 pad_0x00[0x4];        // 0x00
    f32 x;                   // 0x04
    f32 y;                   // 0x08
    f32 z;                   // 0x0C
    u8 pad_0x10[0x20];       // 0x10
    s32 field_0x30;          // 0x30
    f32 field_0x34;          // 0x34
    effAkariPosePart part[]; // 0x38
} effAkariPoseData;

// (1) The original emits `li r31,1` (i) before `addi r30,r30,0x38` (part = data->part); mwcc here
//     always schedules the pointer advance first.
// (2) GXSetTevColor's GXColor is built at r1+0x8 and word-copied to the outgoing arg at r1+0xc;
//     here the local lands at r1+0xc and is copied to r1+0x8 (same known quirk as eff_booboo_n64.c,
//     eff_batten.c, eff_irekaeru.c).
// NONMATCHING (99.22%): only those two scheduler/allocation artifacts differ.
void effAkariPoseDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effAkariPoseData* data;
    effAkariPosePart* part;
    CameraEntry* cam;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx viewMtx;
    s32 alpha;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x30;
    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e8d0);
    PSMTXScale(scaleMtx, data->field_0x34, data->field_0x34, data->field_0x34);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(mtx, scaleMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevOp(0, 3);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scaleMtx, float_0p03125_8041e8d4, float_0p03125_8041e8d4, float_0_8041e8d8);
    GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
    effGetTexObjN64(0x76, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
    for (i = 1, part = data->part; i < effect->count; i++, part++) {
        if (part->field_0x20 < 0) {
            continue;
        }
        PSMTXTrans(mtx, part->x, part->y, float_0_8041e8d8);
        PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e8d0 * part->field_0x24);
        PSMTXScale(scaleMtx, part->field_0x34, part->field_0x34, part->field_0x34);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
        color = unk_80423098;
        color.a = (alpha * part->field_0x30) >> 8;
        GXSetTevColor(1, color);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
