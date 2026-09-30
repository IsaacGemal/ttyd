#include "eff/n64/eff_blow_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 word constant of this unit (volatile: forces the per-element reload the original does).
const volatile u32 dat_8041e980 = 0xC8FFFF00;

// .data: retail 16x16 quad vertex array.
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008,
	0xFFF80000, 0x04000000, 0xFFFFFFFF, 0x00080008,
	0x00000400, 0x0400FFFF, 0xFFFFFFF8, 0x00080000,
	0x00000400, 0xFFFFFFFF,
};

static inline void blowSetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041e980;
    color.a = a;
    GXSetTevColor(1, color);
}

typedef struct effBlowParticle {
	s32 field_0x00;  // 0x00
	s32 type;        // 0x04
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 vx;          // 0x14
	f32 vy;          // 0x18
	f32 vz;          // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	f32 field_0x2C;  // 0x2C
	f32 field_0x30;  // 0x30
	f32 field_0x34;  // 0x34
	f32 field_0x38;  // 0x38
	f32 field_0x3C;  // 0x3C
	f32 field_0x40;  // 0x40
	f32 scaleX;      // 0x44
	f32 scaleY;      // 0x48
	f32 rotZ;        // 0x4C
	f32 field_0x50;  // 0x50
	f32 field_0x54;  // 0x54
	s32 field_0x58;  // 0x58
	s32 life;        // 0x5C
	s32 field_0x60;  // 0x60
	s32 field_0x64;  // 0x64
} effBlowParticle;

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
extern const f32 float_0p1_8041e9cc;

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

// The first mag/sqrt block only matches with opt_common_subs off: without it MWCC CSEs the
// float_0/float_1 pool loads across the two normalization blocks and the whole first half shifts
// (same idiom as effKemuri10N64Entry). What remains is a register-allocation tie-break in the loop
// preheader: the original keeps the per-element -0.2*dir accelerations in f20-f22 and the
// loop-persistent float_0/deg2rad/double_to_int in f27-f29, while this build swaps the two groups;
// the 2*dir velocities also land in the opposite f23-f25 order, and the int->float conversion's
// integer temps and stack slots differ too (original r7/r6/r5 at 0x5c/0x10/0x14/0x18, this build
// r6/r5/r0). The .rodata+0 zero Vec resolves to the section symbol instead of vec3_802fa250.
// NONMATCHING (98.45%): see above and REPORT.md.
#pragma opt_common_subs off // without it MWCC CSEs the float_0/float_1 pool loads and the first mag/sqrt block shifts
EffectEntry* effBlowN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 dirX, f32 dirY, f32 dirZ) {
	EffectEntry* effect;
	effBlowParticle* data;
	effBlowParticle* work;
	Vec axis;
	Mtx mtx;
	f32 mag;
	f32 norm;
	f32 px;
	f32 py;
	f32 pz;
	f32 angle;
	s32 i;

	effect = effEntry();
	if (count == 0) {
		return NULL;
	}

	mag = dirZ * dirZ + (dirX * dirX + dirY * dirY);
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

	data->type = 0;
	data->field_0x00 = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = 10;
	data->field_0x60 = 0;

	work = data + 1;
	for (i = 1; i < count + 1; i++, work++) {
		work->x = float_0_8041e988;
		work->y = float_0_8041e988;
		work->z = float_0_8041e988;
		angle = float_deg2rad_8041e98c * (f32)(i * 360 / 5);
		axis = blowInitVec(dirX, dirY, dirZ);
		PSMTXRotAxisRad(mtx, &axis, angle);

		work->field_0x20 = float_2_8041e9c4 * dirX;
		work->vx = float_2_8041e9c4 * dirX;
		work->field_0x24 = float_2_8041e9c4 * dirY;
		work->vy = float_2_8041e9c4 * dirY;
		work->field_0x28 = float_2_8041e9c4 * dirZ;
		work->vz = float_2_8041e9c4 * dirZ;
		work->field_0x38 = -dirX * float_0p05_8041e9c8 * float_4_8041e994;
		work->field_0x3C = -dirY * float_0p05_8041e9c8 * float_4_8041e994;
		work->field_0x40 = -dirZ * float_0p05_8041e9c8 * float_4_8041e994;
		work->field_0x2C = float_4_8041e994 * (float_0p1_8041e9cc * (mtx[0][0] * px + mtx[0][1] * py + mtx[0][2] * pz));
		work->field_0x30 = float_4_8041e994 * (float_0p1_8041e9cc * (mtx[1][0] * px + mtx[1][1] * py + mtx[1][2] * pz));
		work->field_0x34 = float_4_8041e994 * (float_0p1_8041e9cc * (mtx[2][0] * px + mtx[2][1] * py + mtx[2][2] * pz));

		work->scaleX = float_0p5_8041e990;
		work->scaleY = float_4_8041e994;
		work->field_0x50 = float_20_8041e998;
		angle = (f32)(rand() % 360);
		work->field_0x54 = angle;
		work->rotZ = angle;
		work->field_0x58 = 0xFF;
		work->field_0x64 = ((i - 1) / 5) * 2;
		work->life = 10;
	}

	return effect;
}
#pragma opt_common_subs reset

