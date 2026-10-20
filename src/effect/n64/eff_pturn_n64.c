#include "matching.h"
#include "eff/n64/eff_pturn_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include "driver/dispdrv.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
extern const f32 float_deg2rad_8041fa60;
extern const f32 float_1_8041fa64;
extern const f32 float_0_8041fa68;
extern const f32 float_0p03125_8041fa6c;
extern const f32 float_0p0026042_8041fa70;
extern const f32 float_6p2832_8041fa74;
extern const f32 float_0p05_8041fa78;
extern const f32 float_360_8041fa7c;

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803A4790[2] = { 0xBF800000, 0x3F800000 };

// .rodata and .sdata2 constants (named after their original addresses in the asm).
f32 scale_dat[9] = { 5.0f, 4.5f, 4.0f, 3.0f, 2.0f, 1.0f, 0.7f, 1.2f, 1.0f };
#pragma explicit_zero_data on
#pragma force_active on
u32 gap_07_80413C9C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

// .sbss2 zero GXColor scratch objects (declared in reverse: placed in reverse of declaration order).
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_8042322c;
__declspec(section ".sdata") GXColor unk_80423228;
#pragma section ".sdata" ".sbss"

// .data anchors ahead of the vertex arrays (compiler-generated in the original; kept so the
// section bytes between lbl_803A4790 and the vertex arrays match).
#pragma force_active on
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803A47A0[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_803A47B0[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803A47C0[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
#pragma force_active reset

#pragma force_active on
// .data vertex arrays owned by this unit.
u32 size48x48_tex32x32_pturn_vtx[0x38 / 4] = {
    0xFFE8FFE8, 0x0000FE00, 0x0200FFFF, 0xFFFF0018,
    0xFFE80000, 0x0200FE00, 0xFFFFFFFF, 0x00180018,
    0x00000600, 0x0200FFFF, 0xFFFFFFE8, 0x00180000,
    0x02000600, 0xFFFFFFFF,
};
u32 size24x24_tex32x32_pturn_vtx[0x38 / 4] = {
    0xFFF4FFF4, 0x00000000, 0x0000FAFA, 0x00FF000C,
    0xFFF40000, 0x04000000, 0xFAFA00FF, 0x000C000C,
    0x00000400, 0x0400FAFA, 0x00FFFFF4, 0x000C0000,
    0x00000400, 0xFAFA00FF,
};
u32 size16x24_tex32x32_l_pturn_vtx[0x38 / 4] = {
    0xFFF0FFF4, 0x00000000, 0x0000FAFA, 0x00FF0004,
    0xFFF40000, 0x04000000, 0xFAFA00FF, 0x0004000C,
    0x00000400, 0x0400FAFA, 0x00FFFFF0, 0x000C0000,
    0x00000400, 0xFAFA00FF,
};
u32 size16x24_tex32x32_r_pturn_vtx[0x38 / 4] = {
    0xFFFCFFF4, 0x00000000, 0x0000FAFA, 0x00FF0010,
    0xFFF40000, 0x04000000, 0xFAFA00FF, 0x0010000C,
    0x00000400, 0x0400FAFA, 0x00FFFFFC, 0x000C0000,
    0x00000400, 0xFAFA00FF,
};
#pragma force_active reset

EffectEntry* effPturnN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    PturnN64Elem* data;

    effect = effEntry();
    effect->type = "PturnN64\0\0\0\0\0\0\0";
    effect->count = 2;
    data = __memAlloc(HEAP_EFFECT, 0x90);
    effect->userdata = data;
    effect->callback = effPturnMain;
    effect->flags |= 2;
    data->type = type;
    data->scaleFactor = float_1_8041fa64;
    if (type == 1) {
        data->displayNum = 1;
        data->targetNum = 0;
    } else {
        data->displayNum = 0;
        data->targetNum = a1;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    switch (type) {
    case 0:
        data->life = 100;
        break;
    case 1:
        data->life = 100;
        break;
    case 10:
        data->life = 0x15;
        break;
    }
    data->counter = 0;
    data->stepDelay = 0;
    data->active = 1;
    data[1].z = float_0_8041fa68;
    data[1].y = float_0_8041fa68;
    data[1].x = float_0_8041fa68;
    data[1].bobY = float_0_8041fa68;
    data[1].velZ = float_0_8041fa68;
    data[1].velY = float_0_8041fa68;
    data[1].velX = float_0_8041fa68;
    data[1].angle = float_0_8041fa68;
    data[1].life = 0;
    data[1].alpha = 0xFF;
    return effect;
}

void effPturnMain(EffectEntry* effect) {
    s32 life25;
    PturnN64Elem* data;
    s32 i;
    s32 life;
    s32 type;
    s32 counter;
    s32 targetNum;
    s32 displayNum;
    f32 f31;
    f32 f30;
    f32 scaleFactor;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as
       eff_bush_n64.c). */
    Vec position;
#endif
    Vec disp;

    data = effect->userdata;
    disp = (Vec){ 0.0f, 0.0f, 0.0f };
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 0x15;
    }
    if (data->life < 100) {
        data->life--;
    }
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    data->counter++;
    if (data->counter > 0x4F1A0) {
        data->counter = 0x100;
    }

    type = data->type;
    life = data->life;
    counter = data->counter;
    targetNum = data->targetNum;
    displayNum = data->displayNum;
    scaleFactor = data->scaleFactor;
    if (type == 0) {
        if (data->stepDelay > 0) {
            data->stepDelay--;
        } else if (targetNum != displayNum) {
            if (targetNum < 0) {
                targetNum = 0;
            }
            if (targetNum > displayNum) {
                data->life = (type == 0) ? 100 : 60;
                data->counter = 1;
                data->displayNum++;
                data[1].life = 0;
            } else if (targetNum < displayNum) {
                effPturnN64Entry(10, 0, data->x, data->y + data[1].bobY, data->z);
                data->displayNum--;
            }
            data->stepDelay = 8;
        }
    }

    f31 = float_6p2832_8041fa74 * (f32)(counter * 12);
    f30 = float_0p05_8041fa78 * (f32)life;
    life25 = life * 25;

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        s32 elemLife = data->life;
        f32 scale;

        scale = (elemLife >= 9 || type == 10) ? float_1_8041fa64 : scale_dat[elemLife];
        data->scale = scale;
        data->scale *= scaleFactor;
        data->bobY = (type == 0 || type == 1) ? 4.0f * (f32)cos(f31 / float_360_8041fa7c) : float_0_8041fa68;
        data->x += data->velX;
        data->y += data->velY;
        data->z += data->velZ;
        if (life == 20) {
            data->velY = 4.0f;
            data->velX = -2.0f;
        }
        if (life < 20) {
            data->bobY *= f30;
            data->velY += -0.5f;
            data->angle += 10.f;
        }
        if (life < 10) {
            data->alpha = life25;
        }
        data->life++;
    }

#ifdef MUST_MATCH
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPturnDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPturnDisp, effect);
#endif
}

void effPturnDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    PturnN64Elem* data = effect->userdata;

    if (data->active == 0) {
        return;
    }
    pturn_main_dispp(camId, param);
    if (data->type == 1) {
        return;
    }
    pturn_num_dispp(camId, param);
}

static inline void pturnMainSetColor(s32 a) {
    GXColor color = unk_80423228;
    color.a = a;
    GXSetTevColor(GX_TEVREG0, color);
}

static inline void pturnNumSetColor(s32 a) {
    GXColor color = unk_8042322c;
    color.a = a;
    GXSetTevColor(GX_TEVREG0, color);
}

void pturn_main_dispp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    PturnN64Elem* data;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    s32 i;
    s32 displayNum;
    s32 type;

    cam = camGetPtr(camId);
    data = effect->userdata;
    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041fa60 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, work);
    PSMTXConcat(cam->view, work, work);

    i = 1;
    displayNum = data->displayNum;
    type = data->type;
    data++;
    for (; i < effect->count; i++, data++) {
        if (displayNum > 0 || type == 10) {
            effGetTexObjN64(0x92, &texObj);
            GXLoadTexObj(&texObj, GX_TEXMAP0);
            PSMTXTrans(trans, data->x, data->y, data->z);
            PSMTXScale(scale, data->scale, data->scale, float_1_8041fa64);
            PSMTXRotRad(rot, 'z', float_deg2rad_8041fa60 * data->angle);
            PSMTXConcat(trans, scale, trans);
            PSMTXConcat(trans, rot, trans);
            PSMTXTrans(scale, float_0_8041fa68, data->bobY, float_0_8041fa68);
            PSMTXConcat(trans, scale, trans);
            PSMTXConcat(work, trans, trans);
            GXLoadPosMtxImm(trans, GX_PNMTX0);
            GXSetCurrentMtx(GX_PNMTX0);
            pturnMainSetColor(data->alpha);
            if (data->alpha == 0xFF) {
                GXSetNumChans(1);
                GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
                GXSetNumTevStages(1);
                GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
                GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
                GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
            } else {
                GXSetNumChans(1);
                GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
                GXSetNumTevStages(1);
                GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
                GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
                GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            }
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
            PSMTXScale(scale, float_0p03125_8041fa6c, float_0p03125_8041fa6c, float_0_8041fa68);
            GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
            GXSetCullMode(GX_CULL_NONE);
            effSetVtxDescN64((effVtxArray*)size48x48_tex32x32_pturn_vtx);
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
        }
    }
}

