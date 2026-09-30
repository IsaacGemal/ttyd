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
    s32 field_0x14;     // 0x14
    s32 field_0x18;     // 0x18
    s32 field_0x1C;     // 0x1C
    s32 field_0x20;     // 0x20
    s32 field_0x24;     // 0x24
    f32 scale;          // 0x28
    f32 field_0x2C[25]; // 0x2C
    f32 field_0x90[25]; // 0x90
    f32 field_0xF4[25]; // 0xF4
    f32 field_0x158[25]; // 0x158
    f32 field_0x1BC[25]; // 0x1BC
    f32 field_0x220[25]; // 0x220
    f32 field_0x284[25]; // 0x284
    f32 field_0x2E8[25]; // 0x2E8
    f32 field_0x34C[25]; // 0x34C
    f32 field_0x3B0[25]; // 0x3B0
    f32 field_0x414[25]; // 0x414
    f32 field_0x478[25]; // 0x478
    f32 field_0x4DC[25]; // 0x4DC
    f32 field_0x540[25]; // 0x540
    f32 field_0x5A4[25]; // 0x5A4
    f32 field_0x608[25]; // 0x608
    s32 field_0x66C[25]; // 0x66C
    s32 field_0x6D0[25]; // 0x6D0
    s32 field_0x734[25]; // 0x734
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

extern const f32 float_0_8041ecf4;
extern const f32 float_0p03125_8041ecf0;
extern const f32 float_deg2rad_8041ecf8;
extern const f32 float_1_8041ecfc;
extern const f32 float_0p01_8041ed00;
extern const f32 float_2_8041ed04;
extern const f32 float_0p5_8041ed08;
extern const f32 float_6p2832_8041ed0c;
extern const f32 float_360_8041ed10;
extern const f32 float_1080_8041ed14;
extern const f32 float_15_8041ed18;
extern const f32 float_0p99_8041ed1c;
extern const f32 float_neg0p1_8041ed20;
extern const f32 float_0p9_8041ed24;
extern const f32 float_120_8041ed28;

// .data vertex array for the 12x12 textured quad.
u32 size12x12_tex32x32_vtx[14] = {
    0xFFFAFFFA, 0x00000000, 0x00000000, 0x00FF0006, 0xFFFA0000, 0x04000000, 0x000000FF,
    0x00060006, 0x00000400, 0x04000000, 0x00FFFFFA, 0x00060000, 0x00000400, 0x000000FF,
};

// .rodata zero vector and the two int->float conversion doubles (named so the pool matches).
const Vec vec3_802fa4b0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fa4c0 = 4503599627370496.0;
const f64 double_to_int_802fa4c8 = 4503601774854144.0;

const char str_802FA4D0[20] = "DogabonAttackN64";

EffectEntry* effDogabonAttackN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    DogabonAttackData* data;
    s32 i;
    f32 s;
    f32 c;
    f32 angle;
    s32 r;

    effect = effEntry();
    effect->type = str_802FA4D0;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(DogabonAttackData));
    effect->userdata = data;
    effect->callback = effDogabonAttackMain;
    effect->flags |= 2;
    data->type = type;
    data->field_0x14 = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->field_0x24 = 0xFF;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;
    data->field_0x18 = 0x46;
    data->field_0x1C = 0xB4;
    data->field_0x20 = 0x78;

    for (i = 0; i < 25; i++) {
        angle = (f32)(i * 1080 / 25) * float_6p2832_8041ed0c / float_360_8041ed10;
        s = (f32)sin(angle);
        c = (f32)cos(angle);
        r = rand();
        data->field_0x478[i] = (f32)(r % 360 - 180);
        data->field_0x284[i] = float_15_8041ed18 * s;
        data->field_0x2E8[i] = (f32)(y_data[i] + 10);
        data->field_0x34C[i] = float_15_8041ed18 * c;
        data->field_0x158[i] = float_120_8041ed28 * s;
        data->field_0x1BC[i] = (f32)(y_data[i] * 8 + 40);
        data->field_0x220[i] = float_120_8041ed28 * c;
        data->field_0x5A4[i] = float_0_8041ecf4;
        data->field_0x66C[i] = i * 2 + 1;
        data->field_0x6D0[i] = 0;
        data->field_0x734[i] = 0;
        data->field_0x4DC[i] = float_0_8041ecf4;
    }

    return effect;
}

