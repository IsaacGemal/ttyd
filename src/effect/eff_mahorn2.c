/* Mahon = Wizzerd */

#include "eff/eff_mahorn2.h"
#include "driver/dispdrv.h"
#include "eff/eff_hit.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
#include <string.h>

#define TWO_PI 6.283185307179586f

// dat_80422598 is a GXColor template (not a float) and stays a named object so the code
// references it by name. It lands first in .sdata2.
const u32 dat_80422598 = 0x00DA0000;

// MUST_MATCH: retail's compiler addressed the three zero Vecs below off one base register
// (offset 0, 0xC, 0x18); MWCC here emits a separate base per `{ 0 }` literal, which changes
// every GPR in the function. Declaring them as one array reproduces the shared base.
#ifdef MUST_MATCH
static const Vec vec3_80301948[3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
#endif

EffectEntry *effMahorn2Entry(BOOL unused, f32 posX, f32 posY, f32 posZ, f32 dstX, f32 dstY, f32 dstZ, f32 scale, f32 radius) {
    EffectEntry *effect;
    effMahorn2Data *data;
    f32 temp;
    int i;
    
    effect = effEntry();
    effect->type = "Mahorn2";
    effect->count = 14;
    data = __memAlloc(HEAP_EFFECT, sizeof(effMahorn2Data) * 14);
    effect->userdata = data;
    effect->callback = effMahorn2Main;
    data->unused = unused;
    data->unused2 = 0;
    data->intplStep = 0;
    data->position.x = posX;
    data->position.y = posY;
    data->position.z = posZ;
    data->target.x = dstX;
    data->target.y = dstY;
    data->target.z = dstZ;
    data->scale = scale;
    data->hitDistance = radius;
    data->alpha = 255;
    data->type = 0;
    data->hasHit = FALSE;
    data++;
    
    for (i = 1; i < 4; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 1;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 6.0f;
        data->alpha = 0;
        data->counter = 15 * (i - 1);
    }
    
    for (i = 4; i < 5; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 2;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 153;
        data->counter = 15;
    }
    
    for (i = 5; i < 6; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 3;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 153;
        data->counter = 60;
    }
    
    for (i = 6; i < effect->count; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 4;
        data->position.x = dstX - posX;
        data->position.y = dstY - posY;
        data->position.z = dstZ - posZ;
        temp = 6.283184f * (f32)(i - 6);
        data->target.x = 6.0f * (f32)sin(temp / (f32)(effect->count - 6));
        data->target.y = 6.0f * (f32)cos(temp / (f32)(effect->count - 6));
        data->target.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 255;
        data->counter = 0;
    }
    
    return effect;
}

#ifdef MUST_MATCH
static inline Vec mahorn2MakeDisp(effMahorn2Data* data) {
    Vec v = vec3_80301948[0];
    v.x = data->position.x;
    v.y = data->position.y;
    v.z = data->position.z;
    return v;
}

static inline Vec mahorn2MakeTarget(effMahorn2Data* data) {
    Vec v = vec3_80301948[1];
    v.x = data->target.x;
    v.y = data->target.y;
    v.z = data->target.z;
    return v;
}

static inline BOOL mahorn2BeyondHitDistance(effMahorn2Data* data, Vec* position, Vec* target, f32 hitDistance) {
    Vec v = vec3_80301948[2];
    v.x = position->x + data->position.x;
    v.y = position->y + data->position.y;
    v.z = position->z + data->position.z;
    return PSVECDistance(target, &v) > hitDistance;
}
#endif

// MUST_MATCH: the original has three Vec stack temps in the order disp 0x20, targetDisp 0x14,
// case-3 sum 0x8. Building them inline (the #else path) lets MWCC coalesce the copy temps and
// allocate the case-3 sum first (0x20); returning each from an inline makes them inlined temps
// allocated in source order, which reproduces both the stack slots and the scratch registers.
// The (C - v) * k + v blends need `k` (and `t` for the 4.0f - v form) as named locals: MWCC then
// loads the multiplier constant into f0 and the negation/4.0f-v temp into f1 in retail's order and
// pools 0.0625f, 4.0f, 0.125f at their original .sdata2 offsets (0x1c/0x2c/0x30). With bare literals
// the two loads land f0/f1 swapped and .sdata2 orders 0.125f before 4.0f.
void effMahorn2Main(EffectEntry* effect) {
    effMahorn2Data* data;
    Vec position;
    Vec target;
#ifdef MUST_MATCH
    Vec disp;
    Vec targetDisp;
#else
    Vec disp = { 0 };
    Vec targetDisp = { 0 };
#endif
    f32 hitDistance;
    f32 scale;
    f32 angle;
    s32 i;

    data = effect->userdata;
#ifdef MUST_MATCH
    disp = mahorn2MakeDisp(data);
    position = disp;
    targetDisp = mahorn2MakeTarget(data);
    target = targetDisp;
#else
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;
    targetDisp.x = data->target.x;
    targetDisp.y = data->target.y;
    targetDisp.z = data->target.z;
    target = targetDisp;
#endif
    hitDistance = data->hitDistance;
    scale = data->scale;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->counter != 0) {
            data->counter--;
            continue;
        }
        switch (data->type) {
        case 0:
            break;
        case 1:
            data->alpha += 4;
            if (data->alpha > 255) {
                data->alpha = 255;
            }
            {
                f32 k = 0.0625f;
                data->scale = (0.0f - data->scale) * k + data->scale;
            }
            break;
        case 2:
            if (data->intplStep < 90) {
                s32 step = data->intplStep + 1;

                data->intplStep = step;
                data->scale = intplGetValue(11, step, 45, 0.0f, 3.0f);
            }
            break;
        case 3: {
#ifdef MUST_MATCH
            if (mahorn2BeyondHitDistance(data, &position, &target, hitDistance)) {
#else
            Vec v = { 0 };
            v.x = position.x + data->position.x;
            v.y = position.y + data->position.y;
            v.z = position.z + data->position.z;
            if (PSVECDistance(&target, &v) > hitDistance) {
#endif
                angle = 6.2831855f * angleABf(position.x + data->position.x, position.y + data->position.y, target.x, target.y) / 360.f;
                data->position.x += hitDistance * (f32)sin(angle);
                data->position.y += hitDistance * (f32)cos(angle);
                {
                    f32 k = 0.125f;
                    f32 t = 4.0f - data->scale;
                    data->scale = t * k + data->scale;
                }
            }
            else {
                {
                    f32 k = 0.125f;
                    data->scale = (0.0f - data->scale) * k + data->scale;
                }
                if (!((effMahorn2Data*)effect->userdata)->hasHit) {
                    ((effMahorn2Data*)effect->userdata)->hasHit = TRUE;
                    effHitEntry(0, 4, target.x, target.y, target.z, scale);
                }
            }
            break;
        }
        case 4:
            if (((effMahorn2Data*)effect->userdata)->hasHit) {
                data->position.x += data->target.x;
                data->position.y += data->target.y;
                data->position.z += data->target.z;
                data->target.x *= 0.88f;
                data->target.y *= 0.88f;
                data->target.z *= 0.88f;
                data->scale = 1.0f;
                data->alpha -= 4;
                if (data->alpha < 0) {
                    data->alpha = 0;
                    effDelete(effect);
                    return;
                }
            }
            break;
        default:
            break;
        }
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMahorn2Disp, effect);
}

