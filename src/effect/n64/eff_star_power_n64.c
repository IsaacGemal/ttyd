#include "eff/n64/eff_star_power_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423284;
__declspec(section ".sdata") GXColor unk_80423280;
__declspec(section ".sdata") GXColor unk_8042327c;
__declspec(section ".sdata") GXColor unk_80423278;



// .data: retail vertex array for this unit (two effVtxArray entries, 0x118 bytes apart).
extern u8 testsrc_akari_v[];
// .sbss2 scratch GXColors the original copies its TEV colours into before overwriting every byte.

#ifdef MUST_MATCH
// Non-const and forced into .rodata: with a plain `const` MWCC hoists two of the three
// word loads into the prologue saves and the schedule no longer matches.
__declspec(section ".rodata") Vec vec3_802fb718 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802fb718 = { 0.0f, 0.0f, 0.0f };
#endif
// Inline copy of the file-local helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
    Vec temp = vec3_802fb718;
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}
EffectEntry* effStarPowerN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    EffStarPowerData* data;

    effect = effEntry();
    effect->type = "StarPowerN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffStarPowerData));
    effect->userdata = data;
    effect->callback = effStarPowerMain;
    effect->flags |= 2;
    data->field_0x0 = 1;
    data->type = type;
    data->field_0x14 = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x38 = scale;
    data->color1R = 0xFF;
    data->color1G = 0xFF;
    data->color1B = 0x78;
    data->color2R = 0xFF;
    data->color2G = 0x78;
    data->field_0x30 = 0;
    data->color2A = 0xFF;
    data->color1A = 0xFF;
    data->field_0x3C = 0.0f;
    data->field_0x40 = 0.0f;
    data->field_0x44 = 0.0f;
    return effect;
}

// The type==1 block's FP register allocation (retail keeps the counter fold in f2,
// float_0p4953 in f3, the float_4*counter product in f7 and the counter*4 conversion in f4)
// only comes out when the product assignment is folded into the field_0x40 multiply's first
// operand; assigning f7 on its own line rotates them to f3/f2/f4/f3. The pool double is placed
// after vec3_802fb718 only because "StarPowerN64" is written as a literal (a named const char[]
// would be laid out before the compiler's pooled constants).
void effStarPowerMain(EffectEntry* effect) {
    EffStarPowerData* data = effect->userdata;
    Vec position = initializeVec((Vec*)&data->x);
    s32 type = data->type;
    s32 counter;
    s32 i;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->field_0x14++;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        counter = data->field_0x14;
        if (data->lifetime < 16) {
            i = data->lifetime * 16;
            if (data->color1A > i) {
                data->color1A = i;
            }
            if (data->color2A > i) {
                data->color2A = i;
            }
        }
        if (counter < 16) {
            i = counter * 16 + 15;
            if (data->color1A < i) {
                data->color1A = i;
            }
            if (data->color2A < i) {
                data->color2A = i;
            }
        }
        *(f32*)&data->field_0x48 = data->field_0x3C;
        *(f32*)&data->field_0x4C = data->field_0x40;
        *(f32*)&data->field_0x50 = data->field_0x44;
        *(f32*)&data->field_0x54 = data->field_0x38;
        if (type == 1) {
#ifdef MUST_MATCH
            /* MUST_MATCH: the embedded `f7 = ...` is allocator steering, not logic; the natural
               form under #else rotates the FPRs (fold f3, product f4, counter*4 f3). */
            f32 f7;
            data->field_0x3C = (f32)(counter * 4);
            data->field_0x40 = 0.4953f * (f7 = 4.0f * (f32)counter);
            data->field_0x44 = 0.2234f * f7;
#else
            f32 f7;
            data->field_0x3C = (f32)(counter * 4);
            f7 = 4.0f * (f32)counter;
            data->field_0x40 = 0.4953f * f7;
            data->field_0x44 = 0.2234f * f7;
#endif
            data->color1A = 0xFF;
            data->color2A = (u8)(127.0f * (f32)sin(6.2831855f * (7.12343f * (f32)counter) / 360.f) + 128.0f);
            data->color1R = (u8)(127.0f * (f32)sin(6.2831855f * (1.231f * (f32)counter) / 360.f) + 215.0f);
            data->color1G = (u8)(127.0f * (f32)sin(6.2831855f * (0.531f * (f32)counter) / 360.f) + 215.0f);
            data->color1B = (u8)(127.0f * (f32)sin(6.2831855f * (3.231f * (f32)counter) / 360.f) + 215.0f);
            data->color2R = (u8)(127.0f * (f32)sin(6.2831855f * (0.298f * (f32)counter) / 360.f) + 188.0f);
            data->color2G = (u8)(127.0f * (f32)sin(6.2831855f * (0.831f * (f32)counter) / 360.f) + 188.0f);
            data->field_0x30 = (u8)(127.0f * (f32)sin(6.2831855f * (2.231f * (f32)counter) / 360.f) + 188.0f);
            data->field_0x38 = 0.3f * (f32)sin(6.2831855f * (2.044f * (f32)counter) / 360.f) + 0.7f;
        }
        if (counter == 1) {
            *(f32*)&data->field_0x48 = data->field_0x3C;
            *(f32*)&data->field_0x4C = data->field_0x40;
            *(f32*)&data->field_0x50 = data->field_0x44;
            *(f32*)&data->field_0x54 = data->field_0x38;
        }
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effStarPowerDisp, effect);
    }
}

