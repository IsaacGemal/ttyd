#include "eff/eff_miss_star.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct MissStarData {
    Vec position;   // 0x0
    f32 vx;         // 0xC
    f32 vy;         // 0x10
    f32 vz;         // 0x14
    f32 dvx;        // 0x18
    f32 dvy;        // 0x1C
    f32 dvz;        // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    f32 angle;      // 0x2C
    f32 angleVel;   // 0x30
    f32 field_0x34;  // 0x34
    f32 scale;      // 0x38
    s32 field_0x3C; // 0x3C
    f32 field_0x40; // 0x40
} MissStarData;


// .data: initial velocity vectors used to fan the stars out.
Vec spd_tbl[15] = {
    { 0.0f, 4.5f, 0.0f },  { 1.0f, 4.0f, 0.0f },  { 2.0f, 3.0f, 0.0f },
    { 3.0f, 2.0f, 0.0f },  { 3.5f, 1.0f, 0.0f },  { 4.0f, 0.0f, 0.0f },
    { 4.5f, 0.0f, 0.0f },  { 5.0f, 0.0f, 0.0f },  { 4.5f, 0.0f, 0.0f },
    { 4.0f, 0.0f, 0.0f },  { 3.5f, -1.0f, 0.0f }, { 3.0f, -2.0f, 0.0f },
    { 2.0f, -3.0f, 0.0f }, { 1.0f, -4.0f, 0.0f }, { 0.0f, -4.5f, 0.0f },
};

// .data: per-star scale multipliers, grouped three at a time.
Vec scale_tbl[15] = {
    { 1.0f, 1.0f, 1.0f },    { 0.8f, 0.8f, 0.8f },    { 0.9f, 0.9f, 0.9f },
    { 1.1f, 1.1f, 1.1f },    { 1.0f, 1.0f, 1.0f },    { 0.8f, 0.8f, 0.8f },
    { 0.9f, 0.9f, 0.9f },    { 1.1f, 1.1f, 1.1f },    { 1.0f, 1.0f, 1.0f },
    { 0.8f, 0.8f, 0.8f },    { 0.9f, 0.9f, 0.9f },    { 1.1f, 1.1f, 1.1f },
    { 1.0f, 1.0f, 1.0f },    { 0.8f, 0.8f, 0.8f },    { 0.9f, 0.9f, 0.9f },
};

// .sdata2

EffectEntry* effMissStarEntry(s32 type, f32 x, f32 y, f32 z, s32 lifetime, s32 angle) {
    EffectEntry* entry;
    MissStarData* data;
    s32 i;
    s32 sign;
    f32 scale;
    f32 speed;

    entry = effEntry();
    scale = 0.800000012f;
    speed = 1.0f;
    if (type <= 0) {
        type = 1;
        scale = 0.400000006f;
        speed = 0.699999988f;
    }

    sign = 1;
    if (angle < 0) {
        angle = -angle;
        sign = -1;
    }
#ifdef MUST_MATCH
    // The original tests the bound with an explicit break; a plain `while (angle > 5)`
    // lets MWCC fold the trip count into a divwu/bdnz loop that does not match.
    while (1) {
        if (angle <= 5) {
            break;
        }
        angle -= 5;
    }
#else
    while (angle > 5) {
        angle -= 5;
    }
#endif

    entry->type = "SmallStar";
    entry->count = type;
    data = __memAlloc(HEAP_EFFECT, type * sizeof(MissStarData));
    entry->userdata = data;
    entry->callback = effMissStarMain;

    for (i = 0; i < type; i++, data++) {
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->dvx = speed * ((f32)sign * (2.0f * spd_tbl[angle % 8].x));
        data->dvy = speed * (2.0f * spd_tbl[angle % 8].y);
        data->dvz = speed * (2.0f * spd_tbl[angle % 8].z);
        data->vx = speed * ((f32)sign * spd_tbl[angle % 8].x);
        data->vy = speed * spd_tbl[angle % 8].y;
        data->vz = speed * spd_tbl[angle % 8].z;
        // Incremented here rather than in the for header: this is where the
        // original's scheduler emits the addi (before the scale lookup).
        angle++;
        data->scale = scale * scale_tbl[i % 8].x;
        data->angle = 0.0f;
        data->angleVel = (f32)(sign * 107);
        data->field_0x34 = -camGetPtr(CAMERA_3D)->viewYaw;
        data->field_0x24 = 14;
        data->field_0x28 = lifetime;
        data->field_0x3C = 10;
        data->field_0x40 = 255.0f;
    }

    return entry;
}

void effMissStarMain(EffectEntry* effect) {
    MissStarData* data;
    Vec position;
    Vec disp;
    f32 angleVelScale;
    s32 i;

    data = effect->userdata;

    disp = (Vec){ 0.0f, 0.0f, 0.0f };
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;
    angleVelScale = 0.800000012f;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x28 != 0) {
            data->field_0x28--;
            if (data->field_0x28 == 0) {
                break;
            }
        }

        if (data->field_0x24 >= 0) {
            data->position.x += data->dvx;
            data->position.y += data->dvy;
            data->position.z += data->dvz;
        }

        data->field_0x34 = -camGetPtr(CAMERA_3D)->viewYaw;
        data->angle = data->angle + data->angleVel;
        data->angle = reviseAngle(data->angle);
        data->angleVel *= angleVelScale;

        if (data->field_0x24 < 10) {
            data->vx *= 0.5f;
            data->vy *= 0.5f;
            data->vz *= 0.5f;
            data->dvx = data->vx;
            data->dvy = data->vy;
            data->dvz = data->vz;
        }

        data->field_0x24--;
        if (data->field_0x24 < 0) {
            data->field_0x3C--;
            if (data->field_0x3C < 0) {
                effDelete(effect);
                return;
            }
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effMissStarDisp, effect);
}

void effMissStarDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    MissStarData* data = effect->userdata;
    GXTexObj texObj;
    Mtx mtxB8;
    Mtx mtx88;
    Mtx mtx58;
    Mtx mtx28;
    s32 i;

    effGetTexObj(0x18, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x28 != 0) {
            break;
        }

        PSMTXTrans(mtxB8, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(mtx58, 'y', MTXDegToRad(-camGetPtr(CAMERA_3D)->viewYaw));
        PSMTXRotRad(mtx28, 'z', MTXDegToRad(data->angle));
        PSMTXScale(mtx88, data->scale, data->scale, data->scale);
        PSMTXConcat(mtx58, mtxB8, mtxB8);
        PSMTXConcat(mtxB8, mtx28, mtxB8);
        PSMTXConcat(mtxB8, mtx88, mtxB8);
        PSMTXConcat(camGetPtr(CAMERA_3D)->view, mtxB8, mtxB8);
        GXLoadPosMtxImm(mtxB8, 0);
        GXSetCurrentMtx(0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(-16.0f, 16.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(16.0f, 16.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(16.0f, -16.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-16.0f, -16.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}
