#include "eff/n64/eff_kemuri7_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

u32 foot_vtx[14] = {
    0xFFFB0000, 0xFFFB0000, 0x04000000, 0x00FF0005,
    0x0000FFFB, 0x04000400, 0x000000FF, 0x00050000,
    0x00050400, 0x00000000, 0x00FFFFFB, 0x00000005,
    0x00000000, 0x000000FF,
};

// .data vertex array defined by this unit.

typedef struct effKemuri7Data {
    s32 active;                  // 0x0: cleared once the puff expires
    u8 pad_4[2];                 // 0x4
    s16 lifetime;                // 0x6: frames left
    u8 alpha;                    // 0x8: TEV register colour alpha
    u8 pad_9[3];                 // 0x9
    Vec position;                // 0xC
    f32 scaleX;                  // 0x18: initialised to 1
    f32 scaleY;                  // 0x1C: initialised to 1
    f32 scaleZ;                  // 0x20: initialised to 1
    f32 rotX;                    // 0x24: initialised to 0
    f32 rotY;                    // 0x28: yaw in degrees
    f32 rotZ;                    // 0x2C: initialised to 0
    Mtx matrix;                  // 0x30: world matrix built each frame
    f32 direction;               // 0x60: 0 mirrors the quad on X
    u8 pad_64[0x6C - 0x64];      // 0x64
    f32 field_0x6C;              // 0x6C
    f32 field_0x70;              // 0x70
    f32 field_0x74;              // 0x74
    u8 pad_78[0x80 - 0x78];      // 0x78
    f32 sinAngle;                // 0x80: sine of the spawn yaw
    f32 cosAngle;                // 0x84: cosine of the spawn yaw
} effKemuri7Data;

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effKemuri7N64Entry(f32 x, f32 y, f32 z, f32 angleDeg, f32 direction) {
    EffectEntry* effect;
    effKemuri7Data* data;
    f32 yaw;
    f32 baseAngle;
    f32 sideAngle;
    f32 baseRad;
    f32 offsetRad;
    s32 i;

    effect = effEntry();
    effect->type = "Kemuri7N64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x88);
    effect->userdata = data;
    effect->callback = effKemuri7Main;

    for (i = 0; i < 1; i++) {
        yaw = 30.0f * direction + angleDeg - 15.0f;
        data->active = 1;
        data->field_0x6C = 0.0f;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->scaleX = 1.0f;
        data->scaleY = 1.0f;
        data->scaleZ = 1.0f;
        data->alpha = 0xFF;
        data->direction = direction;
        data->field_0x70 = 0.0f;
        data->field_0x74 = 0.0f;
        data->lifetime = 0xFF;
        data->rotX = 0.0f;
        data->rotY = yaw;
        data->rotZ = 0.0f;

        sideAngle = 180.0f * direction;
        baseAngle = reviseAngle(angleDeg);
        baseRad = 6.2831855f * baseAngle / 360.0f;
        data->sinAngle = (f32)sin(baseRad);
        data->cosAngle = (f32)cos(baseRad);

        // Spawn 5 units out at 45 degrees to the facing direction, moved to the
        // opposite side when direction is set.
        offsetRad = 6.2831855f * (45.0f + baseAngle - sideAngle) / 360.0f;
        data->position.x += 5.0f * (f32)sin(offsetRad);
        data->position.z += 5.0f * (f32)cos(offsetRad);
        data->alpha = 0xFF;
    }
    return effect;
}

void effKemuri7Main(EffectEntry* effect) {
    effKemuri7Data* data = effect->userdata;
    Vec position;
    Mtx m1;
    Mtx m0;
    s32 i;
    s32 active;

    position = initializeVec(&data->position);
    active = 0;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->active == 0) {
            continue;
        }
        if (--data->lifetime <= 0) {
            data->active = 0;
            continue;
        }
        active = 1;
        PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(m1, 'y', 0.017453292f * data->rotY);
        PSMTXConcat(m0, m1, data->matrix);
        data->alpha--;
    }

    if (active == 0) {
        effDelete(effect);
    }
    else {
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri7Disp, effect);
    }
}

void effKemuri7Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effKemuri7Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx work;
    Mtx scale;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    effGetTexObjN64(0x4f, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64(foot_vtx);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->active == 0) {
            continue;
        }
        GXSetTevColor(1, (GXColor){ 0x70, 0x60, 0x18, data->alpha });
        PSMTXConcat(cam->view, data->matrix, work);
        if (!data->direction) {
            PSMTXScale(scale, -1.0f, 1.0f, 1.0f);
            PSMTXConcat(work, scale, work);
        }
        GXLoadPosMtxImm(work, 0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

