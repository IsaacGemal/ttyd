/* Mahon = Wizzerd */

#include "eff/eff_mahorn2.h"
#include "driver/dispdrv.h"
#include "eff/eff_hit.h"
#include "memory.h"
#include "system.h"
#include <math.h>
#include <string.h>

#define TWO_PI 6.283185307179586f

// .sdata2 data referenced by effMahorn2Disp
// data externs that have no header declaration
extern u32 dat_80422598;
extern const f32 float_deg2rad_8042259c;
extern const f32 float_16_804225a0;
extern const f32 float_2_804225a4;
extern const f32 float_8_804225a8;
extern const f32 float_0p5_804225ac;
extern const f32 float_0p0625_804225b4;
extern const f32 float_3_804225b8;
extern const f32 float_6p2832_804225bc;
extern const f32 float_360_804225c0;
extern const f32 float_4_804225c4;
extern const f32 float_0p125_804225c8;
extern const f32 float_0p88_804225cc;
extern const f32 float_1_804225d0;
#ifdef MUST_MATCH
// MUST_MATCH: retail reloads float_0_804225b0 for every 0.0f FIFO write; a plain literal lets
// MWCC pool it into an anonymous constant, so the extern is volatile to force the same reloads.
extern const volatile f32 float_0_804225b0;
#else
extern const f32 float_0_804225b0;
#endif

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
        temp = TWO_PI * (f32)(i - 6);
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
// The (C - v) * k + v blend needs `t` as a named temp, or the 4.0f/0.125f loads land f0/f1 swapped.
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
            data->scale = (0.0f - data->scale) * float_0p0625_804225b4 + data->scale;
            break;
        case 2:
            if (data->intplStep < 90) {
                s32 step = data->intplStep + 1;

                data->intplStep = step;
                data->scale = intplGetValue(11, step, 45, float_0_804225b0, float_3_804225b8);
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
                angle = float_6p2832_804225bc * angleABf(position.x + data->position.x, position.y + data->position.y, target.x, target.y) / float_360_804225c0;
                data->position.x += hitDistance * (f32)sin(angle);
                data->position.y += hitDistance * (f32)cos(angle);
#ifdef MUST_MATCH
                {
                    f32 t = float_4_804225c4 - data->scale;
                    data->scale = t * float_0p125_804225c8 + data->scale;
                }
#else
                data->scale = (float_4_804225c4 - data->scale) * float_0p125_804225c8 + data->scale;
#endif
            }
            else {
                data->scale = (0.0f - data->scale) * float_0p125_804225c8 + data->scale;
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
                data->target.x *= float_0p88_804225cc;
                data->target.y *= float_0p88_804225cc;
                data->target.z *= float_0p88_804225cc;
                data->scale = float_1_804225d0;
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

// Remaining differences are compiler register allocation and symbol naming, not values:
// (1) the quad's -w and 0.5f temporaries land in f1/f2 swapped from retail (the four products
// and four stfs keep the same order; only the temp FPRs differ); (2) the type-3 -h/0.5f temps
// land in f0/f1 swapped (the fmuls f30 encoding is fixed by multiplying the constant first);
// (3) the two local array-init templates are objdiff-named @252/@253 instead of retail's
// dat_8030196c/84 (addresses and bytes are identical: they are a function-local aggregate init
// and MWCC gives its .rodata an anonymous @N label); (4) the type-3 2.0f literal pools into @324
// instead of float_2_804225a4 (sourcing it from the named constant rescues that load but shifts
// the whole f24-f31 allocation down one and scores lower).
#ifdef MUST_MATCH
// MUST_MATCH: retail builds the GXColor in a 0x8-based temp and copies it to the 0xc outgoing arg;
// assigning the field inline makes MWCC use 0xc for the temp and 0x8 for the outgoing copy.
static inline GXColor mahorn2ChanColor(u8 a) {
    GXColor c = *(GXColor*)&dat_80422598;
    c.a = a;
    return c;
}
#endif

// NONMATCHING (99.78%): all logic matches; the texture switch dispatch and GXColor stack copy are
// fixed, what remains is the quad/type-3 temp-FPR allocation and the anonymous .rodata symbols.
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
#ifdef MUST_MATCH
        color = mahorn2ChanColor((data->alpha * headerAlpha) / 255);
#else
        color = *(GXColor*)&dat_80422598;
        color.a = (data->alpha * headerAlpha) / 255;
#endif
        GXSetChanMatColor(4, color);

        PSMTXTrans(mtxFinal, data->position.x, data->position.y, data->position.z);
        PSMTXScale(mtxScale, data->scale * rootScale, data->scale * rootScale, data->scale * rootScale);
        PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
        PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8042259c);
        PSMTXConcat(mtxFinal, mtxRot, mtxFinal);
        PSMTXConcat(mtxRoot, mtxFinal, mtxFinal);
        PSMTXConcat(cam->view, mtxFinal, mtxFinal);
        GXLoadPosMtxImm(mtxFinal, 0);
        GXSetCurrentMtx(0);

        type = data->type;
        switch (type) {
        case 0:
            break;
        case 1:
            w = float_16_804225a0;
            texW = float_2_804225a4;
            h = w;
            texH = texW;
            break;
        case 2:
            w = float_16_804225a0;
            texW = float_2_804225a4;
            h = w;
            texH = texW;
            break;
        case 3:
            h = float_2_804225a4;
            w = float_16_804225a0;
            texW = h;
            texH = h;
            break;
        case 4:
            w = float_8_804225a8;
            texW = float_2_804225a4;
            h = w;
            texH = texW;
            break;
        }

        if (type != 3) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(-w * float_0p5_804225ac, h * float_0p5_804225ac, float_0_804225b0);
            GXTexCoord2f32(float_0_804225b0, float_0_804225b0);
            GXPosition3f32(w * float_0p5_804225ac, h * float_0p5_804225ac, float_0_804225b0);
            GXTexCoord2f32(texW, float_0_804225b0);
            GXPosition3f32(w * float_0p5_804225ac, -h * float_0p5_804225ac, float_0_804225b0);
            GXTexCoord2f32(texW, texH);
            GXPosition3f32(-w * float_0p5_804225ac, -h * float_0p5_804225ac, float_0_804225b0);
            GXTexCoord2f32(float_0_804225b0, texH);
        }
        else {
            top = h * float_0p5_804225ac;
#ifdef MUST_MATCH
            // MUST_MATCH: multiplying with the constant first keeps the fmuls operand encoding while the
            // allocator holds -h and 0.5f in the opposite FPRs from `-h * 0.5f`.
            bottom = float_0p5_804225ac * -h;
#else
            bottom = -h * float_0p5_804225ac;
#endif

            for (j = 0; j < 3; j++) {
                GXBegin(0x80, 0, 4);
                {
                    f32 widths[6] = { 0.0f, 2.0f, 2.0f, 12.0f, 12.0f, 14.0f };
                    f32 texs[6] = { 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f };

                    GXPosition3f32(widths[j * 2], top, float_0_804225b0);
                    GXTexCoord2f32(2.0f * texs[j * 2], float_0_804225b0);
                    GXPosition3f32(widths[j * 2 + 1], top, float_0_804225b0);
                    GXTexCoord2f32(2.0f * texs[j * 2 + 1], float_0_804225b0);
                    GXPosition3f32(widths[j * 2 + 1], bottom, float_0_804225b0);
                    GXTexCoord2f32(2.0f * texs[j * 2 + 1], texH);
                    GXPosition3f32(widths[j * 2], bottom, float_0_804225b0);
                    GXTexCoord2f32(2.0f * texs[j * 2], texH);
                }
            }
        }
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_8042259c = 0.017453292f;
const f32 float_16_804225a0 = 16.0f;
const f32 float_2_804225a4 = 2.0f;
const f32 float_8_804225a8 = 8.0f;
const f32 float_0p5_804225ac = 0.5f;
#ifdef MUST_MATCH
const volatile f32 float_0_804225b0 = 0.0f;
#else
const f32 float_0_804225b0 = 0.0f;
#endif
const f32 float_0p0625_804225b4 = 0.0625f;
const f32 float_3_804225b8 = 3.0f;
const f32 float_6p2832_804225bc = 6.2831855f;
const f32 float_360_804225c0 = 360.0f;
const f32 float_4_804225c4 = 4.0f;
const f32 float_0p125_804225c8 = 0.125f;
const f32 float_0p88_804225cc = 0.88f;
const f32 float_1_804225d0 = 1.0f;
