#include "eff/n64/eff_magic3_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math_ext.h>
#include <stdlib.h>

// .data: retail Ncube vertices for the 32x16 quad textured with a 64x32 image.
u32 size32x16_tex64x32_vtx[14] = {
	0xFFF0FFF8, 0x00000000, 0x00000000, 0x00FF0010, 0xFFF80000, 0x08000000, 0x000000FF,
	0x00100008, 0x00000800, 0x04000000, 0x00FFFFF0, 0x00080000, 0x00000400, 0x000000FF,
};

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p015625_8041f704;
extern const f32 float_0p03125_8041f708;
extern const f32 float_0_8041f70c;
extern const f32 float_deg2rad_8041f710;
extern const f32 float_1_8041f714;
// static const: folded into the pool at first use, so they keep the retail pool order.

// Inlined MetroWerks sqrtf (same shape as eff_blow_n64.c and eff_kemuri10_n64.c).
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

	if (x > float_0_8041f70c) {
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

EffectEntry* effMagic3N64Entry(s32 type, s32 life, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effMagic3Data* data;
	f32 dirX;
	f32 dirY;
	f32 dirZ;
	f32 perpX;
	f32 perpY;
	f32 perpZ;
	Vec axis;
	Mtx mtx;
	f32 angle;
	f32 mag;
	f32 norm;
	s32 i;

	effect = effEntry();
	dirZ = float_0_8041f70c;
	dirX = float_0_8041f70c;
	dirY = -1.0f;
	mag = dirX * dirX + float_1_8041f714 + dirZ;
	if (mag == float_0_8041f70c) {
		return NULL;
	}
	norm = -1.0f / sqrtf(mag);
	dirX *= norm;
	dirY *= norm;
	dirZ *= norm;
	if (dirX != float_0_8041f70c) {
		perpX = -dirY / dirX;
		perpY = float_1_8041f714;
		perpZ = float_0_8041f70c;
	}
	else if (dirY != float_0_8041f70c) {
		perpX = float_1_8041f714;
		perpY = -dirX / dirY;
		perpZ = float_0_8041f70c;
	}
	else {
		perpX = float_1_8041f714;
		perpY = float_0_8041f70c;
		perpZ = -dirX / dirZ;
	}
	mag = perpX * perpX + perpY * perpY + perpZ * perpZ;
	if (mag == float_0_8041f70c) {
		return NULL;
	}
	norm = float_1_8041f714 / sqrtf(mag);
	perpX *= norm;
	perpY *= norm;
	perpZ *= norm;
	dirX *= 18.0f;
	dirY *= 18.0f;
	dirZ *= 18.0f;
	effect->type = "Magic3N64";
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0xF0);
	effect->userdata = data;
	effect->callback = effMagic3Main;
	for (i = 0; i < 5; i++, data++) {
		f32 yaw = float_6p2832_8041f718 * camGetPtr(CAMERA_3D)->viewYaw / float_360_8041f724;
		axis = (Vec){ (f32)sin(yaw), 0.0f, -(f32)cos(yaw) };
		angle = (i == 1) ? float_0_8041f70c : 100.0f * i / 4.0f - 50.0f;
		PSMTXRotAxisRad(mtx, &axis, float_deg2rad_8041f710 * angle);
		data->type = type;
		data->x = x;
		data->y = y;
		data->z = z;
		data->vx = float_0p6_8041f72c * (mtx[0][0] * dirX + mtx[0][1] * dirY + mtx[0][2] * dirZ);
		data->vy = mtx[1][0] * dirX + mtx[1][1] * dirY + mtx[1][2] * dirZ;
		data->vz = float_0p6_8041f72c * (mtx[2][0] * dirX + mtx[2][1] * dirY + mtx[2][2] * dirZ);
		data->rot = (f32)(-(i * 100 / 4 - 50));
		data->scale = 0.5f;
		data->life = life;
		data->timer = 0;
		data->alpha = 0;
	}
	return effect;
}

