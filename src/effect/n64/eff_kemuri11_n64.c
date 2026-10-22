#include "eff/n64/eff_kemuri11_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

// .sdata2 float constants (named after their original addresses).
extern const f32 float_deg2rad_8041f4e8;
extern const f32 float_0p3_8041f4ec;
extern const f32 float_0p015625_8041f4f0;
extern const f32 float_0p03125_8041f4f4;
extern const f32 float_1_8041f4f8;
extern const f32 float_0_8041f4fc;
extern const f32 float_0p25_8041f500;
extern const f32 float_0p125_8041f504;
extern const f32 float_0p8_8041f508;
extern const f32 float_0p94_8041f50c;
extern const f32 float_1p002_8041f540;
extern const f32 float_128_8041f544;

// .rodata constants (named after their original addresses).

typedef struct effKemuri11Data {
    u16 type;        // 0x0
    u8 pad_2[2];     // 0x2
    Vec position;    // 0x4: the puff's offset (the main puff is at the effect origin)
    f32 dirX;        // 0x10: 0.5 * -sin(rotZ), set when a child spawns; never read
    f32 dirZ;        // 0x14: 0.5 * cos(rotZ), same
    f32 field_0x18;  // 0x18
    f32 scaleX;      // 0x1C: puff scale, times 0.3 in the draw
    f32 scaleY;      // 0x20
    f32 scaleZ;      // 0x24
    s32 life;        // 0x28: frames left; deleted below 0
    s32 age;         // 0x2C: frames elapsed
    f32 rotY;        // 0x30: yaw (degrees)
    f32 rotZ;        // 0x34: tilt (degrees), starts at 0
    s32 alpha;       // 0x38: TEVREG0 alpha, decays every frame after 8
    f32 texScrollA;  // 0x3C: texture 0 scroll (grows by texSpeedA)
    f32 texSpeedA;   // 0x40: decays by 0.75..0.9 per frame
    f32 texScrollB;  // 0x44: texture 1 scroll (grows by texSpeedB)
    f32 texSpeedB;   // 0x48
    s32 delay;       // 0x4C: child frames before it appears (counts up to 0, then shows)
    u8 color[6];     // 0x50
    u8 pad_56[2];    // 0x56
} effKemuri11Data;

// Vertex data for the hammer smoke mesh. The original is a raw 0x118-byte
// .data object; keep it as words and pass its address to effSetVtxDescN64.
static u32 stg_thruhammer_v[0x118 / 4] = {
    0xFFDEFFFE, 0x001410AB, 0x0400FFFF, 0xFFFFFFCC, 0x00320000, 0x0F000800, 0xFFFFFF00,
    0xFFDEFFFE, 0xFFEC0D55, 0x0400FFFF, 0xFFFFFFCC, 0x0032FFE2, 0x0D550800, 0xFFFFFF00,
    0xFFE60032, 0xFFD30BAB, 0x0800FFFF, 0xFF000000, 0xFFFEFFD8, 0x0A000400, 0xFFFFFFFF,
    0x00000032, 0xFFC40A00, 0x0800FFFF, 0xFF00001A, 0x0032FFD3, 0x08550800, 0xFFFFFF00,
    0x0022FFFE, 0xFFEC06AB, 0x0400FFFF, 0xFFFFFFCC, 0x0032001E, 0x10AB0800, 0xFFFFFF00,
    0xFFE60032, 0x002D1255, 0x0800FFFF, 0xFF000000, 0xFFFE0028, 0x14000400, 0xFFFFFFFF,
    0x00000032, 0x003C1400, 0x0800FFFF, 0xFF00001A, 0x0032002D, 0x15AB0800, 0xFFFFFF00,
    0x00340032, 0xFFE206AB, 0x0800FFFF, 0xFF000034, 0x00320000, 0x05000800, 0xFFFFFF00,
    0x0022FFFE, 0x00140355, 0x0400FFFF, 0xFFFF0034, 0x0032001E, 0x03550800, 0xFFFFFF00,
    0x001A0032, 0x002D01AB, 0x0800FFFF, 0xFF000000, 0xFFFE0028, 0x00000400, 0xFFFFFFFF,
};