// The only objdiff differences left are anonymous .rodata labels for the block-local widths/texs
// aggregate initialisers (ours @252/@253 vs retail's dat_8030196c/dat_80301984); the bytes and
// addresses are identical, so they resolve at link time.
// Retail builds the GXColor in a 0x8-based temp and copies it to the 0xc outgoing arg;
// assigning the field inline makes MWCC use 0xc for the temp and 0x8 for the outgoing copy.
static inline GXColor mahorn2ChanColor(u8 a) {
    GXColor c = *(const GXColor*)&dat_80422598;
    c.a = a;
    return c;
}

void effMahorn2Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effMahorn2Data* data = effect->userdata;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtxFinal;
    Mtx mtxRot;
    Mtx mtxScale;
    Mtx mtxRoot;
    GXColor color;
    f32 top;
    f32 bottom;
    s32 headerAlpha;
    f32 rootScale;
    f32 w;
    f32 h;
    f32 half;
    f32 texW;
    f32 texH;
    s32 type;
    s32 i;
    s32 j;

    cam = camGetPtr(camId);
    headerAlpha = data->alpha;
    rootScale = data->scale;
    PSMTXTrans(mtxRoot, data->position.x, data->position.y, data->position.z);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevAlphaIn(0, 7, 5, 4, 7);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->counter != 0) {
            continue;
        }
        switch (data->type) {
        case 0:
            break;
        case 1:
            effGetTexObj(0x8F, &texObj);
            GXLoadTexObj(&texObj, 0);
            break;
        case 2:
        case 3:
        case 4:
            effGetTexObj(0x90, &texObj);
            GXLoadTexObj(&texObj, 0);
            break;
        }
        color = mahorn2ChanColor((data->alpha * headerAlpha) / 255);
        GXSetChanMatColor(4, color);

        PSMTXTrans(mtxFinal, data->position.x, data->position.y, data->position.z);
        PSMTXScale(mtxScale, data->scale * rootScale, data->scale * rootScale, data->scale * rootScale);
        PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
        PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
        PSMTXConcat(mtxFinal, mtxRot, mtxFinal);
        PSMTXConcat(mtxRoot, mtxFinal, mtxFinal);
        PSMTXConcat(cam->view, mtxFinal, mtxFinal);
        GXLoadPosMtxImm(mtxFinal, 0);
        GXSetCurrentMtx(0);

        half = 0.5f;
        type = data->type;
        switch (type) {
        case 0:
            break;
        case 1:
            w = 16.0f;
            texW = 2.0f;
            h = w;
            texH = texW;
            break;
        case 2:
            w = 16.0f;
            texW = 2.0f;
            h = w;
            texH = texW;
            break;
        case 3:
            h = 2.0f;
            w = 16.0f;
            texW = h;
            texH = h;
            break;
        case 4:
            w = 8.0f;
            texW = 2.0f;
            h = w;
            texH = texW;
            break;
        }

        if (type != 3) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(-w * half, h * half, 0.0f);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(w * half, h * half, 0.0f);
            GXTexCoord2f32(texW, 0.0f);
            GXPosition3f32(w * half, -h * half, 0.0f);
            GXTexCoord2f32(texW, texH);
            GXPosition3f32(-w * half, -h * half, 0.0f);
            GXTexCoord2f32(0.0f, texH);
        }
        else {
            // A local for the multiplier keeps the compiler from shifting the whole f24-f31
            // allocation when the pool load is named; the multiply follows in the natural order.
            f32 two = 2.0f;
            bottom = -h * half;
            top = h * half;

            for (j = 0; j < 3; j++) {
                GXBegin(0x80, 0, 4);
                {
                    f32 widths[6] = { 0.0f, 2.0f, 2.0f, 12.0f, 12.0f, 14.0f };
                    f32 texs[6] = { 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f };

                    GXPosition3f32(widths[j * 2], top, 0.0f);
                    GXTexCoord2f32(two * texs[j * 2], 0.0f);
                    GXPosition3f32(widths[j * 2 + 1], top, 0.0f);
                    GXTexCoord2f32(two * texs[j * 2 + 1], 0.0f);
                    GXPosition3f32(widths[j * 2 + 1], bottom, 0.0f);
                    GXTexCoord2f32(two * texs[j * 2 + 1], texH);
                    GXPosition3f32(widths[j * 2], bottom, 0.0f);
                    GXTexCoord2f32(two * texs[j * 2], texH);
                }
            }
        }
    }
}