void effDogabonAttackMain(EffectEntry* effect) {
    DogabonAttackData* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second local before
       dispCalcZ; a single local folds the copy away (same idiom as effGlassMain/effStoneSmokeMain). */
    Vec position;
#endif
    Vec disp = vec3_802fa4b0;
    s32 i;
    s32 type;
    s32 index;
    f32 x0;
    f32 y0;
    f32 z0;

    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    type = data->type;
    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x40;
    }

    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->field_0x14++;

    if (data->lifetime < 0) {
        effDelete(effect);
        return;
    }

    index = data->field_0x14 % 7;

    for (i = 0; i < 25; i++) {
        f32 scale;

        scale = float_0p01_8041ed00 * (f32)scale_data[index];
        data->field_0x414[i] = scale;
        data->field_0x3B0[i] = float_2_8041ed04 - scale;

        x0 = data->field_0x158[i];
        y0 = data->field_0x1BC[i];
        z0 = data->field_0x220[i];

        switch (data->field_0x6D0[i]) {
        case 0:
            data->field_0x2C[i] = x0;
            data->field_0x90[i] = y0;
            data->field_0xF4[i] = z0;
            if (data->field_0x66C[i] != 0) {
                data->field_0x66C[i]--;
            } else {
                f32 s;
                data->field_0x6D0[i] = 1;
                s = float_0p01_8041ed00 * (f32)scale_data_2[data->field_0x734[i]];
                data->field_0x414[i] = s;
                data->field_0x3B0[i] = s;
                data->field_0x734[i]++;
            }
            break;
        case 1:
            {
                f32 s = float_0p01_8041ed00 * (f32)scale_data_2[data->field_0x734[i]];
                data->field_0x414[i] = s;
                data->field_0x3B0[i] = s;
                data->field_0x734[i]++;
            }
            if (data->field_0x734[i] >= 8) {
                if (type == 0) {
                    data->field_0x6D0[i] = 2;
                } else {
                    data->field_0x6D0[i] = 20;
                }
                data->field_0x734[i] = 0;
            }
            break;
        case 20:
            {
                f32 s = float_0p01_8041ed00 * (f32)move_data_hazure[data->field_0x734[i]];
                data->field_0x2C[i] = (data->field_0x284[i] - x0) * s + x0;
                data->field_0x90[i] = (data->field_0x2E8[i] - y0) * s + y0;
                data->field_0xF4[i] = (data->field_0x34C[i] - z0) * s + z0;
            }
            data->field_0x414[i] = float_1_8041ecfc;
            data->field_0x3B0[i] = float_1_8041ecfc;
            data->field_0x734[i]++;
            if (data->field_0x734[i] >= 20) {
                data->field_0x6D0[i] = 5;
                data->field_0x734[i] = 0;
                data->field_0x540[i] = float_0p5_8041ed08 * data->field_0x284[i];
                data->field_0x5A4[i] = float_0p5_8041ed08 * data->field_0x2E8[i];
                data->field_0x608[i] = float_0p5_8041ed08 * data->field_0x34C[i];
            }
            break;
        case 2:
            {
                f32 s = float_0p01_8041ed00 * (f32)move_data_1[data->field_0x734[i]];
                data->field_0x2C[i] = (data->field_0x284[i] - x0) * s + x0;
                data->field_0x90[i] = (data->field_0x2E8[i] - y0) * s + y0;
                data->field_0xF4[i] = (data->field_0x34C[i] - z0) * s + z0;
            }
            data->field_0x414[i] = float_1_8041ecfc;
            data->field_0x3B0[i] = float_1_8041ecfc;
            data->field_0x734[i]++;
            if (data->field_0x734[i] >= 15) {
                data->field_0x6D0[i] = 3;
                data->field_0x734[i] = 0;
            }
            break;
        case 3:
            if (i == 24) {
                /* MWCC only emits the original's 25 straight stores if they are written out; a loop
                   leaves a redundant guard. */
                data->field_0x6D0[0] = 4;
                data->field_0x6D0[1] = 4;
                data->field_0x6D0[2] = 4;
                data->field_0x6D0[3] = 4;
                data->field_0x6D0[4] = 4;
                data->field_0x6D0[5] = 4;
                data->field_0x6D0[6] = 4;
                data->field_0x6D0[7] = 4;
                data->field_0x6D0[8] = 4;
                data->field_0x6D0[9] = 4;
                data->field_0x6D0[10] = 4;
                data->field_0x6D0[11] = 4;
                data->field_0x6D0[12] = 4;
                data->field_0x6D0[13] = 4;
                data->field_0x6D0[14] = 4;
                data->field_0x6D0[15] = 4;
                data->field_0x6D0[16] = 4;
                data->field_0x6D0[17] = 4;
                data->field_0x6D0[18] = 4;
                data->field_0x6D0[19] = 4;
                data->field_0x6D0[20] = 4;
                data->field_0x6D0[21] = 4;
                data->field_0x6D0[22] = 4;
                data->field_0x6D0[23] = 4;
                data->field_0x6D0[24] = 4;
            }
            break;
        case 4:
            {
                f32 t = float_0p01_8041ed00 * (f32)move_data_2[data->field_0x734[i]];
                f32 sinAng;
                f32 ang;
                f32 cosAng;
                f32 factor;
                f32 cosVal = (f32)cos(float_6p2832_8041ed0c * (f32)(i * 180 / 25) / float_360_8041ed10);
                f32 amp = float_0p5_8041ed08 * (float_1080_8041ed14 * (float_1_8041ecfc - t));

                ang = float_6p2832_8041ed0c
                    * ((f32)(i * 1080 / 25) + amp * cosVal)
                    / float_360_8041ed10;
                sinAng = (f32)sin(ang);
                cosAng = (f32)cos(ang);
                factor = float_15_8041ed18 / t / t;
                data->field_0x2C[i] = sinAng * factor;
                data->field_0x90[i] = (f32)(y_data[i] + 10);
                data->field_0xF4[i] = cosAng * factor;
            }
            data->field_0x734[i]++;
            if (data->field_0x734[i] >= 70) {
                data->field_0x6D0[i] = 5;
                data->field_0x734[i] = 0;
                data->field_0x540[i] = float_0p5_8041ed08 * data->field_0x284[i];
                data->field_0x5A4[i] = float_0p5_8041ed08 * data->field_0x2E8[i];
                data->field_0x608[i] = float_0p5_8041ed08 * data->field_0x34C[i];
            }
            break;
        case 5:
            data->field_0x2C[i] += data->field_0x540[i];
            data->field_0x90[i] += data->field_0x5A4[i];
            data->field_0xF4[i] += data->field_0x608[i];
            data->field_0x540[i] *= float_0p99_8041ed1c;
            data->field_0x5A4[i] *= float_0p99_8041ed1c;
            data->field_0x608[i] *= float_0p99_8041ed1c;
            data->field_0x5A4[i] += float_neg0p1_8041ed20;
            data->field_0x734[i]++;
            if (data->field_0x734[i] > 16) {
                data->field_0x734[i] = 0;
                data->field_0x6D0[i] = 6;
            }
            data->field_0x478[i] *= float_0p9_8041ed24;
            break;
        case 6:
            {
                Mtx transMtx;
                Mtx rotMtx;
                EffectEntry* smokeEffect;
                f32 x;
                f32 y;
                f32 z;

                PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041ecf8 * data->field_0x4DC[i]);
                PSMTXTrans(transMtx, data->field_0x2C[i], data->field_0x90[i], data->field_0xF4[i]);
                PSMTXConcat(rotMtx, transMtx, transMtx);
                x = data->x + transMtx[0][3];
                y = data->y + transMtx[1][3];
                z = data->z + transMtx[2][3];
                smokeEffect = effStoneSmokeN64Entry(0, 0x10, x, y, z, float_1_8041ecfc);
                ((effStoneSmokeN64Data*)smokeEffect->userdata)->field_0x28 = 100;
                ((effStoneSmokeN64Data*)smokeEffect->userdata)->field_0x2C = 0;
                ((effStoneSmokeN64Data*)smokeEffect->userdata)->field_0x30 = 0;
                data->field_0x6D0[i] = 7;
            }
            break;
        default:
            break;
        }
    }

