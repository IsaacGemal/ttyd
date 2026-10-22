#include "eff/n64/eff_blow_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .data: retail 16x16 quad vertex array.
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008,
	0xFFF80000, 0x04000000, 0xFFFFFFFF, 0x00080008,
	0x00000400, 0x0400FFFF, 0xFFFFFFF8, 0x00080000,
	0x00000400, 0xFFFFFFFF,
};

typedef struct effBlowParticle {
	s32 type;        // 0x00 caller type value
	s32 state;       // 0x04 0 = respawn, 1 = finishing, 2 = dead
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 vx;          // 0x14
	f32 vy;          // 0x18
	f32 vz;          // 0x1C
	f32 spawnVx;     // 0x20
	f32 spawnVy;     // 0x24
	f32 spawnVz;     // 0x28
	f32 burstAx;     // 0x2C
	f32 burstAy;     // 0x30
	f32 burstAz;     // 0x34
	f32 accX;        // 0x38
	f32 accY;        // 0x3C
	f32 accZ;        // 0x40
	f32 scaleX;      // 0x44
	f32 scaleY;      // 0x48
	f32 rotZ;        // 0x4C
	f32 rotSpeed;    // 0x50
	f32 spawnRotZ;   // 0x54
	s32 alpha;       // 0x58
	s32 life;        // 0x5C
	s32 age;         // 0x60
	s32 delay;       // 0x64
} effBlowParticle;

extern const f32 float_0p03125_8041e984;
extern const f32 float_0p03_8041e9a0;
extern const f32 lbl_8041E9A4;
extern const f32 float_6_8041e9a8;
extern const f32 float_0p2_8041e9ac;
extern const f32 float_0p225_8041e9b0;
extern const f32 float_0p45_8041e9b4;
extern const f32 float_0p8_8041e9b8;
extern const f32 float_0_8041e988;
extern const f32 float_deg2rad_8041e98c;
extern const f32 float_0p5_8041e990;
extern const f32 float_4_8041e994;
extern const f32 float_20_8041e998;
extern const f32 float_8_8041e99c;
extern const f32 float_neg1_8041e9bc;
extern const f32 float_1_8041e9c0;
extern const f32 float_2_8041e9c4;
extern const f32 float_0p05_8041e9c8;

// Inlined MetroWerks sqrtf (copy of the shape used in eff_kemuri10_n64.c / eff_sstar_n64.c).
int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
		case 0x7F800000:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_NAN;
			}
			else {
				return FP_INFINITE;
			}
		case 0:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_SUBNORMAL;
			}
			else {
				return FP_ZERO;
			}
		default:
			return FP_NORMAL;
	}
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
	const f64 _half = .5;
	const f64 _three = 3.0;

	if (x > float_0_8041e988) {
		f64 guess = __frsqrte(x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		return x * guess;
	}
	else if (x < 0.0) {
		return NAN;
	}
	else if (isnan(x)) {
		return NAN;
	}
	return x;
}