// The original builds each TEV colour in a local GXColor and copies it into the outgoing argument
// slot; returning the colour from an inline makes mwcc lay those two slots out the same way it did
// in the original (the same quirk eff_akari_charge_n64.c documents). MUST_MATCH only: the plain
// build under #else uses the natural local-shaped code.
#ifdef MUST_MATCH
static inline GXColor starPowerColor(GXColor c, s32 r, s32 g, s32 b, s32 a) {
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}
#endif
void effStarPowerDisp(CameraId camId, void* param) {
    s32 halfAlpha;
    EffStarPowerData* data;
    CameraEntry* cam;
    s32 color1R;
    s32 color1G;
    s32 color1B;
    s32 color1A;
    s32 color2R;
    s32 color2G;
    s32 color2B;
    s32 color2A;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    GXTexObj texObj;
#ifndef MUST_MATCH
    GXColor color;
#endif
    f32 scaleVal;

    cam = camGetPtr(camId);
    data = ((EffectEntry*)param)->userdata;
    color1R = data->color1R;
    color1G = data->color1G;
    color1B = data->color1B;
    color1A = data->color1A;
    color2R = data->color2R;
    color2G = data->color2G;
    color2B = data->field_0x30;
    color2A = data->color2A;
    if (color1R > 255) {
        color1R = 255;
    }
    if (color1G > 255) {
        color1G = 255;
    }
    if (color1B > 255) {
        color1B = 255;
    }
    if (color1A > 255) {
        color1A = 255;
    }
    if (color2R > 255) {
        color2R = 255;
    }
    if (color2G > 255) {
        color2G = 255;
    }
    if (color2B > 255) {
        color2B = 255;
    }
    if (color2A > 255) {
        color2A = 255;
    }
    if (color1R < 0) {
        color1R = 0;
    }
    if (color1G < 0) {
        color1G = 0;
    }
    if (color1B < 0) {
        color1B = 0;
    }
    if (color1A < 0) {
        color1A = 0;
    }
    if (color2R < 0) {
        color2R = 0;
    }
    if (color2G < 0) {
        color2G = 0;
    }
    if (color2B < 0) {
        color2B = 0;
    }
    if (color2A < 0) {
        color2A = 0;
    }

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scale, 0.015625f, 0.015625f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    effGetTexObjN64(0x49, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);

    if (color2A != 0) {
        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
        scaleVal = 0.4f * *(f32*)&data->field_0x54;
        PSMTXScale(scale, scaleVal, scaleVal, scaleVal);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(trans, scale, trans);
        PSMTXRotRad(rot, 'x', 0.017453292f * *(f32*)&data->field_0x48);
        PSMTXConcat(trans, rot, trans);
        PSMTXRotRad(rot, 'y', 0.017453292f * *(f32*)&data->field_0x4C);
        PSMTXConcat(trans, rot, trans);
        PSMTXRotRad(rot, 'z', 0.017453292f * *(f32*)&data->field_0x50);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(cam->view, trans, work);
        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        halfAlpha = color2A >> 1;
#ifdef MUST_MATCH
        GXSetTevColor(1, starPowerColor(unk_80423278, color1R, color1G, color1B, halfAlpha));
        GXSetTevColor(2, starPowerColor(unk_8042327c, color2R, color2G, color2B, halfAlpha));
#else
        color = unk_80423278;
        color.r = color1R;
        color.g = color1G;
        color.b = color1B;
        color.a = halfAlpha;
        GXSetTevColor(1, color);
        color = unk_8042327c;
        color.r = color2R;
        color.g = color2G;
        color.b = color2B;
        color.a = halfAlpha;
        GXSetTevColor(2, color);
#endif
        GXSetNumChans(1);
        GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 4, 2, 0xa, 0xf);
        GXSetTevAlphaIn(0, 7, 1, 4, 7);
        effSetVtxDescN64((effVtxArray*)(testsrc_akari_v + 0x118));
        GXBegin(0x90, 0, 0x3c);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        tri2(4, 0, 3, 0, 4, 3, 5, 0);
        tri2(1, 6, 7, 0, 1, 7, 2, 0);
        tri2(6, 8, 9, 0, 6, 9, 7, 0);
        tri2(10, 9, 8, 0, 10, 8, 11, 0);
        tri2(11, 12, 13, 0, 11, 13, 10, 0);
        tri2(14, 4, 5, 0, 14, 5, 15, 0);
        tri2(12, 16, 17, 0, 12, 17, 13, 0);
        tri2(16, 18, 19, 0, 16, 19, 17, 0);
        tri2(18, 14, 15, 0, 18, 15, 19, 0);
    }

    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    scaleVal = 0.4f * data->field_0x38;
    PSMTXScale(scale, scaleVal, scaleVal, scaleVal);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(trans, scale, trans);
    PSMTXRotRad(rot, 'x', 0.017453292f * data->field_0x3C);
    PSMTXConcat(trans, rot, trans);
    PSMTXRotRad(rot, 'y', 0.017453292f * data->field_0x40);
    PSMTXConcat(trans, rot, trans);
    PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x44);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(cam->view, trans, work);
    GXLoadPosMtxImm(work, 0);
    GXSetCurrentMtx(0);

    if (color2A != 0) {
        GXBegin(0x90, 0, 0x3c);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        tri2(4, 0, 3, 0, 4, 3, 5, 0);
        tri2(1, 6, 7, 0, 1, 7, 2, 0);
        tri2(6, 8, 9, 0, 6, 9, 7, 0);
        tri2(10, 9, 8, 0, 10, 8, 11, 0);
        tri2(11, 12, 13, 0, 11, 13, 10, 0);
        tri2(14, 4, 5, 0, 14, 5, 15, 0);
        tri2(12, 16, 17, 0, 12, 17, 13, 0);
        tri2(16, 18, 19, 0, 16, 19, 17, 0);
        tri2(18, 14, 15, 0, 18, 15, 19, 0);
    }

