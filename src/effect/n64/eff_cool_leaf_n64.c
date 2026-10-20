#include "eff/n64/eff_cool_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <stdlib.h>
#include "evt/evt_cmd.h"

extern const f32 float_0_8041ec60;
extern const f32 float_0p1_8041ecb0;
extern const f32 float_2_8041ec70;
extern const f32 float_0p05_8041ec88;
extern const f32 float_deg2rad_8041ec54;
extern const f32 float_4_8041ec58;
extern const f32 float_1_8041ec5c;
extern const f32 float_0p3_8041ec64;
extern const f32 float_0p8_8041ec68;
extern const f32 float_6p2832_8041ec6c;
extern const f32 float_360_8041ec74;
extern const f32 float_neg0p05_8041ec78;
extern const f32 float_0p2_8041ec7c;
extern const f32 float_0p94_8041ec80;
extern const f32 float_0p95_8041ec84;
extern const f32 float_0p92_8041ec8c;
extern const f32 float_10_8041ec90;
extern const f32 float_0p5_8041ec94;
extern const f32 float_30_8041ec98;
extern const f32 float_neg200_8041ec9c;
extern const f32 float_0p4_8041eca0;
extern const f32 float_0p98_8041eca4;
extern const f32 float_0p8464_8041eca8;
extern const f32 float_0p9604_8041ecac;
extern const u32 coolLeafTevColor;
extern u8 size12x12_tex64x64_brown_vtx[];
extern u8 size12x12_tex64x64_red_vtx[];

// clang-format off
static inline void effCoolLeafSetColor(u8 alpha) {
    GXColor color = *(GXColor*)&coolLeafTevColor;
    color.a = alpha;
    GXSetTevColor(GX_TEVREG0, color);
}




typedef struct effCoolLeafData {
    s32 type;        // 0x00
    f32 x;           // 0x04
    f32 y;           // 0x08
    f32 z;           // 0x0C
    f32 velX;        // 0x10: added to x each frame (type 2 starts it pointing back toward x = 0)
    f32 velY;        // 0x14: added to y each frame, minus 0.05 gravity
    f32 rotZ;        // 0x18: tilt, drawn with PSMTXRotRad 'z'
    f32 spin;        // 0x1C: random-walk rate that drives rotZ through sin
    f32 rotY;        // 0x20: yaw, drawn with PSMTXRotRad 'y'
    s32 alpha;       // 0x24: TEVREG0 alpha, fades in over 10 frames and out near the end
    s32 life;        // 0x28: frames left; the leaf is deleted when it goes below 0
    s32 age;         // 0x2C: frames since spawn
    f32 driftX;      // 0x30: per-frame x drift, damped each frame and nudged toward x = 0 (x = -200 below y = 30)
    f32 driftY;      // 0x34: per-frame y drift (always 0 in the original)
    f32 driftZ;      // 0x38: per-frame z drift, damped like driftX
} effCoolLeafData;


static inline Vec makeVec(f32 x, f32 y, f32 z) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

EffectEntry* effCoolLeafN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effCoolLeafData* data;
    effCoolLeafData* leaf;
    s32 i;
    s32 randX;
    s32 randY;
    s32 randAngle;
    s32 randSpin;

    effect = effEntry();
    effect->type = "CoolLeafN64";
    effect->count = 5;
    data = __memAlloc(HEAP_EFFECT, sizeof(effCoolLeafData) * 5);
    effect->userdata = data;
    effect->callback = effCoolLeafMain;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->life = 100;
    data->age = 0;
    data->alpha = 0;
    leaf = data + 1;
    for (i = 1; i < 5; i++, leaf++) {
        randX = rand() % 51;
        randY = rand() % 21;
        randAngle = rand() % 361;
        randSpin = rand() % 101;
        leaf->x = (f32)(randX + 75);
        leaf->y = (f32)(randY + 70);
        leaf->z = float_0_8041ec60;
        leaf->rotZ = (f32)randAngle;
        leaf->rotY = (f32)(360 - randAngle);
        switch (type) {
            case 0:
                leaf->spin = float_0p1_8041ecb0 * (f32)randSpin;
                leaf->velX = float_0_8041ec60;
                leaf->velY = float_0_8041ec60;
                leaf->driftX = -3.0f;
                leaf->driftY = float_0_8041ec60;
                leaf->driftZ = 3.0f;
                break;
            case 1:
                leaf->spin = float_0p1_8041ecb0 * (f32)randSpin * float_2_8041ec70;
                leaf->velX = float_0_8041ec60;
                leaf->velY = float_0_8041ec60;
                leaf->driftX = -6.0f;
                leaf->driftY = float_0_8041ec60;
                leaf->driftZ = 6.0f;
                break;
            case 2:
                leaf->spin = float_0p1_8041ecb0 * (f32)randSpin * float_2_8041ec70;
                leaf->velX = -leaf->x * (0.008f * (f32)(rand() % 11) + float_0p05_8041ec88);
                leaf->velY = -leaf->y * (0.008f * (f32)(rand() % 11) + float_0p05_8041ec88);
                break;
            default:
                break;
        }
    }
    return effect;
}


