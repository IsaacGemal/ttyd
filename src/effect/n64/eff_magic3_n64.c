#include "eff/n64/eff_magic3_n64.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

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
