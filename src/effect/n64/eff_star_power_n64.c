#include "eff/n64/eff_star_power_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"

// The linker-visible symbols keep their original address names; these aliases give them the role
// the code uses them for.
#define TEV_COLOR_SCRATCH_3 unk_80423284
#define TEV_COLOR_SCRATCH_2 unk_80423280
#define TEV_COLOR_SCRATCH_1 unk_8042327c
#define TEV_COLOR_SCRATCH_0 unk_80423278
#define TEXCOORD_SCALE float_0p015625_8041fe28
#define ZERO_F32 float_0_8041fe2c
#define DEG_TO_RAD float_deg2rad_8041fe30
#define SPRITE_SCALE float_0p4_8041fe34
#define ROT_Y_FACTOR float_0p4953_8041fe38
#define ROT_X_RATE float_4_8041fe3c
#define ROT_Z_FACTOR float_0p2234_8041fe40
#define TAU float_6p2832_8041fe44
#define ALPHA_RATE float_7p1234_8041fe48
#define TURN_DEGREES float_360_8041fe4c
#define ALPHA_MIDPOINT float_128_8041fe50
#define COLOR_AMPLITUDE float_127_8041fe54
#define COLOR1_R_RATE float_1p231_8041fe58
#define COLOR1_MIDPOINT float_215_8041fe5c
#define COLOR1_G_RATE float_0p531_8041fe60
#define COLOR1_B_RATE float_3p231_8041fe64
#define COLOR2_R_RATE float_0p298_8041fe68
#define COLOR2_MIDPOINT float_188_8041fe6c
#define COLOR2_G_RATE float_0p831_8041fe70
#define COLOR2_B_RATE float_2p231_8041fe74
#define SCALE_RATE float_2p044_8041fe78
#define SCALE_MIDPOINT float_0p7_8041fe7c
#define SCALE_AMPLITUDE float_0p3_8041fe80
#define ZERO_VEC vec3_802fb718

// .sbss2 zero GXColor scratch objects (declared in reverse: placed in reverse of declaration order)
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor TEV_COLOR_SCRATCH_3;
__declspec(section ".sdata") GXColor TEV_COLOR_SCRATCH_2;
__declspec(section ".sdata") GXColor TEV_COLOR_SCRATCH_1;
__declspec(section ".sdata") GXColor TEV_COLOR_SCRATCH_0;

u8 testsrc_akari_v[560] = {
	0x00, 0x93, 0xFF, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
	0xFF, 0xFF, 0x00, 0x8F, 0xFF, 0xD2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x85, 0xFF, 0xD5, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x88, 0xFF, 0x45, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x6A,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0xFF, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0xEE, 0x00, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
	0xFF, 0xFF, 0x00, 0xDC, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x58, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x00, 0x71, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xE8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xA8, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xAE, 0x00, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x6D, 0xFF, 0x36, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x78, 0xFF, 0x45, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x00, 0x4D,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x24,
	0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x71, 0xFF, 0xD2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x7B, 0xFF, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0xA8, 0xFF, 0x19, 0x00, 0x00, 0x06, 0x7B,
	0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xA3, 0xFF, 0xCB, 0x00, 0x00,
	0x06, 0x69, 0x02, 0xBA, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x6B, 0xFF, 0xDD,
	0x00, 0x00, 0x05, 0x94, 0x03, 0x02, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x6D,
	0xFF, 0x6A, 0x00, 0x00, 0x05, 0x9D, 0x01, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0xFF, 0x55, 0x00, 0x00, 0x04, 0x00, 0x00, 0xE8, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0xFF, 0x90, 0x00, 0x00, 0x04, 0x00, 0x01, 0xD0,
	0x00, 0x00, 0x00, 0xFF, 0x01, 0x0F, 0x00, 0x58, 0x00, 0x00, 0x08, 0x04,
	0x04, 0xE4, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xB0, 0x00, 0x39, 0x00, 0x00,
	0x06, 0x9D, 0x04, 0x6C, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x65, 0x00, 0x8A,
	0x00, 0x00, 0x05, 0x7D, 0x05, 0xAA, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x42,
	0x00, 0x5B, 0x00, 0x00, 0x04, 0xFA, 0x04, 0xEE, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xB9, 0x00, 0x00, 0x04, 0x00, 0x06, 0x64, 0xFF, 0xFF,
	0xFF, 0xFF, 0x00, 0x00, 0x01, 0x1D, 0x00, 0x00, 0x04, 0x00, 0x07, 0xEC,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9B, 0x00, 0x8A, 0x00, 0x00, 0x02, 0x83,
	0x05, 0xAA, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xBE, 0x00, 0x5B, 0x00, 0x00,
	0x03, 0x06, 0x04, 0xEE, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x58, 0xFF, 0x19,
	0x00, 0x00, 0x01, 0x85, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x93,
	0xFF, 0x6A, 0x00, 0x00, 0x02, 0x63, 0x01, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFE, 0xF1, 0x00, 0x58, 0x00, 0x00, 0xFF, 0xFC, 0x04, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x50, 0x00, 0x39, 0x00, 0x00, 0x01, 0x63, 0x04, 0x6C,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5D, 0xFF, 0xCB, 0x00, 0x00, 0x01, 0x97,
	0x02, 0xBA, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x95, 0xFF, 0xDD, 0x00, 0x00,
	0x02, 0x6C, 0x03, 0x02, 0x00, 0x00, 0x00, 0xFF,
};