// Feeding the type-2 loop's sin/cos results through the `trig` temp (a natural shape,
// guarded by MUST_MATCH below) changes the allocator's global callee-saved FPR
// assignment so both the type-1 and type-2 loop-constant groups land on the original's
// registers; the plain straight-into-the-multiply form scores 99.82%. Previously the
// type-1 group came out f31=double, f30=10.0, f29=0.5, f28=30.0 (original f28=double,
// f29=10.0, f30=30.0, f31=0.5) and type-2 f28=double, f29=6.2832, f30=360.0, f31=10.0
// (original f30=double, f31=6.2832, f23=10.0, f24=360.0).
void effCoolLeafMain(EffectEntry* effect) {
    effCoolLeafData* data = effect->userdata;
    Vec position;
    s32 type;
    s32 age;
    s32 life;
    s32 randIndex;
    s32 i;
    f32 sway;
    f32 swayPush;
    f32 doublePush;
    f32 tiltSin;
    f32 trig;

    position = makeVec(data->x, data->y, data->z);

    data->life--;
    data->age++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    age = data->age;
    type = data->type;
    life = data->life;
    if (age < 10) {
        data->alpha = (s32)(float_0p3_8041ec64 * (f32)(255 - data->alpha) + (f32)data->alpha);
    }
    if (life < 10) {
        data->alpha = (s32)((f32)data->alpha * float_0p8_8041ec68);
    }
    data++;
    switch (type) {
    case 0:
        i = 1;
        randIndex = age + 20;
        for (; i < effect->count; randIndex += 20, i++, data++) {
            sway = (f32)sin(float_6p2832_8041ec6c * (float_2_8041ec70 * data->rotZ) / float_360_8041ec74);
            data->velY += float_neg0p05_8041ec78;
            swayPush = float_0p2_8041ec7c * sway;
            data->velX += swayPush;
            if (type == 0) {
                data->velX *= float_0p94_8041ec80;
                tiltSin = (f32)sin(float_6p2832_8041ec6c * data->rotZ / float_360_8041ec74);
                data->velY *= float_0p05_8041ec88 * tiltSin + float_0p95_8041ec84;
            }
            else {
                data->velY += float_neg0p05_8041ec78;
                data->velX *= float_0p92_8041ec8c;
            }
            data->spin += (f32)(s32)(effTblRandN64(50, randIndex) - 25);
            data->rotZ += float_10_8041ec90 * (f32)sin(float_6p2832_8041ec6c * data->spin / float_360_8041ec74);
            data->rotY += float_10_8041ec90 * (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->spin) / float_360_8041ec74);
            data->x += data->velX;
            data->y += data->velY;
            if (data->y < float_30_8041ec98) {
                if (data->x > float_neg200_8041ec9c) {
                    data->driftX -= float_0p4_8041eca0;
                }
                else {
                    data->driftX += float_0p4_8041eca0;
                }
                if (data->z > float_0_8041ec60) {
                    data->driftZ -= float_0p4_8041eca0;
                }
                else {
                    data->driftZ += float_0p4_8041eca0;
                }
                data->driftX *= float_0p98_8041eca4;
                data->driftZ *= float_0p98_8041eca4;
                data->velY *= float_0p98_8041eca4;
            }
            else {
                if (data->x > float_0_8041ec60) {
                    data->driftX -= float_0p4_8041eca0;
                }
                else {
                    data->driftX += float_0p4_8041eca0;
                }
                if (data->z > float_0_8041ec60) {
                    data->driftZ -= float_0p4_8041eca0;
                }
                else {
                    data->driftZ += float_0p4_8041eca0;
                }
                data->driftX *= float_0p98_8041eca4;
                data->driftZ *= float_0p98_8041eca4;
            }
            data->x += data->driftX;
            data->y += data->driftY;
            data->z += data->driftZ;
        }
        break;
    case 1:
        i = 1;
        randIndex = age + 20;
        for (; i < effect->count; randIndex += 20, i++, data++) {
            sway = (f32)sin(float_6p2832_8041ec6c * (float_2_8041ec70 * data->rotZ) / float_360_8041ec74);
            data->velY += float_neg0p05_8041ec78;
            swayPush = float_0p2_8041ec7c * sway;
            doublePush = float_2_8041ec70 * swayPush;
            data->velX += doublePush;
            data->velY += float_neg0p05_8041ec78;
            data->velX *= float_0p8464_8041eca8;
            data->spin += (f32)(s32)(effTblRandN64(50, randIndex) - 25);
            data->rotZ += float_10_8041ec90 * (f32)sin(float_6p2832_8041ec6c * data->spin / float_360_8041ec74);
            data->rotY += float_10_8041ec90 * (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->spin) / float_360_8041ec74);
            data->x += data->velX;
            data->y += data->velY;
            if (data->y < float_30_8041ec98) {
                if (data->x > float_neg200_8041ec9c) {
                    data->driftX -= float_0p8_8041ec68;
                }
                else {
                    data->driftX += float_0p8_8041ec68;
                }
                if (data->z > float_0_8041ec60) {
                    data->driftZ -= float_0p8_8041ec68;
                }
                else {
                    data->driftZ += float_0p8_8041ec68;
                }
                data->driftX *= float_0p9604_8041ecac;
                data->driftZ *= float_0p9604_8041ecac;
                data->velY *= float_0p9604_8041ecac;
            }
            else {
                if (data->x > float_0_8041ec60) {
                    data->driftX -= float_0p8_8041ec68;
                }
                else {
                    data->driftX += float_0p8_8041ec68;
                }
                if (data->z > float_0_8041ec60) {
                    data->driftZ -= float_0p8_8041ec68;
                }
                else {
                    data->driftZ += float_0p8_8041ec68;
                }
                data->driftX *= float_0p9604_8041ecac;
                data->driftZ *= float_0p9604_8041ecac;
            }
            data->x += data->driftX;
            data->y += data->driftY;
            data->z += data->driftZ;
        }
        break;
    case 2:
        i = 1;
        randIndex = age + 20;
        for (; i < effect->count; randIndex += 20, i++, data++) {
            data->spin += (f32)(s32)(effTblRandN64(50, randIndex) - 25);
#ifdef MUST_MATCH
            // Match shape: keeping the call results in `trig` pins the loop-constant FPRs to the original's.
            trig = (f32)sin(float_6p2832_8041ec6c * data->spin / float_360_8041ec74);
            data->rotZ += float_10_8041ec90 * trig;
            trig = (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->spin) / float_360_8041ec74);
            data->rotY += float_10_8041ec90 * trig;
