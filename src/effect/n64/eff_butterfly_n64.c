#include "eff/n64/eff_butterfly_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .data / .sdata
// One 0x54-byte block of but_02_v; the table holds 30 of them (630 words).
typedef struct butterflyVtxFrame {
    u32 data[21];
} butterflyVtxFrame;

u32 but_02_v[630] = {
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF013E, 0x001F00FA, 0x04000800, 0xFFFFFFFF,
	0x013E001F, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEC2001F, 0x00FA0400, 0x0800FFFF, 0xFFFFFEC2, 0x001FFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF013D, 0x002A00FA, 0x04000800, 0xFFFFFFFF,
	0x013D002A, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEC3002A, 0x00FA0400, 0x0800FFFF, 0xFFFFFEC3, 0x002AFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF013A, 0x003A00FA, 0x04000800, 0xFFFFFFFF,
	0x013A003A, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEC6003A, 0x00FA0400, 0x0800FFFF, 0xFFFFFEC6, 0x003AFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0135, 0x004F00FA, 0x04000800, 0xFFFFFFFF,
	0x0135004F, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFECB004F, 0x00FA0400, 0x0800FFFF, 0xFFFFFECB, 0x004FFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF012E, 0x006800FA, 0x04000800, 0xFFFFFFFF,
	0x012E0068, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFED20068, 0x00FA0400, 0x0800FFFF, 0xFFFFFED2, 0x0068FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0123, 0x008400FA, 0x04000800, 0xFFFFFFFF,
	0x01230084, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEDD0084, 0x00FA0400, 0x0800FFFF, 0xFFFFFEDD, 0x0084FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0114, 0x00A000FA, 0x04000800, 0xFFFFFFFF,
	0x011400A0, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEEC00A0, 0x00FA0400, 0x0800FFFF, 0xFFFFFEEC, 0x00A0FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0101, 0x00BD00FA, 0x04000800, 0xFFFFFFFF,
	0x010100BD, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEFF00BD, 0x00FA0400, 0x0800FFFF, 0xFFFFFEFF, 0x00BDFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF00EB, 0x00D800FA, 0x04000800, 0xFFFFFFFF,
	0x00EB00D8, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF1500D8, 0x00FA0400, 0x0800FFFF, 0xFFFFFF15, 0x00D8FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF00D3, 0x00F000FA, 0x04000800, 0xFFFFFFFF,
	0x00D300F0, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF2D00F0, 0x00FA0400, 0x0800FFFF, 0xFFFFFF2D, 0x00F0FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF00B8, 0x010500FA, 0x04000800, 0xFFFFFFFF,
	0x00B80105, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF480105, 0x00FA0400, 0x0800FFFF, 0xFFFFFF48, 0x0105FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF009D, 0x011600FA, 0x04000800, 0xFFFFFFFF,
	0x009D0116, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF630116, 0x00FA0400, 0x0800FFFF, 0xFFFFFF63, 0x0116FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0082, 0x012400FA, 0x04000800, 0xFFFFFFFF,
	0x00820124, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF7E0124, 0x00FA0400, 0x0800FFFF, 0xFFFFFF7E, 0x0124FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF006A, 0x012D00FA, 0x04000800, 0xFFFFFFFF,
	0x006A012D, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF96012D, 0x00FA0400, 0x0800FFFF, 0xFFFFFF96, 0x012DFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0055, 0x013400FA, 0x04000800, 0xFFFFFFFF,
	0x00550134, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFFAB0134, 0x00FA0400, 0x0800FFFF, 0xFFFFFFAB, 0x0134FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0045, 0x013800FA, 0x04000800, 0xFFFFFFFF,
	0x00450138, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFFBB0138, 0x00FA0400, 0x0800FFFF, 0xFFFFFFBB, 0x0138FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF003B, 0x013A00FA, 0x04000800, 0xFFFFFFFF,
	0x003B013A, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFFC5013A, 0x00FA0400, 0x0800FFFF, 0xFFFFFFC5, 0x013AFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0037, 0x013B00FA, 0x04000800, 0xFFFFFFFF,
	0x0037013B, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFFC9013B, 0x00FA0400, 0x0800FFFF, 0xFFFFFFC9, 0x013BFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF003F, 0x013900FA, 0x04000800, 0xFFFFFFFF,
	0x003F0139, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFFC10139, 0x00FA0400, 0x0800FFFF, 0xFFFFFFC1, 0x0139FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0055, 0x013400FA, 0x04000800, 0xFFFFFFFF,
	0x00550134, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFFAB0134, 0x00FA0400, 0x0800FFFF, 0xFFFFFFAB, 0x0134FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0076, 0x012900FA, 0x04000800, 0xFFFFFFFF,
	0x00760129, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF8A0129, 0x00FA0400, 0x0800FFFF, 0xFFFFFF8A, 0x0129FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF009D, 0x011600FA, 0x04000800, 0xFFFFFFFF,
	0x009D0116, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF630116, 0x00FA0400, 0x0800FFFF, 0xFFFFFF63, 0x0116FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF00C5, 0x00FB00FA, 0x04000800, 0xFFFFFFFF,
	0x00C500FB, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF3B00FB, 0x00FA0400, 0x0800FFFF, 0xFFFFFF3B, 0x00FBFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF00EB, 0x00D800FA, 0x04000800, 0xFFFFFFFF,
	0x00EB00D8, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFF1500D8, 0x00FA0400, 0x0800FFFF, 0xFFFFFF15, 0x00D8FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF010B, 0x00AF00FA, 0x04000800, 0xFFFFFFFF,
	0x010B00AF, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEF500AF, 0x00FA0400, 0x0800FFFF, 0xFFFFFEF5, 0x00AFFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0123, 0x008400FA, 0x04000800, 0xFFFFFFFF,
	0x01230084, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEDD0084, 0x00FA0400, 0x0800FFFF, 0xFFFFFEDD, 0x0084FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF0132, 0x005C00FA, 0x04000800, 0xFFFFFFFF,
	0x0132005C, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFECE005C, 0x00FA0400, 0x0800FFFF, 0xFFFFFECE, 0x005CFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF013A, 0x003A00FA, 0x04000800, 0xFFFFFFFF,
	0x013A003A, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEC6003A, 0x00FA0400, 0x0800FFFF, 0xFFFFFEC6, 0x003AFF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF013D, 0x002400FA, 0x04000800, 0xFFFFFFFF,
	0x013D0024, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEC30024, 0x00FA0400, 0x0800FFFF, 0xFFFFFEC3, 0x0024FF02, 0x04000000, 0xFFFFFFFF,
	0x00000000, 0x00FA0000, 0x0800FFFF, 0xFFFF013E, 0x001B00FA, 0x04000800, 0xFFFFFFFF,
	0x013E001B, 0xFF020400, 0x0000FFFF, 0xFFFF0000, 0x0000FF02, 0x00000000, 0xFFFFFFFF,
	0xFEC2001B, 0x00FA0400, 0x0800FFFF, 0xFFFFFEC2, 0x001BFF02, 0x04000000, 0xFFFFFFFF,
};


