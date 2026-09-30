#include "eff/n64/eff_magic3_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math_ext.h>

// .data: retail Ncube vertices for the 32x16 quad textured with a 64x32 image.
u32 size32x16_tex64x32_vtx[14] = {
	0xFFF0FFF8, 0x00000000, 0x00000000, 0x00FF0010, 0xFFF80000, 0x08000000, 0x000000FF,
	0x00100008, 0x00000800, 0x04000000, 0x00FFFFF0, 0x00080000, 0x00000400, 0x000000FF,
};

// .sdata2 float constants (named after their original addresses).
extern const u32 dat_8041f700;
extern const f32 float_0p015625_8041f704;
extern const f32 float_0p03125_8041f708;
extern const f32 float_0_8041f70c;
extern const f32 float_deg2rad_8041f710;
extern const f32 float_1_8041f714;

// .rodata zero vectors (named after their original addresses).
const Vec vec3_802fab48 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fab54 = { 0.0f, 0.0f, 0.0f };

// Local copy of system.c's inline helper so it inlines here (as in eff_onpu.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fab54;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// The life<5 scale block is one float-register colouring off. Retail keeps float_3/the fmadds
// accumulator in f0, double_to_int in f3 and float_30 in f2; our allocator uses f3/f2/f0 for the
// same three values, so the fmadds destination is the only real difference and the rest cascades.
// NONMATCHING (99.77%): that one volatile-FPR colouring block; every opcode/constant/order matches.
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
			data->scale = float_3_8041f728 * (f32)sin(float_6p2832_8041f718 * (f32)(data->alpha / 2) / float_360_8041f724) + (f32)sin(angle / float_360_8041f724) + (f32)(255 - data->alpha) / float_30_8041f734;
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

	color = *(GXColor*)&dat_8041f700;
	color.a = data->alpha;
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
	effSetVtxDescN64((effVtxArray*)size32x16_tex64x32_vtx);
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