#else
            data->rotZ += float_10_8041ec90 * (f32)sin(float_6p2832_8041ec6c * data->spin / float_360_8041ec74);
            data->rotY += float_10_8041ec90 * (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->spin) / float_360_8041ec74);
#endif
            data->x += data->velX;
            data->y += data->velY;
        }
        break;
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCoolLeafDisp, effect);
}

void effCoolLeafDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam = camGetPtr(cameraId);
    effCoolLeafData* data = effect->userdata;
    GXTexObj texObj;
    Mtx trans;
    Mtx texMtx;
    Mtx rot;
    Mtx viewMtx;
    s32 type = data->type;
    s32 i;
    f32 deg2rad;

    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041ec54 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, viewMtx);
    PSMTXConcat(cam->view, viewMtx, viewMtx);

    effCoolLeafSetColor(data->alpha);

    effGetTexObjN64(0x14, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(texMtx, float_4_8041ec58, float_1_8041ec5c, float_0_8041ec60);
    GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
    GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xC);

    deg2rad = float_deg2rad_8041ec54;
    for (i = 1, data++; i < effect->count; i++, data++) {
        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'z', deg2rad * data->rotZ);
        PSMTXConcat(trans, rot, trans);
        PSMTXRotRad(rot, 'y', deg2rad * data->rotY);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(viewMtx, trans, trans);
        GXLoadPosMtxImm(trans, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        switch (type) {
        case 0:
            GXSetArray(GX_VA_POS, size12x12_tex64x64_brown_vtx, 0xE);
            GXSetArray(GX_VA_CLR0, size12x12_tex64x64_brown_vtx + 0xA, 0xE);
            GXSetArray(GX_VA_TEX0, size12x12_tex64x64_brown_vtx + 6, 0xE);
            break;
        case 1:
            GXSetArray(GX_VA_POS, size12x12_tex64x64_red_vtx, 0xE);
            GXSetArray(GX_VA_CLR0, size12x12_tex64x64_red_vtx + 0xA, 0xE);
            GXSetArray(GX_VA_TEX0, size12x12_tex64x64_red_vtx + 6, 0xE);
            break;
        }
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 12);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        tri2(6, 5, 4, 4, 7, 6, 4, 4);
    }
}

