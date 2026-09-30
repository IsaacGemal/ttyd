#include "eff/n64/eff_kemuri9_n64.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 constants (named after their original addresses in the asm).
extern const GXColor dat_8041f478;
extern const f32 float_deg2rad_8041f47c;
extern const f32 float_1_8041f480;
extern const f32 float_0p0625_8041f484;
extern const f32 float_32_8041f490;
extern const f32 float_0p5_8041f494;
extern const f32 float_16_8041f48c;
extern const f32 float_0p1_8041f498;
extern const f32 float_0p04_8041f49c;
extern const f32 float_0p9_8041f4a0;
extern const f32 float_0_8041f488;
extern const f32 float_10_8041f4a4;
extern const f32 float_6p2832_8041f4a8;
extern const f32 float_360_8041f4ac;

// .rodata: the zero Vec, the int->float conversion double and the "Kemuri9N64" string are all
// compiler-generated pool entries (like effKemuri8's). Building them as compound/string literals
// instead of named consts is what makes them land at the original addresses (0x802fa998, 0x802fa9a8,
// 0x802fa9b0) and lets objdiff resolve the relocations.

typedef struct effKemuri9Particle {
    u8 pad_00[4];                                // 0x00
    f32 x;                                       // 0x04
    f32 y;                                       // 0x08
    f32 z;                                       // 0x0C
    f32 field_0x10;                              // 0x10
    f32 field_0x14;                              // 0x14
    f32 field_0x18;                              // 0x18
    u8 pad_1C[0x40 - 0x1C];                      // 0x1C
} effKemuri9Particle;

typedef struct effKemuri9Data {
    u16 type;      // 0x00
    u8 pad_02[2];  // 0x02
    f32 x;         // 0x04
    f32 y;         // 0x08
    f32 z;         // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 scale;      // 0x1C
    f32 targetScale; // 0x20
    s32 life;       // 0x24
    s32 age;        // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    s32 field_0x34; // 0x34
    s32 field_0x38; // 0x38
    s32 field_0x3C; // 0x3C
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
    data->field_0x2C = a2;
    data->field_0x30 = 0;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = float_10_8041f4a4;
    data->targetScale = a6;
    data->field_0x30 = 0;
    data->field_0x34 = 0;
    data->field_0x3C = a2;
    data->field_0x38 = 0;

    p = (effKemuri9Particle*)(data + 1);
    for (i = 1; i < count + 1; i++, p++) {
        p->x = float_0_8041f488;
        p->y = float_0_8041f488;
        p->z = float_0_8041f488;
        angle = float_6p2832_8041f4a8 * (a7 + (f32)(((i - 1) * 360) / count)) / float_360_8041f4ac;
        p->field_0x10 = sin(angle);
        p->field_0x14 = float_0_8041f488;
        p->field_0x18 = cos(angle);
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
    s32 field30;
    s32 field34;
    s32 i;

    data = effect->userdata;
    position = kemuri9InitializeVec(data);

    field30 = (s32)(float_16_8041f48c * (f32)data->field_0x38 / (f32)data->field_0x3C);
    field34 = (s32)(float_16_8041f48c * (f32)(data->field_0x38 + 1) / (f32)data->field_0x3C);
    data->field_0x38++;
    if (field34 > 15) {
        field34 = 15;
    }
    if (field30 > 15) {
        field30 = 15;
    }
    data->field_0x30 = field30;
    data->field_0x34 = field34;

    data->age++;
    data->life--;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    data->scale += (data->targetScale - data->scale) * float_0p1_8041f498;
    scale = (s32)data->scale;

    for (i = 1, data++; i < effect->count; i++, data++) {
        data->field_0x14 += float_0p04_8041f49c;
        data->field_0x14 *= float_0p9_8041f4a0;
        data->x = (f32)scale * data->field_0x10;
        data->y = data->y + data->field_0x14;
        data->z = (f32)scale * data->field_0x18;
    }

    dispEntry(4, dispCalcZ(&position), 2, effKemuri9Disp, effect);
}

// NONMATCHING (88.47%): setup, GX state and TEV sequence match; the loop's float register allocation
// differs. MWCC keeps the corner constants 0.0/1.0 live in f30/f29 (frame 0x150, three saved FPRs)
// where the target reloads them per vertex, and it CSEs the four `size * 0.5` corners into two.
void effKemuri9Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effKemuri9Data* data;
    effKemuri9Particle* p;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtxWork;
    Mtx mtxScale;
    Mtx mtxRot;
    Mtx mtxView;
    s32 field30;
    s32 field34;
    s32 type;
    s32 i;
    f32 size;

    cam = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    field30 = data->field_0x30;
    field34 = data->field_0x34;

    PSMTXTrans(mtxWork, data->x, data->y, data->z);
    PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041f47c);
    PSMTXScale(mtxScale, float_1_8041f480, float_1_8041f480, float_1_8041f480);
    PSMTXConcat(mtxWork, mtxRot, mtxWork);
    PSMTXConcat(mtxWork, mtxScale, mtxWork);
    PSMTXConcat(cam->view, mtxWork, mtxView);

    effGetTexObj(0x28, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(4, dat_8041f478);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);

    PSMTXTrans(mtxWork, (f32)field30 * float_0p0625_8041f484, float_0_8041f488, float_0_8041f488);
    PSMTXScale(mtxScale, float_0p0625_8041f484, float_1_8041f480, float_1_8041f480);
    PSMTXConcat(mtxWork, mtxScale, mtxWork);
    GXLoadTexMtxImm(mtxWork, 0x1E, 1);

    PSMTXTrans(mtxWork, (f32)field34 * float_0p0625_8041f484, float_0_8041f488, float_0_8041f488);
    PSMTXScale(mtxScale, float_0p0625_8041f484, float_1_8041f480, float_1_8041f480);
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

    p = (effKemuri9Particle*)(data + 1);
    for (i = 1; i < effect->count; i++, p++) {
        PSMTXTrans(mtxWork, p->x, p->y, p->z);
        PSMTXConcat(mtxView, mtxWork, mtxWork);
        GXLoadPosMtxImm(mtxWork, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x80, 0, 4);
        if (type != 0) {
            size = float_16_8041f48c;
        }
        else {
            size = float_32_8041f490;
        }
        GXPosition3f32(-size * float_0p5_8041f494, size * float_0p5_8041f494, float_0_8041f488);
        GXTexCoord2f32(float_0_8041f488, float_0_8041f488);
        GXPosition3f32(size * float_0p5_8041f494, size * float_0p5_8041f494, float_0_8041f488);
        GXTexCoord2f32(float_1_8041f480, float_0_8041f488);
        GXPosition3f32(size * float_0p5_8041f494, -size * float_0p5_8041f494, float_0_8041f488);
        GXTexCoord2f32(float_1_8041f480, float_1_8041f480);
        GXPosition3f32(-size * float_0p5_8041f494, -size * float_0p5_8041f494, float_0_8041f488);
        GXTexCoord2f32(float_0_8041f488, float_1_8041f480);
    }
}
