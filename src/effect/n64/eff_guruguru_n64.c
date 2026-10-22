#include "eff/n64/eff_guruguru_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// .sdata2 constants belonging to this unit, referenced by name in the original asm.

// .data: retail 16x16 quad vertex array used by effGuruguruDisp.
static u32 size16x16_tex32x32_vtx[14] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x00400000, 0x000000FF,
    0x00080008, 0x00000040, 0x00200000, 0x00FFFFF8, 0x00080000, 0x00000020, 0x000000FF,
};

// .rodata: scratch vector the original zeroes before filling in the effect position.

// Inline copy of system.c's helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effGuruguruN64Entry(s32 type, s32 numParticles, f32 x, f32 y, f32 z, f32 radius) {
    EffectEntry* effect;
    EffGuruguruData* data;
    s32 i;

    effect = effEntry();
    effect->type = "GuruguruN64";
    effect->count = numParticles + 1;
    data = __memAlloc(HEAP_EFFECT, (numParticles + 1) * sizeof(EffGuruguruData));
    effect->userdata = data;
    effect->callback = effGuruguruMain;

    data[0].type = type;
    data[0].position.x = x;
    data[0].position.y = y;
    data[0].position.z = z;
    data[0].radius = 0.0f;
    data[0].yaw = 0;
    data[0].targetRadius = radius;
    data[0].active = 1;
    data[0].drawScale = 1.0f;

    for (i = 1; i < numParticles + 1; i++) {
        s32 angle = ((i - 1) * 360) / numParticles;

        data[i].position.x = 0.0f;
        data[i].position.y = 0.0f;
        data[i].position.z = 0.0f;
        data[i].angle = angle;
        data[i].angleSpeed = 20.0f;
        data[i].pitch = 0.0f;
        data[i].yaw = angle;
    }

    return effect;
}

void effGuruguruMain(EffectEntry* effect) {
    EffGuruguruData* data;
    Vec pos;
    f32 radius;
    s32 i;

    data = effect->userdata;
    pos = initializeVec(&data->position);

    if (data->active != 0) {
        data->yaw++;
        data->radius += 0.1f * (data->targetRadius - data->radius);
        radius = data->radius;

        data++;
        for (i = 1; i < effect->count; i++, data++) {
            data->position.x = radius * (f32)sin(6.2831855f * (f32)data->yaw / 360.0f);
            data->position.z = radius * (f32)cos(6.2831855f * (f32)data->yaw / 360.0f) *
                               (f32)cos(6.2831855f * data->pitch / 360.0f);
            data->position.y = radius * (f32)cos(6.2831855f * (f32)data->yaw / 360.0f) *
                               (f32)sin(6.2831855f * data->pitch / 360.0f);
            data->angle += data->angleSpeed;
            data->pitch = 0.0f;
            data->yaw += 16;
            if (data->yaw > 360) {
                data->yaw -= 360;
            }
        }

        dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effGuruguruDisp, effect);
    }
}

void effGuruguruDisp(CameraId camId, void* param) {
    EffectEntry* effect;
    EffGuruguruData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx0;
    Mtx mtx1;
    Mtx scaleMtx;
    Mtx mtx2;
    f32 deg2rad;
    s32 i;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;

    if (data->active != 0) {
        GXSetNumChans(0);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        effGetTexObjN64(0x1d, &texObj);
        GXLoadTexObj(&texObj, GX_TEXMAP0);
        GXSetNumTevStages(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        GXSetCullMode(GX_CULL_NONE);
        effSetVtxDescN64(size16x16_tex32x32_vtx);

        PSMTXTrans(mtx0, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(mtx1, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXScale(scaleMtx, data->drawScale, data->drawScale, data->drawScale);
        PSMTXConcat(mtx0, mtx1, mtx2);
        PSMTXConcat(mtx2, scaleMtx, mtx2);
        PSMTXConcat(cam->view, mtx2, mtx2);

        color = (GXColor){ 0xDC, 0xDC, 0x28, 0xFF };
        GXSetTevColor(GX_TEVREG0, color);

        deg2rad = 0.017453292f;
        i = 1;
        data++;
        for (; i < effect->count; i++, data++) {
            PSMTXTrans(mtx0, data->position.x, data->position.y, data->position.z);
            PSMTXRotRad(mtx1, 'z', deg2rad * data->angle);
            PSMTXConcat(mtx0, mtx1, mtx0);
            PSMTXConcat(mtx2, mtx0, mtx0);
            GXLoadPosMtxImm(mtx0, GX_PNMTX0);
            GXSetCurrentMtx(GX_PNMTX0);
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
        }
    }
}