extern const f32 ZERO_F32;
extern const f32 TEXCOORD_SCALE;
extern const f32 DEG_TO_RAD;
extern const f32 SPRITE_SCALE;

// .data: retail vertex array for this unit (two effVtxArray entries, 0x118 bytes apart).
extern u8 testsrc_akari_v[];
// .sbss2 scratch GXColors the original copies its TEV colours into before overwriting every byte.
extern const f32 ROT_X_RATE;
extern const f32 ROT_Y_FACTOR;
extern const f32 ROT_Z_FACTOR;
extern const f32 TAU;
extern const f32 ALPHA_RATE;
extern const f32 TURN_DEGREES;
extern const f32 ALPHA_MIDPOINT;
extern const f32 COLOR_AMPLITUDE;
extern const f32 COLOR1_R_RATE;
extern const f32 COLOR1_MIDPOINT;
extern const f32 COLOR1_G_RATE;
extern const f32 COLOR1_B_RATE;
extern const f32 COLOR2_R_RATE;
extern const f32 COLOR2_MIDPOINT;
extern const f32 COLOR2_G_RATE;
extern const f32 COLOR2_B_RATE;
extern const f32 SCALE_RATE;
extern const f32 SCALE_MIDPOINT;
extern const f32 SCALE_AMPLITUDE;

#ifdef MUST_MATCH
// Non-const and forced into .rodata: with a plain `const` MWCC hoists two of the three
// word loads into the prologue saves and the schedule no longer matches.
__declspec(section ".rodata") Vec ZERO_VEC = { 0.0f, 0.0f, 0.0f };
#else
const Vec ZERO_VEC = { 0.0f, 0.0f, 0.0f };
#endif
// Inline copy of the file-local helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
    Vec temp = ZERO_VEC;
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
    data->field_0x3C = ZERO_F32;
    data->field_0x40 = ZERO_F32;
    data->field_0x44 = ZERO_F32;
    return effect;
}

// The type==1 block's FP register allocation (retail keeps the counter fold in f2,
// ROT_Y_FACTOR in f3, the ROT_X_RATE*counter product in f7 and the counter*4 conversion in f4)
// only comes out when the product assignment is folded into the field_0x40 multiply's first
// operand; assigning f7 on its own line rotates them to f3/f2/f4/f3. The pool double is placed
// after ZERO_VEC only because "StarPowerN64" is written as a literal (a named const char[]
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
        // 0x38/0x3C/0x40/0x44 are the animated scale and x/y/z angles; 0x48..0x54 keep
        // float copies of them for the first draw pass.
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
            data->field_0x40 = ROT_Y_FACTOR * (f7 = ROT_X_RATE * (f32)counter);
            data->field_0x44 = ROT_Z_FACTOR * f7;
#else
            f32 f7;
            data->field_0x3C = (f32)(counter * 4);
            f7 = ROT_X_RATE * (f32)counter;
            data->field_0x40 = ROT_Y_FACTOR * f7;
            data->field_0x44 = ROT_Z_FACTOR * f7;