void effBlowMain(EffectEntry* effect) {
	effBlowParticle* data;
	Vec position2;
	Vec position;
	s32 life;
	s32 i;
	s32 elemLife;
	s32 type;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->life--;
	data->field_0x60++;
	life = data->life;
	type = data->type;
	if (life < 0 && type == 2) {
		effDelete(effect);
		return;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->field_0x64 > 0) {
			data->field_0x64--;
			if (data->field_0x64 > 0) {
				continue;
			}
		}
		data->life--;
		elemLife = data->life;
		if (elemLife < 0) {
			if (type == 0) {
				data->x = 0.0f;
				data->y = 0.0f;
				data->z = 0.0f;
				data->vx = data->field_0x20;
				data->vy = data->field_0x24;
				data->vz = data->field_0x28;
				data->scaleX = 0.5f;
				data->scaleY = 4.0f;
				data->field_0x50 = 20.0f;
				data->rotZ = data->field_0x54;
				data->field_0x58 = 0xFF;
				data->life = 10;
				data->life--;
				elemLife = data->life;
			} else {
				data->x = 0.0f;
				data->y = 0.0f;
				data->z = 0.0f;
				data->vx = 0.0f;
				data->vy = 0.0f;
				data->vz = 0.0f;
				data->field_0x58 = 0;
				data->life = 0;
			}
		}
		if (elemLife == 5 || elemLife == 6 || elemLife == 4) {
			data->vx += data->field_0x2C;
			data->vy += data->field_0x30;
			data->vz += data->field_0x34;
		}
		if (elemLife < 8) {
			data->scaleX += 8.0f * (0.03f * (3.0f - data->scaleX));
			data->scaleY += 6.0f * (0.03f * (0.5f - data->scaleY));
		}
		data->vx += data->field_0x38;
		data->vy += data->field_0x3C;
		data->vz += data->field_0x40;
		if (data->vy < 0.0f) {
			data->field_0x50 *= float_0p2_8041e9ac;
			data->vx *= float_0p225_8041e9b0;
			data->vy *= float_0p45_8041e9b4;
			data->vz *= float_0p225_8041e9b0;
			data->field_0x58 = (s32)((f32)data->field_0x58 * float_0p8_8041e9b8);
		}
		data->x += data->vx;
		data->y += data->vy;
		data->z += data->vz;
		data->rotZ += data->field_0x50;
	}

	if (life < 0) {
		effBlowParticle* work = effect->userdata;
		if (work->type == 1) {
			work->type = 2;
		}
		work->life = 10;
		work->field_0x60 = 0;
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
    PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

    PSMTXTrans(base, data->x, data->y, data->z);
    deg2rad = 0.017453292f;
    for (i = 1, data++; i < effect->count; i++, data++) {
        blowSetColor(data->field_0x58);

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