// TODO: type
u8 size12x12_tex64x64_brown_vtx[112] = {
    0xFF, 0xFA, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x2A, 0x11, 0xFF,
    0x00, 0x06, 0xFF, 0xFA, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x19, 0x2A, 0x11, 0xFF,
    0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x68, 0x56, 0x10, 0xFF,
    0xFF, 0xFA, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x68, 0x56, 0x10, 0xFF,
    0xFF, 0xFA, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x18, 0x1B, 0x21, 0xFF,
    0x00, 0x06, 0xFF, 0xFA, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x18, 0x1B, 0x21, 0xFF,
    0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x30, 0x36, 0x25, 0xFF,
    0xFF, 0xFA, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x30, 0x36, 0x25, 0xFF,
};

// TODO: type
u8 size12x12_tex64x64_red_vtx[112] = {
    0xFF, 0xFA, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x15, 0x11, 0xFF,
    0x00, 0x06, 0xFF, 0xFA, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x32, 0x15, 0x11, 0xFF,
    0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x68, 0x2B, 0x10, 0xFF,
    0xFF, 0xFA, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x68, 0x2B, 0x10, 0xFF,
    0xFF, 0xFA, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x30, 0x0D, 0x21, 0xFF,
    0x00, 0x06, 0xFF, 0xFA, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x30, 0x0D, 0x21, 0xFF,
    0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x60, 0x1B, 0x25, 0xFF,
    0xFF, 0xFA, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x60, 0x1B, 0x25, 0xFF,
};
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 coolLeafTevColor = 0x14641400;
const f32 float_deg2rad_8041ec54 = 0.017453292f;
const f32 float_4_8041ec58 = 4.0f;
const f32 float_1_8041ec5c = 1.0f;
const f32 float_0_8041ec60 = 0.0f;
const f32 float_0p3_8041ec64 = 0.3f;
const f32 float_0p8_8041ec68 = 0.8f;
const f32 float_6p2832_8041ec6c = 6.2831855f;
const f32 float_2_8041ec70 = 2.0f;
const f32 float_360_8041ec74 = 360.0f;
const f32 float_neg0p05_8041ec78 = -0.05f;
const f32 float_0p2_8041ec7c = 0.2f;
const f32 float_0p94_8041ec80 = 0.94f;
const f32 float_0p95_8041ec84 = 0.95f;
const f32 float_0p05_8041ec88 = 0.05f;
const f32 float_0p92_8041ec8c = 0.92f;
const f32 float_10_8041ec90 = 10.0f;
const f32 float_0p5_8041ec94 = 0.5f;
const f32 float_30_8041ec98 = 30.0f;
const f32 float_neg200_8041ec9c = -200.0f;
const f32 float_0p4_8041eca0 = 0.4f;
const f32 float_0p98_8041eca4 = 0.98f;
const f32 float_0p8464_8041eca8 = 0.8464f;
const f32 float_0p9604_8041ecac = 0.96040004f;
const f32 float_0p1_8041ecb0 = 0.1f;
