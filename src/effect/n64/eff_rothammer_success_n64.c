#include "eff/n64/eff_rothammer_success_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <math.h>

// .sdata2 scratch GXColor (only its bytes are copied into the TEV register).

// .data vertex array for the textured quad (local to this file in the original).
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x00400000, 0x000000FF,
    0x00080008, 0x00000040, 0x00200000, 0x00FFFFF8, 0x00080000, 0x00000020, 0x000000FF,
};

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effRotHammerSuccessN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, s32 life) {
    EffectEntry* effect;
    effRotHammerSuccessData* data;
    f32 angle;
    s32 i;

    effect = effEntry();
    effect->type = "RotHammerSuccessN64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * sizeof(effRotHammerSuccessData));
    effect->userdata = data;
    effect->callback = effRotHammerSuccessMain;
    data->life = life;
    data->frame = 0;
    data->lifeMax = life;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    for (i = 1, data++; i < count + 1; i++, data++) {
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        angle = 6.2831855f * (f32)((i - 1) * 360 / count) / 360.0f;
        data->velocity.x = 8.0f * (f32)sin(angle);
        data->velocity.y = 8.0f * (f32)cos(angle);
        data->velocity.z = 0.0f;
        data->angle = 0.0f;
        data->rotSpeed = 20.0f;
        data->scale = 1.0f;
    }
    return effect;
}

void effRotHammerSuccessMain(EffectEntry* effect) {
    effRotHammerSuccessData* data = effect->userdata;
    Vec position2;
    Vec position = initializeVec(&data->position);
    s32 life;
    s32 life2;
    s32 i;
    s32 max;

    position2 = position;
    data->frame++;
    data->life--;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    life = data->life;
    max = data->lifeMax;
    life2 = life * 2;
    for (i = 1, data++; i < effect->count; i++, data++) {
        if (life > 5) {
            data->velocity.x *= 0.8f;
            data->velocity.y *= 0.8f;
            data->velocity.z *= 0.8f;
        }
        if (life < 5) {
            data->rotSpeed *= 0.8f;
            data->velocity.x *= 0.7f;
            data->velocity.y *= 0.7f;
            data->velocity.z *= 0.7f;
        }
        if (life2 < max) {
            data->scale += 0.2f * -data->scale;
        }
        data->position.x += data->velocity.x;
        data->position.y += data->velocity.y;
        data->position.z = 0.0f;
        data->angle += data->rotSpeed;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effRotHammerSuccessDisp, effect);
}

void effRotHammerSuccessDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effRotHammerSuccessData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx viewMtx;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
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

    PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);

    color = (GXColor){ 0xDC, 0xDC, 0x28, 0xE6 };
    GXSetTevColor(GX_TEVREG0, color);

    for (i = 1, data++; i < effect->count; i++, data++) {
        PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->angle);
        PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
