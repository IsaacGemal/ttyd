#include "eff/n64/eff_dogabon_attack_n64.h"
#include "eff/n64/eff_stone_smoke_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

#include <stdlib.h>

typedef struct DogabonAttackData {
    s32 type;           // 0x0
    f32 x;              // 0x4
    f32 y;              // 0x8
    f32 z;              // 0xC
    s32 lifetime;       // 0x10
    s32 frame;          // 0x14, ticks since spawn (scale_data index is frame % 7)
    s32 colorR;         // 0x18, TEVREG0 colour
    s32 colorG;         // 0x1C
    s32 colorB;         // 0x20
    s32 alpha;          // 0x24
    f32 scale;          // 0x28
    f32 posX[25];       // 0x2C, per-piece position
    f32 posY[25];       // 0x90
    f32 posZ[25];       // 0xF4
    f32 startX[25];     // 0x158, spawn ring (radius 120)
    f32 startY[25];     // 0x1BC
    f32 startZ[25];     // 0x220
    f32 targetX[25];    // 0x284, contracted ring (radius 15)
    f32 targetY[25];    // 0x2E8
    f32 targetZ[25];    // 0x34C
    f32 scaleX[25];     // 0x3B0, drawn quad X scale
    f32 scaleY[25];     // 0x414, drawn quad Y scale
    f32 rotZ[25];       // 0x478, degrees
    f32 rotY[25];       // 0x4DC, degrees
    f32 velX[25];       // 0x540, half of targetX when state 5 starts
    f32 velY[25];       // 0x5A4, half of targetY at state 5 start, then damped and gravity-pulled
    f32 velZ[25];       // 0x608, half of targetZ at state 5 start
    s32 waitTimer[25];  // 0x66C, state 0 countdown
    s32 state[25];      // 0x6D0, selects the case in effDogabonAttackMain
    s32 stateTimer[25]; // 0x734, frames in the current state (also indexes the move tables)
} DogabonAttackData;

static u8 scale_data[8] = { 0x5E, 0x5C, 0x64, 0x69, 0x6B, 0x69, 0x64, 0x00 };
u8 scale_data_2[8] = { 0x14, 0x28, 0x3A, 0x49, 0x55, 0x5C, 0x61, 0x64 };
u8 move_data_1[16] = { 0x00, 0x02, 0x05, 0x09, 0x0E, 0x14, 0x1B, 0x23, 0x2C, 0x36, 0x40, 0x4A, 0x54, 0x5E, 0x64, 0x00 };
u8 move_data_hazure[20] = { 0x00, 0x0A, 0x14, 0x1E, 0x28, 0x32, 0x3C, 0x46, 0x50, 0x5A,
                            0x64, 0x6D, 0x75, 0x7C, 0x82, 0x87, 0x8B, 0x8E, 0x90, 0x91 };
u8 move_data_2[72] = {
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x65, 0x66, 0x66, 0x67,
    0x67, 0x68, 0x68, 0x68, 0x6B, 0x6B, 0x6E, 0x6E, 0x76, 0x76, 0x7E, 0x7E,
    0x86, 0x8E, 0x91, 0x91, 0x91, 0x8E, 0x86, 0x7E, 0x7E, 0x76, 0x76, 0x6E,
    0x6E, 0x69, 0x69, 0x69, 0x66, 0x67, 0x67, 0x68, 0x68, 0x68, 0x6B, 0x6B,
    0x6E, 0x6E, 0x76, 0x76, 0x7E, 0x7E, 0x86, 0x8E, 0x91, 0x91, 0x91, 0x8E,
    0x86, 0x7E, 0x7E, 0x76, 0x76, 0x6E, 0x6E, 0x69, 0x69, 0x69, 0x00, 0x00,
};

static s8 y_data[28] = {
    -5, -5, -5, -4, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0,
    1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 0, 0, 0,
};

// .data vertex array for the 12x12 textured quad.
u32 size12x12_tex32x32_vtx[14] = {
    0xFFFAFFFA, 0x00000000, 0x00000000, 0x00FF0006, 0xFFFA0000, 0x04000000, 0x000000FF,
    0x00060006, 0x00000400, 0x04000000, 0x00FFFFFA, 0x00060000, 0x00000400, 0x000000FF,
};

// .rodata zero vector and the two int->float conversion doubles (named so the pool matches).

