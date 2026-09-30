#include "eff/n64/eff_small_star_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2: shared scratch GXColor, only its byte 0 is overwritten (see eff_magic2_n64.c).
GXColor unk_80423260;

// .sdata: byte-indexed color rotation table (36 bytes).
static u8 color_rotation_data[36] = {
    0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
    0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
    0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .data: one effVtxArray (0x38 bytes) for the textured quad.
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x00400000, 0x000000FF, 0x00080008,
    0x00000040, 0x00200000, 0x00FFFFF8, 0x00080000,
    0x00000020, 0x000000FF,
};

void effSmallStarDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    s32 idx1;
    s32 idx2;
    s32 idx3;
    effSmallStarN64Data* data;
    s32 i;
    CameraEntry* cam;
    s32 base;
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    GXColor color;

    cam = camGetPtr(camId);
    data = effect->userdata;
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, GX_PTIDENTITY);
    effGetTexObjN64(0x1D, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

    // The split multiply/modulo keeps base in r4; writing it as one expression moves it to r3.
    base = (data->field_0x2C - 1) * 3;
    base %= 36;
    idx1 = base;
    idx2 = base + 1;
    idx3 = base + 2;
    for (i = 0; i < effect->count; i++, data++) {
        color = unk_80423260;
        color.r = color_rotation_data[idx1 % 36];
        color.g = color_rotation_data[idx2 % 36];
        color.b = color_rotation_data[idx3 % 36];
        color.a = data->field_0x24;
        GXSetTevColor(GX_TEVREG0, color);

        PSMTXTrans(mtx1, data->x, data->y, data->z);
        PSMTXRotRad(mtx2, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
        PSMTXRotRad(mtx3, 'z', data->field_0x1C * 0.017453292f);
        PSMTXConcat(mtx1, mtx2, mtx1);
        PSMTXConcat(mtx1, mtx3, mtx1);
        PSMTXConcat(cam->view, mtx1, mtx1);
        GXLoadPosMtxImm(mtx1, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);

        idx1 += 3;
        idx2 += 3;
        idx3 += 3;
    }
}

void effSmallStarN64CamChg(EffectEntry* effect, s32 camId) {
    effSmallStarN64Data* data = effect->userdata;
    s32 i = 0;

    while (i < effect->count) {
        data->camera = camId;
        i++;
        data++;
    }
}