#endif
            data->color1A = 0xFF;
            data->color2A = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (ALPHA_RATE * (f32)counter) / TURN_DEGREES) + ALPHA_MIDPOINT);
            data->color1R = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (COLOR1_R_RATE * (f32)counter) / TURN_DEGREES) + COLOR1_MIDPOINT);
            data->color1G = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (COLOR1_G_RATE * (f32)counter) / TURN_DEGREES) + COLOR1_MIDPOINT);
            data->color1B = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (COLOR1_B_RATE * (f32)counter) / TURN_DEGREES) + COLOR1_MIDPOINT);
            data->color2R = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (COLOR2_R_RATE * (f32)counter) / TURN_DEGREES) + COLOR2_MIDPOINT);
            data->color2G = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (COLOR2_G_RATE * (f32)counter) / TURN_DEGREES) + COLOR2_MIDPOINT);
            data->field_0x30 = (u8)(COLOR_AMPLITUDE * (f32)sin(TAU * (COLOR2_B_RATE * (f32)counter) / TURN_DEGREES) + COLOR2_MIDPOINT);
            data->field_0x38 = SCALE_AMPLITUDE * (f32)sin(TAU * (SCALE_RATE * (f32)counter) / TURN_DEGREES) + SCALE_MIDPOINT;
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
    PSMTXScale(scale, TEXCOORD_SCALE, TEXCOORD_SCALE, ZERO_F32);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    effGetTexObjN64(0x49, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);

    if (color2A != 0) {
        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'y', DEG_TO_RAD * -camGetPtr(CAMERA_3D)->viewYaw);
        scaleVal = SPRITE_SCALE * *(f32*)&data->field_0x54;
        PSMTXScale(scale, scaleVal, scaleVal, scaleVal);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(trans, scale, trans);
        PSMTXRotRad(rot, 'x', DEG_TO_RAD * *(f32*)&data->field_0x48);
        PSMTXConcat(trans, rot, trans);
        PSMTXRotRad(rot, 'y', DEG_TO_RAD * *(f32*)&data->field_0x4C);
        PSMTXConcat(trans, rot, trans);
        PSMTXRotRad(rot, 'z', DEG_TO_RAD * *(f32*)&data->field_0x50);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(cam->view, trans, work);
        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        halfAlpha = color2A >> 1;
#ifdef MUST_MATCH
        GXSetTevColor(1, starPowerColor(TEV_COLOR_SCRATCH_0, color1R, color1G, color1B, halfAlpha));
        GXSetTevColor(2, starPowerColor(TEV_COLOR_SCRATCH_1, color2R, color2G, color2B, halfAlpha));
#else
        color = TEV_COLOR_SCRATCH_0;
        color.r = color1R;
        color.g = color1G;
        color.b = color1B;
        color.a = halfAlpha;
        GXSetTevColor(1, color);
        color = TEV_COLOR_SCRATCH_1;
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
    PSMTXRotRad(rot, 'y', DEG_TO_RAD * -camGetPtr(CAMERA_3D)->viewYaw);
    scaleVal = SPRITE_SCALE * data->field_0x38;
    PSMTXScale(scale, scaleVal, scaleVal, scaleVal);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(trans, scale, trans);
    PSMTXRotRad(rot, 'x', DEG_TO_RAD * data->field_0x3C);
    PSMTXConcat(trans, rot, trans);
    PSMTXRotRad(rot, 'y', DEG_TO_RAD * data->field_0x40);
    PSMTXConcat(trans, rot, trans);
    PSMTXRotRad(rot, 'z', DEG_TO_RAD * data->field_0x44);
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
    GXSetTevColor(1, starPowerColor(TEV_COLOR_SCRATCH_2, color1R, color1G, color1B, color1A));
    GXSetTevColor(2, starPowerColor(TEV_COLOR_SCRATCH_3, color2R, color2G, color2B, color1A));
#else
    color = TEV_COLOR_SCRATCH_2;
    color.r = color1R;
    color.g = color1G;
    color.b = color1B;
    color.a = color1A;
    GXSetTevColor(1, color);
    color = TEV_COLOR_SCRATCH_3;
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
const f32 TEXCOORD_SCALE = 0.015625f;
const f32 ZERO_F32 = 0.0f;
const f32 DEG_TO_RAD = 0.017453292f;
const f32 SPRITE_SCALE = 0.4f;
const f32 ROT_Y_FACTOR = 0.4953f;
const f32 ROT_X_RATE = 4.0f;
const f32 ROT_Z_FACTOR = 0.2234f;
const f32 TAU = 6.2831855f;
const f32 ALPHA_RATE = 7.12343f;
const f32 TURN_DEGREES = 360.0f;
const f32 ALPHA_MIDPOINT = 128.0f;
const f32 COLOR_AMPLITUDE = 127.0f;
const f32 COLOR1_R_RATE = 1.231f;
const f32 COLOR1_MIDPOINT = 215.0f;
const f32 COLOR1_G_RATE = 0.531f;
const f32 COLOR1_B_RATE = 3.231f;
const f32 COLOR2_R_RATE = 0.298f;
const f32 COLOR2_MIDPOINT = 188.0f;
const f32 COLOR2_G_RATE = 0.831f;
const f32 COLOR2_B_RATE = 2.231f;
const f32 SCALE_RATE = 2.044f;
const f32 SCALE_MIDPOINT = 0.7f;
const f32 SCALE_AMPLITUDE = 0.3f;