// Inlined vector builder: original copies the shared zero Vec then writes the fields,
// then copies the temp out as the return value (same idiom as initializeVec elsewhere).
static inline Vec blowInitVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// Passing the deg2rad*i*360/5 angle straight to PSMTXRotAxisRad instead of through an `angle` temp
// fixed the angle block's scheduling.
// The six loop-invariant values (2*dir and the -0.2*dir accelerations) are hoisted into named
// locals before the loop and reused inside it; assigning them explicitly (rather than letting
// MWCC CSE the repeated `2.0f * dirX` expressions) is what gives the original's callee-saved FPR
// allocation (2*dirX/Y/Z -> f25/f24/f23, accelerations -> f22/f21/f20). Writing the acceleration
// as `0.05f * -dirX * 4.0f` keeps the first multiply's operands in the original's order (the
// constant on the left). The .rodata+0 zero Vec resolves to the section symbol instead of
// vec3_802fa250 (objdiff does not flag it).
EffectEntry* effBlowN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 dirX, f32 dirY, f32 dirZ) {
	EffectEntry* effect;
	effBlowParticle* data;
	effBlowParticle* work;
	Vec axis;
	Mtx mtx;
	f32 mag;
	f32 norm;
	f32 twoX;
	f32 twoY;
	f32 twoZ;
	f32 accX;
	f32 accY;
	f32 accZ;
	f32 px;
	f32 py;
	f32 pz;
	f32 angle;
	s32 i;

	effect = effEntry();
	if (count == 0) {
		return NULL;
	}

	mag = dirX * dirX + dirY * dirY + dirZ * dirZ;
	if (mag == float_0_8041e988) {
		return NULL;
	}
	norm = float_neg1_8041e9bc / sqrtf(mag);
	dirX *= norm;
	dirY *= norm;
	dirZ *= norm;

	if (dirX != float_0_8041e988) {
		px = -dirY / dirX;
		py = float_1_8041e9c0;
		pz = float_0_8041e988;
	}
	else if (dirY != float_0_8041e988) {
		px = float_1_8041e9c0;
		py = -dirX / dirY;
		pz = float_0_8041e988;
	}
	else {
		px = float_1_8041e9c0;
		py = float_0_8041e988;
		pz = -dirX / dirZ;
	}

	mag = px * px + py * py + pz * pz;
	if (mag == float_0_8041e988) {
		return NULL;
	}
	norm = float_1_8041e9c0 / sqrtf(mag);
	px *= norm;
	py *= norm;
	pz *= norm;

	dirX *= float_8_8041e99c;
	dirY *= float_8_8041e99c;
	dirZ *= float_8_8041e99c;
	px *= float_8_8041e99c;
	py *= float_8_8041e99c;
	pz *= float_8_8041e99c;

	effect->type = "BlowN64";
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, (count + 1) * sizeof(effBlowParticle));
	effect->userdata = data;
	effect->callback = effBlowMain;

	data->state = 0;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = 10;
	data->age = 0;

	work = data + 1;
	twoX = float_2_8041e9c4 * dirX;
	twoY = float_2_8041e9c4 * dirY;
	twoZ = float_2_8041e9c4 * dirZ;
	accX = float_0p05_8041e9c8 * -dirX * float_4_8041e994;
	accY = float_0p05_8041e9c8 * -dirY * float_4_8041e994;
	accZ = float_0p05_8041e9c8 * -dirZ * float_4_8041e994;
	for (i = 1; i < count + 1; i++, work++) {
		work->x = float_0_8041e988;
		work->y = float_0_8041e988;
		work->z = float_0_8041e988;
		axis = blowInitVec(dirX, dirY, dirZ);
		PSMTXRotAxisRad(mtx, &axis, float_deg2rad_8041e98c * (f32)(i * 360 / 5));

		work->spawnVx = twoX;
		work->vx = twoX;
		work->spawnVy = twoY;
		work->vy = twoY;
		work->spawnVz = twoZ;
		work->vz = twoZ;
		work->accX = accX;
		work->accY = accY;
		work->accZ = accZ;
		work->burstAx = float_4_8041e994 * (0.1f * (mtx[0][0] * px + mtx[0][1] * py + mtx[0][2] * pz));
		work->burstAy = float_4_8041e994 * (0.1f * (mtx[1][0] * px + mtx[1][1] * py + mtx[1][2] * pz));
		work->burstAz = float_4_8041e994 * (0.1f * (mtx[2][0] * px + mtx[2][1] * py + mtx[2][2] * pz));

		work->scaleX = float_0p5_8041e990;
		work->scaleY = float_4_8041e994;
		work->rotSpeed = float_20_8041e998;
		angle = (f32)(rand() % 360);
		work->spawnRotZ = angle;
		work->rotZ = angle;
		work->alpha = 0xFF;
		work->delay = ((i - 1) / 5) * 2;
		work->life = 10;
	}

	return effect;
}