#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 1, effDogabonAttackDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 1, effDogabonAttackDisp, effect);
#endif
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
    alpha = data->field_0x24;

    PSMTXTrans(transMtx, data->x, data->y, data->z);
    PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
    PSMTXConcat(transMtx, scaleMtx, baseMtx);
    PSMTXConcat(camera->view, baseMtx, baseMtx);

    GXSetTevColor(GX_TEVREG0, (GXColor){ data->field_0x18, data->field_0x1C, data->field_0x20, alpha });
    GXSetTevColor(GX_TEVREG1, (GXColor){ 0, 0, 0, 0 });

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_C0, GX_CC_C1, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
    PSMTXScale(scaleMtx, float_0p03125_8041ecf0, float_0p03125_8041ecf0, float_0_8041ecf4);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x8C, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size12x12_tex32x32_vtx);

    for (i = 0; i < 25; i++) {
        if (data->field_0x6D0[i] == 0 || data->field_0x6D0[i] == 7) {
            continue;
        }
        PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041ecf8 * data->field_0x4DC[i]);
        PSMTXTrans(transMtx, data->field_0x2C[i], data->field_0x90[i], data->field_0xF4[i]);
        PSMTXConcat(rotMtx, transMtx, transMtx);
        PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041ecf8 * -data->field_0x4DC[i]);
        PSMTXConcat(transMtx, transMtx, transMtx);
        PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041ecf8 * data->field_0x478[i]);
        PSMTXConcat(transMtx, rotMtx, transMtx);
        PSMTXScale(scaleMtx, data->field_0x3B0[i], data->field_0x414[i], float_1_8041ecfc);
        PSMTXConcat(transMtx, scaleMtx, transMtx);
        PSMTXConcat(baseMtx, transMtx, transMtx);
        GXLoadPosMtxImm(transMtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(0x90, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
