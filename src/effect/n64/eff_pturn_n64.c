#include "eff/n64/eff_pturn_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include "driver/dispdrv.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803A4790[2] = { 0xBF800000, 0x3F800000 };

// .rodata and .sdata2 constants (named after their original addresses in the asm).
f32 scale_dat[9] = { 5.0f, 4.5f, 4.0f, 3.0f, 2.0f, 1.0f, 0.7f, 1.2f, 1.0f };

// .rodata: zero vector copied into the local position before the particle fields are set
const Vec vec3_802fb450 = { 0.0f, 0.0f, 0.0f };

// .sbss2 zero GXColor scratch objects (declared in reverse: placed in reverse of declaration order).
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_8042322c;
__declspec(section ".sdata") GXColor unk_80423228;

// .data anchors ahead of the vertex arrays (compiler-generated in the original; kept so the
// section bytes between lbl_803A4790 and the vertex arrays match).
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803A47A0[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_803A47B0[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803A47C0[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };

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

EffectEntry* effPturnN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effPturnN64Data* data;

    effect = effEntry();
    effect->type = "PturnN64";
    effect->count = 2;
    data = __memAlloc(HEAP_EFFECT, 0x90);
    effect->userdata = data;
    effect->callback = effPturnMain;
    effect->flags |= 2;
    data->field_0x0 = type;
    data->field_0x34 = 1.0f;
    if (type == 1) {
        data->field_0x38 = 1;
        data->field_0x3C = 0;
    } else {
        data->field_0x38 = 0;
        data->field_0x3C = a1;
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
    data->field_0x2C = 0;
    data->field_0x44 = 0;
    data->field_0x4 = 1;
    data->field_0x58 = 0.0f;
    data->field_0x54 = 0.0f;
    data->field_0x50 = 0.0f;
    data->field_0x68 = 0.0f;
    data->field_0x64 = 0.0f;
    data->field_0x60 = 0.0f;
    data->field_0x5C = 0.0f;
    data->field_0x88 = 0.0f;
    data->field_0x70 = 0;
    data->field_0x6C = 0xFF;
    return effect;
}

void effPturnMain(EffectEntry* effect) {
    s32 life25;
    PturnN64Elem* data;
    s32 i;
    s32 life;
    s32 type;
    s32 counter;
    s32 field3C;
    s32 field38;
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
    disp = vec3_802fb450;
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
    data->field_0x2C++;
    if (data->field_0x2C > 0x4F1A0) {
        data->field_0x2C = 0x100;
    }

    type = data->field_0x0;
    life = data->life;
    counter = data->field_0x2C;
    field3C = data->field_0x3C;
    field38 = data->field_0x38;
    scaleFactor = data->field_0x34;
    if (type == 0) {
        if (data->field_0x44 > 0) {
            data->field_0x44--;
        } else if (field3C != field38) {
            if (field3C < 0) {
                field3C = 0;
            }
            if (field3C > field38) {
                data->life = (type == 0) ? 100 : 60;
                data->field_0x2C = 1;
                data->field_0x38++;
                data[1].life = 0;
            } else if (field3C < field38) {
                effPturnN64Entry(10, 0, data->x, data->y + data[1].field_0x20, data->z);
                data->field_0x38--;
            }
            data->field_0x44 = 8;
        }
    }

    f31 = 6.2831855f * (f32)(counter * 12);
    f30 = 0.05f * (f32)life;
    life25 = life * 25;

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        s32 elemLife = data->life;
        f32 scale;

        scale = (elemLife >= 9 || type == 10) ? 1.0f : scale_dat[elemLife];
        data->field_0x30 = scale;
        data->field_0x30 *= scaleFactor;
        data->field_0x20 = (type == 0 || type == 1) ? 4.0f * (f32)cos(f31 / 360.f) : 0.0f;
        data->x += data->field_0x14;
        data->y += data->field_0x18;
        data->z += data->field_0x1C;
        if (life == 20) {
            data->field_0x18 = 4.0f;
            data->field_0x14 = -2.0f;
        }
        if (life < 20) {
            data->field_0x20 *= f30;
            data->field_0x18 += -0.5f;
            data->field_0x40 += 10.f;
        }
        if (life < 10) {
            data->field_0x24 = life25;
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
    effPturnN64Data* data = effect->userdata;

    if (data->field_0x4 == 0) {
        return;
    }
    pturn_main_dispp(camId, param);
    if (data->field_0x0 == 1) {
        return;
    }
    pturn_num_dispp(camId, param);
}

static inline void pturnMainSetColor(s32 a) {
    GXColor color = unk_80423228;
    color.a = a;
    GXSetTevColor(1, color);
}

static inline void pturnNumSetColor(s32 a) {
    GXColor color = unk_8042322c;
    color.a = a;
    GXSetTevColor(1, color);
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
    s32 field38;
    s32 type;

    cam = camGetPtr(camId);
    data = effect->userdata;
    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, work);
    PSMTXConcat(cam->view, work, work);

    i = 1;
    field38 = data->field_0x38;
    type = data->field_0x0;
    data++;
    for (; i < effect->count; i++, data++) {
        if (field38 > 0 || type == 10) {
            effGetTexObjN64(0x92, &texObj);
            GXLoadTexObj(&texObj, 0);
            PSMTXTrans(trans, data->x, data->y, data->z);
            PSMTXScale(scale, data->field_0x30, data->field_0x30, 1.0f);
            PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x40);
            PSMTXConcat(trans, scale, trans);
            PSMTXConcat(trans, rot, trans);
            PSMTXTrans(scale, 0.0f, data->field_0x20, 0.0f);
            PSMTXConcat(trans, scale, trans);
            PSMTXConcat(work, trans, trans);
            GXLoadPosMtxImm(trans, 0);
            GXSetCurrentMtx(0);
            pturnMainSetColor(data->field_0x24);
            if (data->field_0x24 == 0xff) {
                GXSetNumChans(1);
                GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
                GXSetNumTevStages(1);
                GXSetTevOrder(0, 0, 0, 4);
                GXSetTevColorOp(0, 0, 0, 0, 1, 0);
                GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
                GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
                GXSetTevAlphaIn(0, 7, 7, 7, 4);
            } else {
                GXSetNumChans(1);
                GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
                GXSetNumTevStages(1);
                GXSetTevOrder(0, 0, 0, 4);
                GXSetTevColorOp(0, 0, 0, 0, 1, 0);
                GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
                GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
                GXSetTevAlphaIn(0, 7, 1, 4, 7);
            }
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
            PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
            GXLoadTexMtxImm(scale, 0x1e, 1);
            GXSetCullMode(0);
            effSetVtxDescN64((effVtxArray*)size48x48_tex32x32_pturn_vtx);
            GXBegin(0x90, 0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
        }
    }
}

// The integer locals are declared in reverse live-range order on purpose: mwcc hands out its
// callee-saved registers in declaration order (highest first), so this pins
// offsetA=r30/offsetB=r29/offsetC=r28/data=r27/i=r26/field38=r25 exactly as the original.
// The vertex arrays are anchored on lbl_803A4790 so the three effSetVtxDescN64 offsets match.
void pturn_num_dispp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
#ifdef MUST_MATCH
    u8* vtxBase;
#endif
    s32 offsetA;
    s32 offsetB;
    s32 offsetC;
    PturnN64Elem* data;
    s32 i;
    s32 field38;

#ifdef MUST_MATCH
    vtxBase = (u8*)lbl_803A4790;
#endif
    cam = camGetPtr(camId);
    data = effect->userdata;
    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, work);
    PSMTXConcat(cam->view, work, work);

    field38 = data->field_0x38;
    offsetA = 0x160 - field38 * 32;
    offsetB = 0x160 - (field38 % 10) * 32;
    offsetC = 0x160 - (field38 / 10) * 32;
    i = 1;
    data++;
    for (; i < effect->count; i++, data++) {
        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXScale(scale, data->field_0x30, data->field_0x30, 1.0f);
        PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x40);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(trans, rot, trans);
        PSMTXTrans(scale, 0.0f, data->field_0x20, 0.0f);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(work, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        if (field38 > 0) {
            GXSetNumChans(1);
            GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
            GXSetNumTevStages(1);
            GXSetTevOrder(0, 0, 0, 4);
            GXSetTevColorOp(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
            GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
            GXSetTevAlphaIn(0, 7, 1, 4, 7);
            pturnNumSetColor(data->field_0x24);
            effGetTexObjN64(0x53, &texObj);
            GXLoadTexObj(&texObj, 0);
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
            if (field38 < 10) {
                PSMTXScale(scale, 0.03125f, 0.0026041667f, 0.0f);
                PSMTXTrans(trans, 0.0f, (f32)offsetA, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, 0x1e, 1);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0x78));
#else
                effSetVtxDescN64((effVtxArray*)size24x24_tex32x32_pturn_vtx);
#endif
                GXBegin(0x90, 0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
            } else {
                PSMTXScale(scale, 0.03125f, 0.0026041667f, 0.0f);
                PSMTXTrans(trans, 0.0f, (f32)offsetB, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, 0x1e, 1);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0xe8));
#else
                effSetVtxDescN64((effVtxArray*)size16x24_tex32x32_r_pturn_vtx);
#endif
                GXBegin(0x90, 0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                PSMTXScale(scale, 0.03125f, 0.0026041667f, 0.0f);
                PSMTXTrans(trans, 0.0f, (f32)offsetC, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, 0x1e, 1);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0xb0));
#else
                effSetVtxDescN64((effVtxArray*)size16x24_tex32x32_l_pturn_vtx);
#endif
                GXBegin(0x90, 0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
            }
        }
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
