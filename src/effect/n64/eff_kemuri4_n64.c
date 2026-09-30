#include "eff/n64/eff_kemuri4_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "mario/mario.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>


extern const f32 float_0p0625_8041f32c;
extern const f32 float_0_8041f330;
extern const f32 float_1_8041f338;
extern const f32 float_5p4_8041f33c;
extern const f32 float_0p5_8041f340;
extern const f32 float_neg0p07_8041f344;
extern const f32 float_0p003_8041f348;
extern const f32 float_6p2832_8041f34c;
extern const f32 float_360_8041f350;
extern const f32 float_deg2rad_8041f334;
// .sdata2 word constant (named after its original address).
extern const u32 dat_8041f328;
// .data vertex array of this unit (named after its original symbol).
extern u8 flowers_v[];

EffectEntry* effKemuri4N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	effKemuri4Data* data;
	f32 angle;
	s32 i;

	effect = effEntry();
	effect->type = "Kemuri4N64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri4Data));
	effect->userdata = data;
	effect->callback = effKemuri4Main;

	for (i = 0; i < 1; i++) {
		data->field_0x00 = 1;
		data->type = type;
		data->field_0x6C = float_0_8041f330;
		data->position.x = x;
		data->position.y = y;
		data->position.z = z;
		data->field_0x18 = float_1_8041f338;
		data->field_0x1C = float_1_8041f338;
		data->field_0x20 = float_1_8041f338;
		data->field_0x08 = 0xFF;
		data->field_0x68 = float_1_8041f338;
		data->field_0x64 = float_5p4_8041f33c;
		data->field_0x60 = (f32)(a5 != float_0_8041f330 ? -10 : 10);
		data->field_0x70 = float_0p5_8041f340;
		data->field_0x74 = float_neg0p07_8041f344;
		data->field_0x78 = float_0p003_8041f348;
		data->field_0x7C = float_0_8041f330;
		data->field_0x06 = 15;
		data->field_0x28 = a4;
		data->field_0x24 = float_0_8041f330;
		data->field_0x2C = float_0_8041f330;
		data->field_0x09 = ((rand() % 100) >> 4) & 1;
		angle = reviseAngle(a4 + (f32)(a5 != float_0_8041f330 ? -90 : 90));
		angle = float_6p2832_8041f34c * angle / float_360_8041f350;
		data->field_0x80 = (f32)sin(angle);
		data->field_0x84 = (f32)cos(angle);
	}
	return effect;
}

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
	Vec temp = { 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

#pragma opt_common_subs off // without it MWCC reuses the compare's float_0 load across the marioGetPtr call, changing FPR allocation
void effKemuri4Main(EffectEntry* effect) {
	effKemuri4Data* data;
	Vec position;
	Mtx rotZ;
	Mtx rotY;
	Mtx rotX;
	Mtx trans;
	Mtx scale;
	s32 i;
	s32 active;

	data = effect->userdata;
	position = initializeVec(&data->position);
	active = 0;

	for (i = 0; i < effect->count; i++, data++) {
		if (data->field_0x00 == 0) {
			continue;
		}
		if (--data->field_0x06 <= 0) {
			data->field_0x00 = 0;
			continue;
		}
		active = 1;

		if (data->type == 0 && data->field_0x68 != float_0_8041f330
		    && marioGetPtr()->currMotionId == MARIO_MOTION_STAY) {
			data->field_0x06++;
		}
		else {
			data->field_0x78 += data->field_0x7C;
			data->field_0x74 += data->field_0x78;
			data->field_0x70 += data->field_0x74;
			data->field_0x2C = data->field_0x70 * data->field_0x60 + data->field_0x2C;
			data->position.x -= data->field_0x80 * (data->field_0x70 * data->field_0x64);
			data->position.z -= data->field_0x84 * (data->field_0x70 * data->field_0x64);
			data->field_0x08 -= 12;
			if (data->field_0x70 < float_0_8041f330) {
				data->field_0x68 = float_0_8041f330;
			}
		}

		PSMTXRotRad(rotX, 'x', float_deg2rad_8041f334 * data->field_0x24);
		PSMTXRotRad(rotY, 'y', float_deg2rad_8041f334 * data->field_0x28);
		PSMTXRotRad(rotZ, 'z', float_deg2rad_8041f334 * data->field_0x2C);
		PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scale, data->field_0x18, data->field_0x1C, data->field_0x20);
		PSMTXConcat(rotX, rotZ, rotZ);
		PSMTXConcat(rotY, rotZ, rotZ);
		PSMTXConcat(rotZ, scale, scale);
		PSMTXConcat(trans, scale, data->field_0x30);
	}

	if (active == 0) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effKemuri4Disp, effect);
	}
}
#pragma opt_common_subs on

void effKemuri4Disp(CameraId camId, void* param) {
	EffectEntry* effect;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtx;
	Mtx texMtx;
	effVtxArray* vtx0;
	effVtxArray* vtx1;
	effKemuri4Data* data;
	s32 i;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	GXSetCullMode(0);
	vtx0 = (effVtxArray*)((u8*)flowers_v + 0x15E);
	vtx1 = (effVtxArray*)((u8*)flowers_v + 0x1EA);

	for (i = 0; i < effect->count; i++, data++) {
		if (data->field_0x00 == 0) {
			continue;
		}
		if ((s8)data->field_0x09 != 0) {
			effGetTexObjN64(0x4C, &texObj);
		}
		else {
			effGetTexObjN64(0x4D, &texObj);
		}
		GXLoadTexObj(&texObj, 0);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
		PSMTXScale(texMtx, float_0p0625_8041f32c, float_0p0625_8041f32c, float_0_8041f330);
		GXLoadTexMtxImm(texMtx, 0x1E, 1);
		GXSetNumChans(0);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xF, 8, 2, 0xF);
		GXSetTevAlphaIn(0, 7, 4, 1, 7);
		PSMTXConcat(cam->view, data->field_0x30, mtx);
		GXLoadPosMtxImm(mtx, 0);
		color = *(GXColor*)&dat_8041f328;
		GXSetTevColor(1, color);
		effSetVtxDescN64(vtx0);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		GXSetNumChans(1);
		GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
		GXSetNumTexGens(0);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0xFF, 0xFF, 4);
		GXSetTevOp(0, 4);
		effSetVtxDescN64(vtx1);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 3, 4, 5, 0);
	}
}