// .sdata
static s8 y_data[32] = {
    -1, -2, -3, -4, -5, -6, -7, -8,
    -9, -10, -11, -12, -13, -14, -15, -16,
    -17, -18, -19, -17, -15, -13, -11, -9,
    -7, -5, -3, -1, 0, 0, 0, 0,
};

typedef struct effButterflyData {
    s32 type;            // 0x0, caller type
    s32 life;            // 0x4, frames remaining
    s32 age;             // 0x8, frames since spawn
    Vec position;        // 0xC
    Vec homePos;         // 0x18, center of the wander area
    s32 alpha;           // 0x24
    f32 heading;         // 0x28, degrees around Y
    s32 frame;           // 0x2C, vertex/animation frame, advanced by 3
    s32 retargetTimer;   // 0x30, frames until a new wander target
    f32 wanderRadius;    // 0x34
    f32 wanderHeight;    // 0x38, vertical range of a new target
    Vec velocity;        // 0x3C
} effButterflyData;

EffectEntry* effButterflyN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effButterflyData* data;

    effect = effEntry();
    effect->type = "ButterflyN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x48);
    effect->userdata = data;
    effect->callback = effButterflyMain;
    effect->flags |= 2;
    data->type = type;
    data->life = 0x3E8;
    data->alpha = 0xFF;
    data->position.x = x;
    data->homePos.x = x;
    data->position.y = y;
    data->homePos.y = y;
    data->position.z = z;
    data->homePos.z = z;
    data->age = 0;
    data->frame = 0;
    data->retargetTimer = 0;
    data->wanderRadius = 50.0f;
    data->wanderHeight = 30.0f;
    return effect;
}

