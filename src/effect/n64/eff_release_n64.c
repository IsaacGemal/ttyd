#include "eff/n64/eff_release_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") f32 lbl_803A4C60[2] = { -1.0f, 1.0f };

// .rodata
// The zero Vec (vec3_802fb4f0), the two int->float magic doubles (double_to_int_802fb500,
// double_to_int_mask_802fb508) and the "ReleaseN64" string are compiler-generated here and land at
// the original's offsets; defining them by name as well only emitted duplicates after them.

// .data
#pragma force_active on
__declspec(section ".data") static f32 negone_one_374[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_803A4C70[2] = {1.0f, -1.0f};
__declspec(section ".data") static f32 one_negone_373[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 lbl_803A4C80[2] = {-1.0f, 1.0f};
__declspec(section ".data") static f32 negone_one_360[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_803A4C90[2] = {1.0f, -1.0f};
__declspec(section ".data") static f32 one_negone_359[2] = {1.0f, -1.0f};

static AnimeData anime_data[30] = {
	{0x0064, 0x00FF}, {0x00C8, 0x00FF}, {0x012C, 0x00FF}, {0x0190, 0x00FF}, {0x01F4, 0x00F5}, {0x0258, 0x00EB},
	{0x02BC, 0x00E1}, {0x0320, 0x00D7}, {0x0384, 0x00CD}, {0x03E8, 0x00C3}, {0x044C, 0x00B9}, {0x04B0, 0x00AF},
	{0x0514, 0x00A5}, {0x0578, 0x009B}, {0x05DC, 0x0091}, {0x0640, 0x0087}, {0x06A4, 0x007D}, {0x0708, 0x0073},
	{0x076C, 0x0069}, {0x07D0, 0x005F}, {0x0834, 0x0055}, {0x0898, 0x004B}, {0x08FC, 0x0041}, {0x0960, 0x0037},
	{0x09C4, 0x002D}, {0x0A28, 0x001E}, {0x0A8C, 0x0014}, {0x0AF0, 0x000A}, {0x0B54, 0x0005}, {0x0BB8, 0x0000},
};
AnimeData anime_data_2[30] = {
	{0x0BB8, 0x0000}, {0x0B54, 0x0005}, {0x0AF0, 0x000A}, {0x0A8C, 0x0014}, {0x0A28, 0x001E}, {0x09C4, 0x002D},
	{0x0960, 0x0037}, {0x08FC, 0x0041}, {0x0898, 0x004B}, {0x0834, 0x0055}, {0x07D0, 0x005F}, {0x076C, 0x0069},
	{0x0708, 0x0073}, {0x06A4, 0x007D}, {0x0640, 0x0087}, {0x05DC, 0x0091}, {0x0578, 0x009B}, {0x0514, 0x00A5},
	{0x04B0, 0x00AF}, {0x044C, 0x00B9}, {0x03E8, 0x00C3}, {0x0384, 0x00CD}, {0x0320, 0x00D7}, {0x02BC, 0x00E1},
	{0x0258, 0x00EB}, {0x01F4, 0x00F5}, {0x0190, 0x00FF}, {0x012C, 0x00FF}, {0x00C8, 0x00FF}, {0x0064, 0x00FF},
};

static u8 size16x16_tex64x64_vtx[] = {
	0xFF, 0xF8, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xF8, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x08,
	0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 size16x16_tex256x32_vtx[] = {
	0xFF, 0xF8, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xF8, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFA,
	0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFA, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFA, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFC, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xF8, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xFC, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFE,
	0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFE, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFE, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x02, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x02, 0x00, 0x00, 0x20, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xF8, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x06, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x06, 0x00, 0x00, 0x20, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x08, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 size16x16_tex128x128_vtx[] = {
	0xFF, 0xF8, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0x00, 0x00, 0xFF, 0xF8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x08, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 size16x16_tex64x64_0_vtx[] = {
	0xFF, 0xF8, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xF8, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 rectangle_vtx[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x80, 0x01, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
#pragma force_active reset

EffectEntry* effReleaseN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    effReleaseN64Data* data;
    s32 typeLo;

    effect = effEntry();
    effect->type = "ReleaseN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x78);
    effect->userdata = data;
    effect->callback = effReleaseMain;
    data->typeFlags = type & 0xFF00;
    typeLo = type & 0xFF;
    data->type = typeLo;
    data->counter = 0;
    if (duration <= 0) {
        data->life = 1000;
    } else {
        data->life = duration;
    }
    data->field_0x28 = 0xFF;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;
    data->tevReg0R = 0xFF;
    data->tevReg0G = 0xFF;
    data->tevReg0B = 0xFF;
    data->tevReg1R = 0xFF;
    data->tevReg1G = 0xFF;
    data->tevReg1B = 0xFF;
    data->tevReg1A = 0xFF;
    data->field_0x60 = 0;
    data->field_0x64 = 0;
    data->field_0x68 = 0;
    data->field_0x6C = 0;
    data->scales[0] = 1.0f;
    data->scales[1] = 0.0f;
    data->scales[2] = 0.0f;
    data->scales[3] = 0.0f;
    data->alphas[0] = 0xFF;
    data->alphas[1] = 0;
    data->alphas[2] = 0;
    data->alphas[3] = 0;
    if (typeLo == 3) {
        data->state = 1;
    } else {
        data->state = 0;
    }
    data->stateFrame = 0;
    return effect;
}

// Builds the initial Vec from an anonymous zero literal so the compiler schedules the loads the way the
// original does (see the prologue).
static inline Vec releaseMakeVec(f32 x, f32 y, f32 z) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

#pragma opt_common_subs off // without it MWCC shares the case-100 anime_data element address and the int->float constant address
void effReleaseMain(EffectEntry* effect) {
    effReleaseN64Data* data = effect->userdata;
    Vec position2;
    Vec position;
    s32 counter;
    s32 type;
    s32 i;

    position = releaseMakeVec(data->x, data->y, data->z);
    position2 = position;
    type = data->type;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 16;
    }
    if (data->life < 1000) {
        data->life--;
    }
    data->counter++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    counter = data->counter;
    if (data->life < 16) {
        data->field_0x28 = data->life << 4;
    }

    switch (data->state) {
        case 0:
            data->stateFrame++;
            data->alphas[0] = 200 + (counter & 3) * 30;
            {
                f32 s = (f32)sin(6.2831855f * (f32)(counter * 20) / 360.f);
                data->scales[0] = 0.04f * s + 0.5f;
            }
            if ((s32)data->typeFlags != 0) {
                data->x += (f32)sin(6.2831855f * (f32)counter / 360.f);
                data->y += (f32)cos(6.2831855f * (1.235631f * (f32)counter) / 360.f);
                if (data->stateFrame > 50) {
                    data->stateFrame = 0;
                    if (type == 3) {
                        data->state = 100;
                    } else {
                        data->state = 1;
                    }
                }
            }
            break;
        case 100: {
            u16 t;
            u8 b;
            f32 s;
            switch (data->stateFrame) {
                case 0:
                    data->scales[3] = 4.0f;
                    data->alphas[3] = 0xFF;
                    break;
                case 1:
                    data->scales[3] = 8.0f;
                    data->alphas[3] = 0xFF;
                    break;
                default:
                    data->scales[3] = 0.0f;
                    data->alphas[3] = 0;
                    break;
            }
            if (data->stateFrame < 30) {
#ifdef MUST_MATCH
                // The element address is built from an explicit byte offset and read through an AnimeData
                // pointer; that steers MWCC's allocation (index in r9, scratch pair r7/r8 instead of r6/r9).
                u32 off = data->stateFrame << 2;
                t = ((AnimeData*)((u8*)anime_data + off))->scale;
                s = 0.3f * (f32)data->stateFrame + 0.01f * (f32)t;
                b = ((AnimeData*)((u8*)anime_data + off))->alpha;
#else
                t = anime_data[data->stateFrame].scale;
                s = 0.3f * (f32)data->stateFrame + 0.01f * (f32)t;
                b = anime_data[data->stateFrame].alpha;
#endif
            } else {
                t = anime_data[29].scale;
                s = 0.01f * (f32)t + 0.3f * (f32)data->stateFrame;
                b = anime_data[29].alpha;
            }
            data->alphas[0] = 0;
            data->scales[0] = 0.0f;
            data->alphas[1] = b & 0xFF;
            data->alphas[2] = b & 0xFF;
            data->scales[1] = s;
            data->scales[2] = 0.6f + s;
            data->stateFrame++;
            break;
        }
        case 1: {
            u16 t;
            u8 a;
            f32 s;
            switch (data->stateFrame) {
                case 0:
                    data->scales[3] = 4.0f;
                    data->alphas[3] = 0xFF;
                    break;
                case 1:
                    data->scales[3] = 8.0f;
                    data->alphas[3] = 0xFF;
                    break;
                default:
                    data->scales[3] = 0.0f;
                    data->alphas[3] = 0;
                    break;
            }
            t = anime_data[data->stateFrame].scale;
            s = 0.01f * (f32)t;
            a = anime_data[data->stateFrame].alpha;
            data->alphas[0] = 0;
            data->alphas[1] = a;
            data->alphas[2] = a;
            data->scales[0] = 0.0f;
            data->scales[1] = s;
            data->scales[2] = 1.2f * s + 4.0f;
            data->stateFrame++;
            if (data->stateFrame >= 30) {
                data->stateFrame = 0;
                data->state = 4;
            }
            break;
        }
        case 4: {
            u16 t;
            u8 a;
            f32 s;
            switch (29 - data->stateFrame) {
                case 0:
                    data->scales[3] = 4.0f;
                    data->alphas[3] = 0xFF;
                    break;
                case 1:
                    data->scales[3] = 8.0f;
                    data->alphas[3] = 0xFF;
                    break;
                default:
                    data->scales[3] = 0.0f;
                    data->alphas[3] = 0;
                    break;
            }
            t = anime_data_2[data->stateFrame].scale;
            s = 0.01f * (f32)t;
            a = anime_data_2[data->stateFrame].alpha;
            data->alphas[0] = 0;
            data->scales[0] = 0.0f;
            data->alphas[1] = a;
            data->alphas[2] = 0;
            data->scales[1] = s;
            data->scales[2] = 0.0f;
            data->stateFrame++;
            if (data->stateFrame >= 30) {
                data->stateFrame = 0;
                data->state = 5;
            }
            break;
        }
        // State 5 is a real state (case 4 sets it; effReleaseDisp tests it), and naming it here also
        // makes MWCC keep the target's redundant `bge default` before the `b default` in the dispatch.
        case 5:
        default:
            data->scales[0] = 0.0f;
            data->scales[1] = 0.0f;
            data->scales[2] = 0.0f;
            data->alphas[0] = 0;
            data->alphas[1] = 0;
            data->alphas[2] = 0;
            data->scales[3] = 0.0f;
            data->alphas[3] = 0;
            data->scales[3] = 0.0f;
            data->stateFrame++;
            break;
    }

    for (i = 0; i < 4; i++) {
        if (data->alphas[i] > 0xFF) {
            data->alphas[i] = 0xFF;
        }
        if (data->alphas[i] < 0) {
            data->alphas[i] = 0;
        }
        if (data->scales[i] < 0.0f) {
            data->scales[i] = 0.0f;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effReleaseDisp, effect);
}
#pragma opt_common_subs reset

// .text

void effReleaseDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effReleaseN64Data* data;
    CameraEntry* cam = camGetPtr(camId);
    s32 type;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx mtx;
    Mtx44 ortho;
    GXTexObj texObj;
    f32 projv[7];
    s32 i;
    s32 alpha;

    data = effect->userdata;
    type = data->type;

    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
    PSMTXScale(scale, data->scale, data->scale, data->scale);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(trans, scale, mtx);

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);

    {
    GXSetTevColor(GX_TEVREG1, (GXColor){ data->tevReg1R, data->tevReg1G, data->tevReg1B, data->tevReg1A });
    }
    GXSetCullMode(GX_CULL_NONE);

    for (i = 0; i < 4; i++) {
        if (type == 1 && i == 2) {
            continue;
        }
        if (0.0f == data->scales[i]) {
            continue;
        }
        if (data->alphas[i] == 0) {
            continue;
        }
        PSMTXScale(scale, data->scales[i], data->scales[i], data->scales[i]);
        PSMTXConcat(mtx, scale, scale);
        PSMTXConcat(cam->view, scale, scale);
        GXLoadPosMtxImm(scale, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXSetTevColor(GX_TEVREG0, (GXColor){ data->tevReg0R, data->tevReg0G, data->tevReg0B, data->alphas[i] });

        switch (i) {
            case 0:
                effGetTexObjN64(0x44, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                effSetVtxDescN64((effVtxArray*)size16x16_tex64x64_vtx);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                break;
            case 1:
                effGetTexObjN64(0x45, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                effSetVtxDescN64((effVtxArray*)size16x16_tex256x32_vtx);
                PSMTXScale(scale, 0.00390625f, 0.00390625f, 0.0f);
                PSMTXTrans(trans, 0.0f, 0.0f, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                tri2(0x1C, 0x1D, 0x1E, 0x1C, 0x1C, 0x1E, 0x1F, 0x1C);
                PSMTXScale(scale, 0.00390625f, 0.00390625f, 0.0f);
                PSMTXTrans(trans, 0.0f, 32.0f, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(4, 5, 6, 4, 4, 6, 7, 4);
                tri2(0x18, 0x19, 0x1A, 0x18, 0x18, 0x1A, 0x1B, 0x18);
                PSMTXScale(scale, 0.00390625f, 0.00390625f, 0.0f);
                PSMTXTrans(trans, 0.0f, 64.0f, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(8, 9, 10, 8, 8, 10, 11, 8);
                tri2(0x14, 0x15, 0x16, 0x14, 0x14, 0x16, 0x17, 0x14);
                PSMTXScale(scale, 0.00390625f, 0.00390625f, 0.0f);
                PSMTXTrans(trans, 0.0f, 96.0f, 0.0f);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(0xC, 0xD, 0xE, 0xC, 0xC, 0xE, 0xF, 0xC);
                tri2(0x10, 0x11, 0x12, 0x10, 0x10, 0x12, 0x13, 0x10);
                break;
            case 2:
                effGetTexObjN64(0x46, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                PSMTXScale(scale, 0.015625f, 0.015625f, 0.0f);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                effSetVtxDescN64((effVtxArray*)size16x16_tex128x128_vtx);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                /* no break: case 2 falls through into case 3 (matches the original) */
            case 3:
                effGetTexObjN64(0x47, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                PSMTXScale(scale, 0.015625f, 0.03125f, 0.0f);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                effSetVtxDescN64((effVtxArray*)size16x16_tex64x64_0_vtx);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                tri2(4, 5, 6, 4, 4, 6, 7, 4);
                break;
            case 4:
                break;
            default:
                break;
        }
    }

    if (type == 2 || type == 3) {
        alpha = 0;
        if (data->state == 1 || data->state == 100) {
            if (data->stateFrame > 2 && data->stateFrame <= 10) {
                alpha = 0xFF - (data->stateFrame - 2) * 30;
            }
        }
        if (data->state == 5) {
            if (data->stateFrame > 0 && data->stateFrame <= 8) {
                alpha = 0xFF - data->stateFrame * 30;
            }
        }
        if (alpha > 0) {
            GXGetProjectionv(projv);
            C_MTXOrtho(ortho, 0.0f, 480.f, 0.0f, 608.0f, 0.0f, 1.0f);
            GXSetProjection(ortho, GX_ORTHOGRAPHIC);
            GXSetNumChans(1);
            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
            GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xD0, 0xD0, 0xD0, alpha });
            GXSetNumTevStages(1);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
            PSMTXIdentity(trans);
            GXLoadPosMtxImm(trans, GX_PNMTX0);
            GXSetCurrentMtx(GX_PNMTX0);
            effSetVtxDescN64((effVtxArray*)rectangle_vtx);
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
            GXSetProjectionv(projv);
        }
    }
}