#ifdef MUST_MATCH
    GXSetTevColor(1, starPowerColor(unk_80423280, color1R, color1G, color1B, color1A));
    GXSetTevColor(2, starPowerColor(unk_80423284, color2R, color2G, color2B, color1A));
#else
    color = unk_80423280;
    color.r = color1R;
    color.g = color1G;
    color.b = color1B;
    color.a = color1A;
    GXSetTevColor(1, color);
    color = unk_80423284;
    color.r = color2R;
    color.g = color2G;
    color.b = color2B;
    color.a = color1A;
    GXSetTevColor(2, color);
#endif
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 4, 2, 0xa, 0xf);
    GXSetTevAlphaIn(0, 7, 7, 7, 1);
    effSetVtxDescN64((effVtxArray*)testsrc_akari_v);
    GXBegin(0x90, 0, 0x3c);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(4, 0, 3, 0, 4, 3, 5, 0);
    tri2(1, 6, 7, 0, 1, 7, 2, 0);
    tri2(6, 8, 9, 0, 6, 9, 7, 0);
    tri2(10, 9, 8, 0, 10, 8, 11, 0);
    tri2(11, 12, 13, 0, 11, 13, 10, 0);
    tri2(14, 4, 5, 0, 14, 5, 15, 0);
    tri2(12, 16, 17, 0, 12, 17, 13, 0);
    tri2(16, 18, 19, 0, 16, 19, 17, 0);
    tri2(18, 14, 15, 0, 18, 15, 19, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