EffectEntry* effDogabonAttackN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    DogabonAttackData* data;
    s32 i;
    f32 s;
    f32 c;
    f32 angle;
    s32 r;

    effect = effEntry();
    effect->type = "DogabonAttackN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(DogabonAttackData));
    effect->userdata = data;
    effect->callback = effDogabonAttackMain;
    effect->flags |= 2;
    data->type = type;
    data->frame = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->alpha = 0xFF;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;
    data->colorR = 0x46;
    data->colorG = 0xB4;
    data->colorB = 0x78;

    for (i = 0; i < 25; i++) {
        angle = (f32)(i * 1080 / 25) * 6.2831855f / 360.0f;
        s = (f32)sin(angle);
        c = (f32)cos(angle);
        r = rand();
        data->rotZ[i] = (f32)(r % 360 - 180);
        data->targetX[i] = 15.0f * s;
        data->targetY[i] = (f32)(y_data[i] + 10);
        data->targetZ[i] = 15.0f * c;
        data->startX[i] = 120.0f * s;
        data->startY[i] = (f32)(y_data[i] * 8 + 40);
        data->startZ[i] = 120.0f * c;
        data->velY[i] = 0.0f;
        data->waitTimer[i] = i * 2 + 1;
        data->state[i] = 0;
        data->stateTimer[i] = 0;
        data->rotY[i] = 0.0f;
    }

    return effect;
}

void effDogabonAttackMain(EffectEntry* effect) {
    DogabonAttackData* data = effect->userdata;
    Vec position;
    Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 type;
    s32 index;
    f32 x0;
    f32 y0;
    f32 z0;

    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
    position = disp;

    type = data->type;
    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x40;
    }

    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->frame++;

    if (data->lifetime < 0) {
        effDelete(effect);
        return;
    }

    index = data->frame % 7;

    for (i = 0; i < 25; i++) {
        f32 scale;

        scale = 0.01f * (f32)scale_data[index];
        data->scaleY[i] = scale;
        data->scaleX[i] = 2.0f - scale;

        x0 = data->startX[i];
        y0 = data->startY[i];
        z0 = data->startZ[i];

        switch (data->state[i]) {
        case 0:
            data->posX[i] = x0;
            data->posY[i] = y0;
            data->posZ[i] = z0;
            if (data->waitTimer[i] != 0) {
                data->waitTimer[i]--;
            } else {
                f32 s;
                data->state[i] = 1;
                s = 0.01f * (f32)scale_data_2[data->stateTimer[i]];
                data->scaleY[i] = s;
                data->scaleX[i] = s;
                data->stateTimer[i]++;
            }
            break;
        case 1:
            {
                f32 s = 0.01f * (f32)scale_data_2[data->stateTimer[i]];
                data->scaleY[i] = s;
                data->scaleX[i] = s;
                data->stateTimer[i]++;
            }
            if (data->stateTimer[i] >= 8) {
                if (type == 0) {
                    data->state[i] = 2;
                } else {
                    data->state[i] = 20;
                }
                data->stateTimer[i] = 0;
            }
            break;
        case 20:
            {
                f32 s = 0.01f * (f32)move_data_hazure[data->stateTimer[i]];
                data->posX[i] = (data->targetX[i] - x0) * s + x0;
                data->posY[i] = (data->targetY[i] - y0) * s + y0;
                data->posZ[i] = (data->targetZ[i] - z0) * s + z0;
            }
            data->scaleY[i] = 1.0f;
            data->scaleX[i] = 1.0f;
            data->stateTimer[i]++;
            if (data->stateTimer[i] >= 20) {
                data->state[i] = 5;
                data->stateTimer[i] = 0;
                data->velX[i] = 0.5f * data->targetX[i];
                data->velY[i] = 0.5f * data->targetY[i];
                data->velZ[i] = 0.5f * data->targetZ[i];
            }
            break;
        case 2:
            {
                f32 s = 0.01f * (f32)move_data_1[data->stateTimer[i]];
                data->posX[i] = (data->targetX[i] - x0) * s + x0;
                data->posY[i] = (data->targetY[i] - y0) * s + y0;
                data->posZ[i] = (data->targetZ[i] - z0) * s + z0;
            }
            data->scaleY[i] = 1.0f;
            data->scaleX[i] = 1.0f;
            data->stateTimer[i]++;
            if (data->stateTimer[i] >= 15) {
                data->state[i] = 3;
                data->stateTimer[i] = 0;
            }
            break;
        case 3:
            if (i == 24) {
                /* MWCC only emits the original's 25 straight stores if they are written out; a loop
                   leaves a redundant guard. */
                data->state[0] = 4;
                data->state[1] = 4;
                data->state[2] = 4;
                data->state[3] = 4;
                data->state[4] = 4;
                data->state[5] = 4;
                data->state[6] = 4;
                data->state[7] = 4;
                data->state[8] = 4;
                data->state[9] = 4;
                data->state[10] = 4;
                data->state[11] = 4;
                data->state[12] = 4;
                data->state[13] = 4;
                data->state[14] = 4;
                data->state[15] = 4;
                data->state[16] = 4;
                data->state[17] = 4;
                data->state[18] = 4;
                data->state[19] = 4;
                data->state[20] = 4;
                data->state[21] = 4;
                data->state[22] = 4;
                data->state[23] = 4;
                data->state[24] = 4;
            }
            break;
        case 4:
            {
                f32 t = 0.01f * (f32)move_data_2[data->stateTimer[i]];
                f32 sinAng;
                f32 ang;
                f32 cosAng;
                f32 factor;
                f32 cosVal = (f32)cos(6.2831855f * (f32)(i * 180 / 25) / 360.0f);
                f32 amp = 0.5f * (1080.0f * (1.0f - t));

                ang = 6.2831855f
                    * ((f32)(i * 1080 / 25) + amp * cosVal)
                    / 360.0f;
                sinAng = (f32)sin(ang);
                cosAng = (f32)cos(ang);
                factor = 15.0f / t / t;
                data->posX[i] = sinAng * factor;
                data->posY[i] = (f32)(y_data[i] + 10);
                data->posZ[i] = cosAng * factor;
            }
            data->stateTimer[i]++;
            if (data->stateTimer[i] >= 70) {
                data->state[i] = 5;
                data->stateTimer[i] = 0;
                data->velX[i] = 0.5f * data->targetX[i];
                data->velY[i] = 0.5f * data->targetY[i];
                data->velZ[i] = 0.5f * data->targetZ[i];
            }
            break;
        case 5:
            data->posX[i] += data->velX[i];
            data->posY[i] += data->velY[i];
            data->posZ[i] += data->velZ[i];
            data->velX[i] *= 0.99f;
            data->velY[i] *= 0.99f;
            data->velZ[i] *= 0.99f;
            data->velY[i] += -0.1f;
            data->stateTimer[i]++;
            if (data->stateTimer[i] > 16) {
                data->stateTimer[i] = 0;
                data->state[i] = 6;
            }
            data->rotZ[i] *= 0.9f;
            break;
        case 6:
            {
                Mtx transMtx;
                Mtx rotMtx;
                EffectEntry* smokeEffect;
                f32 x;
                f32 y;
                f32 z;

                PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY[i]);
                PSMTXTrans(transMtx, data->posX[i], data->posY[i], data->posZ[i]);
                PSMTXConcat(rotMtx, transMtx, transMtx);
                x = data->x + transMtx[0][3];
                y = data->y + transMtx[1][3];
                z = data->z + transMtx[2][3];
                smokeEffect = effStoneSmokeN64Entry(0, 0x10, x, y, z, 1.0f);
                ((effStoneSmokeN64Data*)smokeEffect->userdata)->color2R = 100;
                ((effStoneSmokeN64Data*)smokeEffect->userdata)->color2G = 0;
                ((effStoneSmokeN64Data*)smokeEffect->userdata)->color2B = 0;
                data->state[i] = 7;
            }
            break;
        default:
            break;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effDogabonAttackDisp, effect);
}

void effDogabonAttackDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    DogabonAttackData* data;
    CameraEntry* camera;
    GXTexObj texObj;
    Mtx transMtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx baseMtx;
    s32 alpha;
    s32 i;

    camera = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->alpha;

    PSMTXTrans(transMtx, data->x, data->y, data->z);
    PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
    PSMTXConcat(transMtx, scaleMtx, baseMtx);
    PSMTXConcat(camera->view, baseMtx, baseMtx);

    GXSetTevColor(GX_TEVREG0, (GXColor){ data->colorR, data->colorG, data->colorB, alpha });
    GXSetTevColor(GX_TEVREG1, (GXColor){ 0, 0, 0, 0 });

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_C0, GX_CC_C1, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scaleMtx, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x8C, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64(size12x12_tex32x32_vtx);

    for (i = 0; i < 25; i++) {
        if (data->state[i] == 0 || data->state[i] == 7) {
            continue;
        }
        PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY[i]);
        PSMTXTrans(transMtx, data->posX[i], data->posY[i], data->posZ[i]);
        PSMTXConcat(rotMtx, transMtx, transMtx);
        PSMTXRotRad(rotMtx, 'y', 0.017453292f * -data->rotY[i]);
        PSMTXConcat(transMtx, transMtx, transMtx);
        PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->rotZ[i]);
        PSMTXConcat(transMtx, rotMtx, transMtx);
        PSMTXScale(scaleMtx, data->scaleX[i], data->scaleY[i], 1.0f);
        PSMTXConcat(transMtx, scaleMtx, transMtx);
        PSMTXConcat(baseMtx, transMtx, transMtx);
        GXLoadPosMtxImm(transMtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