// The integer locals are declared in reverse live-range order on purpose: mwcc hands out its
// callee-saved registers in declaration order (highest first), so this pins
// offsetA=r30/offsetB=r29/offsetC=r28/data=r27/i=r26/displayNum=r25 exactly as the original.
void pturn_num_dispp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    s32 offsetA;
    s32 offsetB;
    s32 offsetC;
    PturnN64Elem* data;
    s32 i;
    s32 displayNum;

    cam = camGetPtr(camId);
    data = effect->userdata;
    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041fa60 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, work);
    PSMTXConcat(cam->view, work, work);

    displayNum = data->displayNum;
    offsetA = 0x160 - displayNum * 32;
    offsetB = 0x160 - (displayNum % 10) * 32;
    offsetC = 0x160 - (displayNum / 10) * 32;
    i = 1;
    data++;
    for (; i < effect->count; i++, data++) {
        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXScale(scale, data->scale, data->scale, float_1_8041fa64);
        PSMTXRotRad(rot, 'z', float_deg2rad_8041fa60 * data->angle);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(trans, rot, trans);
        { f32 zz = MATCH_F32(float_0_8041fa68); PSMTXTrans(scale, zz, data->bobY, zz); }
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(work, trans, trans);
        GXLoadPosMtxImm(trans, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        if (displayNum > 0) {
            GXSetNumChans(1);
            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
            GXSetNumTevStages(1);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            pturnNumSetColor(data->alpha);
            effGetTexObjN64(0x53, &texObj);
            GXLoadTexObj(&texObj, GX_TEXMAP0);
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
            if (displayNum < 10) {
                PSMTXScale(scale, float_0p03125_8041fa6c, float_0p0026042_8041fa70, MATCH_F32(float_0_8041fa68));
                { f32 z1 = MATCH_F32(float_0_8041fa68); PSMTXTrans(trans, z1, (f32)offsetA, z1); }
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                effSetVtxDescN64((effVtxArray*)size24x24_tex32x32_pturn_vtx);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
            } else {
                PSMTXScale(scale, float_0p03125_8041fa6c, float_0p0026042_8041fa70, MATCH_F32(float_0_8041fa68));
                { f32 z2 = MATCH_F32(float_0_8041fa68); PSMTXTrans(trans, z2, (f32)offsetB, z2); }
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                effSetVtxDescN64((effVtxArray*)size16x24_tex32x32_r_pturn_vtx);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                PSMTXScale(scale, float_0p03125_8041fa6c, float_0p0026042_8041fa70, MATCH_F32(float_0_8041fa68));
                { f32 z3 = MATCH_F32(float_0_8041fa68); PSMTXTrans(trans, z3, (f32)offsetC, z3); }
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                effSetVtxDescN64((effVtxArray*)size16x24_tex32x32_l_pturn_vtx);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
            }
        }
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_8041fa60 = 0.017453292f;
const f32 float_1_8041fa64 = 1.0f;
const f32 float_0_8041fa68 = 0.0f;
const f32 float_0p03125_8041fa6c = 0.03125f;
const f32 float_0p0026042_8041fa70 = 0.0026041667f;
const f32 float_6p2832_8041fa74 = 6.2831855f;
const f32 float_0p05_8041fa78 = 0.05f;
const f32 float_360_8041fa7c = 360.0f;
