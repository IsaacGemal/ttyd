#include "eff/n64/eff_kemuri9_n64.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata: the zero Vec, the int->float conversion double and the "Kemuri9N64" string are all
// compiler-generated pool entries (like effKemuri8's). Building them as compound/string literals
// instead of named consts is what makes them land at the original addresses (0x802fa998, 0x802fa9a8,
// 0x802fa9b0) and lets objdiff resolve the relocations.

typedef struct effKemuri9Particle {
    u8 pad_00[4];                                // 0x00
    f32 x;                                       // 0x04
    f32 y;                                       // 0x08
    f32 z;                                       // 0x0C
    f32 dirX;                                    // 0x10
    f32 yVelocity;                               // 0x14
    f32 dirZ;                                    // 0x18
    u8 pad_1C[0x40 - 0x1C];                      // 0x1C
} effKemuri9Particle;

typedef struct effKemuri9Data {
    u16 type;      // 0x00
    u8 pad_02[2];  // 0x02
    f32 x;         // 0x04
    f32 y;         // 0x08
    f32 z;         // 0x0C
    f32 dirX;      // 0x10
    f32 yVelocity; // 0x14
    f32 dirZ;      // 0x18
    f32 scale;      // 0x1C
    f32 targetScale; // 0x20
    s32 life;       // 0x24
    s32 age;        // 0x28
    s32 startLife;   // 0x2C
    s32 texFrame;    // 0x30
    s32 nextTexFrame; // 0x34
    s32 frame;       // 0x38
    s32 lifetime;    // 0x3C
} effKemuri9Data;

EffectEntry* effKemuri9N64Entry(s32 type, s32 count, s32 a2, f32 x, f32 y, f32 z, f32 a6, f32 a7) {
    EffectEntry* effect;
    effKemuri9Data* data;
    effKemuri9Particle* p;
    s32 i;
    f32 angle;

    effect = effEntry();
    effect->type = "Kemuri9N64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x40);
    effect->userdata = data;
    effect->callback = effKemuri9Main;

    data->life = a2;
    data->age = 0;
    data->startLife = a2;
    data->texFrame = 0;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = 10.f;
    data->targetScale = a6;
    data->texFrame = 0;
    data->nextTexFrame = 0;
    data->lifetime = a2;
    data->frame = 0;

    p = (effKemuri9Particle*)(data + 1);
    for (i = 1; i < count + 1; i++, p++) {
        p->x = 0.0f;
        p->y = 0.0f;
        p->z = 0.0f;
        angle = 6.2831855f * (a7 + (f32)(((i - 1) * 360) / count)) / 360.f;
        p->dirX = sin(angle);
        p->yVelocity = 0.0f;
        p->dirZ = cos(angle);
    }
    return effect;
}

// Build the zero position with the current data's coordinates; the compound literal reproduces
// the original's zero-Vec pool entry (same shape as effKemuri10Main's initializeVec).
static inline Vec kemuri9InitializeVec(effKemuri9Data* data) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = data->x;
    temp.y = data->y;
    temp.z = data->z;
    return temp;
}

// Note: the loop advances the single `data` pointer (the header and a particle share the first
// 0x1C bytes), which is what lets MWCC keep it in r3; `scale += (target - scale) * 0.1` is the
// operand order that gives the original's f0/f1/f2 colouring.
void effKemuri9Main(EffectEntry* effect) {
    effKemuri9Data* data;
    Vec position;
    s32 scale;
    s32 texFrame;
    s32 nextTexFrame;
    s32 i;

    data = effect->userdata;
    position = kemuri9InitializeVec(data);

    texFrame = (s32)(16.0f * (f32)data->frame / (f32)data->lifetime);
    nextTexFrame = (s32)(16.0f * (f32)(data->frame + 1) / (f32)data->lifetime);
    data->frame++;
    if (nextTexFrame > 15) {
        nextTexFrame = 15;
    }
    if (texFrame > 15) {
        texFrame = 15;
    }
    data->texFrame = texFrame;
    data->nextTexFrame = nextTexFrame;

    data->age++;
    data->life--;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    data->scale += (data->targetScale - data->scale) * 0.1f;
    scale = (s32)data->scale;

    for (i = 1, data++; i < effect->count; i++, data++) {
        data->yVelocity += 0.04f;
        data->yVelocity *= 0.9f;
        data->x = (f32)scale * data->dirX;
        data->y = data->y + data->yVelocity;
        data->z = (f32)scale * data->dirZ;
    }

    dispEntry(4, dispCalcZ(&position), 2, effKemuri9Disp, effect);
}

// The quad's two extents `size2` (x) and `size` (y) are separate locals set to the same value.
void effKemuri9Disp(CameraId camId, void* param) {
	f32 half = 0.5f;
    EffectEntry* effect = param;
    effKemuri9Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtxWork;
    Mtx mtxScale;
    Mtx mtxRot;
    Mtx mtxView;
    s32 texFrame;
    s32 type;
    s32 i;
    f32 size;
    f32 size2;
    s32 nextTexFrame;

    cam = camGetPtr(camId);
    data = effect->userdata;
    texFrame = data->texFrame;
    nextTexFrame = data->nextTexFrame;
    type = data->type;

    PSMTXTrans(mtxWork, data->x, data->y, data->z);
    PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
    PSMTXScale(mtxScale, 1.0f, 1.0f, 1.0f);
    PSMTXConcat(mtxWork, mtxRot, mtxWork);
    PSMTXConcat(mtxWork, mtxScale, mtxWork);
    PSMTXConcat(cam->view, mtxWork, mtxView);

    effGetTexObj(0x28, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(4, (GXColor){ 0xE6, 0xDE, 0xDE, 0xFF });
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);

    PSMTXTrans(mtxWork, 0.0625f * (f32)texFrame, 0.0f, 0.0f);
    PSMTXScale(mtxScale, 0.0625f, 1.0f, 1.0f);
    PSMTXConcat(mtxWork, mtxScale, mtxWork);
    GXLoadTexMtxImm(mtxWork, 0x1E, 1);

    PSMTXTrans(mtxWork, 0.0625f * (f32)nextTexFrame, 0.0f, 0.0f);
    PSMTXScale(mtxScale, 0.0625f, 1.0f, 1.0f);
    PSMTXConcat(mtxWork, mtxScale, mtxWork);
    GXLoadTexMtxImm(mtxWork, 0x21, 1);

    GXSetNumTevStages(3);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0x8);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetTevOrder(1, 1, 0, 0xFF);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 0, 8, 2, 0xF);
    GXSetTevAlphaIn(1, 0, 4, 1, 7);
    GXSetTevOrder(2, 0xFF, 0xFF, 4);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevColorIn(2, 0xF, 0, 0xA, 0xF);
    GXSetTevAlphaIn(2, 7, 0, 5, 7);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 13, 1, 4, 0);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        PSMTXTrans(mtxWork, data->x, data->y, data->z);
        PSMTXConcat(mtxView, mtxWork, mtxWork);
        GXLoadPosMtxImm(mtxWork, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x80, 0, 4);
        if (type != 0) {
            size2 = size = 16.0f;
        }
        else {
            size2 = size = 32.0f;
        }
        GXPosition3f32(-size2 * half, size * half, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(size2 * half, size * half, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(size2 * half, -size * half, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-size2 * half, -size * half, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}