void effBlowMain(EffectEntry* effect) {
	effBlowParticle* data;
	Vec position2;
	Vec position;
	s32 life;
	s32 i;
	s32 elemLife;
	s32 state;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->life--;
	data->age++;
	life = data->life;
	state = data->state;
	if (life < 0 && state == 2) {
		effDelete(effect);
		return;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->delay > 0) {
			data->delay--;
			if (data->delay > 0) {
				continue;
			}
		}
		data->life--;
		elemLife = data->life;
		if (elemLife < 0) {
			if (state == 0) {
				data->x = float_0_8041e988;
				data->y = float_0_8041e988;
				data->z = float_0_8041e988;
				data->vx = data->spawnVx;
				data->vy = data->spawnVy;
				data->vz = data->spawnVz;
				data->scaleX = float_0p5_8041e990;
				data->scaleY = float_4_8041e994;
				data->rotSpeed = float_20_8041e998;
				data->rotZ = data->spawnRotZ;
				data->alpha = 0xFF;
				data->life = 10;
				data->life--;
				elemLife = data->life;
			} else {
				data->x = float_0_8041e988;
				data->y = float_0_8041e988;
				data->z = float_0_8041e988;
				data->vx = float_0_8041e988;
				data->vy = float_0_8041e988;
				data->vz = float_0_8041e988;
				data->alpha = 0;
				data->life = 0;
			}
		}
		if (elemLife == 5 || elemLife == 6 || elemLife == 4) {
			data->vx += data->burstAx;
			data->vy += data->burstAy;
			data->vz += data->burstAz;
		}
		if (elemLife < 8) {
			data->scaleX += float_8_8041e99c * (float_0p03_8041e9a0 * (lbl_8041E9A4 - data->scaleX));
			data->scaleY += float_6_8041e9a8 * (float_0p03_8041e9a0 * (float_0p5_8041e990 - data->scaleY));
		}
		data->vx += data->accX;
		data->vy += data->accY;
		data->vz += data->accZ;
		if (data->vy < float_0_8041e988) {
			data->rotSpeed *= float_0p2_8041e9ac;
			data->vx *= float_0p225_8041e9b0;
			data->vy *= float_0p45_8041e9b4;
			data->vz *= float_0p225_8041e9b0;
			data->alpha = (s32)((f32)data->alpha * float_0p8_8041e9b8);
		}
		data->x += data->vx;
		data->y += data->vy;
		data->z += data->vz;
		data->rotZ += data->rotSpeed;
	}

	if (life < 0) {
		effBlowParticle* work = effect->userdata;
		if (work->state == 1) {
			work->state = 2;
		}
		work->life = 10;
		work->age = 0;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effBlowDisp, effect);
}

void effBlowDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effBlowData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    Mtx base;
    s32 i;
    f32 deg2rad;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xA, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    effGetTexObjN64(0x34, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    {
		f32 s = float_0p03125_8041e984;
		PSMTXScale(scale, s, s, float_0_8041e988);
	}
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetCullMode(0);
    effSetVtxDescN64(size16x16_tex32x32_vtx);

    PSMTXTrans(base, data->x, data->y, data->z);
    deg2rad = float_deg2rad_8041e98c;
    for (i = 1, data++; i < effect->count; i++, data++) {
        GXSetTevColor(1, (GXColor){ 0xC8, 0xFF, 0xFF, data->field_0x58 });

        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'y', deg2rad * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXConcat(trans, rot, work);
        PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleX);
        PSMTXConcat(work, scale, work);
        PSMTXRotRad(rot, 'z', deg2rad * data->rotZ);
        PSMTXConcat(work, rot, work);
        PSMTXConcat(base, work, work);
        PSMTXConcat(cam->view, work, work);

        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

// Original data objects of this unit, in address order.
const f32 float_0p03125_8041e984 = 0.03125f;
const f32 float_0_8041e988 = 0.0f;
const f32 float_deg2rad_8041e98c = 0.017453292f;
const f32 float_0p5_8041e990 = 0.5f;
const f32 float_4_8041e994 = 4.0f;
const f32 float_20_8041e998 = 20.0f;
const f32 float_8_8041e99c = 8.0f;
const f32 float_0p03_8041e9a0 = 0.03f;
const f32 lbl_8041E9A4 = 3.0f;
const f32 float_6_8041e9a8 = 6.0f;
const f32 float_0p2_8041e9ac = 0.2f;
const f32 float_0p225_8041e9b0 = 0.225f;
const f32 float_0p45_8041e9b4 = 0.45f;
const f32 float_0p8_8041e9b8 = 0.8f;
const f32 float_neg1_8041e9bc = -1.0f;
const f32 float_1_8041e9c0 = 1.0f;
const f32 float_2_8041e9c4 = 2.0f;
const f32 float_0p05_8041e9c8 = 0.05f;