// The original tests type with a switch: cases 0/1 share the 2*(i-1) form,
// everything else (including negative types) takes the 3*(i-1) default.
EffectEntry* effKemuri11N64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effKemuri11Data* data;
    s32 count;
    s32 i;

    effect = effEntry();
    switch (type) {
    case 0:
        type = 3;
        break;
    case 1:
        type = 2;
        break;
    case 2:
        type = 4;
        break;
    case 4:
        type = 6;
        break;
    case 5:
        type = 7;
        break;
    default:
        type = 5;
        break;
    }

    switch (type) {
    case 0:
        count = 3;
        break;
    case 1:
        count = 1;
        break;
    case 2:
    case 5:
        count = 3;
        break;
    default:
        count = 2;
        break;
    }

    effect->type = "Kemuri11N64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri11Data) * (count + 1));
    effect->userdata = data;
    effect->callback = effKemuri11Main;
    data->life = type >= 2 ? 60 : 30;
    data->age = 0;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->alpha = 0xff;

    switch (type) {
    case 0:
    case 1:
        data->color[0] = 0x00;
        data->color[1] = 0xff;
        data->color[2] = 0x7a;
        data->color[3] = 0xf0;
        data->color[4] = 0xff;
        data->color[5] = 0xfa;
        break;
    case 2:
        data->color[0] = 0x7d;
        data->color[1] = 0x78;
        data->color[2] = 0x64;
        data->color[3] = 0xff;
        data->color[4] = 0xff;
        data->color[5] = 0xf0;
        break;
    case 3:
        data->color[0] = 0xdc;
        data->color[1] = 0xd2;
        data->color[2] = 0xc8;
        data->color[3] = 0xff;
        data->color[4] = 0xff;
        data->color[5] = 0xfa;
        break;
    case 5:
        data->color[0] = 0xe1;
        data->color[1] = 0xcc;
        data->color[2] = 0x5d;
        data->color[3] = 0xe8;
        data->color[4] = 0xe7;
        data->color[5] = 0xab;
        break;
    case 6:
        data->life = 50;
        data->color[0] = 0xd4;
        data->color[1] = 0xb7;
        data->color[2] = 0x05;
        data->color[3] = 0xc1;
        data->color[4] = 0xa3;
        data->color[5] = 0x09;
        break;
    case 7:
        data->life = 50;
        data->color[0] = 0x21;
        data->color[1] = 0x8c;
        data->color[2] = 0x8d;
        data->color[3] = 0x18;
        data->color[4] = 0xa5;
        data->color[5] = 0xa3;
        break;
    case 4:
    default:
        data->life = 50;
        data->color[0] = 0xd0;
        data->color[1] = 0x88;
        data->color[2] = 0x28;
        data->color[3] = 0xd8;
        data->color[4] = 0xa9;
        data->color[5] = 0x41;
        break;
    }

    for (i = 1, data++; i < count + 1; i++, data++) {
        switch (type) {
        case 0:
        case 1:
            data->delay = -1 - (i - 1) * 2;
            break;
        default:
            data->delay = -1 - (i - 1) * 3;
            break;
        }
        data->rotZ = float_0_8041f4fc;
        data->rotY = (f32)(i * 30);
    }

    if (type == 6) {
        effKemuri1N64Entry(5, x, y, z, float_0_8041f4fc);
    }
    else if (type == 7) {
        effKemuri1N64Entry(6, x, y, z, float_0_8041f4fc);
    }
    return effect;
}

// The original's zero-Vec temp comes from a compound literal, not the named
// vec3_802faa00: the literal is what keeps the pool load from being scheduled
// above the userdata load (same idiom as eff_vapor_n64's makeVec).
static inline Vec kemuri11MakeVec(effKemuri11Data* data) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = data->position.x;
    temp.y = data->position.y;
    temp.z = data->position.z;
    return temp;
}

static void main_dl(EffectEntry* effect, Mtx matrix);