// Local copy of system.c's inline helper so it inlines here (as in eff_onpu.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// The life<5 scale block computes the angle sin, alpha sin and division term into temporaries
// before the sum is formed (a single expression rotates f0/f3/f2).
void effMagic3Main(EffectEntry* effect) {
	s32 life;
	effMagic3Data* data;
	s32 i;
	f32 angle;
	Vec position;

	data = effect->userdata;
	position = initializeVec((Vec*)&data->x);
	data->life--;
	data->timer++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	angle = float_6p2832_8041f718 * (f32)(data->timer * 30);
	for (i = 0; i < effect->count; i++, data++) {
		if (life > 5) {
			data->vx *= float_0p8_8041f71c;
			data->vy *= float_0p8_8041f71c;
			data->vz *= float_0p8_8041f71c;
			data->alpha = (s32)(float_0p4_8041f720 * (f32)(255 - data->alpha) + (f32)data->alpha);
			data->scale = float_3_8041f728 * (f32)sin(float_6p2832_8041f718 * (f32)(data->alpha / 2) / float_360_8041f724) + (f32)sin(angle / float_360_8041f724);
		}
		if (life < 5) {
			data->alpha = (s32)((f32)data->alpha * float_0p6_8041f72c);
			data->vx *= float_0p7_8041f730;
			data->vy *= float_0p7_8041f730;
			data->vz *= float_0p7_8041f730;
			{
				f32 sg = (f32)sin(angle / float_360_8041f724);
				f32 sa = (f32)sin(float_6p2832_8041f718 * (f32)(data->alpha / 2) / float_360_8041f724);
				f32 extra = (f32)(255 - data->alpha) / float_30_8041f734;
				data->scale = float_3_8041f728 * sa + sg + extra;
			}
		}
		data->x += data->vx;
		data->y += data->vy;
		data->z += data->vz;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMagic3Disp, effect);
}

void effMagic3Disp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effMagic3Data* data = effect->userdata;
	GXTexObj texObj;
	GXColor color;
	Mtx mtx1;
	Mtx mtx4;
	Mtx mtx2;
	Mtx mtx3;
	f32 angle;
	s32 i;

	color = (GXColor){ 0xFF, 0xFF, 0xFF, data->alpha };
	GXSetTevColor(1, color);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(mtx4, float_0p015625_8041f704, float_0p03125_8041f708, float_0_8041f70c);
	GXLoadTexMtxImm(mtx4, 0x1E, 1);
	effGetTexObjN64(0x1E, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetCullMode(0);
	effSetVtxDescN64(size32x16_tex64x32_vtx);
	angle = float_deg2rad_8041f710;
	for (i = 0; i < effect->count; i++, data++) {
		PSMTXTrans(mtx1, data->x, data->y, data->z);
		PSMTXRotRad(mtx2, 'y', angle * -camGetPtr(CAMERA_3D)->viewYaw);
		PSMTXRotRad(mtx3, 'z', angle * data->rot);
		PSMTXScale(mtx4, data->scale, data->scale, float_1_8041f714);
		PSMTXConcat(mtx1, mtx2, mtx1);
		PSMTXConcat(mtx1, mtx3, mtx1);
		PSMTXConcat(mtx1, mtx4, mtx1);
		PSMTXConcat(cam->view, mtx1, mtx1);
		GXLoadPosMtxImm(mtx1, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p015625_8041f704 = 0.015625f;
const f32 float_0p03125_8041f708 = 0.03125f;
const f32 float_0_8041f70c = 0.0f;
const f32 float_deg2rad_8041f710 = 0.017453292f;
const f32 float_1_8041f714 = 1.0f;
const f32 float_6p2832_8041f718 = 6.2831855f;
const f32 float_0p8_8041f71c = 0.8f;
const f32 float_0p4_8041f720 = 0.4f;
const f32 float_360_8041f724 = 360.0f;
const f32 float_3_8041f728 = 3.0f;
const f32 float_0p6_8041f72c = 0.6f;
const f32 float_0p7_8041f730 = 0.7f;
const f32 float_30_8041f734 = 30.0f;