// Inlined MetroWerks sqrtf (same shape as eff_sstar_n64.c / eff_blow_n64.c).
int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 butterflyFpClassify(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&x & 0x007FFFFF) {
            return FP_NAN;
        }
        return FP_INFINITE;
    case 0:
        if (*(s32*)&x & 0x007FFFFF) {
            return FP_SUBNORMAL;
        }
        return FP_ZERO;
    default:
        return FP_NORMAL;
    }
}

static inline f32 butterflySqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (butterflyFpClassify(x) == FP_NAN) {
        return NAN;
    }
    return x;
}

static inline Vec butterflyMakeVec(f32 x, f32 y, f32 z) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

// The position components are loaded into scalars before the delta math; with the direct
// data->position member form MWCC assigns the x/z temporaries to the opposite FPRs (f5/f3).
void effButterflyMain(EffectEntry* effect) {
    effButterflyData* data = effect->userdata;
    Vec position;
    Vec prevPosition;
    f32 angle;
    f32 targetX;
    f32 targetY;
    f32 targetZ;
    f32 px;
    f32 py;
    f32 pz;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 mag;

    position = butterflyMakeVec(data->position.x, data->position.y, data->position.z);
    prevPosition = position;
    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 0x384;
    }
    if (data->life < 1000) {
        data->life--;
    }
    data->age++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    if (data->life < 0x10) {
        data->alpha = data->life << 4;
    }
    data->retargetTimer--;
    if (data->retargetTimer <= 0) {
        angle = 6.2831855f * (f32)(rand() % 360) / 360.0f;
        targetX = data->wanderRadius * (f32)sin(angle) + data->homePos.x;
        targetY = data->homePos.y + (f32)(rand() % (s32)data->wanderHeight);
        targetZ = data->wanderRadius * (f32)cos(angle) + data->homePos.z;
        data->retargetTimer = rand() % 100 + 10;
        px = data->position.x;
        py = data->position.y;
        pz = data->position.z;
        dx = targetX - px;
        dy = targetY - py;
        dz = targetZ - pz;
        if (0.0f == dx && 0.0f == dy && 0.0f == dz) {
            dx = data->homePos.x - px;
            dy = data->homePos.y - py;
            dz = data->homePos.z - pz;
            if (0.0f == dx && 0.0f == dy && 0.0f == dz) {
                dx = 1.0f;
                dy = 0.0f;
                dz = 0.0f;
            }
        }
        mag = dx * dx + dy * dy + dz * dz;
        if (mag) {
            mag = 1.0f / butterflySqrtf(mag);
        }
        dx *= mag;
        dy *= mag;
        dz *= mag;
        data->velocity.x = dx;
        data->velocity.y = dy;
        data->velocity.z = dz;
        data->heading = angleABf(0.0f, 0.0f, -data->velocity.x, data->velocity.z);
    }
    data->frame += 3;
    if (data->frame >= 0x1E) {
        data->frame -= 0x1E;
    }
    data->position.x += data->velocity.x;
    data->position.y += data->velocity.y;
    data->position.z += data->velocity.z;
    dispEntry(CAMERA_3D, dispCalcZ(&prevPosition), 1, effButterflyDisp, effect);
}

void effButterflyDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    effButterflyData* data;
    s32 alpha;
    s32 type;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->alpha;
    type = data->type;

    PSMTXTrans(trans, data->position.x,
               0.3f * (f32)y_data[data->frame] + data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', 0.017453292f * data->heading);
    PSMTXConcat(trans, rot, trans);
    PSMTXScale(scale, 0.02f, 0.02f, 0.02f);
    PSMTXConcat(trans, scale, trans);
    PSMTXConcat(cam->view, trans, trans);
    GXLoadPosMtxImm(trans, 0);
    GXSetCurrentMtx(0);

    GXSetTevColor(1, (GXColor){ alpha, alpha, alpha, 0xFF });
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, 0.03125f, 0.015625f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    effGetTexObjN64(type + 0x78, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((butterflyVtxFrame*)but_02_v + data->frame);
    GXBegin(0x90, 0, 0xC);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(3, 4, 0, 0, 3, 5, 4, 0);
}