void effKemuri11Main(EffectEntry* effect) {
    EffectEntry* e;
    effKemuri11Data* data;
    Vec position;
    s32 i;
    s32 type;
    s32 t;
    f32 v;

    e = effect;
    data = e->userdata;
    position = kemuri11MakeVec(data);
    data->age++;
    data->life--;
    if (data->life < 0) {
        effDelete(e);
        return;
    }

    type = data->type;
    t = data->age;
    switch (type) {
    case 0:
    case 1:
        if (t >= 8) {
            data->alpha = (f32)data->alpha * float_0p8_8041f508;
        }
        break;
    default:
        if (t >= 8) {
            data->alpha = (f32)data->alpha * float_0p94_8041f50c;
        }
        break;
    }

    data++;
    for (i = 1; i < e->count; i++, data++) {
        data->delay++;
        if (data->delay < 0) {
            continue;
        }
        if (data->delay == 0) {
            data->position.x = float_0_8041f4fc;
            data->position.y = float_0_8041f4fc;
            data->position.z = float_0_8041f4fc;
            v = (f32)sin(6.2831855f * data->rotZ / 360.0f);
            data->dirX = 0.5f * -v;
            v = (f32)cos(6.2831855f * data->rotZ / 360.0f);
            data->dirZ = 0.5f * v;
            data->field_0x18 = float_0_8041f4fc;
            data->texScrollA = 32.0f;
            data->texSpeedA = 32.0f;
            data->texSpeedB = 32.0f;
            data->scaleY = 8.0f - (f32)i;
            switch (type) {
            case 0:
            case 1:
                v = 0.5f + (f32)i;
                data->scaleX = v;
                data->scaleZ = 0.5f * v;
                break;
            case 2:
            case 5:
                data->scaleX = (f32)(i + 2);
                data->scaleZ = 0.5f * (f32)(i + 2);
                break;
            case 3:
                data->scaleX = (f32)(i + 3);
                data->scaleZ = 0.5f * (f32)(i + 3);
                data->texScrollA = 16.0f;
                data->texSpeedA = 16.0f;
                data->texSpeedB = 16.0f;
                break;
            default:
                v = 0.5f * (f32)(i + 3);
                data->scaleX = v;
                data->scaleZ = 0.5f * v;
                data->scaleY *= float_0p8_8041f508;
                data->texScrollA = 16.0f;
                if (type == 6 || type == 7) {
                    data->texSpeedA = 8.0f;
                    data->texSpeedB = 8.0f;
                }
                else {
                    data->texSpeedA = 16.0f;
                    data->texSpeedB = 16.0f;
                }
                break;
            }
            data->texScrollB = 32.0f;
        }
        data->texScrollA += data->texSpeedA;
        data->texScrollB += data->texSpeedB;
        switch (type) {
        case 0:
        case 1:
            data->texSpeedA *= 0.9f;
            break;
        case 2:
        case 5:
            data->texSpeedA *= 0.75f;
            data->texSpeedB *= 0.99f;
            break;
        default:
            if (type == 6 || type == 7) {
                data->texSpeedA *= 0.88f;
                data->texSpeedB *= 0.98f;
            }
            else {
                data->texSpeedA *= 0.75f;
                data->texSpeedB *= 0.95f;
            }
            break;
        }
        // Literal pool constants (not the named float_1p002/float_128 externs):
        // this is the form that gives the original's f1/f2/f0 register split.
        data->scaleX *= 1.002f;
        data->scaleZ *= 1.002f;
        if (data->texScrollA > 128.0f) {
            data->texScrollA = 128.0f;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri11Disp, e);
}

// The original compiles the two type==0/1 cases as a switch range check; a plain
// `type < 2 && type >= 0` emits the opposite branch sense. The four TEV colours
// are set through void inlines so each by-value copy lands in the original's
// interleaved stack slots, and the alpha is kept in one live local across main_dl.
void effKemuri11Disp(CameraId camId, void* param) {
    effKemuri11Data* data;
    CameraEntry* cam;
    GXTexObj texobj;
    Mtx m0, m1, m2;
    s32 c0, c1, c2, c3, c4, c5;
    s32 d0, d1, d2, d3, d4, d5;
    s32 type;
    s32 alpha;
    EffectEntry* effect;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    c0 = data->color[0];
    c1 = data->color[1];
    c2 = data->color[2];
    c3 = data->color[3];
    c4 = data->color[4];
    c5 = data->color[5];
    d0 = c0;
    d1 = c1;
    d2 = c2;
    d3 = c3;
    d4 = c4;
    d5 = c5;
    if (type > 1) {
        d0 = c0 >> 2;
        d1 = c1 >> 2;
        d2 = c2 >> 2;
        d3 = c3 >> 2;
        d4 = c4 >> 2;
        d5 = c5 >> 2;
    }

    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

    switch (type) {
    case 0:
    case 1:
        GXSetNumTevStages(2);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        effGetTexObjN64(0x23, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP0);
        effGetTexObjN64(0x24, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP1);
        break;
    default:
        GXSetNumTevStages(3);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
        GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        effGetTexObjN64(0x25, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP0);
        effGetTexObjN64(0x24, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP1);
        break;
    }

    effSetVtxDescN64(stg_thruhammer_v);
    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(m1, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041f4e8);
    PSMTXConcat(m0, m1, m2);
    PSMTXConcat(cam->view, m2, m2);

    alpha = data->alpha;
    GXSetTevColor(GX_TEVREG0, (GXColor){ d0, d1, d2, alpha });
    GXSetTevColor(GX_TEVREG1, (GXColor){ d3, d4, d5, 0 });
    GXSetCullMode(GX_CULL_BACK);
    main_dl(effect, m2);
    GXSetTevColor(GX_TEVREG0, (GXColor){ c0, c1, c2, alpha });
    GXSetTevColor(GX_TEVREG1, (GXColor){ c3, c4, c5, 0 });
    GXSetCullMode(GX_CULL_FRONT);
    main_dl(effect, m2);
}

static void main_dl(EffectEntry* effect, Mtx matrix) {
    effKemuri11Data* data;
    Mtx m0, m1, m2;
    s32 i;

    data = effect->userdata;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->delay < 0) {
            continue;
        }
        PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(m1, 'z', float_deg2rad_8041f4e8 * data->rotZ);
        PSMTXConcat(m0, m1, m0);
        PSMTXScale(m2, float_0p3_8041f4ec * data->scaleX, float_0p3_8041f4ec * data->scaleY, float_0p3_8041f4ec * data->scaleZ);
        PSMTXConcat(m0, m2, m0);
        PSMTXRotRad(m1, 'y', float_deg2rad_8041f4e8 * data->rotY);
        PSMTXConcat(m0, m1, m0);
        PSMTXConcat(matrix, m0, m0);
        GXLoadPosMtxImm(m0, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        PSMTXScale(m2, float_0p015625_8041f4f0, float_0p03125_8041f4f4, float_1_8041f4f8);
        PSMTXTrans(m0, float_0_8041f4fc, -data->texScrollA * float_0p25_8041f500, float_0_8041f4fc);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX0, GX_MTX2x4);
        PSMTXScale(m2, float_0p125_8041f504, float_0p125_8041f504, float_1_8041f4f8);
        PSMTXTrans(m0, float_0_8041f4fc, -data->texScrollB, float_0_8041f4fc);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x36);
        tri2(0, 1, 2, 0, 2, 3, 4, 0);
        tri2(2, 4, 5, 0, 1, 3, 2, 0);
        tri2(4, 6, 5, 0, 5, 6, 7, 0);
        tri2(5, 7, 8, 0, 0, 9, 1, 0);
        tri2(10, 9, 0, 0, 11, 10, 0, 0);
        tri2(11, 12, 10, 0, 13, 12, 11, 0);
        tri2(8, 14, 15, 0, 7, 14, 8, 0);
        tri2(8, 15, 16, 0, 15, 17, 16, 0);
        tri2(16, 17, 18, 0, 16, 18, 19, 0);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_8041f4e8 = 0.017453292f;
const f32 float_0p3_8041f4ec = 0.3f;
const f32 float_0p015625_8041f4f0 = 0.015625f;
const f32 float_0p03125_8041f4f4 = 0.03125f;
const f32 float_1_8041f4f8 = 1.0f;
const f32 float_0_8041f4fc = 0.0f;
const f32 float_0p25_8041f500 = 0.25f;
const f32 float_0p125_8041f504 = 0.125f;
const f32 float_0p8_8041f508 = 0.8f;
const f32 float_0p94_8041f50c = 0.94f;
