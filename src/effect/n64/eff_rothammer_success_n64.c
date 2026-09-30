#include "eff/n64/eff_rothammer_success_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <math.h>

// .sdata2 scratch GXColor (only its bytes are copied into the TEV register).
extern const u32 dat_8041fc00;
extern const f32 float_deg2rad_8041fc04;

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

EffectEntry* effRotHammerSuccessN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, s32 a6) {
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
    data->field_0x24 = a6;
    data->field_0x28 = 0;
    data->field_0x2c = a6;
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
        data->field_0x20 = 20.0f;
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
    data->field_0x28++;
    data->field_0x24--;
    if (data->field_0x24 < 0) {
        effDelete(effect);
        return;
    }

    life = data->field_0x24;
    max = data->field_0x2c;
    life2 = life * 2;
    for (i = 1, data++; i < effect->count; i++, data++) {
        if (life > 5) {
            data->velocity.x *= 0.8f;
            data->velocity.y *= 0.8f;
            data->velocity.z *= 0.8f;
        }
        if (life < 5) {
            data->field_0x20 *= 0.8f;
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
        data->angle += data->field_0x20;
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
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    effGetTexObjN64(0x1D, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

    PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041fc04);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);

    color = *(GXColor*)&dat_8041fc00;
    GXSetTevColor(1, color);

    for (i = 1, data++; i < effect->count; i++, data++) {
        PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041fc04 * data->angle);
        PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
